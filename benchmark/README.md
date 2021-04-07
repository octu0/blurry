# Benchmark

In [cgo](https://golang.org/cmd/cgo/), due to the overhead of ffi calls([e.g.](https://about.sourcegraph.com/go/gophercon-2018-adventures-in-cgo-performance/)), 
more complex operations will be optimized for CPU and become faster.  

## Blur

/D is [DisablePool](https://pkg.go.dev/github.com/octu0/blurry#DisablePool), i.e. the benchmark when BufferPool is off.

```
goos: darwin
goarch: amd64
pkg: github.com/octu0/blurry/benchmark
cpu: Intel(R) Core(TM) i7-8569U CPU @ 2.80GHz
BenchmarkBlur
BenchmarkBlur/bild/blur/Box
BenchmarkBlur/bild/blur/Box-8         	     152	   7841712 ns/op	  640313 B/op	      11 allocs/op
BenchmarkBlur/bild/blur/Gaussian
BenchmarkBlur/bild/blur/Gaussian-8    	     327	   3555331 ns/op	 1262504 B/op	      21 allocs/op
BenchmarkBlur/imaging/Blur
BenchmarkBlur/imaging/Blur-8          	     788	   1482044 ns/op	  793709 B/op	      45 allocs/op
BenchmarkBlur/stackblur-go
BenchmarkBlur/stackblur-go-8          	     246	   4851663 ns/op	  925933 B/op	  153609 allocs/op
BenchmarkBlur/libyuv/ARGBBlur
BenchmarkBlur/libyuv/ARGBBlur-8       	    1854	    650947 ns/op	10182723 B/op	       3 allocs/op
BenchmarkBlur/blurry/Boxblur
BenchmarkBlur/blurry/Boxblur-8        	    4244	    304893 ns/op	     161 B/op	       2 allocs/op
BenchmarkBlur/blurry/Gaussianblur
BenchmarkBlur/blurry/Gaussianblur-8   	    5559	    209911 ns/op	     144 B/op	       2 allocs/op
BenchmarkBlur/blurry/Boxblur/D
BenchmarkBlur/blurry/Boxblur/D-8      	    3573	    313649 ns/op	  311361 B/op	       2 allocs/op
BenchmarkBlur/blurry/Gaussianblur/D
BenchmarkBlur/blurry/Gaussianblur/D-8 	    5192	    230376 ns/op	  311361 B/op	       2 allocs/op
```

## Sobel

```
goos: darwin
goarch: amd64
pkg: github.com/octu0/blurry/benchmark
cpu: Intel(R) Core(TM) i7-8569U CPU @ 2.80GHz
BenchmarkSobel
BenchmarkSobel/bild/Sobel
BenchmarkSobel/bild/Sobel-8         	     217	   5576115 ns/op	 2196792 B/op	      32 allocs/op
BenchmarkSobel/libyuv/ARGBSobel
BenchmarkSobel/libyuv/ARGBSobel-8   	   16981	     70402 ns/op	  311361 B/op	       2 allocs/op
BenchmarkSobel/blurry/Sobel
BenchmarkSobel/blurry/Sobel-8       	    5600	    221934 ns/op	  311488 B/op	       3 allocs/op
```

## Contrast

/D is [DisablePool](https://pkg.go.dev/github.com/octu0/blurry#DisablePool), i.e. the benchmark when BufferPool is off.

```
goos: darwin
goarch: amd64
pkg: github.com/octu0/blurry/benchmark
cpu: Intel(R) Core(TM) i7-8569U CPU @ 2.80GHz
BenchmarkContrast
BenchmarkContrast/bild/Contrast
BenchmarkContrast/bild/Contrast-8         	    6477	    177733 ns/op	  311705 B/op	       6 allocs/op
BenchmarkContrast/imaging/Contrast
BenchmarkContrast/imaging/Contrast-8      	    8548	    144601 ns/op	  313794 B/op	       7 allocs/op
BenchmarkContrast/blurry/Contrast
BenchmarkContrast/blurry/Contrast-8       	   10000	    108199 ns/op	     119 B/op	       2 allocs/op
BenchmarkContrast/blurry/Contrast/D
BenchmarkContrast/blurry/Contrast/D-8     	    8751	    132169 ns/op	  311360 B/op	       2 allocs/op
```

## Gamma

```
goos: darwin
goarch: amd64
pkg: github.com/octu0/blurry/benchmark
cpu: Intel(R) Core(TM) i7-8569U CPU @ 2.80GHz
BenchmarkGamma
BenchmarkGamma/bild/Gamma
BenchmarkGamma/bild/Gamma-8         	    6238	    190844 ns/op	  311697 B/op	       6 allocs/op
BenchmarkGamma/imaging/Gamma
BenchmarkGamma/imaging/Gamma-8      	    8145	    155915 ns/op	  313794 B/op	       7 allocs/op
BenchmarkGamma/blurry/Gamma
BenchmarkGamma/blurry/Gamma-8       	    6817	    176591 ns/op	  311487 B/op	       3 allocs/op

```

## Grayscale

/D is [DisablePool](https://pkg.go.dev/github.com/octu0/blurry#DisablePool), i.e. the benchmark when BufferPool is off.

```
goos: darwin
goarch: amd64
pkg: github.com/octu0/blurry/benchmark
cpu: Intel(R) Core(TM) i7-8569U CPU @ 2.80GHz
BenchmarkGrayscale
BenchmarkGrayscale/bild/Grayscale
BenchmarkGrayscale/bild/Grayscale-8         	    7317	    166366 ns/op	  622812 B/op	       6 allocs/op
BenchmarkGrayscale/imaging/Grayscale
BenchmarkGrayscale/imaging/Grayscale-8      	    8612	    142327 ns/op	  313515 B/op	       6 allocs/op
BenchmarkGrayscale/libyuv/ARGBGrayTo
BenchmarkGrayscale/libyuv/ARGBGrayTo-8      	   28510	     41736 ns/op	  311360 B/op	       2 allocs/op
BenchmarkGrayscale/blurry/Grayscale
BenchmarkGrayscale/blurry/Grayscale-8       	   10000	    117069 ns/op	     119 B/op	       2 allocs/op
BenchmarkGrayscale/blurry/Grayscale/D
BenchmarkGrayscale/blurry/Grayscale/D-8     	    8568	    140341 ns/op	  311361 B/op	       2 allocs/op
```
