// HotkeyHandlerDlg.cpp: implementation of the CHotkeyHandlerDlg class.
//
// created by Unwinder
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "HotkeyHandler.h"
#include "HotkeyHandlerDlg.h"
#include "ScreenCaptureDlg.h"
#include "VideoCaptureDlg.h"
#include "VideoPrerecordDlg.h"
#include "BenchmarkDlg.h"
#include "ProfileModifierDlg.h"
#include "OverlayEditorDlg.h"
//////////////////////////////////////////////////////////////////////
// CHotkeyHandlerDlg dialog
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CHotkeyHandlerDlg, CDialog)
//////////////////////////////////////////////////////////////////////
static char g_szDefaultCustomFramesizes[] = "16:9 360p,02800168h,16:9 480p,035601E0h,16:9 720p,050002D0h,16:9 900p,06400384h,16:9 1080p,07800438h,16:10 360p,02400168h,16:10 480p,030001E0h,16:10 720p,048002D0h,16:10 900p,05A00384h,16:10 1050p,0690041Ah";
//////////////////////////////////////////////////////////////////////
CHotkeyHandlerDlg::CHotkeyHandlerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CHotkeyHandlerDlg::IDD, pParent)
{
	m_hBrush = NULL;
}
//////////////////////////////////////////////////////////////////////
CHotkeyHandlerDlg::~CHotkeyHandlerDlg()
{
}
//////////////////////////////////////////////////////////////////////
void CHotkeyHandlerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_OSD_ON_HOTKEY, m_ctrlOSDOnHotkey);
	DDX_Control(pDX, IDC_OSD_OFF_HOTKEY, m_ctrlOSDOffHotkey);
	DDX_Control(pDX, IDC_OSD_TOGGLE_HOTKEY, m_ctrlOSDToggleHotkey);
	DDX_Control(pDX, IDC_LIMITER_ON_HOTKEY, m_ctrlLimiterOnHotkey);
	DDX_Control(pDX, IDC_LIMITER_OFF_HOTKEY, m_ctrlLimiterOffHotkey);
	DDX_Control(pDX, IDC_LIMITER_TOGGLE_HOTKEY, m_ctrlLimiterToggleHotkey);
	DDX_Control(pDX, IDC_SCREEN_CAPTURE_HOTKEY, m_ctrlScreenCaptureHotkey);
	DDX_Control(pDX, IDC_BENCHMARK_BEGIN_RECORDING_HOTKEY, m_ctrlBenchmarkBeginHotkey);
	DDX_Control(pDX, IDC_BENCHMARK_END_RECORDING_HOTKEY, m_ctrlBenchmarkEndHotkey);
	DDX_Control(pDX, IDC_VIDEO_CAPTURE_HOTKEY, m_ctrlVideoCaptureHotkey);
	DDX_Control(pDX, IDC_VIDEO_PRERECORD_HOTKEY, m_ctrlVideoPrerecordHotkey);
	DDX_Control(pDX, IDC_PPM1_HOTKEY, m_ctrlPPM1Hotkey);
	DDX_Control(pDX, IDC_PPM2_HOTKEY, m_ctrlPPM2Hotkey);
	DDX_Control(pDX, IDC_PPM3_HOTKEY, m_ctrlPPM3Hotkey);
	DDX_Control(pDX, IDC_PPM4_HOTKEY, m_ctrlPPM4Hotkey);
}
//////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CHotkeyHandlerDlg, CDialog)
	ON_WM_CTLCOLOR()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_SCREEN_CAPTURE_SETTINGS_BUTTON, &CHotkeyHandlerDlg::OnBnClickedScreenCaptureSettingsButton)
	ON_BN_CLICKED(IDC_BENCHMARK_SETTINGS_BUTTON, &CHotkeyHandlerDlg::OnBnClickedBenchmarkSettingsButton)
	ON_BN_CLICKED(IDC_VIDEO_CAPTURE_SETTINGS_BUTTON, &CHotkeyHandlerDlg::OnBnClickedVideoCaptureSettingsButton)
	ON_BN_CLICKED(IDC_PRERECORD_SETTINGS_BUTTON, &CHotkeyHandlerDlg::OnBnClickedPrerecordSettingsButton)
	ON_BN_CLICKED(IDC_PPM1_SETTINGS_BUTTON, &CHotkeyHandlerDlg::OnBnClickedPpm1SettingsButton)
	ON_BN_CLICKED(IDC_PPM2_SETTINGS_BUTTON, &CHotkeyHandlerDlg::OnBnClickedPpm2SettingsButton)
	ON_BN_CLICKED(IDC_PPM3_SETTINGS_BUTTON, &CHotkeyHandlerDlg::OnBnClickedPpm3SettingsButton)
	ON_BN_CLICKED(IDC_PPM4_SETTINGS_BUTTON, &CHotkeyHandlerDlg::OnBnClickedPpm4SettingsButton)
	ON_BN_CLICKED(IDC_CONFIGURE_OVERLAY_EDITOR_BUTTON, &CHotkeyHandlerDlg::OnBnClickedConfigureOverlayEditorButton)
