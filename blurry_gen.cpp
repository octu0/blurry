// +build ignore.

#include "blurry.hpp"

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
  printf("BenchmarkJIT/%-20s: %-3.5fms\n", fn.name().c_str(), result * 1e3);
  return 0;
}

Buffer<uint8_t> jit_realize_uint8_bounds(Func fn, int32_t width, int32_t height) {
  fn.compile_jit(get_jit_target_from_environment());

  printf("realize %s...\n", fn.name().c_str());
  
  return fn.realize({width, height, 3});
}

Buffer<uint8_t> jit_realize_uint8(Func fn, Buffer<uint8_t> src) {
  return jit_realize_uint8_bounds(fn, src.get()->width(), src.get()->height());
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

// {{{ rotate
void generate_rotate(std::vector<Target::Feature> features) {
  ImageParam src(type_of<uint8_t>(), 3);

  Param<int32_t> width{"width", 1920};
  Param<int32_t> height{"height", 1080};
  Param<int16_t> rotation{"rotation", ROTATE90};

  init_input_rgba(src);

  Func fn = rotate_fn(
    src.in(), width, height, rotation
  );

  init_output_rgba(fn.output_buffer());

  generate_static_link(features, fn, {
    src, width, height, rotation
  }, fn.name());
}

int jit_rotate(char **argv) {
  Buffer<uint8_t> buf_src = load_and_convert_image(argv[2]);

  Param<int32_t> width{"width", buf_src.get()->width()};
  Param<int32_t> height{"height", buf_src.get()->height()};
  Param<int16_t> rotation{"rotation", (int16_t) std::stoi(argv[3])};

  Func fn = rotate_fn(
    wrapFunc(buf_src, "buf_src"), width, height,
    rotation
  );

  Buffer<uint8_t> out;
  if(rotation.get() == ROTATE0 || rotation.get() == ROTATE180) {
    out = jit_realize_uint8_bounds(fn, width.get(), height.get());
  } else {
    out = jit_realize_uint8_bounds(fn, height.get(), width.get());
  }

  printf("save to %s\n", argv[4]);
  save_image(out, argv[4]);
  return 0;
}

int benchmark_rotate(Buffer<uint8_t> buf_src, Param<int32_t> width, Param<int32_t> height) {
  Param<int16_t> rotation{"rotation", ROTATE180};
  return jit_benchmark(rotate_fn(
    wrapFunc(buf_src, "buf_src"), width, height, rotation
  ), buf_src);
}
// }}} rotate

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

// {{{ morphology
void generate_morphology(std::vector<Target::Feature> features) {
  ImageParam src(type_of<uint8_t>(), 3);

  Param<int32_t> width{"width", 1920};
  Param<int32_t> height{"height", 1080};
  Param<uint8_t> mode{"mode", MORPH_OPEN};
  Param<int32_t> size{"size", 3};

  init_input_rgba(src);

  Func fn = morphology_fn(
    src.in(), width, height,
    mode, size
  );

  init_output_rgba(fn.output_buffer());

  generate_static_link(features, fn, {
    src, width, height,
    mode, size
  }, fn.name());
}

int jit_morphology(char **argv) {
  Buffer<uint8_t> buf_src = load_and_convert_image(argv[2]);

  Param<int32_t> width{"width", buf_src.get()->width()};
  Param<int32_t> height{"height", buf_src.get()->height()};
  Param<uint8_t> mode{"mode", (uint8_t) std::stoi(argv[3])};
  Param<int32_t> size{"size", (int32_t) std::stoi(argv[4])};

  Buffer<uint8_t> out = jit_realize_uint8(morphology_fn(
    wrapFunc(buf_src, "buf_src"), width, height,
    mode, size
  ), buf_src);
    
  printf("save to %s\n", argv[5]);
  save_image(out, argv[5]);
  return 0;
}

int benchmark_morphology(Buffer<uint8_t> buf_src, Param<int32_t> width, Param<int32_t> height) {
  Param<uint8_t> mode{"mode", MORPH_OPEN};
  Param<int32_t> size{"size", 3};
  return jit_benchmark(morphology_fn(
    wrapFunc(buf_src, "buf_src"), width, height,
    mode, size
  ), buf_src);
}
// }}} morphology

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
  Param<uint8_t> morphology_mode{"morphology_mode", 0};
  Param<int32_t> morphology_size{"morphology_size", 0};
  Param<int32_t> dilate{"dilate", 0};

  init_input_rgba(src);

  Func fn = canny_fn(
    src.in(), width, height,
    threshold_max, threshold_min,
    morphology_mode, morphology_size,
    dilate
  );

  init_output_rgba(fn.output_buffer());

  generate_static_link(features, fn, {
    src, width, height,
    threshold_max, threshold_min,
    morphology_mode, morphology_size,
    dilate
  }, fn.name());
}

