#include "dip_window.h"
#include "gui_defs.h"

#include <Application.h>

using namespace d14uikit;

DIPWindow::DIPWindow()
{
    setParent(getUIObject(L"mwnd"));
    setSize({ 500, 300 });
    setResizable(false);
    setDecoBarHeight(0);
    setMinimizeButton(false);
    setMaximizeButton(false);
    setVisible(false);
    setEnabled(false);
}

void DIPWindow::onTrigger()
{
    if (visible() || enabled())
    {
        setVisible(false);
        setEnabled(false);
    }
    else // pop up
    {
        moveTopmost();
        auto imgArea = getUIObject(L"imgArea");
        setPosition
        ({
            imgArea->absX() +
            (imgArea->width() - width()) / 2,

            imgArea->absY() +
            (imgArea->height() - height()) / 2
        });
        setVisible(true);
        setEnabled(true);
    }
}

void DIPWindow::onClose() { onTrigger(); }