END_MESSAGE_MAP()
//////////////////////////////////////////////////////////////////////
BOOL CHotkeyHandlerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	LocalizeWnd(m_hWnd);
	AdjustWindowPos(this, GetParent());

	SetHotkeyCtrl(&m_ctrlOSDOnHotkey						, HOTKEY_OSD_ON				);
	SetHotkeyCtrl(&m_ctrlOSDOffHotkey						, HOTKEY_OSD_OFF			);
	SetHotkeyCtrl(&m_ctrlOSDToggleHotkey					, HOTKEY_OSD_TOGGLE			);

	SetHotkeyCtrl(&m_ctrlLimiterOnHotkey					, HOTKEY_LIMITER_ON			);
	SetHotkeyCtrl(&m_ctrlLimiterOffHotkey					, HOTKEY_LIMITER_OFF		);
	SetHotkeyCtrl(&m_ctrlLimiterToggleHotkey				, HOTKEY_LIMITER_TOGGLE		);

	SetHotkeyCtrl(&m_ctrlScreenCaptureHotkey				, HOTKEY_SCREEN_CAPTURE		);

	SetHotkeyCtrl(&m_ctrlVideoCaptureHotkey					, HOTKEY_VIDEO_CAPTURE		);
	SetHotkeyCtrl(&m_ctrlVideoPrerecordHotkey				, HOTKEY_VIDEO_PRERECORD	);

	SetHotkeyCtrl(&m_ctrlBenchmarkBeginHotkey				, HOTKEY_BENCHMARK_BEGIN	);
	SetHotkeyCtrl(&m_ctrlBenchmarkEndHotkey					, HOTKEY_BENCHMARK_END		);

	SetHotkeyCtrl(&m_ctrlPPM1Hotkey							, HOTKEY_PROFILE_MODIFIER1	);
	SetHotkeyCtrl(&m_ctrlPPM2Hotkey							, HOTKEY_PROFILE_MODIFIER2	);
	SetHotkeyCtrl(&m_ctrlPPM3Hotkey							, HOTKEY_PROFILE_MODIFIER3	);
	SetHotkeyCtrl(&m_ctrlPPM4Hotkey							, HOTKEY_PROFILE_MODIFIER4	);

	UpdateProfileControls();

	return TRUE; 
}
//////////////////////////////////////////////////////////////////////
void CHotkeyHandlerDlg::SetHotkeyCtrl(CHotKeyCtrl* pCtrl, DWORD dwHotkeyID)
{
	DWORD dwHotkey = m_config.m_hotkeyManager.GetHotkey(dwHotkeyID);

	pCtrl->SetHotKey((WORD)(dwHotkey & 0xffff), (WORD)(dwHotkey>>16));
}
//////////////////////////////////////////////////////////////////////
void CHotkeyHandlerDlg::GetHotkeyCtrl(CHotKeyCtrl* pCtrl, DWORD dwHotkeyID)
{
	WORD wVirtualKeyCode, wModifiers;
	pCtrl->GetHotKey(wVirtualKeyCode, wModifiers);

	DWORD dwHotkey = wVirtualKeyCode | (wModifiers<<16);

	if (!m_config.m_hotkeyManager.ValidateHotkey(dwHotkeyID, dwHotkey))
	{
		pCtrl->SetHotKey(0, 0);

		m_config.m_hotkeyManager.SetHotkey(dwHotkeyID, 0);
	}
	else
		m_config.m_hotkeyManager.SetHotkey(dwHotkeyID, dwHotkey);
}
//////////////////////////////////////////////////////////////////////
LRESULT CHotkeyHandlerDlg::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if ((message == WM_COMMAND) && ((wParam>>16) == EN_CHANGE))
	{
		switch (wParam & 0xFFFF)
		{
		case IDC_OSD_ON_HOTKEY:
			GetHotkeyCtrl(&m_ctrlOSDOnHotkey		, HOTKEY_OSD_ON);
			break;
		case IDC_OSD_OFF_HOTKEY:
			GetHotkeyCtrl(&m_ctrlOSDOffHotkey		, HOTKEY_OSD_OFF);
			break;
		case IDC_OSD_TOGGLE_HOTKEY:
			GetHotkeyCtrl(&m_ctrlOSDToggleHotkey	, HOTKEY_OSD_TOGGLE);
			break;

		case IDC_LIMITER_ON_HOTKEY:
			GetHotkeyCtrl(&m_ctrlLimiterOnHotkey	, HOTKEY_LIMITER_ON);
			break;
		case IDC_LIMITER_OFF_HOTKEY:
			GetHotkeyCtrl(&m_ctrlLimiterOffHotkey	, HOTKEY_LIMITER_OFF);
			break;
		case IDC_LIMITER_TOGGLE_HOTKEY:
			GetHotkeyCtrl(&m_ctrlLimiterToggleHotkey, HOTKEY_LIMITER_TOGGLE);
			break;

		case IDC_SCREEN_CAPTURE_HOTKEY:
			GetHotkeyCtrl(&m_ctrlScreenCaptureHotkey, HOTKEY_SCREEN_CAPTURE);

		case IDC_VIDEO_CAPTURE_HOTKEY:
			GetHotkeyCtrl(&m_ctrlVideoCaptureHotkey, HOTKEY_VIDEO_CAPTURE);
			break;
		case IDC_VIDEO_PRERECORD_HOTKEY:
			GetHotkeyCtrl(&m_ctrlVideoPrerecordHotkey, HOTKEY_VIDEO_PRERECORD);
			break;

		case IDC_BENCHMARK_BEGIN_RECORDING_HOTKEY:
			GetHotkeyCtrl(&m_ctrlBenchmarkBeginHotkey, HOTKEY_BENCHMARK_BEGIN);
			break;
		case IDC_BENCHMARK_END_RECORDING_HOTKEY:
			GetHotkeyCtrl(&m_ctrlBenchmarkEndHotkey	, HOTKEY_BENCHMARK_END);
			break;

		case IDC_PPM1_HOTKEY:
			GetHotkeyCtrl(&m_ctrlPPM1Hotkey, HOTKEY_PROFILE_MODIFIER1);
			break;
		case IDC_PPM2_HOTKEY:
			GetHotkeyCtrl(&m_ctrlPPM2Hotkey, HOTKEY_PROFILE_MODIFIER2);
			break;
		case IDC_PPM3_HOTKEY:
			GetHotkeyCtrl(&m_ctrlPPM3Hotkey, HOTKEY_PROFILE_MODIFIER3);
			break;
		case IDC_PPM4_HOTKEY:
			GetHotkeyCtrl(&m_ctrlPPM4Hotkey, HOTKEY_PROFILE_MODIFIER4);
			break;
		}
	}

	return CDialog::DefWindowProc(message, wParam, lParam);
}
//////////////////////////////////////////////////////////////////////
HBRUSH CHotkeyHandlerDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	COLORREF clrBk		= g_dwHeaderBgndColor;
	COLORREF clrText	= g_dwHeaderTextColor;

	UINT nID			= pWnd->GetDlgCtrlID();

	if ((nID == IDC_OSD_HOTKEYS_PROPERTIES_HEADER				) ||
		(nID == IDC_LIMITER_HOTKEYS_PROPERTIES_HEADER			) ||
		(nID == IDC_SCREEN_CAPTURE_HOTKEYS_PROPERTIES_HEADER	) ||
		(nID == IDC_VIDEO_CAPTURE_HOTKEYS_PROPERTIES_HEADER		) ||
		(nID == IDC_BENCHMARK_HOTKEYS_PROPERTIES_HEADER			) ||
		(nID == IDC_PROFILES_HOTKEYS_PROPERTIES_HEADER			))
	{
		if (!m_hBrush)
 			m_hBrush = CreateSolidBrush(clrBk);

		pDC->SetBkColor(clrBk);
		pDC->SetTextColor(clrText);
	}
	else 
		return CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	return m_hBrush;
}
//////////////////////////////////////////////////////////////////////
void CHotkeyHandlerDlg::OnDestroy()
{
	if (m_hBrush)
	{
		DeleteObject(m_hBrush);

		m_hBrush = NULL;
	}

	CDialog::OnDestroy();
}
//////////////////////////////////////////////////////////////////////
void CHotkeyHandlerDlg::OnBnClickedScreenCaptureSettingsButton()
{
	CScreenCaptureDlg dlg(this);

	dlg.m_strScreenCaptureFormat	= m_config.m_strScreenCaptureFormat;
	dlg.m_dwScreenCaptureQuality	= m_config.m_dwScreenCaptureQuality;	
	dlg.m_strScreenCaptureFolder	= m_config.m_strScreenCaptureFolder;
	dlg.m_bCaptureOSD				= m_config.m_bCaptureOSD;

	if (dlg.DoModal() == IDOK)
	{
		m_config.m_strScreenCaptureFormat	= dlg.m_strScreenCaptureFormat;
		m_config.m_dwScreenCaptureQuality	= dlg.m_dwScreenCaptureQuality;	
		m_config.m_strScreenCaptureFolder	= dlg.m_strScreenCaptureFolder;
		m_config.m_bCaptureOSD				= dlg.m_bCaptureOSD;
	}
}
//////////////////////////////////////////////////////////////////////
void CHotkeyHandlerDlg::OnBnClickedBenchmarkSettingsButton()
{
	CBenchmarkDlg dlg(this);

	dlg.m_strBenchmarkPath =  m_config.m_strBenchmarkPath;
	dlg.m_bRecreateResults = !m_config.m_bAppendBenchmark;

	if (dlg.DoModal() == IDOK)
	{
		m_config.m_strBenchmarkPath	=  dlg.m_strBenchmarkPath;
		m_config.m_bAppendBenchmark	= !dlg.m_bRecreateResults;
	}
}
//////////////////////////////////////////////////////////////////////
void CHotkeyHandlerDlg::OnBnClickedVideoCaptureSettingsButton()
{
	CVideoCaptureDlg dlg(this);

	dlg.m_strVideoCaptureContainer	= m_config.m_strVideoCaptureContainer;
	dlg.m_strVideoCaptureFormat		= m_config.m_strVideoCaptureFormat;
	dlg.m_dwVideoCaptureQuality		= m_config.m_dwVideoCaptureQuality;
	dlg.m_strVideoCaptureFolder		= m_config.m_strVideoCaptureFolder;
	dlg.m_dwVideoCaptureFramesize	= m_config.m_dwVideoCaptureFramesize;
	dlg.m_dwVideoCaptureFramerate	= m_config.m_dwVideoCaptureFramerate;
	dlg.m_dwAudioCaptureFlags		= m_config.m_dwAudioCaptureFlags;
	dlg.m_dwAudioCaptureFlags2		= m_config.m_dwAudioCaptureFlags2;
	dlg.m_dwVideoCaptureFlagsEx		= m_config.m_dwVideoCaptureFlagsEx;
	dlg.m_bCaptureOSD				= m_config.m_bCaptureOSD;

	dlg.m_strCustomFramesizes = m_config.GetConfigStr("Settings", "VideoCaptureCustomFramesizes", g_szDefaultCustomFramesizes );

	dlg.m_hotkeyManager.Copy(&m_config.m_hotkeyManager);

	if (dlg.DoModal() == IDOK)
	{
		m_config.m_strVideoCaptureContainer	= dlg.m_strVideoCaptureContainer;
		m_config.m_strVideoCaptureFormat	= dlg.m_strVideoCaptureFormat;
		m_config.m_dwVideoCaptureQuality	= dlg.m_dwVideoCaptureQuality;
		m_config.m_strVideoCaptureFolder	= dlg.m_strVideoCaptureFolder;
		m_config.m_dwVideoCaptureFramesize	= dlg.m_dwVideoCaptureFramesize;
		m_config.m_dwVideoCaptureFramerate	= dlg.m_dwVideoCaptureFramerate;
		m_config.m_dwAudioCaptureFlags		= dlg.m_dwAudioCaptureFlags;
		m_config.m_dwAudioCaptureFlags2		= dlg.m_dwAudioCaptureFlags2;
		m_config.m_dwVideoCaptureFlagsEx	= dlg.m_dwVideoCaptureFlagsEx;
		m_config.m_bCaptureOSD				= dlg.m_bCaptureOSD;
			
		m_config.m_hotkeyManager.Copy(&dlg.m_hotkeyManager);
	}
}
//////////////////////////////////////////////////////////////////////
void CHotkeyHandlerDlg::OnBnClickedPrerecordSettingsButton()
{
	CVideoPrerecordDlg dlg(this);

	dlg.m_dwVideoCaptureFlagsEx		= m_config.m_dwVideoCaptureFlagsEx;
	dlg.m_dwPrerecordSizeLimit		= m_config.m_dwPrerecordSizeLimit;
	dlg.m_dwPrerecordTimeLimit		= m_config.m_dwPrerecordTimeLimit;
	dlg.m_bAutoPrerecord			= m_config.m_bAutoPrerecord;

	if (dlg.DoModal() == IDOK)
	{
		m_config.m_dwVideoCaptureFlagsEx	= dlg.m_dwVideoCaptureFlagsEx;
		m_config.m_dwPrerecordSizeLimit		= dlg.m_dwPrerecordSizeLimit;
		m_config.m_dwPrerecordTimeLimit		= dlg.m_dwPrerecordTimeLimit;

		if (m_config.m_bAutoPrerecord != dlg.m_bAutoPrerecord)
		{
			m_config.m_bAutoPrerecord		= dlg.m_bAutoPrerecord;

			g_clientWnd.AutoPrerecordProc(TRUE);
		}
	}
}
//////////////////////////////////////////////////////////////////////
void CHotkeyHandlerDlg::UpdateProfileControls()
{
	if (strlen(m_config.m_strPPM1Desc))
		GetDlgItem(IDC_PPM1_DESC_CAPTION)->SetWindowText(LocalizeStr(m_config.m_strPPM1Desc));
	else
		GetDlgItem(IDC_PPM1_DESC_CAPTION)->SetWindowText(LocalizeStr("Programmable profile modifier 1"));

	if (strlen(m_config.m_strPPM2Desc))
		GetDlgItem(IDC_PPM2_DESC_CAPTION)->SetWindowText(LocalizeStr(m_config.m_strPPM2Desc));
	else
		GetDlgItem(IDC_PPM2_DESC_CAPTION)->SetWindowText(LocalizeStr("Programmable profile modifier 2"));

	if (strlen(m_config.m_strPPM3Desc))
		GetDlgItem(IDC_PPM3_DESC_CAPTION)->SetWindowText(LocalizeStr(m_config.m_strPPM3Desc));
	else
		GetDlgItem(IDC_PPM3_DESC_CAPTION)->SetWindowText(LocalizeStr("Programmable profile modifier 3"));

	if (strlen(m_config.m_strPPM4Desc))
		GetDlgItem(IDC_PPM4_DESC_CAPTION)->SetWindowText(LocalizeStr(m_config.m_strPPM4Desc));
	else
		GetDlgItem(IDC_PPM4_DESC_CAPTION)->SetWindowText(LocalizeStr("Programmable profile modifier 4"));
}
//////////////////////////////////////////////////////////////////////
void CHotkeyHandlerDlg::OnBnClickedPpm1SettingsButton()
{
	CProfileModifierDlg dlg(this);

	dlg.m_strDesc		= m_config.m_strPPM1Desc;
	dlg.m_strProfile	= m_config.m_strPPM1Profile;
	dlg.m_strProperty	= m_config.m_strPPM1Property;
	dlg.m_nType			= m_config.m_dwPPM1Type;
	dlg.m_nValue		= m_config.m_dwPPM1Value;

	if (dlg.DoModal() == IDOK)
	{
		m_config.m_strPPM1Desc		= dlg.m_strDesc;
		m_config.m_strPPM1Profile	= dlg.m_strProfile;
		m_config.m_strPPM1Property	= dlg.m_strProperty;
		m_config.m_dwPPM1Type		= dlg.m_nType;
		m_config.m_dwPPM1Value		= dlg.m_nValue;
	}

	UpdateProfileControls();
}
//////////////////////////////////////////////////////////////////////
void CHotkeyHandlerDlg::OnBnClickedPpm2SettingsButton()
{
	CProfileModifierDlg dlg(this);

	dlg.m_strDesc		= m_config.m_strPPM2Desc;
	dlg.m_strProfile	= m_config.m_strPPM2Profile;
	dlg.m_strProperty	= m_config.m_strPPM2Property;
	dlg.m_nType			= m_config.m_dwPPM2Type;
	dlg.m_nValue		= m_config.m_dwPPM2Value;

	if (dlg.DoModal() == IDOK)
	{
		m_config.m_strPPM2Desc		= dlg.m_strDesc;
		m_config.m_strPPM2Profile	= dlg.m_strProfile;
		m_config.m_strPPM2Property	= dlg.m_strProperty;
		m_config.m_dwPPM2Type		= dlg.m_nType;
		m_config.m_dwPPM2Value		= dlg.m_nValue;
	}

	UpdateProfileControls();
}
//////////////////////////////////////////////////////////////////////
void CHotkeyHandlerDlg::OnBnClickedPpm3SettingsButton()
{
	CProfileModifierDlg dlg(this);

	dlg.m_strDesc		= m_config.m_strPPM3Desc;
	dlg.m_strProfile	= m_config.m_strPPM3Profile;
	dlg.m_strProperty	= m_config.m_strPPM3Property;
	dlg.m_nType			= m_config.m_dwPPM3Type;
	dlg.m_nValue		= m_config.m_dwPPM3Value;

	if (dlg.DoModal() == IDOK)
	{
		m_config.m_strPPM3Desc		= dlg.m_strDesc;
		m_config.m_strPPM3Profile	= dlg.m_strProfile;
		m_config.m_strPPM3Property	= dlg.m_strProperty;
		m_config.m_dwPPM3Type		= dlg.m_nType;
		m_config.m_dwPPM3Value		= dlg.m_nValue;
	}

	UpdateProfileControls();
}
//////////////////////////////////////////////////////////////////////
void CHotkeyHandlerDlg::OnBnClickedPpm4SettingsButton()
{
	CProfileModifierDlg dlg(this);

	dlg.m_strDesc		= m_config.m_strPPM4Desc;
	dlg.m_strProfile	= m_config.m_strPPM4Profile;
	dlg.m_strProperty	= m_config.m_strPPM4Property;
	dlg.m_nType			= m_config.m_dwPPM4Type;
	dlg.m_nValue		= m_config.m_dwPPM4Value;

	if (dlg.DoModal() == IDOK)
	{
		m_config.m_strPPM4Desc		= dlg.m_strDesc;
		m_config.m_strPPM4Profile	= dlg.m_strProfile;
		m_config.m_strPPM4Property	= dlg.m_strProperty;
		m_config.m_dwPPM4Type		= dlg.m_nType;
		m_config.m_dwPPM4Value		= dlg.m_nValue;
	}

	UpdateProfileControls();
}
//////////////////////////////////////////////////////////////////////
void CHotkeyHandlerDlg::OnBnClickedConfigureOverlayEditorButton()
{
	COverlayEditorDlg dlg(this);

	dlg.m_strOVM1Desc		= m_config.m_strOVM1Desc;
	dlg.m_strOVM1Message	= m_config.m_strOVM1Message;
	dlg.m_strOVM1Layer		= m_config.m_strOVM1Layer;
	dlg.m_strOVM1Params		= m_config.m_strOVM1Params;

	dlg.m_strOVM2Desc		= m_config.m_strOVM2Desc;
	dlg.m_strOVM2Message	= m_config.m_strOVM2Message;
	dlg.m_strOVM2Layer		= m_config.m_strOVM2Layer;
	dlg.m_strOVM2Params		= m_config.m_strOVM2Params;

	dlg.m_strOVM3Desc		= m_config.m_strOVM3Desc;
	dlg.m_strOVM3Message	= m_config.m_strOVM3Message;
	dlg.m_strOVM3Layer		= m_config.m_strOVM3Layer;
	dlg.m_strOVM3Params		= m_config.m_strOVM3Params;

	dlg.m_strOVM4Desc		= m_config.m_strOVM4Desc;
	dlg.m_strOVM4Message	= m_config.m_strOVM4Message;
	dlg.m_strOVM4Layer		= m_config.m_strOVM4Layer;
	dlg.m_strOVM4Params		= m_config.m_strOVM4Params;

	dlg.m_hotkeyManager.Copy(&m_config.m_hotkeyManager);

	if (dlg.DoModal() == IDOK)
	{
		m_config.m_strOVM1Desc		= dlg.m_strOVM1Desc;
		m_config.m_strOVM1Message	= dlg.m_strOVM1Message;
		m_config.m_strOVM1Layer		= dlg.m_strOVM1Layer;
		m_config.m_strOVM1Params	= dlg.m_strOVM1Params;
	
		m_config.m_strOVM2Desc		= dlg.m_strOVM2Desc;
		m_config.m_strOVM2Message	= dlg.m_strOVM2Message;
		m_config.m_strOVM2Layer		= dlg.m_strOVM2Layer;
		m_config.m_strOVM2Params	= dlg.m_strOVM2Params;

		m_config.m_strOVM3Desc		= dlg.m_strOVM3Desc;
		m_config.m_strOVM3Message	= dlg.m_strOVM3Message;
		m_config.m_strOVM3Layer		= dlg.m_strOVM3Layer;
		m_config.m_strOVM3Params	= dlg.m_strOVM3Params;
	
		m_config.m_strOVM4Desc		= dlg.m_strOVM4Desc;
		m_config.m_strOVM4Message	= dlg.m_strOVM4Message;
		m_config.m_strOVM4Layer		= dlg.m_strOVM4Layer;
		m_config.m_strOVM4Params	= dlg.m_strOVM4Params;
		
		m_config.m_hotkeyManager.Copy(&dlg.m_hotkeyManager);
	}
}
//////////////////////////////////////////////////////////////////////
