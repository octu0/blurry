// +build ignore.

#include <Halide.h>
#include <halide_benchmark.h>

#include "blurry.hpp"

int jit_benchmark_bounds(Func fn, int32_t width, int32_t height) {
  fn.compile_jit(get_jit_target_from_environment());

  double result = benchmark(10, 10, [&]() {
    fn.realize({width, height, 4});
  });
  printf("BenchmarkJIT/%-30s: %-3.5fms\n", fn.name().c_str(), result * 1e3);
  return 0;
}

int jit_benchmark(Func fn, Buffer<uint8_t> buf_src) {
  return jit_benchmark_bounds(fn, buf_src.get()->width(), buf_src.get()->height());
}

int jit_benchmark_once(Func fn, Buffer<uint8_t> buf_src) {
  fn.compile_jit(get_jit_target_from_environment());

  double result = benchmark(1, 1, [&]() {
    fn.realize({buf_src.get()->width(), buf_src.get()->height(), 4});
  });
  printf("BenchmarkJIT/%-30s: %-3.5fms\n", fn.name().c_str(), result * 1e3);
  return 0;
}

int benchmark_cloneimg(Buffer<uint8_t> buf_src, Param<int32_t> width, Param<int32_t> height) {
  return jit_benchmark(cloneimg_fn(
    wrapFunc(buf_src, "buf_src"), width, height
  ), buf_src);
}

int benchmark_convert_from_argb() {
  FILE *const f = fopen("./testdata/argb_320x240.raw", "rb");
  if(f == nullptr) {
    return 1;
  }
  uint8_t *data = (uint8_t *) calloc(320 * 240 * 4, sizeof(uint8_t));
  fread(data, 1, 320 * 240 * 4, f);
  fclose(f);
  Buffer<uint8_t> buf_src = Buffer<uint8_t>::make_interleaved(data, 320, 240, 4);
  Param<int32_t> width{"width", 320};
  Param<int32_t> height{"height", 240};

  return jit_benchmark(convert_from_argb_fn(
    wrapFunc(buf_src, "buf_src"), width, height
  ), buf_src);
}

int benchmark_convert_from_abgr() {
  FILE *const f = fopen("./testdata/abgr_320x240.raw", "rb");
  if(f == nullptr) {
    return 1;
  }
  uint8_t *data = (uint8_t *) calloc(320 * 240 * 4, sizeof(uint8_t));
  fread(data, 1, 320 * 240 * 4, f);
  fclose(f);
  Buffer<uint8_t> buf_src = Buffer<uint8_t>::make_interleaved(data, 320, 240, 4);
  Param<int32_t> width{"width", 320};
  Param<int32_t> height{"height", 240};

  return jit_benchmark(convert_from_abgr_fn(
    wrapFunc(buf_src, "buf_src"), width, height
  ), buf_src);
}

int benchmark_convert_from_bgra() {
  FILE *const f = fopen("./testdata/rabg_le_bgra_320x240.raw", "rb");
  if(f == nullptr) {
    return 1;
  }
  uint8_t *data = (uint8_t *) calloc(320 * 240 * 4, sizeof(uint8_t));
  fread(data, 1, 320 * 240 * 4, f);
  fclose(f);
  Buffer<uint8_t> buf_src = Buffer<uint8_t>::make_interleaved(data, 320, 240, 4);
  Param<int32_t> width{"width", 320};
  Param<int32_t> height{"height", 240};

  return jit_benchmark(convert_from_bgra_fn(
    wrapFunc(buf_src, "buf_src"), width, height
  ), buf_src);
}

int benchmark_convert_from_rabg() {
  FILE *const f = fopen("./testdata/rabg_le_bgra_320x240.raw", "rb");
  if(f == nullptr) {
    return 1;
  }
  uint8_t *data = (uint8_t *) calloc(320 * 240 * 4, sizeof(uint8_t));
  fread(data, 1, 320 * 240 * 4, f);
  fclose(f);
  Buffer<uint8_t> buf_src = Buffer<uint8_t>::make_interleaved(data, 320, 240, 4);
  Param<int32_t> width{"width", 320};
  Param<int32_t> height{"height", 240};

  return jit_benchmark(convert_from_rabg_fn(
    wrapFunc(buf_src, "buf_src"), width, height
  ), buf_src);
}

