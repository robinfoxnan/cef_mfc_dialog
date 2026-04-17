// Copyright (c) 2013 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#ifndef CEF_TESTS_CEFSIMPLE_SIMPLE_APP_H_
#define CEF_TESTS_CEFSIMPLE_SIMPLE_APP_H_

#include "include/cef_app.h"
#include "include/cef_render_process_handler.h"
#include "NativeTestV8Handler.h"

// Implement application-level callbacks for the browser process.
class SimpleApp : public CefApp,
	public CefBrowserProcessHandler,
	public CefRenderProcessHandler {
public:
	SimpleApp();

	bool createWindow(HWND, const RECT& rect, const char* url);

	// CefApp methods:
	CefRefPtr<CefBrowserProcessHandler> GetBrowserProcessHandler() override {
		return this;
	}

	// 重写：获取渲染进程处理器
	CefRefPtr<CefRenderProcessHandler> GetRenderProcessHandler() override {
		return this;
	}

	// 👇 这个就是你要的 JS 环境创建回调！
	void OnContextCreated(
		CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefFrame> frame,
		CefRefPtr<CefV8Context> context) override;

	// CefBrowserProcessHandler methods:
	void OnContextInitialized() override;
	CefRefPtr<CefClient> GetDefaultClient() override;

private:
	// Include the default reference counting implementation.
	IMPLEMENT_REFCOUNTING(SimpleApp);

private:
	// ✅ 静态持有，保证全程不释放
	static CefRefPtr<NativeTestV8Handler> m_handler;
};

#endif  // CEF_TESTS_CEFSIMPLE_SIMPLE_APP_H_


