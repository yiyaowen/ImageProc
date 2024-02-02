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

#define CV_PI 3.1415926
//=================================================================================================
// 几何变换
//=================================================================================================

Bitmap flip_x(Bitmap src)
{
    Bitmap dst;
    dst.w = src.w;
    dst.h = src.h;
    dst.data = (Pixel32*)malloc(sizeof(Pixel32) * src.w * src.h);

    for (int r = 0; r < src.h; ++r)
    {
        for (int c = 0; c < src.w; ++c)
        {
            int idx1 = r * src.w + (src.w-c-1);
            int idx2 = r * src.w + c;
            Pixel32* p1 = &src.data[idx1];
            Pixel32* p2 = &dst.data[idx2];

            p2->A = 255;
            p2->R = p1->R ;
            p2->G = p1->G;
            p2->B = p1->B;
        }
    }
    return dst;
}

Bitmap flip_y(Bitmap src)
{
    Bitmap dst;
    dst.w = src.w;
    dst.h = src.h;
    dst.data = (Pixel32*)malloc(sizeof(Pixel32) * src.w * src.h);

    for (int r = 0; r < src.h; ++r)
    {
        for (int c = 0; c < src.w; ++c)
        {
            int idx1 = (src.h - r - 1) * src.w + c;
            int idx2 = r * src.w + c;
            Pixel32* p1 = &src.data[idx1];
            Pixel32* p2 = &dst.data[idx2];

            p2->A = 255;
            p2->R = p1->R;
            p2->G = p1->G;
            p2->B = p1->B;
        }
    }
    return dst;
}

Bitmap move(Bitmap src, int dx, int dy)//左移、上移 < 0
{
    Bitmap dst;
    dst.w = src.w;
    dst.h = src.h;
    dst.data = (Pixel32*)malloc(sizeof(Pixel32) * dst.w * dst.h);

    for (int i = 0; i < dst.h; ++i)
    {
        for (int j = 0; j < dst.w; ++j)
        {
            int idx = i * dst.w + j;
            Pixel32* p = &dst.data[idx];
            p->A = 255;
            p->R = 0;
            p->G = 0;
            p->B = 0;
        }
    }
    for (int r = 0; r < src.h; ++r)
    {
        for (int c = 0; c < src.w; ++c)
        {
            if (r + dy < src.h && r + dy >= 0 && c + dx < src.w && c + dx >= 0)
            {
                int idx1 =  r* src.w + c;
                int idx2 = (r + dy) * src.w + (c + dx);
                Pixel32* p1 = &src.data[idx1];
                Pixel32* p2 = &dst.data[idx2];

                p2->A = 255;
                p2->R = p1->R;
                p2->G = p1->G;
                p2->B = p1->B;
            }
        }
    }
    return dst;
}

Bitmap crop(Bitmap src, int x, int y, int w, int h)
{
    Bitmap dst;
    if (w > src.w || h > src.h)
    {
        dst.w = src.w;
        dst.h = src.h;
        dst.data = (Pixel32*)malloc(sizeof(Pixel32) * dst.w * dst.h);
        //wprintf(L"剪裁面积大于原图\n");
        //dst = src;
        //修改 by 文亦尧
        memcpy(dst.data, src.data, sizeof(Pixel32) * dst.w * dst.h);
    }
    else
    {
        dst.w = w;
        dst.h = h;
        dst.data = (Pixel32*)malloc(sizeof(Pixel32) * dst.w * dst.h);

        for (int r = y; r < y + h; ++r)
        {
            for (int c = x; c < x + w; ++c)
            {
                int idx1 = r * src.w + c;
                int idx2 = (r-y) * w + (c-x);
                Pixel32* p1 = &src.data[idx1];
                Pixel32* p2 = &dst.data[idx2];

                p2->A = 255;
                p2->R = p1->R;
                p2->G = p1->G;
                p2->B = p1->B;
            }
        }
    }
    return dst;
}

