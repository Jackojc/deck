#ifndef CDC_PARSER_H
#define CDC_PARSER_H

#include "lexer.h"
#include "def.h"
#include "util.h"
#include "log.h"

typedef struct {
	dk_logger_t log;
	// Function definitions
	// Bindings
	// Typestack
} dk_context_t;

static dk_context_t dk_context_create() {
	return (dk_context_t){.log = dk_logger_create()};
}

// Primary call that sets up lexer and context automatically.
static dk_instr_t* dk_parse(const char* str);

// Forward declarations for mutual recursion
// TODO: Make these functions return dk_instr_t* linked lists.
static dk_instr_t* dk_parse_program(dk_context_t* ctx, dk_lexer_t* lx);
static dk_instr_t* dk_parse_expression(dk_context_t* ctx, dk_lexer_t* lx);
static dk_instr_t* dk_parse_function(dk_context_t* ctx, dk_lexer_t* lx);
static dk_instr_t* dk_parse_builtin(dk_context_t* ctx, dk_lexer_t* lx);
static dk_instr_t* dk_parse_literal(dk_context_t* ctx, dk_lexer_t* lx);
static dk_instr_t* dk_parse_type(dk_context_t* ctx, dk_lexer_t* lx);
static dk_instr_t* dk_parse_fntype(dk_context_t* ctx, dk_lexer_t* lx);
static dk_instr_t* dk_parse_assertion(dk_context_t* ctx, dk_lexer_t* lx);
static dk_instr_t* dk_parse_swizzle(dk_context_t* ctx, dk_lexer_t* lx);

// Convenience functions
typedef bool (*dk_parser_pred_t)(dk_instr_t);  // Used for parser predicates

static bool dk_is_builtin(dk_instr_t instr) {
	return
		// Logical
		instr.kind == DK_OR || instr.kind == DK_AND || instr.kind == DK_NOT ||

		// Arithmetic
		instr.kind == DK_ADD || instr.kind == DK_SUB || instr.kind == DK_MUL ||
		instr.kind == DK_DIV ||

		// Other
		instr.kind == DK_COND || instr.kind == DK_APPLY ||
		instr.kind == DK_EQUAL;
}

static bool dk_is_literal(dk_instr_t instr) {
	return instr.kind == DK_NUMBER || instr.kind == DK_STRING;
}

static bool dk_is_primitive(dk_instr_t instr) {
	return instr.kind == DK_TYPE_NUMBER || instr.kind == DK_TYPE_STRING ||
		   instr.kind == DK_TYPE_ANY;
}

static bool dk_is_expression(dk_instr_t instr) {
	return dk_is_literal(instr) || dk_is_builtin(instr) ||
		   instr.kind == DK_IDENT ||     // Call
		   instr.kind == DK_LBRACKET ||  // Function
		   instr.kind == DK_TYPE ||      // Type assertion
		   instr.kind == DK_LPAREN;      // Swizzle
}

// Automatically handle peeking
static bool dk_peek_is_kind(dk_lexer_t* lx, dk_instr_kind_t kind) {
	dk_instr_t instr;
	dk_lexer_peek(lx, &instr);

	return instr.kind == kind;
}

static bool dk_peek_is(dk_lexer_t* lx, dk_parser_pred_t cond) {
	dk_instr_t instr;
	dk_lexer_peek(lx, &instr);

	return cond(instr);
}

static bool dk_peek_is_expression(dk_lexer_t* lx) {
	return dk_peek_is(lx, dk_is_expression);
}

static bool dk_peek_is_builtin(dk_lexer_t* lx) {
	return dk_peek_is(lx, dk_is_builtin);
}

static bool dk_peek_is_literal(dk_lexer_t* lx) {
	return dk_peek_is(lx, dk_is_literal);
}

static bool dk_peek_is_primitive(dk_lexer_t* lx) {
	return dk_peek_is(lx, dk_is_primitive);
}

static bool dk_peek_is_type(dk_lexer_t* lx) {
	return dk_peek_is_primitive(lx) || dk_peek_is_kind(lx, DK_TYPE_FN);
}

// Parsing utilities
static void
dk_expect_kind(dk_lexer_t* lx, dk_instr_kind_t kind, const char* msg) {
	DK_UNUSED(lx, kind, msg);
	abort();

	// TODO: Check if the next token is `kind`. If not, report `msg` and abort
	// compilation.
}

