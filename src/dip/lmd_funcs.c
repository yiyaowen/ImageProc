#include "dip_funcs.h"

#include <math.h>
#include <stdio.h>

// In case that MinGW doesn't define this for us.
#ifndef UNICODE
#define UNICODE
#endif
#include <Windows.h>

// Disable sign match warnings.
#pragma warning(disable : 4018)
// Disable security func warnings.
#pragma warning(disable : 4996)
// Disable buffer read/write warnings.
#pragma warning(disable : 6385)
#pragma warning(disable : 6386)

#define myAssert(Expression) \
    if ((Expression) == NULL) \
    { wprintf(L"发生了一个未知错误\n"); exit(1); }

// 用于存储离散余弦系数
typedef struct dctBitmap
{
    double* data; int w, h;
}
dBitmap;

// 离散余弦变换
// 作者：李梦达
#define CN 8 // 分成8*8子图依次操作
#define PI 3.1415926
void dct2(double *, double *, double *, double *);       // dct正变换
void idct2(double *, double *, double *, double *);      // dct反变换
void printM(double *, int, char *);                      // 打印中间矩阵
void mulMatrix(double *P, double *Q, double *resM, int); // 矩阵乘法
double *TMatrix(double *, int);                          // 转置矩阵
dBitmap dct(Bitmap src);                                 // DCT变换
Bitmap view(dBitmap src);                                // DCT可视化
Bitmap idct(dBitmap src);                                // DCT反变换

// 各种低通滤波
// 作者：李梦达
// 顺序：理想低通滤波,巴特沃斯低通滤波，高斯低通滤波
Spectrum Ideal_low_pass(Spectrum src, double d0);
Spectrum Butterworth_low_pass(Spectrum src, double d0, double n);
Spectrum Gaussian_low_pass(Spectrum src, double d0);

// 各种高通滤波
// 作者：李梦达
// 顺序：理想高通滤波，巴特沃斯高通滤波，高斯高通滤波
Spectrum Ideal_high_pass(Spectrum src, double d0);
Spectrum Butterworth_high_pass(Spectrum src, double d0, double n);
Spectrum Gaussian_high_pass(Spectrum src, double d0);

