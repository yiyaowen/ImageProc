extern "C"
{
#include "../dip/dip_funcs.h"
}
#include "gui_defs.h"
#include "histogram_wnd.h"
#include "image_area.h"
#include "step_panel.h"
#include "util_menu.h"

#include <Application.h>
#include <Callback.h>
#include <Cursor.h>
#include <Label.h>
#include <RawTextBox.h>
#include <ScrollView.h>

using namespace d14uikit;

#include <Windows.h>

ImageArea::ImageArea()
{
    GeoInfo geoInfo = {};

    auto hintLbl = makeUIObject<Label>(L"hintLbl");
    geoInfo = {};
    geoInfo.Left.ToLeft = 0;
    geoInfo.Top.ToTop = 0;
    geoInfo.Right.ToRight = 0;
    geoInfo.Bottom.ToBottom = 0;
    addElement(hintLbl.get(), geoInfo);
    hintLbl->setText(L"没有打开的位图文件 (*.bmp)");
    hintLbl->setFont(Font(L"默认/正常/14"));
    hintLbl->setHorzAlign(Label::HCenter);

    auto imgBlk = makeUIObject<Panel>(L"imgBlk");

    imgBlk->D14_onMouseMove(p, e)
    {
        Application::app()->cursor()->setIcon(Cursor::Select);

        auto imgArea = getUIObject<ImageArea>(L"imgArea");
        if (imgArea->colorLock) return;

        auto curpos = e->cursorPoint();

        auto imgBlk = getUIObject(L"imgBlk");
        auto dpi = Application::app()->dpi();
        auto dx = (curpos.x - imgBlk->absX()) * dpi / 96;
        auto dy = (curpos.y - imgBlk->absY()) * dpi / 96;

        auto bmp = imgArea->currScaledBmp();
        if (0 <= dx && dx < bmp.w && 0 <= dy && dy < bmp.h)
        {
            auto color = bmp.data[dx + dy * bmp.w];

            auto colorBlk = getUIObject(L"colorBlk");
            colorBlk->setColor({ color.R, color.G, color.B });

            auto colorLbl = getUIObject<LabelArea>(L"colorLbl");
            colorLbl->setText(L"RGB: (" +
                std::to_wstring(color.R) + L", " +
                std::to_wstring(color.G) + L", " +
                std::to_wstring(color.B) + L")");
        }
    };
    auto utilMenu = makeUIObject<UtilMenu>(L"utilMenu");
    imgBlk->D14_onMouseButton(p, e)
    {
        if (e->leftUp())
        {
            auto lockLbl = getUIObject<Label>(L"lockLbl");
            auto imgArea = getUIObject<ImageArea>(L"imgArea");
            if (imgArea->colorLock = !imgArea->colorLock)
            {
                lockLbl->setText(L"单击释放");
            }
            else lockLbl->setText(L"单击锁定");
        }
        else if (e->rightUp())
        {
            auto utilMenu = getUIObject<PopupMenu>(L"utilMenu");
            utilMenu->setPosition(e->cursorPoint());
            utilMenu->setActivated(true);
        }
    };

    auto imgView = makeUIObject<ScrollView>(L"imgView");
    geoInfo = {};
    geoInfo.Left.ToLeft = 0;
    geoInfo.Top.ToTop = 0;
    geoInfo.Right.ToRight = 0;
    geoInfo.Bottom.ToBottom = 0;
    addElement(imgView.get(), geoInfo);
    imgView->setContent(imgBlk.get());
    imgView->setVisible(false);
    imgView->setEnabled(false);

    imgView->D14_onSize(p, e)
    {
        auto imgBlk = getUIObject(L"imgBlk");
        auto blksz = imgBlk->size();
        imgBlk->setPosition
        ({
            (p->width() - blksz.width) / 2,
            (p->height() - blksz.height) / 2
        });
    };
    imgView->D14_onMouseWheel(p, e)
    {
        if (Event::alt())
        {
            auto imgArea = getUIObject<ImageArea>(L"imgArea");
            imgArea->setRatio(imgArea->ratio() + 0.1 * e->deltaCount());
        }
    };
}

double ImageArea::ratio() const
{
    return m_ratio;
}

void ImageArea::setRatio(double c)
{
    m_ratio = min(max(c, 0.1), 10);

    getUIObject<RawTextBox>(L"ratioBox")
        ->setText(formd(m_ratio * 100));

    updateScaledBmp();
}

void ImageArea::updateScaledBmp()
{
    if (m_stepBmps.empty()) return;
    auto& bmp = m_stepBmps.at(m_currBmpName);

    if (bmp.scaled.data != nullptr)
    {
        free(bmp.scaled.data);
    }
    //---------------------------------------------------------------
    // Replace this with DIP algorithm!
    //---------------------------------------------------------------
    bmp.scaled.w = (int)(m_ratio * bmp.raw.w);
    bmp.scaled.h = (int)(m_ratio * bmp.raw.h);

    size_t size = sizeof(Pixel32) * bmp.scaled.w * bmp.scaled.h;
    bmp.scaled.data = (Pixel32*)malloc(size);
    switch (scaleMode)
    {
    case ScaleMode::Nearest: memset(bmp.scaled.data, 255, size); break;
    case ScaleMode::Bilinear: memset(bmp.scaled.data, 127, size); break;
    case ScaleMode::Cubic: memset(bmp.scaled.data, 0, size); break;
    default: break;
    }
    auto w0 = min(bmp.raw.w, bmp.scaled.w);
    auto h0 = min(bmp.raw.h, bmp.scaled.h);
    for (int i = 0; i < w0; ++i)
    for (int j = 0; j < h0; ++j)
    {
        bmp.scaled.data[i * bmp.scaled.h + j] = bmp.raw.data[i * bmp.raw.h + j];
    }
    //---------------------------------------------------------------
    updateImgView();
}

