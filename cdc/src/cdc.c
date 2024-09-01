#include <getopt.h>
#include <libgen.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <cdc/alloc.h>
#include <cdc/cdc.h>
#include <cdc/util.h>

#include <libgccjit.h>

int main(int argc, char* argv[]) {
	int opt = 0;
	char buf[256];

	while ((opt = getopt(argc, argv, "i:")) != -1) {
		switch (opt) {
			case 'i': {
				strncpy(buf, optarg, 256 - 1);
				fprintf(stderr, "path: %s\n", buf);
			} break;

			default: {
				if (!dk_basename(argv[0], buf, 256)) {
					fprintf(stderr, "usage: %s -i [FILE]\n", buf);
				}
				return 1;
			} break;
		}
	}

	char* src;
	size_t len;
	dk_err_t err;

	if (!buf[0]) {
		err = dk_read_stdin(&src, &len);
		if (err) {
			fprintf(stderr, "error: <stdin>: %s\n", strerror(err));
			return 1;
		}
	}
	else {
		err = dk_read_file(buf, &src, &len);
		if (err) {
			fprintf(stderr, "error: '%s': %s\n", buf, strerror(err));
			return 1;
		}
	}

	lexer_t lx = lexer_create(src, len);

	token_t tok;
	while (lexer_next(&lx, &tok) && tok.kind != TOK_EOF) {
		printf("tok: %s : %d\n", tok_to_str[tok.kind], (int) lx.src[tok.start]);
	}

	free(src);

	return 0;
}
