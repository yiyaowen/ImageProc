#pragma once

#include <PopupMenu.h>

class UtilMenu :
public d14uikit::PopupMenu
{
public:
    UtilMenu();

protected:
    void onTriggerMenuItem(const std::wstring& text) override;
};
