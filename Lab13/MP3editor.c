#define _CRT_SECURE_NO_WARNINGS 
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#pragma pack(1) 

struct id3header {
	unsigned char signature[3];
	unsigned char version;
	unsigned char subversion;
	unsigned char flags;
	unsigned char size[4];
};

struct id3tag {
	unsigned char id[4];
	unsigned char size[4];
	unsigned char flags[2];
};

struct metadata_item {
	unsigned char name[5];

	// 00 – ISO - 8859 - 1 (ASCII). 
	// 01 – UCS - 2 (UTF - 16 encoded Unicode with BOM), in ID3v2.2 and ID3v2.3. 
	// 02 – UTF - 16BE encoded Unicode without BOM, in ID3v2.4. 
	// 03 – UTF - 8 encoded Unicode, in ID3v2.4. 
	unsigned char encoding;
	unsigned char* value;
	int size;
};

struct metadata {
	struct metadata_item item[84];
	int count;
};

int getSize(unsigned char size[4], int bits)
{
	return (int)(size[3] + (size[2]<< bits) + (size[1]<<(bits * 2)) + (size[0] << (bits * 3)));
}

void show_header(struct id3header* header)
{
	printf("signature: %c%c%c\n", header->signature[0], header->signature[1], header->signature[2]);
	printf("version: %d.%d\n", header->version, header->subversion);
	printf("flags: a=%d b=%d c=%d\n", (header->flags>> 7) && 1, (header->flags >> 6) && 1, (header->flags >> 5) && 1);
	printf("size: %d\n\n", getSize(header->size, 7));
}

void error()
{
	printf("ERROR");
	exit(1);
}

void file_open(FILE** f, char* filepath)
{
	*f = fopen(filepath, "rb");
	if (!(*f)) error();
}

void read_header(FILE* f, struct id3header* header)
{
	fread(header, sizeof(struct id3header), 1, f);
}

void read_metadata(FILE* f, struct id3header* header, struct metadata* data)
{
	struct id3tag tag;

	int found = 1;
	data->count = 0;
	while (found) {
		struct metadata_item* item = &(data->item[data->count]);

		fread(&tag, sizeof(struct id3tag), 1, f);
		strncpy(item->name, tag.id, 4);
		*(item->name + 4) = '\0';
		item->size = getSize(tag.size, 7);
		if (item->size == 0) {
			found = 0;
		}
		else {
			item->value = malloc(item->size);
			if (item->value) {
				fread(&(item->encoding), 1, 1, f);
				fread(item->value, item->size - 1, 1, f);
				*(item->value + item->size - 1) = '\0';
				data->count++;
			}
			else {
				error();
				free(item);
			}
		}
	}

}

void show_metadata(struct metadata* data)
{
	for (int i = 0; i < data->count; i++) {
		struct metadata_item* item = &(data->item[i]);
		printf("%s %s\n", item->name, item->value);
	}
}

void save_metadata(char* filepath, struct id3header* header, struct metadata* data)
{
	struct id3tag tag;
	FILE* f;
	int total_size = 0;

	for (int i = 0; i < data->count; i++) {
		struct metadata_item* item = &(data->item[i]);
		total_size += 10;
		total_size += item->size;
	}
	if (getSize(header->size, 7) < total_size - 10) {
		error();
	}

	total_size = 0;
	f = fopen(filepath, "r+b");
	fwrite(header, sizeof(struct id3header), 1, f);
	for (int i = 0; i < data->count; i++) {
		struct metadata_item* item = &(data->item[i]);
		strncpy(tag.id, item->name, 4);
		int size = item->size;
		for (int j = 3; j >= 0; j--) {
			tag.size[j] = size & ((1 << 7) - 1);
			size = size >> 7;
		}
		tag.flags[0] = 0;
		tag.flags[1] = 0;
		fwrite(&tag, sizeof(struct id3tag), 1, f);
		fwrite(&(item->encoding), 1, 1, f);
		fwrite(item->value, item->size - 1, 1, f);
		total_size += 10;
		total_size += item->size;
	}
	int left_size = getSize(header->size, 7) - total_size;
	for (int i = 0; i < left_size; i++) {
		unsigned char c = 0;
		fwrite(&c, 1, 1, f);
	}
	fclose(f);
}

void set_value(struct metadata* data, char* name, char* value)
{
	for (int i = 0; i < data->count; i++) {
		struct metadata_item* item = &(data->item[i]);
		if (strcmp(item->name, name) == 0) {
			item->size = strlen(value) + 1;
			free(item->value);
			if (item->size > 0) {
				item->value = malloc(item->size);
				if (item->value) {
					strncpy(item->value, value, item->size - 1);
					*(item->value + item->size - 1) = '\0';
					return;
				}
				else {
					error();
				}
			}
			else {
				error();
			}
		}
	}
	struct metadata_item* item = &(data->item[data->count]);
	item->size = strlen(value) + 1;
	strcpy(item->name, name);
	item->encoding = 0;
	if (item->size > 0) {
		item->value = malloc(item->size);
		if (item->value && item->size > 0) {
			strncpy(item->value, value, item->size - 1);
			*(item->value + item->size - 1) = 0;
		}
		else {
			error();
		}
	}
	else {
		error();
	}
	data->count++;
}

void show_valueof(struct metadata* data, char* name)
{
	for (int i = 0; i < data->count; i++) {
		struct metadata_item* item = &(data->item[i]);
		if (strcmp(item->name, name) == 0) {
			printf("%s\n", item->value);
			break;
		}
	}
}

void free_metadata(struct metadata* data)
{
	for (int i = 0; i < data->count; i++) {
		struct metadata_item* item = &(data->item[i]);
		free(item->value);
	}
}

void main(char argc, char* argv[])
{
	FILE* f;
	struct id3header header;
	struct metadata data;
	enum { CMD_NONE = 0, CMD_GET = 1, CMD_SET = 2, CMD_SHOW = 3 };

	char* filepath = 0;
	char* name = 0;
	char* value = 0;
	int cmd = CMD_NONE;
	for (int i = 1; i < argc; i++) {
		if (strncmp(argv[i], "--filepath=", 11) == 0) {
			if (i + 1 < argc) {
				filepath = argv[i] + 11;
			}
			else {
				error();
			}
		}
		else if (strncmp(argv[i], "--value=", 8) == 0 && (cmd == CMD_NONE || cmd == CMD_SET)) {
			value = argv[i] + 8;
		}
		else if (strncmp(argv[i], "--get=", 6) == 0 && cmd == CMD_NONE) {
			cmd = CMD_GET;
			name = argv[i] + 6;
		}
		else if (strncmp(argv[i], "--set=", 6) == 0 && cmd == CMD_NONE) {
			cmd = CMD_SET;
			name = argv[i] + 6;
		}
		else if (strcmp(argv[i], "--show") == 0 && cmd == CMD_NONE) {
			cmd = CMD_SHOW;
		}
		else {
			error();
		}
	}

	if (filepath) {
		file_open(&f, filepath);
		read_header(f, &header);
		read_metadata(f, &header, &data);
		fclose(f);
	}
	else {
		error();
	}

	if (cmd == CMD_GET && name) {
		show_valueof(&data, name);
	}
	else if (cmd == CMD_SET && name && value) {
		set_value(&data, name, value);
		save_metadata(filepath, &header, &data);
	}
	else if (cmd == CMD_SHOW) {
		show_header(&header);
		show_metadata(&data);
	}
	else {
		error();
	}
  free_metadata(&data);
}