int benchmark_convert_from_yuv_420() {
  int32_t width = 320;
  int32_t height = 240;
  int32_t ysize = 320 * 240;
  int32_t uvsize = ((width + 1) / 2) * ((height + 1)/ 2);

  FILE *const y = fopen("./testdata/yuv_420_y_320x240.raw", "rb");
  if(y == nullptr) {
    return 1;
  }
  uint8_t *data_y = (uint8_t *) calloc(ysize, sizeof(uint8_t));
  if(data_y == nullptr) {
    return 1;
  }
  fread(data_y, 1, ysize, y);
  fclose(y);

  FILE *const u = fopen("./testdata/yuv_420_u_320x240.raw", "rb");
  if(u == nullptr) {
    return 1;
  }
  uint8_t *data_u = (uint8_t *) calloc(uvsize, sizeof(uint8_t));
  if(data_u == nullptr) {
    return 1;
  }
  fread(data_u, 1, uvsize, u);
  fclose(u);

  FILE *const v = fopen("./testdata/yuv_420_v_320x240.raw", "rb");
  if(v == nullptr) {
    return 1;
  }
  uint8_t *data_v = (uint8_t *) calloc(uvsize, sizeof(uint8_t));
  if(data_v == nullptr) {
    return 1;
  }
  fread(data_v, 1, uvsize, v);
  fclose(v);

  Buffer<uint8_t> src_y = Buffer<uint8_t>::make_interleaved(data_y, width, height, 1);
  Buffer<uint8_t> src_u = Buffer<uint8_t>::make_interleaved(data_u, (width / 2) + 1, height / 2, 1);
  Buffer<uint8_t> src_v = Buffer<uint8_t>::make_interleaved(data_v, (width / 2) + 1, height / 2, 1);
  src_y.raw_buffer()->dimensions = 2;
  src_u.raw_buffer()->dimensions = 2;
  src_u.raw_buffer()->dim[0].extent = width / 2;
  src_u.raw_buffer()->dim[0].stride = 1;
  src_u.raw_buffer()->dim[1].extent = height / 2;
  src_u.raw_buffer()->dim[1].stride = width / 2;
  src_v.raw_buffer()->dimensions = 2;
  src_v.raw_buffer()->dim[0].extent = width / 2;
  src_v.raw_buffer()->dim[0].stride = 1;
  src_v.raw_buffer()->dim[1].extent = height / 2;
  src_v.raw_buffer()->dim[1].stride = width / 2;

  Param<int32_t> _width{"width", width};
  Param<int32_t> _height{"height", height};

  return jit_benchmark_bounds(convert_from_yuv_420_fn(
    wrapFunc_xy(src_y, "src_y"),
    wrapFunc_xy(src_u, "src_u"),
    wrapFunc_xy(src_v, "src_v"),
    _width, _height
  ), width, height);
}

