#include <stdio.h>

#include "darray.h"

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
    char* as_str;
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
    float f;
    size_t expr_ndx;   // index from the expressions array
} Value;

typedef struct {
    size_t row;
    size_t col;
    Value value_as;
    ValueType value_type;
} Cell;

typedef struct Table {
    Cell* cells;
} Table;

/// @brief TODO: Finish implementation
/// @param tbl - table with cells
/// @param exprs - array with expressions
/// @return
int read_table_cells(Table* tbl, Expr* exprs) {
    const size_t row_count = 4;
    const size_t col_count = 2;

    darr_type(Cell) cells = NULL;
    darr_reserve(cells, row_count * col_count);
    tbl->cells = cells;

    for (size_t row = 0; row < row_count; row++) {
        for (size_t col = 0; col < col_count; col++) {
            Cell cell = {.row = row, .col = col};

            if (row == 0) {
                // Header cells
                char header = 'A' + col;
                cell.value_as.str = &header;
                cell.value_type = VALUE_TYPE_TEXT;

            } else if (!(row < row_count - 1)) {
                // Number cells
                float number =
                    (float)((row + 5 - row % 3) + col * row - col % 2);
                cell.value_as.f = number;
                cell.value_type = VALUE_TYPE_NUMBER;
            } else {
                // Expr cells
                char expr_str[7];
                char ch = 'A' + col;
                snprintf(expr_str, sizeof(expr_str), "=%c2+%c3", ch, ch);
                fprintf(stdout, "%s\n", expr_str); // Print the formatted string
                Expr expr = {.as_str = expr_str,
                             .state = EVAL_STATE_NOT_STARTED,
                             .value = 0};
                size_t expr_ndx = darr_len(exprs);

                cell.value_as.expr_ndx = expr_ndx;
                cell.value_type = VALUE_TYPE_EXPR;
                darr_append(cells, cell);
                darr_append(exprs, expr);
                size_t ln_cells = darr_len(cells);
                size_t ln_exprs = darr_len(exprs);
                fprintf(stdout, "cells ln: %lu, exprs ln: %lu\n", ln_cells, ln_exprs);
            }
        }
    }

    return 0;
}

/// @brief TODO: Test implementation
/// @param cell table cell
/// @param exprs array with expressions
void print_cell(Cell cell[static 1], Expr exprs[static 1]) {
    ValueType vt = cell->value_type;
    char* val_str = NULL;
    char* val_typ_str = NULL;

    switch (vt) {
        case VALUE_TYPE_EMPTY:
            val_str = "";
            val_typ_str = "*EMPTY*";
            break;
        case VALUE_TYPE_NUMBER: {
            char buf[64];
            snprintf(buf, sizeof(buf), "%.2f", cell->value_as.f);
            val_str = buf;
            val_typ_str = "NUMBER";
        } break;
        case VALUE_TYPE_TEXT:
            val_str = cell->value_as.str;
            val_typ_str = "TEXT";
            break;
        case VALUE_TYPE_EXPR: {
            Expr* exp = &exprs[cell->value_as.expr_ndx];
            val_str = exp->as_str;
            val_typ_str = "EXPR";
        } break;
        default:
            break;
    }

    printf("CELL(%lu, %lu) >> %s(%s)", cell->col, cell->row, val_typ_str,
           val_str);
    return;
}

int main(int argc, char const* argv[]) {
    (void)argc;
    (void)argv;

    darr_type(Expr) exprs = NULL;
    Table tbl = {0};

    read_table_cells(&tbl, exprs);
    size_t ln = darr_len(tbl.cells);

    for (size_t i = 0; i < ln; i++) {
        Cell c = tbl.cells[i];
        print_cell(&c, exprs);
    }

    darr_free(tbl.cells);
    darr_free(exprs);
    printf("Done!\n");
    return 0;
}
