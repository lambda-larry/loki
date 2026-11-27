#include "loki.h"
#include "lex.h"
#include "qbe.h"

#include <stdio.h>
#include <inttypes.h>

#define sv_c(x) (struct string){.s = x, .length = sizeof (x) - 1, }

static bool
sv_parse_i64(struct string sv, i64 *ret)
{
	i64 val  = 0;
	i64 base = 10;

	enum
	{
		SEP = '\'',
	};

	const char *p = sv.s;
	const char *e = sv.s + sv.length;

	if (0 == sv.length)
		return false;

	switch (*p) {
	default:          return false;
	case '0' ... '0': goto parse_xxx;
	case '1' ... '9': goto parse_hex;
	}

parse_xxx:

	switch (*++p) {
	case 'x' ... 'x': base = 16; p++; goto parse_hex;
	case 'b' ... 'b': base = 2;  p++; goto parse_hex;
	case 'o' ... 'o': base = 8;  p++; goto parse_hex;
	case '0' ... '9': base = 10;      goto parse_hex;
	case SEP ... SEP: base = 10;      goto parse_hex;
	}

parse_hex:
	if (p == e)
		goto done;

	switch (*p) {
	case '0' ... '9': val *= base; val += 00 + *p++ - '0'; goto parse_hex;
	case 'a' ... 'f': val *= base; val += 10 + *p++ - 'a'; goto parse_hex;
	case 'A' ... 'F': val *= base; val += 10 + *p++ - 'A'; goto parse_hex;
	case SEP ... SEP: val *= 1;    val += 00 + *p++ - SEP; goto parse_hex;
	}

done:
	*ret = val;

	return true;
}


extern void
qbe_emit_linkage(FILE *f, struct qbe_linkage linkage)
{
	if (linkage.export)
		fprintf(f, "export ");

	if (linkage.thread)
		fprintf(f, "thread ");

	if (linkage.section_name.length) {
		fprintf(f, "section \"%.*s\"",
			(int)linkage.section_name.length, linkage.section_name.s
		);
		if (linkage.section_flag.length) {
			fprintf(f, " \"%.*s\"",
				(int)linkage.section_flag.length, linkage.section_flag.s
			);
		}
		fprintf(f, "\n");
	}
}

extern void
qbe_emit_value(FILE *f, struct qbe_value value)
{
	switch (value.kind) {
	case QBE_VALUE_KIND_IDENT:
		fprintf(f, value.ident.global ? "$" : "%%");
		fprintf(f, "%.*s",
			(int)value.ident.string.length, value.ident.string.s
		);
		break;
	case QBE_VALUE_KIND_DECIMAL:
		fprintf(f, "%"PRId64,   value.constant.i64);
		break;
	case QBE_VALUE_KIND_DP_FP:
		fprintf(f, "s_%"PRIf32, value.constant.f32);
		break;
	case QBE_VALUE_KIND_SP_FP:
		fprintf(f, "d_%"PRIf64, value.constant.f64);
		break;
	}
}

