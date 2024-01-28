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
// 位图操作
//=================================================================================================

typedef struct tagMyBGRA
{
    UINT8 B, G, R, A;
}
MyBGRA; // 32-bit true color

typedef struct tagMyBmpInfo
{
    UINT nWidth, nHeight;
}
MyBmpInfo;

static BYTE* gCurrPos = NULL;
// We use this to simulate fread in memory block.
static void myMemRead(void* pDst, size_t nByteSize)
{
    memcpy(pDst, gCurrPos, nByteSize);
    gCurrPos += nByteSize;
}
#define PTR_SIZE_ARG(X) &X, sizeof(X)

// Load binary data from file; return NULL if failed.
static BYTE* myLoadFileData(PCWSTR szFileName)
{
    // Try to open file.
    FILE* fp = _wfopen(szFileName, L"rb");
    if (fp == NULL) return NULL;

    // Get file byte size.
    fseek(fp, 0, SEEK_END);
    size_t nByteSize = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    // Read file data into memory.
    BYTE* data = (BYTE*)malloc(nByteSize);
    if (data == NULL) return NULL;

    // Check whether all data is loaded.
    if (fread(data, 1, nByteSize, fp) != nByteSize)
    {
        free(data);
        fclose(fp);
        return NULL;
    }
    else
    {
        fclose(fp);
        return data;
    }
}

