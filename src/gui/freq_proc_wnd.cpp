#include "freq_proc_wnd.h"
#include "gui_defs.h"
#include "image_area.h"

#include <Callback.h>
#include <FilledButton.h>
#include <RawTextBox.h>

using namespace d14uikit;

FreqProcWnd::FreqProcWnd()
{
    setTitle(L"频域处理");
    setFontTitle(Font(L"默认/正常/14"));

    auto clntArea = makeUIObject<Panel>(L"freqProcWndClntArea");
    setContent(clntArea.get());

#define CREATE_PASS_BOX(t, n, x, y, precision) \
    auto t##PassBox_##n = makeUIObject<RawTextBox>(L#t L"PassBox_" L#n); \
    t##PassBox_##n->setParent(clntArea.get()); \
    t##PassBox_##n->setSize({ 80, 30 }); \
    t##PassBox_##n->setPosition({ x, y }); \
    t##PassBox_##n->setRoundRadius(5); \
    t##PassBox_##n->setTextRect({ 5, 4, 75, 26 }); \
    t##PassBox_##n->placeholder()->setText(L#n); \
    t##PassBox_##n->setFont(Font(L"默认/正常/11")); \
    t##PassBox_##n->placeholder()->setFont(Font(L"默认/正常/11")); \
    t##PassBox_##n->D14_onLoseFocus(p) \
    { \
        auto pPassBox = dynamic_cast<RawTextBox*>(p); \
        if (pPassBox != nullptr) \
        { \
            if (!pPassBox->text().empty()) \
            { \
                pPassBox->setText(formd(pPassBox->text(), precision)); \
            } \
        } \
    }

#define GET_PASS_BOX(t, p) \
    std::wcstod(getUIObject<RawTextBox>(L#t L"PassBox_" L#p)->text().c_str(), nullptr)

    //---------------------------------------------------------------------------------------------
    // 低通滤波
    //---------------------------------------------------------------------------------------------

    // 理想低通
    auto idealLowPassBtn = makeUIObject<FilledButton>(L"idealLowPassBtn");
    idealLowPassBtn->setParent(clntArea.get());
    idealLowPassBtn->setSize({ 80, 30 });
    idealLowPassBtn->setPosition({ 20, 20 });
    idealLowPassBtn->setRoundRadius(5);
    idealLowPassBtn->setText(L"理想低通");
    idealLowPassBtn->setFont(Font(L"默认/正常/11"));

    idealLowPassBtn->D14_onMouseButtonRelease(p, e)
    {
        if (e->left())
        {
            auto imgArea = getUIObject<ImageArea>(L"imgArea");
            auto src = imgArea->currBmp();
            if (src.data != nullptr)
            {
                auto d0 = GET_PASS_BOX(idealLow, d0);

                auto dst = ideal_low_pass(src, d0);
                imgArea->addBmp(L"理想低通", dst);
            }
        }
    };
    CREATE_PASS_BOX(idealLow, d0, 110, 20, 1);

    // 高斯低通
    auto gaussianLowPassBtn = makeUIObject<FilledButton>(L"gaussianLowPassBtn");
    gaussianLowPassBtn->setParent(clntArea.get());
    gaussianLowPassBtn->setSize({ 80, 30 });
    gaussianLowPassBtn->setPosition({ 200, 20 });
    gaussianLowPassBtn->setRoundRadius(5);
    gaussianLowPassBtn->setText(L"高斯低通");
    gaussianLowPassBtn->setFont(Font(L"默认/正常/11"));

    gaussianLowPassBtn->D14_onMouseButtonRelease(p, e)
    {
        if (e->left())
        {
            auto imgArea = getUIObject<ImageArea>(L"imgArea");
            auto src = imgArea->currBmp();
            if (src.data != nullptr)
            {
                auto d0 = GET_PASS_BOX(gaussianLow, d0);

                auto dst = gaussian_low_pass(src, d0);
                imgArea->addBmp(L"高斯低通", dst);
            }
        }
    };
    CREATE_PASS_BOX(gaussianLow, d0, 290, 20, 1);

    // 巴特沃斯低通
    auto butterworthLowPassBtn = makeUIObject<FilledButton>(L"butterworthLowPassBtn");
    butterworthLowPassBtn->setParent(clntArea.get());
    butterworthLowPassBtn->setSize({ 120, 30 });
    butterworthLowPassBtn->setPosition({ 20, 70 });
    butterworthLowPassBtn->setRoundRadius(5);
    butterworthLowPassBtn->setText(L"巴特沃斯低通");
    butterworthLowPassBtn->setFont(Font(L"默认/正常/11"));

    butterworthLowPassBtn->D14_onMouseButtonRelease(p, e)
    {
        if (e->left())
        {
            auto imgArea = getUIObject<ImageArea>(L"imgArea");
            auto src = imgArea->currBmp();
            if (src.data != nullptr)
            {
                auto d0 = GET_PASS_BOX(butterworthLow, d0);
                auto n = GET_PASS_BOX(butterworthLow, n);

                auto dst = butterworth_low_pass(src, d0, n);
                imgArea->addBmp(L"巴特沃斯低通", dst);
            }
        }
    };
    CREATE_PASS_BOX(butterworthLow, d0, 150, 70, 1);
    CREATE_PASS_BOX(butterworthLow, n, 240, 70, 0);

    //---------------------------------------------------------------------------------------------
    // 高通滤波
    //---------------------------------------------------------------------------------------------

    // 理想高通
    auto idealHighPassBtn = makeUIObject<FilledButton>(L"idealHighPassBtn");
    idealHighPassBtn->setParent(clntArea.get());
    idealHighPassBtn->setSize({ 80, 30 });
    idealHighPassBtn->setPosition({ 20, 120 });
    idealHighPassBtn->setRoundRadius(5);
    idealHighPassBtn->setText(L"理想高通");
    idealHighPassBtn->setFont(Font(L"默认/正常/11"));

    idealHighPassBtn->D14_onMouseButtonRelease(p, e)
    {
        if (e->left())
        {
            auto imgArea = getUIObject<ImageArea>(L"imgArea");
            auto src = imgArea->currBmp();
            if (src.data != nullptr)
            {
                auto d0 = GET_PASS_BOX(idealHigh, d0);

                auto dst = ideal_high_pass(src, d0);
                imgArea->addBmp(L"理想高通", dst);
            }
        }
    };
    CREATE_PASS_BOX(idealHigh, d0, 110, 120, 1);

    // 高斯高通
    auto gaussianHighPassBtn = makeUIObject<FilledButton>(L"gaussianHighPassBtn");
    gaussianHighPassBtn->setParent(clntArea.get());
    gaussianHighPassBtn->setSize({ 80, 30 });
    gaussianHighPassBtn->setPosition({ 200, 120 });
    gaussianHighPassBtn->setRoundRadius(5);
    gaussianHighPassBtn->setText(L"高斯高通");
    gaussianHighPassBtn->setFont(Font(L"默认/正常/11"));

    gaussianHighPassBtn->D14_onMouseButtonRelease(p, e)
    {
        if (e->left())
        {
            auto imgArea = getUIObject<ImageArea>(L"imgArea");
            auto src = imgArea->currBmp();
            if (src.data != nullptr)
            {
                auto d0 = GET_PASS_BOX(gaussianHigh, d0);

                auto dst = gaussian_high_pass(src, d0);
                imgArea->addBmp(L"高斯高通", dst);
            }
        }
    };
    CREATE_PASS_BOX(gaussianHigh, d0, 290, 120, 1);

    // 巴特沃斯高通
    auto butterworthHighPassBtn = makeUIObject<FilledButton>(L"butterworthHighPassBtn");
    butterworthHighPassBtn->setParent(clntArea.get());
    butterworthHighPassBtn->setSize({ 120, 30 });
    butterworthHighPassBtn->setPosition({ 20, 170 });
    butterworthHighPassBtn->setRoundRadius(5);
    butterworthHighPassBtn->setText(L"巴特沃斯高通");
    butterworthHighPassBtn->setFont(Font(L"默认/正常/11"));

    butterworthHighPassBtn->D14_onMouseButtonRelease(p, e)
    {
        if (e->left())
        {
            auto imgArea = getUIObject<ImageArea>(L"imgArea");
            auto src = imgArea->currBmp();
            if (src.data != nullptr)
            {
                auto d0 = GET_PASS_BOX(butterworthHigh, d0);
                auto n = GET_PASS_BOX(butterworthHigh, n);

                auto dst = butterworth_high_pass(src, d0, n);
                imgArea->addBmp(L"巴特沃斯高通", dst);
            }
        }
    };
    CREATE_PASS_BOX(butterworthHigh, d0, 150, 170, 1);
    CREATE_PASS_BOX(butterworthHigh, n, 240, 170, 0);

    //---------------------------------------------------------------------------------------------
    // FFT 和 DCT 变换
    //---------------------------------------------------------------------------------------------

    auto fftPowerBtn = makeUIObject<FilledButton>(L"fftPowerBtn");
    fftPowerBtn->setParent(clntArea.get());
    fftPowerBtn->setSize({ 100, 30 });
    fftPowerBtn->setPosition({ 20, 220 });
    fftPowerBtn->setRoundRadius(5);
    fftPowerBtn->setText(L"FFT Power");
    fftPowerBtn->setFont(Font(L"默认/正常/11"));

    fftPowerBtn->D14_onMouseButtonRelease(p, e)
    {
        if (e->left())
        {
            auto imgArea = getUIObject<ImageArea>(L"imgArea");
            auto src = imgArea->currBmp();
            if (src.data != nullptr)
            {
                auto dst = fft_power(src);
                imgArea->addBmp(L"FFT Power", dst);
            }
        }
    };

    auto fftPhaseBtn = makeUIObject<FilledButton>(L"fftPhaseBtn");
    fftPhaseBtn->setParent(clntArea.get());
    fftPhaseBtn->setSize({ 100, 30 });
    fftPhaseBtn->setPosition({ 130, 220 });
    fftPhaseBtn->setRoundRadius(5);
    fftPhaseBtn->setText(L"FFT Phase");
    fftPhaseBtn->setFont(Font(L"默认/正常/11"));

    fftPhaseBtn->D14_onMouseButtonRelease(p, e)
    {
        if (e->left())
        {
            auto imgArea = getUIObject<ImageArea>(L"imgArea");
            auto src = imgArea->currBmp();
            if (src.data != nullptr)
            {
                auto dst = fft_phase(src);
                imgArea->addBmp(L"FFT Phase", dst);
            }
        }
    };

    auto dctBtn = makeUIObject<FilledButton>(L"dctBtn");
    dctBtn->setParent(clntArea.get());
    dctBtn->setSize({ 130, 30 });
    dctBtn->setPosition({ 240, 220 });
    dctBtn->setRoundRadius(5);
    dctBtn->setText(L"DCT");
    dctBtn->setFont(Font(L"默认/正常/11"));

    dctBtn->D14_onMouseButtonRelease(p, e)
    {
        if (e->left())
        {
            auto imgArea = getUIObject<ImageArea>(L"imgArea");
            auto src = imgArea->currBmp();
            if (src.data != nullptr)
            {
                auto dst = dct_view(src);
                imgArea->addBmp(L"DCT", dst);
            }
        }
    };
}
