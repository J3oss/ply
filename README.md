# PLY Importer

A simple PLY (Polygon File Format) importer written in C.

## Features
- Supports ASCII and binary (little-endian) PLY formats
- Configurable 4-byte alignment for elements
- **Does not support list properties**

## Installation
### Dependencies
- A C compiler (GCC, Clang, or MSVC)
- [vector](https://github.com/J3oss/vector)

## Usage

```c
#include "ply.h"

int main()
{
	struct ply_options op = { .align_4byte = true };
	struct ply ply = ply_import("file.ply", op);
	ply_destory(ply);
	
	return 0;
}
```