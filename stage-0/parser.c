#include "loki.h"
#include "lex.h"

static bool
parse_expect(struct lex *lexer, struct token *token, enum token_type type, const char *expect)
{
	if (!lex(lexer, token)) {
		diag_error(*token, "Unexpected EOF");
		return false;
	}

	if (token->type != type) {
		diag_error(*token, "Expected %s, got {}", expect);
		return false;
	}

	return true;
}

static size_t nodes_cnt = 0;
static struct ast_node nodes[512];

#define ast_node_alloc(type, ...) ((nodes[nodes_cnt] = (struct ast_node){ .kind = type, __VA_ARGS__ }), &nodes[nodes_cnt++])

extern bool
parse_expr_lit_num(struct lex *lex, struct ast_node **ret)
{
	size_t node_chkp = nodes_cnt;
	struct lex lexer = *lex;
	struct ast_node *expr_lit_num = ast_node_alloc(AST_NODE_EXPR_LIT_NUM, .lex_begin = lexer);

	expr_lit_num->lex_end = lexer;

	if (!parse_expect(&lexer, &expr_lit_num->literal, TT_LIT_NUM, "number"))
		goto fail;

	*lex = lexer;
	*ret = expr_lit_num;
	return true;

fail:
	nodes_cnt = node_chkp;
	*ret = nullptr;
	return false;
}

extern bool
parse_stmt_ret(struct lex *lex, struct ast_node **ret)
{
	size_t node_chkp = nodes_cnt;
	struct lex lexer = *lex;
	struct ast_node *stmt_ret = ast_node_alloc(AST_NODE_STMT_RET, .lex_begin = lexer);

	if (!parse_expect(&lexer, &(struct token){}, TT_KW_RETURN, "return"))
		goto fail;

	if (!parse_expr_lit_num(&lexer, &stmt_ret->ret_val))
		goto fail;

	stmt_ret->lex_end = lexer;
	if (!parse_expect(&lexer, &(struct token){}, TT_SEMICOLON, ";"))
		goto fail;

	*lex = lexer;
	*ret = stmt_ret;
	return true;

fail:
	nodes_cnt = node_chkp;
	*ret = nullptr;
	return false;
}

extern bool
parse_decl_func(struct lex *lex, struct ast_node **ret)
{
	size_t node_chkp = nodes_cnt;
	struct lex lexer = *lex;
	struct ast_node *decl_func = ast_node_alloc(AST_NODE_DECL_FUNC, .lex_begin = lexer);

	if (!parse_expect(&lexer, &(struct token){}, TT_IDENT, "identifier"))
		goto fail;

	if (!parse_expect(&lexer, &decl_func->identifier, TT_IDENT, "identifier"))
		goto fail;

	if (!parse_expect(&lexer, &(struct token){}, TT_LPAREN, "(")) goto fail;
	if (!parse_expect(&lexer, &(struct token){}, TT_RPAREN, ")")) goto fail;
	if (!parse_expect(&lexer, &(struct token){}, TT_LBRACE, "{")) goto fail;

	if (!parse_stmt_ret(&lexer, &decl_func->stmt))
		goto fail;

	decl_func->lex_end = lexer;
	if (!parse_expect(&lexer, &(struct token){}, TT_RBRACE, "}"))
		goto fail;

	*lex = lexer;
	*ret = decl_func;
	return true;

fail:
	nodes_cnt = node_chkp;
	*ret = nullptr;
	return false;
}

