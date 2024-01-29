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

//=================================================================================================
// 空域变换
//=================================================================================================

//线性变换
Bitmap lin_trans(Bitmap src, double a, double b, double c, double d)
{
    Bitmap dst;
    dst.w = src.w;
    dst.h = src.h;
    dst.data = (Pixel32*)malloc(sizeof(Pixel32) * dst.w * dst.h);
    memcpy(dst.data, src.data, sizeof(Pixel32) * dst.w * dst.h);

    for (int i = 0; i < dst.h; ++i)
    {
        for (int j = 0; j < dst.w; ++j)
        {
            int idx = i * dst.w + j;

            Pixel32* p1 = &dst.data[idx];

            if(p1->R < a && p1->R >= 0){
                p1->R = p1->G = p1->B = (unsigned char)(c*p1->R/a);
            }
            else if(p1->R >=a && p1->R < b){
                p1->R = p1->G = p1->B = (unsigned char)((d-c)*(p1->R-a)/(b-a)+c);
            }
            else if(p1->R >= b && p1->R <= p1->A){
                p1->R = p1->G = p1->B = (unsigned char)((255-d)*(p1->R-b)/(255-b)+d);
            }
            else{
                p1->R = p1->G = p1->B;
            }
        }
    }
    return dst;
}


// 对数变换
Bitmap log_trans(Bitmap src, double c)
{
    Bitmap dst;
    dst.w = src.w;
    dst.h = src.h;
    dst.data = (Pixel32*)malloc(sizeof(Pixel32) * dst.w * dst.h);
    memcpy(dst.data, src.data, sizeof(Pixel32) * dst.w * dst.h);

    for (int i = 0; i < dst.h; ++i)
    {
        for (int j = 0; j < dst.w; ++j)
        {
            int idx = i * dst.w + j;

            Pixel32* p1 = &dst.data[idx];

            p1->R = p1->G = p1->B = (unsigned char)(c*log((1+p1->R)));
        }
    }
    return dst;
}

// 指数变换
Bitmap exp_trans(Bitmap src, double c, double gamma)
{
    Bitmap dst;
    dst.w = src.w;
    dst.h = src.h;
    dst.data = (Pixel32*)malloc(sizeof(Pixel32) * dst.w * dst.h);
    memcpy(dst.data, src.data, sizeof(Pixel32) * dst.w * dst.h);

    for (int i = 0; i < dst.h; ++i)
    {
        for (int j = 0; j < dst.w; ++j)
        {
            int idx = i * dst.w + j;

            Pixel32* p1 = &dst.data[idx];

            p1->R = p1->G = p1->B = (unsigned char)(c*pow(p1->R,gamma));
        }
    }
    return dst;
}

// 例如：各种微分操作，如索贝尔、拉普拉斯算子等（待补充）
//roberts
Bitmap roberts(Bitmap src)
{
    Bitmap dst;
    dst.w = src.w;
    dst.h = src.h;
    dst.data = (Pixel32*)malloc(sizeof(Pixel32) * dst.w * dst.h);
    memcpy(dst.data, src.data, sizeof(Pixel32) * dst.w * dst.h);

    int temp;
    int ia,ja;
    int im,jm;

    for (int i = 0; i < dst.h; ++i)
    {
        for (int j = 0; j < dst.w; ++j)
        {
            ia = (i + 1 > dst.h - 1) ? 0 : i + 1;
            ja = (j + 1 > dst.w - 1) ? 0 : j + 1;
            im = (i - 1 < 0) ? 0 : i - 1;
            jm = (j - 1 < 0) ? 0 : j - 1;

            int idx1 = im * dst.w + jm;
            Pixel32* p1 = &dst.data[idx1];
            int idx2 = im * dst.w + j;
            Pixel32* p2 = &dst.data[idx2];
            int idx3 = im * dst.w + ja;
            Pixel32* p3 = &dst.data[idx3];
            int idx4 = i * dst.w + jm;
            Pixel32* p4 = &dst.data[idx4];
            int idx5 = i * dst.w + j;
            Pixel32* p5 = &dst.data[idx5];
            int idx6 = i * dst.w + ja;
            Pixel32* p6 = &dst.data[idx6];
            int idx7 = ia * dst.w + jm;
            Pixel32* p7 = &dst.data[idx7];
            int idx8 = ia * dst.w + j;
            Pixel32* p8 = &dst.data[idx8];
            int idx9 = ia * dst.w + ja;
            Pixel32* p9 = &dst.data[idx9];

            temp = (int)(0.5*abs(p9->R - p5->R)+0.5*abs(p8->R - p6->R));
            p1->R = p1->G = p1->B = (unsigned char)temp;
        }
    }
    return dst;
}

