#pragma once

#include <memory>
#include <unordered_map>

#include <Panel.h>

using UIObjectMap = std::unordered_map
<std::wstring, std::shared_ptr<d14uikit::Panel>>;

extern UIObjectMap g_uiobjs;

template<typename T, typename... Types>
std::shared_ptr<T> makeUIObject
(const std::wstring& name, Types&& ...args)
{
    auto uiobj = std::make_shared<T>(args...);
    g_uiobjs[name] = uiobj; return uiobj;
}
template<typename T = d14uikit::Panel>
T* getUIObject(const std::wstring& name)
{
    return dynamic_cast<T*>(g_uiobjs[name].get());
}
std::wstring formd(double in, int precision=1);
std::wstring formd(const std::wstring& in, int precision=1);

struct NameCodeOrder
{
    bool operator()(const std::wstring& a, const std::wstring& b) const
    {
        auto da = std::wcstol(a.c_str(), nullptr, 10);
        auto db = std::wcstol(b.c_str(), nullptr, 10);
        return da < db;
    }
};
