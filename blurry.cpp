// +build ignore.

#define DEBUG 0

#include "blurry.hpp"

const uint8_t GRAY_R = 76, GRAY_G = 152, GRAY_B = 28;

const Expr CANNY_SIGMA = 5.0f;
const Expr acos_v = -1.0f;
const Expr pi = acos(acos_v);
const Expr ui8_0 = cast<uint8_t>(0);
const Expr ui8_255 = cast<uint8_t>(255);
const Expr f05 = cast<float>(0.5f);
const Expr float_0 = cast<float>(0.f);
const Expr float_16 = cast<float>(16.f);
const Expr float_128 = cast<float>(128.f);
const Expr float_255 = cast<float>(255.f);
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

  return kernel_x;
}

Func kernel_sobel3x3_y() {
  Var x("x"), y("y");

  Func kernel_y = Func("kernel_sobel_y");
  kernel_y(x, y) = 0;
  kernel_y(-1, -1) = -1; kernel_y(0, -1) = -2; kernel_y(1, -1) = -1;
  kernel_y(-1,  0) =  0; kernel_y(0,  0) =  0; kernel_y(1,  0) =  0;
  kernel_y(-1,  1) =  1; kernel_y(0,  1) =  2; kernel_y(1,  1) =  1;

  return kernel_y;
}

Func kernel_emboss3x3() {
  Var x("x"), y("y");

  Func kernel = Func("kernel_emboss");
  kernel(x, y) = 0;
  kernel(-1, -1) = -1; kernel(0, -1) = -1; kernel(1, -1) = 0;
  kernel(-1,  0) = -1; kernel(0,  0) =  0; kernel(1,  0) = 1;
  kernel(-1,  1) =  0; kernel(0,  1) =  1; kernel(1,  1) = 1;

  return kernel;
}

Func kernel_laplacian3x3() {
  Var x("x"), y("y");

  Func kernel = Func("kernel_laplacian");
  kernel(x, y) = 0;
  kernel(-1, -1) = 0; kernel(0, -1) =  1; kernel(1, -1) = 0;
  kernel(-1,  0) = 1; kernel(0,  0) = -4; kernel(1,  0) = 1;
  kernel(-1,  1) = 0; kernel(0,  1) =  1; kernel(1,  1) = 0;

  return kernel;
}

Func kernel_highpass3x3() {
  Var x("x"), y("y");

  Func kernel = Func("kernel_highpass");
  kernel(x, y) = 0;
  kernel(-1, -1) = -1; kernel(0, -1) = -1; kernel(1, -1) = -1;
  kernel(-1,  0) = -1; kernel(0,  0) =  8; kernel(1,  0) = -1;
  kernel(-1,  1) = -1; kernel(0,  1) = -1; kernel(1,  1) = -1;

  return kernel;
}

