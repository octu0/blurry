// +build ignore.


#include <Halide.h>

#include "blurry.hpp"

typedef struct mt_score_uint32 {
  int x;
  int y;
  uint32_t score;
} mt_score_uint32;

typedef struct mt_score_float {
  int x;
  int y;
  double score;
} mt_score_float;

Buffer<uint8_t> jit_realize_uint8_bounds(Func fn, int32_t width, int32_t height) {
  fn.compile_jit(get_jit_target_from_environment());

  printf("realize(uint8_t) %s...\n", fn.name().c_str());
  
  return fn.realize({width, height, 4});
}

Buffer<uint16_t> jit_realize_uint16_bounds(Func fn, int32_t width, int32_t height) {
  fn.compile_jit(get_jit_target_from_environment());

  printf("realize(uint16_t) %s...\n", fn.name().c_str());
  
  return fn.realize({width, height, 4});
}

Buffer<int32_t> jit_realize_int32_bounds(Func fn, int32_t width, int32_t height) {
  fn.compile_jit(get_jit_target_from_environment());

  printf("realize(int32_t) %s...\n", fn.name().c_str());
  
  return fn.realize({width, height, 4});
}

Buffer<double> jit_realize_double_bounds(Func fn, int32_t width, int32_t height) {
  fn.compile_jit(get_jit_target_from_environment());

  printf("realize(double) %s...\n", fn.name().c_str());
  
  return fn.realize({width, height, 4});
}

Buffer<uint8_t> jit_realize_uint8(Func fn, Buffer<uint8_t> src) {
  return jit_realize_uint8_bounds(fn, src.get()->width(), src.get()->height());
}

Buffer<uint16_t> jit_realize_uint16(Func fn, Buffer<uint8_t> src) {
  return jit_realize_uint16_bounds(fn, src.get()->width(), src.get()->height());
}

Buffer<int32_t> jit_realize_int32(Func fn, Buffer<uint8_t> src) {
  return jit_realize_int32_bounds(fn, src.get()->width(), src.get()->height());
}

Buffer<double> jit_realize_double(Func fn, Buffer<uint8_t> src) {
  return jit_realize_double_bounds(fn, src.get()->width(), src.get()->height());
}

int jit_cloneimg(char **argv) {
  Buffer<uint8_t> buf_src = load_and_convert_image(argv[2]);

  Param<int32_t> width{"width", buf_src.get()->width()};
  Param<int32_t> height{"height", buf_src.get()->height()};

  Buffer<uint8_t> out = jit_realize_uint8(cloneimg_fn(
    wrapFunc(buf_src, "buf_src"), width, height
  ), buf_src);

  printf("save to %s\n", argv[3]);
  save_image(out, argv[3]);
  return 0;
}

int jit_convert_from_argb(char **argv) {
  FILE *const f = fopen(argv[2], "rb");
  if(f == nullptr) {
    return 1;
  }
  uint8_t *data = (uint8_t *) calloc(320 * 240 * 4, sizeof(uint8_t));
  fread(data, 1, 320 * 240 * 4, f);
  fclose(f);

  Buffer<uint8_t> buf_src = Buffer<uint8_t>::make_interleaved(data, 320, 240, 4);

  Param<int32_t> width{"width", 320};
  Param<int32_t> height{"height", 240};

  Buffer<uint8_t> out = jit_realize_uint8(convert_from_argb_fn(
    wrapFunc(buf_src, "buf_src"), width, height
  ), buf_src);

  printf("save to %s\n", argv[3]);
  save_image(out, argv[3]);
  return 0;
}

int jit_convert_from_abgr(char **argv) {
  FILE *const f = fopen(argv[2], "rb");
  if(f == nullptr) {
    return 1;
  }
  uint8_t *data = (uint8_t *) calloc(320 * 240 * 4, sizeof(uint8_t));
  fread(data, 1, 320 * 240 * 4, f);
  fclose(f);

  Buffer<uint8_t> buf_src = Buffer<uint8_t>::make_interleaved(data, 320, 240, 4);

  Param<int32_t> width{"width", 320};
  Param<int32_t> height{"height", 240};

  Buffer<uint8_t> out = jit_realize_uint8(convert_from_abgr_fn(
    wrapFunc(buf_src, "buf_src"), width, height
  ), buf_src);

  printf("save to %s\n", argv[3]);
  save_image(out, argv[3]);
  return 0;
}

int jit_convert_from_bgra(char **argv) {
  FILE *const f = fopen(argv[2], "rb");
  if(f == nullptr) {
    return 1;
  }
  uint8_t *data = (uint8_t *) calloc(320 * 240 * 4, sizeof(uint8_t));
  fread(data, 1, 320 * 240 * 4, f);
  fclose(f);

  Buffer<uint8_t> buf_src = Buffer<uint8_t>::make_interleaved(data, 320, 240, 4);

  Param<int32_t> width{"width", 320};
  Param<int32_t> height{"height", 240};

  Buffer<uint8_t> out = jit_realize_uint8(convert_from_bgra_fn(
    wrapFunc(buf_src, "buf_src"), width, height
  ), buf_src);

  printf("save to %s\n", argv[3]);
  save_image(out, argv[3]);
  return 0;
}

int jit_convert_from_rabg(char **argv) {
  FILE *const f = fopen(argv[2], "rb");
  if(f == nullptr) {
    return 1;
  }
  uint8_t *data = (uint8_t *) calloc(320 * 240 * 4, sizeof(uint8_t));
  fread(data, 1, 320 * 240 * 4, f);
  fclose(f);

  Buffer<uint8_t> buf_src = Buffer<uint8_t>::make_interleaved(data, 320, 240, 4);

  Param<int32_t> width{"width", 320};
  Param<int32_t> height{"height", 240};

  Buffer<uint8_t> out = jit_realize_uint8(convert_from_rabg_fn(
    wrapFunc(buf_src, "buf_src"), width, height
  ), buf_src);

  printf("save to %s\n", argv[3]);
  save_image(out, argv[3]);
  return 0;
}

int jit_convert_from_yuv_420(char **argv) {
  int32_t width = 320;
  int32_t height = 240;
  int32_t ysize = 320 * 240;
  int32_t uvsize = ((width + 1) / 2) * ((height + 1)/ 2);

  FILE *const y = fopen(argv[2], "rb");
  if(y == nullptr) {
    return 1;
  }
  uint8_t *data_y = (uint8_t *) calloc(ysize, sizeof(uint8_t));
  fread(data_y, 1, ysize, y);
  fclose(y);

  FILE *const u = fopen(argv[3], "rb");
  if(u == nullptr) {
    return 1;
  }
  uint8_t *data_u = (uint8_t *) calloc(uvsize, sizeof(uint8_t));
  fread(data_u, 1, uvsize, u);
  fclose(u);

  FILE *const v = fopen(argv[4], "rb");
  if(v == nullptr) {
    return 1;
  }
  uint8_t *data_v = (uint8_t *) calloc(uvsize, sizeof(uint8_t));
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

  Buffer<uint8_t> out = jit_realize_uint8_bounds(convert_from_yuv_420_fn(
    wrapFunc_xy(src_y, "src_y"),
    wrapFunc_xy(src_u, "src_u"),
    wrapFunc_xy(src_v, "src_v"),
    _width, _height
  ), width, height);

  printf("save to %s\n", argv[5]);
  save_image(out, argv[5]);
  return 0;
}

