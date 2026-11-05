#include <stdint.h>
#include <stdarg.h>

#include <stdio.h>

#include "lex.h"

typedef int8_t  i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

#define CSI "\033["

#define VT_RESET     CSI "0m"
#define VT_BOLD      CSI "1m"
#define VT_ITALIC    CSI "3m"
#define VT_UNDERLINE CSI "4m"
#define VT_BLINK     CSI "5m"
#define VT_STRIKE    CSI "9m"

enum diagnostic_level {
	NOTE,
	INFO,
	WARN,
	ERROR,
	FATAL,
	HINT,
};

struct diagnostic {
	enum diagnostic_level level;
	int32_t narg;
	const char *msg;
	void *args[8];

};



extern void print_token_line(FILE *f, struct token token);

static void _diag(enum diagnostic_level level, struct token token, const char *fmt, va_list args)
{
	char buffer[2048];
	char out[4096];
	int n = vsnprintf(buffer, sizeof buffer, fmt, args);
	buffer[++n] = 0;

	int j = 0;
	for (int i = 0; i < n - 1; i++) {
		if ('{' == buffer[i + 0] && '}' == buffer[i + 1]) {
			j += snprintf(out + j, sizeof out - j, "%.*s", (int)token.sv.length, token.sv.s);
			i += 1;
		} else {
			out[j++] = buffer[i];
		}
	}
	out[j++] = '\n';
	out[j]   = 0;

	fprintf(
		stderr,
		VT_BOLD "%.*s:%i:" VT_RESET
		" %.*s",
		(int)token.src_loc.filename.length,
		token.src_loc.filename.s,
		token.src_loc.line + 1,
		j,
		out
	);

	print_token_line(stderr, token);
}

extern void diag_error(struct token token, const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	_diag(ERROR, token, fmt, args);
	va_end(args);
}

extern void diag_warn(struct token token, const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	_diag(WARN, token, fmt, args);
	va_end(args);
}

extern void diag_info(struct token token, const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	_diag(INFO, token, fmt, args);
	va_end(args);
}

extern void print_token_line(FILE *f, struct token token)
{
	const char *line = &token.sv.s[token.line_offset];
	int line_len = 0;
	for (const char *p = line; '\n' != *p; p++, line_len++)
		;
	fprintf(f, "%.*s\n", line_len, line);
	for (u32 i = 0; i < token.src_loc.column - 1; i++) {
		switch (line[i]) {
		case '\t': fputc('\t', f); break;
		default:   fputc(' ',  f);  break;
		}
	}
	for (u32 i = 0; i < token.sv.length; i++) {
		fputc('^', f);
	}
	fputc('\n', f);

}