double ldistance(int x, int y, int mx, int my)
{
    int dis = (x - mx) * (x - mx) + (y - my) * (y - my);
    double ans = sqrt(dis);
    return ans;
}
// 理想低通
Spectrum Ideal_low_pass(Spectrum src, double d0)
{
    int w = src.w;
    int h = src.h;
    Spectrum dst;
    dst.w = w;
    dst.h = h;
    dst.data = (Complex *)malloc(sizeof(Complex) * src.w * src.h);
    // for (int x = 0; x < src.w; ++x)
    // {
    //     for (int y = 0; y < src.h; ++y)
    //     {
    // 修改 by 文亦尧
    for (int y = 0; y < src.h; ++y)
    {
        for (int x = 0; x < src.w; ++x)
        {
            int idx = y * src.w + x;
            int mx = w / 2; // 256
            int my = h / 2; // 256
            if (ldistance(x, y, mx, my) <= d0)
            {
                dst.data[idx].real = src.data[idx].real;
                dst.data[idx].imag = src.data[idx].imag;
            }
            else
            {
                dst.data[idx].real = 0;
                // dst.data[idx].real = 0;
                // 修改 by 文亦尧
                // 可能编译器自动给 dst 填充 0 了
                // 所以原代码有时也可以正常运行
                dst.data[idx].imag = 0;
            }
        }
    }
    return dst;
}
// 巴特沃斯低通
Spectrum Butterworth_low_pass(Spectrum src, double d0, double n)
{
    int w = src.w;
    int h = src.h;
    Spectrum dst;
    dst.w = w;
    dst.h = h;
    dst.data = (Complex *)malloc(sizeof(Complex) * src.w * src.h);
    // for (int x = 0; x < src.w; ++x)
    // {
    //     for (int y = 0; y < src.h; ++y)
    //     {
    // 修改 by 文亦尧
    for (int y = 0; y < src.h; ++y)
    {
        for (int x = 0; x < src.w; ++x)
        {
            int idx = y * src.w + x;
            int mx = w / 2; // 256
            int my = h / 2; // 256
            double dis = ldistance(x, y, mx, my);
            double ba = 1 / (1 + pow(dis / d0, 2 * n));
            dst.data[idx].real = src.data[idx].real * ba;
            dst.data[idx].imag = src.data[idx].imag * ba;
        }
    }
    return dst;
}
// 高斯低通
Spectrum Gaussian_low_pass(Spectrum src, double d0)
{
    int w = src.w;
    int h = src.h;
    Spectrum dst;
    dst.w = w;
    dst.h = h;
    dst.data = (Complex *)malloc(sizeof(Complex) * src.w * src.h);
    // for (int x = 0; x < src.w; ++x)
    // {
    //     for (int y = 0; y < src.h; ++y)
    //     {
    // 修改 by 文亦尧
    for (int y = 0; y < src.h; ++y)
    {
        for (int x = 0; x < src.w; ++x)
        {
            int idx = y * src.w + x;
            int mx = w / 2; // 256
            int my = h / 2; // 256
            double dis = ldistance(x, y, mx, my);
            double gas = -1 * pow(dis, 2) / pow(d0, 2);
            double ga = exp(gas);
            dst.data[idx].real = src.data[idx].real * ga;
            dst.data[idx].imag = src.data[idx].imag * ga;
        }
    }
    return dst;
}
// 理想高通
Spectrum Ideal_high_pass(Spectrum src, double d0)
{
    int w = src.w;
    int h = src.h;
    Spectrum dst;
    dst.w = w;
    dst.h = h;
    dst.data = (Complex *)malloc(sizeof(Complex) * src.w * src.h);
    // for (int x = 0; x < src.w; ++x)
    // {
    //     for (int y = 0; y < src.h; ++y)
    //     {
    // 修改 by 文亦尧
    for (int y = 0; y < src.h; ++y)
    {
        for (int x = 0; x < src.w; ++x)
        {
            int idx = y * src.w + x;
            int mx = w / 2; // 256
            int my = h / 2; // 256
            if (ldistance(x, y, mx, my) >= d0)
            {
                dst.data[idx].real = src.data[idx].real;
                dst.data[idx].imag = src.data[idx].imag;
            }
            else
            {
                dst.data[idx].real = 0;
                // dst.data[idx].real = 0;
                // 修改 by 文亦尧
                // 可能编译器自动给 dst 填充 0 了
                // 所以原代码有时也可以正常运行
                dst.data[idx].imag = 0;
            }
        }
    }
    return dst;
}
// 巴特沃斯高通
Spectrum Butterworth_high_pass(Spectrum src, double d0, double n)
{
    int w = src.w;
    int h = src.h;
    Spectrum dst;
    dst.w = w;
    dst.h = h;
    dst.data = (Complex *)malloc(sizeof(Complex) * src.w * src.h);
    // for (int x = 0; x < src.w; ++x)
    // {
    //     for (int y = 0; y < src.h; ++y)
    //     {
    // 修改 by 文亦尧
    for (int y = 0; y < src.h; ++y)
    {
        for (int x = 0; x < src.w; ++x)
        {
            int idx = y * src.w + x;
            int mx = w / 2; // 256
            int my = h / 2; // 256
            double dis = ldistance(x, y, mx, my);
            double ba = 1 / (1 + pow(d0 / dis, 2 * n));
            dst.data[idx].real = src.data[idx].real * ba;
            dst.data[idx].imag = src.data[idx].imag * ba;
        }
    }
    return dst;
}
// 高斯高通
Spectrum Gaussian_high_pass(Spectrum src, double d0)
{
    int w = src.w;
    int h = src.h;
    Spectrum dst;
    dst.w = w;
    dst.h = h;
    dst.data = (Complex *)malloc(sizeof(Complex) * src.w * src.h);
    // for (int x = 0; x < src.w; ++x)
    // {
    //     for (int y = 0; y < src.h; ++y)
    //     {
    // 修改 by 文亦尧
    for (int y = 0; y < src.h; ++y)
    {
        for (int x = 0; x < src.w; ++x)
        {
            int idx = y * src.w + x;
            int mx = w / 2; // 256
            int my = h / 2; // 256
            double dis = ldistance(x, y, mx, my);
            double gas = -1 * pow(dis, 2) / pow(d0, 2);
            double ga = 1 - exp(gas);
            dst.data[idx].real = src.data[idx].real * ga;
            dst.data[idx].imag = src.data[idx].imag * ga;
        }
    }
    return dst;
}

