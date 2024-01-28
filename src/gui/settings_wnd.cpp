#include "gui_defs.h"
#include "settings_wnd.h"

#include <Application.h>
#include <Callback.h>
#include <ComboBox.h>
#include <ComboBoxItem.h>
#include <Label.h>
#include <PopupMenu.h>

using namespace d14uikit;

SettingsWnd::SettingsWnd()
{
    setTitle(L"设置");
    setFontTitle(Font(L"默认/正常/14"));

    auto clntArea = makeUIObject<Panel>(L"settingsWndClntArea");
    setContent(clntArea.get());

    auto themeLbl = makeUIObject<Label>(L"themeLbl");
    themeLbl->setParent(clntArea.get());
    themeLbl->setSize({ 100, 40 });
    themeLbl->setPosition({ 50, 100 });
    themeLbl->setText(L"外观");
    themeLbl->setFont(Font(L"默认/正常/14"));

    auto themeBox = makeUIObject<ComboBox>(L"themeBox");
    themeBox->setParent(clntArea.get());
    themeBox->setSize({ 200, 40 });
    themeBox->setPosition({ 120, 100 });
    themeBox->setRoundRadius(5);
    themeBox->setFont(Font(L"默认/正常/14"));

    auto menu = themeBox->dropDownMenu();
    std::wstring itemStrs[] =
    {
        L"浅色", L"深色", L"跟随系统"
    };
    std::list<MenuItem*> items = {};
    for (int i = 0; i < 3; ++i)
    {
        auto objname = L"themeBoxItem" + std::to_wstring(i);
        auto item = makeUIObject<ComboBoxItem>(objname);
        item->setText(itemStrs[i]);
        item->setFont(Font(L"默认/正常/14"));
        items.insert(items.end(), item.get());
    }
    menu->appendItem(items);

    menu->setWidth(themeBox->width());
    menu->setHeight((int)items.size() * 40);
    menu->setRoundExtension(5);

    themeBox->setCurrSelected(2);

    themeBox->D14_onSelectedChange(ComboBox, obj, text)
    {
        auto app = Application::app();
        if (text == L"浅色")
        {
            app->setThemeMode(L"Light");
        }
        else if (text == L"深色")
        {
            app->setThemeMode(L"Dark");
        }
        else if (text == L"跟随系统")
        {
            app->setUseSystemTheme(true);
        }
    };
}
