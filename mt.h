int libmtsad(
  unsigned char *src, int32_t width, int32_t height,
  unsigned char *tpl, int32_t tpl_width, int32_t tpl_height,
  unsigned char *out
);

int libmtssd(
  unsigned char *src, int32_t width, int32_t height,
  unsigned char *tpl, int32_t tpl_width, int32_t tpl_height,
  unsigned char *out
);

//
// {{{ NCC
//
int libmtncc(
  unsigned char *src, int32_t width, int32_t height,
  unsigned char *tpl, int32_t tpl_width, int32_t tpl_height,
  unsigned char *out
);

typedef struct prepared_ncc_template_t {
  int32_t tpl_width;
  int32_t tpl_height;
  unsigned char *tuple_1;  // float *t_val
  unsigned char *tuple_2;  // float *t_sum
} prepared_ncc_template_t;

prepared_ncc_template_t *create_prepare_ncc_template(
  int32_t tpl_width, int32_t tpl_height
);

void free_prepare_ncc_template(prepared_ncc_template_t *p);

int libprepare_ncc_tpl(unsigned char *tpl, prepared_ncc_template_t *p);

int libprepared_mtncc(
  unsigned char *src, int32_t width, int32_t height,
  prepared_ncc_template_t *p,
  unsigned char *out
);
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
);

typedef struct prepared_zncc_template_t {
  int32_t tpl_width;
  int32_t tpl_height;
  unsigned char *tuple_1;  // float *t_val
  unsigned char *tuple_2;  // float *t_stddev
} prepared_zncc_template_t;

prepared_zncc_template_t *create_prepare_zncc_template(
  int32_t tpl_width, int32_t tpl_height
);

void free_prepare_zncc_template(prepared_zncc_template_t *p);

int libprepare_zncc_tpl(unsigned char *tpl, prepared_zncc_template_t *p);

int libprepared_mtzncc(
  unsigned char *src, int32_t width, int32_t height,
  prepared_zncc_template_t *p,
  unsigned char *out
);
//
// }}} ZNCC
//
