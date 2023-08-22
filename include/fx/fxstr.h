#if !defined(FX_FXSTR_H_)
#define FX_FXSTR_H_

#include <stdbool.h>
#include <stdlib.h>

///
#define fxstr_rept_lit0(s)
#define fxstr_rept_lit1(s) s
#define fxstr_rept_lit2(s) fxstr_rept_lit1(s) s
#define fxstr_rept_lit3(s) fxstr_rept_lit2(s) s
#define fxstr_rept_lit4(s) fxstr_rept_lit3(s) s
#define fxstr_rept_lit5(s) fxstr_rept_lit4(s) s
#define fxstr_rept_lit6(s) fxstr_rept_lit5(s) s
#define fxstr_rept_lit7(s) fxstr_rept_lit6(s) s
#define fxstr_rept_lit8(s) fxstr_rept_lit7(s) s
#define fxstr_rept_lit9(s) fxstr_rept_lit8(s) s
#define fxstr_rept_lit10(s) fxstr_rept_lit9(s) s

/**
 * @brief Repeat string literal
 * @param hundreds number from 0 to 10
 * @param tens number from 0 to 10
 * @param ones number from 0 to 10
 * @param s string to repeat
 * @returns repeated string literal
 */
#define fxstr_rept_lit(hundreds, tens, ones, s)                             \
    fxstr_rept_lit##hundreds(fxstr_rept_lit10(fxstr_rept_lit10(s))) \
        fxstr_rept_lit##tens(fxstr_rept_lit10(s)) fxstr_rept_lit##ones(s)

#define DEFINE_TRIVIAL_CLEANUP_FUNC(type, func)             \
    static inline void func##p(type *p) {                   \
            if (*p)                                         \
                    func(*p);                               \
    }                                                       \
    struct __useless_struct_to_allow_trailing_semicolon__

/**
 * @brief fxstr - struct with <string> and <len>.
 */
typedef struct {
    size_t len;
    char* data;
} fxstr;

/**
 * @brief Trims string <s> and writes the result onto <trimmed>.
 * @param s string to trim
 * @returns trimmed fxstr
 */
fxstr fxstr_trim(fxstr* s); 

static inline const char* yes_no(bool b) {
        return b ? "yes" : "no";
}

static inline const char* true_false(bool b) {
        return b ? "true" : "false";
}

static inline const char* one_zero(bool b) {
        return b ? "1" : "0";
}

#endif   // FX_FXSTR_H_
