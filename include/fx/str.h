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
typedef struct fxstr_veiw_s {
    size_t len;
    const char* data;
} str_view_t;

/**
 * @brief struct with len and char data[64]
 */
typedef struct fxstr_buf_s {
    size_t capacity;
    size_t len;
    char* data;
} str_buf_t;

/**
 * @brief struct with len and char data[64]
 */
typedef struct fxstr_buf64_s {
    size_t len;
    char data[64];
} str_buf64_t;

/**
 * @brief struct with len and char data[256]
 */
typedef struct fxstr_buf256_s {
    size_t len;
    char data[256];
} str_buf256_t;

#ifndef FX_NO_SHORT_NAMES

#define strv_null                       fxstr_view_null
#define str_len(str_ptr_)               fxstr_len(str_ptr_)
#define str_is_empty(str_ptr_)          fxstr_is_empty(str_ptr_)
#define strv_len(str_ptr_)              fxstr_view_len(str_ptr_)
#define strv_is_empty(str_ptr_)         fxstr_view_is_empty(str_ptr_)
#define strv_from_chars(len_, chars_)   fxstr_view_from_chars(len_, chars_)
#define strv_from_cstr(cstr)            fxstr_view_from_cstr(cstr)
#define strv_to_cstr(str_ptr_)          fxstr_view_to_cstr(str_ptr_)
#define strb_null                       fxstr_buf_null
#define strb_len(str_ptr_)              fxstr_buf_len(str_ptr_)
#define strb_free(str_ptr_)             fxstr_buf_free(str_ptr_)
#define strb_from_chars(len_, chars_)   fxstr_buf_from_chars(len_, chars_)
#define strb_from_cstr(cstr)            fxstr_buf_from_cstr(cstr)
#define strb_to_cstr(str_ptr_)          fxstr_buf_to_cstr(str_ptr_)
#define memmem(hay, hlen, needle, nlen) fx_memmem(hay, hlen, needle, nlen)

#endif   // FX_NO_SHORT_NAMES

str_view_t fxstr_view_null(void);
str_buf_t fxstr_buf_null(void);

static inline size_t fxstr_view_len(const str_view_t* str_ptr_) {
    return str_ptr_->len;
}

static inline size_t fxstr_buf_len(const str_buf_t* str_ptr_) {
    return str_ptr_->len;
}

#define fxstr_len(str_ptr_) \
    _Generic((str_ptr_), const str_view_t*: fxstr_view_len, const str_buf_t*: fxstr_buf_len)( \
        str_ptr_)

#define fxstr_is_empty(str_ptr_) \
    _Generic((str_ptr_),                        \
        const str_view_t*: fxstr_view_is_empty, \
        const str_buf_t*: fxstr_buf_is_empty)(str_ptr_)

static inline bool fxstr_view_is_empty(const str_view_t* str_ptr_) {
    return fxstr_len(str_ptr_) == 0;
}

static inline bool fxstr_buf_is_empty(const str_buf_t* str_ptr_) {
    return fxstr_len(str_ptr_) == 0;
}

// str_view_t
str_view_t fxstr_view_from_chars(size_t len_, const char chars_[static len_]);
str_view_t fxstr_view_from_cstr(const char cstr[static 1]);
char* fxstr_view_to_cstr(const str_view_t* fxstr_);

// str_buf_t
str_buf_t fxstr_buf_from_chars(size_t len_, const char chars_[static len_]);
str_buf_t fxstr_buf_from_cstr(const char cstr[static 1]);
char* fxstr_buf_to_cstr(const str_buf_t* str_ptr_);
void fxstr_buf_free(str_buf_t* str_ptr_);

// utils
void* fx_memmem(const void* hay, size_t hlen, const void* needle, size_t nlen);

#endif   // FX_FXSTR_H_
