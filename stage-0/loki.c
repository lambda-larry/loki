#include "loki.h"
#include "lex.h"

#include <stdio.h>

extern size_t strlen(const char *);


extern bool
parse_expect(struct lex *lexer, struct token *token, enum token_type type, const char *lit)
{
	if (!lex(lexer, token)) {
		diag_error(*token, "Unexpected EOF");
		return false;
	}

	if (token->type != type) {
		diag_error(*token, "Expected %s, got {}", lit);
		return false;
	}

	return true;
}

extern int
main(int argc, const char *argv[argc])
{
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
	struct token token = {};

#if 0
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

	struct ast_module {
		int i;
	};

	struct ast_expr_lit_num {
		struct token token;
		i64 num;
	};

	struct ast_stmt_ret {
		struct token token_begin;
		struct token token_end;
		struct ast_expr_lit_num *ret_val;
	};

	struct ast_func {
		struct token token_begin;
		struct token token_end;
		struct token identifier;
		struct ast_stmt_ret *stmt;
	};


	struct ast_func ast_func = {
	};

	parse_expect(&lexer, &ast_func.token_begin, TT_IDENT, "identifier");
	parse_expect(&lexer, &ast_func.identifier,  TT_IDENT, "identifier");

	parse_expect(&lexer, &token, TT_LPAREN, "(");
	parse_expect(&lexer, &token, TT_RPAREN, ")");
	parse_expect(&lexer, &token, TT_LBRACE, "{");

	struct ast_stmt_ret ast_stmt_ret = {
	};

	parse_expect(&lexer, &ast_stmt_ret.token_begin, TT_KW_RETURN, "return");

	struct ast_expr_lit_num ast_expr_lit_num = {
	};

	parse_expect(&lexer, &ast_expr_lit_num.token, TT_LIT_NUM, "number");


	parse_expect(&lexer, &ast_stmt_ret.token_end, TT_SEMICOLON, ";");
	ast_stmt_ret.ret_val = &ast_expr_lit_num;

	ast_func.stmt = &ast_stmt_ret;

	parse_expect(&lexer, &ast_func.token_end, TT_RBRACE, "}");

	// }}}

	log_info("success");
	log_fini();

	return 0;
}
