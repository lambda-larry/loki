#include "loki.h"
#include "lex.h"
#include "qbe.h"

#include <stdio.h>
#include <inttypes.h>

#define sv_c(x) (struct string){.s = x, .length = strlen(x), }

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

		 {
			 struct qbe_value val = {
				 .kind  = QBE_VALUE_KIND_IDENT,
				 .type  = QBE_BT_WORD,
				 .ident = sv_c(".0"),
			 };
			 // TODO: Use own int parser rather than using scanf and friends
			 i64 ret_val = 0;
			 sscanf(ast_func->stmt[0].ret_val->literal.sv.s, "%"SCNi64, &ret_val);
			 struct qbe_inst inst = {
				 .kind = QBE_INST_KIND_COPY,
				 .dst  = val,
				 .src  = {
					 .kind         = QBE_VALUE_KIND_DECIMAL,
					 .type         = QBE_BT_WORD,
					 .constant.i64 = ret_val,
				 },
			 };

			 qbe_da_append(&block.inst, inst);

			 block.jmp.value = val;
		 }


		 qbe_da_append(&func.body, block);
		 break;
	}

	qbe_da_append(&module.funcs, func);

	// }}}

	// {{{ emit asm
	FILE *f = stdout;
	for (size_t i = 0; i < module.funcs.count; i++) {
		const struct qbe_function *func = &module.funcs.data[i];

		fprintf(f,
			"export function %c $%.*s()\n",
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
				case QBE_INST_KIND_COPY:
					fprintf(f, "\t%%%.*s =%c copy ",
						(int)inst->dst.ident.length, inst->dst.ident.s,
						(int)inst->dst.type
					);
					switch (inst->src.kind) {
					case QBE_VALUE_KIND_IDENT:
						fprintf(f, "%c%.*s",
							inst->src.global ? '$' : '%', (int)inst->src.ident.length, inst->src.ident.s
						);
						break;
					case QBE_VALUE_KIND_DECIMAL:
						fprintf(f, "%"PRId64, inst->src.constant.i64);
						break;
					case QBE_VALUE_KIND_DP_FP:
						fprintf(f, "%f", inst->src.constant.f32);
						break;
					case QBE_VALUE_KIND_SP_FP:
						fprintf(f, "%lf", inst->src.constant.f64);
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
						fprintf(f, "%c%.*s",
							val->global ? '$' : '%', (int)val->ident.length, val->ident.s
						);
						break;
					case QBE_VALUE_KIND_DECIMAL:
						fprintf(f, "%"PRId64, val->constant.i64);
						break;
					case QBE_VALUE_KIND_DP_FP:
						fprintf(f, "%f", val->constant.f32);
						break;
					case QBE_VALUE_KIND_SP_FP:
						fprintf(f, "%lf", val->constant.f64);
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
