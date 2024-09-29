#ifndef CDC_LEXER_H
#define CDC_LEXER_H

#include <ctype.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>

#include "def.h"
#include "util.h"

// Tokens/Instructions
#define INSTRUCTIONS \
	X(DK_NONE) \
	X(DK_ENDFILE) \
\
	X(DK_WHITESPACE) \
	X(DK_COMMENT) \
\
	/* Atoms */ \
	X(DK_NUMBER) \
	X(DK_STRING) \
	X(DK_IDENT) \
	X(DK_SYMBOL) \
\
	/* Types */ \
	X(DK_TYPE_NUMBER) \
	X(DK_TYPE_STRING) \
	X(DK_TYPE_ANY) \
	X(DK_TYPE_FN) \
\
	/* Keywords */ \
	X(DK_DEFINE) \
	X(DK_LET) \
\
	/* Operators */ \
	X(DK_OR) \
	X(DK_AND) \
	X(DK_NOT) \
\
	X(DK_ADD) \
	X(DK_SUB) \
	X(DK_MUL) \
	X(DK_DIV) \
\
	X(DK_COND) \
	X(DK_APPLY) \
	X(DK_EQUAL) \
\
	X(DK_ARROW) \
	X(DK_TYPE) \
\
	/* Grouping */ \
	X(DK_LPAREN) \
	X(DK_RPAREN) \
\
	X(DK_LBRACKET) \
	X(DK_RBRACKET)

#define X(x) x,
typedef enum {
	INSTRUCTIONS
} dk_instr_kind_t;
#undef X

#define X(x) [x] = #x,
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

// Lexer
typedef struct {
	const char* const src;

	const char* ptr;
	const char* end;

	dk_instr_t peek;
} dk_lexer_t;

static bool dk_lexer_take(dk_lexer_t*, dk_instr_t*);

static dk_lexer_t dk_lexer_create(const char* ptr, const char* end) {
	dk_lexer_t lx = (dk_lexer_t){
		.src = ptr,
		.ptr = ptr,
		.end = end,
		.peek = dk_instr_create(DK_NONE, ptr, ptr),
	};

	dk_lexer_take(&lx, NULL);

	return lx;
}

// Debugging/printing
// TODO: Make these functions returned a formatted buffer rather than
// calling printf within.
// TODO: Print position info for tokens?
static void dk_instr_print(dk_lexer_t* lx, dk_instr_t instr) {
	printf(
		"kind = %s, ptr = %p, end = %p, size = %lu\n",
		DK_INSTR_TO_STR[instr.kind],
		(void*) instr.str.ptr,
		(void*) instr.str.end,
		dk_ptrdiff(instr.str.ptr, instr.str.end));
}

static void dk_lexer_print(dk_lexer_t* lx, dk_instr_t instr) {
	// TODO: Print lexer state in some readable fashion for debugging.
	// Some ideas:
	// - Print all tokens and then reset lexer state (set `ptr` to `src`)
	// - Print pointers, peek token
	// - Total number of tokens
	// - Line count
	// - Percentage of file that has been lexed
}

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
	dk_lexer_t* lx,
	dk_instr_t* instr,
	dk_instr_kind_t kind,
	dk_lexer_pred_t cond) {
	*instr = dk_instr_create(DK_NONE, lx->ptr, lx->ptr);

	if (!dk_take_if(lx, cond)) {
		return false;
	}

	instr->str.end = lx->ptr;
	instr->kind = kind;

	return true;
}

static bool dk_produce_while(
	dk_lexer_t* lx,
	dk_instr_t* instr,
	dk_instr_kind_t kind,
	dk_lexer_pred_t cond) {
	*instr = dk_instr_create(DK_NONE, lx->ptr, lx->ptr);

	if (!dk_take_while(lx, cond)) {
		return false;
	}

	instr->str.end = lx->ptr;
	instr->kind = kind;

	return true;
}

static bool dk_produce_str(
	dk_lexer_t* lx, dk_instr_t* instr, dk_instr_kind_t kind, const char* str) {
	*instr = dk_instr_create(DK_NONE, lx->ptr, lx->ptr);

	if (!dk_take_str(lx, str)) {
		return false;
	}

	instr->str.end = lx->ptr;
	instr->kind = kind;

	return true;
}

