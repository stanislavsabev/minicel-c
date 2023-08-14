#include <stdbool.h>
#include <stdio.h>

#include "fx/fxarray.h"

#define CELL_PRINT_WIDTH 20
#define WITH_TYPE false

typedef struct Address {
    size_t row;
    size_t col;
} Address;

typedef enum EvalState {
    EVAL_STATE_NOT_STARTED,
    EVAL_STATE_IN_PROGRESS,
    EVAL_STATE_EVALUATED,
} EvalState;

typedef struct Expr {
    char* str;
    float value;
    EvalState eval_state;   // index from the expr_states array
} Expr;

typedef enum ValueType {
    VALUE_TYPE_EMPTY,
    VALUE_TYPE_NUMBER,
    VALUE_TYPE_TEXT,
    VALUE_TYPE_EXPR,
    VALUE_TYPE_ERR,
    VALUE_TYPE_COUNT,
} ValueType;

typedef union Value {
    char* str;
    float number;
    size_t expr_ndx;   // index from the expressions array
} Value;

typedef struct {
    size_t row;
    size_t col;
    Value value_as;
    ValueType value_type;
} Cell;

typedef struct Table {
    size_t nrows;
    size_t ncols;
    Cell* cells;
    Expr* exprs;
} Table;

void print_horizontal_line(size_t cell_width, size_t ncols) {
    size_t repetitions = cell_width * ncols + ncols + 1;
    for (size_t i = 0; i < repetitions; i++) {
        fprintf(stdout, "-");
    }
    fprintf(stdout, "\n");
}

/// @brief TODO: Finish implementation
/// @param tbl - table with cells
/// @param exprs - array with expressions
/// @return
int read_table(Table* tbl) {
    size_t row_count = tbl->nrows = 4;
    size_t col_count = tbl->ncols = 3;

    fxarr_type(Cell) cells = NULL;
    fxarr_type(Expr) exprs = NULL;
    fxarr_reserve(cells, row_count * col_count);

    for (size_t row = 0; row < row_count; row++) {
        for (size_t col = 0; col < col_count; col++) {
            Cell cell = {.row = row, .col = col};
            char ch = 'A' + col;

            if (row == 0) {
                // Header cells
                size_t len = 4;
                size_t sz = len * sizeof(char);
                char* header = malloc(sz);
                snprintf(header, sz, "%c", ch);

#if defined(DEBUG)
                fprintf(stdout, "\nsz: %lu vs sizeof(header): %lu\n", sz, sizeof(header));
#endif // DEBUG

                cell.value_type = VALUE_TYPE_TEXT;
                cell.value_as.str = header;
            } else if (row < row_count - 1) {
                // Number cells
                float number = (float)(row * 2 + col);
                cell.value_as.number = number;
                cell.value_type = VALUE_TYPE_NUMBER;
            } else {
                // Expr cells
                size_t len = 64;
                size_t sz = len * sizeof(char);
                char* expr_str = malloc(sz);
                snprintf(expr_str, sz, "=%c2+%c3", ch, ch);
                // fprintf(stdout, "%s\n",
                //         expr_str);   // Print the formatted string
                Expr expr = {.str = expr_str, .eval_state = EVAL_STATE_NOT_STARTED, .value = 0};
                size_t expr_ndx = fxarr_len(exprs);

                cell.value_as.expr_ndx = expr_ndx;
                cell.value_type = VALUE_TYPE_EXPR;
                // fprintf(stdout, "expr_ndx: %lu\n", expr_ndx);
                fxarr_append(exprs, expr);
            }
            fxarr_append(cells, cell);
        }
    }

    tbl->cells = cells;
    tbl->exprs = exprs;
    // size_t ln_cells = fxarr_len(cells);
    // size_t ln_exprs = fxarr_len(exprs);
    // fprintf(stdout, "cells at: %p, ln: %lu\n", cells, ln_cells);
    // fprintf(stdout, "exprs at: %p, ln: %lu\n", exprs, ln_exprs);
    return 0;
}