int benchmark_convert_from_yuv_444() {
  int32_t width = 320;
  int32_t height = 240;
  int32_t ysize = 320 * 240;
  int32_t uvsize = 320 * 240;

  FILE *const y = fopen("./testdata/yuv_444_y_320x240.raw", "rb");
  if(y == nullptr) {
    return 1;
  }
  uint8_t *data_y = (uint8_t *) calloc(ysize, sizeof(uint8_t));
  if(data_y == nullptr) {
    return 1;
  }
  fread(data_y, 1, ysize, y);
  fclose(y);

  FILE *const u = fopen("./testdata/yuv_444_u_320x240.raw", "rb");
  if(u == nullptr) {
    return 1;
  }
  uint8_t *data_u = (uint8_t *) calloc(uvsize, sizeof(uint8_t));
  if(data_u == nullptr) {
    return 1;
  }
  fread(data_u, 1, uvsize, u);
  fclose(u);

  FILE *const v = fopen("./testdata/yuv_444_v_320x240.raw", "rb");
  if(v == nullptr) {
    return 1;
  }
  uint8_t *data_v = (uint8_t *) calloc(uvsize, sizeof(uint8_t));
  if(data_v == nullptr) {
    return 1;
  }
  fread(data_v, 1, uvsize, v);
  fclose(v);

  Buffer<uint8_t> src_y = Buffer<uint8_t>::make_interleaved(data_y, width, height, 1);
  Buffer<uint8_t> src_u = Buffer<uint8_t>::make_interleaved(data_u, width, height, 1);
  Buffer<uint8_t> src_v = Buffer<uint8_t>::make_interleaved(data_v, width, height, 1);
  src_y.raw_buffer()->dimensions = 2;
  src_u.raw_buffer()->dimensions = 2;
  src_u.raw_buffer()->dim[0].extent = width;
  src_u.raw_buffer()->dim[0].stride = 1;
  src_u.raw_buffer()->dim[1].extent = height;
  src_u.raw_buffer()->dim[1].stride = width;
  src_v.raw_buffer()->dimensions = 2;
  src_v.raw_buffer()->dim[0].extent = width;
  src_v.raw_buffer()->dim[0].stride = 1;
  src_v.raw_buffer()->dim[1].extent = height;
  src_v.raw_buffer()->dim[1].stride = width;

  Param<int32_t> _width{"width", width};
  Param<int32_t> _height{"height", height};

  return jit_benchmark_bounds(convert_from_yuv_444_fn(
    wrapFunc_xy(src_y, "src_y"),
    wrapFunc_xy(src_u, "src_u"),
    wrapFunc_xy(src_v, "src_v"),
    _width, _height
  ), width, height);
}

int benchmark_convert_to_yuv_444() {
  Buffer<uint8_t> buf_src = load_and_convert_image("./testdata/src.png");
  Param<int32_t> _width{"width", buf_src.get()->width()};
  Param<int32_t> _height{"height", buf_src.get()->height()};

  int32_t y_width = buf_src.get()->width();
  int32_t uv_width = buf_src.get()->width();
  int32_t y_height = buf_src.get()->height();
  int32_t uv_height = buf_src.get()->height();

  return jit_benchmark_bounds(convert_to_yuv_444_fn(
    wrapFunc(buf_src, "buf_src"),
    _width, _height
  ), y_width, y_height + uv_height + uv_height);
}

int benchmark_rotate0(Buffer<uint8_t> buf_src, Param<int32_t> width, Param<int32_t> height) {
  return jit_benchmark(rotate0_fn(wrapFunc(buf_src, "buf_src"), width, height), buf_src);
}

int benchmark_rotate180(Buffer<uint8_t> buf_src, Param<int32_t> width, Param<int32_t> height) {
  return jit_benchmark(rotate180_fn(wrapFunc(buf_src, "buf_src"), width, height), buf_src);
}

int benchmark_rotate90(Buffer<uint8_t> buf_src, Param<int32_t> width, Param<int32_t> height) {
  return jit_benchmark(rotate90_fn(wrapFunc(buf_src, "buf_src"), width, height), buf_src);
}

int benchmark_rotate270(Buffer<uint8_t> buf_src, Param<int32_t> width, Param<int32_t> height) {
  return jit_benchmark(rotate270_fn(wrapFunc(buf_src, "buf_src"), width, height), buf_src);
}

int benchmark_crop(Buffer<uint8_t> buf_src, Param<int32_t> width, Param<int32_t> height) {
  Param<int32_t> px{"px", 175};
  Param<int32_t> py{"py", 40};
  Param<int32_t> crop_width{"crop_width", 80};
  Param<int32_t> crop_height{"crop_height", 50};
  return jit_benchmark(crop_fn(
    wrapFunc(buf_src, "buf_src"), width, height,
    px, py, crop_width, crop_height
  ), buf_src);
}

