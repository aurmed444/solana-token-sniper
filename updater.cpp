// HotkeyListenerDirectInputThread.cpp : implementation file
//
// created by Unwinder
/////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "HotkeyListenerDirectInputThread.h"

#include <io.h>
/////////////////////////////////////////////////////////////////////////////
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
/////////////////////////////////////////////////////////////////////////////
// CHotkeyListenerDirectInputThread
/////////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CHotkeyListenerDirectInputThread, CWinThread)
/////////////////////////////////////////////////////////////////////////////
#include "HotkeyHandlerWnd.h"
/////////////////////////////////////////////////////////////////////////////
#define DI_BUFFER_SIZE													256
/////////////////////////////////////////////////////////////////////////////
CHotkeyListenerDirectInputThread::CHotkeyListenerDirectInputThread()
{
}  
/////////////////////////////////////////////////////////////////////////////
CHotkeyListenerDirectInputThread::CHotkeyListenerDirectInputThread(CHotkeyHandlerWnd* pWnd)
{
	m_hEventKill			= CreateEvent(NULL, TRUE	, FALSE, NULL);
	m_hEventDead			= CreateEvent(NULL, TRUE	, FALSE, NULL);
	m_hEventNotification	= CreateEvent(NULL, TRUE	, FALSE, NULL);
	m_bAutoDelete			= FALSE;

	m_pWnd					= pWnd;
	m_lpDirectInput8		= NULL;
	m_lpDirectInputDevice8	= NULL;

	m_dwHotkey				= 0;
}
/////////////////////////////////////////////////////////////////////////////
CHotkeyListenerDirectInputThread::~CHotkeyListenerDirectInputThread()
{
	//Uninit and Init must be called from the same thread, we should NOT call it from destructor!
}
/////////////////////////////////////////////////////////////////////////////
BOOL CHotkeyListenerDirectInputThread::InitInstance()
{
	HANDLE waitObj[2];

	waitObj[0]			= m_hEventKill;
	waitObj[1]			= m_hEventNotification;

	BOOL bProcessInput	= TRUE;
	BOOL bInitRequired	= TRUE;	

	while (bProcessInput)
	{
		if (bInitRequired)
		{
			//NOTE: RTSS may create one more internal DirectInput handler for FCAT overlay working as "laterncy marker". DirectInput initialization
			//is not multithreading safe and may result in a deadlock in IDirectInputDevice8::Acquire, so we sync our initializations to prevent it

			HANDLE hMutex = CreateMutex(NULL, FALSE, "Global\\Access_DirectInput");
			if (hMutex)
				WaitForSingleObject(hMutex, INFINITE);

			if (Init())
				bInitRequired = FALSE;

			if (hMutex)
			{
				ReleaseMutex(hMutex);
				CloseHandle(hMutex);
			}

			if (WaitForSingleObject(m_hEventKill, 1000) == WAIT_OBJECT_0)
				bProcessInput = FALSE;
		}

		if (!bInitRequired)
		{
			DWORD dwResult = WaitForMultipleObjects(2, waitObj, FALSE, INFINITE);

			switch (dwResult)
			{
			case WAIT_OBJECT_0:
				bProcessInput = FALSE;
				break;

			case WAIT_OBJECT_0 + 1:
				{
					DIDEVICEOBJECTDATA	data;
					DWORD				size = 1;

					if (m_lpDirectInputDevice8)
					{
						while (size)
						{
							HRESULT err = m_lpDirectInputDevice8->GetDeviceData(sizeof(DIDEVICEOBJECTDATA), &data, &size, 0);

							if (FAILED(err))
							{
								if (err == DIERR_INPUTLOST)
								{
									bInitRequired = TRUE;
									break;
								}
								else
									break;
							}

							if (size)
							{
								DWORD dwVkCode			= DIKToVK(data.dwOfs);	
								DWORD dwHotkey			= m_dwHotkey;
								DWORD dwHotkeyReleased	= 0;

								if ((dwVkCode == VK_LCONTROL) ||
									(dwVkCode == VK_RCONTROL))
								{
									if (!(data.dwData & 0x80))
									{
										dwHotkey &= ~(HOTKEYF_CONTROL<<16);
										dwHotkeyReleased |= (HOTKEYF_CONTROL<<16);
									}
									else
										dwHotkey |= (HOTKEYF_CONTROL<<16);
								}
								else
									if ((dwVkCode == VK_LSHIFT) ||
										(dwVkCode == VK_RSHIFT))
									{
										if (!(data.dwData & 0x80))
										{
											dwHotkey &= ~(HOTKEYF_SHIFT<<16);
											dwHotkeyReleased |= (HOTKEYF_SHIFT<<16);

										}
										else
											dwHotkey |= (HOTKEYF_SHIFT<<16);
									}
									else
										if ((dwVkCode == VK_LMENU) ||
											(dwVkCode == VK_RMENU))
										{
											if (!(data.dwData & 0x80))
											{
												dwHotkey &= ~(HOTKEYF_ALT<<16);
												dwHotkeyReleased |= (HOTKEYF_ALT<<16);
											}
											else
												dwHotkey |= (HOTKEYF_ALT<<16);
										}
										else
										{
											if (!(data.dwData & 0x80))
											{
												dwHotkey &= 0xfff70000;
												dwHotkeyReleased &= 0xfff70000;
												dwHotkeyReleased |= dwVkCode;
											}
											else
											{
												dwHotkey &= 0xfff70000;
												dwHotkey |= dwVkCode;
											}
										}

								if (dwHotkeyReleased)
									m_pWnd->OnHotkey(dwHotkeyReleased | 0x80000000);

								if ((dwHotkey & 0x8ffff) && ((dwHotkey & 0x8ffff) != (m_dwHotkey & 0x8ffff)))
									m_pWnd->OnHotkey(dwHotkey);

								m_dwHotkey = dwHotkey;
							}
						}
					}

				}

				ResetEvent(m_hEventNotification);
				break;
			}
		}
	}

	Uninit();

	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
int CHotkeyListenerDirectInputThread::ExitInstance()
{
	SetEvent(m_hEventDead);

	return CWinThread::ExitInstance();
}
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CHotkeyListenerDirectInputThread, CWinThread)
	//{{AFX_MSG_MAP(CHotkeyListenerDirectInputThread)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// CHotkeyListenerDirectInputThread message handlers
