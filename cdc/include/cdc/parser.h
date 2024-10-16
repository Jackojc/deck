#ifndef CDC_PARSER_H
#define CDC_PARSER_H

#include "lexer.h"
#include "def.h"
#include "util.h"
#include "log.h"

typedef struct {
	// Function definitions
	// Bindings
	// Typestack
} dk_context_t;

static dk_context_t dk_context_create() {
	return (dk_context_t){};
}

// Primary call that sets up lexer and context automatically.
static dk_instr_t*
dk_parse(dk_logger_t* log, dk_context_t* ctx, dk_lexer_t* lx);

// Forward declarations for mutual recursion
// TODO: Make these functions return dk_instr_t* linked lists.
static dk_instr_t*
dk_parse_program(dk_logger_t* log, dk_context_t* ctx, dk_lexer_t* lx);
static dk_instr_t*
dk_parse_expression(dk_logger_t* log, dk_context_t* ctx, dk_lexer_t* lx);
static dk_instr_t*
dk_parse_function(dk_logger_t* log, dk_context_t* ctx, dk_lexer_t* lx);
static dk_instr_t*
dk_parse_builtin(dk_logger_t* log, dk_context_t* ctx, dk_lexer_t* lx);
static dk_instr_t*
dk_parse_literal(dk_logger_t* log, dk_context_t* ctx, dk_lexer_t* lx);
static dk_instr_t*
dk_parse_type(dk_logger_t* log, dk_context_t* ctx, dk_lexer_t* lx);
static dk_instr_t*
dk_parse_fntype(dk_logger_t* log, dk_context_t* ctx, dk_lexer_t* lx);
static dk_instr_t*
dk_parse_assertion(dk_logger_t* log, dk_context_t* ctx, dk_lexer_t* lx);
static dk_instr_t*
dk_parse_swizzle(dk_logger_t* log, dk_context_t* ctx, dk_lexer_t* lx);

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
static bool
dk_peek_is_kind(dk_logger_t* log, dk_lexer_t* lx, dk_instr_kind_t kind) {
	dk_instr_t instr;
	dk_lexer_peek(log, lx, &instr);

	return instr.kind == kind;
}

static bool
dk_peek_is(dk_logger_t* log, dk_lexer_t* lx, dk_parser_pred_t cond) {
	dk_instr_t instr;
	dk_lexer_peek(log, lx, &instr);

	return cond(instr);
}

static bool dk_peek_is_expression(dk_logger_t* log, dk_lexer_t* lx) {
	return dk_peek_is(log, lx, dk_is_expression);
}

static bool dk_peek_is_builtin(dk_logger_t* log, dk_lexer_t* lx) {
	return dk_peek_is(log, lx, dk_is_builtin);
}

static bool dk_peek_is_literal(dk_logger_t* log, dk_lexer_t* lx) {
	return dk_peek_is(log, lx, dk_is_literal);
}

static bool dk_peek_is_primitive(dk_logger_t* log, dk_lexer_t* lx) {
	return dk_peek_is(log, lx, dk_is_primitive);
}

static bool dk_peek_is_type(dk_logger_t* log, dk_lexer_t* lx) {
	return dk_peek_is_primitive(log, lx) ||
		   dk_peek_is_kind(log, lx, DK_TYPE_FN);
}

// Parsing utilities
// TODO: `dk_expect_kind` should call `dk_expect_kind` to avoid redundant code.
static void dk_expect_kind(
	dk_logger_t* log, dk_lexer_t* lx, dk_instr_kind_t kind, const char* fmt,
	...) {
	// TODO: Report line info from deck lexer. (use dk_log_info function).

	if (dk_peek_is_kind(log, lx, kind)) {
		return;
	}

	dk_instr_t peeker;
	dk_lexer_peek(log, lx, &peeker);

	DK_DEBUG(
		log,
		"expected = %s, kind = %s, ptr = %p, end = %p, size = %lu, str = "
		"'%.*s'",
		DK_INSTR_KIND_TO_STR[kind],
		DK_INSTR_KIND_TO_STR[peeker.kind],
		(void*) peeker.str.ptr,
		(void*) peeker.str.end,
		dk_ptrdiff(peeker.str.ptr, peeker.str.end),
		dk_ptrdiff(peeker.str.ptr, peeker.str.end),
		peeker.str.ptr);

	va_list args;
	va_start(args, fmt);

	dk_log(log, DK_ERROR, fmt, args);

	va_end(args);

	exit(EXIT_FAILURE);
}

