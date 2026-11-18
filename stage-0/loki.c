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

	enum ast_node_kind
	{
		AST_NODE_NONE,
		AST_NODE_MODULE,
		AST_NODE_DECL_FUNC,
		AST_NODE_STMT_RET,
		AST_NODE_EXPR_LIT_NUM,
	};

	struct ast_node {
		enum ast_node_kind kind;

		struct token token_begin;
		struct token token_end;

		struct token identifier;

		struct token token;
		i64 num;

		union {
			struct ast_node *stmt;
			struct ast_node *ret_val;
		};

		struct ast_node *next;
		struct ast_node *prev;
	};

	static size_t nodes_cnt = 0;
	static struct ast_node nodes[512];
#define ast_node_alloc(type, ...) ((nodes[nodes_cnt] = (struct ast_node){ .kind = type, __VA_ARGS__ }), &nodes[nodes_cnt++])

	struct ast_node *ast_func = ast_node_alloc(AST_NODE_DECL_FUNC);

	parse_expect(&lexer, &ast_func->token_begin, TT_IDENT, "identifier");
	parse_expect(&lexer, &ast_func->identifier,  TT_IDENT, "identifier");

	parse_expect(&lexer, &token, TT_LPAREN, "(");
	parse_expect(&lexer, &token, TT_RPAREN, ")");
	parse_expect(&lexer, &token, TT_LBRACE, "{");

	struct ast_node *stmt_ret = ast_node_alloc(AST_NODE_STMT_RET);

	parse_expect(&lexer, &stmt_ret->token_begin, TT_KW_RETURN, "return");

	struct ast_node *expr_lit_num = ast_node_alloc(AST_NODE_EXPR_LIT_NUM);

	parse_expect(&lexer, &expr_lit_num->token, TT_LIT_NUM, "number");


	parse_expect(&lexer, &stmt_ret->token_end, TT_SEMICOLON, ";");
	stmt_ret->ret_val = expr_lit_num;

	ast_func->stmt = stmt_ret;

	parse_expect(&lexer, &ast_func->token_end, TT_RBRACE, "}");

	// }}}

	log_info("success");
	log_fini();

	return 0;
}
