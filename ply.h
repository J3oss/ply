#pragma once

#include "cvector/cvec.h"

#define ELEMENTMAXNAME 16

enum ply_format
{
	FORMAT_UNDEFINED,
	FORMAT_ASCII,
	FORMAT_BINARY_LITTLE_INDIAN,
	FORMAT_BINARY_BIG_INDIAN,
};

enum ply_type
{
	TYPE_UNDEFINED,
	TYPE_CHAR,
	TYPE_UCHAR,
	TYPE_SHORT,
	TYPE_USHORT,
	TYPE_INT,
	TYPE_UINT,
	TYPE_FLOAT,
	TYPE_DOUBLE,
};

struct ply_options
{
	bool align_4byte;
};

struct ply_element
{
	char name[ELEMENTMAXNAME];
	size_t count;
	size_t stride;

	cvec(enum ply_type) properties;

	void *data;
};

struct ply
{
	enum ply_format format;
	float version;

	cvec(struct ply_element) elements;
	int list_count;

	size_t data_size;
	void *data;
};

struct ply ply_import(const char *path, struct ply_options option);
void ply_destory(struct ply);