#include <ctype.h>
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef int errno_t;

errno_t read_file(const char *path, char **buf_out, size_t *len_out) {
    FILE *file = fopen(path, "r");
    if (!file) return errno;

    // get file size
    if (fseeko(file, 0, SEEK_END) == -1) return errno;
    off_t len = ftello(file);
    if (len == -1) return errno;

    rewind(file);

    char *buf = malloc(len);
    if (!buf) return errno;

    // read file contents into buf
    off_t nread = fread(buf, 1, len, file);
    if (nread != len || fclose(file) == EOF) {
        errno_t code = errno;
        free(buf);
        return code;
    }

    *buf_out = buf;
    *len_out = len;

    return 0;
}

typedef struct {
    const char *src;
    size_t ptr, len;
} lexer_t;

typedef enum {
    TOK_WS = 256,
    TOK_COMMENT,
    TOK_EOF,

    TOK_INT,
    TOK_STR,
    TOK_IDENT,
    TOK_SYMBOL,

    TOK_KW_INT,
    TOK_KW_DEF,
    TOK_KW_LET,

    TOK_NONE,
} token_kind_t;

typedef struct {
    token_kind_t kind;

    size_t start, end;
} token_t;

const char *tok_to_str[] = {[TOK_WS] = "WS",
                            [TOK_COMMENT] = "COMMENT",
                            [TOK_EOF] = "EOF",
                            [TOK_INT] = "INT",
                            [TOK_STR] = "STR",
                            [TOK_IDENT] = "IDENT",
                            [TOK_SYMBOL] = "SYMBOL",
                            [TOK_KW_INT] = "KW_INT",
                            [TOK_KW_DEF] = "KW_DEF",
                            [TOK_KW_LET] = "KW_LET",
                            [TOK_NONE] = "NONE"};

void lexer_init(lexer_t *lx, const char *src) {
    lx->src = src;
    lx->ptr = 0;
    lx->len = strlen(lx->src);
}

typedef bool (*ccond_t)(char);

static char peek(lexer_t *lx) {
    if (lx->ptr >= lx->len) return '\0';
    return lx->src[lx->ptr];
}

static bool take(lexer_t *lx) {
    if (lx->ptr >= lx->len) return '\0';
    return lx->src[lx->ptr++];
}

static bool take_if(lexer_t *lx, ccond_t cond) {
    char c = peek(lx);
    return cond(c) && take(lx);
}

static bool take_str(lexer_t *lx, const char *str) {
    size_t len = strlen(str);

    if (strncmp(lx->src + lx->ptr, str, len) == 0) {
        lx->ptr += len;
        return true;
    }

    return false;
}

static bool take_while(lexer_t *lx, ccond_t cond) {
    bool taken = false;
    while (take_if(lx, cond)) taken = true;
    return taken;
}

static bool kw_cmp(const char *src, size_t len, const char *kw) {
    return len == strlen(kw) && strncmp(src, kw, len) == 0;
}

static bool take_if_eq(lexer_t *lx, char exp) {
    char c = peek(lx);
    return c == exp && take(lx);
}

/*
lexer:
    lexer_peek
    lexer_next
        next_ident
        next_int
        next_ws
        next_punct
        next_comment
    lexer_peek_expect      |
    lexer_next_expect      |> takes fn pointer, failure string

parser:
    type assertions:
        $(int int)
    swizzling:
        ( a b c -> b b c )
    type checking:
        tracks function start/end

ir:
    swizzle:
        ( a b c -> b b c )
        becomes
        b_idx b_idx c_idx n_swiz n_pop SWIZZLE
        1 1 2 3 3 SWIZZLE
    functions:
        functions point to a control flow graph
        graph of unconditional jumps

        // map to "block" (linked list of instructions) of IR instructions
        // blocks point to previous and next siblings (call destinations)

*/

/*
 * lexer rules
 */

static bool is_ws(char c) {
    return isspace(c);
}

static bool is_not_nl(char c) {
    return c != '\n';
}

static bool is_ident_start(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

static bool is_digit(char c) {
    return c >= '0' && c <= '9';
}

static bool is_ident_body(char c) {
    return is_ident_start(c) || is_digit(c);
}

/*
 * producers
 */

static bool produce_ident(lexer_t *lx, token_t *tok) {
    tok->start = lx->ptr;

    if (!take_if(lx, is_ident_start)) return false;

    take_while(lx, is_ident_body);

    tok->kind = TOK_IDENT;
    tok->end = lx->ptr;

    size_t len = tok->end - tok->start;
    const char *src = lx->src + tok->start;

    if (kw_cmp(src, len, "let"))
        tok->kind = TOK_KW_LET;
    else if (kw_cmp(src, len, "int"))
        tok->kind = TOK_KW_INT;
    else if (kw_cmp(src, len, "def"))
        tok->kind = TOK_KW_DEF;

    return true;
}

static bool produce_symbol(lexer_t *lx, token_t *tok) {
    tok->start = lx->ptr;

    if (!take_if_eq(lx, '#')) return false;

    take_while(lx, is_ident_body);

    tok->kind = TOK_SYMBOL;
    tok->end = lx->ptr;

    size_t len = tok->end - tok->start;
    const char *src = lx->src + tok->start;

    if (kw_cmp(src, len, "let"))
        tok->kind = TOK_KW_LET;
    else if (kw_cmp(src, len, "int"))
        tok->kind = TOK_KW_INT;
    else if (kw_cmp(src, len, "def"))
        tok->kind = TOK_KW_DEF;

    return true;
}

static bool produce_while(lexer_t *lx, token_t *tok, ccond_t cond, token_kind_t kind) {
    tok->kind = kind;
    tok->start = lx->ptr;

    bool res = take_while(lx, cond);

    tok->end = lx->ptr;

    return res;
}

bool lexer_next(lexer_t *lx, token_t *tok) {
    tok->start = lx->ptr;
    tok->end = 0;
    tok->kind = TOK_NONE;

    char c = peek(lx);

    if (!c) {
        tok->kind = TOK_EOF;
        tok->start = lx->ptr;
        tok->end = lx->ptr + 1;
        return true;
    }

    while (take_while(lx, is_ws) || take_str(lx, "#!"))
        if (take_str(lx, "#!")) take_while(lx, is_not_nl);

    if (produce_ident(lx, tok))
        printf("lexer: ident\n");
    else if (produce_symbol(lx, tok))
        printf("lexer: symbol\n");
    else if (produce_while(lx, tok, is_digit, TOK_INT))
        printf("lexer: int\n");
    else
        return false;

    return true;
}

int main(void) {
    const char *src = "     #! foo bar baz\nint let def foobar 123 #abc";
    lexer_t lx;
    lexer_init(&lx, src);

    token_t tok;
    while (lexer_next(&lx, &tok) && tok.kind != TOK_EOF)
        printf("tok: %s : %d\n", tok_to_str[tok.kind], (int)lx.src[tok.start]);

    size_t len = 3;
    char *s = "def";

    if (kw_cmp(s, len, "let"))
        printf("let\n");
    else if (kw_cmp(s, len, "int"))
        printf("int\n");
    else if (kw_cmp(s, len, "def"))
        printf("def");

    return 0;
}
