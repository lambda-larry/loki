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
				(void)inst;
			}

			switch (block->jmp.kind) {
			case QBE_JMP_KIND_JMP: return 2;
			case QBE_JMP_KIND_JNZ: return 3;
			case QBE_JMP_KIND_RET:
				if (0 == block->jmp.value.type && func->return_type.type)
					fprintf(f, "\tret %"PRIi64"\n", block->jmp.value.constant);
				else if (0 == block->jmp.value.type && 0 == func->return_type.type)
					fprintf(f, "\tret\n");
				else
					fprintf(f, "\tret\n");
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
