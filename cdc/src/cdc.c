#include <ctype.h>
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef int errno_t;

errno_t read_file(const char* path, char** buf_out, size_t* len_out) {
	FILE* file = fopen(path, "r");
	if (!file) {
		return errno;
	}

	// get file size
	if (fseeko(file, 0, SEEK_END) == -1) {
		return errno;
	}
	off_t len = ftello(file);
	if (len == -1) {
		return errno;
	}

	rewind(file);

	char* buf = malloc(len);
	if (!buf) {
		return errno;
	}

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
	const char* src;
	size_t ptr, len;
} lexer_t;

#define TOKENS \
	X(WS) \
	X(COMMENT) \
	X(EOF) \
	X(INT) \
	X(STR) \
	X(IDENT) \
	X(SYMBOL) \
	X(KW_INT) \
	X(KW_DEF) \
	X(KW_LET) \
	X(ARROW) \
	X(ADD) \
	X(SUB) \
	X(MUL) \
	X(DIV) \
	X(LPAREN) \
	X(RPAREN) \
	X(COND) \
	X(NONE)

typedef enum {
#define X(name) TOK_##name,
	TOKENS
#undef X
} token_kind_t;

typedef struct {
	token_kind_t kind;
	size_t start, end;
} token_t;

// TODO: X-macro
const char* tok_to_str[] = {
#define X(name) [TOK_##name] = #name,
	TOKENS
#undef x
};

typedef bool (*ccond_t)(char);

// peek at next char in stream
static char peek(lexer_t* lx) {
	if (lx->ptr >= lx->len) {
		return '\0';
	}
	return lx->src[lx->ptr];
}

// consume next char in stream
static bool take(lexer_t* lx) {
	if (lx->ptr >= lx->len) {
		return '\0';
	}
	return lx->src[lx->ptr++];
}

// consume next char in stream if matches cond
static bool take_if(lexer_t* lx, ccond_t cond) {
	char c = peek(lx);
	return cond(c) && take(lx);
}

// consume next chars in stream if it matches str
static bool take_str(lexer_t* lx, const char* str) {
	size_t len = strlen(str);

	if (strncmp(lx->src + lx->ptr, str, len) == 0) {
		lx->ptr += len;
		return true;
	}

	return false;
}

// consume chars from stream while cond holds true
static bool take_while(lexer_t* lx, ccond_t cond) {
	bool taken = false;
	while (take_if(lx, cond)) {
		taken = true;
	}
	return taken;
}

// compare 2 strings, ensuring lengths match
static bool kw_cmp(const char* src, size_t len, const char* kw) {
	return len == strlen(kw) && strncmp(src, kw, len) == 0;
}

// consume next character if it is equal to exp
static bool take_if_eq(lexer_t* lx, char exp) {
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

				// map to "block" (linked list of instructions) of IR
instructions
				// blocks point to previous and next siblings (call
destinations)

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

static bool produce_ident(lexer_t* lx, token_t* tok) {
	tok->start = lx->ptr;

	if (!take_if(lx, is_ident_start)) {
		return false;
	}

	take_while(lx, is_ident_body);

	tok->kind = TOK_IDENT;
	tok->end = lx->ptr;

	size_t len = tok->end - tok->start;
	const char* src = lx->src + tok->start;

	if (kw_cmp(src, len, "let")) {
		tok->kind = TOK_KW_LET;
	}
	else if (kw_cmp(src, len, "int")) {
		tok->kind = TOK_KW_INT;
	}
	else if (kw_cmp(src, len, "def")) {
		tok->kind = TOK_KW_DEF;
	}

	return true;
}

static bool produce_symbol(lexer_t* lx, token_t* tok) {
	tok->start = lx->ptr;

	if (!take_if_eq(lx, '#')) {
		return false;
	}

	take_while(lx, is_ident_body);

	tok->kind = TOK_SYMBOL;
	tok->end = lx->ptr;

	size_t len = tok->end - tok->start;
	const char* src = lx->src + tok->start;

	if (kw_cmp(src, len, "let")) {
		tok->kind = TOK_KW_LET;
	}
	else if (kw_cmp(src, len, "int")) {
		tok->kind = TOK_KW_INT;
	}
	else if (kw_cmp(src, len, "def")) {
		tok->kind = TOK_KW_DEF;
	}

	return true;
}

static bool produce_while(lexer_t* lx, token_t* tok, ccond_t cond, token_kind_t kind) {
	tok->kind = kind;
	tok->start = lx->ptr;

	bool res = take_while(lx, cond);

	tok->end = lx->ptr;

	return res;
}

static bool produce_int(lexer_t* lx, token_t* tok) {
	return produce_while(lx, tok, is_digit, TOK_INT);
}

static bool produce_sigil(lexer_t* lx, token_t* tok) {
	tok->start = lx->ptr;

	if (take_str(lx, "->")) {
		tok->kind = TOK_ARROW;
	}
	else if (take_str(lx, "+")) {
		tok->kind = TOK_ADD;
	}
	else if (take_str(lx, "-")) {
		tok->kind = TOK_SUB;
	}
	else if (take_str(lx, "*")) {
		tok->kind = TOK_MUL;
	}
	else if (take_str(lx, "/")) {
		tok->kind = TOK_DIV;
	}
	else if (take_str(lx, "(")) {
		tok->kind = TOK_LPAREN;
	}
	else if (take_str(lx, ")")) {
		tok->kind = TOK_RPAREN;
	}
	else if (take_str(lx, "?")) {
		tok->kind = TOK_COND;
	}

	tok->end = lx->ptr;

	return true;
}

lexer_t lexer_create(const char* src, size_t len) {
	lexer_t lx;

	lx.src = src;
	lx.ptr = 0;
	lx.len = len;

	return lx;
}

bool lexer_next(lexer_t* lx, token_t* tok) {
	tok->start = lx->ptr;
	tok->end = 0;
	tok->kind = TOK_NONE;

	char c = peek(lx);

	// if eof, return EOF token early
	if (!c) {
		tok->kind = TOK_EOF;
		tok->start = lx->ptr;
		tok->end = lx->ptr + 1;
		return true;
	}

	// skip whitespace and comments
	while (take_while(lx, is_ws) || take_str(lx, "#!")) {
		if (take_str(lx, "#!")) {
			take_while(lx, is_not_nl);
		}
	}

	// produce tokens
	if (produce_ident(lx, tok)) {
		printf("lexer: ident\n");
	}
	else if (produce_symbol(lx, tok)) {
		printf("lexer: symbol\n");
	}
	else if (produce_int(lx, tok)) {
		printf("lexer: int\n");
	}
	else if (produce_sigil(lx, tok)) {
		printf("lexer: sigil\n");
	}
	else {
		return false;
	}

	return true;
}

int main(void) {
	const char* src = "-> + - * / ( )";
	size_t len = strlen(src);
	lexer_t lx = lexer_create(src, len);

	token_t tok;
	while (lexer_next(&lx, &tok) && tok.kind != TOK_EOF) {
		printf("tok: %s : %d\n", tok_to_str[tok.kind], (int)lx.src[tok.start]);
	}

	return 0;
}
