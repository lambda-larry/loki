#ifndef LOKI_H
#define LOKI_H

#include <stdint.h>
#include <stddef.h>

typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t  i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef float  f32;
typedef double f64;

struct string {
	size_t length;
	const char *s;
};
#define lex_sv string
#define qbe_sv string

#include "lex.h"

#include <stdio.h>

extern void log_init(FILE *out);
extern void log_fini();

extern void log_todo(const  char *restrict fmt, ...);
extern void log_debug(const char *restrict fmt, ...);
extern void log_trace(const char *restrict fmt, ...);
extern void log_info(const  char *restrict fmt, ...);
extern void log_warn(const  char *restrict fmt, ...);
extern void log_error(const char *restrict fmt, ...);
extern void log_fatal(const char *restrict fmt, ...);

extern void diag_info(struct token token, const char *fmt, ...);
extern void diag_warn(struct token token, const char *fmt, ...);
extern void diag_error(struct token token, const char *fmt, ...);

extern bool file_load_string(const char *filename, struct string *out);

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

	struct lex lex_begin;
	struct lex lex_end;

	struct token identifier;
	struct token literal;

	struct ast_node *stmt;
	struct ast_node *ret_val;

	struct ast_node *next;
	struct ast_node *prev;
};

extern bool parse_decl_func(struct lex *lex, struct ast_node **ret);
extern bool parse_stmt_ret(struct lex *lex, struct ast_node **ret);
extern bool parse_expr_lit_num(struct lex *lex, struct ast_node **ret);

#endif