static void dk_expect(
	dk_logger_t* log, dk_lexer_t* lx, dk_parser_pred_t cond, const char* fmt,
	...) {
	// TODO: Report line info from deck lexer. (use dk_log_info function).

	if (dk_peek_is(log, lx, cond)) {
		return;
	}

	va_list args;
	va_start(args, fmt);

	dk_log_info_v(log, DK_ERROR, NULL, NULL, NULL, fmt, args);

	va_end(args);

	exit(EXIT_FAILURE);
}

// Implementation of core parsing functions
static dk_instr_t*
dk_parse(dk_logger_t* log, dk_context_t* ctx, dk_lexer_t* lx) {
	DK_FUNCTION_ENTER(log);

	dk_instr_t* program = dk_parse_program(log, ctx, lx);

	return program;
}

// Core parsing functions
static dk_instr_t*
dk_parse_program(dk_logger_t* log, dk_context_t* ctx, dk_lexer_t* lx) {
	DK_FUNCTION_ENTER(log);

	dk_instr_t* program =
		NULL;  // TODO: Use allocator API to construct linked list.

	while (dk_peek_is_expression(log, lx)) {
		// TODO: Concatenate `expr` with `program` to build up a linked list of
		// instructions which will act as our intermediate representation in the
		// compiler.
		dk_instr_t* expr = dk_parse_expression(log, ctx, lx);
	}

	dk_instr_t instr;
	dk_lexer_peek(log, lx, &instr);

	dk_expect_kind(
		log, lx, DK_ENDFILE, "expected EOF but found: '%s'",
		DK_INSTR_TO_STR[instr.kind]);

	return program;
}

static dk_instr_t*
dk_parse_expression(dk_logger_t* log, dk_context_t* ctx, dk_lexer_t* lx) {
	DK_FUNCTION_ENTER(log);

	dk_expect(log, lx, dk_is_expression, "expected an expression");

	dk_instr_t* expression =
		NULL;  // TODO: Use allocator API to construct linked list.

	// Integers/strings
	if (dk_peek_is_literal(log, lx)) {
		expression = dk_parse_literal(log, ctx, lx);
	}

	// Builtin operators/functions
	else if (dk_peek_is_builtin(log, lx)) {
		expression = dk_parse_builtin(log, ctx, lx);
	}

	// Symbol reference
	else if (dk_peek_is_kind(log, lx, DK_IDENT)) {
		dk_instr_t instr;
		dk_lexer_take(log, lx, &instr);

		// TODO: Handle symbol reference that isn't a builtin
		DK_WHEREAMI(log);
	}

	// Functions
	else if (dk_peek_is_kind(log, lx, DK_LBRACKET)) {
		expression = dk_parse_function(log, ctx, lx);
	}

	// Swizzle
	else if (dk_peek_is_kind(log, lx, DK_LPAREN)) {
		expression = dk_parse_swizzle(log, ctx, lx);
	}

	// Type assertion
	else if (dk_peek_is_kind(log, lx, DK_TYPE)) {
		// TODO: Should we even concatenate this expression? Maybe we should
		// just discard it.
		expression = dk_parse_assertion(log, ctx, lx);
	}

	else {
		// TODO: Unreachable
		dk_instr_t instr;
		dk_lexer_peek(log, lx, &instr);

		dk_log(
			log, DK_ERROR, "unexpected token '%s'",
			DK_INSTR_TO_STR[instr.kind]);

		exit(EXIT_FAILURE);
	}

	return expression;
}

static dk_instr_t*
dk_parse_function(dk_logger_t* log, dk_context_t* ctx, dk_lexer_t* lx) {
	DK_FUNCTION_ENTER(log);

	dk_expect_kind(log, lx, DK_LBRACKET, "expected '['");
	dk_lexer_take(log, lx, NULL);

	dk_instr_t* function = NULL;

	while (dk_peek_is_expression(log, lx)) {
		// TODO: Concatenate `expr` with `function` to build up a linked list of
		// instructions which will act as our intermediate representation in the
		// compiler.
		dk_instr_t* expr = dk_parse_expression(log, ctx, lx);
	}

	dk_expect_kind(log, lx, DK_RBRACKET, "expected ']'");
	dk_lexer_take(log, lx, NULL);

	return function;
}

static dk_instr_t*
dk_parse_builtin(dk_logger_t* log, dk_context_t* ctx, dk_lexer_t* lx) {
	DK_FUNCTION_ENTER(log);

	dk_expect(log, lx, dk_is_builtin, "expected a built-in");

	// TODO: Allocate this node on the heap and return the pointer.
	dk_instr_t instr;
	dk_lexer_take(log, lx, &instr);

	return NULL;
}

