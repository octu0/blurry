// +build ignore.

#define DEBUG 0

#include "blurry.hpp"

const uint8_t GRAY_R = 76, GRAY_G = 152, GRAY_B = 28;

const Expr CANNY_SIGMA = 5.0f;
const Expr acos_v = -1.0f;
const Expr pi = acos(acos_v);
const Expr ui8_0 = cast<uint8_t>(0);
const Expr ui8_1 = cast<uint8_t>(1);
const Expr ui8_128 = cast<uint8_t>(128);
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
const Expr pcm16_max = cast<float>(32768.0f); // 2^15
const Expr sqrt2 = cast<float>(1.41421356f); // sqrt(2)
const Expr ln10 = cast<float>(2.30258509299404568401799145468436420760110148862877297603332790f); // https://oeis.org/A002392

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

Func wrapFunc_x(Buffer<uint8_t> buf, const char* name) {
  Var x("x");
  Func f = Func(name);
  f(x) = buf(x);
  return f;
}

Func wrapFunc_x(Buffer<int16_t> buf, const char* name) {
  Var x("x");
  Func f = Func(name);
  f(x) = buf(x);
  return f;
}

Func wrapFunc_x(Buffer<float> buf, const char* name) {
  Var x("x");
  Func f = Func(name);
  f(x) = buf(x);
  return f;
}