extern int
main(int argc, const char *argv[argc])
{
	extern size_t strlen(const char *);

	log_init(stderr);


	const char *filename = argc > 1 ? argv[1] : "test/test.loki";
	struct string content = {};

	file_load_string(filename, &content);


	// {{{ lexer

	struct lex lexer = {
		.buf = content,
		.src_loc.filename.s      = filename,
		.src_loc.filename.length = strlen(filename),
	};
	struct lex checkpoint = lexer;

#if 0
	struct token token = {};
	while (lex(&lexer, &token)) {
		printf("%.*s:%d:%d: %.*s\n",
			(int)token.src_loc.filename.length,
			token.src_loc.filename.s,
			token.src_loc.line,
			token.src_loc.column,
			(int)token.sv.length,
			token.sv.s
		);
		diag_info(token, "yes lit {}, kind %i", token.type);
	}
#endif
	lexer = checkpoint;

	// }}}

	// {{{ parser

	struct ast_node *ast_func = {};

	log_info("parsing");
	parse_decl_func(&lexer, &ast_func);

	// }}}

	// {{{ QBE

	log_info("emiting");
	struct module {
		qbe_da(struct qbe_function) funcs;
	};

	struct module module = {};

	struct qbe_function func = {
		.linkage.export   = true,
		.ident            = ast_func->identifier.sv,
		.return_type.type = QBE_ABI_WORD,
	};

	switch (ast_func->stmt[0].kind) {
	default: return 1;
	case AST_NODE_STMT_RET:
		 struct qbe_block block = {
			 .label    = sv_c("start"),
			 .jmp.kind = QBE_JMP_KIND_RET,
		 };

		 static struct qbe_value val[32] = {
			 [0] = qbe_value_ident(QBE_BT_WORD, ".0"),
			 [1] = qbe_value_ident(QBE_BT_WORD, ".1"),
			 [2] = qbe_value_ident(QBE_BT_WORD, ".2"),
			 [3] = qbe_value_ident(QBE_BT_WORD, ".3"),
			 [4] = qbe_value_ident(QBE_BT_WORD, ".4"),
		 };
		 struct qbe_inst inst_0 = {
			 .kind = QBE_INST_KIND_COPY,
			 .dst  = val[0],
			 .src  = qbe_value_decimal(QBE_BT_WORD, 0),
		 };
		 struct qbe_inst inst_1 = {
			 .kind = QBE_INST_KIND_COPY,
			 .dst  = val[1],
			 .src  = qbe_value_decimal(QBE_BT_WORD, 0),
		 };
		 struct qbe_inst inst_2 = {
			 .kind = QBE_INST_KIND_COPY,
			 .dst  = val[2],
			 .src  = qbe_value_decimal(QBE_BT_WORD, 0),
		 };
		 struct qbe_inst inst_3 = {
			 .kind = QBE_INST_KIND_ADD,
			 .dst  = val[3],
			 .lhs  = val[0],
			 .rhs  = val[1],
		 };
		 struct qbe_inst inst_4 = {
			 .kind = QBE_INST_KIND_MUL,
			 .dst  = val[4],
			 .lhs  = val[3],
			 .rhs  = val[2],
		 };
		 sv_parse_i64(ast_func->stmt[0].expr->lhs->lhs->literal.sv, &inst_0.src.constant.i64);
		 sv_parse_i64(ast_func->stmt[0].expr->lhs->rhs->literal.sv, &inst_1.src.constant.i64);
		 sv_parse_i64(ast_func->stmt[0].expr->rhs->literal.sv,      &inst_2.src.constant.i64);

		 qbe_da_append(&block.inst, inst_0);
		 qbe_da_append(&block.inst, inst_1);
		 qbe_da_append(&block.inst, inst_2);
		 qbe_da_append(&block.inst, inst_3);
		 qbe_da_append(&block.inst, inst_4);

		 block.jmp.value = val[4];


		 qbe_da_append(&func.body, block);
		 break;
	}

	qbe_da_append(&module.funcs, func);

	// }}}

	// {{{ emit asm
	FILE *f = stdout;
	for (size_t i = 0; i < module.funcs.count; i++) {
		const struct qbe_function *func = &module.funcs.data[i];

		qbe_emit_linkage(f, func->linkage);

		fprintf(f,
			"function %c $%.*s()\n",
			func->return_type.type,
			(int)func->ident.length, func->ident.s
		);
		fprintf(f, "{\n");
		for (size_t i = 0; i < func->body.count; i++) {
			const struct qbe_block *block = &func->body.data[i];
			fprintf(f, "@%.*s\n", (int)block->label.length, block->label.s);

			for (size_t i = 0; i < block->inst.count; i++) {
				const struct qbe_inst *inst = &block->inst.data[i];
				switch (inst->kind) {
				case QBE_INST_KIND_COMMENT:
					if (0 == inst->comment.length)
						break;

					fprintf(f, "\t# %.*s\n", (int)inst->comment.length, inst->comment.s);
					break;
				case QBE_INST_KIND_ADD:
					fprintf(f, "\t%%%.*s =%c add ",
						(int)inst->dst.ident.string.length, inst->dst.ident.string.s,
						(int)inst->dst.type
					);
					qbe_emit_value(f, inst->lhs);
					fprintf(f, ", ");
					qbe_emit_value(f, inst->rhs);
					fprintf(f, "\n");

					break;
				case QBE_INST_KIND_MUL:
					fprintf(f, "\t%%%.*s =%c mul ",
						(int)inst->dst.ident.string.length, inst->dst.ident.string.s,
						(int)inst->dst.type
					);
					qbe_emit_value(f, inst->lhs);
					fprintf(f, ", ");
					qbe_emit_value(f, inst->rhs);
					fprintf(f, "\n");

					break;
				case QBE_INST_KIND_NEG:
					fprintf(f, "\t%%%.*s =%c neg ",
						(int)inst->dst.ident.string.length, inst->dst.ident.string.s,
						(int)inst->dst.type
					);
					qbe_emit_value(f, inst->src);
					fprintf(f, "\n");
					break;
				case QBE_INST_KIND_COPY:
					fprintf(f, "\t%%%.*s =%c copy ",
						(int)inst->dst.ident.string.length, inst->dst.ident.string.s,
						(int)inst->dst.type
					);
					qbe_emit_value(f, inst->src);
					fprintf(f, "\n");
					break;
				}
			}

			switch (block->jmp.kind) {
			case QBE_JMP_KIND_JMP: return 2;
			case QBE_JMP_KIND_JNZ: return 3;
			case QBE_JMP_KIND_RET:
				if (0 == block->jmp.value.type && func->return_type.type)
					fprintf(f, "\tret %"PRIi64"\n", block->jmp.value.constant.i64);
				else if (0 == block->jmp.value.type && 0 == func->return_type.type)
					fprintf(f, "\tret\n");
				else {
					fprintf(f, "\tret ");
					qbe_emit_value(f, block->jmp.value);
					fprintf(f, "\n");
				}
				break;
			case QBE_JMP_KIND_HLT: return 4;
			}
		}
		fprintf(f, "}\n");
	}
	fclose(f);

	// }}}

	log_info("success");
	log_fini();

	return 0;
}
