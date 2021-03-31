// +build ignore.

#define DEBUG 0

#include <Halide.h>
#include <halide_image_io.h>
#include <halide_benchmark.h>

using namespace Halide;
using namespace Halide::Tools;

const uint8_t GRAY_R = 76, GRAY_G = 152, GRAY_B = 28;

Func wrapFunc(Buffer<uint8_t> buf, const char* name) {
  Var x("x"), y("y"), ch("channel");

  Func f = Func(name);
  f(x, y, ch) = buf(x, y, ch);
  return f;
}

Func read(Func clamped, const char *name) {
  Var x("x"), y("y"), ch("channel");
  Func read = Func(name);
  read(x, y, ch) = cast<int16_t>(clamped(x, y, ch));
  return read;
}

Func readUI8(Func clamped, const char *name) {
  Var x("x"), y("y"), ch("channel");
  Func read = Func(name);
  read(x, y, ch) = cast<uint8_t>(clamped(x, y, ch));
  return read;
}

Func readUI16(Func clamped, const char *name) {
  Var x("x"), y("y"), ch("channel");
  Func read = Func(name);
  read(x, y, ch) = cast<uint16_t>(clamped(x, y, ch));
  return read;
}

Func readUI32(Func clamped, const char *name) {
  Var x("x"), y("y"), ch("channel");
  Func read = Func(name);
  read(x, y, ch) = cast<uint32_t>(clamped(x, y, ch));
  return read;
}

Func readI32(Func clamped, const char *name) {
  Var x("x"), y("y"), ch("channel");
  Func read = Func(name);
  read(x, y, ch) = cast<int32_t>(clamped(x, y, ch));
  return read;
}

Func cloneimg_fn(Func input, Param<int32_t> width, Param<int32_t> height) {
  Region src_bounds = {{0, width},{0, height},{0, 4}};
  Func in = read(BoundaryConditions::repeat_edge(input, src_bounds), "in");

  Var x("x"), y("y"), ch("ch");

  //
  // RGBA interleave test
  //
  Func cloneimg = Func("cloneimg");
  cloneimg(x, y, ch) = cast<uint8_t>(in(x, y, ch));

  return cloneimg;
}

Func grayscale_fn(Func input, Param<int32_t> width, Param<int32_t> height) {
  Region src_bounds = {{0, width},{0, height},{0, 4}};
  Func in = read(BoundaryConditions::repeat_edge(input, src_bounds), "in");

  Var x("x"), y("y"), ch("ch");
  Var xo("xo"), xi("xi");
  Var yo("yo"), yi("yi");
  Var ti("ti");

  Func grayscale = Func("grayscale");
  Expr r = in(x, y, 0);
  Expr g = in(x, y, 1);
  Expr b = in(x, y, 2);
  Expr value = ((r * GRAY_R) + (g * GRAY_G) + (b * GRAY_B)) >> 8;

  grayscale(x, y, ch) = cast<uint8_t>(value);

  grayscale.compute_root()
    .tile(x, y, xo, yo, xi, yi, 32, 32)
    .fuse(xo, yo, ti)
    .parallel(ch)
    .parallel(ti)
    .vectorize(xi, 32);

  return grayscale;
}

Func invert_fn(Func input, Param<int32_t> width, Param<int32_t> height) {
  Region src_bounds = {{0, width},{0, height},{0, 4}};
  Func in = read(BoundaryConditions::repeat_edge(input, src_bounds), "in");

  Var x("x"), y("y"), ch("ch");
  Var xo("xo"), xi("xi");
  Var yo("yo"), yi("yi");
  Var ti("ti");

  Func invert = Func("invert");
  Expr value = select(
    ch == 3, in(x, y, ch), // alpha
    255 - in(x, y, ch)     // r g b
  );
  invert(x, y, ch) = cast<uint8_t>(value);

  invert.compute_root()
    .tile(x, y, xo, yo, xi, yi, 32, 32)
    .fuse(xo, yo, ti)
    .parallel(ch)
    .parallel(ti)
    .vectorize(xi, 32);

  return invert;
}