int jit_convert_from_yuv_444(char **argv) {
  int32_t width = 320;
  int32_t height = 240;
  int32_t ysize = 320 * 240;
  int32_t uvsize = 320 * 240;

  FILE *const y = fopen(argv[2], "rb");
  if(y == nullptr) {
    return 1;
  }
  uint8_t *data_y = (uint8_t *) calloc(ysize, sizeof(uint8_t));
  fread(data_y, 1, ysize, y);
  fclose(y);

  FILE *const u = fopen(argv[3], "rb");
  if(u == nullptr) {
    return 1;
  }
  uint8_t *data_u = (uint8_t *) calloc(uvsize, sizeof(uint8_t));
  fread(data_u, 1, uvsize, u);
  fclose(u);

  FILE *const v = fopen(argv[4], "rb");
  if(v == nullptr) {
    return 1;
  }
  uint8_t *data_v = (uint8_t *) calloc(uvsize, sizeof(uint8_t));
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

  Buffer<uint8_t> out = jit_realize_uint8_bounds(convert_from_yuv_444_fn(
    wrapFunc_xy(src_y, "src_y"),
    wrapFunc_xy(src_u, "src_u"),
    wrapFunc_xy(src_v, "src_v"),
    _width, _height
  ), width, height);

  printf("save to %s\n", argv[5]);
  save_image(out, argv[5]);
  return 0;
}

int jit_convert_to_yuv_444(char **argv) {
  Buffer<uint8_t> buf_src = load_and_convert_image(argv[2]);

  Param<int32_t> _width{"width", buf_src.get()->width()};
  Param<int32_t> _height{"height", buf_src.get()->height()};

  int32_t width = buf_src.get()->width();
  int32_t height = buf_src.get()->height();

  Func fn = convert_to_yuv_444_fn(
    wrapFunc(buf_src, "buf_src"),
    _width, _height
  );

  fn.compile_jit(get_jit_target_from_environment());

  int32_t y_width = buf_src.get()->width();
  int32_t uv_width = buf_src.get()->width();
  int32_t y_height = buf_src.get()->height();
  int32_t uv_height = buf_src.get()->height();

  printf("realize(uint8_t) %s...\n", fn.name().c_str());
  Buffer<uint8_t> out = fn.realize({
    y_width,
    y_height + uv_height + uv_height,
    2
  });
  uint8_t *yuv = out.data();

  int32_t ysize = y_width * y_height;
  int32_t uvsize = uv_width * uv_height;
  uint8_t *data_y = (uint8_t *) calloc(ysize, sizeof(uint8_t));
  if(data_y == nullptr) {
    return 1;
  }
  uint8_t *data_u = (uint8_t *) calloc(uvsize, sizeof(uint8_t));
  if(data_u == nullptr) {
    return 1;
  }
  uint8_t *data_v = (uint8_t *) calloc(uvsize, sizeof(uint8_t));
  if(data_v == nullptr) {
    return 1;
  }

  int i = 0;
  int ypos = 0;
  for(int y = 0; y < y_height; y += 1) {
    for(int x = 0; x < y_width; x += 1) {
      //data_y[ypos] = yuv[(y * y_height) + x];
      data_y[ypos] = yuv[i];
      ypos += 1;
      i += 1;
    }
  }
  int upos = 0;
  for(int y = 0; y < uv_height; y += 1) {
    for(int x = 0; x < uv_width; x += 1) {
      //data_u[upos] = yuv[(y * y_height) + x];
      data_u[upos] = yuv[i];
      upos += 1;
      i += 1;
    }
  }
  int vpos = 0;
  for(int y = 0; y < uv_height; y += 1) {
    for(int x = 0; x < uv_width; x += 1) {
      //data_v[vpos] = yuv[(y * y_height) + x];
      data_v[vpos] = yuv[i];
      vpos += 1;
      i += 1;
    }
  }

  FILE *const y = fopen(argv[3], "wb");
  if(y == nullptr) {
    return 1;
  }
  fwrite(data_y, 1, ysize, y);
  fclose(y);

  FILE *const u = fopen(argv[4], "wb");
  if(u == nullptr) {
    return 1;
  }
  fwrite(data_u, 1, uvsize, u);
  fclose(u);

  FILE *const v = fopen(argv[5], "wb");
  if(v == nullptr) {
    return 1;
  }
  fwrite(data_v, 1, uvsize, v);
  fclose(v);

  printf("save to %s\n", argv[3]);
  printf("save to %s\n", argv[4]);
  printf("save to %s\n", argv[5]);

  if(0){
    Buffer<uint8_t> src_y = Buffer<uint8_t>::make_interleaved(data_y, y_width, y_height, 1);
    Buffer<uint8_t> src_u = Buffer<uint8_t>::make_interleaved(data_u, y_width, uv_height, 1);
    Buffer<uint8_t> src_v = Buffer<uint8_t>::make_interleaved(data_v, y_width, uv_height, 1);
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

    Buffer<uint8_t> out2 = jit_realize_uint8_bounds(convert_from_yuv_444_fn(
      wrapFunc_xy(src_y, "src_y"),
      wrapFunc_xy(src_u, "src_u"),
      wrapFunc_xy(src_v, "src_v"),
      _width, _height
    ), width, height);

    save_image(out2, "/tmp/test.png");
    printf("save to %s\n", "/tmp/test.png");
  }
  return 0;
}

int jit_rotate0(char **argv) {
  Buffer<uint8_t> buf_src = load_and_convert_image(argv[2]);

  Param<int32_t> width{"width", buf_src.get()->width()};
  Param<int32_t> height{"height", buf_src.get()->height()};

  Func fn = rotate0_fn(wrapFunc(buf_src, "buf_src"), width, height);
  Buffer<uint8_t> out = jit_realize_uint8_bounds(fn, width.get(), height.get());

  printf("save to %s\n", argv[3]);
  save_image(out, argv[3]);
  return 0;
}

int jit_rotate180(char **argv) {
  Buffer<uint8_t> buf_src = load_and_convert_image(argv[2]);

  Param<int32_t> width{"width", buf_src.get()->width()};
  Param<int32_t> height{"height", buf_src.get()->height()};

  Func fn = rotate180_fn(wrapFunc(buf_src, "buf_src"), width, height);
  Buffer<uint8_t> out = jit_realize_uint8_bounds(fn, width.get(), height.get());

  printf("save to %s\n", argv[3]);
  save_image(out, argv[3]);
  return 0;
}

