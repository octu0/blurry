// +build ignore.

#include <Halide.h>
#include <halide_image_io.h>

using namespace Halide;
using namespace Halide::Tools;

#ifndef BLURRY_H_
#define BLURRY_H_

Func kernel_sobel3x3_y();

Func kernel_sobel3x3_x();

Func kernel_emboss3x3();

Func kernel_laplacian3x3();

Func kernel_highpass3x3();

Func kernel_gradient3x3();

Func wrapFunc(Buffer<uint8_t> buf, const char* name);

Func wrapFunc(Buffer<float> buf, const char* name);

Func wrapFunc(Buffer<double> buf, const char* name);

Func wrapFunc_xy(Buffer<uint8_t> buf, const char* name);

Func wrapFunc_xy(Buffer<float> buf, const char* name);

Func wrapFunc_xy(Buffer<double> buf, const char* name);

Func cloneimg_fn(Func input, Param<int32_t> width, Param<int32_t> height);

Func convert_from_abgr_fn(Func input, Param<int32_t> width, Param<int32_t> height);

Func convert_from_argb_fn(Func input, Param<int32_t> width, Param<int32_t> height);

Func convert_from_rabg_fn(Func input, Param<int32_t> width, Param<int32_t> height);

Func convert_from_bgra_fn(Func input, Param<int32_t> width, Param<int32_t> height);

Func convert_from_yuv_444_fn(Func in_y, Func in_u, Func in_v, Param<int32_t> width, Param<int32_t> height);

Func convert_from_yuv_420_fn(Func in_y, Func in_u, Func in_v, Param<int32_t> width, Param<int32_t> height);

Func convert_to_yuv_444_fn(Func input, Param<int32_t> width, Param<int32_t> height);

Func convert_to_yuv_420_fn(Func input, Param<int32_t> width, Param<int32_t> height);

Func rotate0_fn(Func input, Param<int32_t> width, Param<int32_t> height);

Func rotate90_fn(Func input, Param<int32_t> width, Param<int32_t> height);

Func rotate180_fn(Func input, Param<int32_t> width, Param<int32_t> height);

Func rotate270_fn(Func input, Param<int32_t> width, Param<int32_t> height);

Func flipV_fn(Func input, Param<int32_t> width, Param<int32_t> height);

Func flipH_fn(Func input, Param<int32_t> width, Param<int32_t> height);

Func crop_fn(
  Func input,
  Param<int32_t> width, Param<int32_t> height,
  Param<int32_t> px, Param<int32_t> py, Param<int32_t> crop_width, Param<int32_t> crop_height
);

Func scale_fn(
  Func input,
  Param<int32_t> width, Param<int32_t> height,
  Param<int32_t> scale_width, Param<int32_t> scale_height
);

Func scale_box_fn(
  Func input,
  Param<int32_t> width, Param<int32_t> height,
  Param<int32_t> scale_width, Param<int32_t> scale_height
);

Func scale_linear_fn(
  Func input,
  Param<int32_t> width, Param<int32_t> height,
  Param<int32_t> scale_width, Param<int32_t> scale_height
);

Func scale_gaussian_fn(
  Func input,
  Param<int32_t> width, Param<int32_t> height,
  Param<int32_t> scale_width, Param<int32_t> scale_height
);

Func blend_normal_fn(
  Func src0, Param<int32_t> width0, Param<int32_t> height0,
  Func src1, Param<int32_t> width1, Param<int32_t> height1,
  Param<int32_t> px, Param<int32_t> py
);

Func blend_sub_fn(
  Func src0, Param<int32_t> width0, Param<int32_t> height0,
  Func src1, Param<int32_t> width1, Param<int32_t> height1,
  Param<int32_t> px, Param<int32_t> py
);

Func blend_add_fn(
  Func src0, Param<int32_t> width0, Param<int32_t> height0,
  Func src1, Param<int32_t> width1, Param<int32_t> height1,
  Param<int32_t> px, Param<int32_t> py
);