int benchmark_scale(Buffer<uint8_t> buf_src, Param<int32_t> width, Param<int32_t> height) {
  Param<int32_t> scale_width{"scale_width", 80};
  Param<int32_t> scale_height{"scale_height", 50};
  return jit_benchmark(scale_fn(
    wrapFunc(buf_src, "buf_src"), width, height,
    scale_width, scale_height
  ), buf_src);
}

int benchmark_scale_box(Buffer<uint8_t> buf_src, Param<int32_t> width, Param<int32_t> height) {
  Param<int32_t> scale_width{"scale_width", 80};
  Param<int32_t> scale_height{"scale_height", 50};
  return jit_benchmark(scale_box_fn(
    wrapFunc(buf_src, "buf_src"), width, height,
    scale_width, scale_height
  ), buf_src);
}

int benchmark_scale_linear(Buffer<uint8_t> buf_src, Param<int32_t> width, Param<int32_t> height) {
  Param<int32_t> scale_width{"scale_width", 80};
  Param<int32_t> scale_height{"scale_height", 50};
  return jit_benchmark(scale_linear_fn(
    wrapFunc(buf_src, "buf_src"), width, height,
    scale_width, scale_height
  ), buf_src);
}

int benchmark_scale_gaussian(Buffer<uint8_t> buf_src, Param<int32_t> width, Param<int32_t> height) {
  Param<int32_t> scale_width{"scale_width", 80};
  Param<int32_t> scale_height{"scale_height", 50};
  return jit_benchmark(scale_gaussian_fn(
    wrapFunc(buf_src, "buf_src"), width, height,
    scale_width, scale_height
  ), buf_src);
}

int benchmark_blend_normal(
  Buffer<uint8_t> buf_src0, Param<int32_t> width0, Param<int32_t> height0,
  Buffer<uint8_t> buf_src1, Param<int32_t> width1, Param<int32_t> height1
) {
  Param<int32_t> px{"px", buf_src0.get()->width() / 4};
  Param<int32_t> py{"py", buf_src0.get()->height() / 4};
  return jit_benchmark(blend_normal_fn(
    wrapFunc(buf_src0, "buf_src0"), width0, height0,
    wrapFunc(buf_src1, "buf_src1"), width1, height1,
    px, py
  ), buf_src0);
}

int benchmark_blend_sub(
  Buffer<uint8_t> buf_src0, Param<int32_t> width0, Param<int32_t> height0,
  Buffer<uint8_t> buf_src1, Param<int32_t> width1, Param<int32_t> height1
) {
  Param<int32_t> px{"px", buf_src0.get()->width() / 4};
  Param<int32_t> py{"py", buf_src0.get()->height() / 4};
  return jit_benchmark(blend_sub_fn(
    wrapFunc(buf_src0, "buf_src0"), width0, height0,
    wrapFunc(buf_src1, "buf_src1"), width1, height1,
    px, py
  ), buf_src0);
}

int benchmark_blend_add(
  Buffer<uint8_t> buf_src0, Param<int32_t> width0, Param<int32_t> height0,
  Buffer<uint8_t> buf_src1, Param<int32_t> width1, Param<int32_t> height1
) {
  Param<int32_t> px{"px", buf_src0.get()->width() / 4};
  Param<int32_t> py{"py", buf_src0.get()->height() / 4};
  return jit_benchmark(blend_add_fn(
    wrapFunc(buf_src0, "buf_src0"), width0, height0,
    wrapFunc(buf_src1, "buf_src1"), width1, height1,
    px, py
  ), buf_src0);
}

