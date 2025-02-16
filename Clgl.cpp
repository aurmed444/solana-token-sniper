// HotkeyHandler.cpp : Defines the initialization routines for the DLL.
//
// created by Unwinder
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include <afxwin.h>
#include <afxdllx.h>
#include <direct.h>

#include "RTSSExports.h"
#include "HotkeyHandler.h"
#include "HotkeyHandlerWnd.h"
#include "HotkeyHandlerDlg.h"
//////////////////////////////////////////////////////////////////////
#ifdef _DEBUG
#define new DEBUG_NEW
#endif
//////////////////////////////////////////////////////////////////////
static AFX_EXTENSION_MODULE HotkeyHandlerDLL = { NULL, NULL };
//////////////////////////////////////////////////////////////////////
HINSTANCE					g_hModule						= 0;

GET_HOST_APP_PROPERTY_PROC	g_pGetHostAppProperty			= NULL;
LOCALIZEWND_PROC			g_pLocalizeWnd					= NULL;
LOCALIZESTR_PROC			g_pLocalizeStr					= NULL;

DWORD						g_dwHeaderBgndColor				= 0x700000;
DWORD						g_dwHeaderTextColor				= 0xFFFFFF;

CHotkeyHandlerWnd			g_clientWnd;
//////////////////////////////////////////////////////////////////////
extern "C" int APIENTRY
DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	// Remove this if you use lpReserved
	UNREFERENCED_PARAMETER(lpReserved);

	if (dwReason == DLL_PROCESS_ATTACH)
	{
		TRACE0("HotkeyHandler.DLL Initializing!\n");
		
		// Extension DLL one-time initialization
		if (!AfxInitExtensionModule(HotkeyHandlerDLL, hInstance))
			return 0;

		// Insert this DLL into the resource chain
		// NOTE: If this Extension DLL is being implicitly linked to by
		//  an MFC Regular DLL (such as an ActiveX Control)
		//  instead of an MFC application, then you will want to
		//  remove this line from DllMain and put it in a separate
		//  function exported from this Extension DLL.  The Regular DLL
		//  that uses this Extension DLL should then explicitly call that
		//  function to initialize this Extension DLL.  Otherwise,
		//  the CDynLinkLibrary object will not be attached to the
		//  Regular DLL's resource chain, and serious problems will
		//  result.

		new CDynLinkLibrary(HotkeyHandlerDLL);

		g_hModule = hInstance;
	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{
		TRACE0("HotkeyHandler.DLL Terminating!\n");

		// Terminate the library before destructors are called
		AfxTermExtensionModule(HotkeyHandlerDLL);
	}
	return 1;   // ok
}
//////////////////////////////////////////////////////////////////////
HOTKEYHANDLER_API BOOL Start()
{
	if (!IsWindow(g_clientWnd.GetSafeHwnd()))
	{
		g_clientWnd.CreateEx(0, AfxRegisterWndClass(0,::LoadCursor(NULL,IDC_ARROW)), "RTSSHotkeyHandlerClientWnd", WS_POPUP, 0, 0, 0, 0, NULL, NULL);
	}

	return TRUE;
}
//////////////////////////////////////////////////////////////////////
HOTKEYHANDLER_API void Stop()
{
	if (IsWindow(g_clientWnd.GetSafeHwnd()))
	{
		g_clientWnd.DestroyWindow();
	}
}
//////////////////////////////////////////////////////////////////////
HOTKEYHANDLER_API BOOL Setup(HWND hWnd)
{
	if (!hWnd)
		//host will call Setup with hWnd set to NULL to verify if configuration is supported by plugin, so we just return TRUE to indicate
		//that our plugin supports configuratoin
		return TRUE;

	//init host applicaiton API

	HMODULE hHost = GetModuleHandle(NULL);

	g_pGetHostAppProperty	= (GET_HOST_APP_PROPERTY_PROC)GetProcAddress(hHost, "GetHostAppProperty");
	g_pLocalizeWnd			= (LOCALIZEWND_PROC)GetProcAddress(hHost, "LocalizeWnd");
	g_pLocalizeStr			= (LOCALIZESTR_PROC)GetProcAddress(hHost, "LocalizeStr");

	//get host application color scheme defined by currently selected host skin

	if (g_pGetHostAppProperty)
	{
		g_pGetHostAppProperty(HOST_APP_PROPERTY_SKIN_COLOR_HEADER_BGND, &g_dwHeaderBgndColor, sizeof(DWORD));
		g_pGetHostAppProperty(HOST_APP_PROPERTY_SKIN_COLOR_HEADER_TEXT, &g_dwHeaderTextColor, sizeof(DWORD));
	}

	g_clientWnd.ProcessHotkeys(FALSE);
		//temporarily disable hotkey processing while we're configuring the plugin, it is done to prevent
		//you from spamming unwanted screenshots when editing a hotkey associated with screen capture and so on

	g_clientWnd.m_config.Load();

	CHotkeyHandlerDlg dlg;
	dlg.m_config.Copy(&g_clientWnd.m_config);

	if (dlg.DoModal() == IDOK)
	{
		g_clientWnd.m_config.Copy(&dlg.m_config);
		g_clientWnd.m_config.Save();
	}

	g_clientWnd.ProcessHotkeys(TRUE);
		//resore hotkey processing

	return TRUE;
}
//////////////////////////////////////////////////////////////////////
LPCSTR LocalizeStr(LPCSTR lpStr)
{
	if (g_pLocalizeStr)
		return g_pLocalizeStr(lpStr);

	return lpStr;
}
/////////////////////////////////////////////////////////////////////////////
void LocalizeWnd(HWND hWnd)
{
	if (g_pLocalizeWnd)
		g_pLocalizeWnd(hWnd);
}
/////////////////////////////////////////////////////////////////////////////
BOOL IsDirectoryExists(LPCSTR lpDirectory)
{
	DWORD dwAttr = GetFileAttributes(lpDirectory);

	if (dwAttr == INVALID_FILE_ATTRIBUTES)
		return FALSE;

	return (dwAttr & FILE_ATTRIBUTE_DIRECTORY) != 0;
}
/////////////////////////////////////////////////////////////////////////////
typedef int (WINAPI *SHCREATEDIRECTORYEX)(HWND hwndOwner, LPTSTR lpszPath, SECURITY_ATTRIBUTES *psa);
/////////////////////////////////////////////////////////////////////////////
int CreateDirectory(LPCSTR lpDirectory)
{
	if (IsDirectoryExists(lpDirectory))
		//check if directory exists to avoid recreation attempt, which triggers controlled forlder access warning
		return 0;

	int iResult = 0;

	HMODULE  hModule = LoadLibrary("shell32.dll");

	if (hModule)
	{
		SHCREATEDIRECTORYEX pSHCreateDirectoryEx = (SHCREATEDIRECTORYEX)GetProcAddress(hModule, "SHCreateDirectoryExA");

		if (pSHCreateDirectoryEx)
			iResult = pSHCreateDirectoryEx(NULL, (LPSTR)lpDirectory, NULL);
		else
			iResult = _mkdir(lpDirectory);

		FreeLibrary(hModule);
	}
	else
		iResult = _mkdir(lpDirectory);

	return iResult;
}
/////////////////////////////////////////////////////////////////////////////
void AdjustWindowPos(CWnd* pWnd, CWnd* pParent)
{
	CRect wndRect; pWnd->GetWindowRect(&wndRect);

	if (pParent)
	{
		CRect parentRect; pParent->GetWindowRect(&parentRect);

		wndRect = CRect(parentRect.left, parentRect.top, parentRect.left + wndRect.Width(), parentRect.top + wndRect.Height());
	}

	wndRect.OffsetRect(20, 20);
	
	HMONITOR hMonitor = MonitorFromPoint(wndRect.TopLeft(), MONITOR_DEFAULTTONULL);

	MONITORINFO mi; mi.cbSize = sizeof(MONITORINFO);

	CRect rcWork;

	SystemParametersInfo(SPI_GETWORKAREA, 0, (LPVOID) &rcWork, 0);

	if (GetMonitorInfo(hMonitor, &mi))
		rcWork = mi.rcWork;
	else
		SystemParametersInfo(SPI_GETWORKAREA, 0, (LPVOID) &rcWork, 0);

	if (wndRect.bottom > rcWork.bottom)
		wndRect.OffsetRect(0, rcWork.bottom - wndRect.bottom);

	if (wndRect.right > rcWork.right)
		wndRect.OffsetRect(rcWork.right - wndRect.right, 0);

	pWnd->MoveWindow(&wndRect);
}
/////////////////////////////////////////////////////////////////////////////
