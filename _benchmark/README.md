# Benchmark

In [cgo](https://golang.org/cmd/cgo/), due to the overhead of ffi calls([e.g.](https://about.sourcegraph.com/go/gophercon-2018-adventures-in-cgo-performance/)), 
more complex operations will be optimized for CPU and become faster.  


## Blur

![graph](testdata/blur_bench.png)

/D is [DisablePool](https://pkg.go.dev/github.com/octu0/blurry#DisablePool), i.e. the benchmark when BufferPool is off.

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

## Blend

![graph](testdata/blend_bench.png)

```
goos: darwin
goarch: amd64
pkg: github.com/octu0/blurry/benchmark
cpu: Intel(R) Core(TM) i7-8569U CPU @ 2.80GHz
BenchmarkBlend
BenchmarkBlend/bild/Blend/Normal
BenchmarkBlend/bild/Blend/Normal-8         	    1684	    669237 ns/op	  311461 B/op	       4 allocs/op
BenchmarkBlend/bild/Blend/Add
BenchmarkBlend/bild/Blend/Add-8            	    1702	    728350 ns/op	  311427 B/op	       4 allocs/op
BenchmarkBlend/bild/Blend/Subtract
BenchmarkBlend/bild/Blend/Subtract-8       	    1690	    704828 ns/op	  311425 B/op	       4 allocs/op
BenchmarkBlend/imaging/Paste
BenchmarkBlend/imaging/Paste-8             	    5725	    185487 ns/op	  315751 B/op	      10 allocs/op
BenchmarkBlend/libyuv/ARGBAdd
BenchmarkBlend/libyuv/ARGBAdd-8            	   32508	     36814 ns/op	  311361 B/op	       2 allocs/op
BenchmarkBlend/libyuv/ARGBSubtract
BenchmarkBlend/libyuv/ARGBSubtract-8       	   32386	     36555 ns/op	  311361 B/op	       2 allocs/op
BenchmarkBlend/blurry/Blend/Normal
BenchmarkBlend/blurry/Blend/Normal-8       	   10000	    101359 ns/op	     119 B/op	       2 allocs/op
BenchmarkBlend/blurry/Blend/Add
BenchmarkBlend/blurry/Blend/Add-8          	   10000	    106716 ns/op	     119 B/op	       2 allocs/op
BenchmarkBlend/blurry/Blend/Sub
BenchmarkBlend/blurry/Blend/Sub-8          	    9346	    114087 ns/op	     121 B/op	       2 allocs/op
```

## Contrast

![graph](testdata/contrast_bench.png)

/D is [DisablePool](https://pkg.go.dev/github.com/octu0/blurry#DisablePool), i.e. the benchmark when BufferPool is off.

```
goos: darwin
goarch: amd64
pkg: github.com/octu0/blurry/benchmark
cpu: Intel(R) Core(TM) i7-8569U CPU @ 2.80GHz
BenchmarkContrast
BenchmarkContrast/bild/Contrast
BenchmarkContrast/bild/Contrast-8         	    5983	    192479 ns/op	  311706 B/op	       6 allocs/op
BenchmarkContrast/imaging/Contrast
BenchmarkContrast/imaging/Contrast-8      	    8102	    150315 ns/op	  313796 B/op	       7 allocs/op
BenchmarkContrast/blurry/Contrast
BenchmarkContrast/blurry/Contrast-8       	    8150	    148220 ns/op	     126 B/op	       2 allocs/op
BenchmarkContrast/blurry/Contrast/D
BenchmarkContrast/blurry/Contrast/D-8     	    6895	    169469 ns/op	  311360 B/op	       2 allocs/op
```

## Convert

![graph](testdata/convert_bench.png)

```
goos: darwin
goarch: amd64
pkg: github.com/octu0/blurry/benchmark
cpu: Intel(R) Core(TM) i7-8569U CPU @ 2.80GHz
BenchmarkConvert
BenchmarkConvert/libyuv/RGBAtoARGB
BenchmarkConvert/libyuv/RGBAtoARGB-8         	   38299	     28614 ns/op	  311361 B/op	       2 allocs/op
BenchmarkConvert/libyuv/ARGBToRGBA
BenchmarkConvert/libyuv/ARGBToRGBA-8         	   41935	     28581 ns/op	  311361 B/op	       2 allocs/op
BenchmarkConvert/libyuv/ABGRToARGB
BenchmarkConvert/libyuv/ABGRToARGB-8         	   41905	     28873 ns/op	  311361 B/op	       2 allocs/op
BenchmarkConvert/libyuv/BGRAToARGB
BenchmarkConvert/libyuv/BGRAToARGB-8         	   38294	     30199 ns/op	  311361 B/op	       2 allocs/op
BenchmarkConvert/libyuv/I420ToARGB
BenchmarkConvert/libyuv/I420ToARGB-8         	   33225	     36133 ns/op	  311361 B/op	       2 allocs/op
BenchmarkConvert/libyuv/I444ToARGB
BenchmarkConvert/libyuv/I444ToARGB-8         	   33273	     35939 ns/op	  311361 B/op	       2 allocs/op
BenchmarkConvert/libyuv/RGBAToI420
BenchmarkConvert/libyuv/RGBAToI420-8         	   42591	     28921 ns/op	  123008 B/op	       4 allocs/op
BenchmarkConvert/libyuv/ARGBToI420
BenchmarkConvert/libyuv/ARGBToI420-8         	   53196	     20753 ns/op	  123008 B/op	       4 allocs/op
BenchmarkConvert/libyuv/ARGBToI444
BenchmarkConvert/libyuv/ARGBToI444-8         	   25504	     47617 ns/op	  245889 B/op	       4 allocs/op
BenchmarkConvert/blurry/ConvertFromARGB
BenchmarkConvert/blurry/ConvertFromARGB-8    	   15364	     76489 ns/op	  311493 B/op	       3 allocs/op
BenchmarkConvert/blurry/ConvertFromABGR
BenchmarkConvert/blurry/ConvertFromABGR-8    	   14355	     89283 ns/op	  311495 B/op	       3 allocs/op
BenchmarkConvert/blurry/ConvertFromBGRA
BenchmarkConvert/blurry/ConvertFromBGRA-8    	   10000	    108980 ns/op	  311511 B/op	       3 allocs/op
BenchmarkConvert/blurry/ConvertFromYUV420
BenchmarkConvert/blurry/ConvertFromYUV420-8  	   13333	     91177 ns/op	  311525 B/op	       3 allocs/op
BenchmarkConvert/blurry/ConvertFromYUV444
BenchmarkConvert/blurry/ConvertFromYUV444-8  	   13305	     88596 ns/op	  311519 B/op	       3 allocs/op
BenchmarkConvert/blurry/ConvertToYUV444
BenchmarkConvert/blurry/ConvertToYUV444-8    	    5724	    212215 ns/op	  237815 B/op	       3 allocs/op
```

## Crop

![graph](testdata/crop_bench.png)

```
goos: darwin
goarch: amd64
pkg: github.com/octu0/blurry/benchmark
cpu: Intel(R) Core(TM) i7-8569U CPU @ 2.80GHz
BenchmarkCrop
BenchmarkCrop/bild/Crop
BenchmarkCrop/bild/Crop-8         	  520618	      2270 ns/op	   16512 B/op	       3 allocs/op
BenchmarkCrop/imaging/Crop
BenchmarkCrop/imaging/Crop-8      	   69266	     18538 ns/op	   17104 B/op	       6 allocs/op
BenchmarkCrop/libyuv/ARGBScaleClip
BenchmarkCrop/libyuv/ARGBScaleClip-8         	   45445	     29718 ns/op	  311360 B/op	       2 allocs/op
BenchmarkCrop/blurry/Crop
BenchmarkCrop/blurry/Crop-8                  	   63726	     17627 ns/op	   16478 B/op	       3 allocs/op
```

## Edge

![graph](testdata/edge_bench.png)

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

## Emboss

![graph](testdata/emboss_bench.png)

```
goos: darwin
goarch: amd64
pkg: github.com/octu0/blurry/benchmark
cpu: Intel(R) Core(TM) i7-8569U CPU @ 2.80GHz
BenchmarkEmboss
BenchmarkEmboss/bild/Emboss
BenchmarkEmboss/bild/Emboss-8         	     612	   1942848 ns/op	  631282 B/op	      10 allocs/op
BenchmarkEmboss/imaging/Convolve3x3
BenchmarkEmboss/imaging/Convolve3x3-8 	    1234	    978724 ns/op	  627413 B/op	      16 allocs/op
BenchmarkEmboss/blurry/Emboss
BenchmarkEmboss/blurry/Emboss-8       	    4455	    252560 ns/op	      88 B/op	       2 allocs/op
```

## Gamma

![graph](testdata/gamma_bench.png)

```
goos: darwin
goarch: amd64
pkg: github.com/octu0/blurry/benchmark
cpu: Intel(R) Core(TM) i7-8569U CPU @ 2.80GHz
BenchmarkGamma
BenchmarkGamma/bild/Gamma
BenchmarkGamma/bild/Gamma-8         	    5610	    211753 ns/op	  311710 B/op	       6 allocs/op
BenchmarkGamma/imaging/Gamma
BenchmarkGamma/imaging/Gamma-8      	    7389	    167046 ns/op	  313794 B/op	       7 allocs/op
BenchmarkGamma/blurry/Gamma
BenchmarkGamma/blurry/Gamma-8       	    5488	    220096 ns/op	  311487 B/op	       3 allocs/op
```

## Grayscale

![graph](testdata/grayscale_bench.png)

```
goos: darwin
goarch: amd64
pkg: github.com/octu0/blurry/benchmark
cpu: Intel(R) Core(TM) i7-8569U CPU @ 2.80GHz
BenchmarkGrayscale
BenchmarkGrayscale/bild/Grayscale
BenchmarkGrayscale/bild/Grayscale-8         	    6885	    170681 ns/op	  622812 B/op	       6 allocs/op
BenchmarkGrayscale/imaging/Grayscale
BenchmarkGrayscale/imaging/Grayscale-8      	    7684	    152394 ns/op	  313513 B/op	       6 allocs/op
BenchmarkGrayscale/libyuv/ARGBGrayTo
BenchmarkGrayscale/libyuv/ARGBGrayTo-8      	   27867	     42660 ns/op	  311361 B/op	       2 allocs/op
BenchmarkGrayscale/blurry/Grayscale
BenchmarkGrayscale/blurry/Grayscale-8       	   10000	    105756 ns/op	     119 B/op	       2 allocs/op
BenchmarkGrayscale/blurry/Grayscale/D
BenchmarkGrayscale/blurry/Grayscale/D-8     	    8874	    131734 ns/op	  311361 B/op	       2 allocs/op
```

## Rotate

![graph](testdata/rotate_bench.png)

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

## Scale

![graph](testdata/scale_bench.png)

```
goos: darwin
goarch: amd64
pkg: github.com/octu0/blurry/benchmark
cpu: Intel(R) Core(TM) i7-8569U CPU @ 2.80GHz
BenchmarkScale
BenchmarkScale/bild/Resize/NearestNeighbor
BenchmarkScale/bild/Resize/NearestNeighbor-8         	   19599	     62202 ns/op	   49216 B/op	       2 allocs/op
BenchmarkScale/bild/Resize/Box
BenchmarkScale/bild/Resize/Box-8                     	    2521	    493470 ns/op	  172385 B/op	       8 allocs/op
BenchmarkScale/bild/Resize/Linear
BenchmarkScale/bild/Resize/Linear-8                  	    1473	    816779 ns/op	  172384 B/op	       8 allocs/op
BenchmarkScale/bild/Resize/Gaussian
BenchmarkScale/bild/Resize/Gaussian-8                	     130	   8390509 ns/op	  172384 B/op	       8 allocs/op
BenchmarkScale/imaging/Resize/NearestNeighbor
BenchmarkScale/imaging/Resize/NearestNeighbor-8      	   16652	     71154 ns/op	   50241 B/op	       6 allocs/op
BenchmarkScale/imaging/Resize/Box
BenchmarkScale/imaging/Resize/Box-8                  	    3200	    398176 ns/op	  228098 B/op	      32 allocs/op
BenchmarkScale/imaging/Resize/Linear
BenchmarkScale/imaging/Resize/Linear-8               	    2520	    430057 ns/op	  236288 B/op	      32 allocs/op
BenchmarkScale/imaging/Resize/Gaussian
BenchmarkScale/imaging/Resize/Gaussian-8             	    1797	    622349 ns/op	  249858 B/op	      32 allocs/op
BenchmarkScale/libyuv/ARGBScale/None
BenchmarkScale/libyuv/ARGBScale/None-8               	   98314	     11084 ns/op	   49216 B/op	       2 allocs/op
BenchmarkScale/libyuv/ARGBScale/Box
BenchmarkScale/libyuv/ARGBScale/Box-8                	   62830	     19139 ns/op	   49216 B/op	       2 allocs/op
BenchmarkScale/libyuv/ARGBScale/Linear
BenchmarkScale/libyuv/ARGBScale/Linear-8             	   73524	     15755 ns/op	   49216 B/op	       2 allocs/op
BenchmarkScale/blurry/Scale/None
BenchmarkScale/blurry/Scale/None-8                   	   38918	     31137 ns/op	   49259 B/op	       3 allocs/op
BenchmarkScale/blurry/Scale/Box
BenchmarkScale/blurry/Scale/Box-8                    	   12050	     97651 ns/op	   49260 B/op	       3 allocs/op
BenchmarkScale/blurry/Scale/Linear
BenchmarkScale/blurry/Scale/Linear-8                 	   10000	    104896 ns/op	   49260 B/op	       3 allocs/op
BenchmarkScale/blurry/Scale/Gaussian
BenchmarkScale/blurry/Scale/Gaussian-8               	   10000	    112967 ns/op	   49260 B/op	       3 allocs/op
```

## Sobel

![graph](testdata/sobel_bench.png)

```
goos: darwin
goarch: amd64
pkg: github.com/octu0/blurry/benchmark
cpu: Intel(R) Core(TM) i7-8569U CPU @ 2.80GHz
BenchmarkSobel
BenchmarkSobel/bild/Sobel
BenchmarkSobel/bild/Sobel-8         	     210	   5645878 ns/op	 2196751 B/op	      32 allocs/op
BenchmarkSobel/libyuv/ARGBSobel
BenchmarkSobel/libyuv/ARGBSobel-8   	   16543	     72586 ns/op	  311361 B/op	       2 allocs/op
BenchmarkSobel/blurry/Sobel
BenchmarkSobel/blurry/Sobel-8       	    6816	    177510 ns/op	  311514 B/op	       3 allocs/op
```
