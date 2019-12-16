#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bmp.h"
#define BITS 3

int is_alive(struct bmp* image, int x, int y)
{
	int is_alive_count = 0;
	int w = image->info.width;
	int h = image->info.height;
	for (int i = -1; i <= 1; i++)
		if (i + x >= 0 && i + x < w)
			for (int j = -1; j <= 1; j++)
				if (j + y >= 0 && j + y < h)
					if (i != 0 || j != 0)
						is_alive_count += bmp_get(image, x + i, y + j);

	if (bmp_get(image, x, y))
		return is_alive_count >= 2 && is_alive_count <= 3 ? 1 : 0;
	else
		return is_alive_count == 3 ? 1 : 0;
}

void main(int argc, char* argv[])
{
	struct bmp image[1 << BITS];
	int exists[1 << BITS];
	int stop = 0;
	int curr = 0;
	int freq = 1;
	int maxi = 1000000;
	char filename[200];
	char dirname[200];
	strcpy(filename, "default.bmp");
	strcpy(dirname, "default");

	for (int i = 1; i < argc - 1; i++) {
		if (!strcmp(argv[i], "--input")) strcpy(filename, argv[i + 1]);
		if (!strcmp(argv[i], "--output")) strcpy(dirname, argv[i + 1]);
		if (!strcmp(argv[i], "--max_iter")) maxi = atoi(argv[i + 1]);
		if (!strcmp(argv[i], "--dump_freq")) freq = atoi(argv[i + 1]);
	}

	exists[0] = 1;
	for (int i = 1; i < (1 << BITS); i++)
		exists[i] = 0;

	bmp_read(&image[curr], filename);
	image[curr].palette[0] = 255;
	image[curr].palette[1] = 255;
	image[curr].palette[2] = 255;
	image[curr].palette[4] = 0;
	image[curr].palette[5] = 0;
	image[curr].palette[6] = 0;

	for (int i = 0; i < maxi && !stop; i++) {
		int next = (curr + 1) & ((1 << BITS) - 1);

		bmp_clone(&image[curr], &image[next]);
		for (int y = 0; y < image[curr].info.height; y++)
			for (int x = 0; x < image[curr].info.width; x++)
				bmp_set(&image[next], x, y, is_alive(&image[curr], x, y));

		if (i % freq == 0) {
			snprintf(filename, sizeof(filename), "%s/%d.bmp", dirname, i / freq);
			printf("%s\n", filename);
			bmp_write(&image[next], filename);
		}
		curr = next;
		exists[curr] = 1;

		for (int j = 0; j < (1 << BITS); j++)
			if (exists[j] && j != next && !bmp_cmp(&image[next], &image[j]))
				stop = 1;
	}

	for (int i = 0; i < (1 << BITS); i++)
		if (exists[i])
			bmp_free(&image[i]);
}