Func brightness_fn(Func input, Param<int32_t> width, Param<int32_t> height, Param<float> factor) {
  Region src_bounds = {{0, width},{0, height},{0, 4}};
  Func in = read(BoundaryConditions::repeat_edge(input, src_bounds), "in");

  Var x("x"), y("y"), ch("ch");
  Var xo("xo"), xi("xi");
  Var yo("yo"), yi("yi");
  Var ti("ti");

  Func brightness = Func("brightness");
  Expr value = in(x, y, ch);
  value = cast<float>(value);
  value = min(255.0f, value * factor);

  brightness(x, y, ch) = cast<uint8_t>(value);

  brightness.compute_root()
    .tile(x, y, xo, yo, xi, yi, 32, 32)
    .fuse(xo, yo, ti)
    .parallel(ch)
    .parallel(ti)
    .vectorize(xi, 32);

  return brightness;
}

Func gammacorrection_fn(Func input, Param<int32_t> width, Param<int32_t> height, Param<float> factor) {
  Region src_bounds = {{0, width},{0, height},{0, 4}};
  Func in = read(BoundaryConditions::repeat_edge(input, src_bounds), "in");

  Var x("x"), y("y"), ch("ch");
  Var xo("xo"), xi("xi");
  Var yo("yo"), yi("yi");
  Var ti("ti");

  Expr e = 1.0f / max(0.00001f, factor);

  Func gammacorrection = Func("gammacorrection");
  Expr value = in(x, y, ch);
  value = cast<float>(value);
  value = fast_pow(value / 255.0f, e) * 255.0f;

  gammacorrection(x, y, ch) = cast<uint8_t>(value);

  gammacorrection.compute_root()
    .tile(x, y, xo, yo, xi, yi, 32, 32)
    .fuse(xo, yo, ti)
    .parallel(ch)
    .parallel(ti)
    .vectorize(xi, 32);

  return gammacorrection;
}

Func contrast_fn(Func input, Param<int32_t> width, Param<int32_t> height, Param<float> factor) {
  Region src_bounds = {{0, width},{0, height},{0, 4}};
  Func in = read(BoundaryConditions::repeat_edge(input, src_bounds), "in");

  Expr e = max(min(1.0f, factor), 0.0f);

  Var x("x"), y("y"), ch("ch");
  Var xo("xo"), xi("xi");
  Var yo("yo"), yi("yi");
  Var ti("ti");

  Func contrast = Func("contrast");
  Expr value = in(x, y, ch);
  value = cast<float>(value);
  value = (value / 255.0f) - 0.5f;
  value = (value * e) + 0.5f;
  value = value * 255.0f;

  contrast(x, y, ch) = cast<uint8_t>(value);

  contrast.compute_root()
    .tile(x, y, xo, yo, xi, yi, 32, 32)
    .fuse(xo, yo, ti)
    .parallel(ch)
    .parallel(ti)
    .vectorize(xi, 32);

  return contrast;
}

Func boxblur_fn(Func input, Param<int32_t> width, Param<int32_t> height, Param<uint8_t> size) {
  Region src_bounds = {{0, width},{0, height},{0, 4}};
  Func in = readI32(BoundaryConditions::repeat_edge(input, src_bounds), "in");

  Expr box_size = max(size, 1);

  Var x("x"), y("y"), ch("ch");
  Var xo("xo"), xi("xi");
  Var yo("yo"), yi("yi");
  Var ti("ti");

  RDom rd_x = RDom(0, box_size, "rdom_x");
  Func blur_x = Func("blur_x");
  blur_x(x, y, ch) = fast_integer_divide(sum(in(x + rd_x, y, ch)), box_size);

  RDom rd_y = RDom(0, box_size, "rdom_y");
  Func blur_y = Func("blur_y");
  blur_y(x, y, ch) = fast_integer_divide(sum(blur_x(x, y + rd_y, ch)), box_size);

  Func boxblur = Func("boxblur");
  boxblur(x, y, ch) = cast<uint8_t>(blur_y(x, y, ch));

  blur_x
    .compute_at(blur_y, y)
    .parallel(x)
    .vectorize(x, 32);
  blur_y
    .compute_at(boxblur, ti)
    .parallel(y)
    .vectorize(y, 32);

  boxblur.compute_root()
    .async()
    .unroll(ch, 3)
    .tile(x, y, xo, yo, xi, yi, 32, 32)
    .fuse(xo, yo, ti)
    .parallel(ti)
    .vectorize(xi, 32);

  return boxblur;
}