int jit_canny(char **argv) {
  Buffer<uint8_t> buf_src = load_and_convert_image(argv[2]);

  Param<int32_t> width{"width", buf_src.get()->width()};
  Param<int32_t> height{"height", buf_src.get()->height()};
  Param<int32_t> threshold_max{"threshold_max", std::stoi(argv[3])};
  Param<int32_t> threshold_min{"threshold_min", std::stoi(argv[4])};
  Param<uint8_t> morphology_mode{"morphology_mode", (uint8_t) std::stoi(argv[5])};
  Param<int32_t> morphology_size{"morphology_size", std::stoi(argv[6])};
  Param<int32_t> dilate{"dilate", std::stoi(argv[7])};

  Buffer<uint8_t> out = jit_realize_uint8(canny_fn(
    wrapFunc(buf_src, "buf_src"), width, height,
    threshold_max, threshold_min,
    morphology_mode, morphology_size,
    dilate
  ), buf_src);

  printf("save to %s\n", argv[8]);
  save_image(out, argv[8]);
  return 0;
}

int benchmark_canny(Buffer<uint8_t> buf_src, Param<int32_t> width, Param<int32_t> height) {
  Param<int32_t> threshold_max{"threshold_max", 250};
  Param<int32_t> threshold_min{"threshold_min", 100};
  Param<uint8_t> morphology_mode{"morphology_mode", 2};
  Param<int32_t> morphology_size{"morphology_size", 3};
  Param<int32_t> dilate{"dilate", 0};
  return jit_benchmark(canny_fn(
    wrapFunc(buf_src, "buf_src"), width, height,
    threshold_max, threshold_min,
    morphology_mode, morphology_size,
    dilate
  ), buf_src);
}
// }}} canny

// {{{ emboss
void generate_emboss(std::vector<Target::Feature> features) {
  ImageParam src(type_of<uint8_t>(), 3);

  Param<int32_t> width{"width", 1920};
  Param<int32_t> height{"height", 1080};

  init_input_rgba(src);

  Func fn = emboss_fn(
    src.in(), width, height
  );

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
  generate_rotate(features);
  generate_erosion(features);
  generate_dilation(features);
  generate_morphology(features);
  generate_grayscale(features);
  generate_invert(features);
  generate_brightness(features);
  generate_gammacorrection(features);
  generate_contrast(features);
  generate_boxblur(features);
  generate_gaussianblur(features);
  generate_edge(features);
  generate_sobel(features);
  generate_canny(features);
  generate_emboss(features);
  generate_laplacian(features);
  generate_highpass(features);
  generate_gradient(features);
  generate_blockmozaic(features);
}

void benchmark(char **argv) {
  printf("benchmark...\n");
  Buffer<uint8_t> buf_src = load_and_convert_image(argv[2]);

  Param<int32_t> width{"width", buf_src.get()->width()};
  Param<int32_t> height{"height", buf_src.get()->height()};

  printf("src %dx%d\n", width.get(), height.get());
  benchmark_cloneimg(buf_src, width, height);
  benchmark_rotate(buf_src, width, height);
  benchmark_erosion(buf_src, width, height);
  benchmark_dilation(buf_src, width, height);
  benchmark_morphology(buf_src, width, height);
  benchmark_grayscale(buf_src, width, height);
  benchmark_invert(buf_src, width, height);
  benchmark_brightness(buf_src, width, height);
  benchmark_gammacorrection(buf_src, width, height);
  benchmark_contrast(buf_src, width, height);
  benchmark_boxblur(buf_src, width, height);
  benchmark_gaussianblur(buf_src, width, height);
  benchmark_edge(buf_src, width, height);
  benchmark_sobel(buf_src, width, height);
  benchmark_canny(buf_src, width, height);
  benchmark_emboss(buf_src, width, height);
  benchmark_laplacian(buf_src, width, height);
  benchmark_highpass(buf_src, width, height);
  benchmark_gradient(buf_src, width, height);
  benchmark_blockmozaic(buf_src, width, height);
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
  if(strcmp(argv[1], "rotate") == 0) {
    return jit_rotate(argv);
  } 
  if(strcmp(argv[1], "erosion") == 0) {
    return jit_erosion(argv);
  }
  if(strcmp(argv[1], "dilation") == 0) {
    return jit_dilation(argv);
  }
  if(strcmp(argv[1], "morphology") == 0) {
    return jit_morphology(argv);
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

  printf("unknown command: %s\n", argv[1]);
  return 1;
}