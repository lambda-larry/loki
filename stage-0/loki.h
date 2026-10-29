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

extern void log_todo(const  char *restrict fmt, ...);
extern void log_debug(const char *restrict fmt, ...);
extern void log_trace(const char *restrict fmt, ...);
extern void log_info(const  char *restrict fmt, ...);
extern void log_warn(const  char *restrict fmt, ...);
extern void log_error(const char *restrict fmt, ...);
extern void log_fatal(const char *restrict fmt, ...);


extern bool file_load_string(const char *filename, struct string *out);

#endif