dBitmap dct(Bitmap src)
{
    int w = src.w;
    int h = src.h;
    dBitmap src_l;
    int sw, sh;
    int bw, bh;
    if (w % 8 == 0)
        sw = w;
    else
        sw = (w / 8 + 1) * 8;
    if (h % 8 == 0)
        sh = h;
    else
        sh = (h / 8 + 1) * 8;
    src_l.w = sw;
    src_l.h = sh;
    bw = src_l.w / 8;
    bh = src_l.h / 8;
    src_l.data = (double *)malloc(sizeof(double) * src_l.w * src_l.h);
    memset(src_l.data, 0, sizeof(src_l.data));
    // 输出
    dBitmap dst;
    dst.w = sw;
    dst.h = sh;
    dst.data = (double *)malloc(sizeof(double) * dst.w * dst.h);
    memset(dst.data, 0, sizeof(dst.data));
    // 正变换中间矩阵
    // 初始矩阵赋值
    // for (int x = 0; x < sw; ++x)
    // {
    //     for (int y = 0; y < sh; ++y)
    //     {
    //         int idx = x * src.w + y;
    // 修改 by 文亦尧
    for (int y = 0; y < sh; ++y)
    {
        for (int x = 0; x < sw; ++x)
        {
            int idx = y * src.w + x;
            if ((x < w) & (y < h))
            {
                src_l.data[idx] = (double)(src.data[idx].G) * 1.0;
            }
        }
    }
    // for (int bx = 0; bx < bw; ++bx)
    // {
    //     for (int by = 0; by < bh; ++by)
    //     {
    // 修改 by 文亦尧
    for (int by = 0; by < bh; ++by)
    {
        for (int bx = 0; bx < bw; ++bx)
        {
            double *f = (double *)malloc(CN * CN * sizeof(double));
            double *A = (double *)malloc(CN * CN * sizeof(double));
            double *AT = (double *)malloc(CN * CN * sizeof(double));
            double *dct = (double *)malloc(CN * CN * sizeof(double));
            int x = bx * 8;
            int y = by * 8;
            int idx = x + y * src_l.w;
            for (int a = 0; a < 8; ++a)
            {
                for (int b = 0; b < 8; ++b)
                {
                    int offset = a * 8 + b;
                    int offset2 = a * src_l.w + b;
                    f[offset] = src_l.data[idx + offset2];
                }
            }
            // 根据公式，生成转换矩阵A
            double x0 = sqrt(1.0 / CN);
            double x1 = sqrt(2.0 / CN);
            for (int i = 0; i < CN; ++i)
            {
                for (int j = 0; j < CN; ++j)
                {
                    if (i == 0)
                    {
                        A[i * CN + j] = x0 * cos((2 * j + 1) * PI * i / (2 * CN));
                    }
                    else
                    {
                        A[i * CN + j] = x1 * cos((2 * j + 1) * PI * i / (2 * CN));
                    }
                }
            }
            AT = TMatrix(A, CN);
            // 利用转换矩阵A，进行转换
            dct2(f, A, AT, dct);
            for (int a = 0; a < 8; ++a)
            {
                for (int b = 0; b < 8; ++b)
                {
                    int offset = a * 8 + b;
                    int offset2 = a * src_l.w + b;
                    dst.data[idx + offset2] = dct[offset];
                }
            }
            // 清理中间变量 by 文亦尧
            free(f);
            free(A);
            free(AT);
            free(dct);
        }
    }
    // 清理中间变量 by 文亦尧
    free(src_l.data);
    return dst;
}

