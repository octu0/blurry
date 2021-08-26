// +build ignore.

#define DEBUG 0

#include "blurry.hpp"

const uint8_t GRAY_R = 76, GRAY_G = 152, GRAY_B = 28;

const Expr CANNY_SIGMA = 5.0f;
const Expr acos_v = -1.0f;
const Expr pi = acos(acos_v);
const Expr ui8_0 = cast<uint8_t>(0);
const Expr ui8_255 = cast<uint8_t>(255);
const Expr float0 = cast<float>(0.f);
const Expr float128 = cast<float>(128.f);
const Expr float255 = cast<float>(255.f);
const Expr degree0 = cast<uint8_t>(0);
const Expr degree45 = cast<uint8_t>(45);
const Expr degree90 = cast<uint8_t>(90);
const Expr degree135 = cast<uint8_t>(135);

const Func kernel_sobel_x = kernel_sobel3x3_x();
const Func kernel_sobel_y = kernel_sobel3x3_y();
const Func kernel_emboss = kernel_emboss3x3();
const Func kernel_laplacian = kernel_laplacian3x3();
const Func kernel_highpass = kernel_highpass3x3();
const Func kernel_gradient = kernel_gradient3x3();
const RDom rd_kernel = RDom(-1, 3, -1, 3, "rd_kernel");

Func kernel_sobel3x3_x() {
  Var x("x"), y("y");

  Func kernel_x = Func("kernel_sobel_x");
  kernel_x(x, y) = 0;
  kernel_x(-1, -1) = -1; kernel_x(0, -1) = 0; kernel_x(1, -1) = 1;
  kernel_x(-1,  0) = -2; kernel_x(0,  0) = 0; kernel_x(1,  0) = 2;
  kernel_x(-1,  1) = -1; kernel_x(0,  1) = 0; kernel_x(1,  1) = 1;

  kernel_x.compute_root();
  return kernel_x;
}

Func kernel_sobel3x3_y() {
  Var x("x"), y("y");

  Func kernel_y = Func("kernel_sobel_y");
  kernel_y(x, y) = 0;
  kernel_y(-1, -1) = -1; kernel_y(0, -1) = -2; kernel_y(1, -1) = -1;
  kernel_y(-1,  0) =  0; kernel_y(0,  0) =  0; kernel_y(1,  0) =  0;
  kernel_y(-1,  1) =  1; kernel_y(0,  1) =  2; kernel_y(1,  1) =  1;

  kernel_y.compute_root();
  return kernel_y;
}

Func kernel_emboss3x3() {
  Var x("x"), y("y");

  Func kernel = Func("kernel_emboss");
  kernel(x, y) = 0;
  kernel(-1, -1) = -1; kernel(0, -1) = -1; kernel(1, -1) = 0;
  kernel(-1,  0) = -1; kernel(0,  0) =  0; kernel(1,  0) = 1;
  kernel(-1,  1) =  0; kernel(0,  1) =  1; kernel(1,  1) = 1;

  kernel.compute_root();
  return kernel;
}

Func kernel_laplacian3x3() {
  Var x("x"), y("y");

  Func kernel = Func("kernel_laplacian");
  kernel(x, y) = 0;
  kernel(-1, -1) = 0; kernel(0, -1) =  1; kernel(1, -1) = 0;
  kernel(-1,  0) = 1; kernel(0,  0) = -4; kernel(1,  0) = 1;
  kernel(-1,  1) = 0; kernel(0,  1) =  1; kernel(1,  1) = 0;

  kernel.compute_root();
  return kernel;
}

Func kernel_highpass3x3() {
  Var x("x"), y("y");

  Func kernel = Func("kernel_highpass");
  kernel(x, y) = 0;
  kernel(-1, -1) = -1; kernel(0, -1) = -1; kernel(1, -1) = -1;
  kernel(-1,  0) = -1; kernel(0,  0) =  8; kernel(1,  0) = -1;
  kernel(-1,  1) = -1; kernel(0,  1) = -1; kernel(1,  1) = -1;

  kernel.compute_root();
  return kernel;
}

Func kernel_gradient3x3() {
  Var x("x"), y("y");

  Func kernel = Func("kernel_gradient");
  kernel(x, y) = 0;
  kernel(-1, -1) =  1; kernel(0, -1) =  1; kernel(1, -1) =  1;
  kernel(-1,  0) =  0; kernel(0,  0) =  0; kernel(1,  0) =  0;
  kernel(-1,  1) = -1; kernel(0,  1) = -1; kernel(1,  1) = -1;

  kernel.compute_root();
  return kernel;
}

Func wrapFunc(Buffer<uint8_t> buf, const char* name) {
  Var x("x"), y("y"), ch("ch");
  Func f = Func(name);
  f(x, y, ch) = buf(x, y, ch);
  return f;
}

Func wrapFunc(Buffer<float> buf, const char* name) {
  Var x("x"), y("y"), ch("ch");
  Func f = Func(name);
  f(x, y, ch) = buf(x, y, ch);
  return f;
}

Func wrapFunc(Buffer<double> buf, const char* name) {
  Var x("x"), y("y"), ch("ch");
  Func f = Func(name);
  f(x, y, ch) = buf(x, y, ch);
  return f;
}

Func wrapFunc_xy(Buffer<uint8_t> buf, const char* name) {
  Var x("x"), y("y");
  Func f = Func(name);
  f(x, y) = buf(x, y);
  return f;
}

Func wrapFunc_xy(Buffer<float> buf, const char* name) {
  Var x("x"), y("y");
  Func f = Func(name);
  f(x, y) = buf(x, y);
  return f;
}

Func wrapFunc_xy(Buffer<double> buf, const char* name) {
  Var x("x"), y("y");
  Func f = Func(name);
  f(x, y) = buf(x, y);
  return f;
}

Func read(Func clamped, const char *name) {
  Var x("x"), y("y"), ch("ch");
  Func read = Func(name);
  read(x, y, ch) = cast<int16_t>(clamped(x, y, ch));
  return read;
}

Func readUI8(Func clamped, const char *name) {
  Var x("x"), y("y"), ch("ch");
  Func read = Func(name);
  read(x, y, ch) = cast<uint8_t>(clamped(x, y, ch));
  return read;
}

// ABGR to RGBA
Func read_from_abgr(Func in, const char *name) {
  Var x("x"), y("y"), ch("ch");
  Func f = Func(name);
  f(x, y, ch) = select(
    ch == 0, in(x, y, 3), // A
    ch == 1, in(x, y, 2), // B
    ch == 2, in(x, y, 1), // G
    likely(in(x, y, 0))   // R
  );
  return f;
}

// ARGB to RGBA
Func read_from_argb(Func in, const char *name) {
  Var x("x"), y("y"), ch("ch");
  Func f = Func(name);
  f(x, y, ch) = select(
    ch == 0, in(x, y, 3), // A
    ch == 1, in(x, y, 0), // R
    ch == 2, in(x, y, 1), // G
    likely(in(x, y, 2))   // B
  );
  return f;
}

// RABG to RGBA
Func read_from_rabg(Func in, const char *name) {
  Var x("x"), y("y"), ch("ch");
  Func f = Func(name);
  f(x, y, ch) = select(
    ch == 0, in(x, y, 0), // R
    ch == 1, in(x, y, 3), // A
    ch == 2, in(x, y, 2), // B
    likely(in(x, y, 1))   // G
  );
  return f;
}

