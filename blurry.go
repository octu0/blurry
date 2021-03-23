package blurry

import (
	"image"
	"sync"
	"sync/atomic"
)

const (
	RGBAStride int = 4
)

var (
	pools   = new(sync.Map)
	usePool = int32(1)
)

func EnablePool() {
	atomic.StoreInt32(&usePool, 1)
}

func DisablePool() {
	atomic.StoreInt32(&usePool, 0)
}

func wh(img *image.RGBA) (int, int) {
	b := img.Bounds()
	w := b.Max.X - b.Min.X
	h := b.Max.Y - b.Min.Y
	return w, h
}

func rgbaSize(width, height int) int {
	return width * height * RGBAStride
}

func poolKey(width, height int) uint64 {
	return (uint64(width) * 1000) + uint64(height)
}

func GetNRGBA(width, height int) *image.RGBA {
	if atomic.LoadInt32(&usePool) == 0 {
		return image.NewRGBA(image.Rect(0, 0, width, height))
	}

	key := poolKey(width, height)
	pool, ok := pools.Load(key)
	if ok != true {
		pool = &sync.Pool{New: poolNewFunc(width, height)}
		pools.Store(key, pool)
	}
	sp := pool.(*sync.Pool)
	return sp.Get().(*image.RGBA)
}

func PutNRGBA(img *image.RGBA) {
	width, height := wh(img)
	key := poolKey(width, height)
	pool, ok := pools.Load(key)
	if ok != true {
		return // discard
	}
	pool.(*sync.Pool).Put(img)
}

func poolNewFunc(width, height int) func() interface{} {
	return func() interface{} {
		size := rgbaSize(width, height)
		return &image.RGBA{
			Pix:    make([]byte, size),
			Stride: width * RGBAStride,
			Rect:   image.Rect(0, 0, width, height),
		}
	}
}
