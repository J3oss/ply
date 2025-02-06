#include <assert.h>
#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ply.h"

#define MIN(a, b) (a < b ? a : b)

enum ply_type parse_size(char *buffer)
{
	// keyword to match: char uchar, short, ushort, int, uint, float, double
	if (strncmp("char", buffer, 4) == 0)
	{
		return TYPE_CHAR;
	}
	else if (strncmp("uchar", buffer, 5) == 0)
	{
		return TYPE_UCHAR;
	}
	else if (strncmp("short", buffer, 5) == 0)
	{
		return TYPE_SHORT;
	}
	else if (strncmp("ushort", buffer, 6) == 0)
	{
		return TYPE_USHORT;
	}
	else if (strncmp("int", buffer, 3) == 0)
	{
		return TYPE_INT;
	}
	else if (strncmp("uint", buffer, 4) == 0)
	{
		return TYPE_UINT;
	}
	else if (strncmp("float", buffer, 5) == 0)
	{
		return TYPE_FLOAT;
	}
	else if (strncmp("double", buffer, 6) == 0)
	{
		return TYPE_DOUBLE;
	}
	else
	{
		return TYPE_UNDEFINED;
	}
}

size_t type_to_size(enum ply_type t)
{
	switch (t)
	{
	case TYPE_CHAR:
	case TYPE_UCHAR:
		return 1;
	case TYPE_SHORT:
	case TYPE_USHORT:
		return 2;
	case TYPE_INT:
	case TYPE_UINT:
	case TYPE_FLOAT:
		return 4;
	case TYPE_DOUBLE:
		return 8;
	default:
		assert(0);
		return 0;
	}
}

size_t get_word_length(char *buffer)
{
	char *tmp = buffer;
	while (isalpha(*tmp++))
	{
	};
	tmp--;

	return tmp - buffer;
}

void consume_white_space(char **buffer)
{
	while (isspace(*buffer[0]))
	{
		(*buffer)++;
	};
}

void consume_word(char **buffer)
{
	while (!isspace(*buffer[0]))
	{
		(*buffer)++;
	}

	consume_white_space(buffer);
}

char read_line(FILE *f, char **buffer)
{
	char c;
	fpos_t tmp;
	size_t size = 0;

	fgetpos(f, &tmp);
	while (1)
	{
		c = fgetc(f);
		size++;
		if (c == '\n' || c == EOF)
		{
			break;
		}
	}
	size++; // fgets need one additional space for \0

	*buffer = calloc(size, 1);

	fsetpos(f, &tmp);
	fgets(*buffer, size, f);

	return c;
}

// TODO support lists
void parse_list(struct ply *ply, char *buffer)
{
	ply->list_count++;
}

void parse_property(struct ply *ply, char *buffer)
{
	consume_word(&buffer); // consume property

	size_t i = cvec_size(&ply->elements) - 1;
	struct ply_element *e = ply->elements + i;

	// keyword to match: size keywords, list
	if (strncmp("list", buffer, 4) == 0)
	{
		parse_list(ply, buffer);
	}
	else if (parse_size(buffer) != TYPE_UNDEFINED)
	{
		enum ply_type t = parse_size(buffer);
		cvec_push(&e->properties, t);
		e->stride += type_to_size(t);
	}
	else
	{
		goto error;
	}
	return;

error:
	assert(0);
	return;
}

void parse_element(struct ply *ply, char *buffer)
{
	consume_word(&buffer); // consume element

	struct ply_element element = {0, 0, 0, 0};
	element.properties = new_cvec(enum ply_type, 0);

	size_t element_name = MIN(get_word_length(buffer), ELEMENTMAXNAME);
	memcpy(element.name, buffer, element_name);
	element.name[element_name] = '\n';
	consume_word(&buffer); // consume name

	element.count = strtol(buffer, &buffer, 10);
	if (element.count == 0)
	{
		goto error;
	}

	cvec_push(&ply->elements, element);
	return;

error:
	assert(0);
	return;
}

void parse_format(struct ply *ply, char *buffer)
{
	consume_word(&buffer); // consume format

	if (strncmp("ascii", buffer, 5) == 0)
	{
		ply->format = FORMAT_ASCII;
	}
	else if (strncmp("binary_big_endian", buffer, 17) == 0)
	{
		ply->format = FORMAT_BINARY_BIG_INDIAN;
	}
	else if (strncmp("binary_little_endian", buffer, 20) == 0)
	{
		ply->format = FORMAT_BINARY_LITTLE_INDIAN;
	}
	else
	{
		goto error;
	}
	consume_word(&buffer);

	ply->version = strtof(buffer, &buffer);
	return;

error:
	assert(0);
	return;
}