// BGRA to RGBA
Func read_from_bgra(Func in, const char *name) {
  Var x("x"), y("y"), ch("ch");
  Func f = Func(name);
  f(x, y, ch) = select(
    ch == 0, in(x, y, 2), // B
    ch == 1, in(x, y, 1), // G
    ch == 2, in(x, y, 0), // R
    likely(in(x, y, 3))   // A
  );
  return f;
}

Func read_from_i420(Func in_y, Func in_u, Func in_v, const char *name) {
  Var x("x"), y("y"), ch("ch");

  Func yf = Func("y_float");
  yf(x, y) = cast<float>(in_y(x, y) & 0xff);
  Func uf = Func("u_float");
  uf(x, y) = cast<float>((in_u(x / 2, y / 2) & 0xff) - float128);
  Func vf = Func("v_float");
  vf(x, y) = cast<float>((in_v(x / 2, y / 2) & 0xff) - float128);

  Func f = Func(name);

  Expr r = yf(x, y) + (1.370705f * vf(x, y));
  Expr g = yf(x, y) - ((0.698001f * vf(x, y)) - (0.71414f * uf(x, y)));
  Expr b = yf(x, y) + (1.732446f * uf(x, y));

  Expr rr = clamp(r, float0, float255);
  Expr gg = clamp(g, float0, float255);
  Expr bb = clamp(b, float0, float255);

  Expr v = select(
    ch == 0, r,       // R
    ch == 1, g,       // G
    ch == 2, b,       // B
    likely(float255)  // A always 0xff
  );
  f(x, y, ch) = cast<uint8_t>(v);
  return f;
}

Func gray_xy(Func in) {
  Var x("x"), y("y");

  Func gray = Func("gray");
  gray(x, y) = in(x, y, 0); // rgba(r) for grayscale

  gray.compute_root();
  return gray;
}