Func wrapFunc_x(Buffer<double> buf, const char* name) {
  Var x("x");
  Func f = Func(name);
  f(x) = buf(x);
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

  f(x, y, ch) = v;

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

Func gray_xy_float(Func in, const char *name) {
  Var x("x"), y("y");

  Func gray = Func(name);
  gray(x, y) = cast<float>(in(x, y, 0)); // rgba(r) for grayscale

  gray.compute_root();
  return gray;
}

Func rotate90(Func in, Expr width, Expr height, const char *name) {
  Var x("x"), y("y"), ch("ch");
  Func read = Func(name);
  read(x, y, ch) = in(y, height - x, ch);
  return read;
}

Func rotate90(Func clamped, Param<int32_t> width, Param<int32_t> height, const char *name) {
  Expr w = width - 1;
  Expr h = height - 1;
  return rotate90(clamped, w, h, name);
}

Func rotate180(Func in, Expr width, Expr height, const char *name) {
  Var x("x"), y("y"), ch("ch");
  Func read = Func(name);
  read(x, y, ch) = in(width - x, height - y, ch);
  return read;
}

Func rotate180(Func clamped, Param<int32_t> width, Param<int32_t> height, const char *name) {
  Expr w = width - 1;
  Expr h = height - 1;
  return rotate180(clamped, w, h, name);
}

Func rotate270(Func in, Expr width, Expr height, const char *name) {
  Var x("x"), y("y"), ch("ch");
  Func read = Func(name);
  read(x, y, ch) = in(width - y, x, ch);
  return read;
}

Func rotate270(Func clamped, Param<int32_t> width, Param<int32_t> height, const char *name) {
  Expr w = width - 1;
  Expr h = height - 1;
  return rotate270(clamped, w, h, name);
}

// Func flip = flipV(in, width, height, "myFlip")
// f(x, y, ch) = flip(x, y, ch);
Func flipV(Func in, Expr width, Expr height, const char *name) {
  Var x("x"), y("y"), ch("ch");
  Func read = Func(name);
  read(x, y, ch) = in(width - x, y, ch);
  return read;
}

// Func foo(Func input, Param<int32_t> w, Param<int32_t> h) {
//   Func in = flipV(input, w, h);
//   ...
// }
Func flipV(Func clamped, Param<int32_t> width, Param<int32_t> height, const char *name) {
  Expr w = width - 1;
  Expr h = height - 1;
  return flipV(clamped, w, h, name);
}

// Func flip = flipH(in, width, height, "myFlip")
// f(x, y, ch) = flip(x, y, ch);
Func flipH(Func in, Expr width, Expr height, const char *name) {
  Var x("x"), y("y"), ch("ch");
  Func read = Func(name);
  read(x, y, ch) = in(x, height - y, ch);
  return read;
}

// Func foo(Func input, Param<int32_t> w, Param<int32_t> h) {
//   Func in = flipH(input, w, h);
//   ...
// }
Func flipH(Func clamped, Param<int32_t> width, Param<int32_t> height, const char *name) {
  Expr w = width - 1;
  Expr h = height - 1;
  return flipH(clamped, w, h, name);
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

Func haar_x(Func in) {
  Var x("x"), y("y"), ch("ch");

  Func high = Func("haar_x_high");
  high(x, y) = (in(2 * x, y) + in((2 * x) + 1, y)) / sqrt2;

  Func low = Func("haar_x_low");
  low(x, y) = (in(2 * x, y) - in((2 * x) + 1, y)) / sqrt2;

  Func haar = Func("haar_x_highlow");
  haar(x, y, ch) = undef<float>();
  haar(x, y, 0) = high(x, y);
  haar(x, y, 1) = low(x, y);
  return haar;
}

Func haar_y(Func in) {
  Var x("x"), y("y"), ch("ch");

  Func high = Func("haar_y_high");
  high(x, y) = (in(x, 2 * y) + in(x, (2 * y) + 1)) / sqrt2;

  Func low = Func("haar_y_low");
  low(x, y) = (in(x, 2 * y) - in(x, (2 * y) + 1)) / sqrt2;

  Func haar = Func("haar_y_highlow");
  haar(x, y, ch) = undef<float>();
  haar(x, y, 0) = high(x, y);
  haar(x, y, 1) = low(x, y);
  return haar;
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
    .vectorize(x)
    .update(0).unscheduled();

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
    .vectorize(x)
    .update(0).unscheduled();
  return gaussian;
}

Func canny(Func in, Expr threshold_max, Expr threshold_min) {
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
    .vectorize(x)
    .update(0).unscheduled();
  gy.compute_at(hy, ti)
    .vectorize(x)
    .update(0).unscheduled();
  gx.compute_at(hy, ti)
    .vectorize(x)
    .update(0).unscheduled();

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

Func canny(Func in, Param<int32_t> threshold_max, Param<int32_t> threshold_min) {
  Expr th_max = threshold_max;
  Expr th_min = threshold_min;
  return canny(in, th_max, th_min);
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

Pipeline convert_to_yuv_420_fn(Func input, Param<int32_t> width, Param<int32_t> height) {
  Var x("x"), y("y"), ch("ch");
  Var xo("xo"), xi("xi");
  Var yo("yo"), yi("yi");
  Var ti("ti");

  Region src_bounds = {{0, width},{0, height},{0, 3}};
  Func in = readUI8(BoundaryConditions::constant_exterior(input, 0, src_bounds), "in");

  Expr y_max_w = width;
  Expr y_max_h = height;
  Expr y_height = height;
  Expr y_width = width;

  Expr uv_width = width / 2;
  Expr uv_height = height / 2;

  Expr u_max_h = y_height + uv_height;
  Expr v_max_h = u_max_h + uv_height;

  Func yuv = rgb_to_yuv444(in, "rgb_to_yuv444");

  Func yuv444to420 = Func("yuv444to420");
  Expr kx = x * 2;
  Expr ky = y * 2;
  yuv444to420(x, y, ch) = cast<float>(
    yuv(kx, ky, ch) +
    yuv(kx + 1, ky, ch) +
    yuv(kx, ky + 1, ch) +
    yuv(kx + 1, ky + 1, ch)
  ) / 4.f;

  Func fn_y = Func("fn_y");
  Func fn_u = Func("fn_u");
  Func fn_v = Func("fn_v");
  fn_y(x, y) = cast<uint8_t>(yuv(x, y, 0));
  fn_u(x, y) = cast<uint8_t>(yuv444to420(x, y, 1));
  fn_v(x, y) = cast<uint8_t>(yuv444to420(x, y, 2));

  in.compute_root();
  fn_y.compute_at(yuv, ti)
    .store_at(yuv, ti)
    .tile(x, y, xo, yo, xi, yi, 32, 32)
    .fuse(xo, yo, ti)
    .parallel(ti)
    .vectorize(xi, 32);
  fn_u.compute_at(yuv444to420, ti)
    .store_at(yuv444to420, ti)
    .tile(x, y, xo, yo, xi, yi, 32, 32)
    .fuse(xo, yo, ti)
    .parallel(ti)
    .vectorize(xi, 32);
  fn_v.compute_at(yuv444to420, ti)
    .store_at(yuv444to420, ti)
    .tile(x, y, xo, yo, xi, yi, 32, 32)
    .fuse(xo, yo, ti)
    .parallel(ti)
    .vectorize(xi, 32);
  return Pipeline({fn_y, fn_u, fn_v});
}

Pipeline convert_to_yuv_444_fn(Func input, Param<int32_t> width, Param<int32_t> height) {
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

  Func fn_y = Func("fn_y");
  Func fn_u = Func("fn_u");
  Func fn_v = Func("fn_v");
  fn_y(x, y) = cast<uint8_t>(yuv(x, y, 0));
  fn_u(x, y) = cast<uint8_t>(yuv(x, y, 1));
  fn_v(x, y) = cast<uint8_t>(yuv(x, y, 2));

  in.compute_root();
  fn_y.compute_at(yuv, ti)
    .store_at(yuv, ti)
    .tile(x, y, xo, yo, xi, yi, 32, 32)
    .fuse(xo, yo, ti)
    .parallel(ti)
    .vectorize(xi, 32);
  fn_u.compute_at(yuv, ti)
    .store_at(yuv, ti)
    .tile(x, y, xo, yo, xi, yi, 32, 32)
    .fuse(xo, yo, ti)
    .parallel(ti)
    .vectorize(xi, 32);
  fn_v.compute_at(yuv, ti)
    .store_at(yuv, ti)
    .tile(x, y, xo, yo, xi, yi, 32, 32)
    .fuse(xo, yo, ti)
    .parallel(ti)
    .vectorize(xi, 32);
  return Pipeline({fn_y, fn_u, fn_v});
}

Func rotate0_fn(Func input, Param<int32_t> width, Param<int32_t> height) {
  Var x("x"), y("y"), ch("ch");

  Region src_bounds = {{0, width},{0, height},{0, 4}};
  Func in = readUI8(BoundaryConditions::repeat_edge(input, src_bounds), "in");

  // same cloneimg
  Func rotate = Func("rotate0");
  rotate(x, y, ch) = in(x, y, ch);

  rotate.compute_at(in, x)
    .parallel(ch)
    .vectorize(x, 16);
  return rotate;
}

Func rotate90_fn(Func input, Param<int32_t> width, Param<int32_t> height) {
  Var x("x"), y("y"), ch("ch");

  Region src_bounds = {{0, width},{0, height},{0, 4}};
  Func in = readUI8(BoundaryConditions::constant_exterior(input, 0, src_bounds), "in");

  Func rotate = rotate90(in, width, height, "rotate90");

  rotate.compute_at(in, y)
    .parallel(ch)
    .vectorize(y, 16);
  return rotate;
}

Func rotate180_fn(Func input, Param<int32_t> width, Param<int32_t> height) {
  Region src_bounds = {{0, width},{0, height},{0, 4}};
  Func in = readUI8(BoundaryConditions::constant_exterior(input, 0, src_bounds), "in");

  Var x("x"), y("y"), ch("ch");
  Func rotate = rotate180(in, width, height, "rotate180");

  rotate.compute_at(in, x)
    .parallel(ch)
    .vectorize(x, 16);
  return rotate;
}

Func rotate270_fn(Func input, Param<int32_t> width, Param<int32_t> height) {
  Var x("x"), y("y"), ch("ch");

  Region src_bounds = {{0, width},{0, height},{0, 4}};
  Func in = readUI8(BoundaryConditions::constant_exterior(input, 0, src_bounds), "in");

  Func rotate = rotate270(in, width, height, "rotate270");

  rotate.compute_at(in, y)
    .parallel(ch)
    .vectorize(y, 16);
  return rotate;
}

Func flipV_fn(Func input, Param<int32_t> width, Param<int32_t> height) {
  Var x("x"), y("y"), ch("ch");

  Region src_bounds = {{0, width},{0, height},{0, 4}};
  Func in = readUI8(BoundaryConditions::repeat_edge(input, src_bounds), "in");

  Func flip = flipV(in, width, height, "flipV");

  flip.compute_at(in, x)
    .parallel(ch)
    .vectorize(x, 16);
  return flip;
}

Func flipH_fn(Func input, Param<int32_t> width, Param<int32_t> height) {
  Region src_bounds = {{0, width},{0, height},{0, 4}};
  Func in = readUI8(BoundaryConditions::repeat_edge(input, src_bounds), "in");

  Var x("x"), y("y"), ch("ch");
  Func flip = flipH(in, width, height, "flipH");

  flip.compute_at(in, x)
    .parallel(ch)
    .vectorize(x, 16);
  return flip;
}

Func crop(Func input, Expr width, Expr height, Expr px, Expr py, Expr crop_width, Expr crop_height) {
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

Func crop_fn(
  Func input, Param<int32_t> width, Param<int32_t> height,
  Param<int32_t> px, Param<int32_t> py, Param<int32_t> crop_width, Param<int32_t> crop_height
) {
  Expr w = width;
  Expr h = height;
  Expr x = px;
  Expr y = py;
  Expr cw = crop_width;
  Expr ch = crop_height;
  return crop(input, w, h, x, y, cw, ch);
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

Func scale(
  Func input,
  Expr width, Expr height,
  Expr scale_width, Expr scale_height
) {
  Var x("x"), y("y"), ch("ch");
  Var xo("xo"), xi("xi");
  Var yo("yo"), yi("yi");
  Var ti("ti");

  Region src_bounds = {{0, width},{0, height},{0, 4}};
  Func in = BoundaryConditions::constant_exterior(readFloat(input, "in"), 0, src_bounds);

  Expr dx = cast<float>(width) / cast<float>(scale_width);
  Expr dy = cast<float>(height) / cast<float>(scale_height);

  Func scale = Func("scale_normal");
  scale(x, y, ch) = in(cast<int>((x + f05) * dx), cast<int>((y + f05) * dy), ch);

  Func f = Func("scale");
  f(x, y, ch) = cast<uint8_t>(select(
    ch < 3, scale(x, y, ch),
    likely(float_255)
  ));
  f(x, y, 0) = cast<uint8_t>(scale(x, y, 0));
  f(x, y, 1) = cast<uint8_t>(scale(x, y, 1));
  f(x, y, 2) = cast<uint8_t>(scale(x, y, 2));
  f(x, y, 3) = ui8_255;

  scale.compute_root()
    .parallel(y, 8)
    .vectorize(x, 32);
  f.compute_at(scale, ti)
    .store_at(scale, ti)
    .tile(x, y, xo, yo, xi, yi, 32, 32)
    .fuse(xo, yo, ti)
    .parallel(ti, 8)
    .vectorize(xi, 32);
  f.update(0).unscheduled();
  f.update(1).unscheduled();
  f.update(2).unscheduled();
  f.update(3).unscheduled();
  return f;
}

Func scale_fn(
  Func input,
  Param<int32_t> width, Param<int32_t> height,
  Param<int32_t> scale_width, Param<int32_t> scale_height
) {
  Expr w = width;
  Expr h = height;
  Expr sw = scale_width;
  Expr sh = scale_height;
  return scale(input, w, h, sw, sh);
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

Func boxblur(Func input, Expr width, Expr height, Expr size) {
  Var x("x"), y("y"), ch("ch");
  Var xo("xo"), xi("xi");
  Var yo("yo"), yi("yi");
  Var ti("ti");

  Region src_bounds = {{0, width},{0, height},{0, 4}};
  Func in = read(BoundaryConditions::repeat_edge(input, src_bounds), "in");

  Expr box_size = cast<uint8_t>(max(size, 1));

  RDom rd_x = RDom(0, box_size, "rdom_x");
  Func blur_x = Func("blur_x");
  blur_x(x, y, ch) = fast_integer_divide(sum(in(x + rd_x, y, ch)), box_size);

  RDom rd_y = RDom(0, box_size, "rdom_y");
  Func blur_y = Func("blur_y");
  blur_y(x, y, ch) = fast_integer_divide(sum(blur_x(x, y + rd_y, ch)), box_size);

  Func f = Func("boxblur");
  f(x, y, ch) = cast<uint8_t>(blur_y(x, y, ch));

  blur_x.compute_root()
    .parallel(ch)
    .vectorize(x, 64);
  blur_y.compute_at(f, yi)
    .store_at(f, yi)
    .parallel(ch)
    .vectorize(x);

  f.compute_at(in, ti)
    .bound(ch, 0, 4).unroll(ch)
    .bound(x, 0, width)
    .bound(y, 0, height)
    .store_root()
    .tile(x, y, xo, yo, xi, yi, 64, 64)
    .fuse(xo, yo, ti)
    .parallel(ti, 8)
    .vectorize(xi, 64);

  in.compute_at(blur_x, y)
    .parallel(ch)
    .vectorize(x, 64);
  return f;
}

Func boxblur_fn(Func input, Param<int32_t> width, Param<int32_t> height, Param<uint8_t> size) {
  Expr w = width;
  Expr h = height;
  Expr s = size;
  return boxblur(input, w, h, s);
}

Func gaussianblur(Func input, Expr width, Expr height, Expr sigma){
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

  RDom rd_rad = RDom(-1 * half, size - half, "rd_radius");

  Func kernel = Func("kernel");
  Expr denominator = sig2 / sigR;
  Expr s2 = s * s;
  kernel(s) = fast_exp((-1 * s2) / denominator);

  Func sum_kernel = Func("sum_kernel");
  Expr kernel_val = kernel(rd_rad);
  sum_kernel(s) = sum(kernel_val);

  Func f = Func("gaussianblur");
  Expr center_val = sum_kernel(center);
  Expr in_val = in(x + rd_rad.x, y, ch);
  Expr val = sum(in_val * kernel(rd_rad));
  f(x, y, ch) = cast<uint8_t>(val / center_val);

  kernel.compute_root()
    .vectorize(s, 8);

  sum_kernel.compute_at(f, ti)
    .vectorize(s, 32);

  f.compute_at(in, xi)
    .tile(x, y, xo, yo, xi, yi, 32, 32)
    .fuse(xo, yo, ti)
    .parallel(ch)
    .parallel(ti, 32)
    .vectorize(xi, 32);

  return f;
}

Func gaussianblur_fn(Func input, Param<int32_t> width, Param<int32_t> height, Param<float> sigma){
  Expr w = width;
  Expr h = height;
  Expr s = sigma;
  return gaussianblur(input, w, h, s);
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

Func haar_x_fn(Func input, Param<int32_t> width, Param<int32_t> height) {
  Var x("x"), y("y"), ch("ch");

  Region src_bounds = {{0, width},{0, height},{0, 4}};
  Func in = readFloat(BoundaryConditions::constant_exterior(input, 0, src_bounds), "in");

  Func in_r = Func("in_r");
  Func in_g = Func("in_g");
  Func in_b = Func("in_b");
  in_r(x, y) = in(x, y, 0);
  in_g(x, y) = in(x, y, 1);
  in_b(x, y) = in(x, y, 2);

  Func wavelet_r = haar_x(in_r);
  Func wavelet_g = haar_x(in_g);
  Func wavelet_b = haar_x(in_b);

  Expr half_x = width / 2;
  Expr half_y = height / 2;
  Expr high_r = wavelet_r(x, y * 2, 0);
  Expr high_g = wavelet_g(x, y * 2, 0);
  Expr high_b = wavelet_b(x, y * 2, 0);
  Expr low_r = wavelet_r(x - half_x, y * 2, 1);
  Expr low_g = wavelet_g(x - half_x, y * 2, 1);
  Expr low_b = wavelet_b(x - half_x, y * 2, 1);

  Func haar = Func("haar_x");
  Expr value = select(
    ch == 0, select(x < half_x, high_r, low_r),
    ch == 1, select(x < half_x, high_g, low_g),
    ch == 2, select(x < half_x, high_b, low_b),
    likely(float_255)
  );
  haar(x, y, ch) = cast<uint8_t>(clamp(value, 0, 255));
  return haar;
}

Func haar_y_fn(Func input, Param<int32_t> width, Param<int32_t> height) {
  Var x("x"), y("y"), ch("ch");

  Region src_bounds = {{0, width},{0, height},{0, 4}};
  Func in = readFloat(BoundaryConditions::constant_exterior(input, 0, src_bounds), "in");

  Func in_r = Func("in_r");
  Func in_g = Func("in_g");
  Func in_b = Func("in_b");
  in_r(x, y) = in(x, y, 0);
  in_g(x, y) = in(x, y, 1);
  in_b(x, y) = in(x, y, 2);

  Func wavelet_r = haar_y(in_r);
  Func wavelet_g = haar_y(in_g);
  Func wavelet_b = haar_y(in_b);

  Expr half_y = height / 2;
  Expr high_r = wavelet_r(x * 2, y, 0);
  Expr high_g = wavelet_g(x * 2, y, 0);
  Expr high_b = wavelet_b(x * 2, y, 0);
  Expr low_r = wavelet_r(x * 2, y - half_y, 1);
  Expr low_g = wavelet_g(x * 2, y - half_y, 1);
  Expr low_b = wavelet_b(x * 2, y - half_y, 1);

  Func haar = Func("haar_y");
  Expr value = select(
    ch == 0, select(y < half_y, high_r, low_r),
    ch == 1, select(y < half_y, high_g, low_g),
    ch == 2, select(y < half_y, high_b, low_b),
    likely(float_255)
  );
  haar(x, y, ch) = cast<uint8_t>(clamp(value, 0, 255));
  return haar;
}

Func haar_x_edge_fn(Func input, Param<int32_t> width, Param<int32_t> height) {
  Var x("x"), y("y"), ch("ch");

  Region src_bounds = {{0, width},{0, height},{0, 4}};
  Func in = readFloat(BoundaryConditions::constant_exterior(input, 0, src_bounds), "in");

  Func in_r = Func("in_r");
  Func in_g = Func("in_g");
  Func in_b = Func("in_b");
  in_r(x, y) = in(x, y, 0);
  in_g(x, y) = in(x, y, 1);
  in_b(x, y) = in(x, y, 2);

  Func wavelet_r = haar_x(in_r);
  Func wavelet_g = haar_x(in_g);
  Func wavelet_b = haar_x(in_b);

  Expr half_x = width / 2;
  Expr low_r = wavelet_r(x, y * 2, 1);
  Expr low_g = wavelet_g(x, y * 2, 1);
  Expr low_b = wavelet_b(x, y * 2, 1);

  Func haarlow = Func("haar_edge");
  Expr value = select(
    ch == 0, select(x < half_x, low_r, float_0),
    ch == 1, select(x < half_x, low_g, float_0),
    ch == 2, select(x < half_x, low_b, float_0),
    likely(float_255)
  );
  haarlow(x, y, ch) = cast<uint8_t>(clamp(value, 0, 255));

  return haarlow;
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
    .vectorize(x)
    .update(0).unscheduled();
  gx.compute_at(sobel, yi)
    .parallel(y)
    .vectorize(x)
    .update(0).unscheduled();

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
    .vectorize(x)
    .update(0).unscheduled();

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
    .store_at(match, ti)
    .unroll(y, 4)
    .vectorize(x, 32);
  t.compute_at(match, ti)
    .store_at(match, ti)
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
    .store_at(match, ti)
    .unroll(y, 4)
    .vectorize(x, 32);
  t.compute_at(match, ti)
    .store_at(match, ti)
    .unroll(y, 4)
    .vectorize(x, 32);
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
    .store_at(match, ti)
    .unroll(y, 4)
    .vectorize(x, 32);
  t.compute_at(match, ti)
    .store_at(match, ti)
    .unroll(y, 4)
    .vectorize(x, 32);
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
    .store_at(in, ti)
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
    .vectorize(x, 32);
  t.compute_root()
    .unroll(y, 8)
    .vectorize(x, 32);
  return match;
}

Func linearsum(Func in, Expr size, Expr xfactor, Expr yfactor) {
  Var x("x"), y("y"), ch("ch");

  RDom rd = RDom(0, size, "rd_linearsum");
  Func f = Func("linearsum");
  f(x, y, ch) += in(x + (rd * xfactor), y + (rd * yfactor), ch);
  return f;
}

Func linearsum_xy(Func in, Expr size, Expr xfactor, Expr yfactor) {
  Var x("x"), y("y");

  RDom rd = RDom(0, size, "rd_linearsum");
  Func f = Func("linearsum_xy");
  f(x, y) += in(x + (rd * xfactor), y + (rd * yfactor));
  return f;
}

Func squaresum(Func in, Expr size) {
  Var x("x"), y("y"), ch("ch");

  Expr half = size / 2;
  RDom rd = RDom(-1 * half, half, -1 * half, half, "rd_squaresum");
  Func f = Func("squaresum");
  f(x, y, ch) += in(x + rd.x, y + rd.y, ch);
  return f;
}

Func avgslant(Func in, Expr size) {
  Var x("x"), y("y"), ch("ch");
  Func tr = linearsum(in, size, 1, 1);   // top right
  Func br = linearsum(in, size, 1, -1);  // bottom right
  Func bl = linearsum(in, size, -1, -1); // bottom left
  Func tl = linearsum(in, size, -1, 1);  // top left

  Func f = Func("avgslant");
  Expr tr_avg = tr(x, y, ch) / size;
  Expr br_avg = br(x, y, ch) / size;
  Expr bl_avg = bl(x, y, ch) / size;
  Expr tl_avg = tl(x, y, ch) / size;
  f(x, y, ch) = (tr_avg + br_avg + bl_avg + tl_avg) / 4;
  return f;
}

Func avgslant_threshold(Func in, Expr size, Expr threshold, Expr min, Expr max) {
  Var x("x"), y("y"), ch("ch");
  Func avg = avgslant(in, size);
  Func f = Func("avgslant_threshold");
  f(x, y, ch) = select(avg(x, y, ch) < threshold, min, max);
  return f;
}

Func maxslant(Func in, Expr size) {
  Var x("x"), y("y"), ch("ch");
  Func tr = linearsum(in, size, 1, 1);   // top right
  Func br = linearsum(in, size, 1, -1);  // bottom right
  Func bl = linearsum(in, size, -1, -1); // bottom left
  Func tl = linearsum(in, size, -1, 1);  // top left

  Func avg = Func("avgslant");
  Expr tr_avg = tr(x, y, ch) / size;
  Expr br_avg = br(x, y, ch) / size;
  Expr bl_avg = bl(x, y, ch) / size;
  Expr tl_avg = tl(x, y, ch) / size;
  avg(x, y, ch) = max(tr_avg, max(br_avg, max(bl_avg, tl_avg)));
  return avg;
}

Func avgstraight(Func in, Expr size) {
  Var x("x"), y("y"), ch("ch");
  Func t = linearsum(in, size, 0, 1);  // top
  Func r = linearsum(in, size, 1, 0);  // right
  Func b = linearsum(in, size, 0, -1); // bottom
  Func l = linearsum(in, size, -1, 0); // left

  Func avg = Func("avgstraight");
  Expr t_avg = t(x, y, ch) / size;
  Expr r_avg = r(x, y, ch) / size;
  Expr b_avg = b(x, y, ch) / size;
  Expr l_avg = l(x, y, ch) / size;
  avg(x, y, ch) = (t_avg + r_avg + b_avg + l_avg) / 4;
  return avg;
}

Func avgstraight_threshold(Func in, Expr size, Expr threshold, Expr min, Expr max) {
  Var x("x"), y("y"), ch("ch");
  Func avg = avgstraight(in, size);
  Func f = Func("avgstraight_threshold");
  f(x, y, ch) = select(avg(x, y, ch) < threshold, min, max);
  return f;
}

Func maxstraight(Func in, Expr size) {
  Var x("x"), y("y"), ch("ch");
  Func t = linearsum(in, size, 0, 1);  // top
  Func r = linearsum(in, size, 1, 0);  // right
  Func b = linearsum(in, size, 0, -1); // bottom
  Func l = linearsum(in, size, -1, 0); // left

  Func avg = Func("avgstraight");
  Expr t_avg = t(x, y, ch) / size;
  Expr r_avg = r(x, y, ch) / size;
  Expr b_avg = b(x, y, ch) / size;
  Expr l_avg = l(x, y, ch) / size;
  avg(x, y, ch) = max(t_avg, max(r_avg, max(b_avg, l_avg)));
  return avg;
}

Func avgsquare(Func in, Expr size) {
  Var x("x"), y("y"), ch("ch");

  Func avg = squaresum(in, size);
  Func f = Func("avgsquare");
  f(x, y, ch) = avg(x, y, ch) / size;
  return f;
}

Func avgfill(Func in, Expr width, Expr height, Expr size) {
  Var x("x"), y("y"), ch("ch");

  Func f = Func("avgfill");
  Func avg = avgslant(in, size);
  Expr curr = avg(x, y, ch);
  Expr next = avg(x + size, y + size, ch);
  f(x, y, ch) = select(
    curr == next, curr,
    next < curr, curr,
    next
  );
  return f;
}

Func contour_slant(Func binary_input) {
  Var x("x"), y("y");

  Func f = Func("contour_slant");
  Expr tr = binary_input(x + 1, y + 1); // top right
  Expr br = binary_input(x + 1, y - 1); // bottom right
  Expr bl = binary_input(x - 1, y - 1); // bottom left
  Expr tl = binary_input(x - 1, y + 1); // top left

  // https://en.wikipedia.org/wiki/Marching_squares
  f(x, y) = cast<uint8_t>(select(
    tr == 0 && br == 0 && bl == 0 && tl == 0, 100, // case 0
    tr == 0 && br == 0 && bl == 1 && tl == 0, 101, // case 1
    tr == 0 && br == 1 && bl == 0 && tl == 0, 102, // case 2
    tr == 0 && br == 1 && bl == 1 && tl == 0, 103, // case 3
    tr == 1 && br == 0 && bl == 0 && tl == 0, 104, // case 4
    tr == 1 && br == 0 && bl == 1 && tl == 0, 105, // case 5
    tr == 1 && br == 1 && bl == 0 && tl == 0, 106, // case 6
    tr == 1 && br == 1 && bl == 1 && tl == 0, 107, // case 7
    tr == 0 && br == 0 && bl == 0 && tl == 1, 108, // case 8
    tr == 0 && br == 0 && bl == 1 && tl == 1, 109, // case 9
    tr == 0 && br == 1 && bl == 0 && tl == 1, 110, // case 10
    tr == 0 && br == 1 && bl == 1 && tl == 1, 111, // case 11
    tr == 1 && br == 0 && bl == 0 && tl == 1, 112, // case 12
    tr == 1 && br == 0 && bl == 1 && tl == 1, 113, // case 13
    tr == 1 && br == 1 && bl == 0 && tl == 1, 114, // case 14
    tr == 1 && br == 1 && bl == 1 && tl == 1, 115, // case 15
    0
  ));

  return f;
}

Func contour_line(Func binary_input, Expr width, Expr height, Expr size) {
  Var x("x"), y("y");
  Var xo("xo"), xi("xi");
  Var yo("yo"), yi("yi");
  Var ti("ti");

  Func slant = contour_slant(binary_input);

  Func nb = Func("contour_neighbor");
  Expr curr = slant(x, y);

  Expr is_top = curr == 112;
  Expr is_bottom = curr == 103;
  Expr is_right = curr == 106;
  Expr is_left = curr == 109;
  Expr is_top_right = curr == 101;
  Expr is_top_left = curr == 102;
  Expr is_bottom_right = curr == 108;
  Expr is_bottom_left = curr == 104;

  nb(x, y) = select(
    is_top_right || is_top_left || is_bottom_right || is_bottom_left ||
    is_top || is_right || is_left || is_bottom ||
    is_top_right || is_top_left ||
    is_bottom_right || is_bottom_left,
    ui8_1,
    ui8_0
  );

  Func next_top = linearsum_xy(nb, size, 0, 1);
  Func next_top_right = linearsum_xy(nb, size, 1, 1);
  Func next_right = linearsum_xy(nb, size, 1, 0);
  Func next_bottom_right = linearsum_xy(nb, size, 1, -1);
  Func next_bottom = linearsum_xy(nb, size, 0, -1);
  Func next_bottom_left = linearsum_xy(nb, size, -1, -1);
  Func next_left = linearsum_xy(nb, size, -1, 0);
  Func next_top_left = linearsum_xy(nb, size, -1, 1);

  Func f = Func("countor_line");

  Expr top = select(
    next_top(x, y) == size, select(
      nb(x, y + size + 1) == ui8_1, ui8_1,      // top
      nb(x + 1, y + size + 1) == ui8_1, ui8_1,  // top right
      nb(x + 1, y + size) == ui8_1, ui8_1,      // right
      nb(x + 1, y + size - 1) == ui8_1, ui8_1,  // bottom right
      nb(x - 1, y + size - 1) == ui8_1, ui8_1,  // bottom left
      nb(x - 1, y + size) == ui8_1, ui8_1,      // left
      nb(x - 1, y + size + 1) == ui8_1, ui8_1,  // top left
      ui8_0
    ),
    ui8_0
  );
  Expr top_right = select(
    next_top_right(x, y) == size, select(
      nb(x + size, y + size + 1) == ui8_1, ui8_1,      // top
      nb(x + size + 1, y + size + 1) == ui8_1, ui8_1,  // top right
      nb(x + size + 1, y + size) == ui8_1, ui8_1,      // right
      nb(x + size + 1, y + size - 1) == ui8_1, ui8_1,  // bottom right
      nb(x + size, y + size - 1) == ui8_1, ui8_1,      // bottom
      nb(x + size - 1, y + size) == ui8_1, ui8_1,      // left
      nb(x + size - 1, y + size + 1) == ui8_1, ui8_1,  // top left
      ui8_0
    ),
    ui8_0
  );
  Expr right = select(
    next_right(x, y) == size, select(
      nb(x + size, y + 1) == ui8_1, ui8_1,      // top
      nb(x + size + 1, y + 1) == ui8_1, ui8_1,  // top right
      nb(x + size + 1, y) == ui8_1, ui8_1,      // right
      nb(x + size + 1, y - 1) == ui8_1, ui8_1,  // bottom right
      nb(x + size, y - 1) == ui8_1, ui8_1,      // bottom
      nb(x + size - 1, y - 1) == ui8_1, ui8_1,  // bottom left
      nb(x + size - 1, y + 1) == ui8_1, ui8_1,  // top left
      ui8_0
    ),
    ui8_0
  );
  Expr bottom_right = select(
    next_bottom_right(x, y) == size, select(
      nb(x + size, y - size + 1) == ui8_1, ui8_1,      // top
      nb(x + size + 1, y - size + 1) == ui8_1, ui8_1,  // top right
      nb(x + size + 1, y - size) == ui8_1, ui8_1,      // right
      nb(x + size + 1, y - size - 1) == ui8_1, ui8_1,  // bottom right
      nb(x + size, y - size - 1) == ui8_1, ui8_1,      // bottom
      nb(x + size - 1, y - size -1) == ui8_1, ui8_1,   // bottom left
      nb(x + size - 1, y - size) == ui8_1, ui8_1,      // left
      ui8_0
    ),
    ui8_0
  );
  Expr bottom = select(
    next_bottom(x, y) == size, select(
      nb(x + 1, y - size + 1) == ui8_1, ui8_1,  // top right
      nb(x + 1, y - size) == ui8_1, ui8_1,      // right
      nb(x + 1, y - size - 1) == ui8_1, ui8_1,  // bottom right
      nb(x, y - size - 1) == ui8_1, ui8_1,      // bottom
      nb(x - 1, y - size - 1) == ui8_1, ui8_1,  // bottom left
      nb(x - 1, y - size) == ui8_1, ui8_1,      // left
      nb(x - 1, y - size + 1) == ui8_1, ui8_1,  // top left
      ui8_0
    ),
    ui8_0
  );
  Expr bottom_left = select(
    next_bottom_left(x, y) == size, select(
      nb(x - size, y - size + 1) == ui8_1, ui8_1,      // top
      nb(x - size + 1, y - size) == ui8_1, ui8_1,      // right
      nb(x - size + 1, y - size - 1) == ui8_1, ui8_1,  // bottom right
      nb(x - size, y - size - 1) == ui8_1, ui8_1,      // bottom
      nb(x - size - 1, y - size - 1) == ui8_1, ui8_1,  // bottom left
      nb(x - size - 1, y - size) == ui8_1, ui8_1,      // left
      nb(x - size - 1, y - size + 1) == ui8_1, ui8_1,  // top left
      ui8_0
    ),
    ui8_0
  );
  Expr left = select(
    next_left(x, y) == size, select(
      nb(x - size, y + 1) == ui8_1, ui8_1,      // top
      nb(x - size + 1, y + 1) == ui8_1, ui8_1,  // top right
      nb(x - size + 1, y - 1) == ui8_1, ui8_1,  // bottom right
      nb(x - size, y - 1) == ui8_1, ui8_1,      // bottom 
      nb(x - size - 1, y - 1) == ui8_1, ui8_1,  // bottom left
      nb(x - size - 1, y) == ui8_1, ui8_1,      // left
      nb(x - size - 1, y + 1) == ui8_1, ui8_1,  // top left
      ui8_0
    ),
    ui8_0
  );
  Expr top_left = select(
    next_top_left(x, y) == size, select(
      nb(x - size, y + size + 1) == ui8_1, ui8_1,      // top
      nb(x - size + 1, y + size + 1) == ui8_1, ui8_1,  // top right
      nb(x - size + 1, y + size) == ui8_1, ui8_1,      // right
      nb(x - size, y + size - 1) == ui8_1, ui8_1,      // bottom 
      nb(x - size - 1, y + size - 1) == ui8_1, ui8_1,  // bottom left
      nb(x - size - 1, y + size) == ui8_1, ui8_1,      // left
      nb(x - size - 1, y + size + 1) == ui8_1, ui8_1,  // top left
      ui8_0
    ),
    ui8_0
  );
  Expr value = select(
    nb(x, y) == ui8_0, ui8_0, select(
      top == ui8_1, ui8_1,
      top_right == ui8_1, ui8_1,
      right == ui8_1, ui8_1,
      bottom_right == ui8_1, ui8_1,
      bottom == ui8_1, ui8_1,
      bottom_left == ui8_1, ui8_1,
      left == ui8_1, ui8_1,
      top_left == ui8_1, ui8_1,
      ui8_0
    )
  );
  f(x, y) = value;

  slant.compute_at(f, ti)
    .vectorize(y, 8)
    .vectorize(x, 8);

  next_top.compute_at(f, ti)
    .vectorize(y)
    .vectorize(x)
    .update(0).unscheduled();
  next_top_right.compute_at(f, ti)
    .vectorize(y)
    .vectorize(x)
    .update(0).unscheduled();
  next_right.compute_at(f, ti)
    .vectorize(y)
    .vectorize(x)
    .update(0).unscheduled();
  next_bottom_right.compute_at(f, ti)
    .vectorize(y)
    .vectorize(x)
    .update(0).unscheduled();
  next_bottom.compute_at(f, ti)
    .vectorize(y)
    .vectorize(x)
    .update(0).unscheduled();
  next_bottom_left.compute_at(f, ti)
    .vectorize(y)
    .vectorize(x)
    .update(0).unscheduled();
  next_left.compute_at(f, ti)
    .vectorize(y)
    .vectorize(x)
    .update(0).unscheduled();
  next_top_left.compute_at(f, ti)
    .vectorize(y)
    .vectorize(x)
    .update(0).unscheduled();

  nb.compute_at(f, ti)
    .vectorize(y, 8)
    .vectorize(x, 8);

  f.compute_root()
    .tile(x, y, xo, yo, xi, yi, 32, 32)
    .fuse(xo, yo, ti)
    .parallel(ti)
    .vectorize(xi, 32);

  return f;
}

Func contour_overlap_fn(
  Func input, Param<int32_t> width, Param<int32_t> height,
  Param<uint8_t> threshold, Param<uint8_t> size
) {
  Var x("x"), y("y"), ch("ch");

  Region src_bounds = {{0, width},{0, height},{0, 4}};
  Func so = sobel_fn(input, width, height);
  so.compute_root();

  Func filter = Func("contour_filter");
  Expr so_value = so(x, y, ch);
  filter(x, y, ch) = select(
    so_value < threshold, ui8_0, ui8_1  // 0 = off 1 = on
  );

  Func binary = Func("contour_sobel_binary");
  binary(x, y) = filter(x, y, 0);

  Expr w = width - 1;
  Expr h = height - 1;
  Func c = contour_line(binary, w, h, size);

  Func f = Func("contour_overlap");
  Expr value = c(x, y);
  f(x, y, ch) = select(
    ch == 3, ui8_255,
    value == ui8_1, ui8_255, // white = #FFFFFF
    cast<uint8_t>(input(x, y, ch))
  );
  return f;
}

Func contour_fn(
  Func input, Param<int32_t> width, Param<int32_t> height,
  Param<uint8_t> threshold, Param<uint8_t> size
) {
  Var x("x"), y("y"), ch("ch");

  Region src_bounds = {{0, width},{0, height},{0, 4}};
  Func so = sobel_fn(input, width, height);
  so.compute_root();

  Func filter = Func("contour_filter");
  Expr so_value = so(x, y, ch);
  filter(x, y, ch) = select(
    so_value < threshold, ui8_0, ui8_1  // 0 = off 1 = on
  );

  Func binary = Func("contour_sobel_binary");
  binary(x, y) = filter(x, y, 0);

  Expr w = width - 1;
  Expr h = height - 1;
  Func c = contour_line(binary, w, h, size);

  Func f = Func("contour");
  Expr value = c(x, y);
  f(x, y) = value;
  return f;
}

Expr log10(Expr v) {
  return fast_log(v) / ln10;
}

Func pcm16_decibel_fn(
  Func input, Param<int32_t> length
) {
  Var x("x");

  Func normalize = Func("pcm16_normalize");
  Expr in = cast<int16_t>(input(x));
  normalize(x) = cast<float>(in) / pcm16_max; // -1.0 .. x .. 1.0

  Func sq = Func("square");
  sq(x) = fast_pow(normalize(x), 2);

  RDom rd = RDom(0, length, "length");
  Func rms = Func("root_mean_square");
  Expr sum_values = sum(sq(rd));
  Expr mean = sum_values / cast<float>(length);
  rms(_) = sqrt(mean);

  Func decibel = Func("pcm16_decibel");
  decibel(_) = 20 * (log10(rms(_)));

  // schedule
  
  normalize.compute_at(sq, x)
    .store_root()
    .vectorize(x, 64);
  sq.compute_root()
    .store_root()
    .vectorize(x, 64);

  return decibel;
}