MyBGRA* myReadBmp(PCWSTR szFileName, MyBmpInfo* pInfo)
{
    // We decide that user must provide a valid info struct.
    if (pInfo == NULL) return NULL;

    BYTE* data = myLoadFileData(szFileName);
    if (data == NULL) return NULL;

    // Initialize memory pointer.
    gCurrPos = data;

    // Get BMP file header.
    BITMAPFILEHEADER bmpfh;
    BITMAPINFOHEADER bmpih;

    myMemRead(PTR_SIZE_ARG(bmpfh));
    myMemRead(PTR_SIZE_ARG(bmpih));

    // BM, Windows
    if (bmpfh.bfType != 0x4D42) return NULL;

    pInfo->nWidth = bmpih.biWidth;
    // BMP's height can be negative, which means the image is flipped.
    pInfo->nHeight = abs(bmpih.biHeight);

    // We don't support image with too large size.
    if (max(pInfo->nWidth, pInfo->nHeight) > 65536) return NULL;

    // Allocate memory for pixel data.
    size_t nPixelCount = pInfo->nWidth * pInfo->nHeight;
    MyBGRA* pixel = (MyBGRA*)malloc(sizeof(MyBGRA) * nPixelCount);
    if (pixel == NULL) goto pixel_data_allocate_failed;

    // The number of bytes in each scan line of BMP is a multiple of 4.
    UINT nRowLength = 4 * ((bmpih.biWidth * bmpih.biBitCount + 31) / 32);

    //-------------------------------------------------------------------------------------------------
    // For 24-bit or 32-bit color mode, the image stores BGR(A) directly.
    //-------------------------------------------------------------------------------------------------
    if (bmpih.biBitCount == 24 || bmpih.biBitCount == 32)
    {
        // Move to pixel data start.
        gCurrPos = data + bmpfh.bfOffBits;

        UINT8 bgr[3];

        // Store as B G R, B G R, B G R ......
        if (bmpih.biBitCount == 24)
        {
            for (UINT j = 0; j < pInfo->nHeight; ++j)
            {
                // 3 Bytes for 1 Pixel
                for (UINT i = 0; i < nRowLength; i += 3)
                {
                    // Skip padded zero bytes.
                    if (i / 3 >= pInfo->nWidth)
                    {
                        gCurrPos += (nRowLength - i);
                        break; // Move to next scan line.
                    }
                    myMemRead(bgr, sizeof(UINT8) * 3);

                    // Get pixel index in data.
                    UINT idx = i / 3 + j * pInfo->nWidth;

                    pixel[idx].B = bgr[0];
                    pixel[idx].G = bgr[1];
                    pixel[idx].R = bgr[2];
                    pixel[idx].A = 255; // Keep opaque by default.
                }
            }
        }
        // Store as B G R A, B G R A, B G R A ......
        else
        {
            myMemRead(pixel, sizeof(MyBGRA) * nPixelCount);
        }
    }
    //-------------------------------------------------------------------------------------------------
    // For 16-bit color mode, the image stores BGR as 5 (B), 5 (G), 5 (R), 1 (reserved).
    //-------------------------------------------------------------------------------------------------
    else if (bmpih.biBitCount == 16)
    {
        // We decide not to support BMP with compression format.
        if (bmpih.biCompression != BI_RGB) return NULL;

        // Move to pixel data start.
        gCurrPos = data + bmpfh.bfOffBits;

        UINT16 bgr;
        UINT8 tmpB, tmpG, tmpR;

        for (UINT j = 0; j < pInfo->nHeight; ++j)
        {
            // 2 Bytes for 1 Pixel
            for (UINT i = 0; i < nRowLength; i += 2)
            {
                myMemRead(PTR_SIZE_ARG(bgr));
                // Skip padded zero bytes.
                if (i / 2 >= pInfo->nWidth) continue;

                // Get pixel index in data.
                UINT idx = i / 2 + j * pInfo->nWidth;

                tmpB = (bgr & 0x001F) >> 0; // 0000 0000 0001 1111
                tmpG = (bgr & 0x03E0) >> 5; // 0000 0011 1110 0000
                tmpR = (bgr & 0x7C00) >> 10;// 0111 1100 0000 0000

#define MY_RGB_555_TO_888(X) (((X) << 3) + ((X) & 0x07))

                pixel[idx].B = MY_RGB_555_TO_888(tmpB);
                pixel[idx].G = MY_RGB_555_TO_888(tmpG);
                pixel[idx].R = MY_RGB_555_TO_888(tmpR);
                pixel[idx].A = 255; // Keep opaque by default.

#undef MY_RGB_555_TO_888
            }
        }
    }
    //-------------------------------------------------------------------------------------------------
    // For other modes, the image stores color indices.
    //-------------------------------------------------------------------------------------------------
    else
    {
        // Create indexed color palette.
        UINT nColorCount = 2 << bmpih.biBitCount;
        MyBGRA* palette = (MyBGRA*)malloc(sizeof(MyBGRA) * nColorCount);
        if (palette == NULL) goto palette_data_create_failed;
        myMemRead(palette, sizeof(MyBGRA) * nColorCount);

        // Move to pixel data start.
        gCurrPos = data + bmpfh.bfOffBits;

        UINT8 coloridx;

        // 1 Byte for 8 Pixels
        if (bmpih.biBitCount == 1)
        {
            for (UINT j = 0; j < pInfo->nHeight; ++j)
            {
                for (UINT i = 0; i < nRowLength; ++i)
                {
                    // Get color index in palette.
                    myMemRead(PTR_SIZE_ARG(coloridx));
                    // Get pixel index in data.
                    UINT idx = 8 * i + j * pInfo->nWidth;

#define MY_MONOCHROME(Bitpos) pixel[idx + (Bitpos)] = palette[(coloridx >> (7 - (Bitpos))) & 0x01];

                    // Handle remainder at line end.
                    if (8 * i >= pInfo->nWidth) continue;
                    else if (8 * i < pInfo->nWidth && 8 * (i + 1) > pInfo->nWidth)
                    {
                        for (UINT k = 0; k < (pInfo->nWidth - 8 * i); ++k)
                        {
                            MY_MONOCHROME(k)
                        }
                        continue;
                    }

                    // Populate pixel with monochrome byte.
                    for (UINT k = 0; k < 8; ++k) MY_MONOCHROME(k)

#undef MY_MONOCHROME
                }
            }
        }
        // 1 Byte for 2 Pixels
        else if (bmpih.biBitCount == 4)
        {
            for (UINT j = 0; j < pInfo->nHeight; ++j)
            {
                for (UINT i = 0; i < nRowLength; ++i)
                {
                    // Get color index in palette.
                    myMemRead(PTR_SIZE_ARG(coloridx));
                    // Get pixel index in data.
                    UINT idx = 2 * i + j * pInfo->nWidth;

                    // Don't forget the last pixel!
                    if (2 * i + 1 == pInfo->nWidth)
                    {
                        pixel[idx] = palette[coloridx >> 4];
                    }
                    // Skip padded zero bytes.
                    if (2 * i + 1 >= pInfo->nWidth) continue;

                    // Populate pixel with color in palette.
                    pixel[idx] = palette[coloridx >> 4];
                    pixel[idx + 1] = palette[coloridx & 0x0F];
                }
            }
        }
        // 1 Byte for 1 Pixel
        else if (bmpih.biBitCount == 8)
        {
            for (UINT j = 0; j < pInfo->nHeight; ++j)
            {
                for (UINT i = 0; i < nRowLength; ++i)
                {
                    // Get color index in palette.
                    myMemRead(PTR_SIZE_ARG(coloridx));
                    // Skip padded zero bytes.
                    if (i >= pInfo->nWidth) continue;

                    // Get pixel index in data.
                    UINT idx = i + j * pInfo->nWidth;

                    pixel[idx] = palette[coloridx];
                }
            }
        }
        else { free(palette); goto unknown_bit_count_encountered; }

        // Note we have allocated memory for palette data.
        free(palette);
    }

    // Note we have allocated memory for file data.
    free(data);
    return pixel;

palette_data_create_failed:
unknown_bit_count_encountered:
    free(pixel);

pixel_data_allocate_failed:
    free(data);
    return NULL;
}