Func gray_xy_uint8(Func in, const char *name) {
  Var x("x"), y("y");

  Func gray = Func(name);
  gray(x, y) = cast<uint8_t>(in(x, y, 0)); // rgba(r) for grayscale

  gray.compute_root();
  return gray;
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
  Func in = gray_xy_uint8(BoundaryConditions::repeat_edge(input, bounds), "in");

  Var x("x"), y("y"), ch("ch");
  Var xo("xo"), xi("xi");
  Var yo("yo"), yi("yi");
  Var ti("ti");

  Func conv = convolve_xy(in, kernel, rd_kernel);

  Func gradient = Func(name);
  gradient(x, y, ch) = select(
    ch < 3, cast<uint8_t>(conv(x, y) & 128),
    likely(ui8_255)
  );

  conv.compute_root()
    .vectorize(x, 32);

  gradient.compute_at(in, xi)
    .tile(x, y, xo, yo, xi, yi, 32, 32)
    .fuse(xo, yo, ti)
    .parallel(ti, 4)
    .vectorize(xi, 32);

  in.compute_root()
    .unroll(y, 4)
    .vectorize(x, 16);
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

Expr erode(Func in, RDom rd_erode) {
  Var x("x"), y("y");
  Expr val = in(x + rd_erode.x, y + rd_erode.y);
  return minimum(val);
}

Expr dilate(Func in, RDom rd_dilate) {
  Var x("x"), y("y");
  Expr val = in(x + rd_dilate.x, y + rd_dilate.y);
  return maximum(val);
}

Func morphology_open(Func in, Param<int32_t> size) {
  Var x("x"), y("y");

  RDom rd_morph = RDom(-1 * size, (size * 2) + 1, -1 * size, (size * 2) + 1, "rd_morph_open");

  Func erode_tmp = Func("erode_tmp");
  erode_tmp(x, y) = erode(in, rd_morph);
  Func dilate_tmp = Func("dilate_tmp");
  dilate_tmp(x, y) = dilate(erode_tmp, rd_morph);

  Func morph = Func("morph_open");
  morph(x, y) = dilate_tmp(x, y);

  erode_tmp.compute_root()
    .parallel(y, 16)
    .vectorize(x, 32);
  dilate_tmp.compute_root()
    .parallel(y, 16)
    .vectorize(x, 32);

  morph.compute_root()
    .vectorize(x, 32);

  return morph;
}

Func morphology_close(Func in, Param<int32_t> size) {
  Var x("x"), y("y");

  RDom rd_morph = RDom(-1 * size, (size * 2) + 1, -1 * size, (size * 2) + 1, "rd_morph_close");

  Func dilate_tmp = Func("dilate_tmp");
  dilate_tmp(x, y) = dilate(in, rd_morph);
  Func erode_tmp = Func("erode_tmp");
  erode_tmp(x, y) = erode(dilate_tmp, rd_morph);

  Func morph = Func("morph_close");
  morph(x, y) = erode_tmp(x, y);

  dilate_tmp.compute_root()
    .parallel(y, 16)
    .vectorize(x, 32);

  erode_tmp.compute_root()
    .parallel(y, 16)
    .vectorize(x, 32);

  morph.compute_root()
    .vectorize(x, 32);

  return morph;
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
    .tile(x, y, xo, yo, xi, yi, 32, 32)
    .fuse(xo, yo, ti)
    .parallel(ti)
    .vectorize(xi, 32);

  sum_kernel.compute_root();
  return gaussian;
}

Func canny(Func in, Param<int32_t> threshold_max, Param<int32_t> threshold_min) {
  Var x("x"), y("y"), ch("ch");
  Var xo("xo"), xi("xi");
  Var yo("yo"), yi("yi");
  Var ti("ti");

  //
  // gaussian
  //
  RDom gauss_rd = RDom(-1, 3, "gaussian_rdom");
  Func gauss = gaussian(in, CANNY_SIGMA, gauss_rd, "gaussian3x3");

  Func gx = convolve_xy(gauss, kernel_sobel_x, rd_kernel);
  Func gy = convolve_xy(gauss, kernel_sobel_y, rd_kernel);

  //
  // sobel x/y
  //
  Func sobel = Func("sobel");
  Expr pow_gy = fast_pow(abs(gy(x, y)), 2);
  Expr pow_gx = fast_pow(abs(gx(x, y)), 2);
  Expr magnitude = ceil(sqrt(pow_gy + pow_gx));
  sobel(x, y) = magnitude;

  //
  // non max supression
  //
  Func nms = Func("nonmax_supression");
  Expr angle = (atan2(gy(x, y), gx(x, y)) * 180) / pi;
  Expr approx = select(
    angle >=  -22.5f && angle <  22.5f,  degree0,
    angle >=  157.5f && angle <  180.0f, degree0,
    angle >=  180.0f && angle < -157.5f, degree0,
    angle >=   22.5f && angle <   67.5f, degree45,
    angle >= -157.5f && angle < -112.5f, degree45,
    angle >=   67.5f && angle <  112.5f, degree90,
    angle >= -112.5f && angle <  -67.5f, degree90,
    angle >=  112.5f && angle <  157.5f, degree135,
    angle >=  -67.5f && angle <  -22.5f, degree135,
    likely(degree0)
  );
  nms(x, y) = select(
    approx == degree0  && sobel(x, y) < sobel(x + 1, y),     ui8_0,
    approx == degree0  && sobel(x, y) < sobel(x - 1, y),     ui8_0,
    approx == degree45 && sobel(x, y) < sobel(x + 1, y - 1), ui8_0,
    approx == degree45 && sobel(x, y) < sobel(x - 1, y + 1), ui8_0,
    approx == degree90 && sobel(x, y) < sobel(x, y + 1),     ui8_0,
    approx == degree90 && sobel(x, y) < sobel(x, y - 1),     ui8_0,
    sobel(x, y) < sobel(x + 1, y + 1), ui8_0,
    sobel(x, y) < sobel(x - 1, y - 1), ui8_0,
    cast<uint8_t>(sobel(x, y))
  );

  //
  // hysteresis threshold
  //
  RDom rd_nb = RDom(-1, 3, -1, 3, "rd_neighbors");
  Func hy = Func("hysteresis");
  Expr value = nms(x, y);
  Expr nb_val = maximum(nms(x + rd_nb.x, y + rd_nb.y));
  Expr th_val = select(
    value  < threshold_min, ui8_0,
    value  > threshold_max, ui8_255,
    nb_val > threshold_max, ui8_255,
    value
  );
  hy(x, y) = th_val;

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
    .tile(x, y, xo, yo, xi, yi, 32, 32)
    .fuse(xo, yo, ti)
    .parallel(ti)
    .vectorize(xi, 32);

  gauss.compute_root();

  return hy;
}

Func cloneimg_fn(Func input, Param<int32_t> width, Param<int32_t> height) {
  Region src_bounds = {{0, width},{0, height},{0, 4}};
  Func in = readUI8(BoundaryConditions::repeat_edge(input, src_bounds), "in");

  Var x("x"), y("y"), ch("ch");

  //
  // RGBA interleave test
  //
  Func cloneimg = Func("cloneimg");
  cloneimg(x, y, ch) = in(x, y, ch);

  cloneimg.compute_at(in, x)
    .unroll(y, 8)
    .vectorize(x, 16);
  return cloneimg;
}

Func convert_from(Func in, Param<int32_t> width, Param<int32_t> height, const char *name) {
  Var x("x"), y("y"), ch("ch");

  Func convert = Func(name);
  convert(x, y, ch) = in(x, y, ch);

  convert.compute_at(in, x)
    .unroll(y, 8)
    .vectorize(x, 16);
  return convert;
}

// ABGR
Func convert_from_abgr_fn(Func input, Param<int32_t> width, Param<int32_t> height) {
  return convert_from(
    read_from_abgr(input, "in"),
    width,
    height,
    "convert_from_abgr"
  );
}

// ARGB
Func convert_from_argb_fn(Func input, Param<int32_t> width, Param<int32_t> height) {
  return convert_from(
    read_from_argb(input, "in"),
    width,
    height,
    "convert_from_argb"
  );
}

// BGRA
Func convert_from_bgra_fn(Func input, Param<int32_t> width, Param<int32_t> height) {
  return convert_from(
    read_from_bgra(input, "in"),
    width,
    height,
    "convert_from_bgra"
  );
}

// BGRA little endian (argb in memory) at libyuv
Func convert_from_rabg_fn(Func input, Param<int32_t> width, Param<int32_t> height) {
  return convert_from(
    read_from_rabg(input, "in"),
    width,
    height,
    "convert_from_rabg"
  );
}

Func convert_from_i420_fn(Func in_y, Func in_u, Func in_v, Param<int32_t> width, Param<int32_t> height) {
  return convert_from(
    read_from_i420(in_y, in_u, in_v, "in"),
    width,
    height,
    "convert_from_i420"
  );
}

Func rotate0_fn(Func input, Param<int32_t> width, Param<int32_t> height) {
  Region src_bounds = {{0, width},{0, height},{0, 4}};
  Func in = readUI8(BoundaryConditions::repeat_edge(input, src_bounds), "in");

  Var x("x"), y("y"), ch("ch");

  // same cloneimg
  Func rotate = Func("rotate0");
  rotate(x, y, ch) = in(x, y, ch);

  rotate.compute_at(in, x)
    .unroll(y, 8)
    .vectorize(x, 16);
  return rotate;
}

Func rotate90_fn(Func input, Param<int32_t> width, Param<int32_t> height) {
  Region src_bounds = {{0, width},{0, height},{0, 4}};
  Func in = readUI8(BoundaryConditions::constant_exterior(input, 0, src_bounds), "in");

  Var x("x"), y("y"), ch("ch");
  Func rotate = Func("rotate90");
  rotate(x, y, ch) = in(y, (height - 1) - x, ch);

  rotate.compute_at(in, y)
    .unroll(x, 8)
    .vectorize(y, 16);
  return rotate;
}

Func rotate180_fn(Func input, Param<int32_t> width, Param<int32_t> height) {
  Region src_bounds = {{0, width},{0, height},{0, 4}};
  Func in = readUI8(BoundaryConditions::constant_exterior(input, 0, src_bounds), "in");

  Var x("x"), y("y"), ch("ch");
  Func rotate = Func("rotate180");
  rotate(x, y, ch) = in((width - 1) - x, (height - 1) - y, ch);

  rotate.compute_at(in, x)
    .unroll(y, 8)
    .vectorize(x, 16);
  return rotate;
}

Func rotate270_fn(Func input, Param<int32_t> width, Param<int32_t> height) {
  Region src_bounds = {{0, width},{0, height},{0, 4}};
  Func in = readUI8(BoundaryConditions::constant_exterior(input, 0, src_bounds), "in");

  Var x("x"), y("y"), ch("ch");
  Func rotate = Func("rotate270");
  rotate(x, y, ch) = in((width - 1) - y, x, ch);

  rotate.compute_at(in, y)
    .unroll(x, 8)
    .vectorize(y, 16);
  return rotate;
}

Func blend(
  Func source, Func blended,
  Param<int32_t> width0, Param<int32_t> height0,
  Param<int32_t> width1, Param<int32_t> height1,
  Param<int32_t> px, Param<int32_t> py,
  const char *name
) {
  Var x("x"), y("y"), ch("ch");
  Var xo("xo"), xi("xi");
  Var yo("yo"), yi("yi");
  Var ti("ti");

  Expr px_min = px;
  Expr px_max = px + width1;
  Expr py_min = py;
  Expr py_max = py + height1;

  Func f = Func(name);
  Expr value = select(
    ch == 3, 255,
    px_min <= x && x <= px_max && py_min <= y && y <= py_max, blended(x, y, ch),
    source(x, y, ch)
  );
  f(x, y, ch) = cast<uint8_t>(value);

  f.compute_root()
    .tile(x, y, xo, yo, xi, yi, 32, 32)
    .fuse(xo, yo, ti)
    .parallel(ch)
    .parallel(ti, 8)
    .vectorize(xi, 32);

  source.compute_at(f, xi)
    .unroll(y, 8)
    .vectorize(x, 32);
  blended.compute_at(f, xi)
    .unroll(y, 8)
    .vectorize(x, 32);

  return f;
}

Func blend_normal(Func in0, Func in1, Param<int32_t> px, Param<int32_t> py) {
  Var x("x"), y("y"), ch("ch");
  Func f = Func("bld_normal");
  f(x, y, ch) = in1(x - px, y - py, ch);
  return f;
}

Func blend_sub(Func in0, Func in1, Param<int32_t> px, Param<int32_t> py) {
  Var x("x"), y("y"), ch("ch");
  Func f = Func("bld_sub");
  f(x, y, ch) = in0(x, y, ch) - in1(x - px, y - py, ch);
  return f;
}

Func blend_add(Func in0, Func in1, Param<int32_t> px, Param<int32_t> py) {
  Var x("x"), y("y"), ch("ch");
  Func f = Func("bld_add");
  f(x, y, ch) = in0(x, y, ch) + in1(x - px, y - py, ch);
  return f;
}

Func blend_diff(Func in0, Func in1, Param<int32_t> px, Param<int32_t> py) {
  Var x("x"), y("y"), ch("ch");
  Func f = Func("bld_diff");
  f(x, y, ch) = cast<uint8_t>(abs(in0(x, y, ch) - in1(x - px, y - py, ch)));
  return f;
}

Func blend_normal_fn(
  Func src0, Param<int32_t> width0, Param<int32_t> height0,
  Func src1, Param<int32_t> width1, Param<int32_t> height1,
  Param<int32_t> px, Param<int32_t> py
) {
  Region src0_bounds = {{0, width0},{0, height0},{0, 4}};
  Region src1_bounds = {{0, width1},{0, height1},{0, 4}};

  Func in0 = readUI8(BoundaryConditions::repeat_edge(src0, src0_bounds), "in0");
  Func in1 = readUI8(BoundaryConditions::repeat_edge(src1, src1_bounds), "in1");

  return blend(
    in0, blend_normal(in0, in1, px, py),
    width0, height0,
    width1, height1,
    px, py,
    "blend_normal"
  );
}

Func blend_sub_fn(
  Func src0, Param<int32_t> width0, Param<int32_t> height0,
  Func src1, Param<int32_t> width1, Param<int32_t> height1,
  Param<int32_t> px, Param<int32_t> py
) {
  Region src0_bounds = {{0, width0},{0, height0},{0, 4}};
  Region src1_bounds = {{0, width1},{0, height1},{0, 4}};

  Func in0 = readUI8(BoundaryConditions::repeat_edge(src0, src0_bounds), "in0");
  Func in1 = readUI8(BoundaryConditions::repeat_edge(src1, src1_bounds), "in1");

  return blend(
    in0, blend_sub(in0, in1, px, py),
    width0, height0,
    width1, height1,
    px, py,
    "blend_sub"
  );
}

Func blend_add_fn(
  Func src0, Param<int32_t> width0, Param<int32_t> height0,
  Func src1, Param<int32_t> width1, Param<int32_t> height1,
  Param<int32_t> px, Param<int32_t> py
) {
  Region src0_bounds = {{0, width0},{0, height0},{0, 4}};
  Region src1_bounds = {{0, width1},{0, height1},{0, 4}};

  Func in0 = readUI8(BoundaryConditions::repeat_edge(src0, src0_bounds), "in0");
  Func in1 = readUI8(BoundaryConditions::repeat_edge(src1, src1_bounds), "in1");

  return blend(
    in0, blend_add(in0, in1, px, py),
    width0, height0,
    width1, height1,
    px, py,
    "blend_add"
  );
}

Func blend_diff_fn(
  Func src0, Param<int32_t> width0, Param<int32_t> height0,
  Func src1, Param<int32_t> width1, Param<int32_t> height1,
  Param<int32_t> px, Param<int32_t> py
) {
  Region src0_bounds = {{0, width0},{0, height0},{0, 4}};
  Region src1_bounds = {{0, width1},{0, height1},{0, 4}};

  Func c0 = BoundaryConditions::repeat_edge(src0, src0_bounds);
  Func c1 = BoundaryConditions::repeat_edge(src1, src1_bounds);
  Func in0 = read(c0, "in0");
  Func in1 = read(c1, "in1");

  return blend(
    readUI8(c0, "in0_u8"), blend_diff(in0, in1, px, py),
    width0, height0,
    width1, height1,
    px, py,
    "blend_diff"
  );
}

Func erosion_fn(Func input, Param<int32_t> width, Param<int32_t> height, Param<int32_t> size) {
  Region src_bounds = {{0, width},{0, height},{0, 4}};
  Func in = readUI8(BoundaryConditions::repeat_edge(input, src_bounds), "in");

  Var x("x"), y("y"), ch("ch");
  Var xo("xo"), xi("xi");
  Var yo("yo"), yi("yi");
  Var ti("ti");

  RDom rd = RDom(-1 * size, (size * 2) + 1, -1 * size, (size * 2) + 1, "rd_erode");
  Func erosion = Func("erosion");
  Expr value = in(x + rd.x, y + rd.y, ch);
  erosion(x, y, ch) = minimum(value);

  erosion.compute_at(in, x)
    .tile(x, y, xo, yo, xi, yi, 32, 32)
    .fuse(xo, yo, ti)
    .parallel(ch)
    .parallel(ti, 4)
    .vectorize(xi, 32);

  in.compute_root();
  return erosion;
}

Func dilation_fn(Func input, Param<int32_t> width, Param<int32_t> height, Param<int32_t> size) {
  Region src_bounds = {{0, width},{0, height},{0, 4}};
  Func in = readUI8(BoundaryConditions::repeat_edge(input, src_bounds), "in");

  Var x("x"), y("y"), ch("ch");
  Var xo("xo"), xi("xi");
  Var yo("yo"), yi("yi");
  Var ti("ti");

  RDom rd = RDom(-1 * size, (size * 2) + 1, -1 * size, (size * 2) + 1, "rd_dilate");
  Func dilation = Func("dilation");
  Expr value = in(x + rd.x, y + rd.y, ch);
  dilation(x, y, ch) = maximum(value);

  dilation.compute_root()
    .tile(x, y, xo, yo, xi, yi, 32, 32)
    .fuse(xo, yo, ti)
    .parallel(ch)
    .parallel(ti, 4)
    .vectorize(xi, 32);

  in.compute_root();
  return dilation;
}

Func morphology_open_fn(
  Func input, Param<int32_t> width, Param<int32_t> height,
  Param<int32_t> size
) {
  Region src_bounds = {{0, width},{0, height},{0, 4}};
  Func in = gray_xy_uint8(BoundaryConditions::mirror_image(input, src_bounds), "in");

  Var x("x"), y("y"), ch("ch");

  Func f = morphology_open(in, size);
  Func morph = Func("morphology_open");
  morph(x, y, ch) = select(
    ch == 3, 255,
    f(x, y)
  );

  return morph;
}

Func morphology_close_fn(
  Func input, Param<int32_t> width, Param<int32_t> height,
  Param<int32_t> size
) {
  Region src_bounds = {{0, width},{0, height},{0, 4}};
  Func in = gray_xy_uint8(BoundaryConditions::mirror_image(input, src_bounds), "in");

  Var x("x"), y("y"), ch("ch");

  Func f = morphology_close(in, size);
  Func morph = Func("morphology_close");
  morph(x, y, ch) = select(
    ch == 3, 255,
    f(x, y)
  );

  return morph;
}

Func morphology_gradient_fn(
  Func input, Param<int32_t> width, Param<int32_t> height,
  Param<int32_t> size
) {
  Region src_bounds = {{0, width},{0, height},{0, 4}};
  Func in = gray_xy_uint8(BoundaryConditions::mirror_image(input, src_bounds), "in");

  Var x("x"), y("y"), ch("ch");

  RDom rd_morph = RDom(0, size, 0, size, "rd_morph_gradient");
  Func morph = Func("morphology_gradient");
  Expr val_dilate = dilate(in, rd_morph);
  Expr val_erode = erode(in, rd_morph);
  morph(x, y, ch) = val_dilate - val_erode;

  morph.compute_root()
    .parallel(y, 16)
    .vectorize(x, 32);

  return morph;
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

  grayscale(x, y, ch) = select(
    ch < 3, cast<uint8_t>(value),
    likely(ui8_255)
  );

  grayscale.compute_at(in, xi)
    .tile(x, y, xo, yo, xi, yi, 32, 32)
    .fuse(xo, yo, ti)
    .parallel(ch)
    .parallel(ti, 4)
    .vectorize(xi, 32);

  in.compute_root()
    .unroll(y, 8)
    .vectorize(x, 16);

  return grayscale;
}

Func invert_fn(Func input, Param<int32_t> width, Param<int32_t> height) {
  Region src_bounds = {{0, width},{0, height},{0, 4}};
  Func in = readUI8(BoundaryConditions::repeat_edge(input, src_bounds), "in");

  Var x("x"), y("y"), ch("ch");
  Var xo("xo"), xi("xi");
  Var yo("yo"), yi("yi");
  Var ti("ti");

  Func invert = Func("invert");
  invert(x, y, ch) = select(
    ch < 3, ui8_255 - in(x, y, ch),
    likely(ui8_255)
  );

  invert.compute_at(in, xi)
    .tile(x, y, xo, yo, xi, yi, 32, 32)
    .fuse(xo, yo, ti)
    .parallel(ch)
    .parallel(ti, 4)
    .vectorize(xi, 32);

  in.compute_root()
    .unroll(y, 8)
    .vectorize(x, 16);

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

  brightness.compute_at(in, xi)
    .tile(x, y, xo, yo, xi, yi, 32, 32)
    .fuse(xo, yo, ti)
    .parallel(ch)
    .parallel(ti, 8)
    .vectorize(xi, 32);

  in.compute_root()
    .unroll(y, 8)
    .vectorize(x, 16);

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

  gammacorrection(x, y, ch) = select(
    ch < 3, cast<uint8_t>(value),
    likely(ui8_255)
  );

  gammacorrection.compute_at(in, xi)
    .tile(x, y, xo, yo, xi, yi, 32, 32)
    .fuse(xo, yo, ti)
    .parallel(ch)
    .parallel(ti, 8)
    .vectorize(xi, 32);

  in.compute_root()
    .unroll(y, 8)
    .vectorize(x, 16);

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

  contrast.compute_at(in, xi)
    .tile(x, y, xo, yo, xi, yi, 32, 32)
    .fuse(xo, yo, ti)
    .parallel(ch)
    .parallel(ti, 8)
    .vectorize(xi, 32);

  in.compute_root()
    .unroll(y, 8)
    .vectorize(x, 16);

  return contrast;
}

Func boxblur_fn(Func input, Param<int32_t> width, Param<int32_t> height, Param<uint8_t> size) {
  Region src_bounds = {{0, width},{0, height},{0, 4}};
  Func in = read(BoundaryConditions::repeat_edge(input, src_bounds), "in");

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
  blur_y.compute_at(boxblur, yi)
    .parallel(y, 8)
    .vectorize(x, 32);

  boxblur.compute_at(in, ti)
    .tile(x, y, xo, yo, xi, yi, 32, 32)
    .fuse(xo, yo, ti)
    .parallel(ch)
    .parallel(ti, 8)
    .vectorize(xi, 32);

  in.compute_at(blur_x, y)
    .unroll(y, 8)
    .vectorize(x, 32);
  return boxblur;
}

Func gaussianblur_fn(Func input, Param<int32_t> width, Param<int32_t> height, Param<float> sigma){
  Region src_bounds = {{0, width},{0, height},{0, 4}};
  Func in = read(BoundaryConditions::repeat_edge(input, src_bounds), "in");

  Expr radius = cast<int16_t>(ceil(sigma * 3));
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
    .unroll(x, 4);

  sum_kernel.compute_at(gaussianblur, ti)
    .unroll(x, 8);

  gaussianblur.compute_at(in, ti)
    .tile(x, y, xo, yo, xi, yi, 32, 32)
    .fuse(xo, yo, ti)
    .parallel(ch)
    .parallel(ti, 16)
    .vectorize(xi, 32);

  in.compute_root()
    .unroll(y, 8)
    .vectorize(x, 16);

  return gaussianblur;
}

Func edge_fn(Func input, Param<int32_t> width, Param<int32_t> height){
  Region src_bounds = {{0, width},{0, height},{0, 4}};
  Func in = gray_xy_uint8(BoundaryConditions::repeat_edge(input, src_bounds), "in");

  Var x("x"), y("y"), ch("ch");

  Func gy = Func("gy");
  gy(x, y) = (in(x, y + 1) - in(x, y - 1)) / 2;

  Func gx = Func("gx");
  gx(x, y) = (in(x + 1, y) - in(x - 1, y)) / 2;

  Func edge = Func("edgedetect");
  Expr pow_gy = fast_pow(gy(x, y), 2);
  Expr pow_gx = fast_pow(gx(x, y), 2);
  Expr magnitude = pow_gy + pow_gx;
  edge(x, y, ch) = select(
    ch < 3, cast<uint8_t>(magnitude),
    likely(ui8_255)
  );

  gy.compute_at(edge, x)
    .parallel(y)
    .vectorize(x, 8);
  gx.compute_at(edge, x)
    .parallel(x)
    .vectorize(y, 8);

  edge.compute_root()
    .parallel(ch)
    .parallel(y, 8);

  in.compute_root()
    .unroll(y, 8)
    .vectorize(x, 16);
  return edge;
}

Func sobel_fn(Func input, Param<int32_t> width, Param<int32_t> height){
  Region src_bounds = {{0, width},{0, height},{0, 4}};
  Func in = gray_xy_uint8(BoundaryConditions::repeat_edge(input, src_bounds), "in");

  Var x("x"), y("y"), ch("ch");

  Func gy = Func("grad_y");
  Expr gy_in = in(x + rd_kernel.x, y + rd_kernel.y);
  Expr gy_val = kernel_sobel_y(rd_kernel.x, rd_kernel.y);
  gy(x, y) += gy_in * gy_val;

  Func gx = Func("grad_x");
  Expr gx_in = in(x + rd_kernel.x, y + rd_kernel.y);
  Expr gx_val = kernel_sobel_x(rd_kernel.x, rd_kernel.y);
  gx(x, y) += gx_in * gx_val;

  Func sobel = Func("sobel");
  Expr pow_gy = fast_pow(abs(gy(x, y)), 2);
  Expr pow_gx = fast_pow(abs(gx(x, y)), 2);
  Expr magnitude = ceil(sqrt(pow_gy + pow_gx));
  sobel(x, y, ch) = select(
    ch < 3, cast<uint8_t>(magnitude),
    likely(ui8_255)
  );

  gy.compute_at(sobel, x)
    .parallel(y, 16)
    .vectorize(x, 3);
  gx.compute_at(sobel, x)
    .parallel(y, 16)
    .vectorize(x, 3);

  sobel.compute_root()
    .parallel(ch)
    .parallel(y, 8);

  return sobel;
}

Func canny_fn(
  Func input, Param<int32_t> width, Param<int32_t> height,
  Param<int32_t> threshold_max, Param<int32_t> threshold_min
) {
  Region src_bounds = {{0, width},{0, height},{0, 4}};
  Func in = gray_xy_uint8(BoundaryConditions::repeat_edge(input, src_bounds), "in");

  Var x("x"), y("y"), ch("ch");

  Func hy = canny(in, threshold_max, threshold_min);

  Func canny = Func("canny");
  Expr hysteresis = hy(x, y);

  canny(x, y, ch) = select(
    ch < 3, cast<uint8_t>(hysteresis),
    likely(ui8_255)
  );

  canny.compute_root()
    .parallel(y, 16)
    .vectorize(x, 32);

  return canny;
}

Func canny_dilate_fn(
  Func input, Param<int32_t> width, Param<int32_t> height,
  Param<int32_t> threshold_max, Param<int32_t> threshold_min,
  Param<int32_t> dilate_size
) {
  Region src_bounds = {{0, width},{0, height},{0, 4}};
  Func in = gray_xy_uint8(BoundaryConditions::repeat_edge(input, src_bounds), "in");

  Var x("x"), y("y"), ch("ch");

  Func hy = canny(in, threshold_max, threshold_min);

  Func canny = Func("canny_dilate");
  RDom rd_dilate = RDom(0, dilate_size, 0, dilate_size, "rd_canny_dilate");
  Expr hysteresis_dilate = dilate(hy, rd_dilate);

  canny(x, y, ch) = select(
    ch < 3, cast<uint8_t>(hysteresis_dilate),
    likely(ui8_255)
  );

  canny.compute_root()
    .parallel(y, 16)
    .vectorize(x, 32);

  return canny;
}

Func canny_morphology_open_fn(
  Func input, Param<int32_t> width, Param<int32_t> height,
  Param<int32_t> threshold_max, Param<int32_t> threshold_min,
  Param<int32_t> morphology_size,
  Param<int32_t> dilate_size
) {
  Region src_bounds = {{0, width},{0, height},{0, 4}};
  Func in = gray_xy_uint8(BoundaryConditions::repeat_edge(input, src_bounds), "in");

  Var x("x"), y("y"), ch("ch");

  Func morph = morphology_open(in, morphology_size);

  Func hy = canny(morph, threshold_max, threshold_min);

  Func canny = Func("canny_morphology_open");
  RDom rd_dilate = RDom(0, dilate_size, 0, dilate_size, "rd_canny_dilate");
  Expr hysteresis_dilate = dilate(hy, rd_dilate);

  canny(x, y, ch) = select(
    ch < 3, cast<uint8_t>(hysteresis_dilate),
    likely(ui8_255)
  );

  canny.compute_root()
    .parallel(y, 16)
    .vectorize(x, 32);

  return canny;
}

Func canny_morphology_close_fn(
  Func input, Param<int32_t> width, Param<int32_t> height,
  Param<int32_t> threshold_max, Param<int32_t> threshold_min,
  Param<int32_t> morphology_size,
  Param<int32_t> dilate_size
) {
  Region src_bounds = {{0, width},{0, height},{0, 4}};
  Func in = gray_xy_uint8(BoundaryConditions::repeat_edge(input, src_bounds), "in");

  Var x("x"), y("y"), ch("ch");

  Func morph = morphology_close(in, morphology_size);

  Func hy = canny(morph, threshold_max, threshold_min);

  Func canny = Func("canny_morphology_close");
  RDom rd_dilate = RDom(0, dilate_size, 0, dilate_size, "rd_canny_dilate");
  Expr hysteresis_dilate = dilate(hy, rd_dilate);

  canny(x, y, ch) = select(
    ch < 3, cast<uint8_t>(hysteresis_dilate),
    likely(ui8_255)
  );

  canny.compute_root()
    .parallel(y, 16)
    .vectorize(x, 32);

  return canny;
}

Func emboss_fn(Func input, Param<int32_t> width, Param<int32_t> height){
  Region src_bounds = {{0, width},{0, height},{0, 4}};
  Func in = read(BoundaryConditions::repeat_edge(input, src_bounds), "in");

  Var x("x"), y("y"), ch("ch");
  Var xo("xo"), xi("xi");
  Var yo("yo"), yi("yi");
  Var ti("ti");

  Func conv = convolve(in, kernel_emboss, rd_kernel);

  Func emboss = Func("emboss");
  emboss(x, y, ch) = select(
    ch < 3, clamp(conv(x, y, ch) + 128, 0, 255),
    likely(ui8_255)
  );

  conv.compute_root()
    .vectorize(x, 32);

  emboss.compute_at(in, xi)
    .tile(x, y, xo, yo, xi, yi, 32, 32)
    .fuse(xo, yo, ti)
    .parallel(ti, 4)
    .vectorize(xi, 32);

  in.compute_root()
    .unroll(y, 4)
    .vectorize(x, 16);

  return emboss;
}

Func laplacian_fn(Func input, Param<int32_t> width, Param<int32_t> height) {
  Var x("x"), y("y");
  Region bounds = {{0, width},{0, height},{0, 4}};

  return filter2d_gray(input, bounds, kernel_laplacian, rd_kernel, "laplacian");
}

Func highpass_fn(Func input, Param<int32_t> width, Param<int32_t> height) {
  Var x("x"), y("y");
  Region bounds = {{0, width},{0, height},{0, 4}};

  return filter2d_gray(input, bounds, kernel_highpass, rd_kernel, "highpass");
}

// gradient filter
Func gradient_fn(Func input, Param<int32_t> width, Param<int32_t> height) {
  Var x("x"), y("y");
  Region bounds = {{0, width},{0, height},{0, 4}};

  return filter2d_gray(input, bounds, kernel_gradient, rd_kernel, "gradient");
}

Func blockmozaic_fn(Func input, Param<int32_t> width, Param<int32_t> height, Param<int32_t> block_size){
  Region src_bounds = {{0, width},{0, height},{0, 4}};
  Func in = read(BoundaryConditions::repeat_edge(input, src_bounds), "in");

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
    ch < 3, block_color(x, y, ch) / base,
    likely(float255)
  );

  Func blockmozaic = Func("blockmozaic");
  Expr avg_val = avg_color(x + x % block_size, y + y % block_size, ch);
  blockmozaic(x, y, ch) = cast<uint8_t>(avg_val);

  avg_color.compute_root()
    .parallel(y, 8)
    .vectorize(x, 32);

  blockmozaic.compute_at(in, ti)
    .tile(x, y, xo, yo, xi, yi, 32, 32)
    .fuse(xo, yo, ti)
    .parallel(ch)
    .parallel(ti, 8)
    .vectorize(xi, 32);

  in.compute_at(avg_color, y)
    .unroll(y, 8)
    .vectorize(x, 16);

  return blockmozaic;
}

