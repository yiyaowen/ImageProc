#pragma once

#include <Window.h>

class DIPWindow :
public d14uikit::Window
{
public:
    DIPWindow();
    void onTrigger();

protected:
    void onClose() override;
};