BOOL myWriteBmp(PCWSTR szFileName, const MyBGRA* pData, const MyBmpInfo* pInfo)
{
    FILE* fp = _wfopen(szFileName, L"wb");

    // Set BMP file header.
    BITMAPFILEHEADER bmpfh;
    bmpfh.bfType = 0x4D42; // BM, Windows
    bmpfh.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
    // The image stores BGR(A) directly in 32-bit color mode.
    size_t nPixelDataByteSize = sizeof(MyBGRA) * pInfo->nWidth * pInfo->nHeight;
    bmpfh.bfSize = bmpfh.bfOffBits + (DWORD)nPixelDataByteSize;
    bmpfh.bfReserved1 = bmpfh.bfReserved2 = 0;

    BITMAPINFOHEADER bmpih;
    bmpih.biSize = sizeof(BITMAPINFOHEADER);
    bmpih.biWidth = pInfo->nWidth;
    bmpih.biHeight = pInfo->nHeight;
    bmpih.biPlanes = 1; // always be 1
    bmpih.biBitCount = 32; // true color
    bmpih.biCompression = BI_RGB; // no compression
    bmpih.biSizeImage = 0; // could be 0 for RGB
    // We don't care about these fields.
    bmpih.biXPelsPerMeter = bmpih.biYPelsPerMeter = 0;
    bmpih.biClrUsed = bmpih.biClrImportant = 0;

    fwrite(PTR_SIZE_ARG(bmpfh), 1, fp);
    fwrite(PTR_SIZE_ARG(bmpih), 1, fp);

    fwrite(pData, nPixelDataByteSize, 1, fp);

    // Whether closed successfully. EOF(-1) if failed.
    return fclose(fp) == 0;
}

// 打开 BMP 文件（以左上角为原点，先行后列）
// 作者：文亦尧
Bitmap load_bmp(const wchar_t* filename)
{
    MyBmpInfo info;
    info.nWidth = 0;
    info.nHeight = 0;

    MyBGRA* data = myReadBmp((PCWSTR)filename, &info);

    Bitmap bmp;
    bmp.w = info.nWidth;
    bmp.h = info.nHeight;
    bmp.data = (Pixel32*)malloc(sizeof(Pixel32) * bmp.w * bmp.h);

    for (int r = 0; r < bmp.h; ++r)
    {
        for (int c = 0; c < bmp.w; ++c)
        {
            MyBGRA* p1 = &data[r * bmp.w + c];
            Pixel32* p2 = &bmp.data[(bmp.h - r - 1) * bmp.w + c];

            p2->R = p1->R;
            p2->G = p1->G;
            p2->B = p1->B;
            p2->A = p1->A;
        }
    }
    free(data);
    return bmp;
}

// 保存 BMP 文件（以左上角为原点，先行后列）
// 作者：文亦尧
//
// 返回值：
// 表示 BMP 文件是否保存成功
//
bool save_bmp(const wchar_t* filename, Bitmap bmp)
{
    MyBmpInfo info;
    info.nWidth = bmp.w;
    info.nHeight = bmp.h;
    MyBGRA* data = (MyBGRA*)malloc(sizeof(MyBGRA) * bmp.w * bmp.h);

    for (int r = 0; r < bmp.h; ++r)
    {
        for (int c = 0; c < bmp.w; ++c)
        {
            MyBGRA* p1 = &data[r * bmp.w + c];
            Pixel32* p2 = &bmp.data[(bmp.h - r - 1) * bmp.w + c];

            p1->R = p2->R;
            p1->G = p2->G;
            p1->B = p2->B;
            p1->A = p2->A;
        }
    }
    bool is_ok = myWriteBmp((PCWSTR)filename, data, &info);
    free(data);
    return is_ok;
}

// 彩色图像转灰度图像
// 作者：文亦尧
Bitmap gray(Bitmap src)
{
    Bitmap dst;
    dst.w = src.w;
    dst.h = src.h;
    dst.data = (Pixel32*)malloc(sizeof(Pixel32) * dst.w * dst.h);

    for (int r = 0; r < src.h; ++r)
    {
        for (int c = 0; c < src.w; ++c)
        {
            int idx = r * src.w + c;
            Pixel32* p1 = &src.data[idx];
            Pixel32* p2 = &dst.data[idx];

            // Empirical Formula:
            // Gray = 0.299 * R + 0.587 * G + 0.114 * B
            // ==> Gray = (299 * R + 587 * G + 114 * B) / 1000
            // ==> Gray = (30 * R + 59 * G + 11 * B) / 100
            // Take binary operation into consideration:
            // ==> Gray = (19595 * R + 38469 * G + 7472 * B) / 65536
            // ==> Gray = (19595 * R + 38469 * G + 7472 * B) >> 16
            // ==> Gray = (38 * R + 75 * G + 15 * B) >> 7, (the one we use here).

            p2->R = p2->G = p2->B = (38 * p1->R + 75 * p1->G + 15 * p1->B) >> 7;
        }
    }
    return dst;
}

