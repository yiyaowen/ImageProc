#include "gui_defs.h"
#include "histogram_wnd.h"
#include "image_area.h"

#include <Application.h>
#include <FilledButton.h>
#include <Label.h>

using namespace d14uikit;

HistogramWnd::HistogramWnd()
{
    setTitle(L"直方图操作");
    setFontTitle(Font(L"默认/正常/14"));

    auto clntArea = makeUIObject<Panel>(L"histogramWndClntArea");
    setContent(clntArea.get());

    auto barChart = makeUIObject<Panel>(L"barChart");
    barChart->setParent(clntArea.get());
    barChart->setSize({ 256, 200 });
    barChart->setPosition({ 22, 20 });
    barChart->setOutlineColor(Application::app()->themeColor());
    barChart->setOutlineOpacity(0.5f);

    for (int i = 0; i < 256; ++i)
    {
        auto& bar = (m_bars[i] = std::make_shared<Panel>());
        bar->setParent(barChart.get());
        bar->setSize({ 1, 0 });
        bar->setPosition({ i, 200 });
        bar->setColor(Application::app()->themeColor());
        bar->setOpacity(1.0f);
    }
    auto histEqBtn = makeUIObject<FilledButton>(L"histEqBtn");
    histEqBtn->setParent(clntArea.get());
    histEqBtn->setSize({ 180, 40 });
    histEqBtn->setPosition({ 298, 22 });
    histEqBtn->setRoundRadius(5);
    histEqBtn->setText(L"直方图均衡化");
    histEqBtn->setFont(Font(L"默认/正常/14"));

#define CREATE_HIST_LBL(tick, halign) \
    auto histTickLbl_##tick = makeUIObject<Label>(L"histTickLbl_" L#tick); \
    histTickLbl_##tick->setParent(clntArea.get()); \
    histTickLbl_##tick->setSize({ 0, 24 }); \
    histTickLbl_##tick->setPosition({ 22 + tick, 220 }); \
    histTickLbl_##tick->setText(L#tick); \
    histTickLbl_##tick->setFont(Font(L"默认/正常/12")); \
    histTickLbl_##tick->setHorzAlign(Label::halign); \
    histTickLbl_##tick->setVertAlign(Label::Bottom); \
    histTickLbl_##tick->setVertHardAlign(true)

    CREATE_HIST_LBL(0, Left);
    CREATE_HIST_LBL(255, Right);
}

void HistogramWnd::updateBarChart()
{
    m_counts = {};

    auto imgArea = getUIObject<ImageArea>(L"imgArea");
    auto bmp = imgArea->currBmp();
    if (bmp.data == nullptr)
    {
        for (auto& bar : m_bars)
        {
            bar->setHeight(0);
            bar->setY(220);
        }
    }
    else // resize 256 Panel-bars
    {
        int pxnum = bmp.w * bmp.h;
        for (int i = 0; i < pxnum; ++i)
        {
            ++m_counts[bmp.data[i].R];
        }
        int maxnum = 0;
        for (int i = 0; i < 256; ++i)
        {
            maxnum = std::max(
                m_counts[i], maxnum);
        }
        for (int i = 0; i < 256; ++i)
        {
            auto& cnt = m_counts[i];
            auto h = 200 * cnt / maxnum;

            auto& bar = m_bars[i];
            bar->setHeight(h);
            bar->setY(200-h);
        }
    }
}

void HistogramWnd::onChangeTheme(const std::wstring& name)
{
    auto barChart = getUIObject(L"barChart");
    barChart->setOutlineColor(Application::app()->themeColor());

    for (auto& bar : m_bars)
    {
        bar->setColor(Application::app()->themeColor());
    }
}