Func match_template_sad_fn(
  Func input, Param<int32_t> width, Param<int32_t> height,
  Func tpl, Param<int32_t> tpl_width, Param<int32_t> tpl_height
) {
  Region src_bounds = {{0, width},{0, height},{0, 4}};
  Region tpl_bounds = {{0, tpl_width},{0, tpl_height},{0, 4}};
  Func in = read(BoundaryConditions::repeat_edge(input, src_bounds), "in");
  Func t = read(BoundaryConditions::repeat_edge(tpl, tpl_bounds), "tpl");

  Var x("x"), y("y"), ch("ch");
  Var xo("xo"), xi("xi");
  Var yo("yo"), yi("yi");
  Var ti("ti");

  RDom rd_template = RDom(0, tpl_width, 0, tpl_height, "rd_template");
  Func sad = Func("sad");
  Expr diff_r = abs(in(x + rd_template.x, y + rd_template.y, 0) - t(rd_template.x, rd_template.y, 0));
  Expr diff_g = abs(in(x + rd_template.x, y + rd_template.y, 1) - t(rd_template.x, rd_template.y, 1));
  Expr diff_b = abs(in(x + rd_template.x, y + rd_template.y, 2) - t(rd_template.x, rd_template.y, 2));
  Expr value = diff_r + diff_g + diff_b;
  sad(x, y) += value;

  Func match = Func("match_template_sad");
  match(x, y) = cast<uint16_t>(sad(x, y));
  
  match.compute_at(in, ti)
    .tile(x, y, xo, yo, xi, yi, 32, 32)
    .fuse(xo, yo, ti)
    .parallel(ti)
    .vectorize(xi, 32);
  in.compute_at(match, ti)
    .unroll(y, 4)
    .vectorize(x, 32);
  t.compute_at(match, ti)
    .unroll(y, 4)
    .vectorize(x, 32);
  return match;
}