int benchmark_blend_diff(
  Buffer<uint8_t> buf_src0, Param<int32_t> width0, Param<int32_t> height0,
  Buffer<uint8_t> buf_src1, Param<int32_t> width1, Param<int32_t> height1
) {
  Param<int32_t> px{"px", buf_src0.get()->width() / 4};
  Param<int32_t> py{"py", buf_src0.get()->height() / 4};
  return jit_benchmark(blend_diff_fn(
    wrapFunc(buf_src0, "buf_src0"), width0, height0,
    wrapFunc(buf_src1, "buf_src1"), width1, height1,
    px, py
  ), buf_src0);
}

int benchmark_erosion(Buffer<uint8_t> buf_src, Param<int32_t> width, Param<int32_t> height) {
  Param<int32_t> size{"size", 5};
  return jit_benchmark(erosion_fn(
    wrapFunc(buf_src, "buf_src"), width, height, size
  ), buf_src);
}

int benchmark_dilation(Buffer<uint8_t> buf_src, Param<int32_t> width, Param<int32_t> height) {
  Param<int32_t> size{"size", 5};
  return jit_benchmark(dilation_fn(
    wrapFunc(buf_src, "buf_src"), width, height, size
  ), buf_src);
}

int benchmark_morphology_open(Buffer<uint8_t> buf_src, Param<int32_t> width, Param<int32_t> height) {
  Param<int32_t> size{"size", 3};
  return jit_benchmark(morphology_open_fn(wrapFunc(buf_src, "buf_src"), width, height, size), buf_src);
}

int benchmark_morphology_close(Buffer<uint8_t> buf_src, Param<int32_t> width, Param<int32_t> height) {
  Param<int32_t> size{"size", 3};
  return jit_benchmark(morphology_close_fn(wrapFunc(buf_src, "buf_src"), width, height, size), buf_src);
}

int benchmark_morphology_gradient(Buffer<uint8_t> buf_src, Param<int32_t> width, Param<int32_t> height) {
  Param<int32_t> size{"size", 3};
  return jit_benchmark(morphology_gradient_fn(wrapFunc(buf_src, "buf_src"), width, height, size), buf_src);
}

int benchmark_grayscale(Buffer<uint8_t> buf_src, Param<int32_t> width, Param<int32_t> height) {
  return jit_benchmark(grayscale_fn(
    wrapFunc(buf_src, "buf_src"), width, height
  ), buf_src);
}

int benchmark_invert(Buffer<uint8_t> buf_src, Param<int32_t> width, Param<int32_t> height) {
  return jit_benchmark(invert_fn(
    wrapFunc(buf_src, "buf_src"), width, height
  ), buf_src);
}

int benchmark_brightness(Buffer<uint8_t> buf_src, Param<int32_t> width, Param<int32_t> height) {
  Param<float> factor{"factor", 1.5f};
  return jit_benchmark(brightness_fn(
    wrapFunc(buf_src, "buf_src"), width, height, factor
  ), buf_src);
}

int benchmark_gammacorrection(Buffer<uint8_t> buf_src, Param<int32_t> width, Param<int32_t> height) {
  Param<float> factor{"factor", 1.25f};
  return jit_benchmark(gammacorrection_fn(
    wrapFunc(buf_src, "buf_src"), width, height, factor
  ), buf_src);
}

int benchmark_contrast(Buffer<uint8_t> buf_src, Param<int32_t> width, Param<int32_t> height) {
  Param<float> factor{"factor", 0.525f};
  return jit_benchmark(contrast_fn(
    wrapFunc(buf_src, "buf_src"), width, height, factor
  ), buf_src);
}

int benchmark_boxblur(Buffer<uint8_t> buf_src, Param<int32_t> width, Param<int32_t> height) {
  Param<uint8_t> size{"size", 10};
  return jit_benchmark(boxblur_fn(
    wrapFunc(buf_src, "buf_src"), width, height, size
  ), buf_src);
}

int benchmark_gaussianblur(Buffer<uint8_t> buf_src, Param<int32_t> width, Param<int32_t> height) {
  Param<float> sigma{"sigma", 5.0f};
  return jit_benchmark(gaussianblur_fn(
    wrapFunc(buf_src, "buf_src"), width, height, sigma
  ), buf_src);
}