Bitmap view(dBitmap src)
{
    int w = src.w;
    int h = src.h;
    Bitmap dst;
    dst.w = w;
    dst.h = h;
    dst.data = (Pixel32 *)malloc(sizeof(Pixel32) * dst.w * dst.h);
    // memset(dst.data, 0, sizeof(dst.data));
    // 修改 by 文亦尧
    memset(dst.data, 0, sizeof(Pixel32) * dst.w * dst.h);
    // for (int x = 0; x < w; ++x)
    // {
    //     for (int y = 0; y < h; ++y)
    //     {
    //         int idx = x * src.w + y;
    // 修改 by 文亦尧
    for (int y = 0; y < h; ++y)
    {
        for (int x = 0; x < w; ++x)
        {
            int idx = y * src.w + x;
            double temp = log(src.data[idx]) * 1.0;
            uint8_t tmp = (uint8_t)(temp);
            if (tmp < 0)
                tmp = 0;
            if (tmp > 255)
                tmp = 255;
            dst.data[idx].R = tmp;
            dst.data[idx].G = tmp;
            dst.data[idx].B = tmp;
        }
    }
    return dst;
}

Bitmap idct(dBitmap src)
{
    int w = src.w;
    int h = src.h;
    dBitmap src_l;
    int sw, sh;
    int bw, bh;
    if (w % 8 == 0)
        sw = w;
    else
        sw = (w / 8 + 1) * 8;
    if (h % 8 == 0)
        sh = h;
    else
        sh = (h / 8 + 1) * 8;
    src_l.w = sw;
    src_l.h = sh;
    bw = src_l.w / 8;
    bh = src_l.h / 8;
    src_l.data = (double *)malloc(sizeof(double) * src_l.w * src_l.h);
    memset(src_l.data, 0, sizeof(src_l.data));
    // 输出
    Bitmap dst;
    dst.w = sw;
    dst.h = sh;
    dst.data = (Pixel32 *)malloc(sizeof(Pixel32) * dst.w * dst.h);
    // 初始矩阵赋值
    // for (int x = 0; x < w; ++x)
    // {
    //     for (int y = 0; y < h; ++y)
    //     {
    //         int idx = x * src.w + y;
    // 修改 by 文亦尧
    for (int y = 0; y < h; ++y)
    {
        for (int x = 0; x < w; ++x)
        {
            int idx = y * src.w + x;
            if ((x < w) & (y < h))
            {
                src_l.data[idx] = src.data[idx];
            }
        }
    }
    // for (int bx = 0; bx < bw; ++bx)
    // {
    //     for (int by = 0; by < bh; ++by)
    //     {
    // 修改 by 文亦尧
    for (int by = 0; by < bh; ++by)
    {
        for (int bx = 0; bx < bw; ++bx)
        {
            double *F = (double *)malloc(CN * CN * sizeof(double));
            double *A = (double *)malloc(CN * CN * sizeof(double));
            double *AT = (double *)malloc(CN * CN * sizeof(double));
            double *idct = (double *)malloc(CN * CN * sizeof(double));
            int x = bx * 8;
            int y = by * 8;
            int idx = x + y * src.w;
            for (int a = 0; a < 8; ++a)
            {
                for (int b = 0; b < 8; ++b)
                {
                    int offset = a * 8 + b;
                    int offset2 = a * src.w + b;
                    F[offset] = src_l.data[idx + offset2];
                }
            }
            // 根据公式，生成转换矩阵A
            double x0 = sqrt(1.0 / CN);
            double x1 = sqrt(2.0 / CN);
            for (int i = 0; i < CN; ++i)
            {
                for (int j = 0; j < CN; ++j)
                {
                    if (i == 0)
                    {
                        A[i * CN + j] = x0 * cos((2 * j + 1) * PI * i / (2 * CN));
                    }
                    else
                    {
                        A[i * CN + j] = x1 * cos((2 * j + 1) * PI * i / (2 * CN));
                    }
                }
            }
            AT = TMatrix(A, CN);
            // 利用转换矩阵A，进行转换
            idct2(F, A, AT, idct);
            for (int a = 0; a < 8; ++a)
            {
                for (int b = 0; b < 8; ++b)
                {
                    int offset = a * 8 + b;
                    int offset2 = a * src.w + b;
                    dst.data[idx + offset2].R = (uint8_t)(idct[offset]);
                    dst.data[idx + offset2].G = (uint8_t)(idct[offset]);
                    dst.data[idx + offset2].B = (uint8_t)(idct[offset]);
                }
            }
            // 清理中间变量 by 文亦尧
            free(F);
            free(A);
            free(AT);
            free(idct);
        }
    }
    // 清理中间变量 by 文亦尧
    free(src_l.data);
    return dst;
}