Func match_template_ssd_fn(
  Func input, Param<int32_t> width, Param<int32_t> height,
  Func tpl, Param<int32_t> tpl_width, Param<int32_t> tpl_height
) {
  Region src_bounds = {{0, width},{0, height},{0, 4}};
  Region tpl_bounds = {{0, tpl_width},{0, tpl_height},{0, 4}};
  Func in = read(BoundaryConditions::repeat_edge(input, src_bounds), "in");
  Func t = read(BoundaryConditions::repeat_edge(tpl, tpl_bounds), "tpl");

  Var x("x"), y("y"), ch("ch");
  Var xo("xo"), xi("xi");
  Var yo("yo"), yi("yi");
  Var ti("ti");

  RDom rd_template = RDom(0, tpl_width, 0, tpl_height, "rd_template");
  Func ssd = Func("ssd");
  Expr diff_r = in(x + rd_template.x, y + rd_template.y, 0) - t(rd_template.x, rd_template.y, 0);
  Expr diff_g = in(x + rd_template.x, y + rd_template.y, 1) - t(rd_template.x, rd_template.y, 1);
  Expr diff_b = in(x + rd_template.x, y + rd_template.y, 2) - t(rd_template.x, rd_template.y, 2);
  Expr value = fast_pow(diff_r + diff_g + diff_b, 2);
  ssd(x, y) += value;

  Func match = Func("match_template_ssd");
  match(x, y) = cast<int32_t>(ssd(x, y));
 
  match.compute_at(in, ti)
    .tile(x, y, xo, yo, xi, yi, 32, 32)
    .fuse(xo, yo, ti)
    .parallel(ti)
    .vectorize(xi, 32);
  in.compute_at(match, ti)
    .unroll(y, 4)
    .vectorize(x, 16);
  t.compute_at(match, ti)
    .unroll(y, 4)
    .vectorize(x, 16);
  return match;
}