/// @param cell table cell
/// @param exprs array with expressions
/// @returns char* with cell's type and contents
int cell_strview(const Cell* cell, const Expr* exprs, const bool with_type, char* buff,
                 size_t buff_len) {
    ValueType vt = cell->value_type;
    switch (vt) {
        case VALUE_TYPE_EMPTY:
            snprintf(buff, buff_len, with_type ? "*EMPTY*" : "");
            break;
        case VALUE_TYPE_NUMBER: {
            snprintf(buff, buff_len, with_type ? "NUMBER(%.2f)" : "%.2f", cell->value_as.number);
        } break;
        case VALUE_TYPE_TEXT:
            snprintf(buff, buff_len, with_type ? "TEXT(%s)" : "%s", cell->value_as.str);
            break;
        case VALUE_TYPE_EXPR: {
            Expr exp = exprs[cell->value_as.expr_ndx];
            snprintf(buff, buff_len, with_type ? "EXPR(%s)" : "%s", exp.str);
        } break;
        default:
            snprintf(buff, buff_len, with_type ? "!#UNKN" : "!#UNKN");
            return 1;
    }

    return 0;
}

/// @param cell table cell
/// @returns char* with cell's address
char* cell_rc_strview(const Cell* cell) {
    size_t sz = 64 * sizeof(char);
    char* buff = malloc(sz);
    snprintf(buff, sz, "CELL(%lu, %lu)", cell->row, cell->col);
    fprintf(stdout, "\nsz: %lu vs sizeof(buff): %lu\n", sz, sizeof(buff));
    return buff;
}

void print_table(Table* tbl) {
    const bool with_type = WITH_TYPE;
    const size_t cell_width = CELL_PRINT_WIDTH / (with_type ? 1 : 2);

    print_horizontal_line(cell_width, tbl->ncols);
    for (size_t r = 0; r < tbl->nrows; r++) {
        fprintf(stdout, "|");
        for (size_t c = 0; c < tbl->ncols; c++) {
            size_t rc = r * tbl->ncols + c;
            Cell cell = tbl->cells[rc];
            char cell_str[CELL_PRINT_WIDTH];
            cell_strview(&cell, tbl->exprs, with_type, cell_str, sizeof(cell_str));
            size_t ln = strlen(cell_str);

            // print centered text in the header
            if ((r == 0 && cell.value_type == VALUE_TYPE_TEXT) ||
                cell.value_type == VALUE_TYPE_ERR) {
                int padding = (cell_width - ln) / 2;
                fprintf(stdout, "%*s%s%*s", padding, "", cell_str, padding, "");
            } else if (r > 0 && cell.value_type == VALUE_TYPE_TEXT) {
                int rpadding = cell_width - ln - 1;
                fprintf(stdout, "%s%*s", cell_str, rpadding, "");
            } else if (r > 0 && cell.value_type == VALUE_TYPE_EXPR) {
                int rpadding = cell_width - ln - 1;
                fprintf(stdout, "%s%*s", cell_str, rpadding, "");
                Expr exp = tbl->exprs[cell.value_as.expr_ndx];
                if(exp.eval_state == EVAL_STATE_EVALUATED){
                    fprintf(stdout, " (%.2f)", exp.value);
                }
            } else if (cell.value_type == VALUE_TYPE_NUMBER) {
                int lpadding = cell_width - ln - 1;
                fprintf(stdout, "%*s%s", lpadding, "", cell_str);
            }
            fprintf(stdout, " |");
        }
        fprintf(stdout, "\n");
        print_horizontal_line(cell_width, tbl->ncols);
    }
}

int main(int argc, char const* argv[]) {
    (void)argc;
    (void)argv;

    Table tbl = {0};
    read_table(&tbl);

    // size_t ln_cells = fxarr_len(tbl.cells);
    // size_t ln_exprs = fxarr_len(tbl.exprs);
    // fprintf(stdout, "cells at: %p, ln: %lu\n", tbl.cells, ln_cells);
    // fprintf(stdout, "exprs at: %p, ln: %lu\n", tbl.exprs, ln_exprs);

    print_table(&tbl);

    fxarr_free(tbl.cells);
    fxarr_free(tbl.exprs);
    return 0;
}
