// +build ignore.

#define DEBUG 0

#include <Halide.h>
#include <halide_image_io.h>
#include <halide_benchmark.h>

using namespace Halide;
using namespace Halide::Tools;

const uint8_t GRAY_R = 76, GRAY_G = 152, GRAY_B = 28;

const int16_t ROTATE0   = 0;   // No rotation
const int16_t ROTATE90  = 90;  // Rotate 90 degrees clockwise
const int16_t ROTATE180 = 180; // Rotate 180 degrees
const int16_t ROTATE270 = 270; // Rotate 270 degrees clockwise

const Expr acos_v = -1.0f;
const Expr pi = acos(acos_v);

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

Func rotate90(Func clamped, Param<int32_t> width, Param<int32_t> height, const char *name) {
  Var x("x"), y("y"), ch("channel");
  Func read = Func(name);
  read(x, y, ch) = clamped(y, (height - 1) - x, ch);
  return read;
}

Func rotate180(Func clamped, Param<int32_t> width, Param<int32_t> height, const char *name) {
  Var x("x"), y("y"), ch("channel");
  Func read = Func(name);
  read(x, y, ch) = clamped((width - 1) - x, (height - 1) - y, ch);
  return read;
}

Func rotate270(Func clamped, Param<int32_t> width, Param<int32_t> height, const char *name) {
  Var x("x"), y("y"), ch("channel");
  Func read = Func(name);
  read(x, y, ch) = clamped((width - 1) - y, x, ch);
  return read;
}

Func rotation(Param<int16_t> mode, Func clamped, Param<int32_t> width, Param<int32_t> height, const char *name) {
  if(mode.get() == ROTATE0) {
    return read(clamped, name);
  }
  if(mode.get() == ROTATE90) {
    return rotate90(clamped, width, height, name);
  }
  if(mode.get() == ROTATE180) {
    return rotate180(clamped, width, height, name);
  }
  if(mode.get() == ROTATE270) {
    return rotate270(clamped, width, height, name);
  }
  return read(clamped, "unsupported rotation");
}

Func convolve(Func in, Func kernel, RDom rd_kernel) {
  Var x("x"), y("y"), ch("ch");

  Func convolve = Func("convolve");
  Expr in_val = in(x + rd_kernel.x, y + rd_kernel.y, ch);
  Expr k_val  = kernel(rd_kernel.x, rd_kernel.y);
  Expr val = in_val * k_val;
  convolve(x, y, ch) += cast<uint8_t>(val);

  return convolve;
}

Func convolve_xy(Func in, Func kernel, RDom rd_kernel) {
  Var x("x"), y("y");

  Func convolve = Func("convolve_xy");
  Expr in_val = in(x + rd_kernel.x, y + rd_kernel.y);
  Expr k_val  = kernel(rd_kernel.x, rd_kernel.y);
  convolve(x, y) += in_val * k_val;

  return convolve;
}

Func filter2d_gray(
  Func input,
  Region bounds,
  Func kernel,
  RDom rd_kernel,
  const char *name
) {
  Func in = read(BoundaryConditions::repeat_edge(input, bounds), "in");

  Var x("x"), y("y"), ch("ch");
  Var xo("xo"), xi("xi");
  Var yo("yo"), yi("yi");
  Var ti("ti");

  Func gray = Func("gray");
  Expr r = in(x, y, 0);
  gray(x, y) = r;

  Func conv = convolve_xy(gray, kernel, rd_kernel);

  Func gradient = Func(name);
  gradient(x, y, ch) = select(
    ch == 3, 255,
    conv(x, y) & 128
  );

  // schedule
  conv.compute_root()
    .vectorize(x, 32);

  gradient.compute_root()
    .tile(x, y, xo, yo, xi, yi, 32, 32)
    .fuse(xo, yo, ti)
    .parallel(ti)
    .vectorize(xi, 32);

  gray.compute_root();
  return gradient;
}

Func gauss_kernel(Expr sigma) {
  Var x("x"), y("y"), ch("ch");

  Expr sig2 = 2 * sigma * sigma;
  Expr sigR = sigma * sqrt(2 * pi);

  Func kernel = Func("kernel");
  kernel(x) = fast_exp(-((x * x)) / sig2 / sigR);

  kernel.compute_root();
  return kernel;
}

