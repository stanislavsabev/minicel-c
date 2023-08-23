#include <ctype.h>
#include <locale.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FX_IMPLEMENTATION
#include "fx/arr.h"
#include "fx/str.h"
#include "fx/util.h"

#define CELL_PRINT_WIDTH 20
#define WITH_TYPE        false
#define OPERATORS        "-+"
#define MAX_ROWS         1024
#define MAX_COLUMNS      702   // ZZ

typedef struct {
    size_t row;
    size_t col;
} AddressRC;

typedef enum {
    EVAL_STATE_NOT_STARTED,
    EVAL_STATE_IN_PROGRESS,
    EVAL_STATE_EVALUATED,
} EvalState;

typedef struct Expr {
    char* str;
    double value;
    EvalState eval_state;   // index from the expr_states array
} Expr;

typedef enum {
    VALUE_TYPE_EMPTY,
    VALUE_TYPE_NUMBER,
    VALUE_TYPE_TEXT,
    VALUE_TYPE_EXPR,
    VALUE_TYPE_ERR,
    VALUE_TYPE_ENUM_COUNT,
} ValueType;

typedef union {
    char* str;
    char* err_str;
    double number;
    size_t expr_ndx;   // index from the expressions array
} Value;

typedef struct {
    size_t row;
    size_t col;
    Value value_as;
    ValueType value_type;
} Cell;

typedef struct {
    size_t nrows;
    size_t ncols;
    Cell* cells;
    Expr* exprs;
} Table;

Table tbl = {0};

void print_horizontal_line(size_t cell_width, size_t ncols) {
    size_t repetitions = cell_width * ncols + ncols + 1;
    for (size_t i = 0; i < repetitions; i++) {
        fprintf(stdout, "-");
    }
    fprintf(stdout, "\n");
}

Cell create_cell(size_t row, size_t col, size_t row_count, Table* tbl) {
    Cell cell = {.row = row, .col = col};
    char ch = 'A' + col;

    if (row == 0) {
        // Header cells
        size_t len = 4;
        size_t sz = len * sizeof(char);
        char* header = malloc(sz);
        snprintf(header, sz, "%c", ch);

        dprintf("\nsz: %lu vs sizeof(header): %lu\n", sz, sizeof(header));

        cell.value_type = VALUE_TYPE_TEXT;
        cell.value_as.str = header;
    } else if (row < row_count - 1) {
        // Number cells
        double number = (double)(row * 2 + col);
        cell.value_as.number = number;
        cell.value_type = VALUE_TYPE_NUMBER;
    } else {
        // Expr cells
        size_t len = 64;
        size_t sz = len * sizeof(char);
        char* expr_str = malloc(sz);
        if (col == 0) {
            snprintf(expr_str, sz, "=%d.5+B%c355", (int)(row + 2 + col), ch);

        } else if (col == 1) {
            snprintf(expr_str, sz, "=%d", (int)(row + 2 + col));
        } else {
            snprintf(expr_str, sz, "=-%d", (int)(row + 2 + col));
        }
        Expr expr = {.str = expr_str, .eval_state = EVAL_STATE_NOT_STARTED, .value = 0};
        size_t expr_ndx = fxarr_len(tbl->exprs);

        cell.value_as.expr_ndx = expr_ndx;
        cell.value_type = VALUE_TYPE_EXPR;

        fxarr_append(tbl->exprs, expr);
        dfprintf(stdout, "exprs at: %p, ln: %lu\n", tbl->exprs, fxarr_len(tbl->exprs));
    }
    return cell;
}

