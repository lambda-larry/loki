#include "loki.h"
#include "lex.h"

static bool
parse_peek(struct lex lexer, struct token *token)
{
	return lex(&lexer, token);
}

static bool
parse_advance(struct lex *lexer)
{
	return lex(lexer, &(struct token){});
}

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

extern bool parse_expr(struct lex *lex, int precedence, struct ast_node **ret);

extern bool
parse_expr_factor(struct lex *lex, struct ast_node **ret)
{
	size_t node_chkp = nodes_cnt;
	struct lex lexer = *lex;

	struct lex lex_end = lexer;

	struct ast_node *expr_ret = {};

	struct token token = {};
	if (!parse_peek(lexer, &token))
		goto fail;

	switch (token.type) {
	case TT_LPAREN:
		parse_advance(&lexer);
		if (false == parse_expr(&lexer, 0, &expr_ret))
			goto fail;
		if (!parse_expect(&lexer, &(struct token){}, TT_RPAREN, ")"))
			goto fail;
		break;
	case TT_LIT_NUM:
		parse_advance(&lexer);
		expr_ret = ast_node_alloc(AST_NODE_EXPR_LIT_NUM, .lex_begin = lexer);
		expr_ret->literal = token;
		break;
	case TT_PLUS:
		parse_advance(&lexer);
		if (false == parse_expr_factor(&lexer, &expr_ret))
			goto fail;
		break;
	case TT_MINUS:
		parse_advance(&lexer);
		expr_ret = ast_node_alloc(AST_NODE_EXPR_UN_NEG,  .lex_begin = lexer);
		if (false == parse_expr(&lexer, 0, &expr_ret->expr))
			goto fail;
		break;
	default: goto fail;
	}

	expr_ret->lex_end = lex_end;

	*lex = lexer;
	*ret = expr_ret;
	return true;

fail:
	nodes_cnt = node_chkp;
	*ret = nullptr;
	return false;
}

extern bool
parse_expr(struct lex *lex, int precedence, struct ast_node **ret)
{
	size_t node_chkp = nodes_cnt;
	struct lex lexer = *lex;

	struct lex lex_end = lexer;

	struct ast_node *lhs_expr = {};
	if (false == parse_expr_factor(&lexer, &lhs_expr))
		goto fail;

	static const bool is_op_bin[TT_LAST] = {
		[TT_PLUS]     = true,
		[TT_ASTERISK] = true,
	};

	static const int op_prec[TT_LAST] = {
		[TT_PLUS]     = 10,
		[TT_ASTERISK] = 20,
	};

	struct token token = {};
	parse_peek(lexer, &token);
	while (parse_peek(lexer, &token) && is_op_bin[token.type] && op_prec[token.type] >= precedence) {
		static const enum ast_node_kind tok_to_ast[TT_LAST] = {
			[TT_PLUS]     = AST_NODE_EXPR_BIN_ADD,
			[TT_ASTERISK] = AST_NODE_EXPR_BIN_MUL,
		};
		lhs_expr = ast_node_alloc(tok_to_ast[token.type], .lex_begin = lexer, .lhs = lhs_expr);

		parse_advance(&lexer);
		if (!parse_expr(&lexer, op_prec[token.type], &lhs_expr->rhs))
			break;
	}

	lhs_expr->lex_end = lex_end;

	*lex = lexer;
	*ret = lhs_expr;
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

	if (!parse_expr(&lexer, 0, &stmt_ret->expr))
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

