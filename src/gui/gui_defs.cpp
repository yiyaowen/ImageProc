#include "file_menu.h"
#include "gui_defs.h"
#include "image_area.h"

#include <sstream>

#include <Callback.h>

using namespace d14uikit;

UIObjectMap g_uiobjs = {};

std::wstring formd(double in, int precision)
{
    std::ostringstream out;
    out.precision(precision);
    out << std::fixed << in;
    auto str = out.str();
    return  { str.begin(), str.end() };
}

std::wstring formd(const std::wstring& in, int precision)
{
    return formd(std::wcstod(
        in.c_str(), nullptr), precision);
}
