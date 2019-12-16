#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "bmp.h"
#include "error.h"

void bmp_read(struct bmp* image, char* filename)
{
	unsigned long size;
	FILE* f = fopen(filename, "rb");
	check(!f, ERR_OPENTOREAD, filename);

	fread(&(image->header), sizeof(struct bmp_header), 1, f);
	fread(&(image->info), sizeof(struct bmp_info), 1, f);
	check(image->info.bit_count != 1, ERR_MONOCHROME, 0);

	size = image->header.offset - sizeof(struct bmp_header) - sizeof(struct bmp_info);
	image->palette = malloc(size);
	fread(image->palette, size, 1, f);

	size = image->header.size - image->header.offset;
	image->data = malloc(size);
	fread(image->data, size, 1, f);

	fclose(f);
}

void bmp_write(struct bmp* image, char* filename)
{
	unsigned long size;
	FILE* f = fopen(filename, "wb+");
	check(!f, ERR_OPENTOWRITE, filename);

	fwrite(&(image->header), sizeof(struct bmp_header), 1, f);
	fwrite(&(image->info), sizeof(struct bmp_info), 1, f);

	size = image->header.offset - sizeof(struct bmp_header) - sizeof(struct bmp_info);
	fwrite(image->palette, size, 1, f);

	size = image->header.size - image->header.offset;
	fwrite(image->data, size, 1, f);

	fclose(f);
}

void pos(struct bmp* image, int x, int y, int* offset, int* bitnum)
{
	int bits = image->info.width & 31;
	bits = bits == 0 ? image->info.width : image->info.width - bits + 32;
	*offset = x + y * bits; // порядковый номер бита во всём массиве пикселей
	*bitnum = 7 - *offset & 7; // остаток от деления на 8, чтобы получить порядковый номер бита
	*offset = *offset >> 3; // делим на 8, чтобы получить номер байта
}

int bmp_get(struct bmp* image, int x, int y)
{
	int offset;
	int bitnum;
	pos(image, x, y, &offset, &bitnum);
	return (image->data[offset] & (1 << bitnum)) != 0;
}

void bmp_set(struct bmp* image, int x, int y, int val)
{
	int offset;
	int bitnum;
	pos(image, x, y, &offset, &bitnum);
	int old = (image->data[offset] & (1 << bitnum)) != 0;
	if (old != val) {
		image->data[offset] ^= (1 << bitnum);
	}
}

void bmp_clone(struct bmp* src, struct bmp* dest)
{
	dest->header = src->header;
	dest->info = src->info;

	int size = src->header.offset - sizeof(struct bmp_header) - sizeof(struct bmp_info);
	dest->palette = malloc(size);
	memcpy(dest->palette, src->palette, size);

	size = src->header.size - src->header.offset;
	dest->data = malloc(size);
	memcpy(dest->data, src->data, size);
}

int bmp_cmp(struct bmp* src, struct bmp* dest)
{
	int size = src->header.size - src->header.offset;
	for (int i = 0; i < size; i++) {
		if (src->data[i] < dest->data[i])
			return -1;
		if (src->data[i] > dest->data[i])
			return 1;
	}
	return 0;
}

void bmp_free(struct bmp* image) {
	free(image->palette);
	free(image->data);
}