int benchmark_edge(Buffer<uint8_t> buf_src, Param<int32_t> width, Param<int32_t> height) {
  return jit_benchmark(edge_fn(
    wrapFunc(buf_src, "buf_src"), width, height
  ), buf_src);
}

int benchmark_sobel(Buffer<uint8_t> buf_src, Param<int32_t> width, Param<int32_t> height) {
  return jit_benchmark(sobel_fn(
    wrapFunc(buf_src, "buf_src"), width, height
  ), buf_src);
}

int benchmark_canny(Buffer<uint8_t> buf_src, Param<int32_t> width, Param<int32_t> height) {
  Param<int32_t> threshold_max{"threshold_max", 250};
  Param<int32_t> threshold_min{"threshold_min", 100};
  return jit_benchmark(canny_fn(
    wrapFunc(buf_src, "buf_src"), width, height,
    threshold_max, threshold_min
  ), buf_src);
}

int benchmark_canny_dilate(Buffer<uint8_t> buf_src, Param<int32_t> width, Param<int32_t> height) {
  Param<int32_t> threshold_max{"threshold_max", 250};
  Param<int32_t> threshold_min{"threshold_min", 100};
  Param<int32_t> dilate{"dilate", 3};
  return jit_benchmark(canny_dilate_fn(
    wrapFunc(buf_src, "buf_src"), width, height,
    threshold_max, threshold_min,
    dilate
  ), buf_src);
}

int benchmark_canny_morphology_open(Buffer<uint8_t> buf_src, Param<int32_t> width, Param<int32_t> height) {
  Param<int32_t> threshold_max{"threshold_max", 250};
  Param<int32_t> threshold_min{"threshold_min", 100};
  Param<int32_t> morphology_size{"morphology_size", 3};
  Param<int32_t> dilate{"dilate", 3};
  return jit_benchmark(canny_morphology_open_fn(
    wrapFunc(buf_src, "buf_src"), width, height,
    threshold_max, threshold_min,
    morphology_size,
    dilate
  ), buf_src);
}

int benchmark_canny_morphology_close(Buffer<uint8_t> buf_src, Param<int32_t> width, Param<int32_t> height) {
  Param<int32_t> threshold_max{"threshold_max", 250};
  Param<int32_t> threshold_min{"threshold_min", 100};
  Param<int32_t> morphology_size{"morphology_size", 3};
  Param<int32_t> dilate{"dilate", 3};
  return jit_benchmark(canny_morphology_close_fn(
    wrapFunc(buf_src, "buf_src"), width, height,
    threshold_max, threshold_min,
    morphology_size,
    dilate
  ), buf_src);
}

int benchmark_emboss(Buffer<uint8_t> buf_src, Param<int32_t> width, Param<int32_t> height) {
  return jit_benchmark(emboss_fn(
    wrapFunc(buf_src, "buf_src"), width, height
  ), buf_src);
}

int benchmark_laplacian(Buffer<uint8_t> buf_src, Param<int32_t> width, Param<int32_t> height) {
  return jit_benchmark(laplacian_fn(
    wrapFunc(buf_src, "buf_src"), width, height
  ), buf_src);
}

int benchmark_highpass(Buffer<uint8_t> buf_src, Param<int32_t> width, Param<int32_t> height) {
  return jit_benchmark(highpass_fn(
    wrapFunc(buf_src, "buf_src"), width, height
  ), buf_src);
}

int benchmark_gradient(Buffer<uint8_t> buf_src, Param<int32_t> width, Param<int32_t> height) {
  return jit_benchmark(gradient_fn(
    wrapFunc(buf_src, "buf_src"), width, height
  ), buf_src);
}

