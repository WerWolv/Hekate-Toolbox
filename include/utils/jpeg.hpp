#pragma once
#include "switch.h"

//Takes a pointer to a jpeg data array and the size of the array.
//Returns a pointer to a dynamically allocated array containing a RGBA32 bitmap image.
//It must be freed manually!
const u8* jpegdec(const u8 *buffer, size_t size);