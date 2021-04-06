// +build ignore.

#define DEBUG 0

#include "blurry.hpp"

const uint8_t GRAY_R = 76, GRAY_G = 152, GRAY_B = 28;

const Expr CANNY_SIGMA = 5.0f;
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

  RDom rd_morph = RDom(0, size, 0, size, "rd_morph_open");

  Func erode_tmp = Func("erode_tmp");
  erode_tmp(x, y) = erode(in, rd_morph);
  Func dilate_tmp = Func("dilate_tmp");
  dilate_tmp(x, y) = dilate(erode_tmp, rd_morph);

  Func morph = Func("morphology_open");
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

  RDom rd_morph = RDom(0, size, 0, size, "rd_morph_close");

  Func dilate_tmp = Func("dilate_tmp");
  dilate_tmp(x, y) = dilate(in, rd_morph);
  Func erode_tmp = Func("erode_tmp");
  erode_tmp(x, y) = erode(dilate_tmp, rd_morph);

  Func morph = Func("morphology_close");
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

Func morphology_gradient(Func in, Param<int32_t> size) {
  Var x("x"), y("y");

  RDom rd_morph = RDom(0, size, 0, size, "rd_morph_gradient");
  Func morph = Func("morphology_gradient");
  Expr val_dilate = dilate(in, rd_morph);
  Expr val_erode = erode(in, rd_morph);
  morph(x, y) = val_dilate - val_erode;

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
  Func in = readUI8(BoundaryConditions::repeat_edge(input, src_bounds), "in");

  Var x("x"), y("y"), ch("ch");

  //
  // RGBA interleave test
  //
  Func cloneimg = Func("cloneimg");
  cloneimg(x, y, ch) = in(x, y, ch);

  return cloneimg;
}

Func rotate0_fn(Func input, Param<int32_t> width, Param<int32_t> height) {
  Region src_bounds = {{0, width},{0, height},{0, 4}};
  Func in = readUI8(BoundaryConditions::repeat_edge(input, src_bounds), "in");

  Var x("x"), y("y"), ch("ch");

  // same cloneimg
  Func rotate = Func("rotate0");
  rotate(x, y, ch) = in(x, y, ch);

  return rotate;
}

Func rotate90_fn(Func input, Param<int32_t> width, Param<int32_t> height) {
  Region src_bounds = {{0, width},{0, height},{0, 4}};
  Func in = readUI8(BoundaryConditions::constant_exterior(input, 0, src_bounds), "in");

  Var x("x"), y("y"), ch("ch");
  Func rotate = Func("rotate90");
  rotate(x, y, ch) = in(y, (height - 1) - x, ch);

  return rotate;
}

Func rotate180_fn(Func input, Param<int32_t> width, Param<int32_t> height) {
  Region src_bounds = {{0, width},{0, height},{0, 4}};
  Func in = readUI8(BoundaryConditions::constant_exterior(input, 0, src_bounds), "in");

  Var x("x"), y("y"), ch("ch");
  Func rotate = Func("rotate180");
  rotate(x, y, ch) = in((width - 1) - x, (height - 1) - y, ch);

  return rotate;
}

Func rotate270_fn(Func input, Param<int32_t> width, Param<int32_t> height) {
  Region src_bounds = {{0, width},{0, height},{0, 4}};
  Func in = readUI8(BoundaryConditions::constant_exterior(input, 0, src_bounds), "in");

  Var x("x"), y("y"), ch("ch");
  Func rotate = Func("rotate270");
  rotate(x, y, ch) = in((width - 1) - y, x, ch);

  return rotate;
}

