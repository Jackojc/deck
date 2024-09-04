#ifndef CDC_STR_H
#define CDC_STR_H

#include <stddef.h>

#include <cdc/alloc.h>
#include <string.h>

typedef struct {
	dk_alloc_t* alloc;
	size_t len;
	size_t cap;
	char data[];
} dk_str_t;

dk_str_t* dk_str_hdr(char* str) {
	return (dk_str_t*) ((char*) str - sizeof(dk_str_t));
}

char* dk_str(dk_alloc_t* alloc, const char* str) {
	size_t len = strlen(str);
	size_t cap = len + 1;
	dk_str_t* tmp = dk_alloc(alloc, sizeof(dk_str_t) + cap);

	tmp->alloc = alloc;
	tmp->len = len;
	tmp->cap = cap;

	return tmp->data;
}

void dk_str_free(char* str) {
	dk_str_t *str_hdr = dk_str_hdr(str);
	dk_free(str_hdr->alloc, str_hdr);
}

char* dk_str_append(char *to, char *from) {
	dk_str_t *to_hdr = dk_str_hdr(to);
	dk_str_t *from_hdr = dk_str_hdr(from);

	size_t new_len = to_hdr->len + from_hdr->len;
	size_t new_cap = to_hdr->cap;
	
	if (new_len + 1 >= to_hdr->cap) {
		new_cap = (new_len / 2) * 3;
	}

	
}

#endif
