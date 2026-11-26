#include "loki.h"
#include "lex.h"
#include "qbe.h"

#include <stdio.h>
#include <inttypes.h>

#define sv_c(x) (struct string){.s = x, .length = strlen(x), }

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
		if (linkage.section_flag.length) {
			fprintf(f, "section \"%.*s\"\n",
				(int)linkage.section_name.length, linkage.section_name.s
		       );
		} else {
			fprintf(f, "section \"%.*s\" \"%.*s\"\n",
				(int)linkage.section_name.length, linkage.section_name.s,
				(int)linkage.section_flag.length, linkage.section_flag.s
		       );
		}
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

		 struct qbe_value val_0 = {
			 .kind         = QBE_VALUE_KIND_IDENT,
			 .type         = QBE_BT_WORD,
			 .ident.string = sv_c(".0"),
		 };
		 struct qbe_value val_1 = {
			 .kind         = QBE_VALUE_KIND_IDENT,
			 .type         = QBE_BT_WORD,
			 .ident.string = sv_c(".1"),
		 };
		 struct qbe_inst inst_0 = {
			 .kind = QBE_INST_KIND_COPY,
			 .dst  = val_0,
			 .src  = {
				 .kind         = QBE_VALUE_KIND_DECIMAL,
				 .type         = QBE_BT_WORD,
				 .constant.i64 = 0,
			 },
		 };
		 struct qbe_inst inst_1 = {
			 .kind = QBE_INST_KIND_NEG,
			 .dst  = val_1,
			 .src  = val_0,
		 };
		 sv_parse_i64(ast_func->stmt[0].expr->expr->literal.sv, &inst_0.src.constant.i64);

		 qbe_da_append(&block.inst, inst_0);
		 qbe_da_append(&block.inst, inst_1);

		 block.jmp.value = val_1;


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
				case QBE_INST_KIND_ADD: return 6;
				case QBE_INST_KIND_NEG:
					fprintf(f, "\t%%%.*s =%c neg ",
						(int)inst->dst.ident.string.length, inst->dst.ident.string.s,
						(int)inst->dst.type
					);
					switch (inst->src.kind) {
					case QBE_VALUE_KIND_IDENT:
						fprintf(f, inst->src.ident.global ? "$" : "%%");
						fprintf(f, "%.*s",
							(int)inst->src.ident.string.length, inst->src.ident.string.s
						);
						break;
					case QBE_VALUE_KIND_DECIMAL:
						fprintf(f, "%"PRId64, inst->src.constant.i64);
						break;
					case QBE_VALUE_KIND_DP_FP:
						fprintf(f, "%"PRIf32, inst->src.constant.f32);
						break;
					case QBE_VALUE_KIND_SP_FP:
						fprintf(f, "%"PRIf64, inst->src.constant.f64);
						break;
					}
					fprintf(f, "\n");
					break;
				case QBE_INST_KIND_COPY:
					fprintf(f, "\t%%%.*s =%c copy ",
						(int)inst->dst.ident.string.length, inst->dst.ident.string.s,
						(int)inst->dst.type
					);
					switch (inst->src.kind) {
					case QBE_VALUE_KIND_IDENT:
						fprintf(f, inst->src.ident.global ? "$" : "%%");
						fprintf(f, "%.*s",
							(int)inst->src.ident.string.length, inst->src.ident.string.s
						);
						break;
					case QBE_VALUE_KIND_DECIMAL:
						fprintf(f, "%"PRId64, inst->src.constant.i64);
						break;
					case QBE_VALUE_KIND_DP_FP:
						fprintf(f, "%"PRIf32, inst->src.constant.f32);
						break;
					case QBE_VALUE_KIND_SP_FP:
						fprintf(f, "%"PRIf64, inst->src.constant.f64);
						break;
					}
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
					const struct qbe_value *val = &block->jmp.value;
					switch (val->kind) {
					case QBE_VALUE_KIND_IDENT:
						fprintf(f, val->ident.global ? "$" : "%%");
						fprintf(f, "%.*s",
							(int)val->ident.string.length, val->ident.string.s
						);
						break;
					case QBE_VALUE_KIND_DECIMAL:
						fprintf(f, "%"PRId64, val->constant.i64);
						break;
					case QBE_VALUE_KIND_DP_FP:
						fprintf(f, "%"PRIf32, val->constant.f32);
						break;
					case QBE_VALUE_KIND_SP_FP:
						fprintf(f, "%"PRIf64, val->constant.f64);
						break;
					}
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
