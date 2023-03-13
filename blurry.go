package blurry

import (
	"image"
	"sync/atomic"

	"github.com/octu0/bp"
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
	usePool = int32(1)
	Pool    = bp.NewMultiBytePool(
		bp.MultiBytePoolSize(100, rgbaSize(160, 120)),
		bp.MultiBytePoolSize(100, rgbaSize(320, 240)),
		bp.MultiBytePoolSize(100, rgbaSize(480, 320)),
		bp.MultiBytePoolSize(100, rgbaSize(640, 480)),
		bp.MultiBytePoolSize(100, rgbaSize(800, 600)),
		bp.MultiBytePoolSize(100, rgbaSize(1024, 768)),
		bp.MultiBytePoolSize(100, rgbaSize(1280, 720)),
		bp.MultiBytePoolSize(100, rgbaSize(1280, 768)),
		bp.MultiBytePoolSize(100, rgbaSize(1280, 960)),
		bp.MultiBytePoolSize(100, rgbaSize(1280, 1024)),
		bp.MultiBytePoolSize(100, rgbaSize(1600, 1200)),
		bp.MultiBytePoolSize(100, rgbaSize(1920, 1080)),
		bp.MultiBytePoolSize(100, rgbaSize(2560, 1440)),
		bp.MultiBytePoolSize(100, rgbaSize(3840, 2160)),
	)
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
	return Pool.Get(size)
}

func PutByteBuf(data []byte) {
	if atomic.LoadInt32(&usePool) == 0 {
		return // discard
	}

	Pool.Put(data)
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
