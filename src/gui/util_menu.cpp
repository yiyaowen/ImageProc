extern "C"
{
#include "../dip/dip_funcs.h"
}
#include "gui_defs.h"
#include "image_area.h"
#include "util_menu.h"

#include <Callback.h>
#include <MenuItem.h>
#include <MenuSeparator.h>

using namespace d14uikit;

UtilMenu::UtilMenu()
{
    setSize({ 200, 182 });
    setBkgnTriggerPanel(true);

    auto grayBmpCmd = makeUIObject<MenuItem>(L"grayBmpCmd");
    grayBmpCmd->setHeight(32);
    grayBmpCmd->setText(L"提取灰度");
    grayBmpCmd->setFont(Font(L"默认/正常/14"));

    auto sep1 = makeUIObject<MenuSeparator>(L"utilMenuSep1");

    auto invertBmpCmd = makeUIObject<MenuItem>(L"invertBmpCmd");
    invertBmpCmd->setHeight(32);
    invertBmpCmd->setText(L"颜色反相");
    invertBmpCmd->setFont(Font(L"默认/正常/14"));

    auto sep2 = makeUIObject<MenuSeparator>(L"utilMenuSep2");

    auto nearestInterpCmd = makeUIObject<MenuItem>(L"nearestInterpCmd");
    nearestInterpCmd->setHeight(32);
    nearestInterpCmd->setText(L"最邻近插值");
    nearestInterpCmd->setFont(Font(L"默认/正常/14"));
    nearestInterpCmd->setHotkeyText(L"✓");
    nearestInterpCmd->setFontHotkey(Font(L"默认/正常/14"));
    nearestInterpCmd->setInstant(false);

    auto bilinearInterpCmd = makeUIObject<MenuItem>(L"bilinearInterpCmd");
    bilinearInterpCmd->setHeight(32);
    bilinearInterpCmd->setText(L"双线性插值");
    bilinearInterpCmd->setFont(Font(L"默认/正常/14"));
    bilinearInterpCmd->setHotkeyText(L"");
    bilinearInterpCmd->setFontHotkey(Font(L"默认/正常/14"));
    bilinearInterpCmd->setInstant(false);

    auto cubicInterpCmd = makeUIObject<MenuItem>(L"cubicInterpCmd");
    cubicInterpCmd->setHeight(32);
    cubicInterpCmd->setText(L"三次内插值");
    cubicInterpCmd->setFont(Font(L"默认/正常/14"));
    cubicInterpCmd->setHotkeyText(L"");
    cubicInterpCmd->setFontHotkey(Font(L"默认/正常/14"));
    cubicInterpCmd->setInstant(false);

    appendItem
    ({
        grayBmpCmd.get(),
        sep1.get(),
        invertBmpCmd.get(),
        sep2.get(),
        nearestInterpCmd.get(),
        bilinearInterpCmd.get(),
        cubicInterpCmd.get()
    });
}

void UtilMenu::onTriggerMenuItem(const std::wstring& text)
{
    auto imgArea = getUIObject<ImageArea>(L"imgArea");

    if (text == L"提取灰度")
    {
        imgArea->addBmp(L"提取灰度", gray(imgArea->currBmp()));
    }
    else if (text == L"颜色反相")
    {
        imgArea->addBmp(L"颜色反相", invert(imgArea->currBmp()));
    }
    else // change btimap scale mode
    {
        auto nearestInterpCmd = getUIObject<MenuItem>(L"nearestInterpCmd");
        auto bilinearInterpCmd = getUIObject<MenuItem>(L"bilinearInterpCmd");
        auto cubicInterpCmd = getUIObject<MenuItem>(L"cubicInterpCmd");

        if (text == L"最邻近插值")
        {
            imgArea->scaleMode = ImageArea::ScaleMode::Nearest;
            imgArea->updateScaledBmp();

            nearestInterpCmd->setHotkeyText(L"✓");
            bilinearInterpCmd->setHotkeyText(L"");
            cubicInterpCmd->setHotkeyText(L"");
        }
        else if (text == L"双线性插值")
        {
            imgArea->scaleMode = ImageArea::ScaleMode::Bilinear;
            imgArea->updateScaledBmp();

            nearestInterpCmd->setHotkeyText(L"");
            bilinearInterpCmd->setHotkeyText(L"✓");
            cubicInterpCmd->setHotkeyText(L"");
        }
        else if (text == L"三次内插值")
        {
            imgArea->scaleMode = ImageArea::ScaleMode::Cubic;
            imgArea->updateScaledBmp();

            nearestInterpCmd->setHotkeyText(L"");
            bilinearInterpCmd->setHotkeyText(L"");
            cubicInterpCmd->setHotkeyText(L"✓");
        }
    }
}
