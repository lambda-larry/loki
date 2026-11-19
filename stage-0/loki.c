#include "loki.h"
#include "lex.h"

#include <stdio.h>

extern int
main(int argc, const char *argv[argc])
{
	extern size_t strlen(const char *);

	log_init(stderr);


	const char *filename = argc > 1 ? argv[1] : "test/test.loki";
	struct string content = {};

	file_load_string(filename, &content);


	// {{{ lexer

	struct lex lexer = {
		.buf = content,
		.src_loc.filename.s      = filename,
		.src_loc.filename.length = strlen(filename),
	};
	struct lex checkpoint = lexer;

#if 0
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
		diag_info(token, "yes lit {}, kind %i", token.type);
	}
#endif
	lexer = checkpoint;

	// }}}

	// {{{ parser

	struct ast_node *ast_func = {};

	parse_decl_func(&lexer, &ast_func);

	// }}}

	log_info("success");
	log_fini();

	return 0;
}
