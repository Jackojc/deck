#ifndef CDC_LEXER_H
#define CDC_LEXER_H

#include <ctype.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>

#include "def.h"
#include "util.h"
#include "log.h"

// Tokens/Instructions
#define INSTRUCTIONS \
	X(DK_NONE, "none") \
	X(DK_ENDFILE, "endfile") \
\
	X(DK_WHITESPACE, "whitespace") \
	X(DK_COMMENT, "comment") \
\
	/* Atoms */ \
	X(DK_NUMBER, "number") \
	X(DK_STRING, "string") \
	X(DK_IDENT, "ident") \
	X(DK_SYMBOL, "symbol") \
\
	/* Types */ \
	X(DK_TYPE_NUMBER, "number type") \
	X(DK_TYPE_STRING, "string type") \
	X(DK_TYPE_ANY, "any type") \
	X(DK_TYPE_FN, "function type") \
\
	/* Keywords */ \
	X(DK_DEFINE, "define") \
	X(DK_LET, "let") \
\
	/* Operators */ \
	X(DK_OR, "or") \
	X(DK_AND, "and") \
	X(DK_NOT, "not") \
\
	X(DK_ADD, "add") \
	X(DK_SUB, "sub") \
	X(DK_MUL, "mul") \
	X(DK_DIV, "div") \
\
	X(DK_COND, "cond") \
	X(DK_APPLY, "apply") \
	X(DK_EQUAL, "equal") \
\
	X(DK_ARROW, "arrow") \
	X(DK_TYPE, "type") \
\
	/* Grouping */ \
	X(DK_LPAREN, "lparen") \
	X(DK_RPAREN, "rparen") \
\
	X(DK_LBRACKET, "lbracket") \
	X(DK_RBRACKET, "rbracket")

#define X(x, y) x,
typedef enum {
	INSTRUCTIONS
} dk_instr_kind_t;
#undef X

#define X(x, y) [x] = #x,
const char* DK_INSTR_KIND_TO_STR[] = {INSTRUCTIONS};
#undef X

#define X(x, y) [x] = y,
const char* DK_INSTR_TO_STR[] = {INSTRUCTIONS};
#undef X

#undef INSTRUCTIONS

// Token/Instruction
typedef bool (*dk_lexer_pred_t)(char);  // Used for lexer predicates

typedef struct dk_instr_t dk_instr_t;

struct dk_instr_t {
	dk_instr_kind_t kind;

	dk_instr_t* next;
	dk_instr_t* prev;

	union {
		struct {
			const char* ptr;
			const char* end;
		} str;

		size_t num;
	};
};

static const char* dk_instr_str(dk_instr_t instr) {
	return instr.str.ptr;
}

static size_t dk_instr_strlen(dk_instr_t instr) {
	return dk_ptrdiff(instr.str.ptr, instr.str.end);
}

static dk_instr_t
dk_instr_create(dk_instr_kind_t kind, const char* ptr, const char* end) {
	return (dk_instr_t){
		.kind = kind,

		.next = NULL,
		.prev = NULL,

		.str.ptr = ptr,
		.str.end = end,
	};
}

static dk_instr_t DK_INSTR_NONE = (dk_instr_t){
	.kind = DK_NONE,

	.next = NULL,
	.prev = NULL,

	.str.ptr = NULL,
	.str.end = NULL,
};

// Lexer
typedef struct {
	const char* const src;

	const char* ptr;
	const char* end;

	dk_instr_t peek;
} dk_lexer_t;

static bool dk_lexer_take(dk_logger_t* log, dk_lexer_t* lx, dk_instr_t* instr);

static dk_lexer_t
dk_lexer_create(dk_logger_t* log, const char* ptr, const char* end) {
	dk_lexer_t lx = (dk_lexer_t){
		.src = ptr,
		.ptr = ptr,
		.end = end,
		.peek = dk_instr_create(DK_NONE, ptr, ptr),
	};

	dk_lexer_take(log, &lx, NULL);

	return lx;
}

// Debugging/printing
// TODO: Make these functions returned a formatted buffer rather than
// calling printf within.
// TODO: Print position info for tokens?
// static void dk_instr_print(dk_lexer_t* lx, dk_instr_t instr) {
// 	printf(
// 		"kind = %s, ptr = %p, end = %p, size = %lu\n",
// 		DK_INSTR_TO_STR[instr.kind],
// 		(void*) instr.str.ptr,
// 		(void*) instr.str.end,
// 		dk_ptrdiff(instr.str.ptr, instr.str.end));
// }

// static void dk_lexer_print(dk_lexer_t* lx, dk_instr_t instr) {
// 	// TODO: Print lexer state in some readable fashion for debugging.
// 	// Some ideas:
// 	// - Print all tokens and then reset lexer state (set `ptr` to `src`)
// 	// - Print pointers, peek token
// 	// - Total number of tokens
// 	// - Line count
// 	// - Percentage of file that has been lexed
// }