// 颜色反相
// 作者：文亦尧
Bitmap invert(Bitmap src)
{
    Bitmap dst;
    dst.w = src.w;
    dst.h = src.h;
    dst.data = (Pixel32*)malloc(sizeof(Pixel32) * dst.w * dst.h);

    for (int r = 0; r < src.h; ++r)
    {
        for (int c = 0; c < src.w; ++c)
        {
            int idx = r * src.w + c;
            Pixel32* p1 = &src.data[idx];
            Pixel32* p2 = &dst.data[idx];

            p2->R = 255 - p1->R;
            p2->G = 255 - p1->G;
            p2->B = 255 - p1->B;
        }
    }
    return dst;
}

//=================================================================================================
// 频域变换
//=================================================================================================

/*
* We define a simple (no optimization) complex math package here to help with Fourier transform.
*/

typedef struct tagMyComplex { long double real, imag; } MyComplex;

/*
* Prefix "mycp" means "my + Complex".
*/

// Return square of magnitude (aka power) of the complex number.
long double mycpMagnitude2(MyComplex* z)
{ return z->real * z->real + z->imag * z->imag; }

// Return magnitude (aka voltage) of the complex number.
long double mycpMagnitude(MyComplex* z)
{ return sqrt(z->real * z->real + z->imag * z->imag); }

// Return phase of the complex number.
long double mycpPhase(MyComplex* z)
{ return (z->real == 0 && z->imag == 0) ? 0 : atan2(z->imag, z->real); }

// Invert [z].
void mycpMinusInPlace(MyComplex* z)
{ z->real = -z->real; z->imag = -z->imag; }

// Return inverted [z].
MyComplex mycpMinus(MyComplex* z)
{ MyComplex g = { -z->real, -z->imag }; return g; }

// Make conjugate of [z].
void mycpConjInPlace(MyComplex* z)
{ z->imag = -z->imag; }

// Return conjugate of [z].
MyComplex mycpConj(MyComplex* z)
{ MyComplex g = { z->real, -z->imag }; return g; }

// Populate [g] with [z1 + z2].
void mycpAddInPlace(MyComplex* z1, MyComplex* z2, MyComplex* g)
{ g->real = z1->real + z2->real; g->imag = z1->imag + z2->imag; }

// Return [z1 + z2].
MyComplex mycpAdd(MyComplex* z1, MyComplex* z2)
{ MyComplex g = { z1->real + z2->real, z1->imag + z2->imag }; return g; }

// Populate [g] with [z1 - z2].
void mycpSubInPlace(MyComplex* z1, MyComplex* z2, MyComplex* g)
{ g->real = z1->real - z2->real; g->imag = z1->imag - z2->imag; }

// Return [z1 - z2].
MyComplex mycpSub(MyComplex* z1, MyComplex* z2)
{ MyComplex g = { z1->real - z2->real, z1->imag - z2->imag }; return g; }

// Populate [g] with [z1 * z2].
void mycpMulInPlace(MyComplex* z1, MyComplex* z2, MyComplex* g)
{
    // Note the multiplication depends both of the real and imag part,
    // so we must use a intermidiate variable to store the 1st result!
    // (In case that the user calls with  [ g = z1 ]  or  [ g = z2 ] )
    long double tmpReal = z1->real * z2->real - z1->imag * z2->imag;
    g->imag = z1->real * z2->imag + z1->imag * z2->real;
    g->real = tmpReal; // In case real changed before calculate imag.
}

// Populate [g] with [c * z].
void mycpMulScalarInPlace(long double c, MyComplex* z, MyComplex* g)
{ g->real = c * z->real; g->imag = c * z->imag; }

// Return [z1 * z2].
MyComplex mycpMul(MyComplex* z1, MyComplex* z2)
{
    MyComplex g =
    {
        z1->real * z2->real - z1->imag * z2->imag,
        z1->real * z2->imag + z1->imag * z2->real
    };
    return g;
}

// Return [c * z].
MyComplex mycpMulScalar(long double c, MyComplex* z)
{ MyComplex g = { c * z->real, c * z->imag }; return g; }

