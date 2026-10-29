#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#include "lex.h"

#define RET_CAP(tok, t1, t2) (                                                                     \
	(lex->src_loc.column = begin - lex->cursor.line_marker + 1),                               \
	(*ret = (struct token){                                                                    \
		.type = tok,                                                                       \
		.line_offset     = lex->cursor.line_marker - (t1),                                 \
		.line_offset_end = lex->buf.length - (t1),                                         \
		.src_loc = lex->src_loc,                                                           \
		.sv = {.s = &lex->buf.s[(t1)], .length = (t2) - (t1)}                              \
	}), true)

#define RET(tok) RET_CAP(tok, begin, lex->cursor.pos)

#define NEW_LINE() do {                                                                            \
		lex->src_loc.line++;                                                               \
		lex->cursor.line_marker = lex->cursor.pos;                                         \
	} while (false)

extern bool
lex(struct lex *lex, struct token *ret)
{
	if (0 == lex->buf.length) {
		return false;
	}

	/*!re2c
	  re2c:yyfill:enable = 0;
	  re2c:case-ranges   = 1;
	  re2c:eof           = 0;
	  re2c:api           = generic;
	  re2c:api:style     = free-form;
	  re2c:encoding:utf8 = 1;
	  re2c:tags          = 1;
	  re2c:define:YYCTYPE     = "uint8_t";
	  re2c:define:YYPEEK      = "lex->buf.s[lex->cursor.pos]";
	  re2c:define:YYSKIP      = "++lex->cursor.pos;";
	  re2c:define:YYBACKUP    = "marker = lex->cursor.pos;";
	  re2c:define:YYRESTORE   = "lex->cursor.pos = marker;";
	  re2c:define:YYLESSTHAN  = "lex->buf.length - lex->cursor.pos < @@{len}";
	  re2c:define:YYSTAGP     = "@@{tag} = lex->cursor.pos;";
	  re2c:define:YYSHIFTSTAG = "@@{tag} += @@{shift};";
	 */

	int32_t marker = 0;

lex_begin_line:
/*!stags:re2c format = '\ttypeof (lex->cursor.pos) @@ = {}; (void)@@;\n'; */

	// NOTE: Meta directive to change line number and filename kinda like
	// #line preprocessor directive in C. Not sure if I like to have this or not yet...
	if (lex->cursor.line_marker == lex->cursor.pos) {
		typeof (lex->cursor.pos) t1 = {}, t2 = {}, t3 = {}, t4 = {};
/*!re2c

		  "//! @lc_line " @t1 [0-9]+ @t2 " "+ @t3 [^ \x0A]+ @t4 " "* [\x0A] {
		  	uint32_t line = 0;
		  	for (int32_t pos = t1; pos < t2; pos++) {
				line *= 10;
				line += lex->buf.s[pos] - '0' + 00;
			}
			lex->src_loc.line = line;

		  	lex->src_loc.filename.s      = &lex->buf.s[t3];
		  	lex->src_loc.filename.length = t4 - t3;
			NEW_LINE();
			goto lex_begin_line;
		  }
		  @t1 * { lex->cursor.pos = t1; goto lex_start; }
		 */
	}

lex_start:
	int32_t begin = lex->cursor.pos;

	/*!re2c

	  [\x0A] { NEW_LINE(); goto lex_begin_line; }

	  [\x20\x09\x0B\x0C\x0D]+ { goto lex_start; }

	  "///".*[\x0A] { NEW_LINE(); goto lex_begin_line; }
	  "//".*[\x0A]  { NEW_LINE(); goto lex_begin_line; }
	  "/*"          {             goto lex_comment;    }

	  hex = [0-9a-fA-F];
	  dec = [0-9];
	  oct = [0-7];
	  bin = [0-1];

	      dec+("'"dec+)* { return RET(TT_LIT_NUM); }
	  "0x"hex+("'"hex+)* { return RET(TT_LIT_NUM); }
	  "0b"bin+("'"bin+)* { return RET(TT_LIT_NUM); }
	  "0o"oct+("'"oct+)* { return RET(TT_LIT_NUM); }

	  $ { return false; }

	  * { return RET(TT_ERR) && false; }

	 */

lex_comment:
	int32_t comment_nesting = 1;
lex_comment_body:
	/*!re2c
	  // / (0x2F)
	  // * (0x2A)

	  [\x0A]       { NEW_LINE(); }
	  [\x2F][\x2A] { comment_nesting++;                      goto lex_comment_body;                      }
	  [\x2A][\x2F] { comment_nesting--; if (comment_nesting) goto lex_comment_body; else goto lex_start; }
	  *    { goto lex_comment_body; }
	  $    { return false; }


	 */

	return false;
}

// vim: set ft=c.re:
