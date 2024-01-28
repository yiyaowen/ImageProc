#include "gui_defs.h"
#include "image_area.h"
#include "status_bar.h"

#include <Callback.h>
#include <LabelArea.h>
#include <RawTextBox.h>

using namespace d14uikit;

StatusBar::StatusBar()
{
    setOpacity(1.0f);
    setOutlineOpacity(1.0f);

    GeoInfo geoInfo = {};

    auto colorBlk = makeUIObject<Panel>(L"colorBlk");
    colorBlk->setSize({ 24, 24 });
    geoInfo = {};
    geoInfo.keepWidth = true;
    geoInfo.Left.ToLeft = 5;
    geoInfo.keepHeight = true;
    geoInfo.Bottom.ToBottom = 5;
    addElement(colorBlk.get(), geoInfo);
    colorBlk->setOpacity(1.0f);

    auto lockLbl = makeUIObject<Label>(L"lockLbl");
    lockLbl->setSize({ 60, 24 });
    geoInfo = {};
    geoInfo.keepWidth = true;
    geoInfo.Left.ToLeft = 34;
    geoInfo.keepHeight = true;
    geoInfo.Bottom.ToBottom = 5;
    addElement(lockLbl.get(), geoInfo);
    lockLbl->setText(L"单击锁定");
    lockLbl->setFont(Font(L"默认/正常/11"));

    auto colorLbl = makeUIObject<LabelArea>(L"colorLbl");
    colorLbl->setSize({ 145, 24 });
    geoInfo = {};
    geoInfo.keepWidth = true;
    geoInfo.Left.ToLeft = 99;
    geoInfo.keepHeight = true;
    geoInfo.Bottom.ToBottom = 5;
    addElement(colorLbl.get(), geoInfo);
    colorLbl->setText(L"RGB: (0, 0, 0)");
    colorLbl->setFont(Font(L"默认/正常/11"));

    auto ratioLbl = makeUIObject<Label>(L"ratioLbl");
    ratioLbl->setSize({ 120, 24 });
    geoInfo = {};
    geoInfo.keepWidth = true;
    geoInfo.Left.ToLeft = 259;
    geoInfo.keepHeight = true;
    geoInfo.Bottom.ToBottom = 5;
    addElement(ratioLbl.get(), geoInfo);
    ratioLbl->setText(L"Alt+滚轮 缩放 (%)");
    ratioLbl->setFont(Font(L"默认/正常/11"));

    auto ratioBox = makeUIObject<RawTextBox>(L"ratioBox");
    ratioBox->setSize({ 60, 24 });
    geoInfo = {};
    geoInfo.keepWidth = true;
    geoInfo.Left.ToLeft = 384;
    geoInfo.keepHeight = true;
    geoInfo.Bottom.ToBottom = 5;
    addElement(ratioBox.get(), geoInfo);
    ratioBox->setRoundRadius(5);
    ratioBox->setTextRect({ 5, 4, 55, 20 });
    ratioBox->setText(L"100.0");
    ratioBox->setFont(Font(L"默认/正常/11"));

    ratioBox->D14_onLoseFocus(p)
    {
        auto pRatioBox = dynamic_cast<RawTextBox*>(p);
        if (pRatioBox != nullptr)
        {
            auto imgArea = getUIObject<ImageArea>(L"imgArea");
            auto ratioStr = pRatioBox->text().c_str();
            imgArea->setRatio(std::wcstod(ratioStr, nullptr) / 100);
        }
    };

    auto bmpPathLbl = makeUIObject<LabelArea>(L"bmpPathLbl");
    geoInfo = {};
    geoInfo.Left.ToLeft = 459;
    geoInfo.Top.ToTop = 5;
    geoInfo.Right.ToRight = 5;
    geoInfo.Bottom.ToBottom = 5;
    addElement(bmpPathLbl.get(), geoInfo);
    bmpPathLbl->setText(L"位图文件路径");
    bmpPathLbl->setFont(Font(L"默认/正常/11"));
}
