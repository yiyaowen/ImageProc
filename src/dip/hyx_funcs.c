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