size_t calculate_total_data_size(struct ply *ply, struct ply_options options)
{
	size_t result = 0;

	size_t element_count = cvec_size(&ply->elements);
	for (size_t i = 0; i < element_count; i++)
	{
		struct ply_element element = ply->elements[i];
		result += element.stride * element.count;
		if (options.align_4byte)
		{
			size_t pad = (element.stride % 4) ? 4 - (element.stride % 4) : 0;
			result += pad * element.count;
		}
	}

	return result;
}

void parse_header(struct ply *ply, FILE *f)
{
	char *buffer;

	read_line(f, &buffer);
	if (strncmp("ply", buffer, 3) != 0)
	{
		goto error;
	}

	ply->elements = new_cvec(struct ply_element, 0);

	// keyword to match: comment, format, element, property, end_header
	while (read_line(f, &buffer) != EOF)
	{
		if (strncmp("comment", buffer, 7) == 0)
		{
			continue;
		}
		else if (strncmp("format", buffer, 6) == 0)
		{
			parse_format(ply, buffer);
		}
		else if (strncmp("element", buffer, 7) == 0)
		{
			parse_element(ply, buffer);
		}
		else if (strncmp("property", buffer, 8) == 0)
		{
			parse_property(ply, buffer);
		}
		else if (strncmp("end_header", buffer, 10) == 0)
		{
			break;
		}
		else
		{
			goto error;
		}

		free(buffer);
	}

	free(buffer);
	return;

error:
	free(buffer);
	assert(0);
	return;
}

void parse_data_ascii(struct ply *ply, FILE *f, struct ply_options options)
{
	size_t data_size = calculate_total_data_size(ply, options);
	ply->data_size = data_size;
	ply->data = calloc(data_size, 1);

	void *data = ply->data;
	size_t element_count = cvec_size(&ply->elements);
	for (size_t i = 0; i < element_count; i++)
	{
		struct ply_element e = ply->elements[i];
		for (size_t j = 0; j < e.count; j++)
		{
			char *buffer;
			read_line(f, &buffer);

			size_t property_count = cvec_size(&e.properties);
			for (size_t k = 0; k < property_count; k++)
			{
				enum ply_type t = e.properties[k];
				size_t s = type_to_size(t);

				switch (t)
				{
				case TYPE_CHAR:
				case TYPE_UCHAR:
				case TYPE_SHORT:
				case TYPE_USHORT:
				case TYPE_INT:
				case TYPE_UINT:
					long l = strtol(buffer, &buffer, 10);
					memcpy(data, &l, s);
					break;
				case TYPE_FLOAT:
					float f = strtof(buffer, &buffer);
					memcpy(data, &f, s);
					break;
				case TYPE_DOUBLE:
					double d = strtod(buffer, &buffer);
					memcpy(data, &d, s);
					break;
				default:
					break;
				}
				data += s;
			}

			if (options.align_4byte)
			{
				size_t pad = (e.stride % 4) ? 4 - (e.stride % 4) : 0;
				data += pad;
			}
		}
	}
}

void parse_data_ble(struct ply *ply, FILE *f, struct ply_options options)
{
	size_t data_size = calculate_total_data_size(ply, options);
	ply->data_size = data_size;
	ply->data = calloc(data_size, 1);

	size_t read_bytes = 0;
	void *data = ply->data;
	if (!options.align_4byte)
	{
		read_bytes = fread(data, 1, data_size, f);
	}
	else
	{
		size_t element_count = cvec_size(&ply->elements);
		for (size_t i = 0; i < element_count; i++)
		{
			struct ply_element e = ply->elements[i];
			size_t pad = (e.stride % 4) ? 4 - (e.stride % 4) : 0;
			for (size_t j = 0; j < e.count; j++)
			{
				read_bytes += fread(data, 1, e.stride, f);
				read_bytes += pad;

				data += (e.stride + pad);
			}
		}
	}

	assert(read_bytes == data_size);
}

struct ply ply_import(const char *path, struct ply_options options)
{
	struct ply ply = {0, 0, 0, 0, 0};

	FILE *f;
	fopen_s(&f, path, "rb");
	if (f == NULL)
	{
		goto error;
	}

	parse_header(&ply, f);
	assert(ply.list_count == 0);

	switch (ply.format)
	{
	case FORMAT_ASCII:
		parse_data_ascii(&ply, f, options);
		break;
	case FORMAT_BINARY_LITTLE_INDIAN:
		parse_data_ble(&ply, f, options);
		break;
	case FORMAT_BINARY_BIG_INDIAN:
	default:
		goto error;
		break;
	}

	fclose(f);
	return ply;

error:
	assert(0);
	return ply;
}

void ply_destory(struct ply ply)
{
	size_t element_count = cvec_size(&ply.elements);
	for (size_t i = 0; i < element_count; i++)
	{
		struct ply_element e = ply.elements[i];
		cvec_free(&e.properties);
	}

	cvec_free(&ply.elements);
	free(ply.data);
}