Func prepare_ncc_template_fn(
  Func tpl, Param<int32_t> tpl_width, Param<int32_t> tpl_height
) {
  Region tpl_bounds = {{0, tpl_width},{0, tpl_height},{0, 4}};
  Func t = gray_xy_uint8(BoundaryConditions::constant_exterior(tpl, 0, tpl_bounds), "tpl");

  Var x("x"), y("y"), ch("ch");
  Var xo("xo"), xi("xi");
  Var yo("yo"), yi("yi");
  Var ti("ti");

  Func serialize = Func("prepare_ncc_template");
  RDom rd_template = RDom(0, tpl_width, 0, tpl_height, "rd_template");
  Expr tpl_val = cast<float>(t(rd_template.x, rd_template.y));
  Expr serialize_sum = sum(fast_pow(tpl_val, 2));

  serialize(x, y) = Tuple(cast<float>(t(x, y)), serialize_sum);

  serialize.compute_root()
    .tile(x, y, xo, yo, xi, yi, 16, 16)
    .fuse(xo, yo, ti)
    .parallel(ti)
    .vectorize(xi, 16);
  return serialize;
}

Func prepared_match_template_ncc_fn(
  Func input, Param<int32_t> width, Param<int32_t> height,
  Func buf_tpl_val, Func buf_tpl_sum,
  Param<int32_t> tpl_width, Param<int32_t> tpl_height
){
  Region src_bounds = {{0, width},{0, height},{0, 4}};
  Region tpl_bounds = {{0, tpl_width},{0, tpl_height},{0, 4}};

  Func in = gray_xy_uint8(BoundaryConditions::constant_exterior(input, 0, src_bounds), "in");
  Var x("x"), y("y"), ch("ch");
  Var xo("xo"), xi("xi");
  Var yo("yo"), yi("yi");
  Var ti("ti");

  RDom rd_template = RDom(0, tpl_width, 0, tpl_height, "rd_template");

  Func match = Func("prepared_match_template_ncc");
  Expr src_val = cast<float>(in(x + rd_template.x, y + rd_template.y));
  Expr vector = sum(src_val * buf_tpl_val(rd_template.x, rd_template.y));
  Expr src_mag = sum(fast_pow(src_val, 2));
  Expr tpl_mag = buf_tpl_sum(0, 0);

  match(x, y) = cast<double>(vector / sqrt(src_mag * tpl_mag));

  match.compute_at(in, ti)
    .tile(x, y, xo, yo, xi, yi, 32, 32)
    .fuse(xo, yo, ti)
    .parallel(ti)
    .vectorize(xi, 32);
  in.compute_at(match, ti)
    .unroll(y, 8)
    .vectorize(x, 16);
  buf_tpl_val.compute_root();
  buf_tpl_sum.compute_root();
  return match;
}