Func gaussianblur_fn(Func input, Param<int32_t> width, Param<int32_t> height, Param<float> sigma){
  Region src_bounds = {{0, width},{0, height},{0, 4}};
  Func in = readI32(BoundaryConditions::repeat_edge(input, src_bounds), "in");

  Expr radius = cast<int16_t>(ceil(sigma * 3));
  Expr acos_v = -1;
  Expr pi = acos(acos_v);
  Expr sig2 = 2 * sigma * sigma;
  Expr sigR = sigma * sqrt(2 * pi);

  Expr size = 2 * (radius + 1);
  Expr center = radius;

  Expr half = fast_integer_divide(radius, 2);

  Var x("x"), y("y"), ch("ch");
  Var xo("xo"), xi("xi");
  Var yo("yo"), yi("yi");
  Var ti("ti");

  RDom rd_rad = RDom(half, size, "rd_radius");

  Func kernel = Func("kernel");
  kernel(x) = fast_exp(-(x * x) / sig2 / sigR);

  Func sum_kernel = Func("sum_kernel");
  Expr kernel_val = kernel(rd_rad);
  sum_kernel(x) = sum(kernel_val);

  Func gaussianblur = Func("gaussianblur");
  Expr center_val = sum_kernel(center);
  Expr in_val = in(x + rd_rad.x, y, ch);
  Expr val = sum(in_val * kernel(rd_rad));
  gaussianblur(x, y, ch) = cast<uint8_t>(val / center_val);

  kernel.compute_root()
    .async()
    .vectorize(x, 32);

  sum_kernel.compute_root()
    .async()
    .vectorize(x, 32);

  gaussianblur.compute_root()
    .async()
    .tile(x, y, xo, yo, xi, yi, 32, 32)
    .fuse(xo, yo, ti)
    .parallel(ti)
    .vectorize(xi, 32);

  return gaussianblur;
}

Func edge_fn(Func input, Param<int32_t> width, Param<int32_t> height){
  Region src_bounds = {{0, width},{0, height},{0, 4}};
  Func in = readI32(BoundaryConditions::repeat_edge(input, src_bounds), "in");

  Var x("x"), y("y"), ch("ch");

  Func gray = Func("gray");
  Expr r = in(x, y, 0);
  Expr g = in(x, y, 1);
  Expr b = in(x, y, 2);
  //Expr value = ((r * GRAY_R) + (g * GRAY_G) + (b * GRAY_B)) >> 8;
  //gray(x, y) = cast<uint8_t>(value);
  gray(x, y) = cast<uint8_t>(r);

  Func gy = Func("gy");
  gy(x, y) = (gray(x, y + 1) - gray(x, y - 1)) / 2;

  Func gx = Func("gx");
  gx(x, y) = (gray(x + 1, y) - gray(x - 1, y)) / 2;

  Func edge = Func("edge");
  Expr pow_gy = fast_pow(gy(x, y), 2);
  Expr pow_gx = fast_pow(gx(x, y), 2);
  Expr magnitude = pow_gy + pow_gx;
  edge(x, y, ch) = select(
    ch == 3, 255,
    cast<uint8_t>(magnitude)
  );

  gy.compute_at(edge, x)
    .parallel(y)
    .vectorize(x, 3);
  gx.compute_at(edge, x)
    .parallel(x)
    .vectorize(y, 3);

  edge.compute_root()
    .parallel(ch);

  gray.compute_root();

  return edge;
}

