#pragma once

#include <PopupMenu.h>

#include <Windows.h>
// must after Windows.h
#include <commdlg.h>

class FileMenu :
public d14uikit::PopupMenu
{
public:
    FileMenu();

private:
    const std::wstring
    m_openBmpStr = L"打开 (*.bmp)",
    m_saveBmpStr = L"保存 (*.bmp)",
    m_saveBmpToStr = L"另存为...";

    HWND m_hwnd = {};
    OPENFILENAME m_ofn = {};
    WCHAR m_fileName[MAX_PATH] = L"\0";
    WCHAR m_titleName[MAX_PATH] = L"\0";

public:
    std::wstring_view fileName() const;
    std::wstring_view titleName() const;

protected:
    void onTriggerMenuItem(const std::wstring& text) override;
};
