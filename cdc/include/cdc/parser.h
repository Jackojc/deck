#ifndef CDC_PARSER_H
#define CDC_PARSER_H

#include "lexer.h"
#include "def.h"
#include "util.h"

typedef struct {
	// Function definitions
	// Bindings
	// Typestack
} dk_context_t;

// Forward declarations for mutual recursion
// TODO: Make these functions return dk_instr_t* linked lists.
static void program(dk_context_t* ctx, dk_lexer_t* lx);
static void function(dk_context_t* ctx, dk_lexer_t* lx);
static void builtin(dk_context_t* ctx, dk_lexer_t* lx);
static void assertion(dk_context_t* ctx, dk_lexer_t* lx);
static void swizzle(dk_context_t* ctx, dk_lexer_t* lx);

// Implementation
static void program(dk_context_t* ctx, dk_lexer_t* lx) {
}

static void function(dk_context_t* ctx, dk_lexer_t* lx) {
}

static void builtin(dk_context_t* ctx, dk_lexer_t* lx) {
}

static void assertion(dk_context_t* ctx, dk_lexer_t* lx) {
}

static void swizzle(dk_context_t* ctx, dk_lexer_t* lx) {
}

#endif
