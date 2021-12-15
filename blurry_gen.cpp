// +build ignore.

#include "blurry.hpp"

void generate_static_link(std::vector<Target::Feature> features, Func fn, std::vector<Argument> args, std::string name) {
  printf("generate %s\n", name.c_str());

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

void generate_static_link(std::vector<Target::Feature> features, Pipeline pipe, std::vector<Argument> args, std::string name) {
  printf("generate %s\n", name.c_str());

  std::string lib_name = "lib" + name;
  std::string name_linux = lib_name + "_linux";
  std::string name_osx = lib_name + "_osx";
  {
    Target target;
    target.os = Target::OSX;
    target.arch = Target::X86;
    target.bits = 64;
    target.set_features(features);
    pipe.compile_to_static_library(
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
    pipe.compile_to_static_library(
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

void init_input_yuv_420(ImageParam in_y, ImageParam in_u, ImageParam in_v, Param<int32_t> width, Param<int32_t> height) {
  in_u.dim(0).set_extent(width / 2);
  in_u.dim(0).set_stride(1);
  in_u.dim(1).set_extent(height / 2);
  in_u.dim(1).set_stride(width / 2);

  in_v.dim(0).set_extent(width / 2);
  in_v.dim(0).set_stride(1);
  in_v.dim(1).set_extent(height / 2);
  in_v.dim(1).set_stride(width / 2);
}

void init_input_yuv_444(ImageParam in_y, ImageParam in_u, ImageParam in_v, Param<int32_t> width, Param<int32_t> height) {
  in_u.dim(0).set_extent(width);
  in_u.dim(0).set_stride(1);
  in_u.dim(1).set_extent(height);
  in_u.dim(1).set_stride(width);

  in_v.dim(0).set_extent(width);
  in_v.dim(0).set_stride(1);
  in_v.dim(1).set_extent(height);
  in_v.dim(1).set_stride(width);
}

void init_input_array(ImageParam in, Param<int32_t> width, Param<int32_t> height) {
  in.dim(0).set_stride(1);
  in.dim(1).set_stride(width);
}

void init_output_rgba(OutputImageParam out) {
  out.dim(0).set_stride(4);
  out.dim(2).set_stride(1);
  out.dim(2).set_bounds(0, 4);
}

void init_output_yuv_420(OutputImageParam out, Param<int32_t> width, Param<int32_t> height) {
  out.dim(0).set_stride(1);
  out.dim(1).set_stride(width);
}

void init_output_yuv_444(OutputImageParam out, Param<int32_t> width, Param<int32_t> height) {
  out.dim(0).set_stride(1);
  out.dim(1).set_stride(width);
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

//
// {{{ cloneimg
//
void generate_cloneimg(std::vector<Target::Feature> features) {
  ImageParam src(type_of<uint8_t>(), 3, "src");

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
//
// }}} cloneimg
//

//
// {{{ convert_from_argb
//
void generate_convert_from_argb(std::vector<Target::Feature> features) {
  ImageParam src(type_of<uint8_t>(), 3, "src");

  Param<int32_t> width{"width", 1920};
  Param<int32_t> height{"height", 1080};

  init_input_rgba(src);

  Func fn = convert_from_argb_fn(src.in(), width, height);

  init_output_rgba(fn.output_buffer());

  generate_static_link(features, fn, {
    src, width, height,
  }, fn.name());
}
//
// }}} convert_from_argb
//

//
// {{{ convert_from_abgr
//
void generate_convert_from_abgr(std::vector<Target::Feature> features) {
  ImageParam src(type_of<uint8_t>(), 3, "src");

  Param<int32_t> width{"width", 1920};
  Param<int32_t> height{"height", 1080};

  init_input_rgba(src);

  Func fn = convert_from_abgr_fn(src.in(), width, height);

  init_output_rgba(fn.output_buffer());

  generate_static_link(features, fn, {
    src, width, height,
  }, fn.name());
}
//
// }}} convert_from_abgr
//

//
// {{{ convert_from_bgra
//
void generate_convert_from_bgra(std::vector<Target::Feature> features) {
  ImageParam src(type_of<uint8_t>(), 3, "src");

  Param<int32_t> width{"width", 1920};
  Param<int32_t> height{"height", 1080};

  init_input_rgba(src);

  Func fn = convert_from_bgra_fn(src.in(), width, height);

  init_output_rgba(fn.output_buffer());

  generate_static_link(features, fn, {
    src, width, height,
  }, fn.name());
}
//
// }}} convert_from_bgra
//

//
// {{{ convert_from_rabg
//
void generate_convert_from_rabg(std::vector<Target::Feature> features) {
  ImageParam src(type_of<uint8_t>(), 3, "src");

  Param<int32_t> width{"width", 1920};
  Param<int32_t> height{"height", 1080};

  init_input_rgba(src);

  Func fn = convert_from_rabg_fn(src.in(), width, height);

  init_output_rgba(fn.output_buffer());

  generate_static_link(features, fn, {
    src, width, height,
  }, fn.name());
}
//
// }}} convert_from_rabg
//

//
// {{{ convert_from_yuv_420
//
void generate_convert_from_yuv_420(std::vector<Target::Feature> features) {
  ImageParam yuv_y(type_of<uint8_t>(), 2, "yuv_y");
  ImageParam yuv_u(type_of<uint8_t>(), 2, "yuv_u");
  ImageParam yuv_v(type_of<uint8_t>(), 2, "yuv_v");

  Param<int32_t> width{"width", 1920};
  Param<int32_t> height{"height", 1080};

  init_input_yuv_420(yuv_y, yuv_u, yuv_v, width, height);

  Func fn = convert_from_yuv_420_fn(yuv_y.in(), yuv_u.in(), yuv_v.in(), width, height);

  init_output_rgba(fn.output_buffer());

  generate_static_link(features, fn, {
    yuv_y,
    yuv_u,
    yuv_v,
    width, height,
  }, fn.name());
}
//
// }}} convert_from_yuv_420
//

//
// {{{ convert_from_yuv_444
//
void generate_convert_from_yuv_444(std::vector<Target::Feature> features) {
  ImageParam yuv_y(type_of<uint8_t>(), 2, "yuv_y");
  ImageParam yuv_u(type_of<uint8_t>(), 2, "yuv_u");
  ImageParam yuv_v(type_of<uint8_t>(), 2, "yuv_v");

  Param<int32_t> width{"width", 1920};
  Param<int32_t> height{"height", 1080};

  init_input_yuv_444(yuv_y, yuv_u, yuv_v, width, height);

  Func fn = convert_from_yuv_444_fn(yuv_y.in(), yuv_u.in(), yuv_v.in(), width, height);

  init_output_rgba(fn.output_buffer());

  generate_static_link(features, fn, {
    yuv_y,
    yuv_u,
    yuv_v,
    width, height,
  }, fn.name());
}
//
// }}} convert_from_yuv_444
//

//
// {{{ convert_to_yuv_420
//
void generate_convert_to_yuv_420(std::vector<Target::Feature> features) {
  ImageParam src(type_of<uint8_t>(), 3, "src");

  Param<int32_t> width{"width", 1920};
  Param<int32_t> height{"height", 1080};

  init_input_rgba(src);

  Pipeline pipe = convert_to_yuv_420_fn(
    src.in(),
    width, height
  );

  generate_static_link(features, pipe, {
    src,
    width, height
  }, "convert_to_yuv_420");
}
//
// }}} convert_to_yuv_420
//

//
// {{{ convert_to_yuv_444
//
void generate_convert_to_yuv_444(std::vector<Target::Feature> features) {
  ImageParam src(type_of<uint8_t>(), 3, "src");

  Param<int32_t> width{"width", 1920};
  Param<int32_t> height{"height", 1080};

  init_input_rgba(src);

  Pipeline pipe = convert_to_yuv_444_fn(
    src.in(),
    width, height
  );

  generate_static_link(features, pipe, {
    src,
    width, height
  }, "convert_to_yuv_444");
}
//
// }}} convert_to_yuv_444
//

//
// {{{ rotate0
//
void generate_rotate0(std::vector<Target::Feature> features) {
  ImageParam src(type_of<uint8_t>(), 3, "src");

  Param<int32_t> width{"width", 1920};
  Param<int32_t> height{"height", 1080};

  init_input_rgba(src);

  Func fn = rotate0_fn(src.in(), width, height);

  init_output_rgba(fn.output_buffer());

  generate_static_link(features, fn, {
    src, width, height
  }, fn.name());
}
//
// }}} rotate0
//

//
// {{{ rotate180
//
void generate_rotate180(std::vector<Target::Feature> features) {
  ImageParam src(type_of<uint8_t>(), 3, "src");

  Param<int32_t> width{"width", 1920};
  Param<int32_t> height{"height", 1080};

  init_input_rgba(src);

  Func fn = rotate180_fn(src.in(), width, height);

  init_output_rgba(fn.output_buffer());

  generate_static_link(features, fn, { src, width, height }, fn.name());
}
//
// }}} rotate180
//

//
// {{{ rotate90
//
void generate_rotate90(std::vector<Target::Feature> features) {
  ImageParam src(type_of<uint8_t>(), 3, "src");

  Param<int32_t> width{"width", 1920};
  Param<int32_t> height{"height", 1080};

  init_input_rgba(src);

  Func fn = rotate90_fn(src.in(), width, height);

  init_output_rgba(fn.output_buffer());

  generate_static_link(features, fn, { src, width, height }, fn.name());
}
//
// }}} rotate90
//

//
// {{{ rotate270
//
void generate_rotate270(std::vector<Target::Feature> features) {
  ImageParam src(type_of<uint8_t>(), 3, "src");

  Param<int32_t> width{"width", 1920};
  Param<int32_t> height{"height", 1080};

  init_input_rgba(src);

  Func fn = rotate270_fn(src.in(), width, height);

  init_output_rgba(fn.output_buffer());

  generate_static_link(features, fn, { src, width, height }, fn.name());
}
//
// }}} rotate270
//

//
// {{{ flipV
//
void generate_flipV(std::vector<Target::Feature> features) {
  ImageParam src(type_of<uint8_t>(), 3, "src");

  Param<int32_t> width{"width", 1920};
  Param<int32_t> height{"height", 1080};

  init_input_rgba(src);

  Func fn = flipV_fn(src.in(), width, height);

  init_output_rgba(fn.output_buffer());

  generate_static_link(features, fn, { src, width, height }, fn.name());
}
//
// }}} flipV
//

//
// {{{ flipH
//
void generate_flipH(std::vector<Target::Feature> features) {
  ImageParam src(type_of<uint8_t>(), 3, "src");

  Param<int32_t> width{"width", 1920};
  Param<int32_t> height{"height", 1080};

  init_input_rgba(src);

  Func fn = flipH_fn(src.in(), width, height);

  init_output_rgba(fn.output_buffer());

  generate_static_link(features, fn, { src, width, height }, fn.name());
}
//
// }}} flipH
//

//
// {{{ crop
//
void generate_crop(std::vector<Target::Feature> features) {
  ImageParam src(type_of<uint8_t>(), 3, "src");

  Param<int32_t> width{"width", 1920};
  Param<int32_t> height{"height", 1080};
  Param<int32_t> px{"px", 100};
  Param<int32_t> py{"py", 200};
  Param<int32_t> crop_width{"crop_width", 400};
  Param<int32_t> crop_height{"crop_height", 300};

  init_input_rgba(src);

  Func fn = crop_fn(
    src.in(), width, height,
    px, py, crop_width, crop_height
  );

  init_output_rgba(fn.output_buffer());

  generate_static_link(features, fn, {
    src, width, height,
    px, py, crop_width, crop_height
  }, fn.name());
}
//
// }}} crop
//

//
// {{{ scale
//
void generate_scale(std::vector<Target::Feature> features) {
  ImageParam src(type_of<uint8_t>(), 3, "src");

  Param<int32_t> width{"width", 1920};
  Param<int32_t> height{"height", 1080};
  Param<int32_t> scale_width{"scale_width", 400};
  Param<int32_t> scale_height{"scale_height", 300};

  init_input_rgba(src);

  Func fn = scale_fn(
    src.in(), width, height,
    scale_width, scale_height
  );

  init_output_rgba(fn.output_buffer());

  generate_static_link(features, fn, {
    src, width, height,
    scale_width, scale_height
  }, fn.name());
}
//
// }}} scale
//

//
// {{{ scale_box
//
void generate_scale_box(std::vector<Target::Feature> features) {
  ImageParam src(type_of<uint8_t>(), 3, "src");

  Param<int32_t> width{"width", 1920};
  Param<int32_t> height{"height", 1080};
  Param<int32_t> scale_width{"scale_width", 400};
  Param<int32_t> scale_height{"scale_height", 300};

  init_input_rgba(src);

  Func fn = scale_box_fn(
    src.in(), width, height,
    scale_width, scale_height
  );

  init_output_rgba(fn.output_buffer());

  generate_static_link(features, fn, {
    src, width, height,
    scale_width, scale_height
  }, fn.name());
}
//
// }}} scale_box
//

//
// {{{ scale_linear
//
void generate_scale_linear(std::vector<Target::Feature> features) {
  ImageParam src(type_of<uint8_t>(), 3, "src");

  Param<int32_t> width{"width", 1920};
  Param<int32_t> height{"height", 1080};
  Param<int32_t> scale_width{"scale_width", 400};
  Param<int32_t> scale_height{"scale_height", 300};

  init_input_rgba(src);

  Func fn = scale_linear_fn(
    src.in(), width, height,
    scale_width, scale_height
  );

  init_output_rgba(fn.output_buffer());

  generate_static_link(features, fn, {
    src, width, height,
    scale_width, scale_height
  }, fn.name());
}
//
// }}} scale_linear
//

//
// {{{ scale_gaussian
//
void generate_scale_gaussian(std::vector<Target::Feature> features) {
  ImageParam src(type_of<uint8_t>(), 3, "src");

  Param<int32_t> width{"width", 1920};
  Param<int32_t> height{"height", 1080};
  Param<int32_t> scale_width{"scale_width", 400};
  Param<int32_t> scale_height{"scale_height", 300};

  init_input_rgba(src);

  Func fn = scale_gaussian_fn(
    src.in(), width, height,
    scale_width, scale_height
  );

  init_output_rgba(fn.output_buffer());

  generate_static_link(features, fn, {
    src, width, height,
    scale_width, scale_height
  }, fn.name());
}
//
// }}} scale_gaussian
//

//
// {{{ blend_normal
//
void generate_blend_normal(std::vector<Target::Feature> features) {
  ImageParam src0(type_of<uint8_t>(), 3, "src0");
  ImageParam src1(type_of<uint8_t>(), 3, "src1");

  Param<int32_t> width0{"width0", 1920};
  Param<int32_t> height0{"height0", 1080};
  Param<int32_t> width1{"width1", 640};
  Param<int32_t> height1{"height1", 360};
  Param<int32_t> px{"px", 0};
  Param<int32_t> py{"py", 0};

  init_input_rgba(src0);
  init_input_rgba(src1);

  Func fn = blend_normal_fn(
    src0.in(), width0, height0,
    src1.in(), width1, height1,
    px, py
  );

  init_output_rgba(fn.output_buffer());

  generate_static_link(features, fn, {
    src0, width0, height0,
    src1, width1, height1,
    px, py
  }, fn.name());
}
//
// }}} blend_normal
//

//
// {{{ blend_sub
//
void generate_blend_sub(std::vector<Target::Feature> features) {
  ImageParam src0(type_of<uint8_t>(), 3, "src0");
  ImageParam src1(type_of<uint8_t>(), 3, "src1");

  Param<int32_t> width0{"width0", 1920};
  Param<int32_t> height0{"height0", 1080};
  Param<int32_t> width1{"width1", 640};
  Param<int32_t> height1{"height1", 360};
  Param<int32_t> px{"px", 0};
  Param<int32_t> py{"py", 0};

  init_input_rgba(src0);
  init_input_rgba(src1);

  Func fn = blend_sub_fn(
    src0.in(), width0, height0,
    src1.in(), width1, height1,
    px, py
  );

  init_output_rgba(fn.output_buffer());

  generate_static_link(features, fn, {
    src0, width0, height0,
    src1, width1, height1,
    px, py
  }, fn.name());
}
//
// }}} blend_sub
//

//
// {{{ blend_add
//
void generate_blend_add(std::vector<Target::Feature> features) {
  ImageParam src0(type_of<uint8_t>(), 3, "src0");
  ImageParam src1(type_of<uint8_t>(), 3, "src1");

  Param<int32_t> width0{"width0", 1920};
  Param<int32_t> height0{"height0", 1080};
  Param<int32_t> width1{"width1", 640};
  Param<int32_t> height1{"height1", 360};
  Param<int32_t> px{"px", 0};
  Param<int32_t> py{"py", 0};

  init_input_rgba(src0);
  init_input_rgba(src1);

  Func fn = blend_add_fn(
    src0.in(), width0, height0,
    src1.in(), width1, height1,
    px, py
  );

  init_output_rgba(fn.output_buffer());

  generate_static_link(features, fn, {
    src0, width0, height0,
    src1, width1, height1,
    px, py
  }, fn.name());
}
//
// }}} blend_add
//

//
// {{{ blend_diff_
//
void generate_blend_diff(std::vector<Target::Feature> features) {
  ImageParam src0(type_of<uint8_t>(), 3, "src0");
  ImageParam src1(type_of<uint8_t>(), 3, "src1");

  Param<int32_t> width0{"width0", 1920};
  Param<int32_t> height0{"height0", 1080};
  Param<int32_t> width1{"width1", 640};
  Param<int32_t> height1{"height1", 360};
  Param<int32_t> px{"px", 0};
  Param<int32_t> py{"py", 0};

  init_input_rgba(src0);
  init_input_rgba(src1);

  Func fn = blend_diff_fn(
    src0.in(), width0, height0,
    src1.in(), width1, height1,
    px, py
  );

  init_output_rgba(fn.output_buffer());

  generate_static_link(features, fn, {
    src0, width0, height0,
    src1, width1, height1,
    px, py
  }, fn.name());
}
//
// }}} blend_diff
//

//
// {{{ erosion
//
void generate_erosion(std::vector<Target::Feature> features) {
  ImageParam src(type_of<uint8_t>(), 3, "src");

  Param<int32_t> width{"width", 1920};
  Param<int32_t> height{"height", 1080};
  Param<int32_t> size{"size", 8};

  init_input_rgba(src);

  Func fn = erosion_fn(src.in(), width, height, size);

  init_output_rgba(fn.output_buffer());

  generate_static_link(features, fn, {
    src, width, height, size,
  }, fn.name());
}
//
// }}} erosion
//

//
// {{{ dilation
//
void generate_dilation(std::vector<Target::Feature> features) {
  ImageParam src(type_of<uint8_t>(), 3, "src");

  Param<int32_t> width{"width", 1920};
  Param<int32_t> height{"height", 1080};
  Param<int32_t> size{"size", 8};

  init_input_rgba(src);

  Func fn = dilation_fn(src.in(), width, height, size);

  init_output_rgba(fn.output_buffer());

  generate_static_link(features, fn, {
    src, width, height, size,
  }, fn.name());
}
//
// }}} dilation
//

//
// {{{ morphology_open
//
void generate_morphology_open(std::vector<Target::Feature> features) {
  ImageParam src(type_of<uint8_t>(), 3, "src");

  Param<int32_t> width{"width", 1920};
  Param<int32_t> height{"height", 1080};
  Param<int32_t> size{"size", 3};

  init_input_rgba(src);

  Func fn = morphology_open_fn(src.in(), width, height, size);

  init_output_rgba(fn.output_buffer());

  generate_static_link(features, fn, { src, width, height, size }, fn.name());
}
//
// }}} morphology_open
//

//
// {{{ morphology_close
//
void generate_morphology_close(std::vector<Target::Feature> features) {
  ImageParam src(type_of<uint8_t>(), 3, "src");

  Param<int32_t> width{"width", 1920};
  Param<int32_t> height{"height", 1080};
  Param<int32_t> size{"size", 3};

  init_input_rgba(src);

  Func fn = morphology_close_fn(src.in(), width, height, size);

  init_output_rgba(fn.output_buffer());

  generate_static_link(features, fn, { src, width, height, size }, fn.name());
}
//
// }}} morphology_close
//

//
// {{{ morphology_gradient
//
void generate_morphology_gradient(std::vector<Target::Feature> features) {
  ImageParam src(type_of<uint8_t>(), 3, "src");

  Param<int32_t> width{"width", 1920};
  Param<int32_t> height{"height", 1080};
  Param<int32_t> size{"size", 3};

  init_input_rgba(src);

  Func fn = morphology_gradient_fn(src.in(), width, height, size);

  init_output_rgba(fn.output_buffer());

  generate_static_link(features, fn, { src, width, height, size }, fn.name());
}
//
// }}} morphology_gradient
//

//
// {{{ grayscale
//
void generate_grayscale(std::vector<Target::Feature> features) {
  ImageParam src(type_of<uint8_t>(), 3, "src");

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
//
// }}} grayscale
//

//
// {{{ invert
//
void generate_invert(std::vector<Target::Feature> features) {
  ImageParam src(type_of<uint8_t>(), 3, "src");

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
//
// }}} invert
//

//
// {{{ brightness
//
void generate_brightness(std::vector<Target::Feature> features) {
  ImageParam src(type_of<uint8_t>(), 3, "src");

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
//
// }}} brightness
//

//
// {{{ gammacorrection
//
void generate_gammacorrection(std::vector<Target::Feature> features) {
  ImageParam src(type_of<uint8_t>(), 3, "src");

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
//
// }}} gammacorrection
//

//
// {{{ contrast
//
void generate_contrast(std::vector<Target::Feature> features) {
  ImageParam src(type_of<uint8_t>(), 3, "src");

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
//
// }}} contrast
//

//
// {{{ boxblur
//
void generate_boxblur(std::vector<Target::Feature> features) {
  ImageParam src(type_of<uint8_t>(), 3, "src");

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
//
// }}} boxblur
//

//
// {{{ gaussianblur
//
void generate_gaussianblur(std::vector<Target::Feature> features) {
  ImageParam src(type_of<uint8_t>(), 3, "src");

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
//
// }}} gaussianblur
//

//
// {{{ edge
//
void generate_edge(std::vector<Target::Feature> features) {
  ImageParam src(type_of<uint8_t>(), 3, "src");

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
//
// }}} edge
//

//
// {{{ sobel
//
void generate_sobel(std::vector<Target::Feature> features) {
  ImageParam src(type_of<uint8_t>(), 3, "src");

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
//
// }}} sobel
//

//
// {{{ canny
//
void generate_canny(std::vector<Target::Feature> features) {
  ImageParam src(type_of<uint8_t>(), 3, "src");

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
//
// }}} canny
//

//
// {{{ canny_dilate
//
void generate_canny_dilate(std::vector<Target::Feature> features) {
  ImageParam src(type_of<uint8_t>(), 3, "src");

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
//
// }}} canny_dilate
//

//
// {{{ canny_morphology_open
//
void generate_canny_morphology_open(std::vector<Target::Feature> features) {
  ImageParam src(type_of<uint8_t>(), 3, "src");

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
//
// }}} canny_morphology_open
//

//
// {{{ canny_morphology_close
//
void generate_canny_morphology_close(std::vector<Target::Feature> features) {
  ImageParam src(type_of<uint8_t>(), 3, "src");

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
//
// }}} canny_morphology_close
//

//
// {{{ emboss
//
void generate_emboss(std::vector<Target::Feature> features) {
  ImageParam src(type_of<uint8_t>(), 3, "src");

  Param<int32_t> width{"width", 1920};
  Param<int32_t> height{"height", 1080};

  init_input_rgba(src);

  Func fn = emboss_fn(src.in(), width, height);

  init_output_rgba(fn.output_buffer());

  generate_static_link(features, fn, {
    src, width, height,
  }, fn.name());
}
//
// }}} emboss
//

//
// {{{ laplacian
//
void generate_laplacian(std::vector<Target::Feature> features) {
  ImageParam src(type_of<uint8_t>(), 3, "src");

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
//
// }}} laplacian
//

//
// {{{ highpass
//
void generate_highpass(std::vector<Target::Feature> features) {
  ImageParam src(type_of<uint8_t>(), 3, "src");

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
//
// }}} highpass
//

//
// {{{ gradient
//
void generate_gradient(std::vector<Target::Feature> features) {
  ImageParam src(type_of<uint8_t>(), 3, "src");

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
//
// }}} gradient
//

//
// {{{ blockmozaic
//
void generate_blockmozaic(std::vector<Target::Feature> features) {
  ImageParam src(type_of<uint8_t>(), 3, "src");

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
//
// }}} blockmozaic
//

//
// {{{ match_template_sad
//
void generate_match_template_sad(std::vector<Target::Feature> features) {
  ImageParam src(type_of<uint8_t>(), 3, "src");
  ImageParam tpl(type_of<uint8_t>(), 3, "tpl");

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
//
// }}} match_template_sad
//

//
// {{{ match_template_ssd
//
void generate_match_template_ssd(std::vector<Target::Feature> features) {
  ImageParam src(type_of<uint8_t>(), 3, "src");
  ImageParam tpl(type_of<uint8_t>(), 3, "tpl");

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
//
// }}} match_template_ssd
//

//
// {{{ match_template_ncc
//
void generate_match_template_ncc(std::vector<Target::Feature> features) {
  ImageParam src(type_of<uint8_t>(), 3, "src");
  ImageParam tpl(type_of<uint8_t>(), 3, "tpl");

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
//
// }}} match_template_ncc
//

//
// {{{ prepare_ncc_template
void generate_prepare_ncc_template(std::vector<Target::Feature> features) {
  ImageParam tpl(type_of<uint8_t>(), 3, "tpl");
  Param<int32_t> tpl_width{"tpl_width", 60};
  Param<int32_t> tpl_height{"tpl_height", 60};

  init_input_rgba(tpl);

  Func fn = prepare_ncc_template_fn(
    tpl.in(), tpl_width, tpl_height
  );

  generate_static_link(features, fn, {
    tpl, tpl_width, tpl_height
  }, fn.name());
}
//
// }}} prepare_ncc_template
//

//
// {{{ prepared_match_template_ncc
//
void generate_prepared_match_template_ncc(std::vector<Target::Feature> features) {
  ImageParam src(type_of<uint8_t>(), 3, "src");
  ImageParam buf_tpl_val(type_of<float>(), 2, "tpl_val");
  ImageParam buf_tpl_sum(type_of<float>(), 2, "tpl_sum");

  Param<int32_t> width{"width", 1920};
  Param<int32_t> height{"height", 1080};
  Param<int32_t> tpl_width{"tpl_width", 60};
  Param<int32_t> tpl_height{"tpl_height", 60};

  init_input_rgba(src);
  init_input_array(buf_tpl_val, tpl_width, tpl_height);
  init_input_array(buf_tpl_sum, tpl_width, tpl_height);

  Func fn = prepared_match_template_ncc_fn(
    src.in(), width, height,
    buf_tpl_val.in(), buf_tpl_sum.in(),
    tpl_width, tpl_height
  );

  init_output_array(fn.output_buffer(), width, height);

  generate_static_link(features, fn, {
    src, width, height,
    buf_tpl_val, buf_tpl_sum,
    tpl_width, tpl_height
  }, fn.name());
}
//
// }}} match_prepared_template_ncc
//

//
// {{{ match_template_zncc
//
void generate_match_template_zncc(std::vector<Target::Feature> features) {
  ImageParam src(type_of<uint8_t>(), 3, "src");
  ImageParam tpl(type_of<uint8_t>(), 3, "tpl");

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
//
// }}} match_template_zncc
//

//
// {{{ prepare_zncc_template
//
void generate_prepare_zncc_template(std::vector<Target::Feature> features) {
  ImageParam tpl(type_of<uint8_t>(), 3, "tpl");
  Param<int32_t> tpl_width{"tpl_width", 60};
  Param<int32_t> tpl_height{"tpl_height", 60};

  init_input_rgba(tpl);

  Func fn = prepare_zncc_template_fn(
    tpl.in(), tpl_width, tpl_height
  );

  generate_static_link(features, fn, {
    tpl, tpl_width, tpl_height
  }, fn.name());
}
//
// }}} prepare_zncc_template
//

//
// {{{ prepared_match_template_zncc
//
void generate_prepared_match_template_zncc(std::vector<Target::Feature> features) {
  ImageParam src(type_of<uint8_t>(), 3, "src");
  ImageParam buf_tpl_val(type_of<float>(), 2, "tpl_val");
  ImageParam buf_tpl_stddev(type_of<float>(), 2, "tpl_stddev");

  Param<int32_t> width{"width", 1920};
  Param<int32_t> height{"height", 1080};
  Param<int32_t> tpl_width{"tpl_width", 60};
  Param<int32_t> tpl_height{"tpl_height", 60};

  init_input_rgba(src);
  init_input_array(buf_tpl_val, tpl_width, tpl_height);
  init_input_array(buf_tpl_stddev, tpl_width, tpl_height);

  Func fn = prepared_match_template_zncc_fn(
    src.in(), width, height,
    buf_tpl_val.in(), buf_tpl_stddev.in(),
    tpl_width, tpl_height
  );

  init_output_array(fn.output_buffer(), width, height);

  generate_static_link(features, fn, {
    src, width, height,
    buf_tpl_val, buf_tpl_stddev,
    tpl_width, tpl_height
  }, fn.name());
}
//
// }}} match_prepared_template_zncc
//

//
// {{{ contour
//
void generate_contour(std::vector<Target::Feature> features) {
  ImageParam src(type_of<uint8_t>(), 3, "src");

  Param<int32_t> width{"width", 1920};
  Param<int32_t> height{"height", 1080};
  Param<uint8_t> threshold{"threshold", 100};
  Param<uint8_t> size{"size", 4};

  init_input_rgba(src);

  Func fn = contour_fn(src.in(), width, height, threshold, size);

  init_output_array(fn.output_buffer(), width, height);

  generate_static_link(features, fn, { src, width, height, threshold, size }, fn.name());
}
//
// }}} contour
//

int generate(char **argv){
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
  generate_convert_from_argb(features);
  generate_convert_from_abgr(features);
  generate_convert_from_bgra(features);
  generate_convert_from_rabg(features);
  generate_convert_from_yuv_420(features);
  generate_convert_from_yuv_444(features);
  generate_convert_to_yuv_420(features);
  generate_convert_to_yuv_444(features);
  generate_rotate0(features);
  generate_rotate90(features);
  generate_rotate180(features);
  generate_rotate270(features);
  generate_flipV(features);
  generate_flipH(features);
  generate_crop(features);
  generate_scale(features);
  generate_scale_box(features);
  generate_scale_linear(features);
  generate_scale_gaussian(features);
  generate_blend_normal(features);
  generate_blend_sub(features);
  generate_blend_add(features);
  generate_blend_diff(features);
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
  generate_prepare_ncc_template(features);
  generate_prepared_match_template_ncc(features);
  generate_prepare_zncc_template(features);
  generate_prepared_match_template_zncc(features);
  generate_contour(features);

  return 0;
}

int main(int argc, char **argv) {
  return generate(argv);
}
