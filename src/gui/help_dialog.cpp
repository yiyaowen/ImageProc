#include "gui_defs.h"
#include "help_dialog.h"

#include <Application.h>
#include <Callback.h>
#include <CheckBox.h>
#include <ConstraintLayout.h>
#include <FilledButton.h>
#include <LabelArea.h>
#include <ScrollView.h>

using namespace d14uikit;

#ifndef UNICODE
#define UNICODE
#endif
#include <Windows.h>

HelpDialog::HelpDialog()
{
    setGlobal(true);
    setDraggable(false);
    setResizable(false);
    setCaptionHeight(0);
    setDecoBarHeight(0);
    setTitle(L"");
    setMinimizeButton(false);
    setMaximizeButton(false);
    setCloseButton(false);

    m_bkgn = std::make_shared<Panel>();
    m_bkgn->setGlobal(true);

    setPopup(getRegVal());

    auto clntArea = makeUIObject<Panel>(L"helpDialogClntArea");
    setContent(clntArea.get());

    auto helpLbl = makeUIObject<Label>(L"helpLbl");
    helpLbl->setParent(clntArea.get());
    helpLbl->setText(L"帮助 & 关于");
    helpLbl->setSize({ 0, 40 });
    helpLbl->setPosition({ 20, 20 });
    helpLbl->setFont(Font(L"默认/正常/16"));

    auto helpView = makeUIObject<ScrollView>(L"helpView");
    helpView->setParent(clntArea.get());
    helpView->setSize({ 560, 260 });
    helpView->setPosition({ 20, 70 });

    auto helpArea = makeUIObject<ConstraintLayout>(L"helpArea");
    helpArea->setSize({ 560, 600 });
    helpArea->setOpacity(1.0f);
    helpView->setContent(helpArea.get());

    auto helpText = makeUIObject<LabelArea>(L"helpText");
    ConstraintLayout::GeoInfo geoInfo = {};
    geoInfo.Left.ToLeft = 10;
    geoInfo.Top.ToTop = 10;
    geoInfo.Right.ToRight = 10;
    geoInfo.Bottom.ToBottom = 0;
    helpArea->addElement(helpText.get(), geoInfo);
    helpText->setText
    (
        L"该程序为「UCAS 数字图像处理与分析-2023」的课程设计（一）：\n\n" \
        L"一、基本功能\n\n" \
        L"\t◆ 支持导入 24/32bit（RGB-888）、16bit（RGB-555）以及各种调色板格式" \
        L"的 Bitmap 文件；支持导出 32bit（RGB-888）格式的 Bitmap 文件。\n\n" \
        L"\t◆ 在工作区使用 Alt+滚轮 对图像进行缩放，使用 (Shift+)滚轮 进行移动；" \
        L"在工作区右键弹出菜单中提取灰度、颜色反相、更改图像缩放的插值模式。\n\n" \
        L"二、处理算法\n\n" \
        L"\t◆ 几何变换：翻转、平移、裁剪、旋转、拉伸\n\n" \
        L"\t◆ 直方图操作：直方图显示、直方图均衡化\n\n" \
        L"\t◆ 空域处理：线性/对数/指数变换、各种平滑/锐化处理\n\n" \
        L"\t◆ 频域处理：FFT、DCT、各种低通/高通滤波\n\n" \
        L"\n" \
        L"本程序中的各种算法均由小组成员自行设计，感谢大家付出的努力！\n\n" \
        L"Copyleft (cl) 2024 韩奕璇、李梦达、宋家兴、宋子晨、文亦尧\n\n" \
        L"Source Available: https://github.com/yiyaowen/ImageProcessor"
    );
    helpText->setFont(Font(L"默认/正常/12"));
    helpText->setVertAlign(Label::Top);
    helpText->setWordWrapping(Label::Wrap);

    auto ignoreBox = makeUIObject<CheckBox>(L"ignoreBox");
    ignoreBox->setParent(clntArea.get());
    ignoreBox->setPosition({ 300, 348 });

    auto ignoreLbl = makeUIObject<Label>(L"ignoreLbl");
    ignoreLbl->setParent(clntArea.get());
    ignoreLbl->setSize({ 0, 40 });
    ignoreLbl->setPosition({ 334, 340 });
    ignoreLbl->setText(L"打开时自动显示");
    ignoreLbl->setFont(Font(L"默认/正常/14"));

    initRegVal();
    if (getRegVal())
    {
        // do this before setting the callback
        // to avoid messing up the registry value
        ignoreBox->setState(CheckBox::Checked);
    }
    ignoreBox->D14_onStateChange(CheckBox, obj, state)
    {
        setRegVal(state == CheckBox::Checked);
    };

    auto igotitBtn = makeUIObject<FilledButton>(L"igotitBtn");
    igotitBtn->setParent(clntArea.get());
    igotitBtn->setSize({ 80, 40 });
    igotitBtn->setPosition({ 500, 340 });
    igotitBtn->setRoundRadius(5);
    igotitBtn->setText(L"好的");
    igotitBtn->setFont(Font(L"默认/正常/14"));

    igotitBtn->D14_onMouseButtonRelease(p, e, this)
    {
        if (e->left())
        {
            setPopup(!p->parent()->visible());
        }
    };
}

