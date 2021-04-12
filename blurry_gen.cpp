// +build ignore.

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

void generate_static_link(std::vector<Target::Feature> features, Func fn, std::vector<Argument> args, std::string name) {
  printf("generate %s\n", fn.name().c_str());

  std::string lib_name = "lib" + name;
  std::string name_linux = lib_name + "_linux";
  std::string name_osx = lib_name + "_osx";
  {
    Target target;
    target.os = Target::OSX;
    target.arch = Target::X86;
    target.bits = 64;
    target.set_features(features);
    fn.compile_to_static_library(
      name_osx,
      args,
      name,
      target
    );
  }
  {
    Target target;
    target.os = Target::Linux;
    target.arch = Target::X86;
    target.bits = 64;
    target.set_features(features);
    fn.compile_to_static_library(
      name_linux,
      args,
      name,
      target
    );
  }
}

void generate_static_link_runtime(std::vector<Target::Feature> features, Func fn, std::vector<Argument> args, std::string name) {
  printf("generate runtime\n");

  std::string lib_name = "lib" + name;
  std::string name_linux = lib_name + "_linux";
  std::string name_osx = lib_name + "_osx";
  {
    Target target;
    target.os = Target::OSX;
    target.arch = Target::X86;
    target.bits = 64;
    target.set_features(features);
    fn.compile_to_static_library(
      name_osx,
      args,
      name,
      target.without_feature(Target::Feature::NoRuntime)
    );
  }
  {
    Target target;
    target.os = Target::Linux;
    target.arch = Target::X86;
    target.bits = 64;
    target.set_features(features);
    fn.compile_to_static_library(
      name_linux,
      args,
      name,
      target.without_feature(Target::Feature::NoRuntime)
    );
  }
}

void init_input_rgba(ImageParam in) {
  in.dim(0).set_stride(4);
  in.dim(2).set_stride(1);
  in.dim(2).set_bounds(0, 4);
}

void init_output_rgba(OutputImageParam out) {
  out.dim(0).set_stride(4);
  out.dim(2).set_stride(1);
  out.dim(2).set_bounds(0, 4);
}

void init_output_array(OutputImageParam out, Param<int32_t> width, Param<int32_t> height) {
  out.dim(0).set_stride(1);
  out.dim(1).set_stride(width);
}

void generate_runtime(std::vector<Target::Feature> features) {
  Var x("x");
  Func f = Func("noop");
  f(x) = 0;

  generate_static_link_runtime(features, f, {}, "runtime");
}

int jit_benchmark(Func fn, Buffer<uint8_t> buf_src) {
  fn.compile_jit(get_jit_target_from_environment());

  double result = benchmark(10, 10, [&]() {
    fn.realize({buf_src.get()->width(), buf_src.get()->height(), 3});
  });
  printf("BenchmarkJIT/%-25s: %-3.5fms\n", fn.name().c_str(), result * 1e3);
  return 0;
}

Buffer<uint8_t> jit_realize_uint8_bounds(Func fn, int32_t width, int32_t height) {
  fn.compile_jit(get_jit_target_from_environment());

  printf("realize(uint8_t) %s...\n", fn.name().c_str());
  
  return fn.realize({width, height, 3});
}

Buffer<uint16_t> jit_realize_uint16_bounds(Func fn, int32_t width, int32_t height) {
  fn.compile_jit(get_jit_target_from_environment());

  printf("realize(uint16_t) %s...\n", fn.name().c_str());
  
  return fn.realize({width, height, 3});
}

Buffer<int32_t> jit_realize_int32_bounds(Func fn, int32_t width, int32_t height) {
  fn.compile_jit(get_jit_target_from_environment());

  printf("realize(int32_t) %s...\n", fn.name().c_str());
  
  return fn.realize({width, height, 3});
}

