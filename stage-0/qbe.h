#pragma once

// TODO: Missing PHI instruction
// TODO: Implement instructions

#include <stdbool.h>

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

#ifndef qbe_realloc
#define qbe_realloc realloc
extern void *realloc(void *p, size_t size);
#endif

#ifndef qbe_da

#define qbe_da(type, ...) struct __VA_ARGS__ {                                                      \
	size_t count;                                                                               \
	size_t capacity;                                                                            \
	type *data;                                                                                 \
}


#define qbe_da_append(type, list, item) do {                                                        \
	if (0 == (list)->capacity) {                                                                \
		(list)->capacity = 16;                                                              \
		(list)->data     = qbe_realloc((list)->data, sizeof (type) * (list)->capacity);     \
	}                                                                                           \
	if ((list)->count >= (list)->capacity) {                                                    \
		while ((list)->count >= (list)->capacity) {                                         \
			(list)->capacity *= 2;                                                      \
		}                                                                                   \
		(list)->data = qbe_realloc((list)->data, sizeof (type) * (list)->capacity);         \
	}                                                                                           \
	((type *)(list)->data)[(list)->count++] = (item);                                           \
} while (false)


#endif


#ifndef qbe_sv
struct qbe_sv {
	size_t len;
	const char *s;
};
#endif



enum qbe_type
{
	QBE_BT_NONE    = 0,

	QBE_BT_WORD    = 'w',
	QBE_BT_LONG    = 'l',
	QBE_BT_SHORT   = 's',
	QBE_BT_DOUBLE  = 'd',

	QBE_ET_NONE    = 0,

	QBE_ET_WORD    = 'w',
	QBE_ET_LONG    = 'l',
	QBE_ET_SHORT   = 's',
	QBE_ET_DOUBLE  = 'd',

	QBE_ET_BYTE    = 'b',
	QBE_ET_HALF    = 'h',

	QBE_SUB_NONE   = 0,

	QBE_SUB_WORD   = 'w',
	QBE_SUB_LONG   = 'l',
	QBE_SUB_SHORT  = 's',
	QBE_SUB_DOUBLE = 'd',

	QBE_SUB_BYTE   = 'b',
	QBE_SUB_HALF   = 'h',

	QBE_SUB_IDENT  = 'x',

	QBE_ABI_NONE   = 0,

	QBE_ABI_WORD   = 'w',
	QBE_ABI_LONG   = 'l',
	QBE_ABI_SHORT  = 's',
	QBE_ABI_DOUBLE = 'd',

	QBE_ABI_UBYTE  = 'b',
	QBE_ABI_UHALF  = 'h',
	QBE_ABI_SBYTE  = 'B',
	QBE_ABI_SHALF  = 'H',

	QBE_ABI_IDENT  = 'x',

};

struct qbe_type_struct_item {
	enum qbe_type type;
	i32 number;
	struct qbe_sv ident;
};

struct qbe_type_struct {
	struct qbe_sv name;
	i32 alignment;
	qbe_da(struct qbe_type_struct_item, qbe_type_struct_entries) items;
};

struct qbe_type_union {
	struct qbe_sv name;
	i32 alignment;
	qbe_da(struct qbe_type_struct_entries);
};

struct qbe_type_opaque {
	struct qbe_sv name;
	i32 alignment;
	i32 raw_size;
};

extern void qbe_typedef_struct_begin();
extern void qbe_typedef_struct_append();
extern void qbe_typedef_struct_end();

extern void qbe_typedef_union_begin();
extern void qbe_typedef_union_append();
extern void qbe_typedef_union_end();

struct qbe_linkage {
	bool export;
	bool thread;
	struct qbe_sv section_name;
	struct qbe_sv section_flag;
};

enum qbe_data_kind
{
	QBE_DATA_KIND_IDENT,
	QBE_DATA_KIND_STR,
	QBE_DATA_KIND_CONST,
};

struct qbe_data_item {
	enum qbe_type      type;
	enum qbe_data_kind kind;
	union {
		struct {
			struct qbe_sv ident;
			i32 offset;
		};
		struct qbe_sv string;
		i32 constant;
	};
};

struct qbe_data {
	struct qbe_linkage linkage;
	struct qbe_sv name;
	i32 alignment;
	i32 raw_size;

	qbe_da(struct qbe_data_item);
};

struct qbe_abitype {
	enum qbe_type type;
	struct qbe_sv ident;
};

struct qbe_param {
	struct qbe_abitype arg;
	struct qbe_sv      ident;
};

struct qbe_block;

struct qbe_function {
	struct qbe_linkage linkage;
	struct qbe_abitype return_type;

	struct qbe_sv ident;
	bool variadic;
	bool env;

	qbe_da(struct qbe_param) params;
	qbe_da(struct qbe_block) body;
};

enum qbe_block_kind
{
	QBE_BLOCK_KIND_LABEL,
	QBE_BLOCK_KIND_INST,
	QBE_BLOCK_KIND_JMP,
};

enum qbe_jmp_kind
{
	QBE_JMP_KIND_JMP,
	QBE_JMP_KIND_JNZ,
	QBE_JMP_KIND_RET,
	QBE_JMP_KIND_HLT,
};

struct qbe_value {
	enum qbe_type type;
	struct qbe_sv ident;
};

struct qbe_jmp {
	enum qbe_jmp_kind kind;
	struct qbe_value value;

	struct qbe_sv success;
	struct qbe_sv failure;
};

struct qbe_inst;

struct qbe_block {
	enum qbe_block_kind kind;
	union {
		struct qbe_sv label;
		struct qbe_jmp jmp;
		qbe_da(struct qbe_inst) inst;
	};
};

enum qbe_inst_kind
{
	QBE_INST_KIND_ADD,
};

struct qbe_inst {
	int i;
};
