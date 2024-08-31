#ifndef CDC_UTIL_H
#define CDC_UTIL_H

#include <stdbool.h>
#include <stddef.h>

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#include "defs.h"

// Read bytes from stdin to a growing buffer until EOF.
dk_err_t dk_read_stdin(char** buffer, size_t* length) {
	size_t capacity = 0;
	size_t index = 0;

	int c;
	while ((c = fgetc(stdin)) != EOF) {
		if (index > capacity) {
			capacity *= 2;
			*buffer = realloc(*buffer, capacity);

			if (!*buffer) {
				return errno;
			}
		}

		(*buffer)[index++] = c;
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

dk_err_t dk_basename(const char* path, char* out, size_t size) {
	memset(out, 0, size);

	size_t len = strlen(path);

	char* last = strrchr(path, '/');
	if (!last) {
		if (len > size) {
			return ENAMETOOLONG;
		}

		memcpy(out, path, len);
		return 0;
	}

	len = strlen(last + 1);
	if (len > size) {
		return ENAMETOOLONG;
	}

	// memcpy(out, last + 1, len);
	strncpy(out, last + 1, size - len);

	return 0;
}

#endif