// Basic stream interaction
static char dk_peek(dk_lexer_t* lx) {
	if (lx->ptr >= lx->end) {
		return '\0';
	}

	return *lx->ptr;
}

static char dk_take(dk_lexer_t* lx) {
	if (lx->ptr >= lx->end) {
		return '\0';
	}

	return *lx->ptr++;
}

// Conditional consumers
static bool dk_take_if(dk_lexer_t* lx, dk_lexer_pred_t cond) {
	char c = dk_peek(lx);

	if (!c || !cond(c)) {
		return false;
	}

	dk_take(lx);
	return true;
}

// Same as take_if but just takes a character directly
// for common usecases.
static bool dk_take_ifc(dk_lexer_t* lx, char c) {
	if (c != dk_peek(lx)) {
		return false;
	}

	dk_take(lx);
	return true;
}

static bool dk_take_str(dk_lexer_t* lx, const char* str) {
	size_t length = strlen(str);

	if (lx->ptr + length >= lx->end) {
		return false;
	}

	// TODO: Use custom strncmp to avoid iterating strings twice.
	if (strncmp(lx->ptr, str, length) != 0) {
		return false;
	}

	lx->ptr += length;
	return true;
}

static bool dk_take_while(dk_lexer_t* lx, dk_lexer_pred_t cond) {
	bool taken = false;

	while (dk_take_if(lx, cond)) {
		taken = true;
	}

	return taken;
}

// Lexer predicates
static bool dk_is_whitespace(char c) {
	return isspace(c);
}

static bool dk_is_comment(char c) {
	return c != '\n';
}

