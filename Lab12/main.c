#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include "dirent.h"

char entries[1000][200];

void main(char argc, char* argv[])
{
	FILE* f;
	DIR* h;
	struct dirent* entry;
	int n = 0;

	h = opendir(".");
	while ((entry = readdir(h)) != NULL) {
		strcpy(entries[n++], entry->d_name);
	}
	closedir(h);


	for (int i = 1; i < argc; i++) {
		fclose(fopen(argv[i], "w"));
	}

	f = fopen("list.txt", "w");
	h = opendir(".");
	while ((entry = readdir(h)) != NULL) {
		int exists = 0;
		for (int i = 0; i < n; i++) {
			if (strcmp(entry->d_name, entries[i]) == 0) {
				exists = 1;
				break;
			}
		}
		if (exists == 0) {
			fprintf(f, "%s\n", entry->d_name);
		}
	}
	closedir(h);
	fclose(f);
}