Bitmap rotate(Bitmap src, double theta)
{
    double angle = theta * CV_PI / 180;
    //double half_w = src.w / 2, half_h = src.h / 2.0;
    //修改 by 文亦尧
    double half_w = src.w / 2.0, half_h = src.h / 2.0;
    double cosa = cos(angle), sina = sin(angle);
    double x1 = -half_w * cosa + half_h * sina, y1 = half_w * sina + half_h * cosa;
    double x2 = half_w * cosa + half_h * sina, y2 = -half_w * sina + half_h * cosa;
    double x3 = -half_w * cosa - half_h * sina, y3 = half_w * sina - half_h * cosa;
    double x4 = half_w * cosa - half_h * sina, y4 = -half_w * sina - half_h * cosa;

    int width_new = (int)(fmax(fabs(x4 - x1), fabs(x3 - x2)) + 0.5);
    int height_new = (int)(fmax(fabs(y4 - y1), fabs(y3 - y2)) + 0.5);
    Bitmap dst;
    dst.w = width_new;
    dst.h = height_new;
    dst.data = (Pixel32*)malloc(sizeof(Pixel32) * dst.w * dst.h);
    //增加 by 文亦尧
    memset(dst.data, 0, sizeof(Pixel32) * dst.w * dst.h);

    double const1 = -width_new * cosa / 2.0 + height_new * sina / 2.0 + src.w / 2.0;
    double const2 = -width_new * sina / 2.0 - height_new * cosa / 2.0 + src.h / 2.0;

    int x0, y0;
    for (int i = 0; i < dst.h; i++) {
        for (int j = 0; j < dst.w; j++) {
            x0 = (int)(j * cosa - i * sina + const1 + 0.5);
            y0 = (int)(j * sina + i * cosa + const2 + 0.5);
            if (x0 >= 0 && x0 < src.w && y0 >= 0 && y0 < src.h)
            {
                int idx1 = y0 * src.w + x0;
                int idx2 = i * dst.w + j;
                Pixel32* p1 = &src.data[idx1];
                Pixel32* p2 = &dst.data[idx2];

                p2->A = 255;
                p2->R = p1->R;
                p2->G = p1->G;
                p2->B = p1->B;
            }
        }
    }
    return dst;
}

Bitmap scale(Bitmap src, double sx, double sy)
{
    Bitmap dst;
    //dst.w = sx * src.w;
    //dst.h = sy * src.h;
    //修改 by 文亦尧
    dst.w = (int)(src.w * max(sx, 1.0));
    dst.h = (int)(src.h * max(sy, 1.0));
    dst.data = (Pixel32*)malloc(sizeof(Pixel32) * dst.w * dst.h);

    for (int i = 0; i < dst.h; ++i)
    {
        for (int j = 0; j < dst.w; ++j)
        {
            int idx = i * dst.w + j;
            Pixel32* p = &dst.data[idx];
            p->A = 255;
            p->R = 0;
            p->G = 0;
            p->B = 0;
        }
    }
    int x = 0;
    int y = 0;
    for (int i = 0; i < src.h; ++i)
    {
        for (int j = 0; j < src.w; ++j)
        {
            x = (int)(sx * j);
            y = (int)(sy * i);

            int idx1 = i * src.w + j;
            int idx2 = y * dst.w + x;
            Pixel32* p1 = &src.data[idx1];
            Pixel32* p2 = &dst.data[idx2];

            p2->A = 255;
            p2->R = p1->R;
            p2->G = p1->G;
            p2->B = p1->B;
        }
    }
    return dst;
}

Bitmap nearest_interp(Bitmap src, double ratio)
{
    Bitmap dst;
    dst.w = (int)(src.w * ratio);
    dst.h = (int)(src.h * ratio);
    dst.data = (Pixel32*)malloc(sizeof(Pixel32) * dst.w * dst.h);

    for (int r = 0; r < dst.h; ++r)
    {
        for (int c = 0; c < dst.w; ++c)
        {
            //四舍五入
            //int srcX = r / ratio + 0.5;
            //int srcY = c / ratio + 0.5;
            //修改 by 文亦尧
            double srcX_half = (r - dst.h / 2.0) / ratio;
            double srcY_half = (c - dst.w / 2.0) / ratio;
            int srcX = (int)(srcX_half + (src.h / 2.0));
            int srcY = (int)(srcY_half + (src.w / 2.0));
            //防溢出
            if (srcX >= src.h)
                srcX = src.h - 1;
            if (srcY >= src.w)
                srcY = src.w - 1;
            int idx1 = srcX * src.w + srcY;
            int idx2 = r * dst.w + c;
            Pixel32* p1 = &src.data[idx1];
            Pixel32* p2 = &dst.data[idx2];

            p2->A = 255;
            p2->R = p1->R;
            p2->G = p1->G;
            p2->B = p1->B;
        }
    }
    return dst;
}