// Populate [g] with [z1 / z2].
void mycpDivInPlace(MyComplex* z1, MyComplex* z2, MyComplex* g)
{
    long double z2mag = z2->real * z2->real + z2->imag * z2->imag;
    long double tmpReal = (z1->real * z2->real + z1->imag * z2->imag) / z2mag;
    g->imag = (z1->imag * z2->real - z1->real * z2->imag) / z2mag;
    g->real = tmpReal; // @see mycpMulInPlace for the necessity of tmpReal.
}

// Return [z1 / z2].
MyComplex mycpDiv(MyComplex* z1, MyComplex* z2)
{
    long double z2mag = z2->real * z2->real + z2->imag * z2->imag;
    MyComplex g =
    {
        (z1->real * z2->real + z1->imag * z2->imag) / z2mag,
        (z1->imag * z2->real - z1->real * z2->imag) / z2mag
    };
    return g;
}

/*
* We don't provide methods to divide a complex with a scalar,
* since it can be done easily by calling mycpMulScalar(1 / c, z).
*/

// Populate [z] with [e^(j * power)].
void mycpEjInPlace(long double power, MyComplex* z)
{ z->real = cos(power); z->imag = sin(power); }

// Return [e^(j * power)].
MyComplex mycpEj(long double power)
{ MyComplex z = { cos(power), sin(power) }; return z; }

void mycpDFT(MyComplex* pDst, MyComplex* pSrc, UINT L, UINT incre)
{
    // Prepare helper variables.
    long double _2pi = -6.283185L;

    for (UINT f = 0; f < L; ++f)
    {
        UINT dstIdx = f * incre;
        // Populate zero since we will do increment later.
        pDst[dstIdx].real = pDst[dstIdx].imag = 0;
        for (UINT t = 0; t < L; ++t)
        {
            UINT srcIdx = t * incre;
            // e^-j * 2pi * (ft / L)
            MyComplex item = mycpEj(_2pi * ((double)(f * t) / L));
            // f(t) * item
            mycpMulInPlace(pSrc + srcIdx, &item, &item);
            // sum of items
            mycpAddInPlace(pDst + dstIdx, &item, pDst + dstIdx);
        }
    }
}

void mycpIDFT(MyComplex* pDst, MyComplex* pSrc, UINT L, UINT incre)
{
    // Prepare helper variables.
    long double twopi = 6.283185L;
    long double recpL = 1.0L / L;

    for (UINT t = 0; t < L; ++t)
    {
        UINT dstIdx = t * incre;
        // Populate zero since we will do increment later.
        pDst[dstIdx].real = pDst[dstIdx].imag = 0;
        for (UINT f = 0; f < L; ++f)
        {
            UINT srcIdx = f * incre;
            // e^j * 2pi * (ft / L)
            MyComplex item = mycpEj(twopi * ((long double)(f * t) / L));
            // F(f) * item
            mycpMulInPlace(pSrc + srcIdx, &item, &item);
            // sum of items
            mycpAddInPlace(pDst + dstIdx, &item, pDst + dstIdx);
        }
        // normalize, 1 / L
        mycpMulScalarInPlace(recpL, pDst + dstIdx, pDst + dstIdx);
    }
}

UINT myGetRadix2(UINT num)
{
    for (UINT i = 0; i < sizeof(UINT) * 8; ++i)
    {
        if ((0x01 << i) >= num)
        {
            return i;
        }
    }
    return UINT_MAX;
}