static bool dk_is_alpha(char c) {
	return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

static bool dk_is_digit(char c) {
	return c >= '0' && c <= '9';
}

static bool dk_is_alphanum(char c) {
	return dk_is_alpha(c) || dk_is_digit(c);
}

static bool dk_is_ident(char c) {
	return dk_is_alphanum(c) || c == '_';
}

// Token producers
static bool dk_produce_if(
	dk_logger_t* log,
	dk_lexer_t* lx,
	dk_instr_t* instr,
	dk_instr_kind_t kind,
	dk_lexer_pred_t cond) {
	dk_instr_t next_instr = dk_instr_create(DK_NONE, lx->ptr, lx->ptr);

	if (!dk_take_if(lx, cond)) {
		return false;
	}

	next_instr.str.end = lx->ptr;
	next_instr.kind = kind;

	if (instr != NULL) {
		*instr = next_instr;
	}

	return true;
}

static bool dk_produce_while(
	dk_logger_t* log,
	dk_lexer_t* lx,
	dk_instr_t* instr,
	dk_instr_kind_t kind,
	dk_lexer_pred_t cond) {
	dk_instr_t next_instr = dk_instr_create(DK_NONE, lx->ptr, lx->ptr);

	if (!dk_take_while(lx, cond)) {
		return false;
	}

	next_instr.str.end = lx->ptr;
	next_instr.kind = kind;

	if (instr != NULL) {
		*instr = next_instr;
	}

	return true;
}

static bool dk_produce_str(
	dk_logger_t* log, dk_lexer_t* lx, dk_instr_t* instr, dk_instr_kind_t kind,
	const char* str) {
	dk_instr_t next_instr = dk_instr_create(DK_NONE, lx->ptr, lx->ptr);

	if (!dk_take_str(lx, str)) {
		return false;
	}

	next_instr.str.end = lx->ptr;
	next_instr.kind = kind;

	if (instr != NULL) {
		*instr = next_instr;
	}

	return true;
}

static bool
dk_produce_ident(dk_logger_t* log, dk_lexer_t* lx, dk_instr_t* instr) {
	dk_instr_t next_instr = dk_instr_create(DK_NONE, lx->ptr, lx->ptr);

	if (!dk_take_if(lx, dk_is_alpha)) {
		return false;
	}

	dk_take_while(lx, dk_is_ident);

	next_instr.str.end = lx->ptr;

	// We could have used `dk_produce_str` here to handle these cases
	// but we want "maximal munch" meaning that we lex the entire
	// identifier before trying to classify it. Why? because if we
	// didn't, an identifier like "letfoo" would actually be lexed
	// as 2 seperate tokens because it sees `let` and stops there.

	// Keywords
	if (dk_strncmp(next_instr.str.ptr, next_instr.str.end, "let")) {
		next_instr.kind = DK_LET;
	}

	else if (dk_strncmp(next_instr.str.ptr, next_instr.str.end, "def")) {
		next_instr.kind = DK_DEFINE;
	}

	// Operators
	else if (dk_strncmp(next_instr.str.ptr, next_instr.str.end, "or")) {
		next_instr.kind = DK_OR;
	}

	else if (dk_strncmp(next_instr.str.ptr, next_instr.str.end, "and")) {
		next_instr.kind = DK_AND;
	}

	else if (dk_strncmp(next_instr.str.ptr, next_instr.str.end, "not")) {
		next_instr.kind = DK_NOT;
	}

	// Types
	else if (dk_strncmp(next_instr.str.ptr, next_instr.str.end, "int")) {
		next_instr.kind = DK_TYPE_NUMBER;
	}

	else if (dk_strncmp(next_instr.str.ptr, next_instr.str.end, "string")) {
		next_instr.kind = DK_TYPE_STRING;
	}

	else if (dk_strncmp(next_instr.str.ptr, next_instr.str.end, "any")) {
		next_instr.kind = DK_TYPE_ANY;
	}

	// User identifier
	else {
		next_instr.kind = DK_IDENT;
	}

	if (instr != NULL) {
		*instr = next_instr;
	}

	return true;
}

static bool
dk_produce_symbol(dk_logger_t* log, dk_lexer_t* lx, dk_instr_t* instr) {
	dk_instr_t next_instr = dk_instr_create(DK_NONE, lx->ptr, lx->ptr);

	if (!dk_take_ifc(lx, '#')) {
		return false;
	}

	dk_take_while(lx, dk_is_ident);

	next_instr.str.end = lx->ptr;
	next_instr.kind = DK_SYMBOL;

	if (instr != NULL) {
		*instr = next_instr;
	}

	return true;
}

static bool
dk_produce_number(dk_logger_t* log, dk_lexer_t* lx, dk_instr_t* instr) {
	return dk_produce_while(log, lx, instr, DK_NUMBER, dk_is_digit);
}

static bool
dk_produce_sigil(dk_logger_t* log, dk_lexer_t* lx, dk_instr_t* instr) {
#define DK_PRODUCE_SIGIL(s, k) dk_produce_str(log, lx, instr, s, k)
	// clang-format off

	return DK_PRODUCE_SIGIL(DK_ARROW,    "->") ||
	       DK_PRODUCE_SIGIL(DK_ADD,      "+")  ||
		   DK_PRODUCE_SIGIL(DK_SUB,      "-")  ||
		   DK_PRODUCE_SIGIL(DK_MUL,      "*")  ||
		   DK_PRODUCE_SIGIL(DK_DIV,      "/")  ||
		   DK_PRODUCE_SIGIL(DK_APPLY,    ".")  ||
		   DK_PRODUCE_SIGIL(DK_EQUAL,    "=")  ||
		   DK_PRODUCE_SIGIL(DK_LPAREN,   "(")  ||
		   DK_PRODUCE_SIGIL(DK_RPAREN,   ")")  ||
		   DK_PRODUCE_SIGIL(DK_LBRACKET, "[")  ||
		   DK_PRODUCE_SIGIL(DK_RBRACKET, "]")  ||
		   DK_PRODUCE_SIGIL(DK_COND,     "?")  ||
		   DK_PRODUCE_SIGIL(DK_TYPE,     "$");

	// clang-format on
#undef DK_PRODUCE_SIGIL
}

static bool
dk_produce_whitespace(dk_logger_t* log, dk_lexer_t* lx, dk_instr_t* instr) {
	return dk_produce_while(log, lx, instr, DK_WHITESPACE, dk_is_whitespace);
}

static bool
dk_produce_comment(dk_logger_t* log, dk_lexer_t* lx, dk_instr_t* instr) {
	dk_instr_t next_instr = dk_instr_create(DK_NONE, lx->ptr, lx->ptr);

	if (!dk_take_str(lx, "#!")) {
		return false;
	}

	if (!dk_take_while(lx, dk_is_comment)) {
		return false;
	}

	next_instr.str.end = lx->ptr;
	next_instr.kind = DK_COMMENT;

	if (instr != NULL) {
		*instr = next_instr;
	}

	return true;
}

// Core lexer interface
// TODO: Reconsider implementation. Is it safe to always return true?
static bool dk_lexer_peek(dk_logger_t* log, dk_lexer_t* lx, dk_instr_t* instr) {
	if (instr != NULL) {
		*instr = lx->peek;
	}

	return true;
}

// TODO: Make lexer_next produce all tokens and then wrap it in
// another function which skips whitespace and comments.
static bool dk_lexer_take(dk_logger_t* log, dk_lexer_t* lx, dk_instr_t* instr) {
	dk_instr_t next_instr = dk_instr_create(DK_NONE, lx->ptr, lx->ptr);

	while (dk_produce_whitespace(log, lx, NULL) ||
		   dk_produce_comment(log, lx, NULL))
	{}

	// Handle EOF
	if (lx->ptr >= lx->end) {
		next_instr.kind = DK_ENDFILE;
	}

	// Handle normal tokens
	else if (!(dk_produce_ident(log, lx, &next_instr) ||
			   dk_produce_symbol(log, lx, &next_instr) ||
			   dk_produce_number(log, lx, &next_instr) ||
			   dk_produce_sigil(log, lx, &next_instr)))
	{
		dk_log(log, DK_ERROR, "unknown character");
		return false;
	}

	// Return previously peeked token and then store newly
	// lexed token to be used on the next call to peek.
	if (instr != NULL) {
		dk_lexer_peek(log, lx, instr);
	}

	lx->peek = next_instr;

	return true;
}

#endif
