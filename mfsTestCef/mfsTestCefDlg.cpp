
// mfsTestCefDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "mfsTestCef.h"
#include "mfsTestCefDlg.h"
#include "afxdialogex.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#endif




// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CmfsTestCefDlg 对话框



CmfsTestCefDlg::CmfsTestCefDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MFSTESTCEF_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CmfsTestCefDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BROWSER_CONTAINER, mContainer);
}

BEGIN_MESSAGE_MAP(CmfsTestCefDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_SHOWWINDOW()
	ON_WM_QUERYDRAGICON()
	ON_WM_CLOSE()
	ON_WM_SIZE() 

	ON_COMMAND(ID_TOOL_CMD1, &CmfsTestCefDlg::OnToolCmd1)
	ON_COMMAND(ID_TOOL_CMD2, &CmfsTestCefDlg::OnToolCmd2)
	ON_COMMAND(ID_TOOL_CMD3, &CmfsTestCefDlg::OnToolCmd3)
	ON_COMMAND(ID_TOOL_CMD4, &CmfsTestCefDlg::OnToolCmd4)
END_MESSAGE_MAP()




// CmfsTestCefDlg 消息处理程序

BOOL CmfsTestCefDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ✅✅✅ 就这三行，加在最前面！！！
	ModifyStyle(DS_MODALFRAME, 0);
	ModifyStyle(0, WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SYSMENU);
	SetWindowPos(NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
	// ==============================================
	// ✅【必须放在第一行！】先设置样式，再初始化其他！
	// ==============================================
	//LONG_PTR style = GetWindowLongPtr(m_hWnd, GWL_STYLE);
	//style |= WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX;
	//SetWindowLongPtr(m_hWnd, GWL_STYLE, style);
	SetWindowPos(NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);

	// 下面的代码 完全不动！
	CDialogEx::OnInitDialog();

	// 【已经注释】// ModifyStyle(0, WS_CLIPCHILDREN);

	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	SetIcon(m_hIcon, TRUE);
	SetIcon(m_hIcon, FALSE);

	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_TOOLBAR1))
	{
		TRACE0("创建工具条失败\n");
	}
	m_wndToolBar.SetSizes(CSize(40, 40), CSize(32, 32));
	m_wndToolBar.SetBarStyle(m_wndToolBar.GetBarStyle() | CBRS_TOOLTIPS | CBRS_FLYBY);
	RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0);

	// ============ 嵌入 CEF ============
	CmfsTestCefApp* app = dynamic_cast<CmfsTestCefApp*>(AfxGetApp());
	if (app != NULL) {
		CRect rc;
		mContainer.GetClientRect(&rc);
		app->app->createWindow(mContainer.GetSafeHwnd(), rc, "d:\\obuild\\cef_test.html");
		bInited = true;
	}

	return TRUE;
}

bool CmfsTestCefDlg::init() {
	

	return true;

}

void CmfsTestCefDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialogEx::OnShowWindow(bShow, nStatus);

}

void CmfsTestCefDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

//void CmfsTestCefDlg::OnPaint()
//{
//	if (IsIconic())
//	{
//		CPaintDC dc(this); // 用于绘制的设备上下文
//
//		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);
//
//		// 使图标在工作区矩形中居中
//		int cxIcon = GetSystemMetrics(SM_CXICON);
//		int cyIcon = GetSystemMetrics(SM_CYICON);
//		CRect rect;
//		GetClientRect(&rect);
//		int x = (rect.Width() - cxIcon + 1) / 2;
//		int y = (rect.Height() - cyIcon + 1) / 2;
//
//		// 绘制图标
//		dc.DrawIcon(x, y, m_hIcon);
//	}
//	else
//	{
//		CDialogEx::OnPaint();
//	}
//}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CmfsTestCefDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CmfsTestCefDlg::OnClose()
{
	// 1. 获取handler
	auto handler = SimpleHandler::GetInstance();
	if (handler && !handler->IsClosing()) {
		// 2. 开始关闭浏览器（异步！不会阻塞）
		handler->CloseAllBrowsers(true);
		// 3. 【关键】只关闭浏览器，不要在这里关闭窗口！
		return;
	}

	// 4. 只有浏览器真正全部关闭后，才会走到这里！
	CDialogEx::OnClose();
	DestroyWindow();
}

void CmfsTestCefDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	// 让 CEF 浏览器充满整个对话框
	
	if (bInited) {
		CefRefPtr<SimpleHandler> handler = SimpleHandler::GetInstance();
		if (handler) {
			handler->setSize(cx, cy);
		}
	}
	
}
void CmfsTestCefDlg::OnToolCmd1()
{
	// TODO: 在此添加命令处理程序代码
	SimpleHandler::GetInstance()->loadUrl("d:\\obuild\\cef_test.html");
}

void CmfsTestCefDlg::OnToolCmd2()
{
	// TODO: 在此添加命令处理程序代码
	SimpleHandler::GetInstance()->callJS("showMsgFromCppJson", R"({"name":"robin","age":"30"})");

	// // 传递字符串必须加双引号！
	//callJS("showMsgFromCpp", "\"我是普通字符串\"");
}

void CmfsTestCefDlg::OnToolCmd3()
{
	// TODO: 在此添加命令处理程序代码
	SimpleHandler::GetInstance()->loadUrl("d:\\obuild\\cef_chat.html");
}

void CmfsTestCefDlg::OnToolCmd4()
{
	SimpleHandler::GetInstance()->callJS("showMsgFromCpp", "\"你好，我是C++发来的消息\"");
}

void CmfsTestCefDlg::MyDoModal() {
	// 1. 创建非模态对话框
	Create(IDD_MFSTESTCEF_DIALOG);
	ShowWindow(SW_SHOW);

	// 2. 你自己的消息循环！
	MSG msg;
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		// 你可以在这里随便改消息、拦截、加逻辑
		if (!PreTranslateMessage(&msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			CefDoMessageLoopWork();
		}

		// 想退出就 break
		if (msg.message == WM_QUIT)
			break;
	}

	// 3. 销毁
	DestroyWindow();
}