static dk_instr_t*
dk_parse_literal(dk_logger_t* log, dk_context_t* ctx, dk_lexer_t* lx) {
	DK_FUNCTION_ENTER(log);

	dk_expect(log, lx, dk_is_literal, "expected a literal");

	// TODO: Allocate this node on the heap and return the pointer.
	dk_instr_t instr;
	dk_lexer_take(log, lx, &instr);

	return NULL;
}

static dk_instr_t*
dk_parse_type(dk_logger_t* log, dk_context_t* ctx, dk_lexer_t* lx) {
	DK_FUNCTION_ENTER(log);

	dk_instr_t instr;

	if (dk_peek_is_kind(log, lx, DK_TYPE_NUMBER)) {
		// TODO: Check top-of-stack is number.
		dk_lexer_take(log, lx, &instr);
	}

	else if (dk_peek_is_kind(log, lx, DK_TYPE_STRING)) {
		// TODO: Check top-of-stack is string.
		dk_lexer_take(log, lx, &instr);
	}

	else if (dk_peek_is_kind(log, lx, DK_TYPE_ANY)) {
		// TODO: Check that there is an element on the type stack. It doesn't
		// matter what it is.
		dk_lexer_take(log, lx, &instr);
	}

	else if (dk_peek_is_kind(log, lx, DK_TYPE_FN)) {
		dk_parse_fntype(log, ctx, lx);
	}

	return NULL;
}

static dk_instr_t*
dk_parse_fntype(dk_logger_t* log, dk_context_t* ctx, dk_lexer_t* lx) {
	DK_FUNCTION_ENTER(log);

	dk_expect_kind(log, lx, DK_TYPE_FN, "expected 'fn'");
	dk_lexer_take(log, lx, NULL);

	dk_expect_kind(log, lx, DK_LPAREN, "expected '('");
	dk_lexer_take(log, lx, NULL);

	// Parse input types
	while (dk_peek_is_type(log, lx)) {
		dk_parse_type(log, ctx, lx);
	}

	// Seperator
	dk_expect_kind(log, lx, DK_ARROW, "expected '->'");
	dk_lexer_take(log, lx, NULL);

	// Parse output types
	while (dk_peek_is_type(log, lx)) {
		dk_parse_type(log, ctx, lx);
	}

	dk_expect_kind(log, lx, DK_RPAREN, "expected ')'");
	dk_lexer_take(log, lx, NULL);

	return NULL;
}

static dk_instr_t*
dk_parse_assertion(dk_logger_t* log, dk_context_t* ctx, dk_lexer_t* lx) {
	DK_FUNCTION_ENTER(log);

	dk_expect_kind(log, lx, DK_TYPE, "expected '$'");
	dk_lexer_take(log, lx, NULL);

	dk_expect_kind(log, lx, DK_LPAREN, "expected '('");
	dk_lexer_take(log, lx, NULL);

	while (dk_peek_is_type(log, lx)) {
		dk_parse_type(log, ctx, lx);
	}

	dk_expect_kind(log, lx, DK_RPAREN, "expected ')'");
	dk_lexer_take(log, lx, NULL);

	return NULL;  // This function doesn't return anything. It just uses the
				  // type-stack from the context to do assertions on what types
				  // are present. We might need to do some NULL checking from
				  // the caller level in this case.
}

static dk_instr_t*
dk_parse_swizzle(dk_logger_t* log, dk_context_t* ctx, dk_lexer_t* lx) {
	DK_FUNCTION_ENTER(log);

	dk_expect_kind(log, lx, DK_LPAREN, "expected '('");
	dk_lexer_take(log, lx, NULL);

	dk_instr_t* swizzle = NULL;

	// TODO: Collect a list of input identifiers and a list of output
	// identifiers then match up where the old slots map to the new slots and
	// try to synthesize a series of primitive ops that encode the same mapping.

	// Parse at least one identifier on the left side. This is because we can
	// use swizzle to "drop" items from the stack where the right side is empty.
	dk_expect_kind(log, lx, DK_IDENT, "expected an identifier");

	do {
		// TODO: Figure out what swizzle should lower to. A series of primitive
		// dup, rot etc. or a single intrinsic.
		dk_instr_t instr;
		dk_lexer_take(log, lx, &instr);
	} while (dk_peek_is_kind(log, lx, DK_IDENT));

	// Seperator
	dk_expect_kind(log, lx, DK_ARROW, "expected '->'");
	dk_lexer_take(log, lx, NULL);

	// Parse new stack slots.
	while (dk_peek_is_kind(log, lx, DK_IDENT)) {
		dk_instr_t instr;
		dk_lexer_take(log, lx, &instr);
	}

	dk_expect_kind(log, lx, DK_RPAREN, "expected ')'");
	dk_lexer_take(log, lx, NULL);

	return swizzle;
}

#endif
