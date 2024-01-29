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

// 例如：中值滤波、最大/小值滤波、均值滤波

// 冒泡排序
// 作者：宋子晨
int BubbleSort(int* arr, int sz, int option) {
    int result_key = 0;
    bool isSwap = false; //标记一趟排序中是否发生交换
    for (int i = sz - 1; i > 0; i--) { //每趟排序元素下标范围是0 ~ Kernel_size * Kernel_size - 1
        for (int j = 0; j < i; j++)
            if (arr[j] > arr[j + 1]) {
                int temp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = temp;
                isSwap = true;
            }
        if (!isSwap) //若本趟排序未发生交换，则排序完成
            break;
    }

    if (option == 0) result_key = arr[(sz - 1) / 2]; //取中值
    else {
        if (option == 1)    result_key = arr[0]; //取最小值
        else{
            if (option == 2) result_key = arr[sz - 1]; //取最大值
        }
    }
    return result_key;
}

// 中值滤波
// 作者：宋子晨
Bitmap mid_filt(Bitmap src, int c) {
    Bitmap dst;
    dst.w = src.w;
    dst.h = src.h;
    dst.data = (Pixel32*)malloc(sizeof(Pixel32) * src.w * src.h);
    memcpy(dst.data, src.data, sizeof(Pixel32) * src.w * src.h);
    int Kernel_size = 2 * c + 1;
    int Kernel_length = Kernel_size * Kernel_size;

    //给reg赋值
    for (int i = 0; i <= src.h - 2 * c - 1; i=i+1) {//行
        for (int j = 0; j <= src.w - 2 * c - 1; j = j + 1) {//列
            int Kernel_reg_R[289] = { 0 }, Kernel_reg_G[289] = { 0 }, Kernel_reg_B[289] = { 0 };
            int idx = i * src.w + j;//定位各个模板的核心位置
            //取模板内像素灰度值
            int flag = 0;
            for (int ii = i; ii <= i+ 2*c;ii++) {
                for (int jj = j; jj <= j+2*c; jj++) {
                    int idxx = ii * src.w + jj;
                    Kernel_reg_R[flag] = src.data[idxx].R;
                    Kernel_reg_G[flag] = src.data[idxx].G;
                    Kernel_reg_B[flag] = src.data[idxx].B;
                    flag++;
                }
            }
            dst.data[idx].R = BubbleSort(Kernel_reg_R, Kernel_length, 0);
            dst.data[idx].G = BubbleSort(Kernel_reg_G, Kernel_length, 0);
            dst.data[idx].B = BubbleSort(Kernel_reg_B, Kernel_length, 0);
            dst.data[idx].A = 255;
        }
    }
    return dst;
}

// 最小值滤波
// 作者：宋子晨
Bitmap min_filt(Bitmap src, int c) {
    Bitmap dst;
    dst.w = src.w;
    dst.h = src.h;
    dst.data = (Pixel32*)malloc(sizeof(Pixel32) * src.w * src.h);
    memcpy(dst.data, src.data, sizeof(Pixel32) * src.w * src.h);

    int Kernel_size = (2 * c + 1);
    int Kernel_length = Kernel_size * Kernel_size;

    //给reg赋值
    for (int i = 0; i <= src.h - 2 * c - 1; i = i + 3) {//行
        for (int j = 0; j <= src.w - 2 * c - 1; j = j + 3) {//列
            int Kernel_reg_R[289] = { 0 }, Kernel_reg_G[289] = { 0 }, Kernel_reg_B[289] = { 0 };
            int idx = i * src.w + j;//定位各个模板的核心位置
            //取模板内像素灰度值
            int flag = 0;
            for (int ii = i; ii <= i + 2 * c; ii++) {
                for (int jj = j; jj <= j + 2 * c; jj++) {
                    int idxx = ii * src.w + jj;
                    Kernel_reg_R[flag] = src.data[idxx].R;
                    Kernel_reg_G[flag] = src.data[idxx].G;
                    Kernel_reg_B[flag] = src.data[idxx].B;
                    flag++;
                }
            }
            dst.data[idx].R = BubbleSort(Kernel_reg_R, Kernel_length, 1);
            dst.data[idx].G = BubbleSort(Kernel_reg_G, Kernel_length, 1);
            dst.data[idx].B = BubbleSort(Kernel_reg_B, Kernel_length, 1);
            dst.data[idx].A = 255;
        }
    }
    return dst;
}