Buffer<double> jit_realize_double_bounds(Func fn, int32_t width, int32_t height) {
  fn.compile_jit(get_jit_target_from_environment());

  printf("realize(double) %s...\n", fn.name().c_str());
  
  return fn.realize({width, height, 3});
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

// {{{ cloneimg
void generate_cloneimg(std::vector<Target::Feature> features) {
  ImageParam src(type_of<uint8_t>(), 3);

  Param<int32_t> width{"width", 1920};
  Param<int32_t> height{"height", 1080};

  init_input_rgba(src);

  Func fn = cloneimg_fn(
    src.in(), width, height
  );

  init_output_rgba(fn.output_buffer());

  generate_static_link(features, fn, {
    src, width, height,
  }, fn.name());
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

int benchmark_cloneimg(Buffer<uint8_t> buf_src, Param<int32_t> width, Param<int32_t> height) {
  return jit_benchmark(cloneimg_fn(
    wrapFunc(buf_src, "buf_src"), width, height
  ), buf_src);
}
// }}} cloneimg

// {{{ rotate0
void generate_rotate0(std::vector<Target::Feature> features) {
  ImageParam src(type_of<uint8_t>(), 3);

  Param<int32_t> width{"width", 1920};
  Param<int32_t> height{"height", 1080};

  init_input_rgba(src);

  Func fn = rotate0_fn(src.in(), width, height);

  init_output_rgba(fn.output_buffer());

  generate_static_link(features, fn, {
    src, width, height
  }, fn.name());
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

int benchmark_rotate0(Buffer<uint8_t> buf_src, Param<int32_t> width, Param<int32_t> height) {
  return jit_benchmark(rotate0_fn(wrapFunc(buf_src, "buf_src"), width, height), buf_src);
}
// }}} rotate0

// {{{ rotate180
void generate_rotate180(std::vector<Target::Feature> features) {
  ImageParam src(type_of<uint8_t>(), 3);

  Param<int32_t> width{"width", 1920};
  Param<int32_t> height{"height", 1080};

  init_input_rgba(src);

  Func fn = rotate180_fn(src.in(), width, height);

  init_output_rgba(fn.output_buffer());

  generate_static_link(features, fn, { src, width, height }, fn.name());
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

int benchmark_rotate180(Buffer<uint8_t> buf_src, Param<int32_t> width, Param<int32_t> height) {
  return jit_benchmark(rotate180_fn(wrapFunc(buf_src, "buf_src"), width, height), buf_src);
}
// }}} rotate180

// {{{ rotate90
void generate_rotate90(std::vector<Target::Feature> features) {
  ImageParam src(type_of<uint8_t>(), 3);

  Param<int32_t> width{"width", 1920};
  Param<int32_t> height{"height", 1080};

  init_input_rgba(src);

  Func fn = rotate90_fn(src.in(), width, height);

  init_output_rgba(fn.output_buffer());

  generate_static_link(features, fn, { src, width, height }, fn.name());
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

int benchmark_rotate90(Buffer<uint8_t> buf_src, Param<int32_t> width, Param<int32_t> height) {
  return jit_benchmark(rotate90_fn(wrapFunc(buf_src, "buf_src"), width, height), buf_src);
}
// }}} rotate90

// {{{ rotate270
void generate_rotate270(std::vector<Target::Feature> features) {
  ImageParam src(type_of<uint8_t>(), 3);

  Param<int32_t> width{"width", 1920};
  Param<int32_t> height{"height", 1080};

  init_input_rgba(src);

  Func fn = rotate270_fn(src.in(), width, height);

  init_output_rgba(fn.output_buffer());

  generate_static_link(features, fn, { src, width, height }, fn.name());
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

int benchmark_rotate270(Buffer<uint8_t> buf_src, Param<int32_t> width, Param<int32_t> height) {
  return jit_benchmark(rotate270_fn(wrapFunc(buf_src, "buf_src"), width, height), buf_src);
}
// }}} rotate90

// {{{ erosion
void generate_erosion(std::vector<Target::Feature> features) {
  ImageParam src(type_of<uint8_t>(), 3);

  Param<int32_t> width{"width", 1920};
  Param<int32_t> height{"height", 1080};
  Param<uint8_t> size{"size", 8};

  init_input_rgba(src);

  Func fn = erosion_fn(
    src.in(), width, height, size
  );

  init_output_rgba(fn.output_buffer());

  generate_static_link(features, fn, {
    src, width, height, size,
  }, fn.name());
}

int jit_erosion(char **argv) {
  Buffer<uint8_t> buf_src = load_and_convert_image(argv[2]);

  Param<int32_t> width{"width", buf_src.get()->width()};
  Param<int32_t> height{"height", buf_src.get()->height()};
  Param<uint8_t> size{"size", (uint8_t) std::stoi(argv[3])};

  Buffer<uint8_t> out = jit_realize_uint8(erosion_fn(
    wrapFunc(buf_src, "buf_src"), width, height, size
  ), buf_src);
    
  printf("save to %s\n", argv[4]);
  save_image(out, argv[4]);
  return 0;
}

int benchmark_erosion(Buffer<uint8_t> buf_src, Param<int32_t> width, Param<int32_t> height) {
  Param<uint8_t> size{"size", 5};
  return jit_benchmark(erosion_fn(
    wrapFunc(buf_src, "buf_src"), width, height, size
  ), buf_src);
}
// }}} erosion

// {{{ dilation
void generate_dilation(std::vector<Target::Feature> features) {
  ImageParam src(type_of<uint8_t>(), 3);

  Param<int32_t> width{"width", 1920};
  Param<int32_t> height{"height", 1080};
  Param<uint8_t> size{"size", 8};

  init_input_rgba(src);

  Func fn = dilation_fn(
    src.in(), width, height, size
  );

  init_output_rgba(fn.output_buffer());

  generate_static_link(features, fn, {
    src, width, height, size,
  }, fn.name());
}

int jit_dilation(char **argv) {
  Buffer<uint8_t> buf_src = load_and_convert_image(argv[2]);

  Param<int32_t> width{"width", buf_src.get()->width()};
  Param<int32_t> height{"height", buf_src.get()->height()};
  Param<uint8_t> size{"size", (uint8_t) std::stoi(argv[3])};

  Buffer<uint8_t> out = jit_realize_uint8(dilation_fn(
    wrapFunc(buf_src, "buf_src"), width, height, size
  ), buf_src);
    
  printf("save to %s\n", argv[4]);
  save_image(out, argv[4]);
  return 0;
}

int benchmark_dilation(Buffer<uint8_t> buf_src, Param<int32_t> width, Param<int32_t> height) {
  Param<uint8_t> size{"size", 5};
  return jit_benchmark(dilation_fn(
    wrapFunc(buf_src, "buf_src"), width, height, size
  ), buf_src);
}
// }}} dilation

// {{{ morphology_open
void generate_morphology_open(std::vector<Target::Feature> features) {
  ImageParam src(type_of<uint8_t>(), 3);

  Param<int32_t> width{"width", 1920};
  Param<int32_t> height{"height", 1080};
  Param<int32_t> size{"size", 3};

  init_input_rgba(src);

  Func fn = morphology_open_fn(src.in(), width, height, size);

  init_output_rgba(fn.output_buffer());

  generate_static_link(features, fn, { src, width, height, size }, fn.name());
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

int benchmark_morphology_open(Buffer<uint8_t> buf_src, Param<int32_t> width, Param<int32_t> height) {
  Param<int32_t> size{"size", 3};
  return jit_benchmark(morphology_open_fn(wrapFunc(buf_src, "buf_src"), width, height, size), buf_src);
}
// }}} morphology_open

// {{{ morphology_close
void generate_morphology_close(std::vector<Target::Feature> features) {
  ImageParam src(type_of<uint8_t>(), 3);

  Param<int32_t> width{"width", 1920};
  Param<int32_t> height{"height", 1080};
  Param<int32_t> size{"size", 3};

  init_input_rgba(src);

  Func fn = morphology_close_fn(src.in(), width, height, size);

  init_output_rgba(fn.output_buffer());

  generate_static_link(features, fn, { src, width, height, size }, fn.name());
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

int benchmark_morphology_close(Buffer<uint8_t> buf_src, Param<int32_t> width, Param<int32_t> height) {
  Param<int32_t> size{"size", 3};
  return jit_benchmark(morphology_close_fn(wrapFunc(buf_src, "buf_src"), width, height, size), buf_src);
}
// }}} morphology_close

// {{{ morphology_gradient
void generate_morphology_gradient(std::vector<Target::Feature> features) {
  ImageParam src(type_of<uint8_t>(), 3);

  Param<int32_t> width{"width", 1920};
  Param<int32_t> height{"height", 1080};
  Param<int32_t> size{"size", 3};

  init_input_rgba(src);

  Func fn = morphology_gradient_fn(src.in(), width, height, size);

  init_output_rgba(fn.output_buffer());

  generate_static_link(features, fn, { src, width, height, size }, fn.name());
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

int benchmark_morphology_gradient(Buffer<uint8_t> buf_src, Param<int32_t> width, Param<int32_t> height) {
  Param<int32_t> size{"size", 3};
  return jit_benchmark(morphology_gradient_fn(wrapFunc(buf_src, "buf_src"), width, height, size), buf_src);
}
// }}} morphology_gradient

// {{{ grayscale
void generate_grayscale(std::vector<Target::Feature> features) {
  ImageParam src(type_of<uint8_t>(), 3);

  Param<int32_t> width{"width", 1920};
  Param<int32_t> height{"height", 1080};

  init_input_rgba(src);

  Func fn = grayscale_fn(
    src.in(), width, height
  );

  init_output_rgba(fn.output_buffer());

  generate_static_link(features, fn, {
    src, width, height,
  }, fn.name());
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

int benchmark_grayscale(Buffer<uint8_t> buf_src, Param<int32_t> width, Param<int32_t> height) {
  return jit_benchmark(grayscale_fn(
    wrapFunc(buf_src, "buf_src"), width, height
  ), buf_src);
}
// }}} grayscale

// {{{ invert
void generate_invert(std::vector<Target::Feature> features) {
  ImageParam src(type_of<uint8_t>(), 3);

  Param<int32_t> width{"width", 1920};
  Param<int32_t> height{"height", 1080};

  init_input_rgba(src);

  Func fn = invert_fn(
    src.in(), width, height
  );

  init_output_rgba(fn.output_buffer());

  generate_static_link(features, fn, {
    src, width, height,
  }, fn.name());
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

int benchmark_invert(Buffer<uint8_t> buf_src, Param<int32_t> width, Param<int32_t> height) {
  return jit_benchmark(invert_fn(
    wrapFunc(buf_src, "buf_src"), width, height
  ), buf_src);
}
// }}} invert

// {{{ brightness
void generate_brightness(std::vector<Target::Feature> features) {
  ImageParam src(type_of<uint8_t>(), 3);

  Param<int32_t> width{"width", 1920};
  Param<int32_t> height{"height", 1080};
  Param<float> factor{"factor", 1.5f};

  init_input_rgba(src);

  Func fn = brightness_fn(
    src.in(), width, height, factor
  );

  init_output_rgba(fn.output_buffer());

  generate_static_link(features, fn, {
    src, width, height, factor
  }, fn.name());
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

int benchmark_brightness(Buffer<uint8_t> buf_src, Param<int32_t> width, Param<int32_t> height) {
  Param<float> factor{"factor", 1.5f};
  return jit_benchmark(brightness_fn(
    wrapFunc(buf_src, "buf_src"), width, height, factor
  ), buf_src);
}
// }}} brightness

// {{{ gammacorrection
void generate_gammacorrection(std::vector<Target::Feature> features) {
  ImageParam src(type_of<uint8_t>(), 3);

  Param<int32_t> width{"width", 1920};
  Param<int32_t> height{"height", 1080};
  Param<float> factor{"factor", 2.5f};

  init_input_rgba(src);

  Func fn = gammacorrection_fn(
    src.in(), width, height, factor
  );

  init_output_rgba(fn.output_buffer());

  generate_static_link(features, fn, {
    src, width, height, factor
  }, fn.name());
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

int benchmark_gammacorrection(Buffer<uint8_t> buf_src, Param<int32_t> width, Param<int32_t> height) {
  Param<float> factor{"factor", 1.25f};
  return jit_benchmark(gammacorrection_fn(
    wrapFunc(buf_src, "buf_src"), width, height, factor
  ), buf_src);
}
// }}} gammacorrection

// {{{ contrast
void generate_contrast(std::vector<Target::Feature> features) {
  ImageParam src(type_of<uint8_t>(), 3);

  Param<int32_t> width{"width", 1920};
  Param<int32_t> height{"height", 1080};
  Param<float> factor{"factor", 0.525f};

  init_input_rgba(src);

  Func fn = contrast_fn(
    src.in(), width, height, factor
  );

  init_output_rgba(fn.output_buffer());

  generate_static_link(features, fn, {
    src, width, height, factor
  }, fn.name());
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

int benchmark_contrast(Buffer<uint8_t> buf_src, Param<int32_t> width, Param<int32_t> height) {
  Param<float> factor{"factor", 0.525f};
  return jit_benchmark(contrast_fn(
    wrapFunc(buf_src, "buf_src"), width, height, factor
  ), buf_src);
}
// }}} contrast

// {{{ boxblur
void generate_boxblur(std::vector<Target::Feature> features) {
  ImageParam src(type_of<uint8_t>(), 3);

  Param<int32_t> width{"width", 1920};
  Param<int32_t> height{"height", 1080};
  Param<uint8_t> size{"size", 11};

  init_input_rgba(src);

  Func fn = boxblur_fn(
    src.in(), width, height, size
  );

  init_output_rgba(fn.output_buffer());

  generate_static_link(features, fn, {
    src, width, height, size
  }, fn.name());
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

int benchmark_boxblur(Buffer<uint8_t> buf_src, Param<int32_t> width, Param<int32_t> height) {
  Param<uint8_t> size{"size", 10};
  return jit_benchmark(boxblur_fn(
    wrapFunc(buf_src, "buf_src"), width, height, size
  ), buf_src);
}
// }}} boxblur

// {{{ gaussianblur
void generate_gaussianblur(std::vector<Target::Feature> features) {
  ImageParam src(type_of<uint8_t>(), 3);

  Param<int32_t> width{"width", 1920};
  Param<int32_t> height{"height", 1080};
  Param<float> sigma{"sigma", 5.0};

  init_input_rgba(src);

  Func fn = gaussianblur_fn(
    src.in(), width, height, sigma
  );

  init_output_rgba(fn.output_buffer());

  generate_static_link(features, fn, {
    src, width, height, sigma
  }, fn.name());
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

int benchmark_gaussianblur(Buffer<uint8_t> buf_src, Param<int32_t> width, Param<int32_t> height) {
  Param<float> sigma{"sigma", 5.0f};
  return jit_benchmark(gaussianblur_fn(
    wrapFunc(buf_src, "buf_src"), width, height, sigma
  ), buf_src);
}
// }}} gaussianblur

// {{{ edge
void generate_edge(std::vector<Target::Feature> features) {
  ImageParam src(type_of<uint8_t>(), 3);

  Param<int32_t> width{"width", 1920};
  Param<int32_t> height{"height", 1080};

  init_input_rgba(src);

  Func fn = edge_fn(
    src.in(), width, height
  );

  init_output_rgba(fn.output_buffer());

  generate_static_link(features, fn, {
    src, width, height
  }, fn.name());
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

int benchmark_edge(Buffer<uint8_t> buf_src, Param<int32_t> width, Param<int32_t> height) {
  return jit_benchmark(edge_fn(
    wrapFunc(buf_src, "buf_src"), width, height
  ), buf_src);
}
// }}} edge

// {{{ sobel
void generate_sobel(std::vector<Target::Feature> features) {
  ImageParam src(type_of<uint8_t>(), 3);

  Param<int32_t> width{"width", 1920};
  Param<int32_t> height{"height", 1080};

  init_input_rgba(src);

  Func fn = sobel_fn(
    src.in(), width, height
  );

  init_output_rgba(fn.output_buffer());

  generate_static_link(features, fn, {
    src, width, height
  }, fn.name());
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

int benchmark_sobel(Buffer<uint8_t> buf_src, Param<int32_t> width, Param<int32_t> height) {
  return jit_benchmark(sobel_fn(
    wrapFunc(buf_src, "buf_src"), width, height
  ), buf_src);
}
// }}} sobel

// {{{ canny
void generate_canny(std::vector<Target::Feature> features) {
  ImageParam src(type_of<uint8_t>(), 3);

  Param<int32_t> width{"width", 1920};
  Param<int32_t> height{"height", 1080};
  Param<int32_t> threshold_max{"threshold_max", 250};
  Param<int32_t> threshold_min{"threshold_min", 100};

  init_input_rgba(src);

  Func fn = canny_fn(
    src.in(), width, height,
    threshold_max, threshold_min
  );

  init_output_rgba(fn.output_buffer());

  generate_static_link(features, fn, {
    src, width, height,
    threshold_max, threshold_min
  }, fn.name());
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

int benchmark_canny(Buffer<uint8_t> buf_src, Param<int32_t> width, Param<int32_t> height) {
  Param<int32_t> threshold_max{"threshold_max", 250};
  Param<int32_t> threshold_min{"threshold_min", 100};
  return jit_benchmark(canny_fn(
    wrapFunc(buf_src, "buf_src"), width, height,
    threshold_max, threshold_min
  ), buf_src);
}
// }}} canny

// {{{ canny_dilate
void generate_canny_dilate(std::vector<Target::Feature> features) {
  ImageParam src(type_of<uint8_t>(), 3);

  Param<int32_t> width{"width", 1920};
  Param<int32_t> height{"height", 1080};
  Param<int32_t> threshold_max{"threshold_max", 250};
  Param<int32_t> threshold_min{"threshold_min", 100};
  Param<int32_t> dilate{"dilate", 0};

  init_input_rgba(src);

  Func fn = canny_dilate_fn(
    src.in(), width, height,
    threshold_max, threshold_min,
    dilate
  );

  init_output_rgba(fn.output_buffer());

  generate_static_link(features, fn, {
    src, width, height,
    threshold_max, threshold_min,
    dilate
  }, fn.name());
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
// }}} canny_dilate

// {{{ canny_morphology_open
void generate_canny_morphology_open(std::vector<Target::Feature> features) {
  ImageParam src(type_of<uint8_t>(), 3);

  Param<int32_t> width{"width", 1920};
  Param<int32_t> height{"height", 1080};
  Param<int32_t> threshold_max{"threshold_max", 250};
  Param<int32_t> threshold_min{"threshold_min", 100};
  Param<int32_t> morphology_size{"morphology_size", 0};
  Param<int32_t> dilate{"dilate", 0};

  init_input_rgba(src);

  Func fn = canny_morphology_open_fn(
    src.in(), width, height,
    threshold_max, threshold_min,
    morphology_size,
    dilate
  );

  init_output_rgba(fn.output_buffer());

  generate_static_link(features, fn, {
    src, width, height,
    threshold_max, threshold_min,
    morphology_size,
    dilate
  }, fn.name());
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
// }}} canny_morphology_open

// {{{ canny_morphology_close
void generate_canny_morphology_close(std::vector<Target::Feature> features) {
  ImageParam src(type_of<uint8_t>(), 3);

  Param<int32_t> width{"width", 1920};
  Param<int32_t> height{"height", 1080};
  Param<int32_t> threshold_max{"threshold_max", 250};
  Param<int32_t> threshold_min{"threshold_min", 100};
  Param<int32_t> morphology_size{"morphology_size", 0};
  Param<int32_t> dilate{"dilate", 0};

  init_input_rgba(src);

  Func fn = canny_morphology_close_fn(
    src.in(), width, height,
    threshold_max, threshold_min,
    morphology_size,
    dilate
  );

  init_output_rgba(fn.output_buffer());

  generate_static_link(features, fn, {
    src, width, height,
    threshold_max, threshold_min,
    morphology_size,
    dilate
  }, fn.name());
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
// }}} canny_morphology_close

// {{{ emboss
void generate_emboss(std::vector<Target::Feature> features) {
  ImageParam src(type_of<uint8_t>(), 3);

  Param<int32_t> width{"width", 1920};
  Param<int32_t> height{"height", 1080};

  init_input_rgba(src);

  Func fn = emboss_fn(src.in(), width, height);

  init_output_rgba(fn.output_buffer());

  generate_static_link(features, fn, {
    src, width, height,
  }, fn.name());
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

int benchmark_emboss(Buffer<uint8_t> buf_src, Param<int32_t> width, Param<int32_t> height) {
  return jit_benchmark(emboss_fn(
    wrapFunc(buf_src, "buf_src"), width, height
  ), buf_src);
}
// }}} emboss

// {{{ laplacian
void generate_laplacian(std::vector<Target::Feature> features) {
  ImageParam src(type_of<uint8_t>(), 3);

  Param<int32_t> width{"width", 1920};
  Param<int32_t> height{"height", 1080};

  init_input_rgba(src);

  Func fn = laplacian_fn(
    src.in(), width, height
  );

  init_output_rgba(fn.output_buffer());

  generate_static_link(features, fn, {
    src, width, height,
  }, fn.name());
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

int benchmark_laplacian(Buffer<uint8_t> buf_src, Param<int32_t> width, Param<int32_t> height) {
  return jit_benchmark(laplacian_fn(
    wrapFunc(buf_src, "buf_src"), width, height
  ), buf_src);
}
// }}} laplacian

// {{{ highpass
void generate_highpass(std::vector<Target::Feature> features) {
  ImageParam src(type_of<uint8_t>(), 3);

  Param<int32_t> width{"width", 1920};
  Param<int32_t> height{"height", 1080};

  init_input_rgba(src);

  Func fn = highpass_fn(
    src.in(), width, height
  );

  init_output_rgba(fn.output_buffer());

  generate_static_link(features, fn, {
    src, width, height,
  }, fn.name());
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

int benchmark_highpass(Buffer<uint8_t> buf_src, Param<int32_t> width, Param<int32_t> height) {
  return jit_benchmark(highpass_fn(
    wrapFunc(buf_src, "buf_src"), width, height
  ), buf_src);
}
// }}} highpass

// {{{ gradient
void generate_gradient(std::vector<Target::Feature> features) {
  ImageParam src(type_of<uint8_t>(), 3);

  Param<int32_t> width{"width", 1920};
  Param<int32_t> height{"height", 1080};

  init_input_rgba(src);

  Func fn = gradient_fn(
    src.in(), width, height
  );

  init_output_rgba(fn.output_buffer());

  generate_static_link(features, fn, {
    src, width, height,
  }, fn.name());
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

int benchmark_gradient(Buffer<uint8_t> buf_src, Param<int32_t> width, Param<int32_t> height) {
  return jit_benchmark(gradient_fn(
    wrapFunc(buf_src, "buf_src"), width, height
  ), buf_src);
}
// }}} gradient

// {{{ blockmozaic
void generate_blockmozaic(std::vector<Target::Feature> features) {
  ImageParam src(type_of<uint8_t>(), 3);

  Param<int32_t> width{"width", 1920};
  Param<int32_t> height{"height", 1080};
  Param<int32_t> block{"block", 10};

  init_input_rgba(src);

  Func fn = blockmozaic_fn(
    src.in(), width, height, block
  );

  init_output_rgba(fn.output_buffer());

  generate_static_link(features, fn, {
    src, width, height, block
  }, fn.name());
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

int benchmark_blockmozaic(Buffer<uint8_t> buf_src, Param<int32_t> width, Param<int32_t> height) {
  Param<int32_t> block{"block", 10};
  return jit_benchmark(blockmozaic_fn(
    wrapFunc(buf_src, "buf_src"), width, height, block
  ), buf_src);
}
// }}} blockmozaic

// {{{ match_template_sad
void generate_match_template_sad(std::vector<Target::Feature> features) {
  ImageParam src(type_of<uint8_t>(), 3);
  ImageParam tpl(type_of<uint8_t>(), 3);

  Param<int32_t> width{"width", 1920};
  Param<int32_t> height{"height", 1080};
  Param<int32_t> tpl_width{"tpl_width", 60};
  Param<int32_t> tpl_height{"tpl_height", 60};

  init_input_rgba(src);
  init_input_rgba(tpl);

  Func fn = match_template_sad_fn(
    src.in(), width, height,
    tpl.in(), tpl_width, tpl_height
  );

  init_output_array(fn.output_buffer(), width, height);

  generate_static_link(features, fn, {
    src, width, height,
    tpl, tpl_width, tpl_height
  }, fn.name());
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

int benchmark_match_template_sad(
  Buffer<uint8_t> buf_src, Param<int32_t> width, Param<int32_t> height,
  Buffer<uint8_t> buf_tpl, Param<int32_t> tpl_width, Param<int32_t> tpl_height
){
  return jit_benchmark(match_template_sad_fn(
    wrapFunc(buf_src, "buf_src"), width, height,
    wrapFunc(buf_tpl, "buf_tpl"), tpl_width, tpl_height
  ), buf_src);
}
// }}} match_template_sad

// {{{ match_template_ssd
void generate_match_template_ssd(std::vector<Target::Feature> features) {
  ImageParam src(type_of<uint8_t>(), 3);
  ImageParam tpl(type_of<uint8_t>(), 3);

  Param<int32_t> width{"width", 1920};
  Param<int32_t> height{"height", 1080};
  Param<int32_t> tpl_width{"tpl_width", 60};
  Param<int32_t> tpl_height{"tpl_height", 60};

  init_input_rgba(src);
  init_input_rgba(tpl);

  Func fn = match_template_ssd_fn(
    src.in(), width, height,
    tpl.in(), tpl_width, tpl_height
  );

  init_output_array(fn.output_buffer(), width, height);

  generate_static_link(features, fn, {
    src, width, height,
    tpl, tpl_width, tpl_height
  }, fn.name());
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

int benchmark_match_template_ssd(
  Buffer<uint8_t> buf_src, Param<int32_t> width, Param<int32_t> height,
  Buffer<uint8_t> buf_tpl, Param<int32_t> tpl_width, Param<int32_t> tpl_height
) {
  return jit_benchmark(match_template_ssd_fn(
    wrapFunc(buf_src, "buf_src"), width, height,
    wrapFunc(buf_tpl, "buf_tpl"), tpl_width, tpl_height
  ), buf_src);
}
// }}} match_template_ssd

// {{{ match_template_ncc
void generate_match_template_ncc(std::vector<Target::Feature> features) {
  ImageParam src(type_of<uint8_t>(), 3);
  ImageParam tpl(type_of<uint8_t>(), 3);

  Param<int32_t> width{"width", 1920};
  Param<int32_t> height{"height", 1080};
  Param<int32_t> tpl_width{"tpl_width", 60};
  Param<int32_t> tpl_height{"tpl_height", 60};

  init_input_rgba(src);
  init_input_rgba(tpl);

  Func fn = match_template_ncc_fn(
    src.in(), width, height,
    tpl.in(), tpl_width, tpl_height
  );

  init_output_array(fn.output_buffer(), width, height);

  generate_static_link(features, fn, {
    src, width, height,
    tpl, tpl_width, tpl_height
  }, fn.name());
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

int benchmark_match_template_ncc(
  Buffer<uint8_t> buf_src, Param<int32_t> width, Param<int32_t> height,
  Buffer<uint8_t> buf_tpl, Param<int32_t> tpl_width, Param<int32_t> tpl_height
){
  return jit_benchmark(match_template_ncc_fn(
    wrapFunc(buf_src, "buf_src"), width, height,
    wrapFunc(buf_tpl, "buf_tpl"), tpl_width, tpl_height
  ), buf_src);
}
// }}} match_template_ncc

// {{{ prepared_match_template_ncc
void generate_prepared_match_template_ncc(std::vector<Target::Feature> features) {
  ImageParam src(type_of<uint8_t>(), 3);
  ImageParam tpl(type_of<uint8_t>(), 3);

  Param<int32_t> width{"width", 1920};
  Param<int32_t> height{"height", 1080};
  Param<int32_t> tpl_width{"tpl_width", 60};
  Param<int32_t> tpl_height{"tpl_height", 60};

  init_input_rgba(src);
  init_input_rgba(tpl);

  Func fn = match_template_ncc_fn(
    src.in(), width, height,
    tpl.in(), tpl_width, tpl_height
  );

  init_output_array(fn.output_buffer(), width, height);

  generate_static_link(features, fn, {
    src, width, height,
    tpl, tpl_width, tpl_height
  }, fn.name());
}

int jit_prepared_match_template_ncc(char **argv) {
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

int benchmark_prepared_match_template_ncc(
  Buffer<uint8_t> buf_src, Param<int32_t> width, Param<int32_t> height,
  Buffer<uint8_t> buf_tpl, Param<int32_t> tpl_width, Param<int32_t> tpl_height
){
  Func fn = prepare_ncc_template(wrapFunc(buf_tpl, "tpl"), tpl_width, tpl_height);
  Realization r = fn.realize(buf_tpl.get()->width(), buf_tpl.get()->height(), 3);
  Buffer<float> buf_tpl_val = r[0];
  Buffer<float> buf_tpl_sum = r[1];

  return jit_benchmark(prepated_match_template_ncc_fn(
    wrapFunc(buf_src, "buf_src"), width, height,
    wrapFunc_xy(buf_tpl_val, "val"), wrapFunc_xy(buf_tpl_sum, "sum"),
    tpl_width, tpl_height
  ), buf_src);
}
// }}} match_prepared_template_ncc

// {{{ match_template_zncc
void generate_match_template_zncc(std::vector<Target::Feature> features) {
  ImageParam src(type_of<uint8_t>(), 3);
  ImageParam tpl(type_of<uint8_t>(), 3);

  Param<int32_t> width{"width", 1920};
  Param<int32_t> height{"height", 1080};
  Param<int32_t> tpl_width{"tpl_width", 60};
  Param<int32_t> tpl_height{"tpl_height", 60};

  init_input_rgba(src);
  init_input_rgba(tpl);

  Func fn = match_template_zncc_fn(
    src.in(), width, height,
    tpl.in(), tpl_width, tpl_height
  );

  init_output_array(fn.output_buffer(), width, height);

  generate_static_link(features, fn, {
    src, width, height,
    tpl, tpl_width, tpl_height
  }, fn.name());
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

int benchmark_match_template_zncc(
  Buffer<uint8_t> buf_src, Param<int32_t> width, Param<int32_t> height,
  Buffer<uint8_t> buf_tpl, Param<int32_t> tpl_width, Param<int32_t> tpl_height
){
  return jit_benchmark(match_template_zncc_fn(
    wrapFunc(buf_src, "buf_src"), width, height,
    wrapFunc(buf_tpl, "buf_tpl"), tpl_width, tpl_height
  ), buf_src);
}
// }}} match_template_zncc

void generate(){
  printf("generate...\n");

  std::vector<Target::Feature> features;
  features.push_back(Target::AVX);
  features.push_back(Target::AVX2);
  //features.push_back(Target::FMA);
  //features.push_back(Target::FMA4);
  features.push_back(Target::F16C);
  features.push_back(Target::SSE41);
  features.push_back(Target::EmbedBitcode);
  features.push_back(Target::EnableLLVMLoopOpt);
  features.push_back(Target::Feature::NoRuntime);

  generate_runtime(features);
  generate_cloneimg(features);
  generate_rotate0(features);
  generate_rotate90(features);
  generate_rotate180(features);
  generate_rotate270(features);
  generate_grayscale(features);
  generate_invert(features);
  generate_brightness(features);
  generate_gammacorrection(features);
  generate_contrast(features);
  generate_boxblur(features);
  generate_gaussianblur(features);
  generate_blockmozaic(features);
  generate_erosion(features);
  generate_dilation(features);
  generate_morphology_open(features);
  generate_morphology_close(features);
  generate_morphology_gradient(features);
  generate_emboss(features);
  generate_laplacian(features);
  generate_highpass(features);
  generate_gradient(features);
  generate_edge(features);
  generate_sobel(features);
  generate_canny(features);
  generate_canny_dilate(features);
  generate_canny_morphology_open(features);
  generate_canny_morphology_close(features);
  generate_match_template_sad(features);
  generate_match_template_ssd(features);
  generate_match_template_ncc(features);
  generate_match_template_zncc(features);
}

void benchmark(char **argv) {
  printf("benchmark...\n");
  Buffer<uint8_t> buf_src = load_and_convert_image(argv[2]);
  Param<int32_t> width{"width", buf_src.get()->width()};
  Param<int32_t> height{"height", buf_src.get()->height()};

  Buffer<uint8_t> buf_tpl = load_and_convert_image(argv[3]);
  Param<int32_t> tpl_width{"tpl_width", buf_tpl.get()->width()};
  Param<int32_t> tpl_height{"tpl_height", buf_tpl.get()->height()};

  printf("src %dx%d\n", width.get(), height.get());
  benchmark_cloneimg(buf_src, width, height);
  benchmark_rotate0(buf_src, width, height);
  benchmark_rotate90(buf_src, width, height);
  benchmark_rotate180(buf_src, width, height);
  benchmark_rotate270(buf_src, width, height);
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
  // too slow
  //benchmark_match_template_zncc(buf_src, width, height, buf_tpl, tpl_width, tpl_height);
}

int main(int argc, char **argv) {
  if(argc == 1) {
    generate();
    return 0;
  }
  if(strcmp(argv[1], "benchmark") == 0) {
    benchmark(argv);
    return 0;
  }

  if(strcmp(argv[1], "cloneimg") == 0) {
    return jit_cloneimg(argv);
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
  if(strcmp(argv[1], "match_template_zncc") == 0) {
    return jit_match_template_zncc(argv);
  }

  printf("unknown command: %s\n", argv[1]);
  return 1;
}
