
#ifndef FX_FXUTIL_H_
#define FX_FXUTIL_H_

#include <stdbool.h>

#ifndef FX_NO_SHORT_NAMES

#define DEFINE_TRIVIAL_CLEANUP_FUNC           FX_DEFINE_TRIVIAL_CLEANUP_FUNC
#define yes_no(b)                             fx_yes_no(b)
#define true_false(b)                         fx_true_false(b)
#define one_zero(b)                           fx_one_zero(b)
#define rept_literal(hundreds, tens, ones, s) fx_rept_literal(hundreds, tens, ones, s)

#endif   // FX_NO_SHORT_NAMES

///@brief Gets the address pointing to the container which accommodates the respective member
///@param ptr a pointer to container (struct)
///@param type of the container
///@param the name of the member the pointer refers to
#define container_of(ptr, type, member)                   \
    ({                                                    \
        const typeof(((type*)0)->member)* __mptr = (ptr); \
        (type*)((char*)__mptr - offsetof(type, member));  \
    })

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

#ifdef DEBUG
#define dprintf(__format, ...)                  \
    do {                                        \
        fprintf(stderr, __format, __VA_ARGS__); \
    } while (false)

#define dfprintf(__stream, __format, ...)         \
    do {                                          \
        fprintf(__stream, __format, __VA_ARGS__); \
    } while (false)
#else
#define dprintf(...) \
    do {             \
    } while (false)

#define dfprintf(...) \
    do {              \
    } while (false)
#endif

static inline const char* fx_yes_no(bool b) {
    return b ? "yes" : "no";
}

static inline const char* fx_true_false(bool b) {
    return b ? "true" : "false";
}

static inline const char* fx_one_zero(bool b) {
    return b ? "1" : "0";
}

#endif   // FX_FXUTIL_H_