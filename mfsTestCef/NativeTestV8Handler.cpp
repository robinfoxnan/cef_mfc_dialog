#include "pch.h"
#include "NativeTestV8Handler.h"

bool NativeTestV8Handler::Execute(const CefString& name,
    CefRefPtr<CefV8Value> object,
    const CefV8ValueList& arguments,
    CefRefPtr<CefV8Value>& retval,
    CefString& exception)
{
    if (name == "add") {
        if (arguments.size() == 2 && arguments[0]->IsInt() && arguments[1]->IsInt()) {
            // JS 调用了 C++ add(10,20)
            int a = arguments[0]->GetIntValue();
            int b = arguments[1]->GetIntValue();
            retval = CefV8Value::CreateInt(a + b);
            return true;
        }
    }

    return false;
}