Func sobel_fn(Func input, Param<int32_t> width, Param<int32_t> height){
  Region src_bounds = {{0, width},{0, height},{0, 4}};
  Func in = readI32(BoundaryConditions::repeat_edge(input, src_bounds), "in");

  Var x("x"), y("y"), ch("ch");

  Func gray = Func("gray");
  Expr r = in(x, y, 0);
  //Expr g = in(x, y, 1);
  //Expr b = in(x, y, 2);
  //Expr value = ((r * GRAY_R) + (g * GRAY_G) + (b * GRAY_B)) >> 8;
  //Expr value = (r + g + b) / 3;
  //gray(x, y) = cast<uint8_t>(value);
  gray(x, y) = cast<uint8_t>(r);

  Func kernel_y = Func("kernel_y");
  kernel_y(x, y) = 0;
  kernel_y(0, 0) = -1; kernel_y(1, 0) = -2; kernel_y(2, 0) = -1;
  kernel_y(0, 1) =  0; kernel_y(1, 1) =  0; kernel_y(2, 1) =  0;
  kernel_y(0, 2) =  1; kernel_y(1, 2) =  2; kernel_y(2, 2) =  1;

  Func kernel_x = Func("kernel_x");
  kernel_x(x, y) = 0;
  kernel_x(0, 0) = -1; kernel_x(1, 0) = 0; kernel_x(2, 0) = 1;
  kernel_x(0, 1) = -2; kernel_x(1, 1) = 0; kernel_x(2, 1) = 2;
  kernel_x(0, 2) = -1; kernel_x(1, 2) = 0; kernel_x(2, 2) = 1;

  RDom rd_kernel = RDom(0,3, 0,3, "rd_kernel");
  Func gy = Func("grad_y");
  Expr gy_in = gray(x + rd_kernel.x, y + rd_kernel.y);
  Expr gy_val = kernel_y(rd_kernel.x, rd_kernel.y);
  gy(x, y) += gy_in * gy_val;

  Func gx = Func("grad_x");
  Expr gx_in = gray(x + rd_kernel.x, y + rd_kernel.y);
  Expr gx_val = kernel_x(rd_kernel.x, rd_kernel.y);
  gx(x, y) += gx_in * gx_val;

  Func sobel = Func("sobel");
  Expr pow_gy = fast_pow(abs(gy(x, y)), 2);
  Expr pow_gx = fast_pow(abs(gx(x, y)), 2);
  Expr magnitude = ceil(sqrt(pow_gy + pow_gx));
  sobel(x, y, ch) = select(
    ch == 3, 255,
    cast<uint8_t>(magnitude)
  );

  kernel_x.compute_root()
    .parallel(y)
    .vectorize(x, 32);
  kernel_y.compute_root()
    .parallel(y)
    .vectorize(x, 32);

  gy.compute_at(sobel, x)
    .vectorize(y, 3)
    .vectorize(x, 3);
  gx.compute_at(sobel, x)
    .vectorize(y, 3)
    .vectorize(x, 3);

  sobel.compute_root()
    .parallel(ch);

  gray.compute_root();

  return sobel;
}

Func blockmozaic_fn(Func input, Param<int32_t> width, Param<int32_t> height, Param<int32_t> block_size){
  Region src_bounds = {{0, width},{0, height},{0, 4}};
  Func in = readI32(BoundaryConditions::repeat_edge(input, src_bounds), "in");

  Var x("x"), y("y"), ch("ch");
  Var xo("xo"), xi("xi");
  Var yo("yo"), yi("yi");
  Var ti("ti");

  Expr block_size_half = block_size / 2;
  Expr base = block_size_half * block_size_half; // width * height

  RDom rd_block = RDom(-block_size_half, block_size_half, -block_size_half, block_size_half, "rd_block");
  Func block_color = Func("block_color");
  block_color(x, y, ch) = 0.0f;
  Expr in_val = in(x + rd_block.x, y + rd_block.y, ch);
  block_color(x, y, ch) += cast<float>(in_val);

  Func avg_color = Func("avg_color");
  avg_color(x, y, ch) = select(
    ch == 3, 255,
    block_color(x, y, ch) / base
  );

  Func blockmozaic = Func("blockmozaic");
  Expr avg_val = avg_color(x + x % block_size, y + y % block_size, ch);
  blockmozaic(x, y, ch) = cast<uint8_t>(avg_val);

  avg_color.compute_root()
    .parallel(y)
    .vectorize(x, 32);

  blockmozaic.compute_root()
    .async()
    .tile(x, y, xo, yo, xi, yi, 32, 32)
    .fuse(xo, yo, ti)
    .parallel(ti)
    .vectorize(xi, 32);

  in.compute_root();

  return blockmozaic;
}

