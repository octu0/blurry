package blurry

/*
#cgo CFLAGS: -I${SRCDIR}/include
#cgo darwin LDFLAGS: -L${SRCDIR}/lib -lruntime_osx -ldl -lm
#cgo darwin LDFLAGS: -lmatch_template_sad_osx
#cgo darwin LDFLAGS: -lmatch_template_ssd_osx
#cgo darwin LDFLAGS: -lmatch_template_ncc_osx
#cgo darwin LDFLAGS: -lmatch_template_zncc_osx
#cgo darwin LDFLAGS: -lprepare_ncc_template_osx
#cgo darwin LDFLAGS: -lprepared_match_template_ncc_osx
#cgo darwin LDFLAGS: -lprepare_zncc_template_osx
#cgo darwin LDFLAGS: -lprepared_match_template_zncc_osx
#cgo linux  LDFLAGS: -L${SRCDIR}/lib -lruntime_linux -ldl -lm
#cgo linux LDFLAGS: -lmatch_template_sad_linux
#cgo linux LDFLAGS: -lmatch_template_ssd_linux
#cgo linux LDFLAGS: -lmatch_template_ncc_linux
#cgo linux LDFLAGS: -lmatch_template_zncc_linux
#cgo linux LDFLAGS: -lprepare_ncc_template_linux
#cgo linux LDFLAGS: -lprepared_match_template_ncc_linux
#cgo linux LDFLAGS: -lprepare_zncc_template_linux
#cgo linux LDFLAGS: -lprepared_match_template_zncc_linux
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "mt.h"
*/
import "C"
import (
	"bytes"
	"encoding/binary"
	"errors"
	"image"
	"math"
)

var (
	ErrMatchTemplateSAD          = errors.New("match_template_sad cgo call error")
	ErrMatchTemplateSSD          = errors.New("match_template_ssd cgo call error")
	ErrMatchTemplateNCC          = errors.New("match_template_ncc cgo call error")
	ErrMatchTemplateZNCC         = errors.New("match_template_zncc cgo call error")
	ErrCreatePrepareNCCTemplate  = errors.New("create_prepare_ncc_template cgo call error")
	ErrPrepareNCCTemplate        = errors.New("prepare_ncc_template cgo call error")
	ErrPreparedMatchTemplateNCC  = errors.New("prepated_match_template_ncc cgo call error")
	ErrCreatePrepareZNCCTemplate = errors.New("create_prepare_zncc_template cgo call error")
	ErrPrepareZNCCTemplate       = errors.New("prepare_zncc_template cgo call error")
	ErrPreparedMatchTemplateZNCC = errors.New("prepated_match_template_zncc cgo call error")
)

type PreparedNCCTpl struct {
	prepared *C.prepared_ncc_template_t
}

type PreparedZNCCTpl struct {
	prepared *C.prepared_zncc_template_t
}

type MatchTemplateIntScore struct {
	Point image.Point
	Score uint32
}

type MatchTemplateFloatScore struct {
	Point image.Point
	Score float64
}

type byteSliceToIntScore func(data []byte) uint32
type byteSliceToFloatScore func(data []byte) float64

func readUint16(data []byte) uint32 {
	score := binary.LittleEndian.Uint16(data)
	return uint32(score)
}

func readInt32(data []byte) uint32 {
	var score int32
	buf := bytes.NewBuffer(data)
	if err := binary.Read(buf, binary.LittleEndian, &score); err != nil {
		return math.MaxUint32
	}
	return uint32(score)
}

func readFloat32(data []byte) float64 {
	var score float64
	buf := bytes.NewBuffer(data)
	if err := binary.Read(buf, binary.LittleEndian, &score); err != nil {
		return -1.0
	}
	return score
}

func makeScoresUint16(data []byte, width, height int, threshold uint16, bitSize int) []MatchTemplateIntScore {
	return makeIntScore(data, width, height, uint32(threshold), readUint16, bitSize)
}

