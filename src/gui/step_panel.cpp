#include "image_area.h"
#include "step_panel.h"

#include <Callback.h>
#include <ConstraintLayout.h>
#include <FilledButton.h>
#include <Label.h>
#include <ListView.h>

using namespace d14uikit;

StepPanel::StepPanel()
{
    setDraggable(false);
    setResizable(false);
    setResizable(true, Left);
    setDecoBarHeight(0);
    setMinimizeButton(false);
    setMaximizeButton(false);
    setCloseButton(false);
    setTitle(L"历史记录");
    setFontTitle(Font(L"默认/正常/14"));

    auto lay = makeUIObject<ConstraintLayout>(L"stepLay");
    setContent(lay.get());

    ConstraintLayout::GeoInfo geoInfo = {};

    auto undoBtn = makeUIObject<FilledButton>(L"undoBtn");
    geoInfo = {};
    geoInfo.Left.ToLeft = 20;
    geoInfo.Right.ToRight = 20;
    geoInfo.Top.ToTop = 20;
    geoInfo.Bottom.ToTop = 60;
    lay->addElement(undoBtn.get(), geoInfo);
    undoBtn->setRoundRadius(5);
    undoBtn->setText(L"撤销当前操作 (Ctrl+Z)");
    undoBtn->setFont(Font(L"默认/正常/14"));

    undoBtn->D14_onMouseButtonRelease(p, e)
    {
        if (e->left())
        {
            auto imgArea = getUIObject<ImageArea>(L"imgArea");
            imgArea->removeBmp(imgArea->currBmpName());
        }
    };

    auto stepList = makeUIObject<ListView>(L"stepList");
    geoInfo = {};
    geoInfo.Left.ToLeft = 20;
    geoInfo.Right.ToRight = 20;
    geoInfo.Top.ToTop = 80;
    geoInfo.Bottom.ToBottom = 20;
    lay->addElement(stepList.get(), geoInfo);
    stepList->setSelectMode(ListView::Single);

    stepList->D14_onSelectChange(ListView, obj, selected, this)
    {
        if (selected.empty()) return;
        getUIObject<ImageArea>(L"imgArea")->
            selectBmp(getStepName(*selected.begin()));
    };
}

void StepPanel::addStep(const std::wstring& name)
{
    auto item = std::make_shared<ListViewItem>(name);
    item->setHeight(40);
    item->setText(name);
    item->setFont(Font(L"默认/正常/14"));

    auto stepList = getUIObject<ListView>(L"stepList");
    stepList->appendItem({ item.get() });
    m_steps[name] = item;
}

std::wstring StepPanel::removeStep(const std::wstring& name)
{
    auto step = getStep(name);
    if (step.itor == m_steps.end()) return L"";

    std::wstring ret = {};
    if (m_steps.size() != 1)
    {
        if (step.index == 0)
        {
            ret = std::next(step.itor)->first;
        }
        else ret = std::prev(step.itor)->first;
    }
    auto stepList = getUIObject<ListView>(L"stepList");
    stepList->removeItem(step.index);
    m_steps.erase(step.itor);

    return ret;
}

void StepPanel::removeAllSteps()
{
    m_steps.clear();

    auto stepList = getUIObject<ListView>(L"stepList");
    stepList->clearAllItems();
}

const std::wstring& StepPanel::getStepName(int index)
{
    return std::next(m_steps.begin(), index)->first;
}

StepPanel::IndexItor StepPanel::getStep(const std::wstring& name)
{
    int index = 0;
    auto itor = m_steps.begin();
    while (itor != m_steps.end())
    {
        if (itor->first == name) break;
        else { ++itor; ++index; }
    }
    return { index, itor };
}

void StepPanel::onSize(SizeEvent* e)
{
    auto wkspace = getUIObject(L"wkspace");
    auto imgArea = getUIObject(L"imgArea");
    // leave extra 5 dips for the sizing frame
    imgArea->setWidth(
        wkspace->width() - e->size().width - 5);
}
