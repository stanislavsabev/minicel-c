#ifndef FX_FXSTR_H_
#define FX_FXSTR_H_

#include <stdbool.h>
#include <stdlib.h>

// Memory functions
#ifndef fxstr_stdlib_free
#define fxstr_stdlib_free free /* free function */
#endif
#ifndef fxstr_stdlib_malloc
#define fxstr_stdlib_malloc malloc
#endif
#ifndef fxstr_stdlib_calloc
#define fxstr_stdlib_calloc calloc
#endif
#ifndef fxstr_stdlib_realloc
#define fxstr_stdlib_realloc realloc
#endif

/**
 * @brief struct with len and char data[]
 */
typedef struct str_veiw_s {
    size_t len;
    const char* data;
} str_view_t;

/**
 * @brief struct with len and char data[64]
 */
typedef struct str_buf_s {
    size_t capacity;
    size_t len;
    char* data;
} str_buf_t;

/**
 * @brief struct with len and char data[64]
 */
typedef struct str_buf64_s {
    size_t len;
    char data[64];
} str_buf64_t;

/**
 * @brief struct with len and char data[256]
 */
typedef struct str_buf256_s {
    size_t len;
    char data[256];
} str_buf256_t;

#ifndef FX_NO_SHORT_NAMES

#define strv_null                       fxstr_view_null
#define str_len(str_p)                  fxstr_len(str_p)
#define str_is_empty(str_p)             fxstr_is_empty(str_p)
#define strv_len(str_p)                 fxstr_view_len(str_p)
#define strv_is_empty(str_p)            fxstr_view_is_empty(str_p)
#define strv_from_chars(len, chars_)    fxstr_view_from_chars(len, chars_)
#define strv_from_cstr(cstr)            fxstr_view_from_cstr(cstr)
#define strv_to_cstr(str_p)             fxstr_view_to_cstr(str_p)
#define strb_null                       fxstr_buf_null
#define strb_len(str_p)                 fxstr_buf_len(str_p)
#define strb_free(str_p)                fxstr_buf_free(str_p)
#define strb_from_chars(len, chars_)    fxstr_buf_from_chars(len, chars_)
#define strb_from_cstr(cstr)            fxstr_buf_from_cstr(cstr)
#define strb_to_cstr(str_p)             fxstr_buf_to_cstr(str_p)

#endif   // FX_NO_SHORT_NAMES

str_view_t fxstr_view_null(void);
str_buf_t fxstr_buf_null(void);

static inline size_t fxstr_view_len(const str_view_t* str_p) {
    return str_p->len;
}

static inline size_t fxstr_buf_len(const str_buf_t* str_p) {
    return str_p->len;
}

#define fxstr_len(str_p) \
    _Generic((str_p), const str_view_t* : fxstr_view_len, const str_buf_t* : fxstr_buf_len)(str_p)

#define fxstr_is_empty(str_p) \
    _Generic((str_p),                        \
        const str_view_t*: fxstr_view_is_empty, \
        const str_buf_t*: fxstr_buf_is_empty)(str_p)

static inline bool fxstr_view_is_empty(const str_view_t* str_p) {
    return fxstr_len(str_p) == 0;
}

static inline bool fxstr_buf_is_empty(const str_buf_t* str_p) {
    return fxstr_len(str_p) == 0;
}

// str_view_t
str_view_t fxstr_view_from_chars(size_t len, const char chars_[static len]);
str_view_t fxstr_view_from_cstr(const char cstr[static 1]);
char* fxstr_view_to_cstr(const str_view_t* fxstr_);

// str_buf_t
str_buf_t fxstr_buf_from_chars(size_t len, const char chars_[static len]);
str_buf_t fxstr_buf_from_cstr(const char cstr[static 1]);
char* fxstr_buf_to_cstr(str_buf_t* str_p);
char* fxstr_buf_to_cstr_copy(const str_buf_t* str_p);
str_view_t fxstr_buf_to_str_view(const str_buf_t* str_p);
void fxstr_buf_free(str_buf_t* str_p);

// utils
void* fx_memmem(const void* hay, size_t hlen, const void* needle, size_t nlen);

#endif   // FX_FXSTR_H_