Func match_template_ncc_fn(
  Func input, Param<int32_t> width, Param<int32_t> height,
  Func tpl, Param<int32_t> tpl_width, Param<int32_t> tpl_height
) {
  Region src_bounds = {{0, width},{0, height},{0, 4}};
  Region tpl_bounds = {{0, tpl_width},{0, tpl_height},{0, 4}};
  Func in = gray_xy_uint8(BoundaryConditions::constant_exterior(input, 0, src_bounds), "in");
  Func t = gray_xy_uint8(BoundaryConditions::constant_exterior(tpl, 0, tpl_bounds), "tpl");

  Var x("x"), y("y"), ch("ch");
  Var xo("xo"), xi("xi");
  Var yo("yo"), yi("yi");
  Var ti("ti");

  RDom rd_template = RDom(0, tpl_width, 0, tpl_height, "rd_template");

  Func match = Func("match_template_ncc");
  Expr src_val = cast<float>(in(x + rd_template.x, y + rd_template.y));
  Expr tpl_val = cast<float>(t(rd_template.x, rd_template.y));

  Expr vector = sum(src_val * tpl_val);
  Expr src_mag = sum(fast_pow(src_val, 2));
  Expr tpl_mag = sum(fast_pow(tpl_val, 2));

  match(x, y) = cast<double>(vector / sqrt(src_mag * tpl_mag));

  match.compute_at(in, ti)
    .tile(x, y, xo, yo, xi, yi, 32, 32)
    .fuse(xo, yo, ti)
    .parallel(ti)
    .vectorize(xi, 32);
  in.compute_at(match, ti)
    .unroll(y, 4)
    .vectorize(x, 16);
  t.compute_at(match, ti)
    .unroll(y, 4)
    .vectorize(x, 16);
  return match;
}

