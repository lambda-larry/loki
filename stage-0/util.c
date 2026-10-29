#include "loki.h"

#include <stdio.h>
#include <stdlib.h>

extern bool
file_load_string(const char *filename, struct string *out)
{
	FILE *f = fopen(filename, "rb");

	fseek(f, 0, SEEK_END);

	out->length = ftell(f);
	char *p = malloc(out->length);
	out->s = p;

	fseek(f, 0, SEEK_SET);
	fread(p, out->length, 1, f);
	fclose(f);

	return true;
}
