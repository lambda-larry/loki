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

#define VT_BLACK          CSI "30m"
#define VT_RED            CSI "31m"
#define VT_GREEN          CSI "32m"
#define VT_YELLOW         CSI "33m"
#define VT_BLUE           CSI "34m"
#define VT_MAGENTA        CSI "35m"
#define VT_CYAN           CSI "36m"
#define VT_WHITE          CSI "37m"
#define VT_BRIGHT_BLACK   CSI "90m"
#define VT_BRIGHT_RED     CSI "91m"
#define VT_BRIGHT_GREEN   CSI "92m"
#define VT_BRIGHT_YELLOW  CSI "93m"
#define VT_BRIGHT_BLUE    CSI "94m"
#define VT_BRIGHT_MAGENTA CSI "95m"
#define VT_BRIGHT_CYAN    CSI "96m"
#define VT_BRIGHT_WHITE   CSI "97m"


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



extern void print_token_line(enum diagnostic_level level, FILE *f, struct token token);

static void _diag(enum diagnostic_level level, struct token token, const char *fmt, va_list args)
{
	char buffer[2048];
	char out[4096];
	int n = vsnprintf(buffer, sizeof buffer, fmt, args);
	buffer[++n] = 0;

	int j = 0;

	switch (level) {
	case NOTE:  j += snprintf(out + j, sizeof out - j, VT_BRIGHT_CYAN "NOTE:");  break;
	case INFO:  j += snprintf(out + j, sizeof out - j, VT_BRIGHT_CYAN "INFO:");  break;
	case WARN:  j += snprintf(out + j, sizeof out - j, VT_YELLOW      "WARN:");  break;
	case ERROR: j += snprintf(out + j, sizeof out - j, VT_RED         "ERROR:"); break;
	case FATAL: j += snprintf(out + j, sizeof out - j, VT_RED         "FATAL:"); break;
	case HINT:  j += snprintf(out + j, sizeof out - j, VT_BRIGHT_CYAN "HINT:");  break;

	}
	j += snprintf(out + j, sizeof out - j, " "VT_RESET);

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

	print_token_line(level, stderr, token);
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

extern void print_token_line(enum diagnostic_level level, FILE *f, struct token token)
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
	switch (level) {
	case NOTE:  fprintf(f, VT_BRIGHT_CYAN); break;
	case INFO:  fprintf(f, VT_BRIGHT_CYAN); break;
	case WARN:  fprintf(f, VT_YELLOW);      break;
	case ERROR: fprintf(f, VT_RED);         break;
	case FATAL: fprintf(f, VT_RED);         break;
	case HINT:  fprintf(f, VT_BRIGHT_CYAN); break;
	}
	for (u32 i = 0; i < token.sv.length; i++) {
		fputc('^', f);
	}
	fprintf(f, VT_RESET);

	fputc('\n', f);

}