//prewitt
Bitmap prewitt(Bitmap src)
{
    Bitmap dst;
    dst.w = src.w;
    dst.h = src.h;
    dst.data = (Pixel32*)malloc(sizeof(Pixel32) * dst.w * dst.h);
    memcpy(dst.data, src.data, sizeof(Pixel32) * dst.w * dst.h);

    int temp;
    int ia,ja,im,jm;

    for (int i = 0; i < dst.h; ++i)
    {
        for (int j = 0; j < dst.w; ++j)
        {
            ia = (i + 1 > dst.h - 1) ? 0 : i + 1;
            ja = (j + 1 > dst.w - 1) ? 0 : j + 1;
            im = (i - 1 < 0) ? 0 : i - 1;
            jm = (j - 1 < 0) ? 0 : j - 1;

            int idx1 = im * dst.w + jm;
            Pixel32* p1 = &dst.data[idx1];
            int idx2 = im * dst.w + j;
            Pixel32* p2 = &dst.data[idx2];
            int idx3 = im * dst.w + ja;
            Pixel32* p3 = &dst.data[idx3];
            int idx4 = i * dst.w + jm;
            Pixel32* p4 = &dst.data[idx4];
            int idx5 = i * dst.w + j;
            Pixel32* p5 = &dst.data[idx5];
            int idx6 = i * dst.w + ja;
            Pixel32* p6 = &dst.data[idx6];
            int idx7 = ia * dst.w + jm;
            Pixel32* p7 = &dst.data[idx7];
            int idx8 = ia * dst.w + j;
            Pixel32* p8 = &dst.data[idx8];
            int idx9 = ia * dst.w + ja;
            Pixel32* p9 = &dst.data[idx9];

            temp = (int)(0.5*abs(p3->R + p6->R + p9->R- p1->R - p4->R - p7->R)+0.5*abs(p7->R + p8->R + p9->R- p1->R - p2->R - p3->R));

            p1->R = p1->G = p1->B = (unsigned char)temp;
        }
    }
    return dst;
}

//sobel
Bitmap sobel(Bitmap src)
{
    Bitmap dst;
    dst.w = src.w;
    dst.h = src.h;
    dst.data = (Pixel32*)malloc(sizeof(Pixel32) * dst.w * dst.h);
    memcpy(dst.data, src.data, sizeof(Pixel32) * dst.w * dst.h);

    int temp;
    int ia,ja,im,jm;

    for (int i = 0; i < dst.h; ++i)
    {
        for (int j = 0; j < dst.w; ++j)
        {
            ia = (i + 1 > dst.h - 1) ? 0 : i + 1;
            ja = (j + 1 > dst.w - 1) ? 0 : j + 1;
            im = (i - 1 < 0) ? 0 : i - 1;
            jm = (j - 1 < 0) ? 0 : j - 1;

            int idx1 = im * dst.w + jm;
            Pixel32* p1 = &dst.data[idx1];
            int idx2 = im * dst.w + j;
            Pixel32* p2 = &dst.data[idx2];
            int idx3 = im * dst.w + ja;
            Pixel32* p3 = &dst.data[idx3];
            int idx4 = i * dst.w + jm;
            Pixel32* p4 = &dst.data[idx4];
            int idx5 = i * dst.w + j;
            Pixel32* p5 = &dst.data[idx5];
            int idx6 = i * dst.w + ja;
            Pixel32* p6 = &dst.data[idx6];
            int idx7 = ia * dst.w + jm;
            Pixel32* p7 = &dst.data[idx7];
            int idx8 = ia * dst.w + j;
            Pixel32* p8 = &dst.data[idx8];
            int idx9 = ia * dst.w + ja;
            Pixel32* p9 = &dst.data[idx9];

//	1 2 3  -1 0 1  -1 -2 -1
//	4 5 6  -2 0 2   0  0  0
//  7 8 9  -1 0 1   1  2  1

            temp = (int)(0.5*abs(p3->R + 2*p6->R + p9->R- p1->R - 2*p4->R - p7->R)+0.5*abs(p7->R + 2*p8->R + p9->R- p1->R - 2*p2->R - p3->R));

            p1->R = p1->G = p1->B = (unsigned char)temp;
        }
    }
    return dst;
}

//laplace
Bitmap laplacian(Bitmap src)
{
    Bitmap dst;
    dst.w = src.w;
    dst.h = src.h;
    dst.data = (Pixel32*)malloc(sizeof(Pixel32) * dst.w * dst.h);
    memcpy(dst.data, src.data, sizeof(Pixel32) * dst.w * dst.h);

    int temp;
    int ia,ja,im,jm;

    for (int i = 0; i < dst.h; ++i)
    {
        for (int j = 0; j < dst.w; ++j)
        {
            ia = (i + 1 > dst.h - 1) ? 0 : i + 1;
            ja = (j + 1 > dst.w - 1) ? 0 : j + 1;
            im = (i - 1 < 0) ? 0 : i - 1;
            jm = (j - 1 < 0) ? 0 : j - 1;

            int idx1 = im * dst.w + jm;
            Pixel32* p1 = &dst.data[idx1];
            int idx2 = im * dst.w + j;
            Pixel32* p2 = &dst.data[idx2];
            int idx3 = im * dst.w + ja;
            Pixel32* p3 = &dst.data[idx3];
            int idx4 = i * dst.w + jm;
            Pixel32* p4 = &dst.data[idx4];
            int idx5 = i * dst.w + j;
            Pixel32* p5 = &dst.data[idx5];
            int idx6 = i * dst.w + ja;
            Pixel32* p6 = &dst.data[idx6];
            int idx7 = ia * dst.w + jm;
            Pixel32* p7 = &dst.data[idx7];
            int idx8 = ia * dst.w + j;
            Pixel32* p8 = &dst.data[idx8];
            int idx9 = ia * dst.w + ja;
            Pixel32* p9 = &dst.data[idx9];

            temp = abs(p2->R + p4->R - 4 *p5->R + p6->R + p8->R);
            //temp = temp + p1->R;
            //if (temp < 0)temp = 0;
            p1->R = p1->G = p1->B = (unsigned char)temp;
        }
    }
    return dst;
}
