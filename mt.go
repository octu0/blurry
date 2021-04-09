package blurry

/*
#cgo CFLAGS: -I${SRCDIR}/include
#cgo darwin LDFLAGS: -L${SRCDIR}/lib -lruntime_osx -lmatch_template_sad_osx -lmatch_template_ssd_osx -lmatch_template_ncc_osx -lmatch_template_zncc_osx -ldl -lm
#cgo linux  LDFLAGS: -L${SRCDIR}/lib -lruntime_linux -lmatch_template_sad_linux -lmatch_template_ssd_linux -lmatch_template_ncc_linux -lmatch_template_zncc_linux -ldl -lm
#include <stdlib.h>
#include <string.h>

#include "bridge.h"
#ifdef __APPLE__
#include "libmatch_template_sad_osx.h"
#include "libmatch_template_ssd_osx.h"
#include "libmatch_template_ncc_osx.h"
#include "libmatch_template_zncc_osx.h"
#elif __linux__
#include "libmatch_template_sad_linux.h"
#include "libmatch_template_ssd_linux.h"
#include "libmatch_template_ncc_linux.h"
#include "libmatch_template_zncc_linux.h"
#endif

int libmtsad(
  unsigned char *src, int32_t width, int32_t height,
  unsigned char *tpl, int32_t tpl_width, int32_t tpl_height,
  unsigned char *out
) {
  halide_buffer_t *in_src_buf = create_rgba_buffer(src, width, height);
  if(in_src_buf == NULL){
    return 1;
  }
  halide_buffer_t *in_tpl_buf = create_rgba_buffer(tpl, tpl_width, tpl_height);
  if(in_tpl_buf == NULL){
    free_buf(in_src_buf);
    return 1;
  }
  halide_buffer_t *out_buf = create_uint16_array_buffer(out, width, height);
  if(out_buf == NULL){
    free_buf(in_src_buf);
    free_buf(in_tpl_buf);
    return 1;
  }

  int ret = match_template_sad(
    in_src_buf, width, height,
    in_tpl_buf, tpl_width, tpl_height,
    out_buf
  );
  free_buf(in_src_buf);
  free_buf(in_tpl_buf);
  free_buf(out_buf);
  return ret;
}

int libmtssd(
  unsigned char *src, int32_t width, int32_t height,
  unsigned char *tpl, int32_t tpl_width, int32_t tpl_height,
  unsigned char *out
) {
  halide_buffer_t *in_src_buf = create_rgba_buffer(src, width, height);
  if(in_src_buf == NULL){
    return 1;
  }
  halide_buffer_t *in_tpl_buf = create_rgba_buffer(tpl, tpl_width, tpl_height);
  if(in_tpl_buf == NULL){
    free_buf(in_src_buf);
    return 1;
  }
  halide_buffer_t *out_buf = create_int32_array_buffer(out, width, height);
  if(out_buf == NULL){
    free_buf(in_src_buf);
    free_buf(in_tpl_buf);
    return 1;
  }

  int ret = match_template_ssd(
    in_src_buf, width, height,
    in_tpl_buf, tpl_width, tpl_height,
    out_buf
  );
  free_buf(in_src_buf);
  free_buf(in_tpl_buf);
  free_buf(out_buf);
  return ret;
}

int libmtncc(
  unsigned char *src, int32_t width, int32_t height,
  unsigned char *tpl, int32_t tpl_width, int32_t tpl_height,
  unsigned char *out
) {
  halide_buffer_t *in_src_buf = create_rgba_buffer(src, width, height);
  if(in_src_buf == NULL){
    return 1;
  }
  halide_buffer_t *in_tpl_buf = create_rgba_buffer(tpl, tpl_width, tpl_height);
  if(in_tpl_buf == NULL){
    free_buf(in_src_buf);
    return 1;
  }
  halide_buffer_t *out_buf = create_double_array_buffer(out, width, height);
  if(out_buf == NULL){
    free_buf(in_src_buf);
    free_buf(in_tpl_buf);
    return 1;
  }

  int ret = match_template_ncc(
    in_src_buf, width, height,
    in_tpl_buf, tpl_width, tpl_height,
    out_buf
  );
  free_buf(in_src_buf);
  free_buf(in_tpl_buf);
  free_buf(out_buf);
  return ret;
}

int libmtzncc(
  unsigned char *src, int32_t width, int32_t height,
  unsigned char *tpl, int32_t tpl_width, int32_t tpl_height,
  unsigned char *out
) {
  halide_buffer_t *in_src_buf = create_rgba_buffer(src, width, height);
  if(in_src_buf == NULL){
    return 1;
  }
  halide_buffer_t *in_tpl_buf = create_rgba_buffer(tpl, tpl_width, tpl_height);
  if(in_tpl_buf == NULL){
    free_buf(in_src_buf);
    return 1;
  }
  halide_buffer_t *out_buf = create_double_array_buffer(out, width, height);
  if(out_buf == NULL){
    free_buf(in_src_buf);
    free_buf(in_tpl_buf);
    return 1;
  }

  int ret = match_template_zncc(
    in_src_buf, width, height,
    in_tpl_buf, tpl_width, tpl_height,
    out_buf
  );
  free_buf(in_src_buf);
  free_buf(in_tpl_buf);
  free_buf(out_buf);
  return ret;
}
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
	ErrMatchTemplateSAD  = errors.New("match_template_sad cgo call error")
	ErrMatchTemplateSSD  = errors.New("match_template_ssd cgo call error")
	ErrMatchTemplateNCC  = errors.New("match_template_ncc cgo call error")
	ErrMatchTemplateZNCC = errors.New("match_template_zncc cgo call error")
)

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
