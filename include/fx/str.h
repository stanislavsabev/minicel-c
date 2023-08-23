#ifndef FX_FXSTR_H_
#define FX_FXSTR_H_

#include <stdbool.h>
#include <stdlib.h>

/**
 * @brief struct with len and char data[]
 */
typedef struct fxstr_s {
    size_t len;
    char data[];
} fxstr_t;

/**
 * @brief struct with len and char data[64]
 */
typedef struct fxstr_buf64_s {
    size_t len;
    char data[64];
} fxstr_buf64_t;

/**
 * @brief struct with len and char data[256]
 */
typedef struct fxstr_buf256_s {
    size_t len;
    char data[256];
} fxstr_buf256_t;

// ------------------------------------------------------------------------------------------
//  IMPLEMENTATION
// ------------------------------------------------------------------------------------------
#ifdef FX_IMPLEMENTATION

#ifndef FX_NO_SHORT_NAMES

#define DEFINE_TRIVIAL_CLEANUP_FUNC FX_DEFINE_TRIVIAL_CLEANUP_FUNC
#define rept_literal                fx_rept_literal
#define yes_no                      fx_yes_no
#define true_false                  fx_true_false
#define one_zero                    fx_one_zero
#define str                         fxstr_t
#define str_buf64                   fxstr_buf64_t
#define str_buf256                  fxstr_buf256_t

#else

#endif   // FX_NO_SHORT_NAMES

#define FX_DEFINE_TRIVIAL_CLEANUP_FUNC(type, func) \
    static inline void func##p(type* p) {          \
        if (*p) func(*p);                          \
    }                                              \
    struct __useless_struct_to_allow_trailing_semicolon__

#define fx_rept_literal0(s)
#define fx_rept_literal1(s)  s
#define fx_rept_literal2(s)  fx_rept_literal1(s) s
#define fx_rept_literal3(s)  fx_rept_literal2(s) s
#define fx_rept_literal4(s)  fx_rept_literal3(s) s
#define fx_rept_literal5(s)  fx_rept_literal4(s) s
#define fx_rept_literal6(s)  fx_rept_literal5(s) s
#define fx_rept_literal7(s)  fx_rept_literal6(s) s
#define fx_rept_literal8(s)  fx_rept_literal7(s) s
#define fx_rept_literal9(s)  fx_rept_literal8(s) s
#define fx_rept_literal10(s) fx_rept_literal9(s) s

/**
 * @brief Repeat string literal
 * @param hundreds number from 0 to 10
 * @param tens number from 0 to 10
 * @param ones number from 0 to 10
 * @param s string to repeat
 * @returns repeated string literal
 */
#define fx_rept_literal(hundreds, tens, ones, s)                       \
    fx_rept_literal##hundreds(fx_rept_literal10(fx_rept_literal10(s))) \
        fx_rept_literal##tens(fx_rept_literal10(s)) fx_rept_literal##ones(s)

static inline const char* fx_yes_no(bool b) {
    return b ? "yes" : "no";
}

static inline const char* fx_true_false(bool b) {
    return b ? "true" : "false";
}

static inline const char* fx_one_zero(bool b) {
    return b ? "1" : "0";
}

#endif   // FX_IMPLEMENTATION
#endif   // FX_FXSTR_H_