// 最大值滤波
// 作者：宋子晨
Bitmap max_filt(Bitmap src, int c) {
    Bitmap dst;
    dst.w = src.w;
    dst.h = src.h;
    dst.data = (Pixel32*)malloc(sizeof(Pixel32) * src.w * src.h);
    memcpy(dst.data, src.data, sizeof(Pixel32) * src.w * src.h);
    int Kernel_size = (2 * c + 1);
    int Kernel_length = Kernel_size * Kernel_size;

    //给reg赋值
    for (int i = 0; i <= src.h - 2*c - 1; i = i + 3) {//行
        for (int j = 0; j <= src.w - 2 * c - 1; j = j + 3) {//列
            int Kernel_reg_R[289] = { 0 }, Kernel_reg_G[289] = { 0 }, Kernel_reg_B[289] = { 0 };
            int idx = i * src.w + j;//定位各个模板的核心位置
            //取模板内像素灰度值
            int flag = 0;
            for (int ii = i; ii <= i + 2 * c; ii++) {
                for (int jj = j; jj <= j + 2 * c; jj++) {
                    int idxx = ii * src.w + jj;
                    Kernel_reg_R[flag] = src.data[idxx].R;
                    Kernel_reg_G[flag] = src.data[idxx].G;
                    Kernel_reg_B[flag] = src.data[idxx].B;
                    flag++;
                }
            }
            dst.data[idx].R = BubbleSort(Kernel_reg_R, Kernel_length, 2);
            dst.data[idx].G = BubbleSort(Kernel_reg_G, Kernel_length, 2);
            dst.data[idx].B = BubbleSort(Kernel_reg_B, Kernel_length, 2);
            dst.data[idx].A = 255;
        }
    }
    return dst;
}

// 均值滤波
// 作者：宋子晨
Bitmap mean_filt(Bitmap src, int c) {
    Bitmap dst;
    dst.w = src.w;
    dst.h = src.h;
    dst.data = (Pixel32*)malloc(sizeof(Pixel32) * src.w * src.h);
    memcpy(dst.data, src.data, sizeof(Pixel32) * src.w * src.h);
    int Kernel_size = (2 * c + 1);
    int Kernel_length = Kernel_size * Kernel_size;

    //给reg赋值
    int count = (src.w - 2 * c) * (src.h - 2 * c);
    for (int i = 0; i <= src.h - 2*c - 1; i=i+1) {//行
        for (int j = 0; j <= src.w - 2 * c - 1; j = j + 1) {//列
            int idx = i * src.w + j;//定位各个模板的核心位置
            //取模板内像素灰度值之和
            int sum[3] = { 0 , 0, 0 };//sum0->R, sum1->G, sum2->B
            for (int ii = i; ii <= i + 2 * c; ii++) {
                for (int jj = j; jj <= j + 2 * c; jj++) {
                    int idxx = ii * src.w + jj;
                    sum[0] += src.data[idxx].R;
                    sum[1] += src.data[idxx].G;
                    sum[2] += src.data[idxx].B;
                }
            }
            dst.data[idx].R = (int)((double)sum[0] / Kernel_length);
            dst.data[idx].G = (int)((double)sum[1] / Kernel_length);
            dst.data[idx].B = (int)((double)sum[2] / Kernel_length);
            dst.data[idx].A = 255;
        }
    }
    return dst;
}