void printM(double *M, int n, char *str)
{
    printf("%s", str);

    for (int i = 0; i < n; ++i)
    {
        printf("\t");
        for (int j = 0; j < n; ++j)
            printf("%.2lf\t", M[i * CN + j]);
        printf("\n");
    }
}

double *TMatrix(double *M, int n)
{
    double *tmp = (double *)malloc(CN * CN * sizeof(double));
    for (int j = 0; j < CN; ++j)
        for (int i = 0; i < CN; ++i)
            tmp[i * CN + j] = M[j * CN + i];

    return tmp;
}

void dct2(double *in, double *A, double *AT, double *out)
{
    double *tt = (double *)malloc(CN * CN * sizeof(double));
    mulMatrix(A, in, tt, CN);
    // printM(tt,N,"tt=\n");
    mulMatrix(tt, AT, out, CN);
    free(tt);
}

void idct2(double *in, double *A, double *AT, double *out)
{
    double *tmp = (double *)malloc(CN * CN * sizeof(double));

    mulMatrix(AT, in, tmp, CN);
    mulMatrix(tmp, A, out, CN);
    free(tmp);
}

void mulMatrix(double *P, double *Q, double *resM, int n)
{
    double res;

    for (int i = 0; i < n; ++i)
    {
        for (int t = 0; t < n; ++t)
        {
            res = 0;
            for (int j = 0; j < n; ++j)
                res += P[i * CN + j] * Q[j * CN + t];
            resM[i * CN + t] = res;
        }
    }
}

// 可视化离散余弦变换
// 作者：李梦达
Bitmap dct_view(Bitmap src)
{
    dBitmap dbmp = dct(src);
    Bitmap dst = view(dbmp);
    free(dbmp.data);
    return dst;
}

// 包装频域滤波函数
// 作者：文亦尧
#define RUN_FREQ_FILT(func, ...) \
    Spectrum spec1 = real2comp(src); \
    Spectrum fspec1 = fft(fshift(spec1)); \
    Spectrum fspec2 = func(fspec1, __VA_ARGS__); \
    Spectrum spec2 = fshift(ifft(fspec2)); \
    Bitmap dst = comp2real(spec2); \
    free(spec1.data); \
    free(fspec1.data); \
    free(fspec2.data); \
    free(spec2.data); \
    if (dst.w != src.w || dst.h != src.h) \
    { \
        Bitmap tmp; \
        tmp.w = src.w; \
        tmp.h = src.h; \
        tmp.data = (Pixel32*)malloc(sizeof(Pixel32) * tmp.w * tmp.h); \
        for (int r = 0; r < tmp.h; ++r) \
        { \
            for (int c = 0; c < tmp.w; ++c) \
            { \
                tmp.data[r * tmp.w + c] = dst.data[r * dst.w + c]; \
            } \
        } \
        free(dst.data); \
        dst = tmp; \
    } \
    return dst;

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
Bitmap ideal_low_pass(Bitmap src, double d0)
{
    RUN_FREQ_FILT(Ideal_low_pass, d0);
}
Bitmap butterworth_low_pass(Bitmap src, double d0, double n)
{
    RUN_FREQ_FILT(Butterworth_low_pass, d0, n);
}
Bitmap gaussian_low_pass(Bitmap src, double d0)
{
    RUN_FREQ_FILT(Gaussian_low_pass, d0);
}

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
Bitmap ideal_high_pass(Bitmap src, double d0)
{
    RUN_FREQ_FILT(Ideal_high_pass, d0);
}
Bitmap butterworth_high_pass(Bitmap src, double d0, double n)
{
    RUN_FREQ_FILT(Butterworth_high_pass, d0, n);
}
Bitmap gaussian_high_pass(Bitmap src, double d0)
{
    RUN_FREQ_FILT(Gaussian_high_pass, d0);
}
