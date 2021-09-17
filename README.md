# `blurry`

[![MIT License](https://img.shields.io/github/license/octu0/blurry)](https://github.com/octu0/blurry/blob/master/LICENSE)
[![GoDoc](https://godoc.org/github.com/octu0/blurry?status.svg)](https://godoc.org/github.com/octu0/blurry)
[![Go Report Card](https://goreportcard.com/badge/github.com/octu0/blurry)](https://goreportcard.com/report/github.com/octu0/blurry)
[![Releases](https://img.shields.io/github/v/release/octu0/blurry)](https://github.com/octu0/blurry/releases)

**fast**, high peformance image processing libary.

`blurry` provides image processing algorithms with [halide-lang](https://halide-lang.org/) backend.  
implements optimized processor for amd64 CPUs on Linux/macos

# Benchmarks

## Halide JIT benchmarks

This is the result of using halide's [benchamrk](https://github.com/halide/Halide/blob/master/tools/halide_benchmark.h).  
darwin/amd64 Intel(R) Core(TM) i7-8569U CPU @ 2.80GHz

```
src 320x240
BenchmarkJIT/cloneimg                      : 0.01320ms
BenchmarkJIT/convert_from_argb             : 0.04339ms
BenchmarkJIT/convert_from_abgr             : 0.07046ms
BenchmarkJIT/convert_from_bgra             : 0.04780ms
BenchmarkJIT/convert_from_rabg             : 0.07334ms
BenchmarkJIT/convert_from_yuv_420          : 0.07158ms
BenchmarkJIT/convert_from_yuv_444          : 0.05951ms
BenchmarkJIT/convert_to_yuv_444            : 0.15529ms
BenchmarkJIT/rotate0                       : 0.01312ms
BenchmarkJIT/rotate90                      : 0.07968ms
BenchmarkJIT/rotate180                     : 0.01394ms
BenchmarkJIT/rotate270                     : 0.08807ms
BenchmarkJIT/crop                          : 0.05707ms
BenchmarkJIT/scale                         : 0.04763ms
BenchmarkJIT/scale_box                     : 0.10367ms
BenchmarkJIT/scale_linear                  : 0.10360ms
BenchmarkJIT/scale_gaussian                : 0.11636ms
BenchmarkJIT/blend_normal                  : 0.08605ms
BenchmarkJIT/blend_sub                     : 0.07611ms
BenchmarkJIT/blend_add                     : 0.08011ms
BenchmarkJIT/blend_diff                    : 0.11269ms
BenchmarkJIT/grayscale                     : 0.05239ms
BenchmarkJIT/invert                        : 0.04209ms
BenchmarkJIT/brightness                    : 0.05831ms
BenchmarkJIT/gammacorrection               : 0.09816ms
BenchmarkJIT/contrast                      : 0.05776ms
BenchmarkJIT/boxblur                       : 0.10037ms
BenchmarkJIT/gaussianblur                  : 0.23968ms
BenchmarkJIT/blockmozaic                   : 0.26124ms
BenchmarkJIT/erosion                       : 0.13673ms
BenchmarkJIT/dilation                      : 0.13595ms
BenchmarkJIT/morphology_open               : 0.14927ms
BenchmarkJIT/morphology_close              : 0.14888ms
BenchmarkJIT/morphology_gradient           : 0.11828ms
BenchmarkJIT/emboss                        : 0.14875ms
BenchmarkJIT/laplacian$1                   : 0.10491ms
BenchmarkJIT/highpass                      : 0.09658ms
BenchmarkJIT/gradient                      : 0.08674ms
BenchmarkJIT/edgedetect                    : 0.07488ms
BenchmarkJIT/sobel                         : 0.09165ms
BenchmarkJIT/canny                         : 0.44909ms
BenchmarkJIT/canny_dilate                  : 0.46696ms
BenchmarkJIT/canny_morphology_open         : 0.50971ms
BenchmarkJIT/canny_morphology_close        : 0.51879ms
BenchmarkJIT/match_template_sad            : 5.77770ms
BenchmarkJIT/match_template_ssd            : 4.35721ms
BenchmarkJIT/match_template_ncc            : 8.19072ms
BenchmarkJIT/prepared_match_template_ncc   : 6.03240ms
BenchmarkJIT/match_template_zncc           : 11.91219ms
BenchmarkJIT/prepared_match_template_zncc  : 11.20913ms
```

## AOT benchmarks

Calling a library compiled by AOT(ahead-of-time) via cgo.  
In [cgo](https://golang.org/cmd/cgo/), due to the overhead of ffi calls([e.g.](https://about.sourcegraph.com/go/gophercon-2018-adventures-in-cgo-performance/)), 
more complex operations will be optimized for CPU and become faster.  
Also, the execution speed may be reduced by the overhead of multiple calls.

### Blur

`/D` is [DisablePool](https://pkg.go.dev/github.com/octu0/blurry#DisablePool), i.e. the benchmark when BufferPool is off.

![graph](_benchmark/testdata/blur_bench.png)


```
goos: darwin
goarch: amd64
pkg: github.com/octu0/blurry/benchmark
cpu: Intel(R) Core(TM) i7-8569U CPU @ 2.80GHz
BenchmarkBlur
BenchmarkBlur/bild/blur/Box
BenchmarkBlur/bild/blur/Box-8         	     150	   7901781 ns/op	  640405 B/op	      11 allocs/op
BenchmarkBlur/bild/blur/Gaussian
BenchmarkBlur/bild/blur/Gaussian-8    	     331	   3751497 ns/op	 1262480 B/op	      21 allocs/op
BenchmarkBlur/imaging/Blur
BenchmarkBlur/imaging/Blur-8          	     782	   1520597 ns/op	  793694 B/op	      45 allocs/op
BenchmarkBlur/stackblur-go
BenchmarkBlur/stackblur-go-8          	     244	   4963000 ns/op	  925937 B/op	  153609 allocs/op
BenchmarkBlur/libyuv/ARGBBlur
BenchmarkBlur/libyuv/ARGBBlur-8       	    1861	    635287 ns/op	10182724 B/op	       3 allocs/op
BenchmarkBlur/blurry/Boxblur
BenchmarkBlur/blurry/Boxblur-8        	    9319	    134460 ns/op	      88 B/op	       2 allocs/op
BenchmarkBlur/blurry/Gaussianblur
BenchmarkBlur/blurry/Gaussianblur-8   	    2943	    350995 ns/op	     194 B/op	       2 allocs/op
BenchmarkBlur/blurry/Boxblur/D
BenchmarkBlur/blurry/Boxblur/D-8      	    7401	    179539 ns/op	  311361 B/op	       2 allocs/op
BenchmarkBlur/blurry/Gaussianblur/D
BenchmarkBlur/blurry/Gaussianblur/D-8 	    2733	    383585 ns/op	  311361 B/op	       2 allocs/op
```

### Edge

![graph](_benchmark/testdata/edge_bench.png)

```
goos: darwin
goarch: amd64
pkg: github.com/octu0/blurry/benchmark
cpu: Intel(R) Core(TM) i7-8569U CPU @ 2.80GHz
BenchmarkEdge
BenchmarkEdge/bild/EdgeDetection
BenchmarkEdge/bild/EdgeDetection-8         	     618	   2018322 ns/op	  631294 B/op	      10 allocs/op
BenchmarkEdge/blurry/Edge
BenchmarkEdge/blurry/Edge-8                	    7792	    140375 ns/op	  311488 B/op	       3 allocs/op
```

### Rotate

![graph](_benchmark/testdata/rotate_bench.png)

```
goos: darwin
goarch: amd64
pkg: github.com/octu0/blurry/benchmark
cpu: Intel(R) Core(TM) i7-8569U CPU @ 2.80GHz
BenchmarkRotate
BenchmarkRotate/bild/Rotate/90
BenchmarkRotate/bild/Rotate/90-8         	     634	   1941369 ns/op	 1237045 B/op	  115685 allocs/op
BenchmarkRotate/bild/Rotate/180
BenchmarkRotate/bild/Rotate/180-8        	     529	   2426364 ns/op	 1540316 B/op	  153605 allocs/op
BenchmarkRotate/bild/Rotate/270
BenchmarkRotate/bild/Rotate/270-8        	     531	   1927780 ns/op	 1236940 B/op	  115685 allocs/op
BenchmarkRotate/imaging/90
BenchmarkRotate/imaging/90-8             	    7983	    133069 ns/op	  314181 B/op	       6 allocs/op
BenchmarkRotate/imaging/180
BenchmarkRotate/imaging/180-8            	    9532	    124640 ns/op	  313543 B/op	       6 allocs/op
BenchmarkRotate/imaging/270
BenchmarkRotate/imaging/270-8            	    8282	    150670 ns/op	  314163 B/op	       6 allocs/op
BenchmarkRotate/libyuv/ARGBRotate/90
BenchmarkRotate/libyuv/ARGBRotate/90-8   	   14402	     77141 ns/op	  311360 B/op	       2 allocs/op
BenchmarkRotate/libyuv/ARGBRotate/180
BenchmarkRotate/libyuv/ARGBRotate/180-8  	   34760	     33234 ns/op	  311361 B/op	       2 allocs/op
BenchmarkRotate/libyuv/ARGBRotate/270
BenchmarkRotate/libyuv/ARGBRotate/270-8  	   15648	     76499 ns/op	  311361 B/op	       2 allocs/op
BenchmarkRotate/blurry/Rotate/90
BenchmarkRotate/blurry/Rotate/90-8       	    8181	    152166 ns/op	  311515 B/op	       3 allocs/op
BenchmarkRotate/blurry/Rotate/180
BenchmarkRotate/blurry/Rotate/180-8      	    7290	    166957 ns/op	  311515 B/op	       3 allocs/op
BenchmarkRotate/blurry/Rotate/270
BenchmarkRotate/blurry/Rotate/270-8      	    5473	    227141 ns/op	  311515 B/op	       3 allocs/op
```

### Sobel

![graph](_benchmark/testdata/sobel_bench.png)

```
goos: darwin
goarch: amd64
pkg: github.com/octu0/blurry/benchmark
cpu: Intel(R) Core(TM) i7-8569U CPU @ 2.80GHz
BenchmarkSobel
BenchmarkSobel/bild/Sobel
BenchmarkSobel/bild/Sobel-8         	     201	   6093924 ns/op	 2196805 B/op	      32 allocs/op
BenchmarkSobel/libyuv/ARGBSobel
BenchmarkSobel/libyuv/ARGBSobel-8   	   15313	     73065 ns/op	  311361 B/op	       2 allocs/op
BenchmarkSobel/blurry/Sobel
BenchmarkSobel/blurry/Sobel-8       	    5816	    204196 ns/op	  311489 B/op	       3 allocs/op
```

### Other Benchmarks

See [benchmark](https://github.com/octu0/blurry/tree/master/benchmark) for benchmarks of other methods and performance comparison with [libyuv](https://chromium.googlesource.com/libyuv/libyuv/).


# Installation

```shell
$ go get github.com/octu0/blurry
```

## Examples

original image

![original](testdata/src.png)

### Rotate

rotation 0/90/180/270 clockwise

```go
img, err := blurry.Rotate(input, blurry.Rotate90)
```

| `blurry.RotationMode` | Result                             |
| :-------------------: | :--------------------------------: |
| `blurry.Rotate90`     | ![example](testdata/rotate90.png)  |
| `blurry.Rotate180`    | ![example](testdata/rotate180.png) |
| `blurry.Rotate270`    | ![example](testdata/rotate270.png) |

### Crop

crop x,y with crop_width,crop_height

```go
img, err := blurry.Crop(input, image.Pt(175, 40), crop_width, crop_height)
```

| original                      | `x=175,y=40,cw=80,ch=50`      |
| :---------------------------: | :---------------------------: |
| ![original](testdata/src.png) | ![cropped](testdata/crop.png) |

### Scale

a.k.a. Resize resampling

```go
img, err := blurry.Scale(input, scale_width, scale_height, blurry.ScaleFilterNone)
```

| `blurry.ScaleFilter`         | Result                                  |
| :--------------------------: | :-------------------------------------: |
| `blurry.ScaleFilterNone`     | ![example](testdata/scale_none.png)     |
| `blurry.ScaleFilterBox`      | ![example](testdata/scale_box.png)      |
| `blurry.ScaleFilterLinear`   | ![example](testdata/scale_linear.png)   |
| `blurry.ScaleFilterGaussian` | ![example](testdata/scale_gaussian.png) |

### Grayscale

```go
img, err := blurry.Grayscale(input)
```

![example](testdata/grayscale.png)

### Invert

```go
img, err := blurry.Invert(input)
```

![example](testdata/invert.png)

### Brightness

```go
img, err := blurry.Brightness(input, 1.5)
```

![example](testdata/brightness.png)

### Gamma

```go
img, err := blurry.Gamma(input, 2.5)
```

![example](testdata/gamma.png)

### Contrast

```go
img, err := blurry.Contrast(input, 0.525)
```

![example](testdata/contrast.png)

### BoxBlur

```go
img, err := blurry.Boxblur(input, 11)
```

![example](testdata/boxblur.png)

### GaussianBlur

```go
img, err := blurry.Gaussianblur(input, 5.0)
```

![example](testdata/gaussianblur.png)

### BlockMozaic

```go
img, err := blurry.Blockmozaic(input, 10)
```

![example](testdata/blockmozaic.png)

### Erode

```go
img, err := blurry.Erosion(input, 5)
```

![example](testdata/erosion.png)

### Dilate

```go
img, err := blurry.Dilation(input, 8)
```

![example](testdata/dilation.png)

### Morphology

Morphology repeats Erode and Dilate N times.

```go
size := 5
N := 2
img, err := blurry.Morphology(input, MorphOpen, size, N)
```

| `blurry.MorphologyMode`     | Result                                       |
| :-------------------------: | :------------------------------------------: |
| `blurry.MorphologyOpen`     | ![example](testdata/morphology_open.png)     |
| `blurry.MorphologyClose`    | ![example](testdata/morphology_close.png)    |
| `blurry.MorphologyGradient` | ![example](testdata/morphology_gradient.png) |

### Emboss

```go
img, err := blurry.Emboss(input)
```

![example](testdata/emboss.png)

### HighPass

```go
img, err := blurry.Highpass(input)
```

![example](testdata/highpass.png)

### Laplacian

```go
img, err := blurry.Laplacian(input)
```

![example](testdata/laplacian.png)

### Gradient

```go
img, err := blurry.Gradient(input)
```

![example](testdata/gradient.png)

### Edge

a.k.a. Edge Detection

```go
img, err := blurry.Edge(input)
```

![example](testdata/edge.png)

### Sobel

```go
img, err := blurry.Sobel(input)
```

![example](testdata/sobel.png)

### Canny

a.k.a. Canny Edge Detection

```go
img, err := blurry.Canny(input, 250, 100)
```

| `max:250 min:100`                | `max:400 min:10`                             |
| :------------------------------: | :------------------------------------------: |
| ![example](testdata/canny.png)   | ![example2](testdata/canny_max400_min10.png) |

#### Canny with Dilate

```go
img, err := blurry.CannyWithDilate(input, 250, 100, 3)
```

| `max:250 min:100 dilate:3`                           | `max:250 min:150 dilate:4`                            |
| :--------------------------------------------------: | :---------------------------------------------------: |
| ![example](testdata/canny_max250_min100_dilate3.png) | ![example2](testdata/canny_max250_min150_dilate4.png) |

#### Morphology Canny with Dilate

Pre-process morphology before applying Canny process.

```go
mode := blurry.CannyMorphologyClose
morph_size := 5
dilate_size := 3
img, err := blurry.MorphologyCannyWithDilate(input, 250, 100, mode, morph_size, dilate_size);
```

| `blurry.CannyMorphologyMode` | Result                                       |
| :-------------------------:  | :------------------------------------------: |
| `blurry.CannyMorphologyOpen` | ![example](testdata/canny_morphology_open_size5_max250_min100_dilate3.png) |
| `blurry.CannyMorphologyClose` | ![example](testdata/canny_morphology_close_size5_max250_min100_dilate3.png) |

### Template Matching

SAD(Sum of Absolute Difference), SSD(Sum of Squared Difference), NCC(Normalized Cross Correlation) AND ZNCC(Zero means Normalized Cross Correlation) methods are available for template matching.

#### SAD

```go
scores, err := blurry.MatchTemplateSAD(input, template, 1000)
```

| filter                       | input                                            | template                                         | Result                                              |
| :--------------------------: | :----------------------------------------------: | :----------------------------------------------: | :-------------------------------------------------: |
| `none`                       | ![example](testdata/src.png)                     | ![example](testdata/tpl.png)                     | ![example](testdata/mt_sad.png)                     |
| `grayscale`                  | ![example](testdata/grayscale.png)               | ![example](testdata/tpl_gray.png)                | ![example](testdata/mt_sad_gray.png)                |
| `sobel`                      | ![example](testdata/sobel.png)                   | ![example](testdata/tpl_sobel.png)               | ![example](testdata/mt_sad_sobel.png)               |
| `canny dilate:3  morph:open` | ![example](testdata/src_canny_morph_open_d3.png) | ![example](testdata/tpl_canny_morph_open_d3.png) | ![example](testdata/mt_sad_canny_morph_open_d3.png) |

#### SSD

```go
scores, err := blurry.MatchTemplateSSD(input, template, 1000)
```

| filter                       | input                                            | template                                         | Result                                              |
| :--------------------------: | :----------------------------------------------: | :----------------------------------------------: | :-------------------------------------------------: |
| `none`                       | ![example](testdata/src.png)                     | ![example](testdata/tpl.png)                     | ![example](testdata/mt_ssd.png)                     |
| `grayscale`                  | ![example](testdata/grayscale.png)               | ![example](testdata/tpl_gray.png)                | ![example](testdata/mt_ssd_gray.png)                |
| `sobel`                      | ![example](testdata/sobel.png)                   | ![example](testdata/tpl_sobel.png)               | ![example](testdata/mt_ssd_sobel.png)               |
| `canny dilate:3  morph:open` | ![example](testdata/src_canny_morph_open_d3.png) | ![example](testdata/tpl_canny_morph_open_d3.png) | ![example](testdata/mt_ssd_canny_morph_open_d3.png) |

#### NCC

```go
scores, err := blurry.MatchTemplateNCC(input, template, 0.1)
```

| filter                       | input                                            | template                                         | Result                                              |
| :--------------------------: | :----------------------------------------------: | :----------------------------------------------: | :-------------------------------------------------: |
| `none`                       | ![example](testdata/src.png)                     | ![example](testdata/tpl.png)                     | ![example](testdata/mt_ncc.png)                     |
| `grayscale`                  | ![example](testdata/grayscale.png)               | ![example](testdata/tpl_gray.png)                | ![example](testdata/mt_ncc_gray.png)                |
| `sobel`                      | ![example](testdata/sobel.png)                   | ![example](testdata/tpl_sobel.png)               | ![example](testdata/mt_ncc_sobel.png)               |
| `canny dilate:3  morph:open` | ![example](testdata/src_canny_morph_open_d3.png) | ![example](testdata/tpl_canny_morph_open_d3.png) | ![example](testdata/mt_ncc_canny_morph_open_d3.png) |

#### Prepared NCC

Improve processing speed by pre-calculating part of NCC process.

```go
p, err := blurry.PrepareNCCTemplate(template)
if err != nil {
  panic(err)
}
defer blurry.FreePreparedNCCTemplate(p)

for _, img := range images {
  scores, err := blurry.PreparedMatchTemplateNCC(img, p, 0.1)
  if err != nil {
    panic(err)
  }
}
```

#### ZNCC

```go
scores, err := blurry.MatchTemplateZNCC(input, template, 0.1)
```

| filter                       | input                                            | template                                         | Result                                              |
| :--------------------------: | :----------------------------------------------: | :----------------------------------------------: | :-------------------------------------------------: |
| `none`                       | ![example](testdata/src.png)                     | ![example](testdata/tpl.png)                     | ![example](testdata/mt_zncc.png)                     |
| `grayscale`                  | ![example](testdata/grayscale.png)               | ![example](testdata/tpl_gray.png)                | ![example](testdata/mt_zncc_gray.png)                |
| `sobel`                      | ![example](testdata/sobel.png)                   | ![example](testdata/tpl_sobel.png)               | ![example](testdata/mt_zncc_sobel.png)               |
| `canny dilate:3  morph:open` | ![example](testdata/src_canny_morph_open_d3.png) | ![example](testdata/tpl_canny_morph_open_d3.png) | ![example](testdata/mt_zncc_canny_morph_open_d3.png) |

#### Prepared ZNCC

Improve processing speed by pre-calculating part of ZNCC process.

```go
p, err := blurry.PrepareZNCCTemplate(template)
if err != nil {
  panic(err)
}
defer blurry.FreePreparedZNCCTemplate(p)

for _, img := range images {
  scores, err := blurry.PreparedMatchTemplateZNCC(img, p, 0.1)
  if err != nil {
    panic(err)
  }
}
```

### Blend

Blend input1 on input0.

```go
img, err := blurry.Blend(input0, input1, image.Pt(76, 36), blurry.BlendNormal)
```

| `blurry.BlendMode`    | Result                                |
| :-------------------: | :-----------------------------------: |
| `blurry.BlendNormal`  | ![example](testdata/blend_normal.png) |
| `blurry.BlendSub`     | ![example](testdata/blend_sub.png)    |
| `blurry.BlendAdd`     | ![example](testdata/blend_add.png)    |
| `blurry.BlendDiff`    | ![example](testdata/blend_diff.png)   |

### Convert

blurry supports reading ARGB, ABGR, BGRA, YUV420 and YUV444.  
It also supports YUV444 output.

#### Read: RGBA Color Model

```go
img, err := blurry.ConvertFromARGB(input)
```

| ColorModel  | Method                                |
| :---------: | :-----------------------------------: |
| ARGB        | `blurry.ConvertFromARGB(*image.RGBA)` |
| ABGR        | `blurry.ConvertFromABGR(*image.RGBA)` |
| BGRA        | `blurry.ConvertFromBGRA(*image.RGBA)` |
| RABG        | `blurry.ConvertFromRABG(*image.RGBA)` |

#### Read: YUV Chroma Subsampling

```go
img, err := blurry.ConvertFromYUV420(ycbcr)
```

| Subsampling  | Method                                   |
| :----------: | :--------------------------------------: |
| 420          | `blurry.ConvertFromYUV420(*image.YCbCr)` |
| 444          | `blurry.ConvertFromYUV444(*image.YCbCr)` |

or byte slice can also be specified

```go
var y,u,v []byte
var strideY,strideU,strideV int
var width, height int

img, err := blurry.ConvertFromYUV420Plane(y, u, v, strideY, strideU, strideV, width, height)
```

| Subsampling  | Method                                   |
| :----------: | :--------------------------------------: |
| 420          | `blurry.ConvertFromYUV420Plane(y,u,v []byte, int,int,int, w,h int)` |
| 444          | `blurry.ConvertFromYUV444Plane(y,u,v []byte, int,int,int, w,h int)` |

#### Write: YUV Chroma Subsampling

```go
ycbcr, err := blurry.ConvertToYUV444(rgba)
```

| Subsampling  | Method                                |
| :----------: | :-----------------------------------: |
| 444          | `blurry.ConvertToYUV420(*image.RGBA)` |

## CLI usage

Run it via docker.  
Use `docker run -v` to specify where to load the images and where to output them (`/tmp` will be used as a temporary file).

```shell
$ mkdir myimagedir
$ mkdir myimageout
$ cp /from/img/path.png myimagedir/src.png

# grayscale
$ docker run --rm -it \
  -v $PWD/myimagedir:/img \
  -v $PWD/myimageout:/tmp \
  blurry:1.0.0 grayscale -i /img/src.png
```

#### Help

```
NAME:
   blurry

USAGE:
   blurry [global options] command [command options] [arguments...]

VERSION:
   1.18.0

COMMANDS:
     blend
     blockmozaic
     boxblur
     brightness
     canny
     clone
     contrast
     convert
     convert_from_yuv
     convert_to_yuv
     crop
     dilation
     edge
     emboss
     erosion
     gamma
     gaussianblur
     gradient
     grayscale
     highpass
     invert
     laplacian
     morphology
     match_template
     rotate
     scale
     sobel
     help, h           Shows a list of commands or help for one command

GLOBAL OPTIONS:
   --debug, -d    debug mode
   --verbose, -V  verbose. more message
   --help, -h     show help
   --version, -v  print the version
```

# Build

When building, create a docker container with Halide(clang, llvm, etc).
installed as the build environment.

```shell
$ make build-generator
```

Compile `libruntime.a` and all kinds `lib*_osx.a` or `lib*_linu.a` to make static link.

```shell
$ make generate
```

Finally, generate a docker image if necessary.

```
$ make build
```

# Develop

Set up configuration for macos to be able to run image filtering directly through Halide.

## setup Halide on local

```shell
$ make setup-halide-runtime
```

## generate and run

`genrun` package allows you to export images to temporary file and run image filtering directly.

```shell
$ go run cmd/genrun/main.go benchmark
```

# License

MIT, see LICENSE file for details.
