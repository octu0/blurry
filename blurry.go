package blurry

import (
	"image"
	"sync"
	"sync/atomic"
)

type ColorModel uint8

const (
	ColorModelARGB ColorModel = iota + 1
	ColorModelABGR
	ColorModelBGRA
	ColorModelRABG
)

type ChromaSubsampling uint8

const (
	ChromaSubsampling420 ChromaSubsampling = iota + 1
	ChromaSubsampling444
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
	return whFromBounds(img.Bounds())
}

func whYCbCr(img *image.YCbCr) (int, int) {
	return whFromBounds(img.Bounds())
}

func whFromBounds(b image.Rectangle) (int, int) {
	w := b.Max.X - b.Min.X
	h := b.Max.Y - b.Min.Y
	return w, h
}

func rgbaSize(width, height int) int {
	return width * height * RGBAStride
}

func GetByteBuf(size int) []byte {
	if atomic.LoadInt32(&usePool) == 0 {
		return make([]byte, size)
	}

	key := uint64(size)
	pool, ok := pools.Load(key)
	if ok != true {
		pool = &sync.Pool{New: poolNewFunc(size)}
		pools.Store(key, pool)
	}
	sp := pool.(*sync.Pool)
	return sp.Get().([]byte)
}

func PutByteBuf(data []byte) {
	if atomic.LoadInt32(&usePool) == 0 {
		return // discard
	}

	key := uint64(cap(data))
	pool, ok := pools.Load(key)
	if ok != true {
		return // discard
	}
	pool.(*sync.Pool).Put(data)
}

func GetRGBAByteBuf(width, height int) []byte {
	return GetByteBuf(rgbaSize(width, height))
}

func PutRGBAByteBuf(data []byte) {
	PutByteBuf(data)
}

func GetRGBA(width, height int) *image.RGBA {
	return &image.RGBA{
		Pix:    GetRGBAByteBuf(width, height),
		Stride: width * 4,
		Rect:   image.Rect(0, 0, width, height),
	}
}

func PutRGBA(img *image.RGBA) {
	PutRGBAByteBuf(img.Pix)
}

func poolNewFunc(size int) func() interface{} {
	return func() interface{} {
		return make([]byte, size)
	}
}