static void dk_expect(dk_lexer_t* lx, dk_parser_pred_t cond, const char* msg) {
	DK_UNUSED(lx, cond, msg);
	abort();

	// TODO: Check if the next token satisfies `cond`. If not, report `msg` and
	// abort compilation.
}

// Implementation of core parsing functions
static dk_instr_t* dk_parse(const char* str) {  // TODO: Pass string + len
	dk_lexer_t lx = dk_lexer_create(str, str + strlen(str));
	dk_context_t ctx = dk_context_create();     // TODO: Pass by pointer.

	DK_TRACE(ctx.log, "foo");

	dk_instr_t* program = dk_parse_program(&ctx, &lx);

	return program;
}

// Core parsing functions
static dk_instr_t* dk_parse_program(dk_context_t* ctx, dk_lexer_t* lx) {
	DK_TRACE(ctx->log, "foo");
	dk_instr_t* program =
		NULL;  // TODO: Use allocator API to construct linked list.

	while (dk_peek_is_expression(lx)) {
		// TODO: Concatenate `expr` with `program` to build up a linked list of
		// instructions which will act as our intermediate representation in the
		// compiler.
		dk_instr_t* expr = dk_parse_expression(ctx, lx);
	}

	dk_expect_kind(lx, DK_ENDFILE, "unexpected token");

	return program;
}

static dk_instr_t* dk_parse_expression(dk_context_t* ctx, dk_lexer_t* lx) {
	DK_TRACE(ctx->log, "foo");
	dk_expect(lx, dk_is_expression, "expected an expression");

	dk_instr_t* expression =
		NULL;  // TODO: Use allocator API to construct linked list.

	// Integers/strings
	if (dk_peek_is_literal(lx)) {
		expression = dk_parse_literal(ctx, lx);
	}

	// Builtin operators/functions
	else if (dk_peek_is_builtin(lx)) {
		expression = dk_parse_builtin(ctx, lx);
	}

	// Symbol reference
	else if (dk_peek_is_kind(lx, DK_IDENT)) {
		dk_instr_t instr;
		dk_lexer_take(lx, &instr);

		// TODO: Handle symbol reference that isn't a builtin
	}

	// Functions
	else if (dk_peek_is_kind(lx, DK_LBRACKET)) {
		expression = dk_parse_function(ctx, lx);
	}

	// Swizzle
	else if (dk_peek_is_kind(lx, DK_LPAREN)) {
		expression = dk_parse_swizzle(ctx, lx);
	}

	// Type assertion
	else if (dk_peek_is_kind(lx, DK_TYPE)) {
		// TODO: Should we even concatenate this expression? Maybe we should
		// just discard it.
		expression = dk_parse_assertion(ctx, lx);
	}

	else {
		// TODO: Unreachable
		DK_ERROR(ctx->log, "unknown token");
		abort();
	}

	return expression;
}

static dk_instr_t* dk_parse_function(dk_context_t* ctx, dk_lexer_t* lx) {
	DK_TRACE(ctx->log, "foo");
	dk_expect_kind(lx, DK_LBRACKET, "expected '['");
	dk_lexer_take(lx, NULL);

	dk_instr_t* function = NULL;

	while (dk_peek_is_expression(lx)) {
		// TODO: Concatenate `expr` with `function` to build up a linked list of
		// instructions which will act as our intermediate representation in the
		// compiler.
		dk_instr_t* expr = dk_parse_expression(ctx, lx);
	}

	dk_expect_kind(lx, DK_RBRACKET, "expected ']'");
	dk_lexer_take(lx, NULL);

	return function;
}

static dk_instr_t* dk_parse_builtin(dk_context_t* ctx, dk_lexer_t* lx) {
	DK_TRACE(ctx->log, "foo");
	dk_expect(lx, dk_is_builtin, "expected a built-in");

	// TODO: Allocate this node on the heap and return the pointer.
	dk_instr_t instr;
	dk_lexer_take(lx, &instr);

	return NULL;
}

static dk_instr_t* dk_parse_literal(dk_context_t* ctx, dk_lexer_t* lx) {
	DK_TRACE(ctx->log, "foo");
	dk_expect(lx, dk_is_literal, "expected a literal");

	// TODO: Allocate this node on the heap and return the pointer.
	dk_instr_t instr;
	dk_lexer_take(lx, &instr);

	return NULL;
}

