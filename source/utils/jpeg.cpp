#include "jpeg.hpp"
#include "turbojpeg.h"
#include "types.h"
#include <memory>


const u8* jpegdec(const u8 *buffer, size_t size) {
  auto handle = tjInitDecompress();
  if (handle == nullptr)
    return nullptr;

  int w, h, samp, color_space;

  if (tjDecompressHeader3(handle, buffer, size, &w, &h, &samp, &color_space)) {
      tjDestroy(handle);
      return nullptr;
  }

  u8 *img_data = static_cast<u8*>(malloc(w * h * sizeof(color_t)));

  if (tjDecompress2(handle, buffer, size, img_data, w, 0, h, TJPF_RGBA, TJFLAG_ACCURATEDCT)) {
      tjFree(img_data);
      tjDestroy(handle);
      return nullptr;
  }

  tjDestroy(handle);
  return img_data;
}