Func gaussian(Func in, Expr sigma, RDom rd, const char *name) {
  Var x("x"), y("y");
  Var xo("xo"), xi("xi");
  Var yo("yo"), yi("yi");
  Var ti("ti");

  Func kernel = gauss_kernel(sigma);

  Func sum_kernel = Func("sum_kernel");
  Expr kernel_val = kernel(rd);
  sum_kernel(x) += kernel_val;

  Func gaussian = Func(name);
  Expr center_val = sum_kernel(0);
  Expr in_val = in(x + rd.x, y);
  Expr val = in_val * kernel(rd);
  gaussian(x, y) += cast<uint8_t>(val / center_val);

  gaussian.compute_root()
    .async()
    .tile(x, y, xo, yo, xi, yi, 32, 32)
    .fuse(xo, yo, ti)
    .parallel(ti)
    .vectorize(xi, 32);

  sum_kernel.compute_root();
  return gaussian;
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

Func rotate_fn(Func input, Param<int32_t> width, Param<int32_t> height, Param<int16_t> mode) {
  Region src_bounds = {{0, width},{0, height},{0, 4}};
  Func clamped = BoundaryConditions::constant_exterior(input, 0, src_bounds);
  Func in = rotation(mode, clamped, width, height, "in");

  Var x("x"), y("y"), ch("ch");

  Func rotate = Func("rotate");
  rotate(x, y, ch) = cast<uint8_t>(in(x, y, ch));

  return rotate;
}

Func erosion_fn(Func input, Param<int32_t> width, Param<int32_t> height, Param<uint8_t> size) {
  Region src_bounds = {{0, width},{0, height},{0, 4}};
  Func in = read(BoundaryConditions::repeat_edge(input, src_bounds), "in");

  Var x("x"), y("y"), ch("ch");
  Var xo("xo"), xi("xi");
  Var yo("yo"), yi("yi");
  Var ti("ti");

  RDom rd = RDom(0,size, 0,size, "erode");
  Func erosion = Func("erosion");
  Expr value = in(x + rd.x, y + rd.y, ch);
  erosion(x, y, ch) = minimum(cast<uint8_t>(value));

  erosion.compute_root()
    .tile(x, y, xo, yo, xi, yi, 32, 32)
    .fuse(xo, yo, ti)
    .parallel(ch)
    .parallel(ti)
    .vectorize(xi, 32);

  in.compute_root();
  return erosion;
}

Func dilation_fn(Func input, Param<int32_t> width, Param<int32_t> height, Param<uint8_t> size) {
  Region src_bounds = {{0, width},{0, height},{0, 4}};
  Func in = read(BoundaryConditions::repeat_edge(input, src_bounds), "in");

  Var x("x"), y("y"), ch("ch");
  Var xo("xo"), xi("xi");
  Var yo("yo"), yi("yi");
  Var ti("ti");

  RDom rd = RDom(0,size, 0,size, "dilate");
  Func dilation = Func("dilation");
  Expr value = in(x + rd.x, y + rd.y, ch);
  dilation(x, y, ch) = maximum(cast<uint8_t>(value));

  dilation.compute_root()
    .async()
    .tile(x, y, xo, yo, xi, yi, 32, 32)
    .fuse(xo, yo, ti)
    .parallel(ch)
    .parallel(ti)
    .vectorize(xi, 32);

  in.compute_root();
  return dilation;
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

  in.compute_root();

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

  blur_x.compute_root()
    .parallel(y, 8)
    .vectorize(x, 32);
  blur_y.compute_root()
    .parallel(y, 8)
    .vectorize(x, 32);

  boxblur.compute_root()
    .async()
    .tile(x, y, xo, yo, xi, yi, 32, 32)
    .fuse(xo, yo, ti)
    .parallel(ch)
    .parallel(ti, 8)
    .vectorize(xi, 32);

  in.compute_root();
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
    .parallel(x)
    .vectorize(x, 32);

  sum_kernel.compute_root()
    .parallel(x)
    .vectorize(x, 32);

  gaussianblur.compute_root()
    .async()
    .tile(x, y, xo, yo, xi, yi, 32, 32)
    .fuse(xo, yo, ti)
    .parallel(ch)
    .parallel(ti)
    .vectorize(xi, 32);

  in.compute_root();
  return gaussianblur;
}

Func edge_fn(Func input, Param<int32_t> width, Param<int32_t> height){
  Region src_bounds = {{0, width},{0, height},{0, 4}};
  Func in = read(BoundaryConditions::repeat_edge(input, src_bounds), "in");

  Var x("x"), y("y"), ch("ch");

  Func gray = Func("gray");
  Expr r = in(x, y, 0);
  //Expr g = in(x, y, 1);
  //Expr b = in(x, y, 2);
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
  Func in = read(BoundaryConditions::repeat_edge(input, src_bounds), "in");

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
    .parallel(y, 16)
    .vectorize(x, 3);
  gx.compute_at(sobel, x)
    .parallel(y, 16)
    .vectorize(x, 3);

  sobel.compute_root()
    .parallel(ch)
    .parallel(y, 8);

  gray.compute_root();
  return sobel;
}

Func canny_fn(
  Func input, Param<int32_t> width, Param<int32_t> height,
  Param<int32_t> threshold_max, Param<int32_t> threshold_min,
  Param<float> sigma
) {
  Region src_bounds = {{0, width},{0, height},{0, 4}};
  Func in = read(BoundaryConditions::repeat_edge(input, src_bounds), "in");

  Var x("x"), y("y"), ch("ch");
  Var xo("xo"), xi("xi");
  Var yo("yo"), yi("yi");
  Var ti("ti");

  Func gray = Func("gray");
  gray(x, y) = cast<uint8_t>(in(x, y, 0)); // rgba(r) for grayscale

  RDom gauss_rd = RDom(-1, 3, "gaussian_rdom");
  Func gauss = gaussian(gray, sigma, gauss_rd, "gaussian5x5");

  Func ks_x = Func("kernel_sobel_x");
  ks_x(x, y) = 0;
  ks_x(-1, -1) = -1; ks_x(0, -1) = 0; ks_x(1, -1) = 1;
  ks_x(-1,  0) = -2; ks_x(0,  0) = 0; ks_x(1,  0) = 2;
  ks_x(-1,  1) = -1; ks_x(0,  1) = 0; ks_x(1,  1) = 1;

  Func ks_y = Func("kernel_sobel_y");
  ks_y(x, y) = 0;
  ks_y(-1, -1) = -1; ks_y(0, -1) = -2; ks_y(1, -1) = -1;
  ks_y(-1,  0) =  0; ks_y(0,  0) =  0; ks_y(1,  0) =  0;
  ks_y(-1,  1) =  1; ks_y(0,  1) =  2; ks_y(1,  1) =  1;

  Func gx = convolve_xy(gauss, ks_x, RDom(-1,3, -1,3, "rd_kernel_sobel_x"));
  Func gy = convolve_xy(gauss, ks_y, RDom(-1,3, -1,3, "rd_kernel_sobel_y"));

  Func sobel = Func("sobel");
  Expr pow_gy = fast_pow(abs(gy(x, y)), 2);
  Expr pow_gx = fast_pow(abs(gx(x, y)), 2);
  Expr magnitude = ceil(sqrt(pow_gy + pow_gx));
  sobel(x, y) = magnitude;

  Func nms = Func("nonmax_supression");
  Expr angle = (atan2(gy(x, y), gx(x, y)) * 180) / pi;
  Expr approx = select(
    angle >=  -22.5f && angle <  22.5f,    0,
    angle >=  157.5f && angle <  180.0f,   0,
    angle >=  180.0f && angle < -157.5f,   0,
    angle >=   22.5f && angle <   67.5f,  45,
    angle >= -157.5f && angle < -112.5f,  45,
    angle >=   67.5f && angle <  112.5f,  90,
    angle >= -112.5f && angle <  -67.5f,  90,
    angle >=  112.5f && angle <  157.5f, 135,
    angle >=  -67.5f && angle <  -22.5f, 135,
    0
  );
  nms(x, y) = select(
    approx ==  0 && sobel(x, y) < sobel(x + 1, y), 0,
    approx ==  0 && sobel(x, y) < sobel(x - 1, y), 0,
    approx == 45 && sobel(x, y) < sobel(x + 1, y - 1), 0,
    approx == 45 && sobel(x, y) < sobel(x - 1, y + 1), 0,
    approx == 90 && sobel(x, y) < sobel(x, y + 1), 0,
    approx == 90 && sobel(x, y) < sobel(x, y - 1), 0,
    sobel(x, y) < sobel(x + 1, y + 1), 0,
    sobel(x, y) < sobel(x - 1, y - 1), 0,
    sobel(x, y)
  );

  RDom rd_nb = RDom(-1, 3, -1, 3, "rd_neighbors");

  Func hy = Func("hysteresis");
  Expr value = nms(x, y);
  Expr nb_val = maximum(nms(x + rd_nb.x, y + rd_nb.y));
  Expr th_val = select(
    value  < threshold_min, 0,
    value  > threshold_max, 255,
    nb_val > threshold_max, 255,
    value
  );
  hy(x, y) = th_val;

  Func canny = Func("canny");
  Expr hysteresis = hy(x, y);
  canny(x, y, ch) = select(
    ch == 3, 255,
    cast<uint8_t>(hysteresis)
  );

  ks_x.compute_root()
    .parallel(y, 8)
    .vectorize(x, 16);
  ks_y.compute_root()
    .parallel(y, 8)
    .vectorize(x, 16);

  gy.compute_root()
    .parallel(y, 8)
    .vectorize(x, 16);
  gx.compute_root()
    .parallel(y, 8)
    .vectorize(x, 16);

  nms.compute_root()
    .parallel(y, 8)
    .vectorize(x, 8);

  sobel.compute_root();

  hy.compute_root()
    .async()
    .tile(x, y, xo, yo, xi, yi, 32, 32)
    .fuse(xo, yo, ti)
    .parallel(ti)
    .vectorize(xi, 32);

  canny.compute_root();

  gray.compute_root();
  return canny;
}

Func emboss_fn(Func input, Param<int32_t> width, Param<int32_t> height){
  Region src_bounds = {{0, width},{0, height},{0, 4}};
  Func in = read(BoundaryConditions::repeat_edge(input, src_bounds), "in");

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
  Func conv = convolve(in, kernel, rd_kernel);

  Func emboss = Func("emboss");
  emboss(x, y, ch) = select(
    ch == 3, 255,
    clamp(conv(x, y, ch) + 128, 0, 255)
  );

  conv.compute_root()
    .vectorize(x, 32);

  emboss.compute_root()
    .tile(x, y, xo, yo, xi, yi, 32, 32)
    .fuse(xo, yo, ti)
    .parallel(ti)
    .vectorize(xi, 32);

  in.compute_root();

  return emboss;
}

Func laplacian_fn(Func input, Param<int32_t> width, Param<int32_t> height) {
  Var x("x"), y("y");
  Region bounds = {{0, width},{0, height},{0, 4}};

  Func kernel = Func("kernel");
  kernel(x, y) = 0;
  kernel(0, 0) = 0; kernel(1, 0) =  1; kernel(2, 0) = 0;
  kernel(0, 1) = 1; kernel(1, 1) = -4; kernel(2, 1) = 1;
  kernel(0, 2) = 0; kernel(1, 2) =  1; kernel(2, 2) = 0;

  RDom rd_kernel = RDom(0,3, 0,3, "rd_kernel");
  return filter2d_gray(input, bounds, kernel, rd_kernel, "laplacian");
}

Func highpass_fn(Func input, Param<int32_t> width, Param<int32_t> height) {
  Var x("x"), y("y");
  Region bounds = {{0, width},{0, height},{0, 4}};

  Func kernel = Func("kernel");
  kernel(x, y) = 0;
  kernel(0, 0) = -1; kernel(1, 0) = -1; kernel(2, 0) = -1;
  kernel(0, 1) = -1; kernel(1, 1) =  8; kernel(2, 1) = -1;
  kernel(0, 2) = -1; kernel(1, 2) = -1; kernel(2, 2) = -1;

  RDom rd_kernel = RDom(0,3, 0,3, "rd_kernel");
  return filter2d_gray(input, bounds, kernel, rd_kernel, "highpass");
}

// gradient filter
Func gradient_fn(Func input, Param<int32_t> width, Param<int32_t> height) {
  Var x("x"), y("y");
  Region bounds = {{0, width},{0, height},{0, 4}};

  Func kernel = Func("kernel");
  kernel(x, y) = 0;
  kernel(0, 0) =  1; kernel(1, 0) =  1; kernel(2, 0) =  1;
  kernel(0, 1) =  0; kernel(1, 1) =  0; kernel(2, 1) =  0;
  kernel(0, 2) = -1; kernel(1, 2) = -1; kernel(2, 2) = -1;

  RDom rd_kernel = RDom(0,3, 0,3, "rd_kernel");
  return filter2d_gray(input, bounds, kernel, rd_kernel, "gradient");
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
    wrapFunc(buf_src, "buf_src"), width, height, rotation
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
  Param<float> sigma{"sigma", 5.0};

  init_input_rgba(src);

  Func fn = canny_fn(
    src.in(), width, height,
    threshold_max, threshold_min,
    sigma
  );

  init_output_rgba(fn.output_buffer());

  generate_static_link(features, fn, {
    src, width, height,
    threshold_max, threshold_min,
    sigma
  }, fn.name());
}

int jit_canny(char **argv) {
  Buffer<uint8_t> buf_src = load_and_convert_image(argv[2]);

  Param<int32_t> width{"width", buf_src.get()->width()};
  Param<int32_t> height{"height", buf_src.get()->height()};
  Param<int32_t> threshold_max{"threshold_max", std::stoi(argv[3])};
  Param<int32_t> threshold_min{"threshold_min", std::stoi(argv[4])};
  Param<float> sigma{"sigma", std::stof(argv[5])};

  Buffer<uint8_t> out = jit_realize_uint8(canny_fn(
    wrapFunc(buf_src, "buf_src"), width, height,
    threshold_max, threshold_min,
    sigma
  ), buf_src);

  printf("save to %s\n", argv[6]);
  save_image(out, argv[6]);
  return 0;
}

int benchmark_canny(Buffer<uint8_t> buf_src, Param<int32_t> width, Param<int32_t> height) {
  Param<int32_t> threshold_max{"threshold_max", 250};
  Param<int32_t> threshold_min{"threshold_min", 100};
  Param<float> sigma{"sigma", 5.0};
  return jit_benchmark(canny_fn(
    wrapFunc(buf_src, "buf_src"), width, height,
    threshold_max, threshold_min,
    sigma
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
