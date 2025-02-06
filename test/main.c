#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "ply.h"

int main()
{
	struct ply_options op = {.align_4byte = true};

	struct ply ply_ascii = ply_import("cube_ascii.ply", op);
	struct ply ply_binary = ply_import("cube_little.ply", op);

	printf("loaded files\n");

	size_t element_count = cvec_size(&ply_ascii.elements);
	printf("file has %lld element \n", element_count);

	assert(memcmp(ply_ascii.data, ply_binary.data, ply_ascii.data_size) == 0);
	printf("ascii and binary contents are similar\n");

	ply_destory(ply_ascii);
	ply_destory(ply_binary);

	return 0;
}