extern "C"
{
#include "../dip/dip_funcs.h"
}
#include "gui_defs.h"
#include "image_area.h"
#include "spac_proc_wnd.h"

#include <Callback.h>
#include <FilledButton.h>
#include <RawTextBox.h>

using namespace d14uikit;

SpacProcWnd::SpacProcWnd()
{
    setTitle(L"空域处理");
    setFontTitle(Font(L"默认/正常/14"));

    auto clntArea = makeUIObject<Panel>(L"spacProcWndClntArea");
    setContent(clntArea.get());

#define CREATE_TRANS_BOX(t, n, x, y, precision) \
    auto t##TransBox_##n = makeUIObject<RawTextBox>(L#t L"TransBox_" L#n); \
    t##TransBox_##n->setParent(clntArea.get()); \
    t##TransBox_##n->setSize({ 80, 30 }); \
    t##TransBox_##n->setPosition({ x, y }); \
    t##TransBox_##n->setRoundRadius(5); \
    t##TransBox_##n->setTextRect({ 5, 4, 75, 26 }); \
    t##TransBox_##n->placeholder()->setText(L#n); \
    t##TransBox_##n->setFont(Font(L"默认/正常/11")); \
    t##TransBox_##n->placeholder()->setFont(Font(L"默认/正常/11")); \
    t##TransBox_##n->D14_onLoseFocus(p) \
    { \
        auto pTransBox = dynamic_cast<RawTextBox*>(p); \
        if (pTransBox != nullptr) \
        { \
            if (!pTransBox->text().empty()) \
            { \
                pTransBox->setText(formd(pTransBox->text(), precision)); \
            } \
        } \
    }

#define GET_TRANS_BOX(t, p) \
    std::wcstod(getUIObject<RawTextBox>(L#t L"TransBox_" L#p)->text().c_str(), nullptr)


    //---------------------------------------------------------------------------------------------
    // 灰度变换
    //---------------------------------------------------------------------------------------------

    // 线性变换
    auto linTransBtn = makeUIObject<FilledButton>(L"linTransBtn");
    linTransBtn->setParent(clntArea.get());
    linTransBtn->setSize({ 80, 30 });
    linTransBtn->setPosition({ 20, 20 });
    linTransBtn->setRoundRadius(5);
    linTransBtn->setText(L"线性变换");
    linTransBtn->setFont(Font(L"默认/正常/11"));

    linTransBtn->D14_onMouseButtonRelease(p, e)
    {
        if (e->left())
        {
            auto imgArea = getUIObject<ImageArea>(L"imgArea");
            auto src = imgArea->currBmp();
            if (src.data != nullptr)
            {
                auto a = GET_TRANS_BOX(lin, a);
                auto b = GET_TRANS_BOX(lin, b);
                auto c = GET_TRANS_BOX(lin, c);
                auto d = GET_TRANS_BOX(lin, d);

                auto dst = lin_trans(src, a, b, c, d);
                imgArea->addBmp(L"线性变换", dst);
            }
        }
    };
    CREATE_TRANS_BOX(lin, a, 110, 20, 0);
    CREATE_TRANS_BOX(lin, b, 200, 20, 0);
    CREATE_TRANS_BOX(lin, c, 290, 20, 0);
    CREATE_TRANS_BOX(lin, d, 380, 20, 0);

    // 对数变换
    auto logTransBtn = makeUIObject<FilledButton>(L"logTransBtn");
    logTransBtn->setParent(clntArea.get());
    logTransBtn->setSize({ 80, 30 });
    logTransBtn->setPosition({ 20, 70 });
    logTransBtn->setRoundRadius(5);
    logTransBtn->setText(L"对数变换");
    logTransBtn->setFont(Font(L"默认/正常/11"));

    logTransBtn->D14_onMouseButtonRelease(p, e)
    {
        if (e->left())
        {
            auto imgArea = getUIObject<ImageArea>(L"imgArea");
            auto src = imgArea->currBmp();
            if (src.data != nullptr)
            {
                auto c = GET_TRANS_BOX(log, c);

                auto dst = log_trans(src, c);
                imgArea->addBmp(L"对数变换", dst);
            }
        }
    };
    CREATE_TRANS_BOX(log, c, 110, 70, 1);

    // 指数变换
    auto expTransBtn = makeUIObject<FilledButton>(L"expTransBtn");
    expTransBtn->setParent(clntArea.get());
    expTransBtn->setSize({ 80, 30 });
    expTransBtn->setPosition({ 200, 70 });
    expTransBtn->setRoundRadius(5);
    expTransBtn->setText(L"指数变换");
    expTransBtn->setFont(Font(L"默认/正常/11"));

    expTransBtn->D14_onMouseButtonRelease(p, e)
    {
        if (e->left())
        {
            auto imgArea = getUIObject<ImageArea>(L"imgArea");
            auto src = imgArea->currBmp();
            if (src.data != nullptr)
            {
                auto c = GET_TRANS_BOX(exp, c);
                auto gamma = GET_TRANS_BOX(exp, gamma);

                auto dst = exp_trans(src, c, gamma);
                imgArea->addBmp(L"指数变换", dst);
            }
        }
    };
    CREATE_TRANS_BOX(exp, c, 290, 70, 1);
    CREATE_TRANS_BOX(exp, gamma, 380, 70, 2);

    //---------------------------------------------------------------------------------------------
    // 平滑处理
    //---------------------------------------------------------------------------------------------

    auto smoothLbl = makeUIObject<Label>(L"smoothLbl");
    smoothLbl->setParent(clntArea.get());
    smoothLbl->setSize({ 80, 30 });
    smoothLbl->setPosition({ 20, 120 });
    smoothLbl->setText(L"平滑处理");
    smoothLbl->setFont(Font(L"默认/正常/12"));
    smoothLbl->setHorzAlign(Label::HCenter);

#define CREATE_FILT_BOX(t, n, x, y, precision) \
    auto t##FiltBox_##n = makeUIObject<RawTextBox>(L#t L"FiltBox_" L#n); \
    t##FiltBox_##n->setParent(clntArea.get()); \
    t##FiltBox_##n->setSize({ 80, 30 }); \
    t##FiltBox_##n->setPosition({ x, y }); \
    t##FiltBox_##n->setRoundRadius(5); \
    t##FiltBox_##n->setTextRect({ 5, 4, 75, 26 }); \
    t##FiltBox_##n->placeholder()->setText(L#n); \
    t##FiltBox_##n->setFont(Font(L"默认/正常/11")); \
    t##FiltBox_##n->placeholder()->setFont(Font(L"默认/正常/11")); \
    t##FiltBox_##n->D14_onLoseFocus(p) \
    { \
        auto pFiltBox = dynamic_cast<RawTextBox*>(p); \
        if (pFiltBox != nullptr) \
        { \
            if (!pFiltBox->text().empty()) \
            { \
                pFiltBox->setText(formd(pFiltBox->text(), precision)); \
            } \
        } \
    }

#define GET_FILT_BOX(t, p) \
    std::wcstod(getUIObject<RawTextBox>(L#t L"FiltBox_" L#p)->text().c_str(), nullptr)

    // 中值滤波
    auto midFiltBtn = makeUIObject<FilledButton>(L"midFiltBtn");
    midFiltBtn->setParent(clntArea.get());
    midFiltBtn->setSize({ 80, 30 });
    midFiltBtn->setPosition({ 110, 120 });
    midFiltBtn->setRoundRadius(5);
    midFiltBtn->setText(L"中值滤波");
    midFiltBtn->setFont(Font(L"默认/正常/11"));

    midFiltBtn->D14_onMouseButtonRelease(p, e)
    {
        if (e->left())
        {
            auto imgArea = getUIObject<ImageArea>(L"imgArea");
            auto src = imgArea->currBmp();
            if (src.data != nullptr)
            {
                auto c = (int)GET_FILT_BOX(mid, c);
                c = std::min(std::max(c, 1), 8);
                c = std::min(c, (std::min(src.w, src.h) - 1) / 2);

                // auto dst = mid_filt(src, c);
                // imgArea->addBmp(L"中值滤波", dst);
            }
        }
    };
    CREATE_FILT_BOX(mid, c, 200, 120, 0);

    // 均值滤波
    auto meanFiltBtn = makeUIObject<FilledButton>(L"meanFiltBtn");
    meanFiltBtn->setParent(clntArea.get());
    meanFiltBtn->setSize({ 80, 30 });
    meanFiltBtn->setPosition({ 290, 120 });
    meanFiltBtn->setRoundRadius(5);
    meanFiltBtn->setText(L"均值滤波");
    meanFiltBtn->setFont(Font(L"默认/正常/11"));

    meanFiltBtn->D14_onMouseButtonRelease(p, e)
    {
        if (e->left())
        {
            auto imgArea = getUIObject<ImageArea>(L"imgArea");
            auto src = imgArea->currBmp();
            if (src.data != nullptr)
            {
                auto c = (int)GET_FILT_BOX(mean, c);
                c = std::min(std::max(c, 1), 8);
                c = std::min(c, (std::min(src.w, src.h) - 1) / 2);

                // auto dst = mean_filt(src, c);
                // imgArea->addBmp(L"均值滤波", dst);
            }
        }
    };
    CREATE_FILT_BOX(mean, c, 380, 120, 0);

    // 最小值滤波
    auto minFiltBtn = makeUIObject<FilledButton>(L"minFiltBtn");
    minFiltBtn->setParent(clntArea.get());
    minFiltBtn->setSize({ 80, 30 });
    minFiltBtn->setPosition({ 110, 170 });
    minFiltBtn->setRoundRadius(5);
    minFiltBtn->setText(L"最小值滤波");
    minFiltBtn->setFont(Font(L"默认/正常/10"));

    minFiltBtn->D14_onMouseButtonRelease(p, e)
    {
        if (e->left())
        {
            auto imgArea = getUIObject<ImageArea>(L"imgArea");
            auto src = imgArea->currBmp();
            if (src.data != nullptr)
            {
                auto c = (int)GET_FILT_BOX(min, c);
                c = std::min(std::max(c, 1), 8);
                c = std::min(c, (std::min(src.w, src.h) - 1) / 2);

                // auto dst = min_filt(src, c);
                // imgArea->addBmp(L"最小值滤波", dst);
            }
        }
    };
    CREATE_FILT_BOX(min, c, 200, 170, 0);

    // 最大值滤波
    auto maxFiltBtn = makeUIObject<FilledButton>(L"maxFiltBtn");
    maxFiltBtn->setParent(clntArea.get());
    maxFiltBtn->setSize({ 80, 30 });
    maxFiltBtn->setPosition({ 290, 170 });
    maxFiltBtn->setRoundRadius(5);
    maxFiltBtn->setText(L"最大值滤波");
    maxFiltBtn->setFont(Font(L"默认/正常/10"));

    maxFiltBtn->D14_onMouseButtonRelease(p, e)
    {
        if (e->left())
        {
            auto imgArea = getUIObject<ImageArea>(L"imgArea");
            auto src = imgArea->currBmp();
            if (src.data != nullptr)
            {
                auto c = (int)GET_FILT_BOX(max, c);
                c = std::min(std::max(c, 1), 8);
                c = std::min(c, (std::min(src.w, src.h) - 1) / 2);

                // auto dst = max_filt(src, c);
                // imgArea->addBmp(L"最大值滤波", dst);
            }
        }
    };
    CREATE_FILT_BOX(max, c, 380, 170, 0);

    //---------------------------------------------------------------------------------------------
    // 微分算子
    //---------------------------------------------------------------------------------------------

    auto diffOpLbl = makeUIObject<Label>(L"diffOpLbl");
    diffOpLbl->setParent(clntArea.get());
    diffOpLbl->setSize({ 80, 30 });
    diffOpLbl->setPosition({ 20, 220 });
    diffOpLbl->setText(L"微分算子");
    diffOpLbl->setFont(Font(L"默认/正常/12"));
    diffOpLbl->setHorzAlign(Label::HCenter);

#define CREATE_DIFF_BTN(t, x) \
    auto t##Btn = makeUIObject<FilledButton>(L#t L"Btn"); \
    t##Btn->setParent(clntArea.get()); \
    t##Btn->setSize({ 80, 30 }); \
    t##Btn->setPosition({ x, 220 }); \
    t##Btn->setRoundRadius(5); \
    t##Btn->setText(L#t); \
    t##Btn->setFont(Font(L"默认/正常/11")); \
    t##Btn->D14_onMouseButtonRelease(p, e) \
    { \
        if (e->left()) \
        { \
            auto imgArea = getUIObject<ImageArea>(L"imgArea"); \
            auto src = imgArea->currBmp(); \
            if (src.data != nullptr) \
            { \
                auto dst = t(src); \
                imgArea->addBmp(L#t L"算子", dst); \
            } \
        } \
    }

    CREATE_DIFF_BTN(roberts, 110);
    CREATE_DIFF_BTN(prewitt, 200);
    CREATE_DIFF_BTN(sobel, 290);
    CREATE_DIFF_BTN(laplacian, 380);
}