int benchmark_blockmozaic(Buffer<uint8_t> buf_src, Param<int32_t> width, Param<int32_t> height) {
  Param<int32_t> block{"block", 10};
  return jit_benchmark(blockmozaic_fn(
    wrapFunc(buf_src, "buf_src"), width, height, block
  ), buf_src);
}

int benchmark_match_template_sad(
  Buffer<uint8_t> buf_src, Param<int32_t> width, Param<int32_t> height,
  Buffer<uint8_t> buf_tpl, Param<int32_t> tpl_width, Param<int32_t> tpl_height
){
  return jit_benchmark(match_template_sad_fn(
    wrapFunc(buf_src, "buf_src"), width, height,
    wrapFunc(buf_tpl, "buf_tpl"), tpl_width, tpl_height
  ), buf_src);
}

int benchmark_match_template_ssd(
  Buffer<uint8_t> buf_src, Param<int32_t> width, Param<int32_t> height,
  Buffer<uint8_t> buf_tpl, Param<int32_t> tpl_width, Param<int32_t> tpl_height
) {
  return jit_benchmark(match_template_ssd_fn(
    wrapFunc(buf_src, "buf_src"), width, height,
    wrapFunc(buf_tpl, "buf_tpl"), tpl_width, tpl_height
  ), buf_src);
}

int benchmark_match_template_ncc(
  Buffer<uint8_t> buf_src, Param<int32_t> width, Param<int32_t> height,
  Buffer<uint8_t> buf_tpl, Param<int32_t> tpl_width, Param<int32_t> tpl_height
){
  return jit_benchmark(match_template_ncc_fn(
    wrapFunc(buf_src, "buf_src"), width, height,
    wrapFunc(buf_tpl, "buf_tpl"), tpl_width, tpl_height
  ), buf_src);
}

int benchmark_prepared_match_template_ncc(
  Buffer<uint8_t> buf_src, Param<int32_t> width, Param<int32_t> height,
  Buffer<uint8_t> buf_tpl, Param<int32_t> tpl_width, Param<int32_t> tpl_height
){
  Func fn = prepare_ncc_template_fn(wrapFunc(buf_tpl, "tpl"), tpl_width, tpl_height);
  Realization r = fn.realize({buf_tpl.get()->width(), buf_tpl.get()->height(), 4});
  Buffer<float> buf_tpl_val = r[0];
  Buffer<float> buf_tpl_sum = r[1];

  return jit_benchmark(prepared_match_template_ncc_fn(
    wrapFunc(buf_src, "buf_src"), width, height,
    wrapFunc_xy(buf_tpl_val, "val"), wrapFunc_xy(buf_tpl_sum, "sum"),
    tpl_width, tpl_height
  ), buf_src);
}

int benchmark_match_template_zncc(
  Buffer<uint8_t> buf_src, Param<int32_t> width, Param<int32_t> height,
  Buffer<uint8_t> buf_tpl, Param<int32_t> tpl_width, Param<int32_t> tpl_height
){
  return jit_benchmark(match_template_zncc_fn(
    wrapFunc(buf_src, "buf_src"), width, height,
    wrapFunc(buf_tpl, "buf_tpl"), tpl_width, tpl_height
  ), buf_src);
}

int benchmark_prepared_match_template_zncc(
  Buffer<uint8_t> buf_src, Param<int32_t> width, Param<int32_t> height,
  Buffer<uint8_t> buf_tpl, Param<int32_t> tpl_width, Param<int32_t> tpl_height
){
  Func fn = prepare_zncc_template_fn(wrapFunc(buf_tpl, "tpl"), tpl_width, tpl_height);
  Realization r = fn.realize({buf_tpl.get()->width(), buf_tpl.get()->height(), 4});
  Buffer<float> buf_tpl_val = r[0];
  Buffer<float> buf_tpl_stddev = r[1];

  return jit_benchmark(prepared_match_template_zncc_fn(
    wrapFunc(buf_src, "buf_src"), width, height,
    wrapFunc_xy(buf_tpl_val, "val"), wrapFunc_xy(buf_tpl_stddev, "stddev"),
    tpl_width, tpl_height
  ), buf_src);
}

