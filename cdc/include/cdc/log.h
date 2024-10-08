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
	X(DK_DEBUG, DK_FG_WHITE "[ ]" DK_RESET, "debug") \
	X(DK_TRACE, DK_FG_CYAN "[-]" DK_RESET, "trace") \
	X(DK_WARN, DK_FG_BLUE "[*]" DK_RESET, "warning") \
	X(DK_ERROR, DK_FG_RED "[!]" DK_RESET, "error") \
	X(DK_OKAY, DK_FG_GREEN "[^]" DK_RESET, "okay")

#define X(x, y, z) x,
typedef enum {
	LOGLEVELS
} dk_loglevel_t;
#undef X

#define X(x, y, z) [x] = y,
const char* DK_LOGLEVEL_TO_STR[] = {LOGLEVELS};
#undef X

#define X(x, y, z) [x] = z,
const char* DK_LOGLEVEL_HUMAN_TO_STR[] = {LOGLEVELS};
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
		.dest = NULL,
		.level = DK_DEBUG,
		.indent = 0,
	};
}

static void dk_log_info_v(
	dk_logger_t log, dk_loglevel_t lvl, const char* filename, const char* line,
	const char* func, const char* fmt, va_list args) {
	if (lvl < log.level) {
		return;
	}

	if (log.dest == NULL) {
		log.dest = stderr;  // Default location for logging.
	}

	const char* lvl_s = DK_LOGLEVEL_TO_STR[lvl];
	const char* lvl_hs = DK_LOGLEVEL_HUMAN_TO_STR[lvl];

	fprintf(log.dest, "%s " DK_BOLD "%s" DK_RESET, lvl_s, lvl_hs);

	// TODO: Check if these cases actually work.
	if (filename != NULL && line != NULL) {
		fprintf(log.dest, " [%s:%s]", filename, line);
	}

	else if (filename != NULL && line == NULL) {
		fprintf(log.dest, " [%s]", filename);
	}

	else if (filename == NULL && line != NULL) {
		fprintf(log.dest, " [%s]", line);
	}

	if (func != NULL) {
		fprintf(log.dest, " `%s`", func);
	}

	fprintf(log.dest, ": ");

	vfprintf(log.dest, fmt, args);

	fputc('\n', log.dest);
}

static void dk_log_info(
	dk_logger_t log, dk_loglevel_t lvl, const char* filename, const char* line,
	const char* func, const char* fmt, ...) {
	va_list args;
	va_start(args, fmt);

	dk_log_info_v(log, lvl, filename, line, func, fmt, args);

	va_end(args);
}

static void dk_log(dk_logger_t log, dk_loglevel_t lvl, const char* fmt, ...) {
	va_list args;
	va_start(args, fmt);

	dk_log_info_v(log, lvl, NULL, NULL, NULL, fmt, args);

	va_end(args);
}

#define DK_DEBUG(log, ...) \
	dk_log_info( \
		log, DK_DEBUG, __FILE__, DK_STR(__LINE__), __func__, __VA_ARGS__)

#define DK_TRACE(log, ...) \
	dk_log_info( \
		log, DK_TRACE, __FILE__, DK_STR(__LINE__), __func__, __VA_ARGS__)

#define DK_WARN(log, ...) \
	dk_log_info(log, DK_WARN, __FILE__, DK_STR(__LINE__), __func__, __VA_ARGS__)

#define DK_ERROR(log, ...) \
	dk_log_info( \
		log, DK_ERROR, __FILE__, DK_STR(__LINE__), __func__, __VA_ARGS__)

#define DK_OKAY(log, ...) \
	dk_log_info(log, DK_OKAY, __FILE__, DK_STR(__LINE__), __func__, __VA_ARGS__)

// TODO: Implement "unimplemented" macro that will unconditionally abort
// TODO: Implement "unreachable" macro
// TODO: Implement dk_die function that reports using DK_ERROR and then calls
// abort
// TODO: Implement assert macro
// TODO: Debug macro (might not be reasonable)

// Global logger instance
static dk_logger_t DK_LOGGER = (dk_logger_t){
	.dest = NULL,
	.level = DK_DEBUG,
	.indent = 0,
};

#endif
