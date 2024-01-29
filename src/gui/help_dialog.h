#pragma once

#include <memory>

#include <Window.h>

class HelpDialog :
public d14uikit::Window
{
    static void initRegVal();
    static void setRegVal(bool value);

    static void handleError();

    std::shared_ptr<Panel> m_bkgn = {};

public:
    HelpDialog();

    static bool getRegVal();
    void setPopup(bool value);
};
