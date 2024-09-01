#ifndef CDC_UTIL_H
#define CDC_UTIL_H

#include <stdbool.h>
#include <stddef.h>

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "def.h"

#define DK_MAX(a, b) ((a > b) ? a : b)
#define DK_MIN(a, b) ((a < b) ? a : b)

// Return absolute difference between 2 pointers regardless of order.
size_t dk_ptrdiff(const void* a, const void* b) {
	return b > a ? b - a : a - b;
}

// Compare strings after first comparing length.
bool dk_strncmp(const char* ptr, const char* end, const char* str) {
	size_t length = dk_ptrdiff(ptr, end);

	// TODO: Write our own strncmp implementation here to avoid
	// iterating string twice due to strlen.
	if (length != strlen(str)) {
		return false;
	}

	return strncmp(ptr, str, length) == 0;
}

// Read bytes from stdin to a growing buffer until EOF.
dk_err_t dk_read_stdin(char** buffer, size_t* length) {
	// TODO: Just increase allocations by fixed amount with IO.
	char* buf = realloc(NULL, 256);

	size_t capacity = 256;
	size_t index = 0;

	int c;
	while ((c = fgetc(stdin)) != EOF) {
		// if (index >= capacity) {
		// 	capacity *= 2;
		// 	*buffer = realloc(*buffer, capacity);

		// 	if (!*buffer) {
		// 		return errno;
		// 	}
		// }

		buf[index++] = c;
	}

	if (ferror(stdin)) {
		return errno;
	}

	*length = index;

	return 0;
}

dk_err_t dk_read_file(const char* path, char** buf_out, size_t* len_out) {
	FILE* file = fopen(path, "r");
	if (!file) {
		return errno;
	}

	// get file size
	if (fseek(file, 0, SEEK_END) == -1) {
		return errno;
	}

	int len = ftell(file);
	if (len == -1) {
		return errno;
	}

	rewind(file);

	char* buf = malloc(len);
	if (!buf) {
		return errno;
	}

	// read file contents into buf
	int nread = fread(buf, 1, len, file);
	if (nread != len || fclose(file) == EOF) {
		dk_err_t code = errno;
		free(buf);
		return code;
	}

	*buf_out = buf;
	*len_out = len;

	return 0;
}

// dk_err_t dk_basename(const char* path, char* out, size_t size) {
// 	memset(out, 0, size);

// 	size_t len = strlen(path);

// 	char* last = strrchr(path, '/');
// 	if (!last) {
// 		if (len > size) {
// 			return ENAMETOOLONG;
// 		}

// 		memcpy(out, path, len);
// 		return 0;
// 	}

// 	len = strlen(last + 1);
// 	if (len > size) {
// 		return ENAMETOOLONG;
// 	}

// 	// memcpy(out, last + 1, len);
// 	strncpy(out, last + 1, size - len);

// 	return 0;
// }

// Get the name of the binary from argv[0].
// Basically `basename` but without allocating or trimming trailing slashes.
const char* dk_exe(const char* exe) {
	size_t slash = 0;
	size_t i = 0;

	for (; exe[i] != '\0'; ++i) {
		if (exe[i] == '/') {
			slash = i + 1;
		}
	}

	return exe + DK_MIN(slash, i);
}

#endif