Func zncc_avg(Func in, RDom rd, Expr size) {
  Var x("x"), y("y");
  Func avg = Func("zncc_avg");
  Expr val = cast<float>(in(x + rd.x, y + rd.y));
  avg(x, y) = cast<float>(sum(val)) / size;
  return avg;
}

Func zncc_avg_tpl(Func in, RDom rd, Expr size) {
  Func avg = Func("zncc_avg_tpl");
  Expr val = cast<float>(in(rd.x, rd.y));
  avg(_) = cast<float>(sum(val)) / size;
  return avg;
}

Func zncc_stddev(Func in, RDom rd, Expr size, Func avg) {
  Var x("x"), y("y");
  Func stddev = Func("zncc_stddev");
  Expr val = cast<float>(in(x + rd.x, y + rd.y));
  Expr s = sum(fast_pow(val - avg(x, y), 2));
  stddev(x, y) = cast<float>(s);
  return stddev;
}

Func zncc_stddev_tpl(Func in, RDom rd, Expr size, Func avg) {
  Func stddev = Func("zncc_stddev_tpl");
  Expr val = cast<float>(in(rd.x, rd.y));
  Expr s = sum(fast_pow(val - avg(_), 2));
  stddev(_) = cast<float>(s);
  return stddev;
}

Func prepare_zncc_template_fn(
  Func tpl, Param<int32_t> tpl_width, Param<int32_t> tpl_height
) {
  Region tpl_bounds = {{0, tpl_width},{0, tpl_height},{0, 4}};
  Func t = gray_xy_uint8(BoundaryConditions::constant_exterior(tpl, 0, tpl_bounds), "tpl");

  Var x("x"), y("y"), ch("ch");
  Var xo("xo"), xi("xi");
  Var yo("yo"), yi("yi");
  Var ti("ti");

  Func serialize = Func("prepare_zncc_template");
  Expr tpl_size = cast<float>(tpl_width * tpl_height);
  RDom rd_template = RDom(0, tpl_width, 0, tpl_height, "rd_template");
  Func tpl_avg = zncc_avg_tpl(t, rd_template, tpl_size);
  Func tpl_stddev = zncc_stddev_tpl(t, rd_template, tpl_size, tpl_avg);

  serialize(x, y) = Tuple(
    cast<float>(t(x, y) - tpl_avg(_)),
    tpl_stddev(_)
  );

  serialize.compute_root()
    .tile(x, y, xo, yo, xi, yi, 16, 16)
    .fuse(xo, yo, ti)
    .parallel(ti)
    .vectorize(xi, 16);
  return serialize;
}

Func prepared_match_template_zncc_fn(
  Func input, Param<int32_t> width, Param<int32_t> height,
  Func buf_tpl_val, Func buf_tpl_stddev,
  Param<int32_t> tpl_width, Param<int32_t> tpl_height
) {
  Region src_bounds = {{0, width},{0, height},{0, 4}};
  Func in = gray_xy_uint8(BoundaryConditions::constant_exterior(input, 0, src_bounds), "in");

  Var x("x"), y("y"), ch("ch");
  Var xo("xo"), xi("xi");
  Var yo("yo"), yi("yi");
  Var ti("ti");

  RDom rd_template = RDom(0, tpl_width, 0, tpl_height, "rd_template");

  Func match = Func("prepared_match_template_zncc");
  Expr tpl_size = cast<float>(tpl_width * tpl_height);
  Func src_avg = zncc_avg(in, rd_template, tpl_size);
  Func src_stddev = zncc_stddev(in, rd_template, tpl_size, src_avg);

  Expr src_val = cast<float>(in(x + rd_template.x, y + rd_template.y)) - src_avg(x, y);
  Expr tpl_val = cast<float>(buf_tpl_val(rd_template.x, rd_template.y));
  Expr s = cast<float>(sum(src_val * tpl_val));
  Expr v = s / sqrt(src_stddev(x, y) * buf_tpl_stddev(0, 0));
  match(x, y) = cast<double>(v);

  match.compute_root()
    .tile(x, y, xo, yo, xi, yi, 32, 32)
    .fuse(xo, yo, ti)
    .parallel(ti)
    .vectorize(xi, 32);
  src_avg.compute_root()
    .parallel(y)
    .vectorize(x, 32);
  src_stddev.compute_root()
    .parallel(y)
    .vectorize(x, 32);
  in.compute_root();
  buf_tpl_val.compute_root();
  buf_tpl_stddev.compute_root();
  return match;
}

Func match_template_zncc_fn(
  Func input, Param<int32_t> width, Param<int32_t> height,
  Func tpl, Param<int32_t> tpl_width, Param<int32_t> tpl_height
) {
  Region src_bounds = {{0, width},{0, height},{0, 4}};
  Region tpl_bounds = {{0, tpl_width},{0, tpl_height},{0, 4}};
  Func in = gray_xy_uint8(BoundaryConditions::constant_exterior(input, 0, src_bounds), "in");
  Func t = gray_xy_uint8(BoundaryConditions::constant_exterior(tpl, 0, tpl_bounds), "tpl");

  Var x("x"), y("y"), ch("ch");
  Var xo("xo"), xi("xi");
  Var yo("yo"), yi("yi");
  Var ti("ti");

  RDom rd_template = RDom(0, tpl_width, 0, tpl_height, "rd_template");

  Func match = Func("match_template_zncc");
  Expr tpl_size = cast<float>(tpl_width * tpl_height);
  Func src_avg = zncc_avg(in, rd_template, tpl_size);
  Func src_stddev = zncc_stddev(in, rd_template, tpl_size, src_avg);
  Func tpl_avg = zncc_avg_tpl(t, rd_template, tpl_size);
  Func tpl_stddev = zncc_stddev_tpl(t, rd_template, tpl_size, tpl_avg);

  Expr src_val = cast<float>(in(x + rd_template.x, y + rd_template.y)) - src_avg(x, y);
  Expr tpl_val = cast<float>(t(rd_template.x, rd_template.y)) - tpl_avg(_);
  Expr s = cast<float>(sum(src_val * tpl_val));
  Expr v = s / sqrt(src_stddev(x, y) * tpl_stddev(_));
  match(x, y) = cast<double>(v);

  match.compute_at(in, ti)
    .tile(x, y, xo, yo, xi, yi, 32, 32)
    .fuse(xo, yo, ti)
    .parallel(ti)
    .vectorize(xi, 32);
  src_avg.compute_root()
    .parallel(y)
    .vectorize(x, 32);
  src_stddev.compute_root()
    .parallel(y)
    .vectorize(x, 32);
  tpl_avg.compute_root();
  tpl_stddev.compute_root();
  in.compute_root()
    .unroll(y, 8)
    .vectorize(x, 16);
  t.compute_root()
    .unroll(y, 8)
    .vectorize(x, 16);
  return match;
}
