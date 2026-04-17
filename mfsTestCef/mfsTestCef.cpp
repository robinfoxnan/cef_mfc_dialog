
// mfsTestCef.cpp: 定义应用程序的类行为。
//

#include "pch.h"
#include "framework.h"
#include "mfsTestCef.h"
#include "mfsTestCefDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CmfsTestCefApp

BEGIN_MESSAGE_MAP(CmfsTestCefApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CmfsTestCefApp 构造

CmfsTestCefApp::CmfsTestCefApp()
{
	// 支持重新启动管理器
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}


// 唯一的 CmfsTestCefApp 对象

CmfsTestCefApp theApp;


BOOL CmfsTestCefApp::OnIdle(LONG lCount)
{
	// 让MFC先处理
	BOOL bResult = CWinApp::OnIdle(lCount);

	// ✅ CEF 在这里跑一帧！！！
	//CefDoMessageLoopWork();

	return bResult;
}

// CmfsTestCefApp 初始化

BOOL CmfsTestCefApp::InitInstance()
{
	// 如果应用程序存在以下情况，Windows XP 上需要 InitCommonControlsEx()
	// 使用 ComCtl32.dll 版本 6 或更高版本来启用可视化方式，
	//则需要 InitCommonControlsEx()。  否则，将无法创建窗口。
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// 将它设置为包括所有要在应用程序中使用的
	// 公共控件类。
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();


	AfxEnableControlContainer();

	// 创建 shell 管理器，以防对话框包含
	// 任何 shell 树视图控件或 shell 列表视图控件。
	CShellManager *pShellManager = new CShellManager;

	// 激活“Windows Native”视觉管理器，以便在 MFC 控件中启用主题
	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));

	// 标准初始化
	// 如果未使用这些功能并希望减小
	// 最终可执行文件的大小，则应移除下列
	// 不需要的特定初始化例程
	// 更改用于存储设置的注册表项
	// TODO: 应适当修改该字符串，
	// 例如修改为公司或组织名
	SetRegistryKey(_T("应用程序向导生成的本地应用程序"));

	// ======================== 正确 CEF 初始化 ========================
	HINSTANCE hInstance = AfxGetInstanceHandle();
	void* sandbox_info = nullptr;
	CefMainArgs main_args(hInstance);

	// ✅ 1. 先创建 app
	

	CefRefPtr<CefCommandLine> command_line = CefCommandLine::CreateCommandLine();
	command_line->InitFromString(::GetCommandLineW());

	//command_line->AppendSwitch("single-process");  // 如果需要调试
	command_line->AppendSwitch("no-sandbox");
	command_line->AppendSwitch("disable-gpu");
	command_line->AppendSwitch("disable-win32k-lockdown");
	command_line->AppendSwitchWithValue("device-auth-disabled", "1");
	command_line->AppendSwitchWithValue("allow-running-insecure-content", "1");
	command_line->AppendSwitch("--use-alloy-style");

	app = new SimpleApp();

	// ✅ 2. 只调用一次 CefExecuteProcess，必须传 app！！！
	int exit_code = CefExecuteProcess(main_args, app, sandbox_info);
	if (exit_code >= 0) {
		return FALSE;
	}

	// CEF 设置
	CefSettings settings;
	settings.no_sandbox = true;
	settings.multi_threaded_message_loop = false;
	settings.windowless_rendering_enabled = false;
	//settings.remote_debugging_port = 9222;

	// ✅ 3. 初始化，传入 app
	CefInitialize(main_args, settings, app, sandbox_info);

	

	////////////////////////////////////////////
	// 3. **先创建对话框，再显示**
	CmfsTestCefDlg dlg;
	m_pMainWnd = &dlg;
	
	//dlg.MyDoModal();
	//dlg.DoModal();

	
	//MSG msg;
	//while (GetMessage(&msg, NULL, 0, 0))
	//{
	//	TranslateMessage(&msg);
	//	DispatchMessage(&msg);

	//	// 让 CEF 跑一帧（必须加！）
	//	CefDoMessageLoopWork();
	//}

	dlg.Create(IDD_MFSTESTCEF_DIALOG);
	dlg.ShowWindow(SW_SHOW);
	CefRunMessageLoop();

	

#if !defined(_AFXDLL) && !defined(_AFX_NO_MFC_CONTROLS_IN_DIALOGS)
	ControlBarCleanUp();
#endif

	// 由于对话框已关闭，所以将返回 FALSE 以便退出应用程序，
	//  而不是启动应用程序的消息泵。

	// ========================
	
	CefShutdown();
	// ========================

	// 删除上面创建的 shell 管理器。
	if (pShellManager != nullptr)
	{
		delete pShellManager;
	}

	return FALSE;
}

