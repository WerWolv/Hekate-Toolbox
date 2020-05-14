#pragma once
#include "switch.h"

struct WidthHeight {
    int width;
    int height;
};

//Takes a pointer to a jpeg data array and the size of the array.
//Returns a pointer to a dynamically allocated array containing a RGBA32 bitmap image.
//If w and h are not 0, it will use that resolution instead.
//It must be freed manually!
const u8 *jpegdec(const u8 *buffer, size_t size, WidthHeight res = {});