// 直方图均衡化
// 作者：宋子晨
Bitmap hist_eq(Bitmap src) {
    Bitmap dst;
    dst.w = src.w;
    dst.h = src.h;
    dst.data = (Pixel32*)malloc(sizeof(Pixel32) * src.w * src.h);
    memcpy(dst.data, src.data, sizeof(Pixel32) * src.w * src.h);
    int hist_R[256], hist_G[256], hist_B[256];
    float  fpHist_R[256], fpHist_G[256], fpHist_B[256];
    float eqHistTemp_R[256], eqHistTemp_G[256], eqHistTemp_B[256];
    int eqHist_R[256], eqHist_G[256], eqHist_B[256];

    int size = src.w * src.h;

    memset(&hist_R, 0x00, sizeof(int) * 256);
    memset(&hist_G, 0x00, sizeof(int) * 256);
    memset(&hist_B, 0x00, sizeof(int) * 256);

    memset(&fpHist_R, 0x00, sizeof(float) * 256);
    memset(&fpHist_G, 0x00, sizeof(float) * 256);
    memset(&fpHist_B, 0x00, sizeof(float) * 256);

    memset(&eqHistTemp_R, 0x00, sizeof(float) * 256);
    memset(&eqHistTemp_G, 0x00, sizeof(float) * 256);
    memset(&eqHistTemp_B, 0x00, sizeof(float) * 256);


    for (int i = 0; i < src.h; i++) { //计算差分矩阵直方图  直方图  统计每个灰度级像素点的个数
        for (int j = 0; j < src.w; j++){
            int idx = i * src.w + j;
            unsigned char GrayIndex_R = dst.data[idx].R;
            unsigned char GrayIndex_G = dst.data[idx].G;
            unsigned char GrayIndex_B = dst.data[idx].B;
            hist_R[GrayIndex_R]++;
            hist_G[GrayIndex_G]++;
            hist_B[GrayIndex_B]++;
        }
    }
    for (int i = 0; i < 256; i++) {  // 计算灰度分布密度
        fpHist_R[i] = (float)hist_R[i] / (float)size;
        fpHist_G[i] = (float)hist_G[i] / (float)size;
        fpHist_B[i] = (float)hist_B[i] / (float)size;
    }
    for (int i = 0; i < 256; i++) {  // 计算累计直方图分布
        if (i == 0) {
            eqHistTemp_R[i] = fpHist_R[i];
            eqHistTemp_G[i] = fpHist_G[i];
            eqHistTemp_B[i] = fpHist_B[i];
        }
        else {
            eqHistTemp_R[i] = eqHistTemp_R[i - 1] + fpHist_R[i];
            eqHistTemp_G[i] = eqHistTemp_G[i - 1] + fpHist_G[i];
            eqHistTemp_B[i] = eqHistTemp_B[i - 1] + fpHist_B[i];
        }
    }
    //累计分布取整，保存计算出来的灰度映射关系
    for (int i = 0; i < 256; i++){
        eqHist_R[i] = (int)(255.0 * eqHistTemp_R[i] + 0.5);
        eqHist_G[i] = (int)(255.0 * eqHistTemp_G[i] + 0.5);
        eqHist_B[i] = (int)(255.0 * eqHistTemp_B[i] + 0.5);
    }
    for (int i = 0; i < src.h; i++) { //进行灰度映射均衡化
        for (int j = 0; j < src.w; j++) {
            int idx = i * src.w + j;
            unsigned char GrayIndex_R = dst.data[idx].R;
            unsigned char GrayIndex_G = dst.data[idx].G;
            unsigned char GrayIndex_B = dst.data[idx].B;

            dst.data[idx].R = eqHist_R[GrayIndex_R];
            dst.data[idx].G = eqHist_G[GrayIndex_G];
            dst.data[idx].B = eqHist_B[GrayIndex_B];
        }
    }
    return dst;
}
