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

#define DK_FG_BLACK_BRIGHT "\x1b[90m"
#define DK_FG_RED_BRIGHT "\x1b[91m"
#define DK_FG_GREEN_BRIGHT "\x1b[92m"
#define DK_FG_YELLOW_BRIGHT "\x1b[93m"
#define DK_FG_BLUE_BRIGHT "\x1b[94m"
#define DK_FG_MAGENTA_BRIGHT "\x1b[95m"
#define DK_FG_CYAN_BRIGHT "\x1b[96m"
#define DK_FG_WHITE_BRIGHT "\x1b[97m"

#define DK_BG_BLACK "\x1b[40m"
#define DK_BG_RED "\x1b[41m"
#define DK_BG_GREEN "\x1b[42m"
#define DK_BG_YELLOW "\x1b[43m"
#define DK_BG_BLUE "\x1b[44m"
#define DK_BG_MAGENTA "\x1b[45m"
#define DK_BG_CYAN "\x1b[46m"
#define DK_BG_WHITE "\x1b[47m"

#define DK_BG_BLACK_BRIGHT "\x1b[100m"
#define DK_BG_RED_BRIGHT "\x1b[101m"
#define DK_BG_GREEN_BRIGHT "\x1b[102m"
#define DK_BG_YELLOW_BRIGHT "\x1b[103m"
#define DK_BG_BLUE_BRIGHT "\x1b[104m"
#define DK_BG_MAGENTA_BRIGHT "\x1b[105m"
#define DK_BG_CYAN_BRIGHT "\x1b[106m"
#define DK_BG_WHITE_BRIGHT "\x1b[107m"

#define LOGLEVELS \
	X(DK_DEBUG, "[.]", "debug", DK_FG_CYAN_BRIGHT) \
	X(DK_TRACE, "[-]", "trace", DK_FG_MAGENTA_BRIGHT) \
	X(DK_WARN, "[*]", "warning", DK_FG_BLUE) \
	X(DK_ERROR, "[!]", "error", DK_FG_RED) \
	X(DK_OKAY, "[^]", "okay", DK_FG_GREEN)

#define X(x, y, z, w) x,
typedef enum {
	LOGLEVELS
} dk_loglevel_t;
#undef X

#define X(x, y, z, w) [x] = y,
const char* DK_LOGLEVEL_TO_STR[] = {LOGLEVELS};
#undef X

#define X(x, y, z, w) [x] = z,
const char* DK_LOGLEVEL_HUMAN_TO_STR[] = {LOGLEVELS};
#undef X

#define X(x, y, z, w) [x] = w,
const char* DK_LOGLEVEL_COLOUR[] = {LOGLEVELS};
#undef X

#undef LOGLEVELS

typedef struct {
	const char* name;  // Name of logger for filtering by pass or stage
	FILE* dest;        // Destination to log to (usually stderr)
	dk_loglevel_t level;
	size_t indent;
} dk_logger_t;

static dk_logger_t dk_logger_create(const char* name) {
	return (dk_logger_t){
		.name = name,
		.dest = NULL,
		.level = DK_DEBUG,
		.indent = 0,
	};
}

static void dk_log_info_v(
	dk_logger_t* log, dk_loglevel_t lvl, const char* filename, const char* line,
	const char* func, const char* fmt, va_list args) {
	if (lvl < log->level) {
		return;
	}

	if (log->dest == NULL) {
		log->dest = stderr;  // Default location for logging.
	}

	const char* lvl_s = DK_LOGLEVEL_TO_STR[lvl];
	const char* lvl_hs = DK_LOGLEVEL_HUMAN_TO_STR[lvl];
	const char* lvl_col = DK_LOGLEVEL_COLOUR[lvl];

	fprintf(
		log->dest,
		DK_BOLD "%s%s" DK_RESET
				" "
				"%s%s" DK_RESET,
		lvl_col, lvl_s, lvl_col, lvl_hs);

	// TODO: Check if these cases actually work.
	if (filename != NULL && line != NULL) {
		fprintf(log->dest, " [%s:%s]", filename, line);
	}

	else if (filename != NULL && line == NULL) {
		fprintf(log->dest, " [%s]", filename);
	}

	else if (filename == NULL && line != NULL) {
		fprintf(log->dest, " [%s]", line);
	}

	if (func != NULL) {
		fprintf(log->dest, " `%s`", func);
	}

	if (fmt != NULL) {
		fprintf(log->dest, ": ");
		vfprintf(log->dest, fmt, args);
	}

	fputc('\n', log->dest);
}

static void dk_log_info(
	dk_logger_t* log, dk_loglevel_t lvl, const char* filename, const char* line,
	const char* func, const char* fmt, ...) {
	va_list args;
	va_start(args, fmt);

	dk_log_info_v(log, lvl, filename, line, func, fmt, args);

	va_end(args);
}

static void dk_log(dk_logger_t* log, dk_loglevel_t lvl, const char* fmt, ...) {
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

// Find out where you are with a rainbow.
#define DK_WHEREAMI(log) \
	dk_log_info( \
		log, DK_DEBUG, __FILE__, DK_STR(__LINE__), __func__, \
		DK_FG_RED "Y" DK_FG_RED_BRIGHT "O" DK_FG_YELLOW "U" DK_RESET \
				  " " DK_FG_GREEN "A" DK_FG_BLUE "R" DK_FG_MAGENTA \
				  "E" DK_RESET " " DK_FG_MAGENTA_BRIGHT "H" DK_FG_RED \
				  "E" DK_FG_RED_BRIGHT "R" DK_FG_YELLOW "E" DK_RESET)

#define DK_FUNCTION_ENTER(log) \
	dk_log_info(log, DK_DEBUG, __FILE__, DK_STR(__LINE__), __func__, NULL)

// TODO: Implement "unimplemented" macro that will unconditionally abort
// TODO: Implement "unreachable" macro
// TODO: Implement dk_die function that reports using DK_ERROR and then calls
// abort
// TODO: Implement assert macro
// TODO: Debug macro (might not be reasonable)

// Global logger instance
// static dk_logger_t DK_LOGGER = (dk_logger_t){
// 	.dest = NULL,
// 	.level = DK_DEBUG,
// 	.indent = 0,
// };

#endif
