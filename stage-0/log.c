#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>
#include <time.h>
#include <inttypes.h>

#define CSI "\033["

#define VT_RESET          CSI "0m"
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


#define VT_BLACK_BG          CSI "40m"
#define VT_RED_BG            CSI "41m"
#define VT_GREEN_BG          CSI "42m"
#define VT_YELLOW_BG         CSI "43m"
#define VT_BLUE_BG           CSI "44m"
#define VT_MAGENTA_BG        CSI "45m"
#define VT_CYAN_BG           CSI "46m"
#define VT_WHITE_BG          CSI "47m"
#define VT_BRIGHT_BLACK_BG   CSI "100m"
#define VT_BRIGHT_RED_BG     CSI "101m"
#define VT_BRIGHT_GREEN_BG   CSI "102m"
#define VT_BRIGHT_YELLOW_BG  CSI "103m"
#define VT_BRIGHT_BLUE_BG    CSI "104m"
#define VT_BRIGHT_MAGENTA_BG CSI "105m"
#define VT_BRIGHT_CYAN_BG    CSI "106m"
#define VT_BRIGHT_WHITE_BG   CSI "107m"

// clear(3) dumps this sequence on xterm
#define VT_CLEAR     CSI "H" CSI "J" CSI "3J"
#define VT_BOLD      CSI "1m"
#define VT_ITALIC    CSI "3m"
#define VT_UNDERLINE CSI "4m"
#define VT_BLINK     CSI "5m"
#define VT_STRIKE    CSI "9m"

enum log_level
{
	TODO,
	DEBUG,
	TRACE,
	INFO,
	WARN,
	ERROR,
	FATAL,
};

extern void log_init(FILE *out);
extern void log_fini(void);

static struct logger {
	FILE *out;
	bool no_color;
} g_log;

extern void
log_init(FILE *out)
{
	g_log.out = out;
}

extern void
log_fini()
{
	fclose(g_log.out);
}

static void
_log(enum log_level level, const char *restrict fmt, va_list args)
{
	int n = 0;
	char buffer[4096];

	buffer[n] = 0;

	{
		struct tm tm = {0};
		struct timespec ts;
		timespec_get(&ts, TIME_UTC);

		// Included in POSIX and C23
		extern struct tm *gmtime_r(const time_t *restrict timep, struct tm *restrict result);

		gmtime_r((time_t *)&ts.tv_sec, &tm);

		n += strftime(buffer + n, sizeof buffer - n, "[%Y-%m-%d %H:%M:%S", &tm);
		n += snprintf(buffer + n, sizeof buffer - n, ".%03"PRIu64"]", ts.tv_nsec / 1000000);
	}

	if (g_log.no_color) {
		static const char *log_level_str[] = {
			[TODO]  = "TODO",
			[DEBUG] = "DEBUG",
			[TRACE] = "TRACE",
			[INFO]  = "INFO",
			[WARN]  = "WARN",
			[ERROR] = "ERROR",
			[FATAL] = "FATAL",
		};
		n += snprintf(buffer + n, sizeof buffer - n, "[%s]", log_level_str[level]);
	} else {
		static const char *log_level_str[] = {
			[TODO]  = VT_CYAN "TODO",
			[DEBUG] = VT_CYAN "DEBUG",
			[TRACE] = VT_BRIGHT_BLACK "TRACE",
			[INFO]  = VT_BRIGHT_GREEN "INFO",
			[WARN]  = VT_BRIGHT_YELLOW "WARN",
			[ERROR] = VT_RED "ERROR",
			[FATAL] = VT_WHITE VT_RED_BG VT_BLINK "FATAL",
		};
		n += snprintf(buffer + n, sizeof buffer - n, "[%s" VT_RESET "]", log_level_str[level]);
	}

	if (n)
		buffer[n++] = ' ';
	buffer[n] = 0;
	n += vsnprintf(buffer + n, sizeof buffer - n, fmt, args);

	buffer[n++] = '\n';
	buffer[n]   = '\0';
	fwrite(buffer, 1, n, g_log.out);
}

extern void
log_todo(const char *restrict fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	_log(TODO, fmt, args);
	va_end(args);
}

extern void
log_debug(const char *restrict fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	_log(DEBUG, fmt, args);
	va_end(args);
}

extern void
log_trace(const char *restrict fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	_log(TRACE, fmt, args);
	va_end(args);
}

extern void
log_info(const char *restrict fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	_log(INFO, fmt, args);
	va_end(args);
}

extern void
log_warn(const char *restrict fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	_log(WARN, fmt, args);
	va_end(args);
}

extern void
log_error(const char *restrict fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	_log(ERROR, fmt, args);
	va_end(args);
}

extern void
log_fatal(const char *restrict fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	_log(FATAL, fmt, args);
	va_end(args);
}
