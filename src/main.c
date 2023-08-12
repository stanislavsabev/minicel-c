#include <stdbool.h>
#include <stdio.h>

#include "darray.h"

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
    EvalState state;   // index from the expr_states array
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


void print_horizontal_line(size_t cell_width, size_t ncols){
    size_t repetitions = cell_width * ncols + ncols + 1;
    for (size_t i = 0; i < repetitions; i++)
    {
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

    darr_type(Cell) cells = NULL;
    darr_type(Expr) exprs = NULL;
    darr_reserve(cells, row_count * col_count);

    for (size_t row = 0; row < row_count; row++) {
        for (size_t col = 0; col < col_count; col++) {
            Cell cell = {.row = row, .col = col};
            char ch = 'A' + col;

            if (row == 0) {
                // Header cells
                char* header = malloc(sizeof(char) * 2);
                snprintf(header, sizeof(header), "%c", ch);
                cell.value_type = VALUE_TYPE_TEXT;
                cell.value_as.str = header;
            } else if (row < row_count - 1) {
                // Number cells
                float number = (float)(row * 2 + col);
                cell.value_as.number = number;
                cell.value_type = VALUE_TYPE_NUMBER;
            } else {
                // Expr cells
                char* expr_str = malloc(sizeof(char) * 8);
                snprintf(expr_str, sizeof(expr_str), "=%c2+%c3", ch, ch);
                // fprintf(stdout, "%s\n",
                //         expr_str);   // Print the formatted string
                Expr expr = {.str = expr_str, .state = EVAL_STATE_NOT_STARTED, .value = 0};
                size_t expr_ndx = darr_len(exprs);

                cell.value_as.expr_ndx = expr_ndx;
                cell.value_type = VALUE_TYPE_EXPR;
                // fprintf(stdout, "expr_ndx: %lu\n", expr_ndx);
                darr_append(exprs, expr);
            }
            darr_append(cells, cell);
        }
    }

    tbl->cells = cells;
    tbl->exprs = exprs;
    // size_t ln_cells = darr_len(cells);
    // size_t ln_exprs = darr_len(exprs);
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
    char* buff = malloc(16 * sizeof(char));
    snprintf(buff, sizeof(buff), "CELL(%lu, %lu)", cell->row, cell->col);
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
            } else if (r > 0 && (cell.value_type == VALUE_TYPE_TEXT ||
                                 cell.value_type == VALUE_TYPE_EXPR)) {
                int rpadding = cell_width - ln - 1;
                fprintf(stdout, "%s%*s", cell_str, rpadding, "");
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

    // size_t ln_cells = darr_len(tbl.cells);
    // size_t ln_exprs = darr_len(tbl.exprs);
    // fprintf(stdout, "cells at: %p, ln: %lu\n", tbl.cells, ln_cells);
    // fprintf(stdout, "exprs at: %p, ln: %lu\n", tbl.exprs, ln_exprs);

    print_table(&tbl);

    darr_free(tbl.cells);
    darr_free(tbl.exprs);
    return 0;
}
