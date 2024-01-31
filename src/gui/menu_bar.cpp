#include "file_menu.h"
#include "freq_proc_wnd.h"
#include "geo_trans_wnd.h"
#include "gui_defs.h"
#include "help_dialog.h"
#include "histogram_wnd.h"
#include "menu_bar.h"
#include "settings_wnd.h"
#include "spac_proc_wnd.h"

#include <Callback.h>
#include <ConstraintLayout.h>
#include <Label.h>
#include <OutlinedButton.h>

using namespace d14uikit;

MenuBar::MenuBar()
{
    setOpacity(1.0f);
    setOutlineOpacity(1.0f);

    auto fileBtn = makeUIObject<FlatButton>(L"fileBtn");
    fileBtn->setParent(this);
    fileBtn->setSize({ 60, 32 });
    fileBtn->setPosition({ 5, 5 });
    fileBtn->setRoundRadius(5);
    fileBtn->setText(L"文件");
    fileBtn->setFont(Font(L"默认/正常/14"));

    auto fileMenu = makeUIObject<FileMenu>(L"fileMenu");
    fileBtn->D14_onMouseButtonRelease(p, e)
    {
        if (e->left())
        {
            auto fileMenu = getUIObject<PopupMenu>(L"fileMenu");
            fileMenu->setPosition
            ({
                p->absX(),
                p->absY() + p->height() + fileMenu->roundExtension()
            });
            fileMenu->setActivated(true);
        }
    };

    auto geoTransBtn = makeUIObject<FlatButton>(L"geoTransBtn");
    geoTransBtn->setParent(this);
    geoTransBtn->setSize({ 100, 32});
    geoTransBtn->setPosition({ 65, 5 });
    geoTransBtn->setRoundRadius(5);
    geoTransBtn->setText(L"几何变换");
    geoTransBtn->setFont(Font(L"默认/正常/14"));

    auto geoTransWnd = makeUIObject<GeoTransWnd>(L"geoTransWnd");
    geoTransBtn->D14_onMouseButtonRelease(p, e)
    {
        if (e->left())
        {
            getUIObject<DIPWindow>(L"geoTransWnd")->onTrigger();
        }
    };

    auto histogramBtn = makeUIObject<FlatButton>(L"histogramBtn");
    histogramBtn->setParent(this);
    histogramBtn->setSize({ 120, 32});
    histogramBtn->setPosition({ 165, 5 });
    histogramBtn->setRoundRadius(5);
    histogramBtn->setText(L"直方图操作");
    histogramBtn->setFont(Font(L"默认/正常/14"));

    auto histogramWnd = makeUIObject<HistogramWnd>(L"histogramWnd");
    histogramBtn->D14_onMouseButtonRelease(p, e)
    {
        if (e->left())
        {
            getUIObject<DIPWindow>(L"histogramWnd")->onTrigger();
        }
    };

    auto spacProcBtn = makeUIObject<FlatButton>(L"spacProcBtn");
    spacProcBtn->setParent(this);
    spacProcBtn->setSize({ 100, 32});
    spacProcBtn->setPosition({ 285, 5 });
    spacProcBtn->setRoundRadius(5);
    spacProcBtn->setText(L"空域处理");
    spacProcBtn->setFont(Font(L"默认/正常/14"));

    auto spacProcWnd = makeUIObject<SpacProcWnd>(L"spacProcWnd");
    spacProcBtn->D14_onMouseButtonRelease(p, e)
    {
        if (e->left())
        {
            getUIObject<DIPWindow>(L"spacProcWnd")->onTrigger();
        }
    };

    auto freqProcBtn = makeUIObject<FlatButton>(L"freqProcBtn");
    freqProcBtn->setParent(this);
    freqProcBtn->setSize({ 100, 32});
    freqProcBtn->setPosition({ 385, 5 });
    freqProcBtn->setRoundRadius(5);
    freqProcBtn->setText(L"频域处理");
    freqProcBtn->setFont(Font(L"默认/正常/14"));

    auto freqProcWnd = makeUIObject<FreqProcWnd>(L"freqProcWnd");
    freqProcBtn->D14_onMouseButtonRelease(p, e)
    {
        if (e->left())
        {
            getUIObject<DIPWindow>(L"freqProcWnd")->onTrigger();
        }
    };

    auto settingsBtn = makeUIObject<FlatButton>(L"settingsBtn");
    settingsBtn->setParent(this);
    settingsBtn->setSize({ 60, 32 });
    settingsBtn->setPosition({ 485, 5 });
    settingsBtn->setRoundRadius(5);
    settingsBtn->setText(L"设置");
    settingsBtn->setFont(Font(L"默认/正常/14"));

    auto settingsWnd = makeUIObject<SettingsWnd>(L"settingsWnd");
    settingsBtn->D14_onMouseButtonRelease(p, e)
    {
        if (e->left())
        {
            getUIObject<DIPWindow>(L"settingsWnd")->onTrigger();
        }
    };

    auto helpBtn = makeUIObject<OutlinedButton>(L"helpBtn");
    helpBtn->setParent(this);
    helpBtn->setSize({ 88, 24 });
    helpBtn->setPosition({ 555, 9 });
    helpBtn->setText(L"帮助 & 关于");
    helpBtn->setFont(Font(L"默认/正常/11"));

    auto helpDialog = makeUIObject<HelpDialog>(L"helpDialog");
    helpBtn->D14_onMouseButtonRelease(p, e)
    {
        if (e->left())
        {
            getUIObject<HelpDialog>(L"helpDialog")->setPopup(true);
        }
    };

    auto imgSizeLbl = makeUIObject<Label>(L"imgSizeLbl");
    GeoInfo geoInfo = {};
    geoInfo.Left.ToRight = 5;
    geoInfo.Right.ToRight = 5;
    geoInfo.Top.ToTop = 5;
    geoInfo.Bottom.ToBottom = 5;
    addElement(imgSizeLbl.get(), geoInfo);
    imgSizeLbl->setText(L"(Width ⨉ Height)");
    imgSizeLbl->setFont(Font(L"默认/正常/12"));
    imgSizeLbl->setHorzAlign(Label::Right);
}
