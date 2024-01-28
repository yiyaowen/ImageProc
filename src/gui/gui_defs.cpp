#include "gui_defs.h"

#include <sstream>

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
