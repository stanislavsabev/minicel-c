#if !defined(FX_FXSTRING_H_)
#define FX_FXSTRING_H_

#include <stdlib.h>

///
#define FXSTR_REPT0(X)
#define FXSTR_REPT1(X) X
#define FXSTR_REPT2(X) FXSTR_REPT1(X) X
#define FXSTR_REPT3(X) FXSTR_REPT2(X) X
#define FXSTR_REPT4(X) FXSTR_REPT3(X) X
#define FXSTR_REPT5(X) FXSTR_REPT4(X) X
#define FXSTR_REPT6(X) FXSTR_REPT5(X) X
#define FXSTR_REPT7(X) FXSTR_REPT6(X) X
#define FXSTR_REPT8(X) FXSTR_REPT7(X) X
#define FXSTR_REPT9(X) FXSTR_REPT8(X) X
#define FXSTR_REPT10(X) FXSTR_REPT9(X) X

#define FXSTR_REPT(HUNDREDS, TENS, ONES, X)                                               \
    FXSTR_REPT##HUNDREDS(FXSTR_REPT10(FXSTR_REPT10(X))) FXSTR_REPT##TENS(FXSTR_REPT10(X)) \
        FXSTR_REPT##ONES(X)

/**
 * @brief Fx string - struct with <string> and <len>.
 */
typedef struct FxStr {
    size_t len;
    char* s;
} FxStr;

/**
 * @brief Trims string <s> and writes the result onto <trimmed>.
 * @param s string to trim
 * @param trimmed buffer with at least len size, to hold the result
 * @returns number of characters trimmed
 */
int fxstr_trim(FxStr* s, FxStr* trimmed);

#endif   // FX_FXSTRING_H_
