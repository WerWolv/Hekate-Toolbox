#include "jpeg.hpp"
#include "turbojpeg.h"
#include "types.h"

const u8 *jpegdec(const u8 *buffer, size_t size, WidthHeight res) {
    auto handle = tjInitDecompress();
    if (handle == nullptr)
        return nullptr;

    int w, h, samp, color_space;

    if (tjDecompressHeader3(handle, buffer, size, &w, &h, &samp, &color_space)) {
        tjDestroy(handle);
        return nullptr;
    }

    u8 *img_data = tjAlloc((res.width == 0 ? w : res.width) * (res.height == 0 ? h : res.height) * sizeof(color_t));

    if (tjDecompress2(handle, buffer, size, img_data, res.width, 0, res.height, TJPF_RGBA, TJFLAG_ACCURATEDCT)) {
        tjFree(img_data);
        tjDestroy(handle);
        return nullptr;
    }

    tjDestroy(handle);
    return img_data;
}