// +build ignore.

#include <Halide.h>
#include <halide_image_io.h>
#include <halide_benchmark.h>

using namespace Halide;
using namespace Halide::Tools;

#ifndef BLURRY_H_
#define BLURRY_H_

const int16_t ROTATE0   = 0;   // No rotation
const int16_t ROTATE90  = 90;  // Rotate 90 degrees clockwise
const int16_t ROTATE180 = 180; // Rotate 180 degrees
const int16_t ROTATE270 = 270; // Rotate 270 degrees clockwise
const uint8_t MORPH_OPEN = 1, MORPH_CLOSE = 2, MORPH_GRADIENT = 3;

Func wrapFunc(Buffer<uint8_t> buf, const char* name);

Func cloneimg_fn(Func input, Param<int32_t> width, Param<int32_t> height);

Func rotate_fn(Func input, Param<int32_t> width, Param<int32_t> height, Param<int16_t> mode);

Func erosion_fn(Func input, Param<int32_t> width, Param<int32_t> height, Param<uint8_t> size);

Func dilation_fn(Func input, Param<int32_t> width, Param<int32_t> height, Param<uint8_t> size);

Func morphology_fn(
  Func input, Param<int32_t> width, Param<int32_t> height,
  Param<uint8_t> mode, Param<int32_t> size
);

Func grayscale_fn(Func input, Param<int32_t> width, Param<int32_t> height);

Func invert_fn(Func input, Param<int32_t> width, Param<int32_t> height);

Func brightness_fn(Func input, Param<int32_t> width, Param<int32_t> height, Param<float> factor);

Func gammacorrection_fn(Func input, Param<int32_t> width, Param<int32_t> height, Param<float> factor);

Func contrast_fn(Func input, Param<int32_t> width, Param<int32_t> height, Param<float> factor);

Func boxblur_fn(Func input, Param<int32_t> width, Param<int32_t> height, Param<uint8_t> size);

Func gaussianblur_fn(Func input, Param<int32_t> width, Param<int32_t> height, Param<float> sigma);

Func edge_fn(Func input, Param<int32_t> width, Param<int32_t> height);

Func sobel_fn(Func input, Param<int32_t> width, Param<int32_t> height);

Func canny_fn(
  Func input, Param<int32_t> width, Param<int32_t> height,
  Param<int32_t> threshold_max, Param<int32_t> threshold_min,
  Param<int32_t> dilate_size
);

Func emboss_fn(Func input, Param<int32_t> width, Param<int32_t> height);

Func laplacian_fn(Func input, Param<int32_t> width, Param<int32_t> height);

Func highpass_fn(Func input, Param<int32_t> width, Param<int32_t> height);

Func gradient_fn(Func input, Param<int32_t> width, Param<int32_t> height);

Func blockmozaic_fn(Func input, Param<int32_t> width, Param<int32_t> height, Param<int32_t> block_size);

#endif // BLURRY_H_