/////////////////////////////////////////////////////////////////////////////
void CHotkeyListenerDirectInputThread::Kill()
{
	SetEvent(m_hEventKill);
	SetThreadPriority(THREAD_PRIORITY_HIGHEST);

	WaitForSingleObject(m_hEventDead	, INFINITE);
	WaitForSingleObject(m_hThread		, INFINITE);

	Destroy();
}
/////////////////////////////////////////////////////////////////////////////
BOOL CHotkeyListenerDirectInputThread::Init()
{
	char czPath[MAX_PATH];
	GetSystemDirectory(czPath, MAX_PATH);

	strcat_s(czPath, sizeof(czPath), "\\dinput8.dll");

	if (_taccess(czPath, 0))
		return FALSE;

	Uninit();

	if (FAILED(DirectInput8Create(AfxGetInstanceHandle(), DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_lpDirectInput8, NULL)))
		return FALSE;

	if (FAILED(m_lpDirectInput8->CreateDevice(GUID_SysKeyboard, &m_lpDirectInputDevice8, NULL)))
	{
		Uninit();
		return FALSE;
	}

	if (FAILED(m_lpDirectInputDevice8->SetDataFormat(&c_dfDIKeyboard)))
	{
		Uninit();
		return FALSE;
	}

	if (FAILED(m_lpDirectInputDevice8->SetCooperativeLevel(m_pWnd ? m_pWnd->m_hWnd : 0, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE)))
	{
		Uninit();
		return FALSE;
	}

	DIPROPDWORD dipdw; 
	dipdw.diph.dwSize		= sizeof(DIPROPDWORD); 
	dipdw.diph.dwHeaderSize = sizeof(DIPROPHEADER); 
	dipdw.diph.dwObj		= 0; 
	dipdw.diph.dwHow		= DIPH_DEVICE; 
	dipdw.dwData			= DI_BUFFER_SIZE; 

	if (FAILED(m_lpDirectInputDevice8->SetProperty(DIPROP_BUFFERSIZE, &dipdw.diph)))
	{
		Uninit();
		return FALSE;
	}

	if (FAILED(m_lpDirectInputDevice8->SetEventNotification(m_hEventNotification)))
	{
		Uninit();
		return FALSE;
	}

	if (FAILED(m_lpDirectInputDevice8->Acquire()))
	{
		Uninit();
		return FALSE;
	}

	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
void CHotkeyListenerDirectInputThread::Uninit()
{
	if (m_lpDirectInputDevice8)
	{
		m_lpDirectInputDevice8->Unacquire(); 
		m_lpDirectInputDevice8->Release();
	}
	m_lpDirectInputDevice8 = NULL;

	if (m_lpDirectInput8)
		m_lpDirectInput8->Release();
	m_lpDirectInput8 = NULL;
}
/////////////////////////////////////////////////////////////////////////////
DWORD CHotkeyListenerDirectInputThread::DIKToVK(DWORD dwCode)
{
	DWORD map[256] =
	{
		0                , //0x00 
		0x1B             , //0x01 DIK_ESCAPE
		0x31             , //0x02 DIK_1
		0x32             , //0x03 DIK_2
		0x33             , //0x04 DIK_3
		0x34             , //0x05 DIK_4
		0x35             , //0x06 DIK_5
		0x36             , //0x07 DIK_6
		0x37             , //0x08 DIK_7
		0x38             , //0x09 DIK_8
		0x39             , //0x0A DIK_9
		0x30             , //0x0B DIK_0
		0xBD             , //0x0C DIK_MINUS
		0xBB             , //0x0D DIK_EQUALS
		0x08             , //0x0E DIK_BACK
		0x09             , //0x0F DIK_TAB
		0x51             , //0x10 DIK_Q
		0x57             , //0x11 DIK_W
		0x45             , //0x12 DIK_E
		0x52             , //0x13 DIK_R
		0x54             , //0x14 DIK_T
		0x59             , //0x15 DIK_Y
		0x55             , //0x16 DIK_U
		0x49             , //0x17 DIK_I
		0x4F             , //0x18 DIK_O
		0x50             , //0x19 DIK_P
		0xDB             , //0x1A DIK_LBRACKET
		0xDD             , //0x1B DIK_RBRACKET
		0x0D             , //0x1C DIK_RETURN
		0xA2             , //0x1D DIK_LCONTROL
		0x41             , //0x1E DIK_A
		0x53             , //0x1F DIK_S
		0x44             , //0x20 DIK_D
		0x46             , //0x21 DIK_F
		0x47             , //0x22 DIK_G
		0x48             , //0x23 DIK_H
		0x4A             , //0x24 DIK_J
		0x4B             , //0x25 DIK_K
		0x4C             , //0x26 DIK_L
		0xBA             , //0x27 DIK_SEMICOLON
		0xDE             , //0x28 DIK_APOSTROPHE
		0xC0             , //0x29 DIK_GRAVE
		0xA0             , //0x2A DIK_LSHIFT
		0xDC             , //0x2B DIK_BACKSLASH
		0x5A             , //0x2C DIK_Z
		0x58             , //0x2D DIK_X
		0x43             , //0x2E DIK_C
		0x56             , //0x2F DIK_V
		0x42             , //0x30 DIK_B
		0x4E             , //0x31 DIK_N
		0x4D             , //0x32 DIK_M
		0xBC             , //0x33 DIK_COMMA
		0xBE             , //0x34 DIK_PERIOD
		0xBF             , //0x35 DIK_SLASH
		0xA1             , //0x36 DIK_RSHIFT
		0x6A             , //0x37 DIK_MULTIPLY
		0xA4             , //0x38 DIK_LMENU
		0x20             , //0x39 DIK_SPACE
		0x14             , //0x3A DIK_CAPITAL
		0x70             , //0x3B DIK_F1
		0x71             , //0x3C DIK_F2
		0x72             , //0x3D DIK_F3
		0x73             , //0x3E DIK_F4
		0x74             , //0x3F DIK_F5
		0x75             , //0x40 DIK_F6
		0x76             , //0x41 DIK_F7
		0x77             , //0x42 DIK_F8
		0x78             , //0x43 DIK_F9
		0x79             , //0x44 DIK_F10
		0x80090			 , //0x45 DIK_NUMLOCK
		0x91             , //0x46 DIK_SCROLL
		0x24             , //0x47 DIK_NUMPAD7
		0x26             , //0x48 DIK_NUMPAD8
		0x21             , //0x49 DIK_NUMPAD9
		0x6D             , //0x4A DIK_SUBTRACT
		0x25             , //0x4B DIK_NUMPAD4
		0x0C             , //0x4C DIK_NUMPAD5
		0x27             , //0x4D DIK_NUMPAD6
		0x6B             , //0x4E DIK_ADD
		0x23             , //0x4F DIK_NUMPAD1
		0x28             , //0x50 DIK_NUMPAD2
		0x22             , //0x51 DIK_NUMPAD3
		0x2D             , //0x52 DIK_NUMPAD0
		0x2E             , //0x53 DIK_DECIMAL
		0                , //0x54 
		0                , //0x55 
		0xE2             , //0x56 DIK_OEM_102
		0x7A             , //0x57 DIK_F11
		0x7B             , //0x58 DIK_F12
		0                , //0x59 
		0                , //0x5A 
		0                , //0x5B 
		0                , //0x5C 
		0                , //0x5D 
		0                , //0x5E 
		0                , //0x5F 
		0                , //0x60 
		0                , //0x61 
		0                , //0x62 
		0                , //0x63 
		0x7C             , //0x64 DIK_F13
		0x7D             , //0x65 DIK_F14
		0x7E             , //0x66 DIK_F15
		0                , //0x67 
		0                , //0x68 
		0                , //0x69 
		0                , //0x6A 
		0                , //0x6B 
		0                , //0x6C 
		0                , //0x6D 
		0                , //0x6E 
		0                , //0x6F 
		0                , //0x70 DIK_KANA
		0                , //0x71 
		0                , //0x72 
		0xC1             , //0x73 DIK_ABNT_C1
		0                , //0x74 
		0                , //0x75 
		0                , //0x76 
		0                , //0x77 
		0                , //0x78 
		0                , //0x79 DIK_CONVERT
		0                , //0x7A 
		0xEB             , //0x7B DIK_NOCONVERT
		0                , //0x7C 
		0                , //0x7D DIK_YEN
		0xC2             , //0x7E DIK_ABNT_C2
		0                , //0x7F 
		0                , //0x80 
		0                , //0x81 
		0                , //0x82 
		0                , //0x83 
		0                , //0x84 
		0                , //0x85 
		0                , //0x86 
		0                , //0x87 
		0                , //0x88 
		0                , //0x89 
		0                , //0x8A 
		0                , //0x8B 
		0                , //0x8C 
		0                , //0x8D DIK_NUMPADEQUALS
		0                , //0x8E 
		0                , //0x8F 
		0                , //0x90 DIK_PREVTRACK
		0                , //0x91 DIK_AT
		0                , //0x92 DIK_COLON
		0                , //0x93 DIK_UNDERLINE
		0                , //0x94 DIK_KANJI
		0                , //0x95 DIK_STOP
		0                , //0x96 DIK_AX
		0                , //0x97 DIK_UNLABELED
		0                , //0x98 
		0                , //0x99 DIK_NEXTTRACK
		0                , //0x9A 
		0                , //0x9B 
		0x8000D          , //0x9C DIK_NUMPADENTER
		0xA3             , //0x9D DIK_RCONTROL
		0                , //0x9E 
		0                , //0x9F 
		0                , //0xA0 DIK_MUTE
		0                , //0xA1 DIK_CALCULATOR
		0                , //0xA2 DIK_PLAYPAUSE
		0                , //0xA3 
		0                , //0xA4 DIK_MEDIASTOP
		0                , //0xA5 
		0                , //0xA6 
		0                , //0xA7 
		0                , //0xA8 
		0                , //0xA9 
		0                , //0xAA 
		0                , //0xAB 
		0                , //0xAC 
		0                , //0xAD 
		0                , //0xAE DIK_VOLUMEDOWN
		0                , //0xAF 
		0                , //0xB0 DIK_VOLUMEUP
		0                , //0xB1 
		0                , //0xB2 DIK_WEBHOME
		0                , //0xB3 DIK_NUMPADCOMMA
		0                , //0xB4 
		0x8006F          , //0xB5 DIK_DIVIDE
		0                , //0xB6 
		0x8002C          , //0xB7 DIK_SYSRQ
		0xA5             , //0xB8 DIK_RMENU
		0                , //0xB9 
		0                , //0xBA 
		0                , //0xBB 
		0                , //0xBC 
		0                , //0xBD 
		0                , //0xBE 
		0                , //0xBF 
		0                , //0xC0 
		0                , //0xC1 
		0                , //0xC2 
		0                , //0xC3 
		0                , //0xC4 
		0x13             , //0xC5 DIK_PAUSE
		0                , //0xC6 
		0x80024          , //0xC7 DIK_HOME
		0x80026          , //0xC8 DIK_UP
		0x80021          , //0xC9 DIK_PRIOR
		0                , //0xCA 
		0x80025          , //0xCB DIK_LEFT
		0                , //0xCC 
		0x80027          , //0xCD DIK_RIGHT
		0                , //0xCE 
		0x80023          , //0xCF DIK_END
		0x80028          , //0xD0 DIK_DOWN
		0x80022          , //0xD1 DIK_NEXT
		0x8002D          , //0xD2 DIK_INSERT
		0x8002E          , //0xD3 DIK_DELETE
		0                , //0xD4 
		0                , //0xD5 
		0                , //0xD6 
		0                , //0xD7 
		0                , //0xD8 
		0                , //0xD9 
		0                , //0xDA 
		0x5B             , //0xDB DIK_LWIN
		0x5C             , //0xDC DIK_RWIN
		0x5D             , //0xDD DIK_APPS
		0                , //0xDE DIK_POWER
		0                , //0xDF DIK_SLEEP
		0                , //0xE0 
		0                , //0xE1 
		0                , //0xE2 
		0                , //0xE3 DIK_WAKE
		0                , //0xE4 
		0                , //0xE5 DIK_WEBSEARCH
		0                , //0xE6 DIK_WEBFAVORITES
		0                , //0xE7 DIK_WEBREFRESH
		0                , //0xE8 DIK_WEBSTOP
		0                , //0xE9 DIK_WEBFORWARD
		0                , //0xEA DIK_WEBBACK
		0                , //0xEB DIK_MYCOMPUTER
		0                , //0xEC DIK_MAIL
		0                , //0xED DIK_MEDIASELECT
		0                , //0xEE 
		0                , //0xEF 
		0                , //0xF0 
		0                , //0xF1 
		0                , //0xF2 
		0                , //0xF3 
		0                , //0xF4 
		0                , //0xF5 
		0                , //0xF6 
		0                , //0xF7 
		0                , //0xF8 
		0                , //0xF9 
		0                , //0xFA 
		0                , //0xFB 
		0                , //0xFC 
		0                , //0xFD 
		0                , //0xFE 
		0                  //0xFF 
	};

	return map[dwCode];
}
/////////////////////////////////////////////////////////////////////////////
void CHotkeyListenerDirectInputThread::Destroy()
{
	delete this;
}
/////////////////////////////////////////////////////////////////////////////
void CHotkeyListenerDirectInputThread::ResetState()
{
	m_dwHotkey = 0;
}
/////////////////////////////////////////////////////////////////////////////
