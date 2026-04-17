// Copyright (c) 2013 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include "pch.h"
#include "simple_handler.h"
#include <Windows.h>  // 必须加

#include <sstream>
#include <string>

#include "include/base/cef_callback.h"
#include "include/cef_app.h"
#include "include/cef_parser.h"
#include "include/views/cef_browser_view.h"
#include "include/views/cef_window.h"
#include "include/wrapper/cef_closure_task.h"
#include "include/wrapper/cef_helpers.h"

namespace {

	SimpleHandler* g_instance = nullptr;

	// Returns a data: URI with the specified contents.
	std::string GetDataURI(const std::string& data, const std::string& mime_type) {
		return "data:" + mime_type + ";base64," +
			CefURIEncode(CefBase64Encode(data.data(), data.size()), false)
			.ToString();
	}

}  // namespace

SimpleHandler::SimpleHandler(bool is_alloy_style)
	: is_alloy_style_(is_alloy_style) {
	DCHECK(!g_instance);
	g_instance = this;
}

SimpleHandler::~SimpleHandler() {
	g_instance = nullptr;
}

// static
SimpleHandler* SimpleHandler::GetInstance() {
	return g_instance;
}

void SimpleHandler::OnTitleChange(CefRefPtr<CefBrowser> browser,
	const CefString& title) {
	CEF_REQUIRE_UI_THREAD();

	if (auto browser_view = CefBrowserView::GetForBrowser(browser)) {
		// Set the title of the window using the Views framework.
		CefRefPtr<CefWindow> window = browser_view->GetWindow();
		if (window) {
			window->SetTitle(title);
		}
	}
	else if (is_alloy_style_) {
		// Set the title of the window using platform APIs.
		PlatformTitleChange(browser, title);
	}
}

void SimpleHandler::OnAfterCreated(CefRefPtr<CefBrowser> browser) {
	CEF_REQUIRE_UI_THREAD();

	// Sanity-check the configured runtime style.
	CHECK_EQ(is_alloy_style_ ? CEF_RUNTIME_STYLE_ALLOY : CEF_RUNTIME_STYLE_CHROME,
		browser->GetHost()->GetRuntimeStyle());

	// Add to the list of existing browsers.
	browser_list_.push_back(browser);
}

bool SimpleHandler::DoClose(CefRefPtr<CefBrowser> browser) {
	CEF_REQUIRE_UI_THREAD();

	// Closing the main window requires special handling. See the DoClose()
	// documentation in the CEF header for a detailed destription of this
	// process.
	if (browser_list_.size() == 1) {
		// Set a flag to indicate that the window close should be allowed.
		is_closing_ = true;
	}

	// Allow the close. For windowed browsers this will result in the OS close
	// event being sent.
	return false;
}

void SimpleHandler::OnBeforeClose(CefRefPtr<CefBrowser> browser) {
	CEF_REQUIRE_UI_THREAD();

	// Remove from the list of existing browsers.
	BrowserList::iterator bit = browser_list_.begin();
	for (; bit != browser_list_.end(); ++bit) {
		if ((*bit)->IsSame(browser)) {
			browser_list_.erase(bit);
			break;
		}
	}

	if (browser_list_.empty()) {
		// All browser windows have closed. Quit the application message loop.
		CefQuitMessageLoop();
	}
}

void SimpleHandler::OnLoadError(CefRefPtr<CefBrowser> browser,
	CefRefPtr<CefFrame> frame,
	ErrorCode errorCode,
	const CefString& errorText,
	const CefString& failedUrl) {
	CEF_REQUIRE_UI_THREAD();

	// Allow Chrome to show the error page.
	if (!is_alloy_style_) {
		return;
	}

	// Don't display an error for downloaded files.
	if (errorCode == ERR_ABORTED) {
		return;
	}

	// Display a load error message using a data: URI.
	std::stringstream ss;
	ss << "<html><body bgcolor=\"white\">"
		"<h2>Failed to load URL "
		<< std::string(failedUrl) << " with error " << std::string(errorText)
		<< " (" << errorCode << ").</h2></body></html>";

	frame->LoadURL(GetDataURI(ss.str(), "text/html"));
}

//void SimpleHandler::ShowMainWindow() {
//  if (!CefCurrentlyOn(TID_UI)) {
//    // Execute on the UI thread.
//    CefPostTask(TID_UI, base::BindOnce(&SimpleHandler::ShowMainWindow, this));
//    return;
//  }
//
//  if (browser_list_.empty()) {
//    return;
//  }
//
//  auto main_browser = browser_list_.front();
//
//  if (auto browser_view = CefBrowserView::GetForBrowser(main_browser)) {
//    // Show the window using the Views framework.
//    if (auto window = browser_view->GetWindow()) {
//      window->Show();
//    }
//  } else if (is_alloy_style_) {
//    PlatformShowWindow(main_browser);
//  }
//}

void SimpleHandler::CloseAllBrowsers(bool force_close) {
	if (!CefCurrentlyOn(TID_UI)) {
		// Execute on the UI thread.
		CefPostTask(TID_UI, base::BindOnce(&SimpleHandler::CloseAllBrowsers, this,
			force_close));
		return;
	}

	if (browser_list_.empty()) {
		return;
	}

	for (const auto& browser : browser_list_) {
		browser->GetHost()->CloseBrowser(force_close);
	}
}

void SimpleHandler::PlatformTitleChange(CefRefPtr<CefBrowser> browser,
	const CefString& title) {
	CefWindowHandle hwnd = browser->GetHost()->GetWindowHandle();
	if (hwnd) {
		SetWindowText(hwnd, std::string(title).c_str());
	}
}