int jit_rotate90(char **argv) {
  Buffer<uint8_t> buf_src = load_and_convert_image(argv[2]);

  Param<int32_t> width{"width", buf_src.get()->width()};
  Param<int32_t> height{"height", buf_src.get()->height()};

  Func fn = rotate90_fn(wrapFunc(buf_src, "buf_src"), width, height);
  Buffer<uint8_t> out = jit_realize_uint8_bounds(fn, height.get(), width.get());

  printf("save to %s\n", argv[3]);
  save_image(out, argv[3]);
  return 0;
}

int jit_rotate270(char **argv) {
  Buffer<uint8_t> buf_src = load_and_convert_image(argv[2]);

  Param<int32_t> width{"width", buf_src.get()->width()};
  Param<int32_t> height{"height", buf_src.get()->height()};

  Func fn = rotate270_fn(wrapFunc(buf_src, "buf_src"), width, height);
  Buffer<uint8_t> out = jit_realize_uint8_bounds(fn, width.get(), height.get());

  printf("save to %s\n", argv[3]);
  save_image(out, argv[3]);
  return 0;
}

int jit_crop(char **argv) {
  Buffer<uint8_t> buf_src = load_and_convert_image(argv[2]);

  Param<int32_t> width{"width", buf_src.get()->width()};
  Param<int32_t> height{"height", buf_src.get()->height()};
  Param<int32_t> px{"px", (int32_t) std::stoi(argv[3])};
  Param<int32_t> py{"py", (int32_t) std::stoi(argv[4])};
  Param<int32_t> crop_width{"crop_width", (int32_t) std::stoi(argv[5])};
  Param<int32_t> crop_height{"crop_height", (int32_t) std::stoi(argv[6])};

  Func fn = crop_fn(
    wrapFunc(buf_src, "buf_src"), width, height,
    px, py, crop_width, crop_height
  );
  Buffer<uint8_t> out = jit_realize_uint8_bounds(fn, crop_width.get(), crop_height.get());

  printf("save to %s\n", argv[7]);
  save_image(out, argv[7]);
  return 0;
}

int jit_scale(char **argv) {
  Buffer<uint8_t> buf_src = load_and_convert_image(argv[2]);

  Param<int32_t> width{"width", buf_src.get()->width()};
  Param<int32_t> height{"height", buf_src.get()->height()};
  Param<int32_t> scale_width{"scale_width", (int32_t) std::stoi(argv[3])};
  Param<int32_t> scale_height{"scale_height", (int32_t) std::stoi(argv[4])};

  Func fn = scale_fn(
    wrapFunc(buf_src, "buf_src"), width, height,
    scale_width, scale_height
  );
  Buffer<uint8_t> out = jit_realize_uint8_bounds(fn, scale_width.get(), scale_height.get());

  printf("save to %s\n", argv[5]);
  save_image(out, argv[5]);
  return 0;
}

int jit_scale_box(char **argv) {
  Buffer<uint8_t> buf_src = load_and_convert_image(argv[2]);

  Param<int32_t> width{"width", buf_src.get()->width()};
  Param<int32_t> height{"height", buf_src.get()->height()};
  Param<int32_t> scale_width{"scale_width", (int32_t) std::stoi(argv[3])};
  Param<int32_t> scale_height{"scale_height", (int32_t) std::stoi(argv[4])};

  Func fn = scale_box_fn(
    wrapFunc(buf_src, "buf_src"), width, height,
    scale_width, scale_height
  );
  Buffer<uint8_t> out = jit_realize_uint8_bounds(fn, scale_width.get(), scale_height.get());

  printf("save to %s\n", argv[5]);
  save_image(out, argv[5]);
  return 0;
}

int jit_scale_linear(char **argv) {
  Buffer<uint8_t> buf_src = load_and_convert_image(argv[2]);

  Param<int32_t> width{"width", buf_src.get()->width()};
  Param<int32_t> height{"height", buf_src.get()->height()};
  Param<int32_t> scale_width{"scale_width", (int32_t) std::stoi(argv[3])};
  Param<int32_t> scale_height{"scale_height", (int32_t) std::stoi(argv[4])};

  Func fn = scale_linear_fn(
    wrapFunc(buf_src, "buf_src"), width, height,
    scale_width, scale_height
  );
  Buffer<uint8_t> out = jit_realize_uint8_bounds(fn, scale_width.get(), scale_height.get());

  printf("save to %s\n", argv[5]);
  save_image(out, argv[5]);
  return 0;
}

int jit_scale_gaussian(char **argv) {
  Buffer<uint8_t> buf_src = load_and_convert_image(argv[2]);

  Param<int32_t> width{"width", buf_src.get()->width()};
  Param<int32_t> height{"height", buf_src.get()->height()};
  Param<int32_t> scale_width{"scale_width", (int32_t) std::stoi(argv[3])};
  Param<int32_t> scale_height{"scale_height", (int32_t) std::stoi(argv[4])};

  Func fn = scale_gaussian_fn(
    wrapFunc(buf_src, "buf_src"), width, height,
    scale_width, scale_height
  );
  Buffer<uint8_t> out = jit_realize_uint8_bounds(fn, scale_width.get(), scale_height.get());

  printf("save to %s\n", argv[5]);
  save_image(out, argv[5]);
  return 0;
}

int jit_blend_normal(char **argv) {
  Buffer<uint8_t> buf_src0 = load_and_convert_image(argv[2]);
  Buffer<uint8_t> buf_src1 = load_and_convert_image(argv[3]);

  Param<int32_t> width0{"width0", buf_src0.get()->width()};
  Param<int32_t> height0{"height0", buf_src0.get()->height()};
  Param<int32_t> width1{"width1", buf_src1.get()->width()};
  Param<int32_t> height1{"height1", buf_src1.get()->height()};
  Param<int32_t> px{"px", (buf_src0.get()->width() / 2) - (buf_src1.get()->width() / 2)};
  Param<int32_t> py{"py", (buf_src0.get()->height()/ 2) - (buf_src1.get()->height()/ 2)};

  Func fn = blend_normal_fn(
    wrapFunc(buf_src0, "buf_src0"), width0, height0,
    wrapFunc(buf_src1, "buf_src1"), width1, height1,
    px, py
  );
  Buffer<uint8_t> out = jit_realize_uint8_bounds(fn, width0.get(), height0.get());

  printf("save to %s\n", argv[4]);
  save_image(out, argv[4]);
  return 0;
}

