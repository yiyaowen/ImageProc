# ImageProcessor 图像处理工具

## 简介

ImageProcessor 是一个基于 [D14UIKit](https://github.com/DreamersGather/D14UIKit) 开发的图像查看、处理工具。该项目是 [BitmapViewer](https://github.com/yiyaowen/BitmapViewer) 的升级。相较于 BitmapViewer 的 UI 直接使用原生的 Win32 API 实现，升级后的 ImageProcessor 基于轻量、高性能的 D14UIKit 库进行开发，提供了更好的 UI 交互体验。

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
