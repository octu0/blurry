#ifdef __APPLE__
#include "libmatch_template_sad_osx.h"
#include "libmatch_template_ssd_osx.h"
#include "libmatch_template_ncc_osx.h"
#include "libmatch_template_zncc_osx.h"
#include "libprepare_ncc_template_osx.h"
#include "libprepared_match_template_ncc_osx.h"
#include "libprepare_zncc_template_osx.h"
#include "libprepared_match_template_zncc_osx.h"
#elif __linux__
#include "libmatch_template_sad_linux.h"
#include "libmatch_template_ssd_linux.h"
#include "libmatch_template_ncc_linux.h"
#include "libmatch_template_zncc_linux.h"
#include "libprepare_ncc_template_linux.h"
#include "libprepared_match_template_ncc_linux.h"
#include "libprepare_zncc_template_linux.h"
#include "libprepared_match_template_zncc_linux.h"
#endif

#include <stdlib.h>
#include "bridge.h"
#include "mt.h"

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

//
// {{{ NCC
//
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

prepared_ncc_template_t *create_prepare_ncc_template(int32_t tpl_width, int32_t tpl_height) {
  prepared_ncc_template_t *p = (prepared_ncc_template_t *) malloc(sizeof(prepared_ncc_template_t));
  if(p == NULL) {
    return NULL;
  }

  unsigned char *tuple_1 = (unsigned char *) malloc(tpl_width * tpl_height * sizeof(float));
  if(tuple_1 == NULL) {
    free(p);
    return NULL;
  }
  unsigned char *tuple_2 = (unsigned char *) malloc(tpl_width * tpl_height * sizeof(float));
  if(tuple_2 == NULL) {
    free(p);
    free(tuple_1);
    return NULL;
  }

  p->tpl_width = tpl_width;
  p->tpl_height = tpl_height;
  p->tuple_1 = tuple_1;
  p->tuple_2 = tuple_2;
  return p;
}

void free_prepare_ncc_template(prepared_ncc_template_t *p) {
  if(p != NULL) {
    free(p->tuple_1);
    free(p->tuple_2);
  }
  free(p);
}

int libprepare_ncc_tpl(unsigned char *tpl, prepared_ncc_template_t *p){
  halide_buffer_t *in_tpl_buf = create_rgba_buffer(tpl, p->tpl_width, p->tpl_height);
  if(in_tpl_buf == NULL){
    return 1;
  }
  halide_buffer_t *out_tpl_1_buf = create_float_array_buffer(p->tuple_1, p->tpl_width, p->tpl_height);
  if(out_tpl_1_buf == NULL) {
    free_buf(in_tpl_buf);
    return 1;
  }
  halide_buffer_t *out_tpl_2_buf = create_float_array_buffer(p->tuple_2, p->tpl_width, p->tpl_height);
  if(out_tpl_2_buf == NULL) {
    free_buf(in_tpl_buf);
    free_buf(out_tpl_1_buf);
    return 1;
  }

  int ret = prepare_ncc_template(
    in_tpl_buf, p->tpl_width, p->tpl_height,
    out_tpl_1_buf, out_tpl_2_buf
  );
  free_buf(in_tpl_buf);
  free_buf(out_tpl_1_buf);
  free_buf(out_tpl_2_buf);
  return ret;
}

int libprepared_mtncc(
  unsigned char *src, int32_t width, int32_t height,
  prepared_ncc_template_t *p,
  unsigned char *out
) {
  halide_buffer_t *in_src_buf = create_rgba_buffer(src, width, height);
  if(in_src_buf == NULL){
    return 1;
  }
  halide_buffer_t *in_tpl_1_buf = create_float_array_buffer(p->tuple_1, p->tpl_width, p->tpl_height);
  if(in_tpl_1_buf == NULL) {
    free_buf(in_src_buf);
    return 1;
  }
  halide_buffer_t *in_tpl_2_buf = create_float_array_buffer(p->tuple_2, p->tpl_width, p->tpl_height);
  if(in_tpl_2_buf == NULL) {
    free_buf(in_src_buf);
    free_buf(in_tpl_1_buf);
    return 1;
  }
  halide_buffer_t *out_buf = create_double_array_buffer(out, width, height);
  if(out_buf == NULL){
    free_buf(in_src_buf);
    free_buf(in_tpl_1_buf);
    free_buf(in_tpl_2_buf);
    return 1;
  }

  int ret = prepared_match_template_ncc(
    in_src_buf, width, height,
    in_tpl_1_buf, in_tpl_2_buf, p->tpl_width, p->tpl_height,
    out_buf
  );
  free_buf(in_src_buf);
  free_buf(in_tpl_1_buf);
  free_buf(in_tpl_2_buf);
  free_buf(out_buf);
  return ret;
}
//
// }}} NCC
//

