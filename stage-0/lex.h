#ifndef LEX_H
#define LEX_H

#include <stdint.h>
#include <stddef.h>

enum token_type : int16_t
{
	TT_NONE,
	TT_ERR,
	TT_LIT_NUM,
};

#ifndef lex_sv
struct lex_sv {
	size_t length;
	const char *s;
};
#endif

struct src_loc {
	struct lex_sv filename;
	uint32_t line;
	uint32_t column;
};

struct token {
	enum token_type type;
	int16_t line_offset;
	int32_t line_offset_end;
	struct lex_sv sv;

	struct src_loc src_loc;
};

struct lex {
	struct lex_sv buf;
	struct {
		int32_t pos;
		int32_t line_marker;
	} cursor;

	struct src_loc src_loc;
};

extern bool lex(struct lex *lex, struct token *ret);

#endif
