#include <getopt.h>
#include <libgen.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// #include <cdc/alloc.h>
#include <cdc/backend.h>
#include <cdc/def.h>
#include <cdc/lexer.h>
#include <cdc/parser.h>
#include <cdc/util.h>
#include <cdc/str.h>
#include <cdc/log.h>

int main(int argc, char* argv[]) {
	// int opt = 0;
	// char buf[256];

	// while ((opt = getopt(argc, argv, "i:")) != -1) {
	// 	switch (opt) {
	// 		case 'i': {
	// 			strncpy(buf, optarg, 256 - 1);
	// 			fprintf(stderr, "path: %s\n", buf);
	// 		} break;

	// 		default: {
	// 			if (!dk_basename(argv[0], buf, 256)) {
	// 				fprintf(stderr, "usage: %s -i [FILE]\n", buf);
	// 			}
	// 			return 1;
	// 		} break;
	// 	}
	// }

	// char* src;
	// size_t len;
	// dk_err_t err;

	// if (!buf[0]) {
	// 	err = dk_read_stdin(&src, &len);
	// 	if (err) {
	// 		fprintf(stderr, "error: <stdin>: %s\n", strerror(err));
	// 		return 1;
	// 	}
	// }
	// else {
	// 	err = dk_read_file(buf, &src, &len);
	// 	if (err) {
	// 		fprintf(stderr, "error: '%s': %s\n", buf, strerror(err));
	// 		return 1;
	// 	}
	// }

	// dk_lexer_t lx = dk_lexer_create(src, len);

	// token_t tok;
	// while (lexer_next(&lx, &tok) && tok.kind != TOK_EOF) {
	// 	printf("tok: %s : %d\n", tok_to_str[tok.kind], (int) lx.src[tok.start]);
	// }

	// free(src);

	// char* str;
	// size_t length;

	// dk_err_t err;

	// if ((err = dk_read_stdin(&str, &length))) {
	// 	fprintf(stderr, "error: <stdin>: %s\n", strerror(err));
	// 	return 1;
	// }

	// const char* str =
	// 	"f ba baz     \n FUCKER  #! sdofhsodfhoisdfhio\n          fuck";

	// dk_instr_t instr;
	// dk_lexer_t lx = dk_lexer_create(str, str + strlen(str));

	// while (dk_lexer_take(&lx, &instr) && instr.kind != DK_ENDFILE) {
	// 	dk_instr_print(&lx, instr);
	// }

	const char* input = "123 +123";

	dk_logger_t log = dk_logger_create("global");
	dk_context_t ctx = dk_context_create();
	dk_lexer_t lx = dk_lexer_create(&log, input, input + strlen(input));

	dk_parse(&log, &ctx, &lx);

	return 0;
}
