#include "geo_trans_wnd.h"
#include "gui_defs.h"

#include <Callback.h>
#include <FilledButton.h>
#include <GridLayout.h>
#include <RawTextBox.h>

using namespace d14uikit;

GeoTransWnd::GeoTransWnd()
{
    setTitle(L"几何变换");
    setFontTitle(Font(L"默认/正常/14"));

    auto clntArea = makeUIObject<GridLayout>(L"geoTransWndClntArea");
    setContent(clntArea.get());
    clntArea->setHorzCellCount(5);
    clntArea->setVertCellCount(4);
    clntArea->setHorzMargin(20);
    clntArea->setVertMargin(20);

    GridLayout::GeoInfo geoInfo = {};

#define ADD_ELEMENT(o, xx, w, yy, h) \
    geoInfo = {}; \
    geoInfo.x = { xx, w }; \
    geoInfo.y = { yy, h }; \
    geoInfo.spacing = { 5, 10, 5, 10 }; \
    clntArea->addElement(o.get(), geoInfo)

#define CREATE_BTN(btn, str, xx, w, yy, h) \
    auto btn##Btn = makeUIObject<FilledButton>(L#btn L"Btn"); \
    ADD_ELEMENT(btn##Btn, xx, w, yy, h); \
    btn##Btn->setRoundRadius(5); \
    btn##Btn->setText(L#str); \
    btn##Btn->setFont(Font(L"默认/正常/12"))

#define CREATE_BOX(box, str, xx, w, yy, h, precision) \
    auto box##Box_##str= makeUIObject<RawTextBox>(L#box L"Box" L#str); \
    ADD_ELEMENT(box##Box_##str, xx, w, yy, h); \
    box##Box_##str->setRoundRadius(5); \
    { \
        auto r = box##Box_##str->width() - 10; \
        auto b = box##Box_##str->height() - 5; \
        box##Box_##str->setTextRect({ 10, 5, r, b }); \
    } \
    box##Box_##str->placeholder()->setText(L#str); \
    box##Box_##str->setFont(Font(L"默认/正常/12")); \
    box##Box_##str->placeholder()->setFont(Font(L"默认/正常/12")); \
    box##Box_##str->D14_onLoseFocus(p) \
    { \
        auto pBox = dynamic_cast<RawTextBox*>(p); \
        if (pBox != nullptr) \
        { \
            if (!pBox->text().empty()) \
            { \
                pBox->setText(formd(pBox->text(), precision)); \
            } \
        } \
    }

    CREATE_BTN(move, 平移, 0, 1, 0, 1);
    CREATE_BOX(move, dx, 1, 1, 0, 1, 1);
    CREATE_BOX(move, dy, 2, 1, 0, 1, 1);

    CREATE_BTN(flipx, 水平翻转, 3, 1, 0, 1);
    CREATE_BTN(flipy, 垂直翻转, 4, 1, 0, 1);

    CREATE_BTN(crop, 裁剪, 0, 1, 1, 1);
    CREATE_BOX(crop, x, 1, 1, 1, 1, 0);
    CREATE_BOX(crop, y, 2, 1, 1, 1, 0);
    CREATE_BOX(crop, w, 3, 1, 1, 1, 0);
    CREATE_BOX(crop, h, 4, 1, 1, 1, 0);

    CREATE_BTN(rotate, 旋转, 0, 1, 2, 1);
    CREATE_BOX(rotate, delta, 1, 1, 2, 1, 1);

    CREATE_BTN(scale, 拉伸, 2, 1, 2, 1);
    CREATE_BOX(scale, sx, 3, 1, 2, 1, 1);
    CREATE_BOX(scale, sy, 4, 1, 2, 1, 1);
}
