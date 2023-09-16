#include <Windows.h>

#include <Application.h>
#include <MainWindow.h>

using namespace d14uikit;

int main() {
#ifdef _DEBUG
    SetDllDirectory(D14LIB_PATH L"debug");
#else
    SetDllDirectory(D14LIB_PATH L"release");
#endif
    Application app(L"Image Processor");
    MainWindow mwnd(L"Image Processor");
    return app.run();
}