int benchmark(char **argv) {
  printf("benchmark...\n");
  Buffer<uint8_t> buf_src = load_and_convert_image(argv[2]);
  Param<int32_t> width{"width", buf_src.get()->width()};
  Param<int32_t> height{"height", buf_src.get()->height()};

  Buffer<uint8_t> buf_tpl = load_and_convert_image(argv[3]);
  Param<int32_t> tpl_width{"tpl_width", buf_tpl.get()->width()};
  Param<int32_t> tpl_height{"tpl_height", buf_tpl.get()->height()};

  printf("src %dx%d\n", width.get(), height.get());
  benchmark_cloneimg(buf_src, width, height);
  benchmark_convert_from_argb();
  benchmark_convert_from_abgr();
  benchmark_convert_from_bgra();
  benchmark_convert_from_rabg();
  benchmark_convert_from_yuv_420();
  benchmark_convert_from_yuv_444();
  benchmark_convert_to_yuv_444();
  benchmark_rotate0(buf_src, width, height);
  benchmark_rotate90(buf_src, width, height);
  benchmark_rotate180(buf_src, width, height);
  benchmark_rotate270(buf_src, width, height);
  benchmark_crop(buf_src, width, height);
  benchmark_scale(buf_src, width, height);
  benchmark_scale_box(buf_src, width, height);
  benchmark_scale_linear(buf_src, width, height);
  benchmark_scale_gaussian(buf_src, width, height);
  benchmark_blend_normal(buf_src, width, height, buf_tpl, tpl_width, tpl_height);
  benchmark_blend_sub(buf_src, width, height, buf_tpl, tpl_width, tpl_height);
  benchmark_blend_add(buf_src, width, height, buf_tpl, tpl_width, tpl_height);
  benchmark_blend_diff(buf_src, width, height, buf_tpl, tpl_width, tpl_height);
  benchmark_grayscale(buf_src, width, height);
  benchmark_invert(buf_src, width, height);
  benchmark_brightness(buf_src, width, height);
  benchmark_gammacorrection(buf_src, width, height);
  benchmark_contrast(buf_src, width, height);
  benchmark_boxblur(buf_src, width, height);
  benchmark_gaussianblur(buf_src, width, height);
  benchmark_blockmozaic(buf_src, width, height);
  benchmark_erosion(buf_src, width, height);
  benchmark_dilation(buf_src, width, height);
  benchmark_morphology_open(buf_src, width, height);
  benchmark_morphology_close(buf_src, width, height);
  benchmark_morphology_gradient(buf_src, width, height);
  benchmark_emboss(buf_src, width, height);
  benchmark_laplacian(buf_src, width, height);
  benchmark_highpass(buf_src, width, height);
  benchmark_gradient(buf_src, width, height);
  benchmark_edge(buf_src, width, height);
  benchmark_sobel(buf_src, width, height);
  benchmark_canny(buf_src, width, height);
  benchmark_canny_dilate(buf_src, width, height);
  benchmark_canny_morphology_open(buf_src, width, height);
  benchmark_canny_morphology_close(buf_src, width, height);
  benchmark_match_template_sad(buf_src, width, height, buf_tpl, tpl_width, tpl_height);
  benchmark_match_template_ssd(buf_src, width, height, buf_tpl, tpl_width, tpl_height);
  benchmark_match_template_ncc(buf_src, width, height, buf_tpl, tpl_width, tpl_height);
  benchmark_prepared_match_template_ncc(buf_src, width, height, buf_tpl, tpl_width, tpl_height);
  benchmark_match_template_zncc(buf_src, width, height, buf_tpl, tpl_width, tpl_height);
  benchmark_prepared_match_template_zncc(buf_src, width, height, buf_tpl, tpl_width, tpl_height);

  return 0;
}

int main(int argc, char **argv) {
  return benchmark(argv);
}
