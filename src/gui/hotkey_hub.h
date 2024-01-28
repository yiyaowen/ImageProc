#pragma once

#include <Panel.h>

class HotkeyHub :
public d14uikit::Panel
{
public:
    HotkeyHub();

protected:
    void onKeyboard(d14uikit::KeyboardEvent* e) override;
};
