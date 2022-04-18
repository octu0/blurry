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
BenchmarkJIT/cloneimg                      : 0.00828ms
BenchmarkJIT/convert_from_argb             : 0.02433ms
BenchmarkJIT/convert_from_abgr             : 0.03744ms
BenchmarkJIT/convert_from_bgra             : 0.02504ms
BenchmarkJIT/convert_from_rabg             : 0.03304ms
BenchmarkJIT/convert_from_yuv_420          : 0.02976ms
BenchmarkJIT/convert_from_yuv_444          : 0.02669ms
BenchmarkJIT/convert_to_yuv_420            : 0.05801ms
BenchmarkJIT/convert_to_yuv_444            : 0.07256ms
BenchmarkJIT/rotate0                       : 0.00827ms
BenchmarkJIT/rotate90                      : 0.02512ms
BenchmarkJIT/rotate180                     : 0.00764ms
BenchmarkJIT/rotate270                     : 0.02568ms
BenchmarkJIT/crop                          : 0.06114ms
BenchmarkJIT/scale                         : 0.14102ms
BenchmarkJIT/scale_box                     : 0.20871ms
BenchmarkJIT/scale_linear                  : 0.21035ms
BenchmarkJIT/scale_gaussian                : 0.31641ms
BenchmarkJIT/blend_normal$1                : 0.08424ms
BenchmarkJIT/blend_sub                     : 0.08446ms
BenchmarkJIT/blend_add                     : 0.08519ms
BenchmarkJIT/blend_diff                    : 0.08393ms
BenchmarkJIT/grayscale                     : 0.03982ms
BenchmarkJIT/invert                        : 0.03996ms
BenchmarkJIT/brightness                    : 0.04356ms
BenchmarkJIT/gammacorrection               : 0.07944ms
BenchmarkJIT/contrast                      : 0.01561ms
BenchmarkJIT/boxblur                       : 0.11670ms
BenchmarkJIT/gaussianblur                  : 0.31318ms
BenchmarkJIT/blockmozaic                   : 0.27497ms
BenchmarkJIT/erosion                       : 0.12557ms
BenchmarkJIT/dilation                      : 0.12463ms
BenchmarkJIT/morphology_open               : 0.10129ms
BenchmarkJIT/morphology_close              : 0.10146ms
BenchmarkJIT/morphology_gradient           : 0.07967ms
BenchmarkJIT/emboss$1                      : 0.04691ms
BenchmarkJIT/laplacian                     : 0.03240ms
BenchmarkJIT/highpass                      : 0.03840ms
BenchmarkJIT/gradient                      : 0.03355ms
BenchmarkJIT/edgedetect                    : 0.02747ms
BenchmarkJIT/sobel                         : 0.06346ms
BenchmarkJIT/canny                         : 0.29947ms
BenchmarkJIT/canny_dilate                  : 0.34755ms
BenchmarkJIT/canny_morphology_open         : 0.39220ms
BenchmarkJIT/canny_morphology_close        : 0.39533ms
BenchmarkJIT/match_template_sad            : 5.52883ms
BenchmarkJIT/match_template_ssd            : 4.55103ms
BenchmarkJIT/match_template_ncc            : 8.14574ms
BenchmarkJIT/prepared_match_template_ncc   : 6.01201ms
BenchmarkJIT/match_template_zncc           : 11.76755ms
BenchmarkJIT/prepared_match_template_zncc  : 10.86375ms
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
BenchmarkBlur/bild/blur/Box-8         	     154	   7812824 ns/op	  640402 B/op	      11 allocs/op
BenchmarkBlur/bild/blur/Gaussian
BenchmarkBlur/bild/blur/Gaussian-8    	     333	   3486751 ns/op	 1262485 B/op	      21 allocs/op
BenchmarkBlur/imaging/Blur
BenchmarkBlur/imaging/Blur-8          	     786	   1520193 ns/op	  793698 B/op	      45 allocs/op
BenchmarkBlur/stackblur-go
BenchmarkBlur/stackblur-go-8          	     231	   5147219 ns/op	  925937 B/op	  153609 allocs/op
BenchmarkBlur/libyuv/ARGBBlur
BenchmarkBlur/libyuv/ARGBBlur-8       	    1861	    642486 ns/op	10182722 B/op	       3 allocs/op
BenchmarkBlur/blurry/Boxblur
BenchmarkBlur/blurry/Boxblur-8        	    7257	    178086 ns/op	      88 B/op	       2 allocs/op
BenchmarkBlur/blurry/Gaussianblur
BenchmarkBlur/blurry/Gaussianblur-8   	    5367	    222615 ns/op	     146 B/op	       2 allocs/op
BenchmarkBlur/blurry/Boxblur/D
BenchmarkBlur/blurry/Boxblur/D-8      	    6093	    201573 ns/op	  311361 B/op	       2 allocs/op
BenchmarkBlur/blurry/Gaussianblur/D
BenchmarkBlur/blurry/Gaussianblur/D-8 	    4629	    257483 ns/op	  311361 B/op	       2 allocs/op
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
BenchmarkEdge/bild/EdgeDetection-8         	     643	   1858350 ns/op	  631257 B/op	      10 allocs/op
BenchmarkEdge/blurry/Edge
BenchmarkEdge/blurry/Edge-8                	   10000	    100695 ns/op	  311513 B/op	       3 allocs/op
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
BenchmarkRotate/bild/Rotate/90-8         	     612	   2080543 ns/op	 1237046 B/op	  115685 allocs/op
BenchmarkRotate/bild/Rotate/180
BenchmarkRotate/bild/Rotate/180-8        	     480	   2355424 ns/op	 1540311 B/op	  153605 allocs/op
BenchmarkRotate/bild/Rotate/270
BenchmarkRotate/bild/Rotate/270-8        	     520	   2061518 ns/op	 1236932 B/op	  115685 allocs/op
BenchmarkRotate/imaging/90
BenchmarkRotate/imaging/90-8             	    7918	    130736 ns/op	  314181 B/op	       6 allocs/op
BenchmarkRotate/imaging/180
BenchmarkRotate/imaging/180-8            	    9654	    138252 ns/op	  313542 B/op	       6 allocs/op
BenchmarkRotate/imaging/270
BenchmarkRotate/imaging/270-8            	    6972	    163349 ns/op	  314165 B/op	       6 allocs/op
BenchmarkRotate/libyuv/ARGBRotate/90
BenchmarkRotate/libyuv/ARGBRotate/90-8   	   13423	     81131 ns/op	  311360 B/op	       2 allocs/op
BenchmarkRotate/libyuv/ARGBRotate/180
BenchmarkRotate/libyuv/ARGBRotate/180-8  	   34771	     34425 ns/op	  311361 B/op	       2 allocs/op
BenchmarkRotate/libyuv/ARGBRotate/270
BenchmarkRotate/libyuv/ARGBRotate/270-8  	   15904	     78290 ns/op	  311361 B/op	       2 allocs/op
BenchmarkRotate/blurry/Rotate/90
BenchmarkRotate/blurry/Rotate/90-8       	   10000	    109336 ns/op	  311514 B/op	       3 allocs/op
BenchmarkRotate/blurry/Rotate/180
BenchmarkRotate/blurry/Rotate/180-8      	   13102	     89067 ns/op	  311514 B/op	       3 allocs/op
BenchmarkRotate/blurry/Rotate/270
BenchmarkRotate/blurry/Rotate/270-8      	   10000	    124949 ns/op	  311514 B/op	       3 allocs/op
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
BenchmarkSobel/bild/Sobel-8         	     208	   5707148 ns/op	 2196784 B/op	      32 allocs/op
BenchmarkSobel/libyuv/ARGBSobel
BenchmarkSobel/libyuv/ARGBSobel-8   	   16557	     72320 ns/op	  311361 B/op	       2 allocs/op
BenchmarkSobel/blurry/Sobel
BenchmarkSobel/blurry/Sobel-8       	    9255	    140586 ns/op	  311515 B/op	       3 allocs/op
```

### Other Benchmarks

See [_benchmark](https://github.com/octu0/blurry/tree/master/_benchmark) for benchmarks of other methods and performance comparison with [libyuv](https://chromium.googlesource.com/libyuv/libyuv/).


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

### Flip

flipV(vertically) flipH(horizontally)

```go
img, err := blurry.Flip(input, blurry.FlipVertical)
```

| `blurry.FlipMode`     | Result                             |
| :-------------------: | :--------------------------------: |
| `blurry.FlipVertical` | ![example](testdata/flipv.png)  |
| `blurry.FlipHorizon`  | ![example](testdata/fliph.png) |


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

### Contour

Extract contours based on the sobel filter for binarization.  
In actual use, it is better to denoise the image before passing it through the sobel filter.

```go
points, err := blurry.Contour(input, 100, 4)
```

| `threshold` | size | Result                               |
| :---------: | :--: | :----------------------------------: |
| 100         | 4 | ![example](testdata/contour_100_4.png)  |
| 150         | 4 | ![example](testdata/contour_150_4.png)  |
| 200         | 4 | ![example](testdata/contour_200_4.png)  |
| 250         | 4 | ![example](testdata/contour_250_4.png)  |
| 100         | 2 | ![example](testdata/contour_100_2.png)  |
| 200         | 2 | ![example](testdata/contour_200_2.png)  |

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

| Subsampling  | Method                                                              |
| :----------: | :-----------------------------------------------------------------: |
| 420          | `blurry.ConvertFromYUV420Plane(y,u,v []byte, int,int,int, w,h int)` |
| 444          | `blurry.ConvertFromYUV444Plane(y,u,v []byte, int,int,int, w,h int)` |

#### Write: YUV Chroma Subsampling

```go
ycbcr, err := blurry.ConvertToYUV444(rgba)
```

| Subsampling  | Method                                |
| :----------: | :-----------------------------------: |
| 420          | `blurry.ConvertToYUV420(*image.RGBA)` |
| 444          | `blurry.ConvertToYUV444(*image.RGBA)` |

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
   1.19.0

COMMANDS:
     blend             
     blockmozaic       
     boxblur           
     brightness        
     canny             
     clone             
     contour           
     contrast          
     convert           
     convert_from_yuv  
     convert_to_yuv    
     crop              
     dilation          
     edge              
     emboss            
     erosion           
     flip              
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