static bool dk_produce_ident(dk_lexer_t* lx, dk_instr_t* instr) {
	*instr = dk_instr_create(DK_NONE, lx->ptr, lx->ptr);

	if (!dk_take_if(lx, dk_is_alpha)) {
		return false;
	}

	dk_take_while(lx, dk_is_ident);

	instr->str.end = lx->ptr;

	// We could have used `dk_produce_str` here to handle these cases
	// but we want "maximal munch" meaning that we lex the entire
	// identifier before trying to classify it. Why? because if we
	// didn't, an identifier like "letfoo" would actually be lexed
	// as 2 seperate tokens because it sees `let` and stops there.

	// Keywords
	if (dk_strncmp(instr->str.ptr, instr->str.end, "let")) {
		instr->kind = DK_LET;
	}

	else if (dk_strncmp(instr->str.ptr, instr->str.end, "def")) {
		instr->kind = DK_DEFINE;
	}

	// Operators
	else if (dk_strncmp(instr->str.ptr, instr->str.end, "or")) {
		instr->kind = DK_OR;
	}

	else if (dk_strncmp(instr->str.ptr, instr->str.end, "and")) {
		instr->kind = DK_AND;
	}

	else if (dk_strncmp(instr->str.ptr, instr->str.end, "not")) {
		instr->kind = DK_NOT;
	}

	// Types
	else if (dk_strncmp(instr->str.ptr, instr->str.end, "int")) {
		instr->kind = DK_TYPE_NUMBER;
	}

	else if (dk_strncmp(instr->str.ptr, instr->str.end, "string")) {
		instr->kind = DK_TYPE_STRING;
	}

	else if (dk_strncmp(instr->str.ptr, instr->str.end, "any")) {
		instr->kind = DK_TYPE_ANY;
	}

	// User identifier
	else {
		instr->kind = DK_IDENT;
	}

	return true;
}

static bool dk_produce_symbol(dk_lexer_t* lx, dk_instr_t* instr) {
	*instr = dk_instr_create(DK_NONE, lx->ptr, lx->ptr);

	if (!dk_take_ifc(lx, '#')) {
		return false;
	}

	dk_take_while(lx, dk_is_ident);

	instr->str.end = lx->ptr;
	instr->kind = DK_SYMBOL;

	return true;
}

static bool dk_produce_number(dk_lexer_t* lx, dk_instr_t* instr) {
	return dk_produce_while(lx, instr, DK_NUMBER, dk_is_digit);
}

static bool dk_produce_sigil(dk_lexer_t* lx, dk_instr_t* instr) {
#define DK_PRODUCE_SIGIL(s, k) dk_produce_str(lx, instr, s, k)
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

static bool dk_produce_whitespace(dk_lexer_t* lx, dk_instr_t* instr) {
	return dk_produce_while(lx, instr, DK_WHITESPACE, dk_is_whitespace);
}

static bool dk_produce_comment(dk_lexer_t* lx, dk_instr_t* instr) {
	*instr = dk_instr_create(DK_NONE, lx->ptr, lx->ptr);

	if (!dk_take_str(lx, "#!")) {
		return false;
	}

	if (!dk_take_while(lx, dk_is_comment)) {
		return false;
	}

	instr->str.end = lx->ptr;
	instr->kind = DK_COMMENT;

	return true;
}

// Core lexer interface
// TODO: Reconsider implementation. Is it safe to always return true?
static bool dk_lexer_peek(dk_lexer_t* lx, dk_instr_t* instr) {
	if (instr != NULL) {
		*instr = lx->peek;
	}

	return true;
}

// TODO: Make lexer_next produce all tokens and then wrap it in
// another function which skips whitespace and comments.
static bool dk_lexer_take(dk_lexer_t* lx, dk_instr_t* instr) {
	dk_instr_t next_instr = dk_instr_create(DK_NONE, lx->ptr, lx->ptr);

	// Handle EOF
	if (lx->ptr >= lx->end) {
		next_instr.kind = DK_ENDFILE;
	}

	// Handle normal tokens
	else if (!(dk_produce_whitespace(lx, &next_instr) ||
			   dk_produce_comment(lx, &next_instr) ||
			   dk_produce_ident(lx, &next_instr) ||
			   dk_produce_symbol(lx, &next_instr) ||
			   dk_produce_number(lx, &next_instr) ||
			   dk_produce_sigil(lx, &next_instr)))
	{
		// TODO: Error in the case of no tokens matched.
		return false;
	}

	// Return previously peeked token and then store newly
	// lexed token to be used on the next call to peek.
	if (instr != NULL) {
		dk_lexer_peek(lx, instr);
	}

	lx->peek = next_instr;

	return true;
}

#endif
