extern "C"
{
#include "../dip/dip_funcs.h"
}
#include "file_menu.h"
#include "gui_defs.h"
#include "image_area.h"

#include <Callback.h>
#include <LabelArea.h>
#include <MenuItem.h>
#include <MenuSeparator.h>

using namespace d14uikit;

FileMenu::FileMenu()
{
    setSize({ 200, 107 });
    setBkgnTriggerPanel(true);

    auto openBmpCmd = makeUIObject<MenuItem>(L"openBmpCmd");
    openBmpCmd->setHeight(32);
    openBmpCmd->setText(m_openBmpStr);
    openBmpCmd->setFont(Font(L"默认/正常/14"));

    auto sep1 = makeUIObject<MenuSeparator>(L"fileMenuSep1");

    auto saveBmpCmd = makeUIObject<MenuItem>(L"saveBmpCmd");
    saveBmpCmd->setHeight(32);
    saveBmpCmd->setText(m_saveBmpStr);
    saveBmpCmd->setFont(Font(L"默认/正常/14"));
    saveBmpCmd->setEnabled(false);

    auto saveBmpToCmd = makeUIObject<MenuItem>(L"saveBmpToCmd");
    saveBmpToCmd->setHeight(32);
    saveBmpToCmd->setText(m_saveBmpToStr);
    saveBmpToCmd->setFont(Font(L"默认/正常/14"));
    saveBmpToCmd->setEnabled(false);

    appendItem
    ({
        openBmpCmd.get(),
        sep1.get(),
        saveBmpCmd.get(),
        saveBmpToCmd.get()
    });

    m_hwnd = FindWindow(
        L"ImageProcessor",
        L"ImageProcessor");

    m_ofn.lStructSize = sizeof(OPENFILENAME);
    m_ofn.hwndOwner = m_hwnd;
    m_ofn.hInstance = NULL;
    m_ofn.lpstrFilter =
        L"位图文件 (*.bmp)\0*.bmp\0";
    m_ofn.lpstrCustomFilter = NULL;
    m_ofn.nMaxCustFilter = 0;
    m_ofn.nFilterIndex = 0;
    m_ofn.lpstrFile = NULL;
    m_ofn.nMaxFile = MAX_PATH;
    m_ofn.lpstrFileTitle = NULL;
    m_ofn.nMaxFileTitle = MAX_PATH;
    m_ofn.lpstrInitialDir = NULL;
    m_ofn.lpstrTitle = NULL;
    m_ofn.Flags = 0;
    m_ofn.nFileExtension = 0;
    m_ofn.lpstrDefExt = L"bmp";
    m_ofn.lCustData = 0;
    m_ofn.lpfnHook = NULL;
    m_ofn.lpTemplateName = NULL;
}

std::wstring_view FileMenu::fileName() const
{
    return m_fileName;
}

std::wstring_view FileMenu::titleName() const
{
    return m_titleName;
}

void FileMenu::onTriggerMenuItem(const std::wstring& text)
{
    auto imgArea = getUIObject<ImageArea>(L"imgArea");
    auto bmpPathLbl = getUIObject<LabelArea>(L"bmpPathLbl");

    if (text == m_openBmpStr)
    {
        m_ofn.hwndOwner = m_hwnd;
        m_ofn.lpstrFile = m_fileName;
        m_ofn.lpstrFileTitle = m_titleName;
        m_ofn.Flags = OFN_HIDEREADONLY | OFN_CREATEPROMPT;

        if (GetOpenFileName(&m_ofn))
        {
            imgArea->removeAllBmps();
            imgArea->setRatio(1.0);
            auto bmp = load_bmp(m_fileName);
            imgArea->addBmp(L"原始图像", bmp);
            bmpPathLbl->setText(m_fileName);

            auto saveBmpCmd = getUIObject(L"saveBmpCmd");
            auto saveBmpToCmd = getUIObject(L"saveBmpToCmd");
            saveBmpCmd->setEnabled(true);
            saveBmpToCmd->setEnabled(true);
        }
    }
    else if (text == m_saveBmpStr)
    {
        if (imgArea->currBmp().data != nullptr)
        {
            save_bmp(m_fileName, imgArea->currBmp());
        }
    }
    else if (text == m_saveBmpToStr)
    {
        if (imgArea->currBmp().data != nullptr)
        {
            m_ofn.hwndOwner = m_hwnd;
            m_ofn.lpstrFile = m_fileName;
            m_ofn.lpstrFileTitle = m_titleName;
            m_ofn.Flags = OFN_OVERWRITEPROMPT;

            if (GetSaveFileName(&m_ofn))
            {
                save_bmp(m_fileName, imgArea->currBmp());
                bmpPathLbl->setText(m_fileName);
            }
        }
    }
}