Func kernel_gradient3x3() {
  Var x("x"), y("y");

  Func kernel = Func("kernel_gradient");
  kernel(x, y) = 0;
  kernel(-1, -1) =  1; kernel(0, -1) =  1; kernel(1, -1) =  1;
  kernel(-1,  0) =  0; kernel(0,  0) =  0; kernel(1,  0) =  0;
  kernel(-1,  1) = -1; kernel(0,  1) = -1; kernel(1,  1) = -1;

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

Func readFloat(Func clamped, const char *name) {
  Var x("x"), y("y"), ch("ch");
  Func read = Func(name);
  read(x, y, ch) = cast<float>(clamped(x, y, ch));
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

// BT.601 limited range
Func read_from_yuv_bt601_limited(Func yf, Func uf, Func vf, const char *name) {
  Var x("x"), y("y"), ch("ch");

  Func f = Func(name);
  Expr yy = (yf(x, y) - float_16) * 1.164f;
  Expr uu = uf(x, y) - float_128;
  Expr vv = vf(x, y) - float_128;

  Expr r = yy + (1.596f * vv);
  Expr g = yy - (0.391f * uu) - (0.813f * vv);
  Expr b = yy + (2.018f * uu);

  Expr rr = clamp(r, float_0, float_255);
  Expr gg = clamp(g, float_0, float_255);
  Expr bb = clamp(b, float_0, float_255);

  Expr v = select(
    ch == 0, rr,       // R
    ch == 1, gg,       // G
    ch == 2, bb,       // B
    likely(float_255)  // A always 0xff
  );
  f(x, y, ch) = cast<uint8_t>(v);
  return f;
}

// BT.601 full range
Func read_from_yuv_bt601_fullrange(Func yf, Func uf, Func vf, const char *name) {
  Var x("x"), y("y"), ch("ch");

  Func f = Func(name);
  Expr yy = yf(x, y);
  Expr uu = uf(x, y) - float_128;
  Expr vv = vf(x, y) - float_128;

  Expr r = yy + (1.40200f * vv);
  Expr g = yy - (0.34414f * uu) - (0.71414f * vv);
  Expr b = yy + (1.77200f * uu);

  Expr rr = clamp(r, float_0, float_255);
  Expr gg = clamp(g, float_0, float_255);
  Expr bb = clamp(b, float_0, float_255);

  Expr v = select(
    ch == 0, rr,       // R
    ch == 1, gg,       // G
    ch == 2, bb,       // B
    likely(float_255)  // A always 0xff
  );
  f(x, y, ch) = cast<uint8_t>(v);
  return f;
}

// BT.709 limited range
Func read_from_yuv_bt709_limited(Func yf, Func uf, Func vf, const char *name) {
  Var x("x"), y("y"), ch("ch");

  Func f = Func(name);
  Expr yy = (yf(x, y) - float_16) * 1.164f;
  Expr uu = uf(x, y) - float_128;
  Expr vv = vf(x, y) - float_128;

  Expr r = yy + (1.793f * vv);
  Expr g = yy - (0.213f * uu) - (0.533f * vv);
  Expr b = yy + (2.112f * uu);

  Expr rr = clamp(r, float_0, float_255);
  Expr gg = clamp(g, float_0, float_255);
  Expr bb = clamp(b, float_0, float_255);

  Expr v = select(
    ch == 0, rr,       // R
    ch == 1, gg,       // G
    ch == 2, bb,       // B
    likely(float_255)  // A always 0xff
  );
  f(x, y, ch) = cast<uint8_t>(v);
  return f;
}

// BT.2020 full range
Func read_from_yuv_bt2020_fullrange(Func yf, Func uf, Func vf, const char *name) {
  Var x("x"), y("y"), ch("ch");

  Func f = Func(name);
  Expr yy = yf(x, y);
  Expr uu = uf(x, y) - float_128;
  Expr vv = vf(x, y) - float_128;

  Expr r = yy + (1.474600f * vv);
  Expr g = yy - (0.164553f * uu) - (0.571353f * vv);
  Expr b = yy + (1.881400f * uu);

  Expr rr = clamp(r, float_0, float_255);
  Expr gg = clamp(g, float_0, float_255);
  Expr bb = clamp(b, float_0, float_255);

  Expr v = select(
    ch == 0, rr,       // R
    ch == 1, gg,       // G
    ch == 2, bb,       // B
    likely(float_255)  // A always 0xff
  );
  f(x, y, ch) = cast<uint8_t>(v);
  return f;
}

// BT.2020 limited range
Func read_from_yuv_bt2020_limited(Func yf, Func uf, Func vf, const char *name) {
  Var x("x"), y("y"), ch("ch");

  Func f = Func(name);
  Expr yy = (yf(x, y) - float_16) * 1.164384f;
  Expr uu = uf(x, y) - float_128;
  Expr vv = vf(x, y) - float_128;

  Expr r = yy + (1.67867f * vv);
  Expr g = yy - (0.187326f * uu) - (0.65042f * vv);
  Expr b = yy + (2.14177f * uu);

  Expr rr = clamp(r, float_0, float_255);
  Expr gg = clamp(g, float_0, float_255);
  Expr bb = clamp(b, float_0, float_255);

  Expr v = select(
    ch == 0, rr,       // R
    ch == 1, gg,       // G
    ch == 2, bb,       // B
    likely(float_255)  // A always 0xff
  );
  f(x, y, ch) = cast<uint8_t>(v);
  return f;
}

Func read_from_yuv_444(Func in_y, Func in_u, Func in_v, const char *name) {
  Var x("x"), y("y");

  Func yf = Func("y_float");
  Func uf = Func("u_float");
  Func vf = Func("v_float");
  yf(x, y) = cast<float>(in_y(x, y));
  uf(x, y) = cast<float>(in_u(x, y));
  vf(x, y) = cast<float>(in_v(x, y));

  return read_from_yuv_bt2020_limited(yf, uf, vf, name);
}

Func read_from_yuv_420(Func in_y, Func in_u, Func in_v, const char *name) {
  Var x("x"), y("y");

  Func yf = Func("y_float");
  Func uf = Func("u_float");
  Func vf = Func("v_float");
  yf(x, y) = cast<float>(in_y(x, y));
  uf(x, y) = cast<float>(in_u(x / 2, y / 2));
  vf(x, y) = cast<float>(in_v(x / 2, y / 2));

  return read_from_yuv_bt2020_limited(yf, uf, vf, name);
}

// BT.601
Func rgb_to_yuv_bt601(Func rf, Func gf, Func bf, const char *name) {
  Var x("x"), y("y"), ch("ch");

  Func f = Func(name);
  Expr yuv_y = ((rf(x, y) * 0.257f) + (gf(x, y) * 0.504f) + (bf(x, y) * 0.098f)) + float_16;
  Expr yuv_u = ((rf(x, y) * -0.148f) - (gf(x, y) * 0.291f) + (bf(x, y) * 0.439f)) + float_128;
  Expr yuv_v = ((rf(x, y) * 0.439f) - (gf(x, y) * 0.368f) - (bf(x, y) * 0.071f)) + float_128;

  Expr yy = clamp(yuv_y, float_0, float_255);
  Expr uu = clamp(yuv_u, float_0, float_255);
  Expr vv = clamp(yuv_v, float_0, float_255);

  Expr v = select(
    ch == 0, yy,      // Y
    ch == 1, uu,      // U
    ch == 2, vv,      // V
    likely(float_255) // A always 0xff
  );

  f(x, y, ch) = cast<uint8_t>(v);
  return f;
}

// BT.2020 full range
Func rgb_to_yuv_bt2020_fullrange(Func rf, Func gf, Func bf, const char *name) {
  Var x("x"), y("y"), ch("ch");

  Func f = Func(name);
  Expr yuv_y = ((rf(x, y) * 0.26270f) + (gf(x, y) * 0.67800f) + (bf(x, y) * 0.05930f)) + float_16;
  Expr yuv_u = ((rf(x, y) * -0.13963f) - (gf(x, y) * 0.36037f) + (bf(x, y) * 0.50000f)) + float_128;
  Expr yuv_v = ((rf(x, y) * 0.50000f) - (gf(x, y) * 0.45979f) - (bf(x, y) * 0.04021f)) + float_128;

  Expr yy = clamp(yuv_y, float_0, float_255);
  Expr uu = clamp(yuv_u, float_0, float_255);
  Expr vv = clamp(yuv_v, float_0, float_255);

  Expr v = select(
    ch == 0, yy,      // Y
    ch == 1, uu,      // U
    ch == 2, vv,      // V
    likely(float_255) // A always 0xff
  );

  f(x, y, ch) = cast<uint8_t>(v);
  return f;
}

// BT.2020 limited range
Func rgb_to_yuv_bt2020_limited(Func rf, Func gf, Func bf, const char *name) {
  Var x("x"), y("y"), ch("ch");

  Func f = Func(name);
  Expr yuv_y = ((rf(x, y) * 0.22564f) + (gf(x, y) * 0.59558f) + (bf(x, y) * 0.05209f)) + float_16;
  Expr yuv_u = ((rf(x, y) * -0.11992f) - (gf(x, y) * 0.31656f) + (bf(x, y) * 0.43922f)) + float_128;
  Expr yuv_v = ((rf(x, y) * 0.42941f) - (gf(x, y) * 0.40389f) - (bf(x, y) * 0.03533f)) + float_128;

  Expr yy = clamp(yuv_y, float_0, float_255);
  Expr uu = clamp(yuv_u, float_0, float_255);
  Expr vv = clamp(yuv_v, float_0, float_255);

  Expr v = select(
    ch == 0, yy,      // Y
    ch == 1, uu,      // U
    ch == 2, vv,      // V
    likely(float_255) // A always 0xff
  );

  f(x, y, ch) = cast<uint8_t>(v);
  return f;
}

Func rgb_to_yuv444(Func in, const char *name) {
  Var x("x"), y("y");

  Func rf = Func("r_float");
  Func gf = Func("g_float");
  Func bf = Func("b_float");
  rf(x, y) = cast<float>(in(x, y, 0));
  gf(x, y) = cast<float>(in(x, y, 1));
  bf(x, y) = cast<float>(in(x, y, 2));

  return rgb_to_yuv_bt2020_limited(rf, gf, bf, name);
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
  Var x("x"), y("y"), ch("ch");
  Func read = Func(name);
  read(x, y, ch) = clamped(y, (height - 1) - x, ch);
  return read;
}

Func rotate180(Func clamped, Param<int32_t> width, Param<int32_t> height, const char *name) {
  Var x("x"), y("y"), ch("ch");
  Func read = Func(name);
  read(x, y, ch) = clamped((width - 1) - x, (height - 1) - y, ch);
  return read;
}

Func rotate270(Func clamped, Param<int32_t> width, Param<int32_t> height, const char *name) {
  Var x("x"), y("y"), ch("ch");
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
  Var x("x"), y("y"), ch("ch");
  Var xo("xo"), xi("xi");
  Var yo("yo"), yi("yi");
  Var ti("ti");

  Func in = gray_xy_uint8(BoundaryConditions::repeat_edge(input, bounds), "in");

  Func conv = convolve_xy(in, kernel, rd_kernel);

  Func gradient = Func(name);
  gradient(x, y, ch) = select(
    ch < 3, cast<uint8_t>(conv(x, y) & 128),
    likely(ui8_255)
  );

  conv.compute_at(gradient, yi)
    .vectorize(x);

  gradient.compute_at(in, ti)
    .tile(x, y, xo, yo, xi, yi, 32, 32)
    .fuse(xo, yo, ti)
    .parallel(ch)
    .parallel(ti, 8)
    .vectorize(xi, 32);

  return gradient;
}

Func gauss_kernel(Expr sigma) {
  Var x("x"), y("y"), ch("ch");

  Expr sig2 = 2 * sigma * sigma;
  Expr sigR = sigma * sqrt(2 * pi);

  Func kernel = Func("kernel");
  kernel(x) = fast_exp(-((x * x)) / sig2 / sigR);

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
    .parallel(y, 8)
    .vectorize(x, 32);
  dilate_tmp.compute_root()
    .parallel(y, 8)
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
    .parallel(y, 8)
    .vectorize(x, 32);

  erode_tmp.compute_root()
    .parallel(y, 8)
    .vectorize(x, 32);

  return morph;
}

Func gaussian(Func in, Expr sigma, RDom rd, const char *name) {
  Var x("x"), y("y");

  Func kernel = gauss_kernel(sigma);

  Func sum_kernel = Func("sum_kernel");
  Expr kernel_val = kernel(rd);
  sum_kernel(x) += kernel_val;

  Func gaussian = Func(name);
  Expr center_val = sum_kernel(0);
  Expr in_val = in(x + rd.x, y);
  Expr val = in_val * kernel(rd);
  gaussian(x, y) += cast<uint8_t>(val / center_val);

  sum_kernel.compute_at(gaussian, y)
    .vectorize(x);
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

  gauss.compute_at(hy, ti)
    .vectorize(y)
    .vectorize(x);
  gy.compute_at(hy, ti)
    .vectorize(x);
  gx.compute_at(hy, ti)
    .vectorize(x);

  nms.compute_at(hy, ti)
    .vectorize(x);

  sobel.compute_at(hy, ti)
    .unroll(y, 8)
    .vectorize(x);

  hy.compute_root()
    .tile(x, y, xo, yo, xi, yi, 32, 32)
    .fuse(xo, yo, ti)
    .parallel(ti)
    .vectorize(xi, 32);

  return hy;
}

Func cloneimg_fn(Func input, Param<int32_t> width, Param<int32_t> height) {
  Var x("x"), y("y"), ch("ch");
  Region src_bounds = {{0, width},{0, height},{0, 4}};
  Func in = readUI8(BoundaryConditions::repeat_edge(input, src_bounds), "in");

  //
  // RGBA interleave test
  //
  Func cloneimg = Func("cloneimg");
  cloneimg(x, y, ch) = in(x, y, ch);

  cloneimg.compute_at(in, x)
    .parallel(ch)
    .vectorize(x, 16);
  return cloneimg;
}

Func convert_from(Func in, Param<int32_t> width, Param<int32_t> height, const char *name) {
  Var x("x"), y("y"), ch("ch");
  Var xo("xo"), xi("xi");
  Var yo("yo"), yi("yi");
  Var ti("ti");

  Func convert = Func(name);
  convert(x, y, ch) = in(x, y, ch);

  convert.compute_at(in, xi)
    .tile(x, y, xo, yo, xi, yi, 32, 32)
    .fuse(xo, yo, ti)
    .parallel(ch)
    .parallel(ti, 16)
    .vectorize(xi, 32);

  in.compute_at(convert, xi)
    .parallel(ch)
    .vectorize(x, 32);
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

Func convert_from_yuv_444_fn(Func in_y, Func in_u, Func in_v, Param<int32_t> width, Param<int32_t> height) {
  return convert_from(
    read_from_yuv_444(in_y, in_u, in_v, "in"),
    width,
    height,
    "convert_from_yuv_444"
  );
}

Func convert_from_yuv_420_fn(Func in_y, Func in_u, Func in_v, Param<int32_t> width, Param<int32_t> height) {
  return convert_from(
    read_from_yuv_420(in_y, in_u, in_v, "in"),
    width,
    height,
    "convert_from_yuv_420"
  );
}

Func convert_to_yuv_444_fn(Func input, Param<int32_t> width, Param<int32_t> height) {
  Var x("x"), y("y");
  Var xo("xo"), xi("xi");
  Var yo("yo"), yi("yi");
  Var ti("ti");

  Region src_bounds = {{0, width},{0, height},{0, 3}};
  Func in = readUI8(BoundaryConditions::constant_exterior(input, 0, src_bounds), "in");

  Expr y_max_w = width;
  Expr y_max_h = height;
  Expr uv_max_h = y_max_h + y_max_h;

  Func yuv = rgb_to_yuv444(in, "rgb_to_yuv444");

  Func f = Func("convert_to_yuv_444");
  f(x, y) = select(
    y < y_max_h,                  yuv(x, y, 0),
    y_max_h <= y && y < uv_max_h, yuv(x, (y - y_max_h), 1),
    uv_max_h <= y,                yuv(x, (y - uv_max_h), 2),
    ui8_0
  );

  f.compute_at(yuv, ti)
    .tile(x, y, xo, yo, xi, yi, 32, 32)
    .fuse(xo, yo, ti)
    .parallel(ti)
    .vectorize(xi, 32);

  return f;
}

Func convert_to_yuv_420_fn(Func input, Param<int32_t> width, Param<int32_t> height) {
  Var x("x"), y("y"), ch("ch");
  Region src_bounds = {{0, width},{0, height},{0, 3}};
  Func in = readUI8(BoundaryConditions::constant_exterior(input, 0, src_bounds), "in");

  Expr y_max_w = width;
  Expr y_max_h = height;
  Expr y_height = height;
  Expr uv_size = (width * height) / 4;
  Expr uv_stride = width / 2;
  Expr uv_height = uv_size / width;

  Expr u_max_h = y_height + uv_height;
  Expr v_max_h = u_max_h + uv_height;

  Func yuv = rgb_to_yuv444(in, "rgb_to_yuv444");

  Func yuv444to420 = Func("yuv444to420");
  Expr kx = 2 * (x % uv_stride);
  Expr ky = (y * uv_stride) + (2 * (x / uv_stride)) + y;
  yuv444to420(x,y,ch) = (
    yuv(kx, ky, ch) +
    yuv(kx + 1, ky, ch) +
    yuv(kx, ky + 1, ch) +
    yuv(kx + 1, ky + 1, ch)
  ) / 4;

  Func f = Func("convert_to_yuv_420");
  f(x, y) = select(
    y < y_max_h,                 yuv(x, y, 0),
    y_max_h <= y && y < u_max_h, yuv444to420(x, y - y_max_h, 1),
    u_max_h <= y && y < v_max_h, yuv444to420(x, y - u_max_h, 2),
    ui8_0
  );
  return f;
}

Func rotate0_fn(Func input, Param<int32_t> width, Param<int32_t> height) {
  Region src_bounds = {{0, width},{0, height},{0, 4}};
  Func in = readUI8(BoundaryConditions::repeat_edge(input, src_bounds), "in");

  Var x("x"), y("y"), ch("ch");

  // same cloneimg
  Func rotate = Func("rotate0");
  rotate(x, y, ch) = in(x, y, ch);

  rotate.compute_at(in, x)
    .parallel(ch)
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
    .parallel(ch)
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
    .parallel(ch)
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
    .parallel(ch)
    .vectorize(y, 16);
  return rotate;
}

Func crop_fn(
  Func input, Param<int32_t> width, Param<int32_t> height,
  Param<int32_t> px, Param<int32_t> py, Param<int32_t> crop_width, Param<int32_t> crop_height
) {
  Var x("x"), y("y"), ch("ch");
  Var xo("xo"), xi("xi");
  Var yo("yo"), yi("yi");
  Var ti("ti");

  Region src_bounds = {{0, width},{0, height},{0, 4}};
  Func in = readUI8(BoundaryConditions::constant_exterior(input, 0, src_bounds), "in");

  Expr max_x = px + crop_width;
  Expr max_y = py + crop_height;

  Func crop = Func("crop");
  Expr value = select(
    ch < 3 && ((px + x) <= max_x && (py + y) <= max_y), in(px + x, y + py, ch),
    likely(ui8_255)
  );
  crop(x, y, ch) = cast<uint8_t>(value);

  crop.compute_root()
    .tile(x, y, xo, yo, xi, yi, 16, 16)
    .fuse(xo, yo, ti)
    .parallel(ch)
    .parallel(ti, 8)
    .vectorize(xi, 16);
  
  in.compute_at(crop, xi)
    .unroll(y, 8)
    .vectorize(x, 16);
  return crop;
}

Func scale_kernel_box() {
  Var x("x");
  Func f = Func("scale_kernel_box");
  f(x) = select(abs(x) < 0.5f, 1.f, 0.f);

  f.compute_root();
  return f;
}

Func scale_kernel_linar() {
  Var x("x");
  Func f = Func("scale_kernel_gaussian");
  Expr xx = abs(x);
  f(x) = select(xx < 1.f, 1.f - xx, 0.f);

  f.compute_root();
  return f;
}

Func scale_kernel_gaussian() {
  Var x("x");
  Func f = Func("scale_kernel_gaussian");
  Expr xx = abs(x);
  Expr xx2 = fast_pow(0.5f, fast_pow(xx, 2.f));
  Expr base = fast_pow(0.5f, fast_pow(2, 2.f));
  f(x) = select(xx < 1.f, (xx2 - base) / (1 - base), 0.f);

  f.compute_root();
  return f;
}

Func scale_fn(
  Func input,
  Param<int32_t> width, Param<int32_t> height,
  Param<int32_t> scale_width, Param<int32_t> scale_height
) {
  Var x("x"), y("y"), ch("ch");
  Var xo("xo"), xi("xi");
  Var yo("yo"), yi("yi");
  Var ti("ti");

  Region src_bounds = {{0, width},{0, height},{0, 4}};
  Func in = readFloat(BoundaryConditions::constant_exterior(input, 0, src_bounds), "in");

  Expr dx = cast<float>(width) / cast<float>(scale_width);
  Expr dy = cast<float>(height) / cast<float>(scale_height);

  Func scale = Func("scale_normal");
  scale(x, y, ch) = in(cast<int>((x + f05) * dx), cast<int>((y + f05) * dy), ch);

  Func f = Func("scale");
  Expr value = select(
    ch < 3, scale(x, y, ch),
    likely(float_255)
  );
  f(x, y, ch) = cast<uint8_t>(value);

  f.compute_at(scale, ti)
    .tile(x, y, xo, yo, xi, yi, 32, 32)
    .fuse(xo, yo, ti)
    .parallel(ch)
    .parallel(ti, 8)
    .vectorize(xi, 32);
  return f;
}

Func scale_by_kernel(
  Func input,
  Param<int32_t> width, Param<int32_t> height,
  Param<int32_t> scale_width, Param<int32_t> scale_height,
  Func kernel, Expr size,
  const char* name
) {
  Var x("x"), y("y"), ch("ch");
  Var s("s");
  Var xo("xo"), xi("xi");
  Var yo("yo"), yi("yi");
  Var ti("ti");

  Region src_bounds = {{0, width},{0, height},{0, 4}};
  Func in = readFloat(BoundaryConditions::constant_exterior(input, 0, src_bounds), "in");

  Expr delta_w = cast<float>(width) / cast<float>(scale_width);
  Expr delta_h = cast<float>(height) / cast<float>(scale_height);
  Expr rate_w = max(1.f, delta_w);
  Expr rate_h = max(1.f, delta_h);
  Expr kernel_radius_w = rate_w * 1.0f;
  Expr kernel_radius_h = rate_h * 1.0f;
  RDom rd_scale = RDom(0, size, "rd_scale_box");

  Expr src_x = ((x + f05) * delta_w) - f05;
  Expr src_y = ((y + f05) * delta_h) - f05;
  Expr begin_x = cast<int>(ceil(src_x - kernel_radius_w));
  Expr begin_y = cast<int>(ceil(src_y - kernel_radius_h));
  begin_x = clamp(begin_x, 0, (width + 1) - size);
  begin_y = clamp(begin_y, 0, (height + 1) - size);

  Func kernel_val_x = Func("kernel_val_x"), kernel_val_y = Func("kernel_val_y");
  kernel_val_x(x, s) = kernel(cast<int>((s + begin_x - src_x) * rate_w));
  kernel_val_y(y, s) = kernel(cast<int>((s + begin_y - src_y) * rate_h));

  Func kernel_sum_x = Func("kernel_sum_x"), kernel_sum_y = Func("kernel_sum_y");
  kernel_sum_x(x) = sum(kernel_val_x(x, rd_scale));
  kernel_sum_y(y) = sum(kernel_val_y(y, rd_scale));

  Func kernel_x = Func("kernel_x"), kernel_y = Func("kernel_y");
  kernel_x(x, s) = kernel_val_x(x, s) / kernel_sum_x(x);
  kernel_y(y, s) = kernel_val_y(y, s) / kernel_sum_y(y);

  Func scale_y = Func("scale_y");
  scale_y(x, y, ch) = sum(kernel_y(y, rd_scale) * in(x, rd_scale + begin_y, ch));

  Func scale_x = Func("scale_x");
  scale_x(x, y, ch) = sum(kernel_x(x, rd_scale) * scale_y(begin_x + rd_scale, y, ch));

  Func f = Func(name);
  Expr value = select(
    ch < 3, scale_x(x, y, ch),
    likely(float_255)
  );
  f(x, y, ch) = cast<uint8_t>(value);

  kernel_val_x.compute_at(kernel_x, x)
    .vectorize(x);
  kernel_sum_x.compute_at(kernel_x, x)
    .vectorize(x);
  kernel_x.compute_root()
    .reorder(s, x)
    .vectorize(x, 32);

  kernel_val_y.compute_at(kernel_y, y)
    .vectorize(y, 32);
  kernel_sum_y.compute_at(kernel_y, y)
    .vectorize(y);
  kernel_y.compute_at(f, yi)
    .reorder(s, y)
    .vectorize(y, 32);

  f.compute_at(in, ti)
    .tile(x, y, xo, yo, xi, yi, 32, 32)
    .fuse(xo, yo, ti)
    .parallel(ch)
    .parallel(ti, 8)
    .vectorize(xi, 32);

  in.compute_root()
    .parallel(ch)
    .vectorize(x, 16);
  return f;
}

Func scale_box_fn(
  Func input,
  Param<int32_t> width, Param<int32_t> height,
  Param<int32_t> scale_width, Param<int32_t> scale_height
) {
  return scale_by_kernel(
    input,
    width, height,
    scale_width, scale_height,
    scale_kernel_box(), 1,
    "scale_box"
  );
}

Func scale_linear_fn(
  Func input,
  Param<int32_t> width, Param<int32_t> height,
  Param<int32_t> scale_width, Param<int32_t> scale_height
) {
  return scale_by_kernel(
    input, width, height,
    scale_width, scale_height,
    scale_kernel_linar(), 1,
    "scale_linear"
  );
}

Func scale_gaussian_fn(
  Func input,
  Param<int32_t> width, Param<int32_t> height,
  Param<int32_t> scale_width, Param<int32_t> scale_height
) {
  return scale_by_kernel(
    input, width, height,
    scale_width, scale_height,
    scale_kernel_gaussian(), 1,
    "scale_gaussian"
  );
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
    ch == 3, likely(ui8_255),
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
  Var x("x"), y("y"), ch("ch");
  Var xo("xo"), xi("xi");
  Var yo("yo"), yi("yi");
  Var ti("ti");

  Region src_bounds = {{0, width},{0, height},{0, 4}};
  Func in = readUI8(BoundaryConditions::repeat_edge(input, src_bounds), "in");

  RDom rd = RDom(-1 * size, (size * 2) + 1, -1 * size, (size * 2) + 1, "rd_erode");
  Func erosion = Func("erosion");
  Expr value = in(x + rd.x, y + rd.y, ch);
  erosion(x, y, ch) = minimum(value);

  erosion.compute_at(in, yi)
    .tile(x, y, xo, yo, xi, yi, 32, 32)
    .fuse(xo, yo, ti)
    .parallel(ch)
    .parallel(ti, 4)
    .vectorize(xi, 32);

  in.compute_root()
    .parallel(ch)
    .vectorize(x, 16);
  return erosion;
}

Func dilation_fn(Func input, Param<int32_t> width, Param<int32_t> height, Param<int32_t> size) {
  Var x("x"), y("y"), ch("ch");
  Var xo("xo"), xi("xi");
  Var yo("yo"), yi("yi");
  Var ti("ti");

  Region src_bounds = {{0, width},{0, height},{0, 4}};
  Func in = readUI8(BoundaryConditions::repeat_edge(input, src_bounds), "in");

  RDom rd = RDom(-1 * size, (size * 2) + 1, -1 * size, (size * 2) + 1, "rd_dilate");
  Func dilation = Func("dilation");
  Expr value = in(x + rd.x, y + rd.y, ch);
  dilation(x, y, ch) = maximum(value);

  dilation.compute_at(in, yi)
    .tile(x, y, xo, yo, xi, yi, 32, 32)
    .fuse(xo, yo, ti)
    .parallel(ch)
    .parallel(ti, 4)
    .vectorize(xi, 32);

  in.compute_root()
    .parallel(ch)
    .vectorize(x, 16);
  return dilation;
}

Func morphology_open_fn(
  Func input, Param<int32_t> width, Param<int32_t> height,
  Param<int32_t> size
) {
  Var x("x"), y("y"), ch("ch");
  Var xo("xo"), xi("xi");
  Var yo("yo"), yi("yi");
  Var ti("ti");

  Region src_bounds = {{0, width},{0, height},{0, 4}};
  Func in = gray_xy_uint8(BoundaryConditions::mirror_image(input, src_bounds), "in");

  Func f = morphology_open(in, size);
  Func morph = Func("morphology_open");
  morph(x, y, ch) = select(
    ch < 3, f(x, y),
    likely(ui8_255)
  );

  morph.compute_at(in, ti)
    .tile(x, y, xo, yo, xi, yi, 32, 32)
    .fuse(xo, yo, ti)
    .parallel(ch)
    .parallel(ti, 8)
    .vectorize(xi, 32);

  in.compute_root()
    .parallel(y)
    .vectorize(x, 16);
  return morph;
}

Func morphology_close_fn(
  Func input, Param<int32_t> width, Param<int32_t> height,
  Param<int32_t> size
) {
  Var x("x"), y("y"), ch("ch");
  Var xo("xo"), xi("xi");
  Var yo("yo"), yi("yi");
  Var ti("ti");

  Region src_bounds = {{0, width},{0, height},{0, 4}};
  Func in = gray_xy_uint8(BoundaryConditions::mirror_image(input, src_bounds), "in");

  Func f = morphology_close(in, size);
  Func morph = Func("morphology_close");
  morph(x, y, ch) = select(
    ch < 3, f(x, y),
    likely(ui8_255)
  );

  morph.compute_at(in, ti)
    .tile(x, y, xo, yo, xi, yi, 32, 32)
    .fuse(xo, yo, ti)
    .parallel(ch)
    .parallel(ti, 8)
    .vectorize(xi, 32);

  in.compute_root()
    .parallel(y)
    .vectorize(x, 16);
  return morph;
}

Func morphology_gradient_fn(
  Func input, Param<int32_t> width, Param<int32_t> height,
  Param<int32_t> size
) {
  Var x("x"), y("y"), ch("ch");
  Var xo("xo"), xi("xi");
  Var yo("yo"), yi("yi");
  Var ti("ti");

  Region src_bounds = {{0, width},{0, height},{0, 4}};
  Func in = gray_xy_uint8(BoundaryConditions::mirror_image(input, src_bounds), "in");

  RDom rd_morph = RDom(0, size, 0, size, "rd_morph_gradient");
  Func morph = Func("morphology_gradient");
  Expr val_dilate = dilate(in, rd_morph);
  Expr val_erode = erode(in, rd_morph);
  morph(x, y, ch) = val_dilate - val_erode;

  morph.compute_at(in, ti)
    .tile(x, y, xo, yo, xi, yi, 32, 32)
    .fuse(xo, yo, ti)
    .parallel(ch)
    .parallel(ti, 8)
    .vectorize(xi, 32);

  in.compute_root()
    .parallel(y)
    .vectorize(x, 16);
  return morph;
}

Func grayscale_fn(Func input, Param<int32_t> width, Param<int32_t> height) {
  Var x("x"), y("y"), ch("ch");
  Var xo("xo"), xi("xi");
  Var yo("yo"), yi("yi");
  Var ti("ti");

  Region src_bounds = {{0, width},{0, height},{0, 4}};
  Func in = read(BoundaryConditions::repeat_edge(input, src_bounds), "in");

  Func grayscale = Func("grayscale");
  Expr r = in(x, y, 0);
  Expr g = in(x, y, 1);
  Expr b = in(x, y, 2);
  Expr value = ((r * GRAY_R) + (g * GRAY_G) + (b * GRAY_B)) >> 8;

  grayscale(x, y, ch) = select(
    ch < 3, cast<uint8_t>(value),
    likely(ui8_255)
  );

  grayscale.compute_at(in, ti)
    .tile(x, y, xo, yo, xi, yi, 32, 32)
    .fuse(xo, yo, ti)
    .parallel(ch)
    .parallel(ti, 8)
    .vectorize(xi, 32);

  in.compute_root()
    .parallel(ch)
    .vectorize(x, 16);

  return grayscale;
}

Func invert_fn(Func input, Param<int32_t> width, Param<int32_t> height) {
  Var x("x"), y("y"), ch("ch");
  Var xo("xo"), xi("xi");
  Var yo("yo"), yi("yi");
  Var ti("ti");

  Region src_bounds = {{0, width},{0, height},{0, 4}};
  Func in = readUI8(BoundaryConditions::repeat_edge(input, src_bounds), "in");

  Func invert = Func("invert");
  invert(x, y, ch) = select(
    ch < 3, ui8_255 - in(x, y, ch),
    likely(ui8_255)
  );

  invert.compute_at(in, ti)
    .tile(x, y, xo, yo, xi, yi, 32, 32)
    .fuse(xo, yo, ti)
    .parallel(ch)
    .parallel(ti, 8)
    .vectorize(xi, 32);

  in.compute_root()
    .parallel(ch)
    .vectorize(x, 16);

  return invert;
}

Func brightness_fn(Func input, Param<int32_t> width, Param<int32_t> height, Param<float> factor) {
  Var x("x"), y("y"), ch("ch");
  Var xo("xo"), xi("xi");
  Var yo("yo"), yi("yi");
  Var ti("ti");

  Region src_bounds = {{0, width},{0, height},{0, 4}};
  Func in = read(BoundaryConditions::repeat_edge(input, src_bounds), "in");

  Func brightness = Func("brightness");
  Expr value = in(x, y, ch);
  value = cast<float>(value);
  value = min(255.0f, value * factor);

  brightness(x, y, ch) = cast<uint8_t>(value);

  brightness.compute_at(in, ti)
    .tile(x, y, xo, yo, xi, yi, 32, 32)
    .fuse(xo, yo, ti)
    .parallel(ch)
    .parallel(ti, 8)
    .vectorize(xi, 32);

  in.compute_root()
    .parallel(ch)
    .vectorize(x, 16);

  return brightness;
}

Func gammacorrection_fn(Func input, Param<int32_t> width, Param<int32_t> height, Param<float> factor) {
  Var x("x"), y("y"), ch("ch");
  Var xo("xo"), xi("xi");
  Var yo("yo"), yi("yi");
  Var ti("ti");

  Region src_bounds = {{0, width},{0, height},{0, 4}};
  Func in = read(BoundaryConditions::repeat_edge(input, src_bounds), "in");

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
    .parallel(ch)
    .vectorize(x, 16);

  return gammacorrection;
}

Func contrast_fn(Func input, Param<int32_t> width, Param<int32_t> height, Param<float> factor) {
  Var x("x"), y("y"), ch("ch");

  Region src_bounds = {{0, width},{0, height},{0, 4}};
  Func in = read(BoundaryConditions::repeat_edge(input, src_bounds), "in");

  Expr e = max(min(1.0f, factor), 0.0f);

  Func contrast = Func("contrast");
  Expr value = in(x, y, ch);
  value = cast<float>(value);
  value = (value / 255.0f) - 0.5f;
  value = (value * e) + 0.5f;
  value = value * 255.0f;

  contrast(x, y, ch) = cast<uint8_t>(value);

  contrast.compute_at(in, x)
    .parallel(ch)
    .vectorize(x, 16);
  return contrast;
}

Func boxblur_fn(Func input, Param<int32_t> width, Param<int32_t> height, Param<uint8_t> size) {
  Var x("x"), y("y"), ch("ch");
  Var xo("xo"), xi("xi");
  Var yo("yo"), yi("yi");
  Var ti("ti");

  Region src_bounds = {{0, width},{0, height},{0, 4}};
  Func in = read(BoundaryConditions::repeat_edge(input, src_bounds), "in");

  Expr box_size = max(size, 1);

  RDom rd_x = RDom(0, box_size, "rdom_x");
  Func blur_x = Func("blur_x");
  blur_x(x, y, ch) = fast_integer_divide(sum(in(x + rd_x, y, ch)), box_size);

  RDom rd_y = RDom(0, box_size, "rdom_y");
  Func blur_y = Func("blur_y");
  blur_y(x, y, ch) = fast_integer_divide(sum(blur_x(x, y + rd_y, ch)), box_size);

  Func boxblur = Func("boxblur");
  boxblur(x, y, ch) = cast<uint8_t>(blur_y(x, y, ch));

  blur_x.compute_root()
    .parallel(ch)
    .vectorize(x, 32);
  blur_y.compute_at(boxblur, yi)
    .parallel(ch)
    .vectorize(x, 32);

  boxblur.compute_at(in, ti)
    .tile(x, y, xo, yo, xi, yi, 32, 32)
    .fuse(xo, yo, ti)
    .parallel(ch)
    .parallel(ti, 8)
    .vectorize(xi, 32);

  in.compute_at(blur_x, y)
    .parallel(ch)
    .vectorize(x, 32);
  return boxblur;
}

Func gaussianblur_fn(Func input, Param<int32_t> width, Param<int32_t> height, Param<float> sigma){
  Var x("x"), y("y"), ch("ch");
  Var s("s");
  Var xo("xo"), xi("xi");
  Var yo("yo"), yi("yi");
  Var ti("ti");

  Region src_bounds = {{0, width},{0, height},{0, 4}};
  Func in = read(BoundaryConditions::repeat_edge(input, src_bounds), "in");

  Expr radius = cast<int16_t>(ceil(sigma * 3));
  Expr sig2 = 2 * sigma * sigma;
  Expr sigR = sigma * sqrt(2 * pi);

  Expr size = 2 * (radius + 1);
  Expr center = radius;

  Expr half = fast_integer_divide(radius, 2);

  RDom rd_rad = RDom(half, size, "rd_radius");

  Func kernel = Func("kernel");
  kernel(s) = fast_exp(-(s * s) / sig2 / sigR);

  Func sum_kernel = Func("sum_kernel");
  Expr kernel_val = kernel(rd_rad);
  sum_kernel(s) = sum(kernel_val);

  Func gaussianblur = Func("gaussianblur");
  Expr center_val = sum_kernel(center);
  Expr in_val = in(x + rd_rad.x, y, ch);
  Expr val = sum(in_val * kernel(rd_rad));
  gaussianblur(x, y, ch) = cast<uint8_t>(val / center_val);

  kernel.compute_root()
    .vectorize(s, 8);

  sum_kernel.compute_at(gaussianblur, ti)
    .vectorize(s, 32);

  gaussianblur.compute_at(in, ti)
    .tile(x, y, xo, yo, xi, yi, 32, 32)
    .fuse(xo, yo, ti)
    .parallel(ch)
    .parallel(ti, 8)
    .vectorize(xi, 32);

  in.compute_root()
    .parallel(ch)
    .vectorize(x, 16);

  return gaussianblur;
}

Func edge_fn(Func input, Param<int32_t> width, Param<int32_t> height){
  Var x("x"), y("y"), ch("ch");
  Var xo("xo"), xi("xi");
  Var yo("yo"), yi("yi");
  Var ti("ti");

  Region src_bounds = {{0, width},{0, height},{0, 4}};
  Func in = gray_xy_uint8(BoundaryConditions::repeat_edge(input, src_bounds), "in");

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

  gy.compute_at(edge, yi)
    .vectorize(y);
  gx.compute_at(edge, xi)
    .vectorize(x);

  edge.compute_at(in, ti)
    .tile(x, y, xo, yo, xi, yi, 32, 32)
    .fuse(xo, yo, ti)
    .parallel(ch)
    .parallel(ti, 8)
    .vectorize(xi, 32);

  return edge;
}

Func sobel_fn(Func input, Param<int32_t> width, Param<int32_t> height){
  Var x("x"), y("y"), ch("ch");
  Var xo("xo"), xi("xi");
  Var yo("yo"), yi("yi");
  Var ti("ti");

  Region src_bounds = {{0, width},{0, height},{0, 4}};
  Func in = gray_xy_uint8(BoundaryConditions::repeat_edge(input, src_bounds), "in");

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

  gy.compute_at(sobel, yi)
    .parallel(y)
    .vectorize(x);
  gx.compute_at(sobel, yi)
    .parallel(y)
    .vectorize(x);

  sobel.compute_at(in, ti)
    .tile(x, y, xo, yo, xi, yi, 32, 32)
    .fuse(xo, yo, ti)
    .parallel(ch)
    .parallel(ti, 8)
    .vectorize(xi, 32);
  return sobel;
}

Func canny_fn(
  Func input, Param<int32_t> width, Param<int32_t> height,
  Param<int32_t> threshold_max, Param<int32_t> threshold_min
) {
  Var x("x"), y("y"), ch("ch");

  Region src_bounds = {{0, width},{0, height},{0, 4}};
  Func in = gray_xy_uint8(BoundaryConditions::repeat_edge(input, src_bounds), "in");

  Func hy = canny(in, threshold_max, threshold_min);

  Func f = Func("canny");
  Expr hysteresis = hy(x, y);

  f(x, y, ch) = select(
    ch < 3, cast<uint8_t>(hysteresis),
    likely(ui8_255)
  );

  f.compute_at(hy, y)
    .parallel(ch)
    .vectorize(x, 32);

  in.compute_root()
    .parallel(y)
    .vectorize(x, 16);
  return f;
}

Func canny_dilate_fn(
  Func input, Param<int32_t> width, Param<int32_t> height,
  Param<int32_t> threshold_max, Param<int32_t> threshold_min,
  Param<int32_t> dilate_size
) {
  Var x("x"), y("y"), ch("ch");

  Region src_bounds = {{0, width},{0, height},{0, 4}};
  Func in = gray_xy_uint8(BoundaryConditions::repeat_edge(input, src_bounds), "in");

  Func hy = canny(in, threshold_max, threshold_min);

  Func f = Func("canny_dilate");
  RDom rd_dilate = RDom(0, dilate_size, 0, dilate_size, "rd_canny_dilate");
  Expr hysteresis_dilate = dilate(hy, rd_dilate);

  f(x, y, ch) = select(
    ch < 3, cast<uint8_t>(hysteresis_dilate),
    likely(ui8_255)
  );

  f.compute_at(hy, y)
    .parallel(ch)
    .vectorize(x, 32);
  return f;
}

Func canny_morphology_open_fn(
  Func input, Param<int32_t> width, Param<int32_t> height,
  Param<int32_t> threshold_max, Param<int32_t> threshold_min,
  Param<int32_t> morphology_size,
  Param<int32_t> dilate_size
) {
  Var x("x"), y("y"), ch("ch");

  Region src_bounds = {{0, width},{0, height},{0, 4}};
  Func in = gray_xy_uint8(BoundaryConditions::repeat_edge(input, src_bounds), "in");

  Func morph = morphology_open(in, morphology_size);

  Func hy = canny(morph, threshold_max, threshold_min);

  Func f = Func("canny_morphology_open");
  RDom rd_dilate = RDom(0, dilate_size, 0, dilate_size, "rd_canny_dilate");
  Expr hysteresis_dilate = dilate(hy, rd_dilate);

  f(x, y, ch) = select(
    ch < 3, cast<uint8_t>(hysteresis_dilate),
    likely(ui8_255)
  );

  f.compute_at(hy, y)
    .parallel(ch)
    .vectorize(x, 32);
  return f;
}

Func canny_morphology_close_fn(
  Func input, Param<int32_t> width, Param<int32_t> height,
  Param<int32_t> threshold_max, Param<int32_t> threshold_min,
  Param<int32_t> morphology_size,
  Param<int32_t> dilate_size
) {
  Var x("x"), y("y"), ch("ch");

  Region src_bounds = {{0, width},{0, height},{0, 4}};
  Func in = gray_xy_uint8(BoundaryConditions::repeat_edge(input, src_bounds), "in");

  Func morph = morphology_close(in, morphology_size);

  Func hy = canny(morph, threshold_max, threshold_min);

  Func f = Func("canny_morphology_close");
  RDom rd_dilate = RDom(0, dilate_size, 0, dilate_size, "rd_canny_dilate");
  Expr hysteresis_dilate = dilate(hy, rd_dilate);

  f(x, y, ch) = select(
    ch < 3, cast<uint8_t>(hysteresis_dilate),
    likely(ui8_255)
  );

  f.compute_at(hy, y)
    .parallel(ch)
    .vectorize(x, 32);
  return f;
}

Func emboss_fn(Func input, Param<int32_t> width, Param<int32_t> height){
  Var x("x"), y("y"), ch("ch");
  Var xo("xo"), xi("xi");
  Var yo("yo"), yi("yi");
  Var ti("ti");

  Region src_bounds = {{0, width},{0, height},{0, 4}};
  Func in = read(BoundaryConditions::repeat_edge(input, src_bounds), "in");

  Func conv = convolve(in, kernel_emboss, rd_kernel);

  Func emboss = Func("emboss");
  emboss(x, y, ch) = select(
    ch < 3, clamp(conv(x, y, ch) + 128, 0, 255),
    likely(ui8_255)
  );

  conv.compute_at(emboss, yi)
    .vectorize(x);

  emboss.compute_at(in, ti)
    .tile(x, y, xo, yo, xi, yi, 32, 32)
    .fuse(xo, yo, ti)
    .parallel(ch)
    .parallel(ti, 8)
    .vectorize(xi, 32);

  in.compute_root()
    .parallel(ch)
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
    likely(float_255)
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
