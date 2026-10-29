#include "loki.h"
#include "lex.h"

#include <stdio.h>

extern size_t strlen(const char *);

extern int
main(int argc, const char *argv[argc])
{
	const char *filename = argc > 1 ? argv[1] : "test/test.loki";
	struct string content = {};

	file_load_string(filename, &content);


	struct lex lexer = {
		.buf = content,
		.src_loc.filename.s      = filename,
		.src_loc.filename.length = strlen(filename),
	};
	struct token token = {};

	while (lex(&lexer, &token)) {
		printf("%.*s:%d:%d: %.*s\n",
			(int)token.src_loc.filename.length,
			token.src_loc.filename.s,
			token.src_loc.line,
			token.src_loc.column,
			(int)token.sv.length,
			token.sv.s
		);
	}




	return 0;
}
