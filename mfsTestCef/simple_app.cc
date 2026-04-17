

#include "pch.h"
#include "simple_app.h"

#include <string>

#include "include/cef_browser.h"
#include "include/cef_command_line.h"
#include "include/views/cef_browser_view.h"
#include "include/views/cef_window.h"
#include "include/wrapper/cef_helpers.h"
#include "simple_handler.h"


//namespace {
//
//	// When using the Views framework this object provides the delegate
//	// implementation for the CefWindow that hosts the Views-based browser.
//	class SimpleWindowDelegate : public CefWindowDelegate {
//	public:
//		SimpleWindowDelegate(CefRefPtr<CefBrowserView> browser_view,
//			cef_runtime_style_t runtime_style,
//			cef_show_state_t initial_show_state)
//			: browser_view_(browser_view),
//			runtime_style_(runtime_style),
//			initial_show_state_(initial_show_state) {
//		}
//
//		SimpleWindowDelegate(const SimpleWindowDelegate&) = delete;
//		SimpleWindowDelegate& operator=(const SimpleWindowDelegate&) = delete;
//
//		void OnWindowCreated(CefRefPtr<CefWindow> window) override {
//			// Add the browser view and show the window.
//			window->AddChildView(browser_view_);
//
//			if (initial_show_state_ != CEF_SHOW_STATE_HIDDEN) {
//				window->Show();
//			}
//		}
//
//		void OnWindowDestroyed(CefRefPtr<CefWindow> window) override {
//			browser_view_ = nullptr;
//		}
//
//		bool CanClose(CefRefPtr<CefWindow> window) override {
//			// Allow the window to close if the browser says it's OK.
//			CefRefPtr<CefBrowser> browser = browser_view_->GetBrowser();
//			if (browser) {
//				return browser->GetHost()->TryCloseBrowser();
//			}
//			return true;
//		}
//
//		CefSize GetPreferredSize(CefRefPtr<CefView> view) override {
//			return CefSize(800, 600);
//		}
//
//		cef_show_state_t GetInitialShowState(CefRefPtr<CefWindow> window) override {
//			return initial_show_state_;
//		}
//
//		cef_runtime_style_t GetWindowRuntimeStyle() override {
//			return runtime_style_;
//		}
//
//	private:
//		CefRefPtr<CefBrowserView> browser_view_;
//		const cef_runtime_style_t runtime_style_;
//		const cef_show_state_t initial_show_state_;
//
//		IMPLEMENT_REFCOUNTING(SimpleWindowDelegate);
//	};
//
//	class SimpleBrowserViewDelegate : public CefBrowserViewDelegate {
//	public:
//		explicit SimpleBrowserViewDelegate(cef_runtime_style_t runtime_style)
//			: runtime_style_(runtime_style) {
//		}
//
//		SimpleBrowserViewDelegate(const SimpleBrowserViewDelegate&) = delete;
//		SimpleBrowserViewDelegate& operator=(const SimpleBrowserViewDelegate&) =
//			delete;
//
//		bool OnPopupBrowserViewCreated(CefRefPtr<CefBrowserView> browser_view,
//			CefRefPtr<CefBrowserView> popup_browser_view,
//			bool is_devtools) override {
//			// Create a new top-level Window for the popup. It will show itself after
//			// creation.
//			CefWindow::CreateTopLevelWindow(new SimpleWindowDelegate(
//				popup_browser_view, runtime_style_, CEF_SHOW_STATE_NORMAL));
//
//			// We created the Window.
//			return true;
//		}
//
//		cef_runtime_style_t GetBrowserRuntimeStyle() override {
//			return runtime_style_;
//		}
//
//	private:
//		const cef_runtime_style_t runtime_style_;
//
//		IMPLEMENT_REFCOUNTING(SimpleBrowserViewDelegate);
//	};
//
//}  // namespace



SimpleApp::SimpleApp() = default;

// ✅ 初始化静态 handler
CefRefPtr<NativeTestV8Handler> SimpleApp::m_handler;



void SimpleApp::OnContextCreated(
	CefRefPtr<CefBrowser> browser,
	CefRefPtr<CefFrame> frame,
	CefRefPtr<CefV8Context> context)
{
	CEF_REQUIRE_RENDERER_THREAD();

	// 1. 获取 window
	CefRefPtr<CefV8Value> window = context->GetGlobal();

	// 2. 只创建一次，全局生效，永不释放
	if (!m_handler) {
		m_handler = new NativeTestV8Handler();
	}

	// 3. 创建空对象（正确！无handler参数）
	CefRefPtr<CefV8Value> native = CefV8Value::CreateObject(nullptr, nullptr);

	// 创建函数
	CefRefPtr<CefV8Value> addFunc = CefV8Value::CreateFunction("add", m_handler);


	// 4. 给对象注册 add 方法（绑定 handler）
	native->SetValue("add",
		addFunc,
		V8_PROPERTY_ATTRIBUTE_NONE
	);

	// 5. 挂载到 window.native
	window->SetValue("native", native, V8_PROPERTY_ATTRIBUTE_NONE);
}

void SimpleApp::OnContextInitialized() {
	CEF_REQUIRE_UI_THREAD();
	// 这里什么都不写！
	// 绝对不能创建浏览器！
}

bool SimpleApp::createWindow(HWND hParent, const RECT& rect, const char * url) {

	bool isHolly = true;
	//isHolly = false;

	cef_runtime_style_t runtime_style = CEF_RUNTIME_STYLE_DEFAULT;
	
	if (isHolly)
		runtime_style = CEF_RUNTIME_STYLE_ALLOY;


	CefWindowInfo info;
	info.runtime_style = runtime_style;
	//info.style = WS_OVERLAPPEDWINDOW
	//	| WS_VISIBLE
	//	| WS_SYSMENU       // 系统菜单（必须）
	//	| WS_MINIMIZEBOX   // 最小化按钮
	//	| WS_MAXIMIZEBOX;  // 最大化按钮

	CefRect cefRect(rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top);
	info.SetAsChild(hParent, cefRect);

	CefBrowserSettings browser_settings;

	// 正确创建 SimpleHandler
	// ✅ 正确：获取已存在的单例，绝对不要 new！
	CefRefPtr<SimpleHandler> handler = new SimpleHandler(isHolly);



	CefBrowserHost::CreateBrowser(
		info,
		handler,
		url,
		browser_settings,
		nullptr,
		nullptr
	);
	//auto url = _T("https://www.baidu.com");

	//CefRefPtr<CefBrowserView> browser_view = CefBrowserView::CreateBrowserView(
	//	handler, url, browser_settings, nullptr, nullptr,
	//	new SimpleBrowserViewDelegate(runtime_style));

	return true;
}



CefRefPtr<CefClient> SimpleApp::GetDefaultClient() {
	// Called when a new browser window is created via Chrome style UI.
	return SimpleHandler::GetInstance();
}