Bitmap ImageArea::currBmp() const
{
    if (m_stepBmps.empty()) return {};
    return m_stepBmps.at(m_currBmpName).raw;
}

Bitmap ImageArea::currScaledBmp() const
{
    if (m_stepBmps.empty()) return {};
    return m_stepBmps.at(m_currBmpName).scaled;
}

int ImageArea::currBmpCode() const
{
    return m_currBmpCode;
}

const std::wstring& ImageArea::currBmpName() const
{
    return m_currBmpName;
}

void ImageArea::addBmp(const std::wstring& opStr, Bitmap bmp)
{
    auto hintLbl = getUIObject(L"hintLbl");
    hintLbl->setVisible(false);
    hintLbl->setEnabled(false);

    auto imgView = getUIObject(L"imgView");
    imgView->setVisible(true);
    imgView->setEnabled(true);

    if (m_currBmpCode == 0) // init
    {
        m_currBmpName = L"0 = " + opStr;
    }
    else // from the previous bitmap
    {
        auto prev = std::to_wstring(std::wcstol(m_currBmpName.c_str(), nullptr, 10));
        auto curr = std::to_wstring(m_currBmpCode);
        m_currBmpName = curr + L" = " + opStr + L"(" + prev + L")";
    }
    ++m_currBmpCode;
    m_stepBmps[m_currBmpName] = { bmp, {} };

    updateScaledBmp();

    auto stepPanel = getUIObject<StepPanel>(L"stepPanel");
    stepPanel->addStep(m_currBmpName);

    auto histogramWnd = getUIObject<HistogramWnd>(L"histogramWnd");
    histogramWnd->updateBarChart();
}

void ImageArea::selectBmp(const std::wstring& name)
{
    m_currBmpName = name;
    setRatio(1.0);

    auto histogramWnd = getUIObject<HistogramWnd>(L"histogramWnd");
    histogramWnd->updateBarChart();
}

void ImageArea::updateImgView()
{
    auto itor = m_stepBmps.find(m_currBmpName);
    if (itor == m_stepBmps.end()) return;

    auto& bmp = itor->second.scaled;
    m_currImg.setSize({ bmp.w, bmp.h });
    m_currImg.copy({ 0, 0, bmp.w, bmp.h }, (Pixel*)bmp.data);

    auto imgBlk = getUIObject(L"imgBlk");
    auto imgView = getUIObject<ScrollView>(L"imgView");
    imgBlk->setSize(m_currImg.dipSize());
    imgBlk->setImage(&m_currImg);

    auto blksz = imgBlk->size();
    auto viewsz = imgView->size();
    if (blksz.width < viewsz.width &&
        blksz.height < viewsz.height)
    {
        imgBlk->setPosition
        ({
            (viewsz.width - blksz.width) / 2,
            (viewsz.height - blksz.height) / 2
        });
    }
    else // moving imgBlk may cause viewport offset invalid
    {
        imgBlk->setPosition({ 0, 0 });
        imgView->setViewportOffset({ 0, 0 });
    }
}

void ImageArea::onRemoveAllBmp()
{
    m_currBmpCode = 0;
    m_currBmpName = L"";

    auto stepPanel = getUIObject<StepPanel>(L"stepPanel");
    stepPanel->removeAllSteps();

    auto bmpPathLbl = getUIObject<LabelArea>(L"bmpPathLbl");
    bmpPathLbl->setText(L"位图文件路径");

    auto hintLbl = getUIObject(L"hintLbl");
    hintLbl->setVisible(true);
    hintLbl->setEnabled(true);

    auto imgView = getUIObject(L"imgView");
    imgView->setVisible(false);
    imgView->setEnabled(false);

    auto saveBmpCmd = getUIObject(L"saveBmpCmd");
    auto saveBmpToCmd = getUIObject(L"saveBmpToCmd");
    saveBmpCmd->setEnabled(false);
    saveBmpToCmd->setEnabled(false);
}

void ImageArea::removeBmp(const std::wstring& name)
{
    auto itor = m_stepBmps.find(name);
    if (itor == m_stepBmps.end()) return;

    auto stepPanel = getUIObject<StepPanel>(L"stepPanel");
    auto prevName = stepPanel->removeStep(name);

    free(itor->second.raw.data);
    free(itor->second.scaled.data);
    m_stepBmps.erase(itor);

    if (m_stepBmps.empty())
    {
        onRemoveAllBmp();
    }
    else if (name == m_currBmpName)
    {
        selectBmp(prevName);
    }
}

void ImageArea::removeAllBmps()
{
    for (auto& kv : m_stepBmps)
    {
        free(kv.second.raw.data);
        free(kv.second.scaled.data);
    }
    m_stepBmps.clear();

    onRemoveAllBmp();
}