Func blend_diff_fn(
  Func src0, Param<int32_t> width0, Param<int32_t> height0,
  Func src1, Param<int32_t> width1, Param<int32_t> height1,
  Param<int32_t> px, Param<int32_t> py
);

Func erosion_fn(Func input, Param<int32_t> width, Param<int32_t> height, Param<int32_t> size);

Func dilation_fn(Func input, Param<int32_t> width, Param<int32_t> height, Param<int32_t> size);

Func morphology_open_fn(Func input, Param<int32_t> width, Param<int32_t> height, Param<int32_t> size);

Func morphology_close_fn(Func input, Param<int32_t> width, Param<int32_t> height, Param<int32_t> size);

Func morphology_gradient_fn(Func input, Param<int32_t> width, Param<int32_t> height, Param<int32_t> size);

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
  Param<int32_t> threshold_max, Param<int32_t> threshold_min
);

Func canny_dilate_fn(
  Func input, Param<int32_t> width, Param<int32_t> height,
  Param<int32_t> threshold_max, Param<int32_t> threshold_min,
  Param<int32_t> dilate_size
);

Func canny_morphology_open_fn(
  Func input, Param<int32_t> width, Param<int32_t> height,
  Param<int32_t> threshold_max, Param<int32_t> threshold_min,
  Param<int32_t> morphology_size,
  Param<int32_t> dilate_size
);

Func canny_morphology_close_fn(
  Func input, Param<int32_t> width, Param<int32_t> height,
  Param<int32_t> threshold_max, Param<int32_t> threshold_min,
  Param<int32_t> morphology_size,
  Param<int32_t> dilate_size
);

Func emboss_fn(Func input, Param<int32_t> width, Param<int32_t> height);

Func laplacian_fn(Func input, Param<int32_t> width, Param<int32_t> height);

Func highpass_fn(Func input, Param<int32_t> width, Param<int32_t> height);

Func gradient_fn(Func input, Param<int32_t> width, Param<int32_t> height);

Func blockmozaic_fn(Func input, Param<int32_t> width, Param<int32_t> height, Param<int32_t> block_size);

Func match_template_sad_fn(
  Func input, Param<int32_t> width, Param<int32_t> height,
  Func tpl, Param<int32_t> tpl_width, Param<int32_t> tpl_height
);

Func match_template_ssd_fn(
  Func input, Param<int32_t> width, Param<int32_t> height,
  Func tpl, Param<int32_t> tpl_width, Param<int32_t> tpl_height
);

Func match_template_ncc_fn(
  Func input, Param<int32_t> width, Param<int32_t> height,
  Func tpl, Param<int32_t> tpl_width, Param<int32_t> tpl_height
);

Func prepare_ncc_template_fn(
  Func tpl, Param<int32_t> tpl_width, Param<int32_t> tpl_height
);

Func prepared_match_template_ncc_fn(
  Func input, Param<int32_t> width, Param<int32_t> height,
  Func buf_tpl_val, Func buf_tpl_sum,
  Param<int32_t> tpl_width, Param<int32_t> tpl_height
);

Func match_template_zncc_fn(
  Func input, Param<int32_t> width, Param<int32_t> height,
  Func tpl, Param<int32_t> tpl_width, Param<int32_t> tpl_height
);

Func prepare_zncc_template_fn(
  Func tpl, Param<int32_t> tpl_width, Param<int32_t> tpl_height
);

Func prepared_match_template_zncc_fn(
  Func input, Param<int32_t> width, Param<int32_t> height,
  Func buf_tpl_val, Func buf_tpl_stddev,
  Param<int32_t> tpl_width, Param<int32_t> tpl_height
);

Func contour_fn(
  Func input, Param<int32_t> width, Param<int32_t> height,
  Param<uint8_t> threshold, Param<uint8_t> size
);

#endif // BLURRY_H_