void mycpFFT(MyComplex* pData, UINT R, UINT incre)
{
    // We keep the twiddle factor array as static; there's no need to recalculate
    // the factors repeatedly if [N] equals [originN], which would save some time.
    static UINT originRadix = 0u;
    static UINT originLength = 0u;
    static UINT nFactorCount = 0u;
    static MyComplex* WN = NULL; // twiddle factors

    // Height of each butterfly unit.
    static UINT nBtfyDist = 0u;
    // Distance between the headers of 2 adjacent butterfly units.
    // This is always equals to [nBtfyDist * 2] or [nBtfyDist << 1].
    static UINT nBtfyInterGap = 0u;

    static UINT wnidx = 0u; // current twiddle factor index
    static MyComplex* wnptr = NULL; // points to current WN

    // Declare common index variables here.
    static UINT i, j, k, idx, idx2;

    // Prepare helper variables.
    static long double _2pi = -6.283185L;
    static MyComplex* x = NULL, * xp = NULL; // p: peer
    static MyComplex temp = {0,0}; // temp intermidiate

    UINT N = (1 << R);
    // Do nothing for empty/single array.
    if (N <= 1) return;

    // Check whether need to recalculate twiddle factors.
    if (N != originLength)
    {
        originRadix = R;
        originLength = N;
        nFactorCount = N >> 1; // N / 2

        // 2-dot FFT doesn't need twiddle factor.
        if (nFactorCount != 1)
        {
            if (WN != NULL) free(WN);
            // Allocate memory for twiddle factors.
            WN = (MyComplex*)malloc(nFactorCount * sizeof(MyComplex));
            myAssert(WN);

            long double _2pi_N = _2pi / N;
            // Calculate and store twiddle factors.
            for (i = 0; i < nFactorCount; ++i)
            {
                mycpEjInPlace(_2pi_N * i, WN + i);
            }
        }
    }

    // Calculate butterfly operation units of FFT.
    nBtfyDist = N;
    wnidx = 1; // WN_1
    // For each decimation level:
    for (i = 0; i < R; ++i)
    {
        nBtfyInterGap = nBtfyDist;
        nBtfyDist >>= 1; // nBtfyDist / 2

        // Part 1:
        // The 1st butterfly operation never needs twiddle factor,
        // since it is always multiplied by WN_0, i.e. e^(j0) = 1.
        for (j = 0; j < N; j += nBtfyInterGap)
        {
            x = pData + j * incre;
            xp = x + nBtfyDist * incre;

            // y = x + xp
            mycpAddInPlace(x, xp, &temp);
            // yp = x - xp
            mycpSubInPlace(x, xp, xp);

            (*x) = temp; // avoid overwrite
        }

        // Part 2:
        // The remained butterfly units always need twiddle factors.
        wnptr = WN + wnidx;
        // For different twiddle factor groups ......
        for (j = 1; j < nBtfyDist; ++j)
        {
            // For those with the same twiddle factors ......
            for (k = j; k < N; k += nBtfyInterGap)
            {
                x = pData + k * incre;
                xp = x + nBtfyDist * incre;

                // y = x + xp
                mycpAddInPlace(x, xp, &temp);
                // yp = (x - xp) * WN
                mycpSubInPlace(x, xp, x);
                mycpMulInPlace(x, wnptr, xp);

                (*x) = temp; // avoid overwrite
            }
            wnptr += wnidx;
        }
        wnidx <<= 1; // wnidx * 2
    }

    // Rearrange results by bit reversing.
    // For example, for 8-dot FFT of (radix 2) frequency decimation:
    //
    // x(t)    bit reverse  X(f)
    // 0, 000   -------->   0, 000
    // 1, 001   -------->   4, 100
    // 2, 010   -------->   2, 010
    // 3, 011   -------->   6, 110
    // 4, 100   -------->   1, 001
    // 5, 101   -------->   5, 101
    // 6, 110   -------->   3, 011
    // 7, 111  --------->   7, 111
    //
    for (i = 0; i < N; ++i)
    {
        k = 0;
        idx = i * incre;
        // Get reversed number.
        for (j = 0; j < R; ++j)
        {
            k = (k << 1) | (0x01 & (i >> j));
        }
        // Swap if found.
        if (i < k)
        {
            idx2 = k * incre;
            temp = pData[idx];
            pData[idx] = pData[idx2];
            pData[idx2] = temp;
        }
    }
}

