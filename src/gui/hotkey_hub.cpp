#include "file_menu.h"
#include "gui_defs.h"
#include "hotkey_hub.h"
#include "image_area.h"

using namespace d14uikit;

HotkeyHub::HotkeyHub()
{
    setPinned(true);
}

void HotkeyHub::onKeyboard(KeyboardEvent* e)
{
    if (e->pressed())
    {
        switch (e->vkey())
        {
        case 'O': // Open Bmp
        {
            if (e->ctrl())
            {
                auto fileMenu = getUIObject<FileMenu>(L"fileMenu");
                fileMenu->triggerOpenBmpCmd();
            }
            break;
        }
        case 'S': // Save Bmp
        {
            if (e->ctrl())
            {
                auto fileMenu = getUIObject<FileMenu>(L"fileMenu");
                if (e->shift())
                {
                    fileMenu->triggerSaveBmpToCmd();
                }
                else fileMenu->triggerSaveBmpCmd();
            }
            break;
        }
        case 'Z': // Undo Step
        {
            if (e->ctrl())
            {
                auto imgArea = getUIObject<ImageArea>(L"imgArea");
                imgArea->removeBmp(imgArea->currBmpName());
            }
            break;
        }
        default: break;
        }
    }
}
