// Copyright (c) 2013 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#ifndef CEF_TESTS_CEFSIMPLE_SIMPLE_HANDLER_H_
#define CEF_TESTS_CEFSIMPLE_SIMPLE_HANDLER_H_

#include <list>

#include "include/cef_client.h"
#include "include/cef_context_menu_handler.h"
#include "include/cef_browser.h"
#include "include/cef_frame.h"
#include "include/cef_keyboard_handler.h"


// 自定义菜单ID（自己随便定义，不要重复）
enum MenuID {
	MENU_ID_OPEN_NEW_TAB = 11001,  // 新标签打开
	MENU_ID_AI_SUMMARIZE,          // AI总结
	MENU_ID_AI_EXPLAIN,            // AI解释
	MENU_ID_COPY_URL,              // 复制链接
};

class SimpleHandler : public CefClient, 
	public CefDisplayHandler,
	public CefLifeSpanHandler,
	public CefLoadHandler, 
	public CefContextMenuHandler, 
	public CefKeyboardHandler {
public:
	explicit SimpleHandler(bool is_alloy_style);
	~SimpleHandler() override;

	void setSize(int cx, int cy);

	// Provide access to the single global instance of this object.
	static SimpleHandler* GetInstance();

	// 上下文菜单重写
	// 页面加载完成后注入AI功能  
	void OnLoadingStateChange(CefRefPtr<CefBrowser> browser,
		bool isLoading,
		bool canGoBack,
		bool canGoForward) override {
		if (!isLoading) {
			// 注入AI助手脚本  
			//InjectAIScript(browser);
		}
	}

	// 自定义右键菜单  
	// 1. 右键菜单构建
	void OnBeforeContextMenu(CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefFrame> frame,
		CefRefPtr<CefContextMenuParams> params,
		CefRefPtr<CefMenuModel> model) override;

	// 2. 菜单点击事件
	bool OnContextMenuCommand(CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefFrame> frame,
		CefRefPtr<CefContextMenuParams> params,
		int command_id,
		EventFlags event_flags) override;


	// CefClient methods:
	CefRefPtr<CefDisplayHandler> GetDisplayHandler() override { return this; }
	CefRefPtr<CefLifeSpanHandler> GetLifeSpanHandler() override { return this; }
	CefRefPtr<CefLoadHandler> GetLoadHandler() override { return this; }
	// 菜单上下文
	CefRefPtr<CefContextMenuHandler> GetContextMenuHandler() override {
		return this;
	}
	// 2. 告诉 CEF：用我这个键盘处理器（关键！）
	CefRefPtr<CefKeyboardHandler> GetKeyboardHandler() override {
		return this; // 自己处理键盘
	}



	// CefDisplayHandler methods:
	void OnTitleChange(CefRefPtr<CefBrowser> browser,
		const CefString& title) override;

	// CefLifeSpanHandler methods:
	void OnAfterCreated(CefRefPtr<CefBrowser> browser) override;
	bool DoClose(CefRefPtr<CefBrowser> browser) override;
	void OnBeforeClose(CefRefPtr<CefBrowser> browser) override;

	// CefLoadHandler methods:
	void OnLoadError(CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefFrame> frame,
		ErrorCode errorCode,
		const CefString& errorText,
		const CefString& failedUrl) override;

	// 加入这个方法
	bool OnBeforePopup(CefRefPtr<CefBrowser> browser,
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
		bool* no_javascript_access) override;



	// 键盘事件处理（必须重写）
	bool OnKeyEvent(CefRefPtr<CefBrowser> browser,
		const CefKeyEvent& event,
		CefEventHandle os_event) override;

	//void ShowMainWindow();

	// Request that all existing browser windows close.
	void CloseAllBrowsers(bool force_close);

	bool IsClosing() const { return is_closing_; }

private:
	// Platform-specific implementation.
	void PlatformTitleChange(CefRefPtr<CefBrowser> browser,
		const CefString& title);
	void PlatformShowWindow(CefRefPtr<CefBrowser> browser);

	// True if this client is Alloy style, otherwise Chrome style.
	const bool is_alloy_style_;

	// List of existing browser windows. Only accessed on the CEF UI thread.
	typedef std::list<CefRefPtr<CefBrowser>> BrowserList;
	BrowserList browser_list_;

	bool is_closing_ = false;

	// Include the default reference counting implementation.
	IMPLEMENT_REFCOUNTING(SimpleHandler);
public:
	bool loadUrl(const char* str);
	void callJS(const char* funName, const char* jsonParam);
};

#endif  // CEF_TESTS_CEFSIMPLE_SIMPLE_HANDLER_H_