func makeScoresInt32(data []byte, width, height int, threshold uint32, bitSize int) []MatchTemplateIntScore {
	return makeIntScore(data, width, height, threshold, readInt32, bitSize)
}

func makeScoresFloat32(data []byte, width, height int, threshold float64, bitSize int) []MatchTemplateFloatScore {
	return makeFloatScore(data, width, height, threshold, readFloat32, bitSize)
}

func makeIntScore(data []byte, width, height int, threshold uint32, reader byteSliceToIntScore, bitSize int) []MatchTemplateIntScore {
	scores := make([]MatchTemplateIntScore, 0, width*height)
	offset := 0
	for y := 0; y < height; y += 1 {
		for x := 0; x < width; x += 1 {
			score := reader(data[offset : offset+bitSize])
			offset += bitSize

			if threshold < score { // best match is nearest to 0
				continue
			}

			scores = append(scores, MatchTemplateIntScore{
				Point: image.Pt(x, y),
				Score: score,
			})
		}
	}
	return scores
}

func makeFloatScore(data []byte, width, height int, threshold float64, reader byteSliceToFloatScore, bitSize int) []MatchTemplateFloatScore {
	scores := make([]MatchTemplateFloatScore, 0, width*height)
	offset := 0
	for y := 0; y < height; y += 1 {
		for x := 0; x < width; x += 1 {
			score := reader(data[offset : offset+bitSize])
			offset += bitSize

			if math.IsNaN(score) {
				continue
			}
			if score < threshold { // best match is nearest to 1.0
				continue
			}

			scores = append(scores, MatchTemplateFloatScore{
				Point: image.Pt(x, y),
				Score: score,
			})
		}
	}
	return scores
}

func MatchTemplateSAD(img *image.RGBA, tpl *image.RGBA, threshold uint16) ([]MatchTemplateIntScore, error) {
	width, height := wh(img)
	tplWidth, tplHeight := wh(tpl)

	out := GetByteBuf(width * height * C.sizeof_uint16_t)
	defer PutByteBuf(out)

	ret := C.libmtsad(
		(*C.uchar)(&img.Pix[0]),
		C.int(width),
		C.int(height),
		(*C.uchar)(&tpl.Pix[0]),
		C.int(tplWidth),
		C.int(tplHeight),
		(*C.uchar)(&out[0]),
	)
	if int(ret) != 0 {
		return nil, ErrMatchTemplateSAD
	}
	return makeScoresUint16(out, width, height, threshold, C.sizeof_uint16_t), nil
}

func MatchTemplateSSD(img *image.RGBA, tpl *image.RGBA, threshold uint32) ([]MatchTemplateIntScore, error) {
	width, height := wh(img)
	tplWidth, tplHeight := wh(tpl)

	out := GetByteBuf(width * height * C.sizeof_int32_t)
	defer PutByteBuf(out)

	ret := C.libmtssd(
		(*C.uchar)(&img.Pix[0]),
		C.int(width),
		C.int(height),
		(*C.uchar)(&tpl.Pix[0]),
		C.int(tplWidth),
		C.int(tplHeight),
		(*C.uchar)(&out[0]),
	)
	if int(ret) != 0 {
		return nil, ErrMatchTemplateSSD
	}
	return makeScoresInt32(out, width, height, threshold, C.sizeof_int32_t), nil
}

func MatchTemplateNCC(img *image.RGBA, tpl *image.RGBA, threshold float64) ([]MatchTemplateFloatScore, error) {
	width, height := wh(img)
	tplWidth, tplHeight := wh(tpl)

	out := GetByteBuf(width * height * C.sizeof_double)
	defer PutByteBuf(out)

	ret := C.libmtncc(
		(*C.uchar)(&img.Pix[0]),
		C.int(width),
		C.int(height),
		(*C.uchar)(&tpl.Pix[0]),
		C.int(tplWidth),
		C.int(tplHeight),
		(*C.uchar)(&out[0]),
	)
	if int(ret) != 0 {
		return nil, ErrMatchTemplateNCC
	}
	return makeScoresFloat32(out, width, height, threshold, C.sizeof_double), nil
}

