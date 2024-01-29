#include "gui/gui_defs.h"
#include "gui/hotkey_hub.h"
#include "gui/workspace.h"

#include <Application.h>
#include <Callback.h>
#include <MainWindow.h>

using namespace d14uikit;

#ifndef UNICODE
#define UNICODE
#endif
#include <Windows.h>

int main()
{
#ifdef _DEBUG
    SetDllDirectory(D14LIB_PATH L"debug");
#else
    SetDllDirectory(D14LIB_PATH L"release");
#endif
    Application app(L"ImageProcessor");
    app.setResizable(true);
    app.setMinSize({ 800, 600 });
    app.setClearType(true);
    app.setTextVertSmooth(true);
    app.setBmpQualityInterp(true);

    auto mwnd = makeUIObject<MainWindow>(L"mwnd");
    mwnd->setCaptionHeight(36);
    mwnd->setTitle(L"数字图像处理工具");
    mwnd->setFontTitle(Font(L"默认/正常/16"));
    mwnd->showMaximized();

    auto wkspace = makeUIObject<Workspace>(L"wkspace");
    mwnd->setContent(wkspace.get());

    auto hkhub = makeUIObject<HotkeyHub>(L"hkhub");

    return app.run();
}
