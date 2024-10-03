#ifndef CDC_LOG_H
#define CDC_LOG_H

#include <stdio.h>
#include <stddef.h>
#include <stdarg.h>

#define DK_RESET "\x1b[0m"
#define DK_BOLD "\x1b[1m"

#define DK_FG_BLACK "\x1b[30m"
#define DK_FG_RED "\x1b[31m"
#define DK_FG_GREEN "\x1b[32m"
#define DK_FG_YELLOW "\x1b[33m"
#define DK_FG_BLUE "\x1b[34m"
#define DK_FG_MAGENTA "\x1b[35m"
#define DK_FG_CYAN "\x1b[36m"
#define DK_FG_WHITE "\x1b[37m"

#define DK_FG_BLACK_BRIGHT "\x1b[30;1m"
#define DK_FG_RED_BRIGHT "\x1b[31;1m"
#define DK_FG_GREEN_BRIGHT "\x1b[32;1m"
#define DK_FG_YELLOW_BRIGHT "\x1b[33;1m"
#define DK_FG_BLUE_BRIGHT "\x1b[34;1m"
#define DK_FG_MAGENTA_BRIGHT "\x1b[35;1m"
#define DK_FG_CYAN_BRIGHT "\x1b[36;1m"
#define DK_FG_WHITE_BRIGHT "\x1b[37;1m"

#define DK_BG_BLACK "\x1b[40m"
#define DK_BG_RED "\x1b[41m"
#define DK_BG_GREEN "\x1b[42m"
#define DK_BG_YELLOW "\x1b[43m"
#define DK_BG_BLUE "\x1b[44m"
#define DK_BG_MAGENTA "\x1b[45m"
#define DK_BG_CYAN "\x1b[46m"
#define DK_BG_WHITE "\x1b[47m"

#define DK_BG_BLACK_BRIGHT "\x1b[40;1m"
#define DK_BG_RED_BRIGHT "\x1b[41;1m"
#define DK_BG_GREEN_BRIGHT "\x1b[42;1m"
#define DK_BG_YELLOW_BRIGHT "\x1b[43;1m"
#define DK_BG_BLUE_BRIGHT "\x1b[44;1m"
#define DK_BG_MAGENTA_BRIGHT "\x1b[45;1m"
#define DK_BG_CYAN_BRIGHT "\x1b[46;1m"
#define DK_BG_WHITE_BRIGHT "\x1b[47;1m"

#define LOGLEVELS \
	X(DK_DEBUG, DK_FG_WHITE "[ ]" DK_RESET) \
	X(DK_TRACE, DK_FG_CYAN "[-]" DK_RESET) \
	X(DK_WARN, DK_FG_BLUE "[*]" DK_RESET) \
	X(DK_ERROR, DK_FG_RED "[!]" DK_RESET) \
	X(DK_OKAY, DK_FG_GREEN "[^]" DK_RESET)

#define X(x, y) x,
typedef enum {
	LOGLEVELS
} dk_loglevel_t;
#undef X

#define X(x, y) [x] = y,
const char* DK_LOGLEVEL_TO_STR[] = {LOGLEVELS};
#undef X

#undef LOGLEVELS

typedef struct {
	FILE* dest;
	dk_loglevel_t level;
	size_t indent;
	// TODO: Add field to track pass name.
} dk_logger_t;

static dk_logger_t dk_logger_create() {
	return (dk_logger_t){
		.dest = stderr,
		.level = DK_DEBUG,
		.indent = 0,
	};
}

// TODO: Implement version of logging function which doesn't use line info.

static void dk_log(
	dk_logger_t log, dk_loglevel_t lvl, const char* filename, size_t line,
	const char* func, const char* fmt, ...) {
	if (lvl < log.level) {
		return;
	}

	fprintf(
		log.dest,
		"%s " DK_BOLD "[%s:%lu]" DK_RESET " `%s`: ", DK_LOGLEVEL_TO_STR[lvl],
		filename, line, func);

	va_list args;
	va_start(args, fmt);

	vfprintf(log.dest, fmt, args);

	va_end(args);

	fputc('\n', log.dest);
}

#define DK_DEBUG(log, ...) \
	dk_log(log, DK_DEBUG, __FILE__, __LINE__, __func__, __VA_ARGS__)

#define DK_TRACE(log, ...) \
	dk_log(log, DK_TRACE, __FILE__, __LINE__, __func__, __VA_ARGS__)

#define DK_WARN(log, ...) \
	dk_log(log, DK_WARN, __FILE__, __LINE__, __func__, __VA_ARGS__)

#define DK_ERROR(log, ...) \
	dk_log(log, DK_ERROR, __FILE__, __LINE__, __func__, __VA_ARGS__)

#define DK_OKAY(log, ...) \
	dk_log(log, DK_OKAY, __FILE__, __LINE__, __func__, __VA_ARGS__)

// TODO: Implement "unimplemented" macro that will unconditionally abort
// TODO: Implement "unreachable" macro
// TODO: Implement dk_die function that reports using DK_ERROR and then calls
// abort
// TODO: Implement assert macro
// TODO: Debug macro (might not be reasonable)

#endif