func PrepareNCCTemplate(tpl *image.RGBA) (*PreparedNCCTpl, error) {
	tplWidth, tplHeight := wh(tpl)

	p := C.create_prepare_ncc_template(C.int(tplWidth), C.int(tplHeight))
	if p == nil {
		return nil, ErrCreatePrepareNCCTemplate
	}

	ret := C.libprepare_ncc_tpl(
		(*C.uchar)(&tpl.Pix[0]),
		p,
	)
	if ret != 0 {
		return nil, ErrPrepareNCCTemplate
	}
	return &PreparedNCCTpl{prepared: p}, nil
}

func PrepareZNCCTemplate(tpl *image.RGBA) (*PreparedZNCCTpl, error) {
	tplWidth, tplHeight := wh(tpl)

	p := C.create_prepare_zncc_template(C.int(tplWidth), C.int(tplHeight))
	if p == nil {
		return nil, ErrCreatePrepareZNCCTemplate
	}

	ret := C.libprepare_zncc_tpl(
		(*C.uchar)(&tpl.Pix[0]),
		p,
	)
	if ret != 0 {
		return nil, ErrPrepareZNCCTemplate
	}
	return &PreparedZNCCTpl{prepared: p}, nil
}

func FreePreparedNCCTemplate(ptpl *PreparedNCCTpl) {
	C.free_prepare_ncc_template(ptpl.prepared)
}

func FreePreparedZNCCTemplate(ptpl *PreparedZNCCTpl) {
	C.free_prepare_zncc_template(ptpl.prepared)
}

func PreparedMatchTemplateNCC(img *image.RGBA, ptpl *PreparedNCCTpl, threshold float64) ([]MatchTemplateFloatScore, error) {
	width, height := wh(img)

	out := GetByteBuf(width * height * C.sizeof_double)
	defer PutByteBuf(out)

	ret := C.libprepared_mtncc(
		(*C.uchar)(&img.Pix[0]),
		C.int(width),
		C.int(height),
		ptpl.prepared,
		(*C.uchar)(&out[0]),
	)
	if int(ret) != 0 {
		return nil, ErrPrepareNCCTemplate
	}
	return makeScoresFloat32(out, width, height, threshold, C.sizeof_double), nil
}

func MatchTemplateZNCC(img *image.RGBA, tpl *image.RGBA, threshold float64) ([]MatchTemplateFloatScore, error) {
	width, height := wh(img)
	tplWidth, tplHeight := wh(tpl)

	out := GetByteBuf(width * height * C.sizeof_double)
	defer PutByteBuf(out)

	ret := C.libmtzncc(
		(*C.uchar)(&img.Pix[0]),
		C.int(width),
		C.int(height),
		(*C.uchar)(&tpl.Pix[0]),
		C.int(tplWidth),
		C.int(tplHeight),
		(*C.uchar)(&out[0]),
	)
	if int(ret) != 0 {
		return nil, ErrMatchTemplateZNCC
	}
	return makeScoresFloat32(out, width, height, threshold, C.sizeof_double), nil
}

func PreparedMatchTemplateZNCC(img *image.RGBA, ptpl *PreparedZNCCTpl, threshold float64) ([]MatchTemplateFloatScore, error) {
	width, height := wh(img)

	out := GetByteBuf(width * height * C.sizeof_double)
	defer PutByteBuf(out)

	ret := C.libprepared_mtzncc(
		(*C.uchar)(&img.Pix[0]),
		C.int(width),
		C.int(height),
		ptpl.prepared,
		(*C.uchar)(&out[0]),
	)
	if int(ret) != 0 {
		return nil, ErrPrepareZNCCTemplate
	}
	return makeScoresFloat32(out, width, height, threshold, C.sizeof_double), nil
}