int jit_blend_sub(char **argv) {
  Buffer<uint8_t> buf_src0 = load_and_convert_image(argv[2]);
  Buffer<uint8_t> buf_src1 = load_and_convert_image(argv[3]);

  Param<int32_t> width0{"width0", buf_src0.get()->width()};
  Param<int32_t> height0{"height0", buf_src0.get()->height()};
  Param<int32_t> width1{"width1", buf_src1.get()->width()};
  Param<int32_t> height1{"height1", buf_src1.get()->height()};
  Param<int32_t> px{"px", (buf_src0.get()->width() / 2) - (buf_src1.get()->width() / 2)};
  Param<int32_t> py{"py", (buf_src0.get()->height()/ 2) - (buf_src1.get()->height()/ 2)};

  Func fn = blend_sub_fn(
    wrapFunc(buf_src0, "buf_src0"), width0, height0,
    wrapFunc(buf_src1, "buf_src1"), width1, height1,
    px, py
  );
  Buffer<uint8_t> out = jit_realize_uint8_bounds(fn, width0.get(), height0.get());

  printf("save to %s\n", argv[4]);
  save_image(out, argv[4]);
  return 0;
}

int jit_blend_add(char **argv) {
  Buffer<uint8_t> buf_src0 = load_and_convert_image(argv[2]);
  Buffer<uint8_t> buf_src1 = load_and_convert_image(argv[3]);

  Param<int32_t> width0{"width0", buf_src0.get()->width()};
  Param<int32_t> height0{"height0", buf_src0.get()->height()};
  Param<int32_t> width1{"width1", buf_src1.get()->width()};
  Param<int32_t> height1{"height1", buf_src1.get()->height()};
  Param<int32_t> px{"px", (buf_src0.get()->width() / 2) - (buf_src1.get()->width() / 2)};
  Param<int32_t> py{"py", (buf_src0.get()->height()/ 2) - (buf_src1.get()->height()/ 2)};

  Func fn = blend_add_fn(
    wrapFunc(buf_src0, "buf_src0"), width0, height0,
    wrapFunc(buf_src1, "buf_src1"), width1, height1,
    px, py
  );
  Buffer<uint8_t> out = jit_realize_uint8_bounds(fn, width0.get(), height0.get());

  printf("save to %s\n", argv[4]);
  save_image(out, argv[4]);
  return 0;
}

int jit_blend_diff(char **argv) {
  Buffer<uint8_t> buf_src0 = load_and_convert_image(argv[2]);
  Buffer<uint8_t> buf_src1 = load_and_convert_image(argv[3]);

  Param<int32_t> width0{"width0", buf_src0.get()->width()};
  Param<int32_t> height0{"height0", buf_src0.get()->height()};
  Param<int32_t> width1{"width1", buf_src1.get()->width()};
  Param<int32_t> height1{"height1", buf_src1.get()->height()};
  Param<int32_t> px{"px", (buf_src0.get()->width() / 2) - (buf_src1.get()->width() / 2)};
  Param<int32_t> py{"py", (buf_src0.get()->height()/ 2) - (buf_src1.get()->height()/ 2)};

  Func fn = blend_diff_fn(
    wrapFunc(buf_src0, "buf_src0"), width0, height0,
    wrapFunc(buf_src1, "buf_src1"), width1, height1,
    px, py
  );
  Buffer<uint8_t> out = jit_realize_uint8_bounds(fn, width0.get(), height0.get());

  printf("save to %s\n", argv[4]);
  save_image(out, argv[4]);
  return 0;
}

int jit_erosion(char **argv) {
  Buffer<uint8_t> buf_src = load_and_convert_image(argv[2]);

  Param<int32_t> width{"width", buf_src.get()->width()};
  Param<int32_t> height{"height", buf_src.get()->height()};
  Param<int32_t> size{"size", (int32_t) std::stoi(argv[3])};

  Buffer<uint8_t> out = jit_realize_uint8(erosion_fn(
    wrapFunc(buf_src, "buf_src"), width, height, size
  ), buf_src);
    
  printf("save to %s\n", argv[4]);
  save_image(out, argv[4]);
  return 0;
}

int jit_dilation(char **argv) {
  Buffer<uint8_t> buf_src = load_and_convert_image(argv[2]);

  Param<int32_t> width{"width", buf_src.get()->width()};
  Param<int32_t> height{"height", buf_src.get()->height()};
  Param<int32_t> size{"size", (int32_t) std::stoi(argv[3])};

  Buffer<uint8_t> out = jit_realize_uint8(dilation_fn(
    wrapFunc(buf_src, "buf_src"), width, height, size
  ), buf_src);
    
  printf("save to %s\n", argv[4]);
  save_image(out, argv[4]);
  return 0;
}

int jit_morphology_open(char **argv) {
  Buffer<uint8_t> buf_src = load_and_convert_image(argv[2]);

  Param<int32_t> width{"width", buf_src.get()->width()};
  Param<int32_t> height{"height", buf_src.get()->height()};
  Param<int32_t> size{"size", (int32_t) std::stoi(argv[3])};

  Buffer<uint8_t> out = jit_realize_uint8(morphology_open_fn(
    wrapFunc(buf_src, "buf_src"), width, height,
    size
  ), buf_src);
    
  printf("save to %s\n", argv[4]);
  save_image(out, argv[4]);
  return 0;
}

int jit_morphology_close(char **argv) {
  Buffer<uint8_t> buf_src = load_and_convert_image(argv[2]);

  Param<int32_t> width{"width", buf_src.get()->width()};
  Param<int32_t> height{"height", buf_src.get()->height()};
  Param<int32_t> size{"size", (int32_t) std::stoi(argv[3])};

  Buffer<uint8_t> out = jit_realize_uint8(morphology_close_fn(
    wrapFunc(buf_src, "buf_src"), width, height,
    size
  ), buf_src);
    
  printf("save to %s\n", argv[4]);
  save_image(out, argv[4]);
  return 0;
}

int jit_morphology_gradient(char **argv) {
  Buffer<uint8_t> buf_src = load_and_convert_image(argv[2]);

  Param<int32_t> width{"width", buf_src.get()->width()};
  Param<int32_t> height{"height", buf_src.get()->height()};
  Param<int32_t> size{"size", (int32_t) std::stoi(argv[3])};

  Buffer<uint8_t> out = jit_realize_uint8(morphology_gradient_fn(
    wrapFunc(buf_src, "buf_src"), width, height,
    size
  ), buf_src);
    
  printf("save to %s\n", argv[4]);
  save_image(out, argv[4]);
  return 0;
}

int jit_grayscale(char **argv) {
  Buffer<uint8_t> buf_src = load_and_convert_image(argv[2]);

  Param<int32_t> width{"width", buf_src.get()->width()};
  Param<int32_t> height{"height", buf_src.get()->height()};

  Buffer<uint8_t> out = jit_realize_uint8(grayscale_fn(
    wrapFunc(buf_src, "buf_src"), width, height
  ), buf_src);
    
  printf("save to %s\n", argv[3]);
  save_image(out, argv[3]);
  return 0;
}