#if !defined(OS_MAC)
void SimpleHandler::PlatformShowWindow(CefRefPtr<CefBrowser> browser) {
	NOTIMPLEMENTED();
}
#endif


bool SimpleHandler::OnBeforePopup(CefRefPtr<CefBrowser> browser,
	CefRefPtr<CefFrame> frame,
	int popup_id,
	const CefString& target_url,
	const CefString& target_frame_name,
	WindowOpenDisposition target_disposition,
	bool user_gesture,
	const CefPopupFeatures& popupFeatures,
	CefWindowInfo& windowInfo,
	CefRefPtr<CefClient>& client,
	CefBrowserSettings& settings,
	CefRefPtr<CefDictionaryValue>& extra_info,
	bool* no_javascript_access)
{
	// 阻止弹出新窗口
	// 并把链接在当前浏览器打开
	if (!target_url.empty()) {
		frame->LoadURL(target_url);
	}

	// 返回 true = 取消弹窗
	return true;
}

void SimpleHandler::setSize(int cx, int cy) {
	if (!browser_list_.empty())
	{
		// 获取浏览器句柄
		HWND browserHwnd = browser_list_.front()->GetHost()->GetWindowHandle();
		if (browserHwnd)
		{
			// 拉伸浏览器到对话框大小
			SetWindowPos(browserHwnd, NULL, 0, 0, cx, cy, SWP_NOZORDER);
		}
	}
}

void SimpleHandler::OnBeforeContextMenu(
	CefRefPtr<CefBrowser> browser,
	CefRefPtr<CefFrame> frame,
	CefRefPtr<CefContextMenuParams> params,
	CefRefPtr<CefMenuModel> model)
{

	//// 清空默认菜单
	////model->Clear();

	//// 添加你自己的菜单
	//model->AddItem(MENU_ID_OPEN_NEW_TAB, _T("新标签打开"));
	model->AddItem(MENU_ID_AI_SUMMARIZE, _T("AI总结"));
	//model->AddItem(MENU_ID_AI_EXPLAIN, L"AI解释");
	//model->AddItem(MENU_ID_COPY_URL, L"复制链接");
}

bool SimpleHandler::OnContextMenuCommand(CefRefPtr<CefBrowser> browser,
	CefRefPtr<CefFrame> frame,
	CefRefPtr<CefContextMenuParams> params,
	int command_id,
	EventFlags event_flags)
{
	CefString url = params->GetLinkUrl();  // 获取当前链接

	switch (command_id)
	{
	//case MENU_ID_OPEN_NEW_TAB:
	//	browser->CreateBrowser(NULL, frame, url, CefBrowserSettings(), NULL);
	//	break;

	case MENU_ID_AI_SUMMARIZE:
		// 安全转换：UTF8 → WideChar（Windows 原生能看懂）
		

		MessageBoxW(NULL, L"点击了 AI总结", L"menu", MB_OK);
	
		break;

	case MENU_ID_AI_EXPLAIN:
		MessageBoxW(NULL, L"你点击了：AI解释", L"菜单", MB_OK);
		break;

	//case MENU_ID_COPY_URL:
	//	if (!url.empty()) {
	//		frame->CopyURLToClipboard();
	//	}
		break;
	}

	return true;
}

bool SimpleHandler::loadUrl(const char* str)
{
	// 1. 判断URL是否有效
	if (!str || strlen(str) == 0)
		return false;

	// 2. 判断浏览器列表是否为空
	if (browser_list_.empty())
		return false;

	// 3. 取出第一个（也是唯一一个）浏览器对象
	CefRefPtr<CefBrowser> browser = browser_list_.front();
	if (!browser)
		return false;

	// 4. 加载URL
	browser->GetMainFrame()->LoadURL(CefString(str));

	// 5. 成功返回
	return true;
}

// 通用：调用JS函数（支持 字符串、JSON、任意参数）
void SimpleHandler::callJS(const char* funName, const char* jsonParam)
{
	if (browser_list_.empty())
		return;

	CefRefPtr<CefBrowser> browser = browser_list_.front();
	if (!browser)
		return;

	CefRefPtr<CefFrame> frame = browser->GetMainFrame();
	if (!frame)
		return;

	// ==============================================
	// ✅ 核心：统一拼接，自动传参
	// ==============================================
	std::string script = std::string(funName) + "(" + jsonParam + ");";

	// 执行JS
	frame->ExecuteJavaScript(CefString(script), frame->GetURL(), 0);
}


// 1. 重写键盘事件（捕获 F12）
bool SimpleHandler::OnKeyEvent(CefRefPtr<CefBrowser> browser,
	const CefKeyEvent& event,
	CefEventHandle os_event) {
	// 只处理：按下 F12（key code = 123）
	// F12 键码 123
	if (event.type == KEYEVENT_RAWKEYDOWN && event.windows_key_code == 123)
	{
		if (browser && browser->GetHost())
		{
			// ========================
		// 正确：必须用这种方式创建窗口
		// ========================
			CefWindowInfo info;
			RECT rc = { 0, 0, 800, 600 };

			// 关键：必须用 SetAsChild 或 SetAsPopup 并传入父窗口
			HWND hParent = browser->GetHost()->GetWindowHandle();
			info.SetAsPopup(hParent, L"CEF DevTools");

			CefBrowserSettings settings;
			CefPoint point;

			// 调用完整版
			browser->GetHost()->ShowDevTools(
				info,
				this,       // 必须传this
				settings,
				point
			);
		}
		return true;
	}

	// 其他键不处理
	return false;
}
