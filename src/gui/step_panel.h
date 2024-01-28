#pragma once

#include "gui_defs.h"

#include <map>
#include <memory>

#include <ListViewItem.h>
#include <Window.h>

class StepPanel :
public d14uikit::Window
{
    using StepMap =
    std::map
    <
        std::wstring,
        std::shared_ptr<d14uikit::ListViewItem>,
        NameCodeOrder
    >;
    StepMap m_steps = {};

    struct IndexItor
    {
        int index = {};
        StepMap::iterator itor = {};
    };
    IndexItor getStep(const std::wstring& name);

public:
    StepPanel();

    void addStep(const std::wstring& name);
    // returns the previous available name
    std::wstring removeStep(const std::wstring& name);
    void removeAllSteps();

    const std::wstring& getStepName(int index);

protected:
    void onSize(d14uikit::SizeEvent* e) override;
};
