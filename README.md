# ImageProcessor

## 简介

ImageProcessor 是一个基于 [D14UIKit](https://github.com/DreamersGather/D14UIKit) 开发的数字图像处理工具，支持导入 24/32bit（RGB-888）、16bit（RGB-555）以及各种调色板格式的 Bitmap 文件；支持导出 32bit（RGB-888）格式的 Bitmap 文件。该程序初步支持的处理算法如下：

* 几何变换：翻转、平移、裁剪、旋转、拉伸
* 直方图操作：直方图显示、直方图均衡化
* 空域处理：线性/对数/指数变换、各种平滑/锐化处理
* 频域处理：FFT、DCT、各种低通/高通滤波

其中 `src/gui` 中存放有 D14UIKit 库相关的 GUI 设计代码，`src/dip` 中存放有各种数字图像处理算法的具体实现。

## 构建

从 [D14UIKit-release](https://github.com/DreamersGather/D14UIKit/releases) 页面下载 `d14uikit_cpp_v0_8.zip`，再将其解压至 `d14uikit` 文件夹（需要你自己创建）中：

* d14uikit
  * demo
  * include
  * lib
    * debug / D14UIKit.dll
    * release / D14UIKit.dll
    * D14UIKit.lib
* src
* 其它文件

然后使用 CMake 运行常规的构建流程即可。

例如在 Windows 上使用 MSVC 工具链如下：

* 首先打开一个 VS 命令窗口；
* 然后使用 CMake + MSBuild 进行构建：

```bat
> cd Path/to/ImageProcessor
> mkdir build && cd build
> cmake ..
> msbuild ImageProcessor.sln
```