void mycpIFFT(MyComplex* pData, UINT R, UINT incre)
{
    // We keep the twiddle factor array as static; there's no need to recalculate
    // the factors repeatedly if [N] equals [originN], which would save some time.
    static UINT originRadix = 0u;
    static UINT originLength = 0u;
    static UINT nFactorCount = 0u;
    static MyComplex* WN = NULL; // twiddle factors

    // Height of each butterfly unit.
    static UINT nBtfyDist = 0u;
    // Distance between the headers of 2 adjacent butterfly units.
    // This is always equals to [nBtfyDist * 2] or [nBtfyDist << 1].
    static UINT nBtfyInterGap = 0u;

    static UINT wnidx = 0u; // current twiddle factor index
    static MyComplex* wnptr = NULL; // points to current WN

    // Declare common index variables here.
    static UINT i, j, k, idx, idx2;

    // Prepare helper variables.
    static long double twopi = 6.283185L;
    static MyComplex* x = NULL, * xp = NULL; // p: peer
    static MyComplex temp = {0,0}; // temp intermidiate
    static long double dNormCoef = 0; // set to [1 / N]

    UINT N = (1 << R);
    // Do nothing for empty/single array.
    if (N <= 1) return;

    // Check whether need to recalculate twiddle factors.
    if (N != originLength)
    {
        originRadix = R;
        originLength = N;
        nFactorCount = N >> 1; // N / 2

        // 2-dot FFT doesn't need twiddle factor.
        if (nFactorCount != 1)
        {
            if (WN != NULL) free(WN);
            // Allocate memory for twiddle factors.
            WN = (MyComplex*)malloc(nFactorCount * sizeof(MyComplex));
            myAssert(WN);

            long double twopi_N = twopi / N;
            // Calculate and store twiddle factors.
            for (i = 0; i < nFactorCount; ++i)
            {
                mycpEjInPlace(twopi_N * i, WN + i);
            }
        }
    }

    // Calculate butterfly operation units of FFT.
    nBtfyDist = N;
    wnidx = 1; // WN_1
    // For each decimation level:
    for (i = 0; i < R; ++i)
    {
        nBtfyInterGap = nBtfyDist;
        nBtfyDist >>= 1; // nBtfyDist / 2

        // Part 1:
        // The 1st butterfly operation never needs twiddle factor,
        // since it is always multiplied by WN_0, i.e. e^(j0) = 1.
        for (j = 0; j < N; j += nBtfyInterGap)
        {
            x = pData + j * incre;
            xp = x + nBtfyDist * incre;

            // y = x + xp
            mycpAddInPlace(x, xp, &temp);
            // yp = x - xp
            mycpSubInPlace(x, xp, xp);

            (*x) = temp; // avoid overwrite
        }

        // Part 2:
        // The remained butterfly units always need twiddle factors.
        wnptr = WN + wnidx;
        // For different twiddle factor groups ......
        for (j = 1; j < nBtfyDist; ++j)
        {
            // For those with the same twiddle factors ......
            for (k = j; k < N; k += nBtfyInterGap)
            {
                x = pData + k * incre;
                xp = x + nBtfyDist * incre;

                // y = x + xp
                mycpAddInPlace(x, xp, &temp);
                // yp = (x - xp) * WN
                mycpSubInPlace(x, xp, x);
                mycpMulInPlace(x, wnptr, xp);

                (*x) = temp; // avoid overwrite
            }
            wnptr += wnidx;
        }
        wnidx <<= 1; // wnidx * 2
    }

    // Rearrange results by bit reversing.
    // For example, for 8-dot FFT of (radix 2) frequency decimation:
    //
    // x(t)    bit reverse  X(f)
    // 0, 000   -------->   0, 000
    // 1, 001   -------->   4, 100
    // 2, 010   -------->   2, 010
    // 3, 011   -------->   6, 110
    // 4, 100   -------->   1, 001
    // 5, 101   -------->   5, 101
    // 6, 110   -------->   3, 011
    // 7, 111  --------->   7, 111
    //
    for (i = 0; i < N; ++i)
    {
        k = 0;
        idx = i * incre;
        // Get reversed number.
        for (j = 0; j < R; ++j)
        {
            k = (k << 1) | (0x01 & (i >> j));
        }
        // Swap if found.
        if (i < k)
        {
            idx2 = k * incre;
            temp = pData[idx];
            pData[idx] = pData[idx2];
            pData[idx2] = temp;
        }
    }

    dNormCoef = 1.0L / N;
    // Normalize results with dot count.
    for (i = 0; i < N; ++i)
    {
        idx = i * incre;
        mycpMulScalarInPlace(dNormCoef, pData + idx, pData + idx);
    }
}

MyComplex* mycpDFT2(MyComplex* pData, UINT M, UINT N)
{
    size_t nByteSize = M * N * sizeof(MyComplex);
    // Allocate transformed results buffer.
    MyComplex* buffer = (MyComplex*)malloc(nByteSize);
    myAssert(buffer);
    // Allocate temporary intermidiate buffer to calculate separately.
    MyComplex* intermidiate = (MyComplex*)malloc(nByteSize);
    myAssert(intermidiate);

    //--------------------------------------------------------------- Horizontal
    for (UINT u = 0; u < M; ++u)
    {
        UINT incre = u;
        mycpDFT(intermidiate + incre, pData + incre, N, M);
    }
    //--------------------------------------------------------------- Vertical
    for (UINT v = 0; v < N; ++v)
    {
        UINT incre = v * M;
        mycpDFT(buffer + incre, intermidiate + incre, M, 1);
    }

    // Note we have allocated memory for intermidiate buffer data.
    free(intermidiate);

    return buffer;
}

MyComplex* mycpIDFT2(MyComplex* pData, UINT M, UINT N)
{
    size_t nByteSize = M * N * sizeof(MyComplex);
    // Allocate transformed results buffer.
    MyComplex* buffer = (MyComplex*)malloc(nByteSize);
    myAssert(buffer);
    // Allocate temporary intermidiate buffer to calculate separately.
    MyComplex* intermidiate = (MyComplex*)malloc(nByteSize);
    myAssert(intermidiate);

    //--------------------------------------------------------------- Horizontal
    for (UINT u = 0; u < M; ++u)
    {
        UINT incre = u;
        mycpIDFT(intermidiate + incre, pData + incre, N, M);
    }
    //--------------------------------------------------------------- Vertical
    for (UINT v = 0; v < N; ++v)
    {
        UINT incre = v * M;
        mycpIDFT(buffer + incre, intermidiate + incre, M, 1);
    }

    // Note we have allocated memory for intermidiate buffer data.
    free(intermidiate);

    return buffer;
}