/// @brief TODO: Finish implementation
/// @param tbl - table with cells
/// @param exprs - array with expressions
/// @return
int read_table(Table* tbl) {
    size_t row_count = tbl->nrows = 4;
    size_t col_count = tbl->ncols = 3;

    fxarr_type(Cell) cells = NULL;
    // fxarr_reserve(cells, row_count * col_count);

    fxarr_type(Expr) exprs = NULL;
    // fxarr_reserve(exprs, 3);
    tbl->cells = cells;
    tbl->exprs = exprs;

    for (size_t row = 0; row < row_count; row++) {
        for (size_t col = 0; col < col_count; col++) {
            Cell cell = create_cell(row, col, row_count, tbl);
            fxarr_append(tbl->cells, cell);
        }
    }

    dfprintf(stderr, "exprs p == %p, tbl.exprs %p, same: %d\n", exprs, tbl->exprs,
             exprs == tbl->exprs);
    dfprintf(stderr, "cells at: %p, ln: %lu\n", tbl->cells, fxarr_len(tbl->cells));
    dfprintf(stderr, "exprs at: %p, ln: %lu\n", tbl->exprs, fxarr_len(tbl->exprs));
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
        case VALUE_TYPE_ERR:
            snprintf(buff, buff_len, with_type ? "ERR(%s)" : "%s", cell->value_as.err_str);
            break;
        case VALUE_TYPE_EXPR: {
            size_t index = cell->value_as.expr_ndx;
            Expr exp = exprs[index];
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
    dfprintf(stdout, "\nsz: %lu vs sizeof(buff): %lu\n", sz, sizeof(buff));
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
            char cell_str[CELL_PRINT_WIDTH + 1];
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
                if (exp.eval_state == EVAL_STATE_EVALUATED) {
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

typedef enum {
    AST_VALUE_TYPE_NUMBER,
    AST_VALUE_TYPE_ADDRESS_RC,
    AST_VALUE_TYPE_UNARY,
    AST_VALUE_TYPE_BINARY,
} AstValueType;

typedef union AstValue AstValue;
typedef struct Ast {
    AstValue* val_as;
    AstValueType val_type;
} Ast;

typedef struct {
    char operator;
    Ast rhs;
} UnaryAst;

typedef struct {
    char operator;
    Ast lhs;
    Ast rhs;
} BinaryAst;

typedef union AstValue {
    double number;
    AddressRC address_rc;
    UnaryAst unary;
    BinaryAst binary;
} AstValue;

typedef enum {
    TOKEN_TYPE_NUMBER,
    TOKEN_TYPE_OPERATOR,
    TOKEN_TYPE_CELL_ADDRESS,
    TOKEN_TYPE_INVALID,
    TOKEN_TYPE_EOF,
    TOKEN_TYPE_ENUM_COUNT,
} TokenType;

typedef struct {
    TokenType type;
    size_t len;
    const char* s;
} Token;

#define EOF_TOKEN                                   \
    (Token) {                                       \
        .type = TOKEN_TYPE_EOF, .s = NULL, .len = 0 \
    }

bool is_operator(const char* ch) {
    return strchr(OPERATORS, *ch) != NULL;
}

bool is_column_address(const char* s, size_t len) {
    return false;
}
bool is_row_address(const char* s, size_t len) {
    return false;
}

char get_decimal_point() {
    setlocale(LC_ALL, "");
    struct lconv* lc = localeconv();
    if (lc) {
        return *lc->decimal_point;
    }
    return '.';
}

Cell* table_cell_at(Table* tbl, AddressRC* addr) {
    if (addr->row > tbl->nrows || addr->col > tbl->ncols) {
        fprintf(stderr, "Cannot find Cell(%lu, %lu)", addr->row, addr->col);
        return NULL;
    }
    size_t index = addr->row * tbl->ncols + addr->col;
    return &tbl->cells[index];
}

Token number_token(const char* s, size_t len) {
    Token token = {0};
    size_t i = 0;

    while (isdigit(s[i]) && i < len) {
        ++i;
    }

    // Set the locale based on the user's environment
    char dec_point = get_decimal_point();
    if (i < len && s[i] == dec_point) {
        ++i;
        while (isdigit(s[i]) && i < len) {
            ++i;
        }
    }

    token.len = i;
    token.s = s;
    token.type = TOKEN_TYPE_NUMBER;
    return token;
}

Token operator_token(const char* s, size_t len) {
    return (Token){.s = s, .len = 1, .type = TOKEN_TYPE_OPERATOR};
}

Token cell_address_token(const char* s, size_t len) {
    Token token = {0};
    size_t i = 0;
    while (s[i] >= 'A' && s[i] <= 'Z' && i < len) {
        ++i;
    }
    if (!isdigit(s[i])) {
        fprintf(stderr, "Invalid cell address token. Expected row number at pos %lu", i);
        exit(EXIT_FAILURE);
    }
    while (isdigit(s[i])) {
        ++i;
    }
    token.len = i;
    token.s = s;
    token.type = TOKEN_TYPE_CELL_ADDRESS;
    return token;
}

Token get_next_token(const char** s, size_t* len) {
    // number
    size_t ln = *len;
    const char* sp = *s;

    Token token = {0};
    if (!(ln > 0)) {
        token = EOF_TOKEN;
    } else if (isdigit(*sp)) {
        token = number_token(sp, ln);
    } else if (*sp >= 'A' && *sp <= 'Z') {
        // Address
        token = cell_address_token(sp, ln);
    } else if (is_operator(sp)) {
        // Operator
        token = operator_token(sp, ln);
    } else {
        fprintf(stderr, "Unknown token. %.*s", (int)ln, sp);
        token = (Token){.type = TOKEN_TYPE_INVALID, .s = sp, .len = ln};
    }
    sp = sp + token.len;
    ln -= token.len;
    *s = sp;
    *len = ln;
    return token;
}

void consume(const char* what, const char** s, size_t* len) {
    const char* sp = *s;
    size_t i = 0;

    while (what[i] == sp[i]) {
        dfprintf(stderr, "consuming: %c\n", what[i]);
        ++i;
    }
    *s = sp + i;
    *len -= i;
}

Ast ast_default() {
    Ast ast = {0};
    ast.val_as = calloc(1, sizeof(AstValue));
    return ast;
}

AddressRC cell_address_rc(const char* s, size_t len) {
    size_t col = 0;
    size_t row = 0;
    size_t i = 0;

    while (s[i] >= 'A' && s[i] <= 'Z' && i < len) {
        col = col * 26 + (s[i] - 'A' + 1);
        ++i;
    }
    if (MAX_COLUMNS < col) {
        fprintf(stderr, "Invalid column address %.*s. Max column address is 'ZZ'", (int)i, s);
        exit(EXIT_FAILURE);
    }

    while (isdigit(s[i]) && i < len) {
        row = row * 10 + (s[i] - '0');
        ++i;
    }

    if (MAX_ROWS < row) {
        fprintf(stderr, "Invalid row address %.*s. Max row address is 'ZZ'", (int)i, s);
        exit(EXIT_FAILURE);
    }

    return (AddressRC){.row = row, .col = col};
}

char* TokenType_to_str(TokenType token_type) {
    switch (token_type) {
        case TOKEN_TYPE_NUMBER:
            return "TOKEN_TYPE_NUMBER";
            break;
        case TOKEN_TYPE_OPERATOR:
            return "TOKEN_TYPE_OPERATOR";
            break;
        case TOKEN_TYPE_CELL_ADDRESS:
            return "TOKEN_TYPE_CELL_ADDRESS";
            break;
        case TOKEN_TYPE_INVALID:
            return "TOKEN_TYPE_INVALID";
            break;
        case TOKEN_TYPE_EOF:
            return "TOKEN_TYPE_EOF";
            break;
        case TOKEN_TYPE_ENUM_COUNT:
            return "TOKEN_TYPE_ENUM_COUNT";
            break;
    }
    return "";
}

double str_tod(const char* s, size_t len) {
    // tmp str for conversions
    static fxarr_type(char) tmp_cstr = NULL;
    fxarr_reserve(tmp_cstr, len);
    strncpy(tmp_cstr, s, len);
    tmp_cstr[len] = '\0';
    return strtod(tmp_cstr, NULL);
}

Ast parse_expression(const char** expr, size_t* len) {
    dfprintf(stderr, "expr %.*s\n", (int)*len, *expr);

    Ast ast = ast_default();

    const char* s = *expr;
    size_t ln = *len;

    while (ln) {
        Token token = get_next_token(&s, &ln);
        ;

        dfprintf(stderr, "token: %.*s, type: %s\n", (int)token.len, token.s,
                 TokenType_to_str(token.type));

        switch (token.type) {
            // Parse Unary Expr
            case TOKEN_TYPE_OPERATOR: {
                char op = *token.s;
                Ast rhs = parse_expression(&s, &ln);
                UnaryAst u_ast = {.operator= op, .rhs = rhs};
                ast.val_type = AST_VALUE_TYPE_UNARY;
                ast.val_as->unary = u_ast;
                break;
            }
            case TOKEN_TYPE_NUMBER: {
                // Parse number
                double number = str_tod(token.s, token.len);
                Token lookahead = get_next_token(&s, &ln);
                dfprintf(stderr, "lookahead: %.*s, type: %s\n", (int)lookahead.len, lookahead.s,
                         TokenType_to_str(lookahead.type));

                // No next token - return the number
                if (lookahead.type == TOKEN_TYPE_EOF) {
                    ast.val_type = AST_VALUE_TYPE_NUMBER;
                    ast.val_as->number = number;
                    return ast;
                }

                // Parse Binary Expr
                if (!(lookahead.type == TOKEN_TYPE_OPERATOR)) {
                    fprintf(stderr, "Unknown token type %.*s. Expected operator.\n",
                            (int)lookahead.len, lookahead.s);
                }
                Ast lhs = ast_default();
                lhs.val_type = AST_VALUE_TYPE_NUMBER;
                lhs.val_as->number = number;
                Ast rhs = parse_expression(&s, &ln);
                BinaryAst b_ast = {.operator= * lookahead.s, .lhs = lhs, .rhs = rhs};
                ast.val_type = AST_VALUE_TYPE_BINARY;
                ast.val_as->binary = b_ast;
                break;
            }
            // Parse Address
            case TOKEN_TYPE_CELL_ADDRESS:
                AddressRC addr = cell_address_rc(token.s, token.len);
                ast.val_type = AST_VALUE_TYPE_ADDRESS_RC;
                ast.val_as->address_rc = addr;
                break;

            default:
                fprintf(stderr, "Unknown token type %.*s\n", (int)token.len, token.s);
                break;
        }
    }

    *len = ln;
    *expr = s;
    return ast;
}

double evaluate_ast(Ast* ast, Table* tbl);

void evaluate_expr_cell(Cell* c, Table* tbl) {
    Expr* expr = &tbl->exprs[c->value_as.expr_ndx];

    if (expr->eval_state == EVAL_STATE_EVALUATED) {
        return;
    }

    if (expr->eval_state == EVAL_STATE_IN_PROGRESS) {
        fprintf(stderr, "Circular reference %s\n", expr->str);
        c->value_as.err_str = "#CREF#";
        c->value_type = VALUE_TYPE_ERR;
        expr->value = 0;
        expr->eval_state = EVAL_STATE_EVALUATED;
        return;
    }

    expr->eval_state = EVAL_STATE_IN_PROGRESS;
    const char* s = expr->str;
    size_t len = strlen(s);
    consume("=", &s, &len);
    Ast ast = parse_expression(&s, &len);
    double result = evaluate_ast(&ast, tbl);
    expr->value = result;
    c->value_as.number = result;
    expr->eval_state = EVAL_STATE_EVALUATED;
}

double evaluate_ast(Ast* ast, Table* tbl) {
    double result = 0;
    switch (ast->val_type) {
        case AST_VALUE_TYPE_NUMBER: {
            result = ast->val_as->number;
            break;
        }
        case AST_VALUE_TYPE_UNARY: {
            int sign = 1;
            UnaryAst unary = ast->val_as->unary;
            if (unary.operator== '-') {
                sign = -1;
            }
            result = evaluate_ast(&unary.rhs, tbl);
            result *= sign;
            break;
        }
        case AST_VALUE_TYPE_BINARY: {
            double lhs_res = evaluate_ast(&ast->val_as->binary.lhs, tbl);
            double rhs_res = evaluate_ast(&ast->val_as->binary.rhs, tbl);
            result = lhs_res + rhs_res;
            break;
        }
        case AST_VALUE_TYPE_ADDRESS_RC: {
            AddressRC a = ast->val_as->address_rc;
            Cell* c = table_cell_at(tbl, &a);
            if (c == NULL) {
                return 0;   // cannot find cell (same as empty)
            }
            if (c->value_type == VALUE_TYPE_EXPR) {
                evaluate_expr_cell(c, tbl);
                if (c->value_type != VALUE_TYPE_ERR) {
                    result = c->value_as.number;
                } else {
                    result = 0;
                }
            } else if (c->value_type == VALUE_TYPE_NUMBER) {
                result = c->value_as.number;
            } else if (c->value_type == VALUE_TYPE_EMPTY) {
                result = 0;
            } else {
                fprintf(stderr, "Non numeric cell(%lu, %lu) cannot be evaluated", c->row, c->col);
            }
        }
        default:
            break;
    }
    return result;
}

void evaluate_expr_cells(Table* tbl) {
    Cell* end = fxarr_end(tbl->cells);
    for (Cell* c = fxarr_begin(tbl->cells); c < end; ++c) {
        if (c->value_type == VALUE_TYPE_EXPR) {
            evaluate_expr_cell(c, tbl);
        }
    }
}

int main(int argc, char const* argv[]) {
    (void)argc;
    (void)argv;

    read_table(&tbl);
    print_table(&tbl);

    evaluate_expr_cells(&tbl);

    dprintf("cells at: %p, ln: %lu\n", tbl.cells, fxarr_len(tbl.cells));
    dprintf("exprs at: %p, ln: %lu\n", tbl.exprs, fxarr_len(tbl.exprs));

    print_table(&tbl);

    fxarr_free(tbl.cells);
    fxarr_free(tbl.exprs);
    return EXIT_SUCCESS;
}