int jit_invert(char **argv) {
  Buffer<uint8_t> buf_src = load_and_convert_image(argv[2]);

  Param<int32_t> width{"width", buf_src.get()->width()};
  Param<int32_t> height{"height", buf_src.get()->height()};

  Buffer<uint8_t> out = jit_realize_uint8(invert_fn(
    wrapFunc(buf_src, "buf_src"), width, height
  ), buf_src);

  printf("save to %s\n", argv[3]);
  save_image(out, argv[3]);
  return 0;
}

int jit_brightness(char **argv) {
  Buffer<uint8_t> buf_src = load_and_convert_image(argv[2]);

  Param<int32_t> width{"width", buf_src.get()->width()};
  Param<int32_t> height{"height", buf_src.get()->height()};
  Param<float>   factor{"factor", std::stof(argv[3])};

  Buffer<uint8_t> out = jit_realize_uint8(brightness_fn(
    wrapFunc(buf_src, "buf_src"), width, height, factor
  ), buf_src);

  printf("save to %s\n", argv[4]);
  save_image(out, argv[4]);
  return 0;
}

int jit_gammacorrection(char **argv) {
  Buffer<uint8_t> buf_src = load_and_convert_image(argv[2]);

  Param<int32_t> width{"width", buf_src.get()->width()};
  Param<int32_t> height{"height", buf_src.get()->height()};
  Param<float>   factor{"factor", std::stof(argv[3])};

  Buffer<uint8_t> out = jit_realize_uint8(gammacorrection_fn(
    wrapFunc(buf_src, "buf_src"), width, height, factor
  ), buf_src);

  printf("save to %s\n", argv[4]);
  save_image(out, argv[4]);
  return 0;
}

int jit_contrast(char **argv) {
  Buffer<uint8_t> buf_src = load_and_convert_image(argv[2]);

  Param<int32_t> width{"width", buf_src.get()->width()};
  Param<int32_t> height{"height", buf_src.get()->height()};
  Param<float>   factor{"factor", std::stof(argv[3])};

  Buffer<uint8_t> out = jit_realize_uint8(contrast_fn(
    wrapFunc(buf_src, "buf_src"), width, height, factor
  ), buf_src);

  printf("save to %s\n", argv[4]);
  save_image(out, argv[4]);
  return 0;
}

int jit_boxblur(char **argv) {
  Buffer<uint8_t> buf_src = load_and_convert_image(argv[2]);

  Param<int32_t> width{"width", buf_src.get()->width()};
  Param<int32_t> height{"height", buf_src.get()->height()};
  Param<uint8_t> size{"size", (uint8_t) std::stoi(argv[3])};

  Buffer<uint8_t> out = jit_realize_uint8(boxblur_fn(
    wrapFunc(buf_src, "buf_src"), width, height, size
  ), buf_src);

  printf("save to %s\n", argv[4]);
  save_image(out, argv[4]);
  return 0;
}

int jit_gaussianblur(char **argv) {
  Buffer<uint8_t> buf_src = load_and_convert_image(argv[2]);

  Param<int32_t> width{"width", buf_src.get()->width()};
  Param<int32_t> height{"height", buf_src.get()->height()};
  Param<float> sigma{"sigma", std::stof(argv[3])};

  Buffer<uint8_t> out = jit_realize_uint8(gaussianblur_fn(
    wrapFunc(buf_src, "buf_src"), width, height, sigma
  ), buf_src);

  printf("save to %s\n", argv[4]);
  save_image(out, argv[4]);
  return 0;
}

int jit_edge(char **argv) {
  Buffer<uint8_t> buf_src = load_and_convert_image(argv[2]);

  Param<int32_t> width{"width", buf_src.get()->width()};
  Param<int32_t> height{"height", buf_src.get()->height()};

  Buffer<uint8_t> out = jit_realize_uint8(edge_fn(
    wrapFunc(buf_src, "buf_src"), width, height
  ), buf_src);

  printf("save to %s\n", argv[3]);
  save_image(out, argv[3]);
  return 0;
}

int jit_sobel(char **argv) {
  Buffer<uint8_t> buf_src = load_and_convert_image(argv[2]);

  Param<int32_t> width{"width", buf_src.get()->width()};
  Param<int32_t> height{"height", buf_src.get()->height()};

  Buffer<uint8_t> out = jit_realize_uint8(sobel_fn(
    wrapFunc(buf_src, "buf_src"), width, height
  ), buf_src);

  printf("save to %s\n", argv[3]);
  save_image(out, argv[3]);
  return 0;
}

int jit_canny(char **argv) {
  Buffer<uint8_t> buf_src = load_and_convert_image(argv[2]);

  Param<int32_t> width{"width", buf_src.get()->width()};
  Param<int32_t> height{"height", buf_src.get()->height()};
  Param<int32_t> threshold_max{"threshold_max", std::stoi(argv[3])};
  Param<int32_t> threshold_min{"threshold_min", std::stoi(argv[4])};

  Buffer<uint8_t> out = jit_realize_uint8(canny_fn(
    wrapFunc(buf_src, "buf_src"), width, height,
    threshold_max, threshold_min
  ), buf_src);

  printf("save to %s\n", argv[5]);
  save_image(out, argv[5]);
  return 0;
}

int jit_canny_dilate(char **argv) {
  Buffer<uint8_t> buf_src = load_and_convert_image(argv[2]);

  Param<int32_t> width{"width", buf_src.get()->width()};
  Param<int32_t> height{"height", buf_src.get()->height()};
  Param<int32_t> threshold_max{"threshold_max", std::stoi(argv[3])};
  Param<int32_t> threshold_min{"threshold_min", std::stoi(argv[4])};
  Param<int32_t> dilate{"dilate", std::stoi(argv[5])};

  Buffer<uint8_t> out = jit_realize_uint8(canny_dilate_fn(
    wrapFunc(buf_src, "buf_src"), width, height,
    threshold_max, threshold_min,
    dilate
  ), buf_src);

  printf("save to %s\n", argv[6]);
  save_image(out, argv[6]);
  return 0;
}

int jit_canny_morphology_open(char **argv) {
  Buffer<uint8_t> buf_src = load_and_convert_image(argv[2]);

  Param<int32_t> width{"width", buf_src.get()->width()};
  Param<int32_t> height{"height", buf_src.get()->height()};
  Param<int32_t> threshold_max{"threshold_max", std::stoi(argv[3])};
  Param<int32_t> threshold_min{"threshold_min", std::stoi(argv[4])};
  Param<int32_t> morphology_size{"morphology_size", std::stoi(argv[5])};
  Param<int32_t> dilate{"dilate", std::stoi(argv[6])};

  Buffer<uint8_t> out = jit_realize_uint8(canny_morphology_open_fn(
    wrapFunc(buf_src, "buf_src"), width, height,
    threshold_max, threshold_min,
    morphology_size,
    dilate
  ), buf_src);

  printf("save to %s\n", argv[7]);
  save_image(out, argv[7]);
  return 0;
}