Bitmap bilinear_interp(Bitmap src, double ratio)
{
    Bitmap dst;
    dst.w = (int)(src.w * ratio);
    dst.h = (int)(src.h * ratio);
    dst.data = (Pixel32*)malloc(sizeof(Pixel32) * dst.w * dst.h);
    memset(dst.data, 0, sizeof(Pixel32) * dst.w * dst.h);

    int x1, x2, y1, y2;
    double srcX_half, srcY_half, srcX, srcY;

    for (int r = 0; r < dst.h; ++r)
    {
        for (int c = 0; c < dst.w; ++c)
        {
            //int srcX = r / ratio;
            //int srcY = c / ratio;
            //修改 by 文亦尧
            srcX_half = (r - dst.h / 2.0) / ratio;
            srcY_half = (c - dst.w / 2.0) / ratio;
            srcX = srcX_half + (src.h / 2.0) - 0.5;
            srcY = srcY_half + (src.w / 2.0) - 0.5;
            //int x1 = srcX;
            //int x2 = srcX + 1 > src.h - 1 ? srcX - 1: srcX + 1;//防溢出
            //修改 by 文亦尧
            x1 = (int)floor(srcX);
            x2 = x1 + 1;
            if (x1 < 0 || x2 > src.h - 1) continue;
            //int y1 = srcY;
            //int y2 = srcY + 1 > src.w - 1 ? srcY - 1: srcY + 1;
            //修改 by 文亦尧
            y1 = (int)floor(srcY);
            y2 = y1 + 1;
            if (y1 < 0 || y2 > src.w - 1) continue;

            int idx1 = x1 * src.w + y1;
            int idx2 = x1 * src.w + y2;
            int idx3 = x2 * src.w + y1;
            int idx4 = x2 * src.w + y2;
            int idx = r * dst.w + c;
            Pixel32* p1 = &src.data[idx1];
            Pixel32* p2 = &src.data[idx2];
            Pixel32* p3 = &src.data[idx3];
            Pixel32* p4 = &src.data[idx4];
            Pixel32* p = &dst.data[idx];

            double a_r = (x2 - srcX) / (x2 - x1) * p1->R + (srcX - x1) / (x2 - x1) * p3->R;
            double b_r = (x2 - srcX) / (x2 - x1) * p2->R + (srcX - x1) / (x2 - x1) * p4->R;
            double a_g = (x2 - srcX) / (x2 - x1) * p1->G + (srcX - x1) / (x2 - x1) * p3->G;
            double b_g = (x2 - srcX) / (x2 - x1) * p2->G + (srcX - x1) / (x2 - x1) * p4->G;
            double a_b = (x2 - srcX) / (x2 - x1) * p1->B + (srcX - x1) / (x2 - x1) * p3->B;
            double b_b = (x2 - srcX) / (x2 - x1) * p2->B + (srcX - x1) / (x2 - x1) * p4->B;
            p->A = 255;
            //p->R = (y2 - srcY) / (y2 - y1) * a_r + (srcY - y1) / (y2 - y1) * a_r;
            //p->G = (y2 - srcY) / (y2 - y1) * a_g + (srcY - y1) / (y2 - y1) * a_g;
            //p->B = (y2 - srcY) / (y2 - y1) * a_b + (srcY - y1) / (y2 - y1) * a_b;
            //修改 by 文亦尧
            p->R = (uint8_t)((y2 - srcY) / (y2 - y1) * a_r + (srcY - y1) / (y2 - y1) * b_r);
            p->G = (uint8_t)((y2 - srcY) / (y2 - y1) * a_g + (srcY - y1) / (y2 - y1) * b_g);
            p->B = (uint8_t)((y2 - srcY) / (y2 - y1) * a_b + (srcY - y1) / (y2 - y1) * b_b);
        }
    }
    return dst;
}