static dk_instr_t* dk_parse_type(dk_context_t* ctx, dk_lexer_t* lx) {
	DK_TRACE(ctx->log, "foo");
	dk_instr_t instr;

	if (dk_peek_is_kind(lx, DK_TYPE_NUMBER)) {
		// TODO: Check top-of-stack is number.
		dk_lexer_take(lx, &instr);
	}

	else if (dk_peek_is_kind(lx, DK_TYPE_STRING)) {
		// TODO: Check top-of-stack is string.
		dk_lexer_take(lx, &instr);
	}

	else if (dk_peek_is_kind(lx, DK_TYPE_ANY)) {
		// TODO: Check that there is an element on the type stack. It doesn't
		// matter what it is.
		dk_lexer_take(lx, &instr);
	}

	else if (dk_peek_is_kind(lx, DK_TYPE_FN)) {
		dk_parse_fntype(ctx, lx);
	}

	return NULL;
}

static dk_instr_t* dk_parse_fntype(dk_context_t* ctx, dk_lexer_t* lx) {
	DK_TRACE(ctx->log, "foo");
	dk_expect_kind(lx, DK_TYPE_FN, "expected 'fn'");
	dk_lexer_take(lx, NULL);

	dk_expect_kind(lx, DK_LPAREN, "expected '('");
	dk_lexer_take(lx, NULL);

	// Parse input types
	while (dk_peek_is_type(lx)) {
		dk_parse_type(ctx, lx);
	}

	// Seperator
	dk_expect_kind(lx, DK_ARROW, "expected '->'");
	dk_lexer_take(lx, NULL);

	// Parse output types
	while (dk_peek_is_type(lx)) {
		dk_parse_type(ctx, lx);
	}

	dk_expect_kind(lx, DK_RPAREN, "expected ')'");
	dk_lexer_take(lx, NULL);

	return NULL;
}

static dk_instr_t* dk_parse_assertion(dk_context_t* ctx, dk_lexer_t* lx) {
	DK_TRACE(ctx->log, "foo");
	dk_expect_kind(lx, DK_TYPE, "expected '$'");
	dk_lexer_take(lx, NULL);

	dk_expect_kind(lx, DK_LPAREN, "expected '('");
	dk_lexer_take(lx, NULL);

	while (dk_peek_is_type(lx)) {
		dk_parse_type(ctx, lx);
	}

	dk_expect_kind(lx, DK_RPAREN, "expected ')'");
	dk_lexer_take(lx, NULL);

	return NULL;  // This function doesn't return anything. It just uses the
				  // type-stack from the context to do assertions on what types
				  // are present. We might need to do some NULL checking from
				  // the caller level in this case.
}

static dk_instr_t* dk_parse_swizzle(dk_context_t* ctx, dk_lexer_t* lx) {
	DK_TRACE(ctx->log, "foo");
	dk_expect_kind(lx, DK_LPAREN, "expected '('");
	dk_lexer_take(lx, NULL);

	dk_instr_t* swizzle = NULL;

	// TODO: Collect a list of input identifiers and a list of output
	// identifiers then match up where the old slots map to the new slots and
	// try to synthesize a series of primitive ops that encode the same mapping.

	// Parse at least one identifier on the left side. This is because we can
	// use swizzle to "drop" items from the stack where the right side is empty.
	dk_expect_kind(lx, DK_IDENT, "expected an identifier");

	do {
		// TODO: Figure out what swizzle should lower to. A series of primitive
		// dup, rot etc. or a single intrinsic.
		dk_instr_t instr;
		dk_lexer_take(lx, &instr);
	} while (dk_peek_is_kind(lx, DK_IDENT));

	// Seperator
	dk_expect_kind(lx, DK_ARROW, "expected '->'");
	dk_lexer_take(lx, NULL);

	// Parse new stack slots.
	while (dk_peek_is_kind(lx, DK_IDENT)) {
		dk_instr_t instr;
		dk_lexer_take(lx, &instr);
	}

	dk_expect_kind(lx, DK_RPAREN, "expected ')'");
	dk_lexer_take(lx, NULL);

	return swizzle;
}

#endif