void HelpDialog::initRegVal()
{
    HKEY hkey;
    DWORD state;

    if (SUCCEEDED(RegCreateKeyEx(
        HKEY_CURRENT_USER, L"Software\\DreamersGather\\ImageProcessor", 0,
        nullptr, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, nullptr, &hkey, &state)))
    {
        if (state == REG_CREATED_NEW_KEY)
        {
            DWORD value = 1;
            if (FAILED(RegSetValueEx(
                hkey, L"ShowHelpDialogOnOpening", 0, REG_DWORD, (BYTE*)&value, sizeof(value))))
            {
                handleError();
            }
        }
    }
    else handleError();
}

bool HelpDialog::getRegVal()
{
    DWORD value = 1, valueSize = sizeof(value);
    if (FAILED(RegGetValue(HKEY_CURRENT_USER,
        L"Software\\DreamersGather\\ImageProcessor", L"ShowHelpDialogOnOpening",
        RRF_RT_DWORD, nullptr, &value, &valueSize)))
    {
        handleError();
    }
    return value == 1;
}

void HelpDialog::setRegVal(bool value)
{
    HKEY hkey;
    if (FAILED(RegOpenKeyEx(HKEY_CURRENT_USER,
        L"Software\\DreamersGather\\ImageProcessor", 0, KEY_ALL_ACCESS, &hkey)))
    {
        handleError();
    }
    DWORD data = value ? 1 : 0;
    // NOTE the 2nd parameter should be the name instead of the full path!
    if (FAILED(RegSetValueEx(hkey,
        L"ShowHelpDialogOnOpening", 0, REG_DWORD, (BYTE*)&data, sizeof(data))))
    {
        handleError();
    }
}

void HelpDialog::handleError()
{
    auto hwnd = FindWindow(L"ImageProcessor", L"ImageProcessor");
    MessageBox(hwnd, L"处理注册表项时发生异常", L"错误", MB_OK | MB_ICONERROR);
    Application::app()->exit();
}

void HelpDialog::setPopup(bool value)
{
    auto app = Application::app();
    if (value)
    {
        setSize({ 600, 400 });
        auto w = app->width();
        auto h = app->height();
        setPosition
        ({
            (w - width()) / 2,
            (h - height()) / 2
        });
        m_bkgn->setSize({ w, h });
        m_bkgn->setPosition({ 0, 0 });
        m_bkgn->moveTopmost();
        moveAbove(m_bkgn.get());
    }
    app->setResizable(!value);
    setVisible(value);
    setEnabled(value);
    m_bkgn->setVisible(value);
    m_bkgn->setEnabled(value);
}