MyComplex* mycpFFT2(MyComplex* pData, UINT M, UINT RM, UINT N, UINT RN)
{
    size_t nByteSize = M * N * sizeof(MyComplex);
    // Allocate transformed results buffer.
    MyComplex* buffer = (MyComplex*)malloc(nByteSize);
    myAssert(buffer);
    // We have to copy origin data to result buffer in advance
    // since FFT will calculate results in place step by step.
    memcpy(buffer, pData, nByteSize);

    //--------------------------------------------------------------- Horizontal
    for (UINT u = 0; u < M; ++u)
    {
        UINT incre = u;
        mycpFFT(buffer + incre, RN, M);
    }
    //--------------------------------------------------------------- Vertical
    for (UINT v = 0; v < N; ++v)
    {
        UINT incre = v * M;
        mycpFFT(buffer + incre, RM, 1);
    }

    return buffer;
}

MyComplex* mycpIFFT2(MyComplex* pData, UINT M, UINT RM, UINT N, UINT RN)
{
    size_t nByteSize = M * N * sizeof(MyComplex);
    // Allocate transformed results buffer.
    MyComplex* buffer = (MyComplex*)malloc(nByteSize);
    myAssert(buffer);
    // We have to copy origin data to result buffer in advance
    // since FFT will calculate results in place step by step.
    memcpy(buffer, pData, nByteSize);

    //--------------------------------------------------------------- Horizontal
    for (UINT u = 0; u < M; ++u)
    {
        UINT incre = u;
        mycpIFFT(buffer + incre, RN, M);
    }
    //--------------------------------------------------------------- Vertical
    for (UINT v = 0; v < N; ++v)
    {
        UINT incre = v * M;
        mycpIFFT(buffer + incre, RM, 1);
    }

    return buffer;
}

// 实/复数域转换
// 作者：文亦尧
Spectrum real2comp(Bitmap src)
{
    Spectrum dst;
    dst.w = src.w;
    dst.h = src.h;
    dst.data = (Complex*)malloc(sizeof(Complex) * dst.w * dst.h);

    for (int r = 0; r < src.h; ++r)
    {
        for (int c = 0; c < src.w; ++c)
        {
            int idx = r * src.w + c;
            dst.data[idx].real = src.data[idx].R;
            dst.data[idx].imag = 0;
        }
    }
    return dst;
}
Bitmap comp2real(Spectrum src)
{
    Bitmap dst;
    dst.w = src.w;
    dst.h = src.h;
    dst.data = (Pixel32*)malloc(sizeof(Pixel32) * dst.w * dst.h);

    for (int r = 0; r < src.h; ++r)
    {
        for (int c = 0; c < src.w; ++c)
        {
            int idx = r * src.w + c;
            dst.data[idx].R =
            dst.data[idx].G =
            dst.data[idx].B =
            (uint8_t)src.data[idx].real;
            dst.data[idx].A = 255;
        }
    }
    return dst;
}

// 频谱中心化
// 作者：文亦尧
//
// 使用方法 fft(fshift(src))
// 注意此方法不会分配新的数组
// 而是直接更改 src 并将其返回
//
Spectrum fshift(Spectrum src)
{
    for (int y = 0; y < src.h; ++y)
    {
        for (int x = 0; x < src.w; ++x)
        {
            Complex* c = &src.data[y * src.w + x];
            if ((x + y) % 2 != 0) c->real = -c->real;
        }
    }
    return src;
}

// 离散傅里叶变换
// 作者：文亦尧
Spectrum dft(Spectrum src)
{
    Spectrum dst;
    dst.w = src.w;
    dst.h = src.h;
    dst.data = (Complex*)mycpDFT2((MyComplex*)src.data, src.w, src.h);
    return dst;
}
Spectrum idft(Spectrum src)
{
    Spectrum dst;
    dst.w = src.w;
    dst.h = src.h;
    dst.data = (Complex*)mycpIDFT2((MyComplex*)src.data, src.w, src.h);
    return dst;
}

// 快速傅里叶变换
// 作者：文亦尧
Spectrum fft(Spectrum src)
{
    Spectrum dst;
    dst.w = src.w;
    dst.h = src.h;
    dst.data = (Complex*)mycpFFT2((MyComplex*)src.data,
        src.w, myGetRadix2(src.w), src.h, myGetRadix2(src.h));
    return dst;
}
Spectrum ifft(Spectrum src)
{
    Spectrum dst;
    dst.w = src.w;
    dst.h = src.h;
    dst.data = (Complex*)mycpIFFT2((MyComplex*)src.data,
        src.w, myGetRadix2(src.w), src.h, myGetRadix2(src.h));
    return dst;
}