Func erosion_fn(Func input, Param<int32_t> width, Param<int32_t> height, Param<uint8_t> size) {
  Region src_bounds = {{0, width},{0, height},{0, 4}};
  Func in = readUI8(BoundaryConditions::repeat_edge(input, src_bounds), "in");

  Var x("x"), y("y"), ch("ch");
  Var xo("xo"), xi("xi");
  Var yo("yo"), yi("yi");
  Var ti("ti");

  RDom rd = RDom(0,size, 0,size, "erode");
  Func erosion = Func("erosion");
  Expr value = in(x + rd.x, y + rd.y, ch);
  erosion(x, y, ch) = minimum(value);

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
  Func in = readUI8(BoundaryConditions::repeat_edge(input, src_bounds), "in");

  Var x("x"), y("y"), ch("ch");
  Var xo("xo"), xi("xi");
  Var yo("yo"), yi("yi");
  Var ti("ti");

  RDom rd = RDom(0,size, 0,size, "dilate");
  Func dilation = Func("dilation");
  Expr value = in(x + rd.x, y + rd.y, ch);
  dilation(x, y, ch) = maximum(value);

  dilation.compute_root()
    .tile(x, y, xo, yo, xi, yi, 32, 32)
    .fuse(xo, yo, ti)
    .parallel(ch)
    .parallel(ti)
    .vectorize(xi, 32);

  in.compute_root();
  return dilation;
}

Func morphology_fn(
  Func input, Param<int32_t> width, Param<int32_t> height,
  Param<uint8_t> mode, Param<int32_t> size
) {
  Region src_bounds = {{0, width},{0, height},{0, 4}};
  Func in = read(BoundaryConditions::mirror_image(input, src_bounds), "in");

  Var x("x"), y("y"), ch("ch");

  Func gray = Func("gray");
  gray(x, y) = cast<uint8_t>(in(x, y, 0));

  Func open = morphology_open(gray, size);
  Func close = morphology_close(gray, size);
  Func grad = morphology_gradient(gray, size);

  Func morphology = Func("morphology");
  morphology(x, y, ch) = select(
    ch == 3, 255,
    mode == MORPH_OPEN,     open(x, y),
    mode == MORPH_CLOSE,    close(x, y),
    mode == MORPH_GRADIENT, grad(x, y),
    gray(x, y)
  );

  gray.compute_root();
  return morphology;
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
  Func in = read(BoundaryConditions::repeat_edge(input, src_bounds), "in");

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
  Param<uint8_t> morphology_mode, Param<int32_t> morphology_size,
  Param<int32_t> dilate_size
) {
  Region src_bounds = {{0, width},{0, height},{0, 4}};
  Func in = read(BoundaryConditions::repeat_edge(input, src_bounds), "in");

  Var x("x"), y("y"), ch("ch");
  Var xo("xo"), xi("xi");
  Var yo("yo"), yi("yi");
  Var ti("ti");

  Func gray = Func("gray");
  gray(x, y) = cast<uint8_t>(in(x, y, 0)); // rgba(r) for grayscale

  Func morph = Func("morphology");
  Func morph_open = morphology_open(gray, morphology_size);
  Func morph_close = morphology_close(gray, morphology_size);
  morph(x, y) = select(
    morphology_size < 1, gray(x, y),
    morphology_mode < 1, gray(x, y),
    morphology_mode == MORPH_OPEN, morph_open(x, y),
    morphology_mode == MORPH_CLOSE, morph_close(x, y),
    gray(x, y)
  );

  RDom gauss_rd = RDom(-1, 3, "gaussian_rdom");
  Func gauss = gaussian(morph, CANNY_SIGMA, gauss_rd, "gaussian5x5");

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
  Expr hysteresis = select(
    dilate_size < 1, hy(x, y),
    dilate(hy, RDom(0, dilate_size, 0, dilate_size, "canny_dilate"))
  );

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
    .tile(x, y, xo, yo, xi, yi, 32, 32)
    .fuse(xo, yo, ti)
    .parallel(ti)
    .vectorize(xi, 32);

  canny.compute_root();

  gauss.compute_root();

  morph_open.compute_root();
  morph_close.compute_root();
  morph.compute_root();

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
