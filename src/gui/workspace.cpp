#include "gui_defs.h"
#include "image_area.h"
#include "menu_bar.h"
#include "status_bar.h"
#include "step_panel.h"
#include "workspace.h"

using namespace d14uikit;

Workspace::Workspace()
{
    GeoInfo geoInfo = {};

    auto menuBar = makeUIObject<MenuBar>(L"menuBar");
    geoInfo = {};
    geoInfo.Left.ToLeft = 0;
    geoInfo.Top.ToTop = 0;
    geoInfo.Right.ToRight = 0;
    geoInfo.Bottom.ToTop = 42;
    addElement(menuBar.get(), geoInfo);

    auto statusBar = makeUIObject<StatusBar>(L"statusBar");
    geoInfo = {};
    geoInfo.Left.ToLeft = 0;
    geoInfo.Top.ToBottom = 34;
    geoInfo.Right.ToRight = 0;
    geoInfo.Bottom.ToBottom = 0;
    addElement(statusBar.get(), geoInfo);
    auto sz = statusBar->size();

    auto imgArea = makeUIObject<ImageArea>(L"imgArea");
    geoInfo = {};
    geoInfo.Left.ToLeft = 0;
    geoInfo.Top.ToTop = 42;
    geoInfo.Right.ToRight = 305;
    geoInfo.Bottom.ToBottom = 34;
    addElement(imgArea.get(), geoInfo);

    auto stepPanel = makeUIObject<StepPanel>(L"stepPanel");
    geoInfo = {};
    geoInfo.Left.ToRight = 300;
    geoInfo.Top.ToTop = 42;
    geoInfo.Right.ToRight = 0;
    geoInfo.Bottom.ToBottom = 34;
    addElement(stepPanel.get(), geoInfo);
    stepPanel->moveTopmost();
    stepPanel->setMinWidth(300);
    stepPanel->setMaxWidth(400);
}