double countW(double a, double x) {
    //x = abs(x);//取绝对值
    //修改 by 文亦尧
    x = fabs(x);
    if (x <= 1)
        return (a + 2) * pow(x, 3) - (a + 3) * pow(x, 2) + 1;
    else if (x > 1 && x < 2)
        return a * pow(x, 3) - 5 * a * pow(x, 2) + 8 * a * x - 4 * a;
    else
        return 0;
}

Bitmap cubic_interp(Bitmap src, double ratio)
{
    Bitmap dst;
    dst.w = (int)(src.w * ratio);
    dst.h = (int)(src.h * ratio);
    dst.data = (Pixel32*)malloc(sizeof(Pixel32) * dst.w * dst.h);
    memset(dst.data, 0, sizeof(Pixel32) * dst.w * dst.h);

    double srcX_half, srcY_half, srcX, srcY;

    int a1 = 0, a2 = 0, a3 = 0, a4 = 0;
    int b1 = 0, b2 = 0, b3 = 0, b4 = 0;
    double w1[4], w2[4];//对应x,y的权重
    int P = 0;
    double sum = 0;

    for (int r = 0; r < dst.h; ++r)
    {
        for (int c = 0; c < dst.w; ++c)
        {
            //int srcX = r / ratio;
            //int srcY = c / ratio;
            //修改 by 文亦尧
            srcX_half = (r - dst.h / 2.0) / ratio;
            srcY_half = (c - dst.w / 2.0) / ratio;
            srcX = srcX_half + (src.h / 2.0) - 0.5;
            srcY = srcY_half + (src.w / 2.0) - 0.5;
            //int x1 = srcX;
            //int x2 = srcX + 1 > src.h - 1 ? srcX - 1: srcX + 1;//防溢出
            //修改 by 文亦尧
            a2 = (int)floor(srcX);
            a1 = a2 - 1;
            a3 = a2 + 1;
            a4 = a2 + 2;
            if (a2 < 0 || a3 > src.h - 1) continue;
            if (a1 < 0) a1 = a2;
            if (a4 > src.h - 1) a4 = a3;
            //int y1 = srcY;
            //int y2 = srcY + 1 > src.w - 1 ? srcY - 1: srcY + 1;
            //修改 by 文亦尧
            b2 = (int)floor(srcY);
            b1 = b2 - 1;
            b3 = b2 + 1;
            b4 = b2 + 2;
            if (b2 < 0 || b3 > src.w - 1) continue;
            if (b1 < 0) b1 = b2;
            if (b4 > src.w - 1) b4 = b3;

            //a1 = (srcX - 1) < 0 ? (srcX + 1) : (srcX - 1);
            //a2 = srcX < 0 ? 0 : srcX;
            //a3 = (srcX + 1) > src.h - 1 ? (srcX - 1) : (srcX + 1);
            //a4 = (srcX + 2) > src.h - 1 ? (srcX - 2) : (srcX + 2);
            //b1 = (srcY - 1) < 0 ? (srcY + 1) : (srcY - 1);
            //b2 = srcY;
            //b3 = (srcY + 1) > src.w - 1 ? (srcY - 1) : (srcY + 1);
            //b4 = (srcY + 2) > src.w - 1 ? (srcY - 2) : (srcY + 2);

            w1[0] = countW(-0.5, srcX - a1);
            w1[1] = countW(-0.5, srcX - a2);
            w1[2] = countW(-0.5, srcX - a3);
            w1[3] = countW(-0.5, srcX - a4);
            w2[0] = countW(-0.5, srcY - b1);
            w2[1] = countW(-0.5, srcY - b2);
            w2[2] = countW(-0.5, srcY - b3);
            w2[3] = countW(-0.5, srcY - b4);
            sum = w1[0] * w2[0] + w1[0] * w2[1] + w1[0] * w2[2] + w1[0] * w2[3]
                + w1[1] * w2[0] + w1[1] * w2[1] + w1[1] * w2[2] + w1[1] * w2[3]
                + w1[2] * w2[0] + w1[2] * w2[1] + w1[2] * w2[2] + w1[2] * w2[3]
                + w1[3] * w2[0] + w1[3] * w2[1] + w1[3] * w2[2] + w1[3] * w2[3];

            int idx1 = a1 * src.w + b1; int idx9 = a3 * src.w + b1;
            int idx2 = a1 * src.w + b2; int idx10 = a3 * src.w + b2;
            int idx3 = a1 * src.w + b3; int idx11 = a3 * src.w + b3;
            int idx4 = a1 * src.w + b4; int idx12 = a3 * src.w + b4;
            int idx5 = a2 * src.w + b1; int idx13 = a4 * src.w + b1;
            int idx6 = a2 * src.w + b2; int idx14 = a4 * src.w + b2;
            int idx7 = a2 * src.w + b3; int idx15 = a4 * src.w + b3;
            int idx8 = a2 * src.w + b4; int idx16 = a4 * src.w + b4;

            int idx = r * dst.w + c;
            Pixel32* p1 = &src.data[idx1]; Pixel32* p9 = &src.data[idx9];
            Pixel32* p2 = &src.data[idx2]; Pixel32* p10 = &src.data[idx10];
            Pixel32* p3 = &src.data[idx3]; Pixel32* p11 = &src.data[idx11];
            Pixel32* p4 = &src.data[idx4]; Pixel32* p12 = &src.data[idx12];
            Pixel32* p5 = &src.data[idx5]; Pixel32* p13 = &src.data[idx13];
            Pixel32* p6 = &src.data[idx6]; Pixel32* p14 = &src.data[idx14];
            Pixel32* p7 = &src.data[idx7]; Pixel32* p15 = &src.data[idx15];
            Pixel32* p8 = &src.data[idx8]; Pixel32* p16 = &src.data[idx16];

            Pixel32* p = &dst.data[idx];

            p->A = 255;
            p->R = (uint8_t)
                  (p1->R * w1[0] * w2[0] / sum + p2->R * w1[0] * w2[1] / sum +
                   p3->R * w1[0] * w2[2] / sum + p4->R * w1[0] * w2[3] / sum +
                   p5->R * w1[1] * w2[0] / sum + p6->R * w1[1] * w2[1] / sum +
                   p7->R * w1[1] * w2[2] / sum + p8->R * w1[1] * w2[3] / sum +
                   p9->R * w1[2] * w2[0] / sum + p10->R * w1[2] * w2[1] / sum +
                   p11->R * w1[2] * w2[2] / sum + p12->R * w1[2] * w2[3] / sum +
                   p13->R * w1[3] * w2[0] / sum + p14->R * w1[3] * w2[1] / sum +
                   p15->R * w1[3] * w2[2] / sum + p16->R * w1[3] * w2[3] / sum);
            p->B = (uint8_t)
                  (p1->B * w1[0] * w2[0] / sum + p2->B * w1[0] * w2[1] / sum +
                   p3->B * w1[0] * w2[2] / sum + p4->B * w1[0] * w2[3] / sum +
                   p5->B * w1[1] * w2[0] / sum + p6->B * w1[1] * w2[1] / sum +
                   p7->B * w1[1] * w2[2] / sum + p8->B * w1[1] * w2[3] / sum +
                   p9->B * w1[2] * w2[0] / sum + p10->B * w1[2] * w2[1] / sum +
                   p11->B * w1[2] * w2[2] / sum + p12->B * w1[2] * w2[3] / sum +
                   p13->B * w1[3] * w2[0] / sum + p14->B * w1[3] * w2[1] / sum +
                   p15->B * w1[3] * w2[2] / sum + p16->B * w1[3] * w2[3] / sum);
            p->G = (uint8_t)
                  (p1->G * w1[0] * w2[0] / sum + p2->G * w1[0] * w2[1] / sum +
                   p3->G * w1[0] * w2[2] / sum + p4->G * w1[0] * w2[3] / sum +
                   p5->G * w1[1] * w2[0] / sum + p6->G * w1[1] * w2[1] / sum +
                   p7->G * w1[1] * w2[2] / sum + p8->G * w1[1] * w2[3] / sum +
                   p9->G * w1[2] * w2[0] / sum + p10->G * w1[2] * w2[1] / sum +
                   p11->G * w1[2] * w2[2] / sum + p12->G * w1[2] * w2[3] / sum +
                   p13->G * w1[3] * w2[0] / sum + p14->G * w1[3] * w2[1] / sum +
                   p15->G * w1[3] * w2[2] / sum + p16->G * w1[3] * w2[3] / sum);
        }
    }
    return dst;
}
