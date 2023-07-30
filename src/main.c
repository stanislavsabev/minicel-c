#include <stdio.h>

#include "darray.h"

typedef struct Address {
    size_t row;
    size_t col;
} Address;

typedef enum ExprState {
    EXPR_STATE_NONE,
    EXPR_STATE_IN_PROGRESS,
    EXPR_STATE_EVALUATED,
} ExprState;

typedef struct {
    char* ast;
    size_t expr_state_ndx;   // index from the expr_states array
} Expr;

typedef enum ValueType {
    VALUE_TYPE_EMPTY,
    VALUE_TYPE_NUMBER,
    VALUE_TYPE_TEXT,
    VALUE_TYPE_EXPR,
} ValueType;

typedef union ValueAs {
    char* str;
    float f;
    size_t expr_ndx;   // index from the expressions array
} ValueAs;

typedef struct {
    size_t row;
    size_t col;
    ValueAs value_as;
    ValueType value_type;
} Cell;

typedef struct Table {
    Cell* cells;
} Table;

int read_table_cells(Table* tbl, Expr* exprs);
void print_cell(Cell* cell, Expr* exprs);

int main(int argc, char const* argv[]) {
    (void)argc;
    (void)argv;

    darr_type(ExprState) expr_states = NULL;
    darr_type(Expr) exprs = NULL;
    Table tbl = {0};

    read_table_cells(&tbl, exprs);

    for (size_t i = 0; i < darr_len(tbl.cells); i++) {
        Cell c = tbl.cells[i];
        print_cell(&c, exprs);
    }

    darr_free(tbl.cells);
    darr_free(exprs);
    darr_free(expr_states);
    printf("Done!\n");
    return 0;
}

/// @brief TODO: Finish implementation
/// @param tbl
/// @param expressions
/// @return
int read_table_cells(Table* tbl, Expr* exprs) {
    const size_t row_count = 4;
    const size_t col_count = 2;

    darr_type(Cell) cells = NULL;
    darr_reserve(cells, row_count * col_count);

    for (size_t row = 0; row < row_count; row++) {
        for (size_t col = 0; col < col_count; col++) {
            if (row == 0) {
                // Header cells
                char header = 'A' + col;
                cells[row * row + col] =
                    (Cell){.row = row,
                           .col = col,
                           .value_as.str = &header,
                           .value_type = VALUE_TYPE_TEXT};

            } else if (!(row < row_count - 1)) {
                // Number cells
                float number =
                    (float)((row + 5 - row % 3) + col * row - col % 2);
                cells[row * row + col] =
                    (Cell){.row = row,
                           .col = col,
                           .value_as.f = number,
                           .value_type = VALUE_TYPE_NUMBER};
            } else {
                // Expr cells
                // TODO: Create cell AST here
                Expr expr = {0};
                darr_push_back(exprs, expr);
                size_t expr_ndx = darr_len(exprs) - 1;

                char* expr_str = "";
                cells[row * row + col] =
                    (Cell){.row = row,
                           .col = col,
                           .value_as.expr_ndx = expr_ndx,
                           .value_type = VALUE_TYPE_EXPR};
            }
        }
    }

    return 0;
}

/// @brief TODO: Test implementation
/// @param cell
void print_cell(Cell* cell, Expr* exprs) {
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
            Expr exp = exprs[cell->value_as.expr_ndx];
            val_str = exp.ast;
            val_typ_str = "EXPR";
        } break;
        default:
            break;
    }

    printf("CELL(%lu, %lu) >> %s(%s)", cell->col, cell->row, val_typ_str,
           val_str);
    return;
}