int jit_canny_morphology_close(char **argv) {
  Buffer<uint8_t> buf_src = load_and_convert_image(argv[2]);

  Param<int32_t> width{"width", buf_src.get()->width()};
  Param<int32_t> height{"height", buf_src.get()->height()};
  Param<int32_t> threshold_max{"threshold_max", std::stoi(argv[3])};
  Param<int32_t> threshold_min{"threshold_min", std::stoi(argv[4])};
  Param<int32_t> morphology_size{"morphology_size", std::stoi(argv[5])};
  Param<int32_t> dilate{"dilate", std::stoi(argv[6])};

  Buffer<uint8_t> out = jit_realize_uint8(canny_morphology_close_fn(
    wrapFunc(buf_src, "buf_src"), width, height,
    threshold_max, threshold_min,
    morphology_size,
    dilate
  ), buf_src);

  printf("save to %s\n", argv[7]);
  save_image(out, argv[7]);
  return 0;
}

int jit_emboss(char **argv) {
  Buffer<uint8_t> buf_src = load_and_convert_image(argv[2]);

  Param<int32_t> width{"width", buf_src.get()->width()};
  Param<int32_t> height{"height", buf_src.get()->height()};

  Buffer<uint8_t> out = jit_realize_uint8(emboss_fn(
    wrapFunc(buf_src, "buf_src"), width, height
  ), buf_src);

  printf("save to %s\n", argv[3]);
  save_image(out, argv[3]);
  return 0;
}

int jit_laplacian(char **argv) {
  Buffer<uint8_t> buf_src = load_and_convert_image(argv[2]);

  Param<int32_t> width{"width", buf_src.get()->width()};
  Param<int32_t> height{"height", buf_src.get()->height()};

  Buffer<uint8_t> out = jit_realize_uint8(laplacian_fn(
    wrapFunc(buf_src, "buf_src"), width, height
  ), buf_src);

  printf("save to %s\n", argv[3]);
  save_image(out, argv[3]);
  return 0;
}

int jit_highpass(char **argv) {
  Buffer<uint8_t> buf_src = load_and_convert_image(argv[2]);

  Param<int32_t> width{"width", buf_src.get()->width()};
  Param<int32_t> height{"height", buf_src.get()->height()};

  Buffer<uint8_t> out = jit_realize_uint8(highpass_fn(
    wrapFunc(buf_src, "buf_src"), width, height
  ), buf_src);

  printf("save to %s\n", argv[3]);
  save_image(out, argv[3]);
  return 0;
}

int jit_gradient(char **argv) {
  Buffer<uint8_t> buf_src = load_and_convert_image(argv[2]);

  Param<int32_t> width{"width", buf_src.get()->width()};
  Param<int32_t> height{"height", buf_src.get()->height()};

  Buffer<uint8_t> out = jit_realize_uint8(gradient_fn(
    wrapFunc(buf_src, "buf_src"), width, height
  ), buf_src);

  printf("save to %s\n", argv[3]);
  save_image(out, argv[3]);
  return 0;
}

int jit_blockmozaic(char **argv) {
  Buffer<uint8_t> buf_src = load_and_convert_image(argv[2]);

  Param<int32_t> width{"width", buf_src.get()->width()};
  Param<int32_t> height{"height", buf_src.get()->height()};
  Param<int32_t> block{"block", std::stoi(argv[3])};

  Buffer<uint8_t> out = jit_realize_uint8(blockmozaic_fn(
    wrapFunc(buf_src, "buf_src"), width, height, block
  ), buf_src);

  printf("save to %s\n", argv[4]);
  save_image(out, argv[4]);
  return 0;
}

int jit_match_template_sad(char **argv) {
  Buffer<uint8_t> buf_src = load_and_convert_image(argv[2]);
  Param<int32_t> width{"width", buf_src.get()->width()};
  Param<int32_t> height{"height", buf_src.get()->height()};

  Buffer<uint8_t> buf_tpl = load_and_convert_image(argv[3]);
  Param<int32_t> tpl_width{"tpl_width", buf_tpl.get()->width()};
  Param<int32_t> tpl_height{"tpl_height", buf_tpl.get()->height()};

  Buffer<uint16_t> out = jit_realize_uint16(match_template_sad_fn(
    wrapFunc(buf_src, "buf_src"), width, height,
    wrapFunc(buf_tpl, "buf_tpl"), tpl_width, tpl_height
  ), buf_src);

  uint16_t *data = out.data();
  int32_t w = out.extent(0);
  int32_t h = out.extent(1);

  std::vector<mt_score_uint32> vec;
  printf("output w:%d, h:%d\n", w, h);
  for(int y = 0; y < h; y += 1) {
    for(int x = 0; x < w; x += 1) {
      int idx = (y * w) + x;
      uint16_t score = data[idx];
      if(1000 < score) {
        continue; // threshold
      }

      vec.push_back({ x, y, score });
    }
  }
  std::sort(vec.begin(), vec.end(), [](const mt_score_uint32 a, const mt_score_uint32 b) {
    return a.score < b.score; // asc. best match is nearst to 0
  });

  int lim = 0;
  for(const mt_score_uint32 e : vec) {
    if(10 < lim) { // top10
      break;
    }
    printf("x:%d y:%d score:%d\n", e.x, e.y, e.score);
    lim += 1;
  }
  return 0;
}

int jit_match_template_ssd(char **argv) {
  Buffer<uint8_t> buf_src = load_and_convert_image(argv[2]);
  Param<int32_t> width{"width", buf_src.get()->width()};
  Param<int32_t> height{"height", buf_src.get()->height()};

  Buffer<uint8_t> buf_tpl = load_and_convert_image(argv[3]);
  Param<int32_t> tpl_width{"tpl_width", buf_tpl.get()->width()};
  Param<int32_t> tpl_height{"tpl_height", buf_tpl.get()->height()};

  Buffer<int32_t> out = jit_realize_int32(match_template_ssd_fn(
    wrapFunc(buf_src, "buf_src"), width, height,
    wrapFunc(buf_tpl, "buf_tpl"), tpl_width, tpl_height
  ), buf_src);

  int32_t *data = out.data();
  int32_t w = out.extent(0);
  int32_t h = out.extent(1);

  std::vector<mt_score_uint32> vec;
  printf("output w:%d, h:%d\n", w, h);
  for(int y = 0; y < h; y += 1) {
    for(int x = 0; x < w; x += 1) {
      int idx = (y * w) + x;
      uint16_t score = data[idx];
      if(1000 < score) {
        continue; // threshold
      }

      vec.push_back({ x, y, score });
    }
  }
  std::sort(vec.begin(), vec.end(), [](const mt_score_uint32 a, const mt_score_uint32 b) {
    return a.score < b.score; // asc. best match is nearest to 0
  });

  int lim = 0;
  for(const mt_score_uint32 e : vec) {
    if(10 < lim) { // top10
      break;
    }
    printf("x:%d y:%d score:%d\n", e.x, e.y, e.score);
    lim += 1;
  }
  return 0;
}

