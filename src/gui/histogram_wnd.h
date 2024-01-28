#pragma once

#include "dip_window.h"

#include <array>

class HistogramWnd :
public DIPWindow
{
    // Use 256 Panels to draw the bars of the gray histogram.
    // It's crazy indeed, but currently (with D14UIKit v0.8)
    // there're no other better ideas for drawing a histogram.

    std::array<int, 256> m_counts = {};
    std::array<std::shared_ptr<d14uikit::Panel>, 256> m_bars;

public:
    HistogramWnd();

    void updateBarChart();

protected:
    void onChangeTheme(const std::wstring& name) override;
};
