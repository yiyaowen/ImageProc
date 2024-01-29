#pragma once

#include <stdbool.h>
#include <stdint.h>

//=================================================================================================
// 位图操作
//=================================================================================================

typedef struct tagPixel32
{
    uint8_t R, G, B, A;
}
Pixel32;

typedef struct tagBitmap
{
    Pixel32* data; int w, h;
}
Bitmap;

// 打开 BMP 文件（以左上角为原点，先行后列）
// 作者：文亦尧
Bitmap load_bmp(const wchar_t* filename);

// 保存 BMP 文件（以左上角为原点，先行后列）
// 作者：文亦尧
//
// 返回值：
// 表示 BMP 文件是否保存成功
//
bool save_bmp(const wchar_t* filename, Bitmap bmp);

// 彩色图像转灰度图像（使用位运算加速的经验公式）
// 作者：文亦尧
Bitmap gray(Bitmap src);

// 颜色反相
// 作者：文亦尧
Bitmap invert(Bitmap src);

//=================================================================================================
// 几何变换
//=================================================================================================

// 水平翻转
// 作者：韩奕璇
Bitmap flip_x(Bitmap src);

// 垂直翻转
// 作者：韩奕璇
Bitmap flip_y(Bitmap src);

// 平移
// 作者：韩奕璇
Bitmap move(Bitmap src, int dx, int dy);

// 裁剪
// 作者：韩奕璇
Bitmap crop(Bitmap src, int x, int y, int w, int h);

// 旋转
// 作者：韩奕璇
Bitmap rotate(Bitmap src, double theta);

// 拉伸
// 作者：韩奕璇
Bitmap scale(Bitmap src, double sx, double sy);

// 最邻近插值
// 作者：韩奕璇
Bitmap nearest_interp(Bitmap src, double ratio);

// 双线性插值
// 作者：韩奕璇
Bitmap bilinear_interp(Bitmap src, double ratio);

// 三次内插值
// 作者：韩奕璇
Bitmap cubic_interp(Bitmap src, double ratio);

//=================================================================================================
// 空域变换
//=================================================================================================

// 线性变换
// 作者：宋家兴
Bitmap lin_trans(Bitmap src, double a, double b, double c, double d);

// 对数变换
// 作者：宋家兴
Bitmap log_trans(Bitmap src, double c);

// 指数变换
// 作者：宋家兴
Bitmap exp_trans(Bitmap src, double c, double gamma);

// 直方图均衡化
// 作者：宋子晨
Bitmap hist_eq(Bitmap src);

// 各种平滑处理
// 作者：宋子晨
//
// 如果涉及到卷积核：
// 核大小 = 2 * c + 1
// 例如 c = 1 时，使用 3x3 的卷积核
// 函数签名形如：Bitmap xxx_filt(Bitmap src, int c);
//
// 其它处理：
// 函数签名形如：Bitmap func_name(Bitmap src, 其他参数);

// 例如：中值滤波、最大/小值滤波、均值滤波等（待补充）
Bitmap mid_filt(Bitmap src, int c);
Bitmap min_filt(Bitmap src, int c);
Bitmap max_filt(Bitmap src, int c);
Bitmap mean_filt(Bitmap src, int c);

// 各种锐化处理
// 作者：宋家兴
//
// 如果涉及到卷积核：
// 核大小 = 2 * c + 1
// 例如 c = 1 时，使用 3x3 的卷积核
// 函数签名形如：Bitmap xxx_filt(Bitmap src, int c);
//
// 其它处理：
// 函数签名形如：Bitmap func_name(Bitmap src, 其他参数);

// 例如：各种微分操作，如索贝尔、拉普拉斯算子等（待补充）
Bitmap roberts(Bitmap src);
Bitmap prewitt(Bitmap src);
Bitmap sobel(Bitmap src);
Bitmap laplacian(Bitmap src);

//=================================================================================================
// 频域变换
//=================================================================================================

typedef struct tagComplex
{
    long double real, imag;
}
Complex;

typedef struct tagSpectrum
{
    Complex* data; int w, h;
}
Spectrum;

// 实/复数域转换
// 作者：文亦尧
//
// 注意 comp2real 直接将实数部分截断为 8bit 的灰度值
//
Spectrum real2comp(Bitmap src);
Bitmap comp2real(Spectrum src);

// 频谱中心化
// 作者：文亦尧
//
// 使用方法 fft(fshift(src))
// 注意此方法不会分配新的数组
// 而是直接更改 src 并将其返回
//
Spectrum fshift(Spectrum src);

// 离散傅里叶变换
// 作者：文亦尧
Spectrum dft(Spectrum src);
Spectrum idft(Spectrum src);

// 快速傅里叶变换
// 作者：文亦尧
//
// 注意如果 w 或 h 不是 2 的幂指数，则返回的结果会
// 自动扩充到向上取整的 radix2(w) * radix2(h) 长度
//
Spectrum fft(Spectrum src);
Spectrum ifft(Spectrum src);

// 傅里叶变换提取功率谱
// 作者：文亦尧
Bitmap fft_power(Bitmap src);

// 傅里叶变换提取相位谱
// 作者：文亦尧
Bitmap fft_phase(Bitmap src);

// 可视化离散余弦变换
// 作者：李梦达
Bitmap dct_view(Bitmap src);

// 各种低通滤波
// 作者：李梦达
//
// 如果涉及到卷积核：
// 核大小 = 2 * c + 1
// 例如 c = 1 时，使用 3x3 卷积核
// 函数签名形如：Bitmap xxx_filt(Bitmap src, int c);
//
// 其它处理：
// 函数签名形如：Bitmap func_name(Bitmap src, 其他参数);

// 例如：理想低通滤波等（待补充）
Bitmap ideal_low_pass(Bitmap src, double d0);
Bitmap butterworth_low_pass(Bitmap src, double d0, double n);
Bitmap gaussian_low_pass(Bitmap src, double d0);

// 各种高通滤波
// 作者：李梦达
//
// 如果涉及到卷积核：
// 核大小 = 2 * c + 1
// 例如 c = 1 时，使用 3x3 的卷积核
// 函数签名形如：Bitmap xxx_filt(Bitmap src, int c);
//
// 其它处理：
// 函数签名形如：Bitmap func_name(Bitmap src, 其他参数);

// 例如：理想高通滤波等（待补充）
Bitmap ideal_high_pass(Bitmap src, double d0);
Bitmap butterworth_high_pass(Bitmap src, double d0, double n);
Bitmap gaussian_high_pass(Bitmap src, double d0);