//
// {{{ ZNCC
//
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

prepared_zncc_template_t *create_prepare_zncc_template(int32_t tpl_width, int32_t tpl_height) {
  prepared_zncc_template_t *p = (prepared_zncc_template_t *) malloc(sizeof(prepared_zncc_template_t));
  if(p == NULL) {
    return NULL;
  }

  unsigned char *tuple_1 = (unsigned char *) malloc(tpl_width * tpl_height * sizeof(float));
  if(tuple_1 == NULL) {
    free(p);
    return NULL;
  }
  unsigned char *tuple_2 = (unsigned char *) malloc(tpl_width * tpl_height * sizeof(float));
  if(tuple_2 == NULL) {
    free(p);
    free(tuple_1);
    return NULL;
  }

  p->tpl_width = tpl_width;
  p->tpl_height = tpl_height;
  p->tuple_1 = tuple_1;
  p->tuple_2 = tuple_2;
  return p;
}

void free_prepare_zncc_template(prepared_zncc_template_t *p) {
  if(p != NULL) {
    free(p->tuple_1);
    free(p->tuple_2);
  }
  free(p);
}

int libprepare_zncc_tpl(unsigned char *tpl, prepared_zncc_template_t *p){
  halide_buffer_t *in_tpl_buf = create_rgba_buffer(tpl, p->tpl_width, p->tpl_height);
  if(in_tpl_buf == NULL){
    return 1;
  }
  halide_buffer_t *out_tpl_1_buf = create_float_array_buffer(p->tuple_1, p->tpl_width, p->tpl_height);
  if(out_tpl_1_buf == NULL) {
    free_buf(in_tpl_buf);
    return 1;
  }
  halide_buffer_t *out_tpl_2_buf = create_float_array_buffer(p->tuple_2, p->tpl_width, p->tpl_height);
  if(out_tpl_2_buf == NULL) {
    free_buf(in_tpl_buf);
    free_buf(out_tpl_1_buf);
    return 1;
  }

  int ret = prepare_zncc_template(
    in_tpl_buf, p->tpl_width, p->tpl_height,
    out_tpl_1_buf, out_tpl_2_buf
  );
  free_buf(in_tpl_buf);
  free_buf(out_tpl_1_buf);
  free_buf(out_tpl_2_buf);
  return ret;
}

int libprepared_mtzncc(
  unsigned char *src, int32_t width, int32_t height,
  prepared_zncc_template_t *p,
  unsigned char *out
) {
  halide_buffer_t *in_src_buf = create_rgba_buffer(src, width, height);
  if(in_src_buf == NULL){
    return 1;
  }
  halide_buffer_t *in_tpl_1_buf = create_float_array_buffer(p->tuple_1, p->tpl_width, p->tpl_height);
  if(in_tpl_1_buf == NULL) {
    free_buf(in_src_buf);
    return 1;
  }
  halide_buffer_t *in_tpl_2_buf = create_float_array_buffer(p->tuple_2, p->tpl_width, p->tpl_height);
  if(in_tpl_2_buf == NULL) {
    free_buf(in_src_buf);
    free_buf(in_tpl_1_buf);
    return 1;
  }
  halide_buffer_t *out_buf = create_double_array_buffer(out, width, height);
  if(out_buf == NULL){
    free_buf(in_src_buf);
    free_buf(in_tpl_1_buf);
    free_buf(in_tpl_2_buf);
    return 1;
  }

  int ret = prepared_match_template_zncc(
    in_src_buf, width, height,
    in_tpl_1_buf, in_tpl_2_buf, p->tpl_width, p->tpl_height,
    out_buf
  );
  free_buf(in_src_buf);
  free_buf(in_tpl_1_buf);
  free_buf(in_tpl_2_buf);
  free_buf(out_buf);
  return ret;
}
//
// }}} ZNCC
//