Func emboss_fn(Func input, Param<int32_t> width, Param<int32_t> height){
  Region src_bounds = {{0, width},{0, height},{0, 4}};
  Func in = readI32(BoundaryConditions::repeat_edge(input, src_bounds), "in");

  Var x("x"), y("y"), ch("ch");
  Var xo("xo"), xi("xi");
  Var yo("yo"), yi("yi");
  Var ti("ti");

  Func kernel = Func("kernel");
  kernel(x, y) = 0;
  kernel(0, 0) = -1; kernel(1, 0) = -1; kernel(2, 0) = 0;
  kernel(0, 1) = -1; kernel(1, 1) =  0; kernel(2, 1) = 1;
  kernel(0, 2) =  0; kernel(1, 2) =  1; kernel(2, 2) = 1;

  RDom rd_kernel = RDom(0,3, 0,3, "rd_kernel");
  Func gradient = Func("gradient");
  Expr in_val = in(x + rd_kernel.x, y + rd_kernel.y, ch);
  Expr k_val  = kernel(rd_kernel.x, rd_kernel.y);
  Expr val = in_val * k_val;
  gradient(x, y, ch) += cast<uint8_t>(val);

  Func emboss = Func("emboss");
  emboss(x, y, ch) = select(
    ch == 3, 255,
    clamp(gradient(x, y, ch) + 128, 0, 255)
  );

  gradient.compute_root()
    .vectorize(x, 32);

  emboss.compute_root()
    .tile(x, y, xo, yo, xi, yi, 32, 32)
    .fuse(xo, yo, ti)
    .parallel(ti)
    .vectorize(xi, 32);

  in.compute_root();

  return emboss;
}

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

  printf("benchmarking %s...\n", fn.name().c_str());
  double result = benchmark(10, 10, [&]() {
    fn.realize({buf_src.get()->width(), buf_src.get()->height(), 3});
  });
  printf("%s: %gms\n", fn.name().c_str(), result * 1e3);
  return 0;
}

Buffer<uint8_t> jit_realize_uint8(Func fn, Buffer<uint8_t> src) {
  fn.compile_jit(get_jit_target_from_environment());

  printf("realize %s...\n", fn.name().c_str());
  
  return fn.realize({src.get()->width(), src.get()->height(), 3});
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

void generate(){
  printf("generate...\n");

  std::vector<Target::Feature> features;
  features.push_back(Target::AVX);
  features.push_back(Target::AVX2);
  features.push_back(Target::FMA);
  features.push_back(Target::FMA4);
  features.push_back(Target::F16C);
  features.push_back(Target::SSE41);
  features.push_back(Target::EmbedBitcode);
  features.push_back(Target::EnableLLVMLoopOpt);
  features.push_back(Target::Feature::NoRuntime);

  generate_runtime(features);
  generate_cloneimg(features);
  generate_grayscale(features);
  generate_invert(features);
  generate_brightness(features);
  generate_gammacorrection(features);
  generate_contrast(features);
  generate_boxblur(features);
  generate_gaussianblur(features);
  generate_edge(features);
  generate_sobel(features);
  generate_blockmozaic(features);
  generate_emboss(features);
}

void benchmark(char **argv) {
  printf("benchmark...\n");
  Buffer<uint8_t> buf_src = load_and_convert_image(argv[2]);

  Param<int32_t> width{"width", buf_src.get()->width()};
  Param<int32_t> height{"height", buf_src.get()->height()};

  printf("w/ src %dx%d\n", width.get(), height.get());
  benchmark_cloneimg(buf_src, width, height);
  benchmark_grayscale(buf_src, width, height);
  benchmark_invert(buf_src, width, height);
  benchmark_brightness(buf_src, width, height);
  benchmark_gammacorrection(buf_src, width, height);
  benchmark_contrast(buf_src, width, height);
  benchmark_boxblur(buf_src, width, height);
  benchmark_gaussianblur(buf_src, width, height);
  benchmark_edge(buf_src, width, height);
  benchmark_sobel(buf_src, width, height);
  benchmark_blockmozaic(buf_src, width, height);
  benchmark_emboss(buf_src, width, height);
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
  if(strcmp(argv[1], "blockmozaic") == 0) {
    return jit_blockmozaic(argv);
  }
  if(strcmp(argv[1], "emboss") == 0) {
    return jit_emboss(argv);
  }

  printf("unknown command: %s\n", argv[1]);
  return 1;
}