int jit_match_template_ncc(char **argv) {
  Buffer<uint8_t> buf_src = load_and_convert_image(argv[2]);
  Param<int32_t> width{"width", buf_src.get()->width()};
  Param<int32_t> height{"height", buf_src.get()->height()};

  Buffer<uint8_t> buf_tpl = load_and_convert_image(argv[3]);
  Param<int32_t> tpl_width{"tpl_width", buf_tpl.get()->width()};
  Param<int32_t> tpl_height{"tpl_height", buf_tpl.get()->height()};

  Buffer<double> out = jit_realize_double(match_template_ncc_fn(
    wrapFunc(buf_src, "buf_src"), width, height,
    wrapFunc(buf_tpl, "buf_tpl"), tpl_width, tpl_height
  ), buf_src);

  double *data = out.data();
  int32_t w = out.extent(0);
  int32_t h = out.extent(1);

  std::vector<mt_score_float> vec;
  printf("output w:%d, h:%d\n", w, h);
  for(int y = 0; y < h; y += 1) {
    for(int x = 0; x < w; x += 1) {
      int idx = (y * w) + x;
      double score = data[idx];
      if(score < 0.1) {
        continue; // threshold
      }

      vec.push_back({ x, y, score });
    }
  }
  std::sort(vec.begin(), vec.end(), [](const mt_score_float a, const mt_score_float b) {
    return a.score > b.score; // desc. best match is nearest to 1.0
  });

  int lim = 0;
  for(const mt_score_float e : vec) {
    if(10 < lim) { // top10
      break;
    }
    printf("x:%d y:%d score:%2.5f\n", e.x, e.y, e.score);
    lim += 1;
  }
  return 0;
}

int jit_prepared_match_template_ncc(char **argv) {
  Buffer<uint8_t> buf_src = load_and_convert_image(argv[2]);
  Param<int32_t> width{"width", buf_src.get()->width()};
  Param<int32_t> height{"height", buf_src.get()->height()};

  Buffer<uint8_t> buf_tpl = load_and_convert_image(argv[3]);
  Param<int32_t> tpl_width{"tpl_width", buf_tpl.get()->width()};
  Param<int32_t> tpl_height{"tpl_height", buf_tpl.get()->height()};

  Func fn = prepare_ncc_template_fn(wrapFunc(buf_tpl, "tpl"), tpl_width, tpl_height);
  Realization r = fn.realize({buf_tpl.get()->width(), buf_tpl.get()->height(), 4});
  Buffer<float> buf_tpl_val = r[0];
  Buffer<float> buf_tpl_sum = r[1];

  Buffer<double> out = jit_realize_double(prepared_match_template_ncc_fn(
    wrapFunc(buf_src, "buf_src"), width, height,
    wrapFunc_xy(buf_tpl_val, "val"), wrapFunc_xy(buf_tpl_sum, "sum"),
    tpl_width, tpl_height
  ), buf_src);

  double *data = out.data();
  int32_t w = out.extent(0);
  int32_t h = out.extent(1);

  std::vector<mt_score_float> vec;
  printf("output w:%d, h:%d\n", w, h);
  for(int y = 0; y < h; y += 1) {
    for(int x = 0; x < w; x += 1) {
      int idx = (y * w) + x;
      double score = data[idx];
      if(score < 0.1) {
        continue; // threshold
      }

      vec.push_back({ x, y, score });
    }
  }
  std::sort(vec.begin(), vec.end(), [](const mt_score_float a, const mt_score_float b) {
    return a.score > b.score; // desc. best match is nearest to 1.0
  });

  int lim = 0;
  for(const mt_score_float e : vec) {
    if(10 < lim) { // top10
      break;
    }
    printf("x:%d y:%d score:%2.5f\n", e.x, e.y, e.score);
    lim += 1;
  }
  return 0;
}

int jit_match_template_zncc(char **argv) {
  Buffer<uint8_t> buf_src = load_and_convert_image(argv[2]);
  Param<int32_t> width{"width", buf_src.get()->width()};
  Param<int32_t> height{"height", buf_src.get()->height()};

  Buffer<uint8_t> buf_tpl = load_and_convert_image(argv[3]);
  Param<int32_t> tpl_width{"tpl_width", buf_tpl.get()->width()};
  Param<int32_t> tpl_height{"tpl_height", buf_tpl.get()->height()};

  Buffer<double> out = jit_realize_double(match_template_zncc_fn(
    wrapFunc(buf_src, "buf_src"), width, height,
    wrapFunc(buf_tpl, "buf_tpl"), tpl_width, tpl_height
  ), buf_src);

  double *data = out.data();
  int32_t w = out.extent(0);
  int32_t h = out.extent(1);

  std::vector<mt_score_float> vec;
  printf("output w:%d, h:%d\n", w, h);
  for(int y = 0; y < h; y += 1) {
    for(int x = 0; x < w; x += 1) {
      int idx = (y * w) + x;
      double score = data[idx];
      if(score < 0.1) {
        continue; // threshold
      }

      vec.push_back({ x, y, score });
    }
  }
  std::sort(vec.begin(), vec.end(), [](const mt_score_float a, const mt_score_float b) {
    return a.score > b.score; // desc. best match is nearest to 1.0
  });

  int lim = 0;
  for(const mt_score_float e : vec) {
    if(10 < lim) { // top10
      break;
    }
    printf("x:%d y:%d score:%2.5f\n", e.x, e.y, e.score);
    lim += 1;
  }
  return 0;
}

int jit_prepared_match_template_zncc(char **argv) {
  Buffer<uint8_t> buf_src = load_and_convert_image(argv[2]);
  Param<int32_t> width{"width", buf_src.get()->width()};
  Param<int32_t> height{"height", buf_src.get()->height()};

  Buffer<uint8_t> buf_tpl = load_and_convert_image(argv[3]);
  Param<int32_t> tpl_width{"tpl_width", buf_tpl.get()->width()};
  Param<int32_t> tpl_height{"tpl_height", buf_tpl.get()->height()};

  Func fn = prepare_zncc_template_fn(wrapFunc(buf_tpl, "tpl"), tpl_width, tpl_height);
  Realization r = fn.realize({buf_tpl.get()->width(), buf_tpl.get()->height(), 4});
  Buffer<float> buf_tpl_val = r[0];
  Buffer<float> buf_tpl_stddev = r[1];

  Buffer<double> out = jit_realize_double(prepared_match_template_zncc_fn(
    wrapFunc(buf_src, "buf_src"), width, height,
    wrapFunc_xy(buf_tpl_val, "val"), wrapFunc_xy(buf_tpl_stddev, "stddev"),
    tpl_width, tpl_height
  ), buf_src);

  double *data = out.data();
  int32_t w = out.extent(0);
  int32_t h = out.extent(1);

  std::vector<mt_score_float> vec;
  printf("output w:%d, h:%d\n", w, h);
  for(int y = 0; y < h; y += 1) {
    for(int x = 0; x < w; x += 1) {
      int idx = (y * w) + x;
      double score = data[idx];
      if(score < 0.1) {
        continue; // threshold
      }

      vec.push_back({ x, y, score });
    }
  }
  std::sort(vec.begin(), vec.end(), [](const mt_score_float a, const mt_score_float b) {
    return a.score > b.score; // desc. best match is nearest to 1.0
  });

  int lim = 0;
  for(const mt_score_float e : vec) {
    if(10 < lim) { // top10
      break;
    }
    printf("x:%d y:%d score:%2.5f\n", e.x, e.y, e.score);
    lim += 1;
  }
  return 0;
}

int jit_run(char **argv) {
  if(strcmp(argv[1], "cloneimg") == 0) {
    return jit_cloneimg(argv);
  } 
  if(strcmp(argv[1], "convertargb") == 0) {
    return jit_convert_from_argb(argv);
  }
  if(strcmp(argv[1], "convertabgr") == 0) {
    return jit_convert_from_abgr(argv);
  }
  if(strcmp(argv[1], "convertbgra") == 0) {
    return jit_convert_from_bgra(argv);
  }
  if(strcmp(argv[1], "convertrabg") == 0) {
    return jit_convert_from_rabg(argv);
  }
  if(strcmp(argv[1], "convert_from_yuv420") == 0) {
    return jit_convert_from_yuv_420(argv);
  }
  if(strcmp(argv[1], "convert_from_yuv444") == 0) {
    return jit_convert_from_yuv_444(argv);
  }
  if(strcmp(argv[1], "convert_to_yuv444") == 0) {
    return jit_convert_to_yuv_444(argv);
  }
  if(strcmp(argv[1], "rotate0") == 0) {
    return jit_rotate0(argv);
  } 
  if(strcmp(argv[1], "rotate90") == 0) {
    return jit_rotate90(argv);
  } 
  if(strcmp(argv[1], "rotate180") == 0) {
    return jit_rotate180(argv);
  } 
  if(strcmp(argv[1], "rotate270") == 0) {
    return jit_rotate270(argv);
  } 
  if(strcmp(argv[1], "crop") == 0) {
    return jit_crop(argv);
  }
  if(strcmp(argv[1], "scale") == 0) {
    return jit_scale(argv);
  }
  if(strcmp(argv[1], "scale_box") == 0) {
    return jit_scale_box(argv);
  }
  if(strcmp(argv[1], "scale_linear") == 0) {
    return jit_scale_linear(argv);
  }
  if(strcmp(argv[1], "scale_gaussian") == 0) {
    return jit_scale_gaussian(argv);
  }
  if(strcmp(argv[1], "blend_normal") == 0) {
    return jit_blend_normal(argv);
  }
  if(strcmp(argv[1], "blend_sub") == 0) {
    return jit_blend_sub(argv);
  }
  if(strcmp(argv[1], "blend_add") == 0) {
    return jit_blend_add(argv);
  }
  if(strcmp(argv[1], "blend_diff") == 0) {
    return jit_blend_diff(argv);
  }
  if(strcmp(argv[1], "erosion") == 0) {
    return jit_erosion(argv);
  }
  if(strcmp(argv[1], "dilation") == 0) {
    return jit_dilation(argv);
  }
  if(strcmp(argv[1], "morphology_open") == 0) {
    return jit_morphology_open(argv);
  }
  if(strcmp(argv[1], "morphology_close") == 0) {
    return jit_morphology_close(argv);
  }
  if(strcmp(argv[1], "morphology_gradient") == 0) {
    return jit_morphology_gradient(argv);
  }
  if(strcmp(argv[1], "grayscale") == 0) {
    return jit_grayscale(argv);
  }
  if(strcmp(argv[1], "invert") == 0) {
    return jit_invert(argv);
  }
  if(strcmp(argv[1], "brightness") == 0) {
    return jit_brightness(argv);
  }
  if(strcmp(argv[1], "gammacorrection") == 0) {
    return jit_gammacorrection(argv);
  }
  if(strcmp(argv[1], "contrast") == 0) {
    return jit_contrast(argv);
  }
  if(strcmp(argv[1], "boxblur") == 0) {
    return jit_boxblur(argv);
  }
  if(strcmp(argv[1], "gaussianblur") == 0) {
    return jit_gaussianblur(argv);
  }
  if(strcmp(argv[1], "edge") == 0) {
    return jit_edge(argv);
  }
  if(strcmp(argv[1], "sobel") == 0) {
    return jit_sobel(argv);
  }
  if(strcmp(argv[1], "canny") == 0) {
    return jit_canny(argv);
  }
  if(strcmp(argv[1], "canny_dilate") == 0) {
    return jit_canny_dilate(argv);
  }
  if(strcmp(argv[1], "canny_morphology_open") == 0) {
    return jit_canny_morphology_open(argv);
  }
  if(strcmp(argv[1], "canny_morphology_close") == 0) {
    return jit_canny_morphology_close(argv);
  }
  if(strcmp(argv[1], "emboss") == 0) {
    return jit_emboss(argv);
  }
  if(strcmp(argv[1], "laplacian") == 0) {
    return jit_laplacian(argv);
  }
  if(strcmp(argv[1], "highpass") == 0) {
    return jit_highpass(argv);
  }
  if(strcmp(argv[1], "gradient") == 0) {
    return jit_gradient(argv);
  }
  if(strcmp(argv[1], "blockmozaic") == 0) {
    return jit_blockmozaic(argv);
  }
  if(strcmp(argv[1], "match_template_sad") == 0) {
    return jit_match_template_sad(argv);
  }
  if(strcmp(argv[1], "match_template_ssd") == 0) {
    return jit_match_template_ssd(argv);
  }
  if(strcmp(argv[1], "match_template_ncc") == 0) {
    return jit_match_template_ncc(argv);
  }
  if(strcmp(argv[1], "prepared_match_template_ncc") == 0) {
    return jit_prepared_match_template_ncc(argv);
  }
  if(strcmp(argv[1], "match_template_zncc") == 0) {
    return jit_match_template_zncc(argv);
  }
  if(strcmp(argv[1], "prepared_match_template_zncc") == 0) {
    return jit_prepared_match_template_zncc(argv);
  }

  printf("unknown command: %s\n", argv[1]);
  return 1;
}

int main(int argc, char **argv) {
  return jit_run(argv);
}
