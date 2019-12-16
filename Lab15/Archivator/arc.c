#pragma comment(linker, "/STACK:64000000")
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include "cmd.h"
#include "arc.h"
#include "error.h"
#define MAXN 65536

unsigned char huffman[MAXN];

void compress(unsigned char* buffer, unsigned int real_size, unsigned int* size)
{
	int alphabet[256]; // первичный алфавит
	int freq[256]; // частоты
	int an = 0; // количество символов в первичном алфавите
	struct huffman_data
	{
		unsigned char symbol; // символ первичного алфавита
		unsigned char code[20]; // строка с битовым представлением символа
		int byte; // битовое представление
		int freq; // частота символа
		int parent; // родитель
		int children[2]; // потомки
	} data[512];

	// заполняем частоты нулями
	for (int i = 0; i < 256; i++)
		freq[i] = 0;

	for (int i = 0; i < real_size; i++) {
		unsigned char c = buffer[i];

		if (freq[c] == 0) // если символ встретился впервые
			alphabet[an++] = c; // заносим его в алфавит

		freq[c]++; // увеличиваем частоту символа
	}

	// начинаем построение дерева с заполнения его листьев
	for (int i = 0; i < an; i++) {
		data[i].symbol = alphabet[i];
		data[i].freq = freq[alphabet[i]];
		data[i].parent = -1;
		data[i].children[0] = -1;
		data[i].children[1] = -1;
		data[i].code[0] = '\0';
	}

	// достраиваем дерево, не хватает an - 1 узлов
	for (int i = an; i < an * 2 - 1; i++) {
		data[i].symbol = '-';
		data[i].parent = -1;
		data[i].children[0] = -1;
		data[i].children[1] = -1;
		data[i].freq = 0;
		data[i].code[0] = '\0';

		// пытаемся найти двух потомков с минимальной частотой
		for (int k = 0; k < 2; k++) {
			int min = real_size;
			// для всех достроенных узлов дерева, то есть меньше i
			for (int j = 0; j < i; j++) {
				// если частота меньше и родителя у узла ещё нет
				if (min > data[j].freq&& data[j].parent == -1) {
					// запоминаем частоту и сохраняем индекс потомка
					min = data[j].freq;
					data[i].children[k] = j;
				}
			}
			// частота узла будет равна сумме частот потомков
			data[i].freq += min;
			// устанавливаем родителя для потомка
			data[data[i].children[k]].parent = i;
		}
	}

	// дерево построено, начинаем построение битовых цепочек
	strcpy(data[an * 2 - 2].code, "1");
	int found = 1;
	int bad = 0;
	// выполняем, пока находим узел, у которого не заполнен битовый код
	while (found) {
		found = 0;
		// перебираем все узлы дерева
		for (int i = 0; i < an * 2 - 1; i++) {
			// если текущий код пустой, а у родителя код уже известен
			if (!strlen(data[i].code) && strlen(data[data[i].parent].code)) {
				// копируем код родителя
				strcpy(data[i].code, data[data[i].parent].code);
				// и добавляем ноль, если данный узел является левым, один, если он правый
				int len = strlen(data[i].code);
				data[i].code[len] = (data[data[i].parent].children[0] == i) ? '0' : '1';
				data[i].code[len + 1] = '\0';
				// для листьев дерева считаем байт-код на основании строковой последовательности из нулей и единиц
				if (i < an) {
					data[i].byte = 0;
					for (int j = 0; j <= len; j++) {
						data[i].byte *= 2;
						data[i].byte += data[i].code[j] == '0' ? 0 : 1;
					}
				}
				found = 1;
			}
		}
	}

	// начинаем заполнение сжатого буфера, сохраняем алфавит
	unsigned int h = 0;
	// записываем количество символов в алфавите
	huffman[h++] = an - 1;
	for (int i = 0; i < an; i++) {
		huffman[h++] = data[i].symbol;
		huffman[h++] = data[i].byte % 256;
		huffman[h++] = data[i].byte / 256;
	}

	unsigned char s[100];
	s[0] = '\0';
	// кодируем все символы из буфера
	for (int i = 0; i < real_size; i++) {
		unsigned char c = buffer[i];
		// ищем подходящий символ
		for (int j = 0; j < an; j++) {
			if (data[j].symbol == c) {
				int len = strlen(s);
				// копируем код без ведущей единицы в строку s
				strcpy(s + len, data[j].code + 1);
				len = strlen(s);
				// пока длина строки достаточна для формирования байта
				while (len >= 8) {
					// вычисляем байт
					unsigned char byte = 0;
					for (int k = 0; k < 8; k++) {
						byte *= 2;
						byte += s[k] == '0' ? 0 : 1;
					}
					// сохраняем в сжатый буфер
					huffman[h++] = byte;
					strcpy(s, s + 8);
					len = strlen(s);
				}
				break;
			}
		}
	}
	// дописываем остаток
	unsigned char byte = 0;
	int len = strlen(s);
	if (len > 0) {
		for (int j = 0; j < len; j++) {
			byte *= 2;
			byte += s[j] == '0' ? 0 : 1;
		}
		for (int j = 0; j < 8 - len; j++)
			byte *= 2;

		huffman[h++] = byte;
	}

	// копируем сжатое сообщение в буфер
	for (int i = 0; i < h; i++)
		buffer[i] = huffman[i];

	*size = h;
}

void decompress(unsigned char* buffer, unsigned int size, unsigned int real_size)
{
	int h = 0;
	int alphabet[256]; // первичный алфавит
	int byte[256]; // первичный алфавит
	int an = buffer[h++] + 1; // количество символов в алфавите
	for (int i = 0; i < an; i++) {
		alphabet[i] = buffer[h++];
		byte[i] = buffer[h++];
		byte[i] += buffer[h++] * 256;
	}
	char s[100];
	int len = 0;
	int x = 1;
	int y = 0;

	while (h < size && y < real_size) {
		unsigned char c = buffer[h++];
		for (int i = 7; i >= 0 && y < real_size; i--) {
			int p = (c & (1 << i)) != 0;
			x = x * 2 + p;
			for (int j = 0; j < an; j++) {
				if (byte[j] == x) {
					huffman[y++] = alphabet[j];
					x = 1;
				}
			}
		}
	}

	for (int i = 0; i < y; i++)
		buffer[i] = huffman[i];
}

void create(struct cmd_data* data)
{
	printf("Creating archive...\n");

	struct arc_header arc;
	unsigned int size;
	unsigned int real_size;
	unsigned char buffer[MAXN];

	strncpy(arc.signature, "AIV+", 4);
	arc.count = 0;
	for (int i = 0; i < 100; i++)
		arc.size[i] = 0;

	for (int i = 0; i < data->count; i++) {
		FILE* fr = fopen(data->files[i], "rb+");
		if (fr) {
			fseek(fr, 0, SEEK_END);
			arc.size[arc.count] = ftell(fr);
			strcpy(arc.filename[arc.count], data->files[i]);
			fclose(fr);
			arc.count++;
		}
	}

	FILE* f = fopen(data->arc_name, "wb+");
	check(!f, ERR_ARCCREATE, "Could not create archive file");

	real_size = sizeof(struct arc_header);
	memcpy(buffer, &arc, real_size);
	compress(buffer, real_size, &size);
	fwrite(&size, sizeof(size), 1, f);
	fwrite(&real_size, sizeof(real_size), 1, f);
	fwrite(buffer, size, 1, f);

	for (int i = 0; i < arc.count; i++) {
		if (arc.size[i] > 0) {
			FILE* fr = fopen(arc.filename[i], "rb+");
			check(!fr, ERR_ARCCREATE, "Could not copy file into archive");

			while (real_size = fread(buffer, 1, 32768, fr)) {
				compress(buffer, real_size, &size);
				fwrite(&size, sizeof(size), 1, f);
				fwrite(&real_size, sizeof(real_size), 1, f);
				fwrite(buffer, 1, size, f);
			}

			fclose(fr);
		}
	}
	fclose(f);
}

void list(struct cmd_data* data)
{
	printf("Listing files of archive...\n");

	struct arc_header arc;
	unsigned int size;
	unsigned int real_size;
	unsigned char buffer[MAXN];

	FILE* f = fopen(data->arc_name, "rb+");
	fread(&size, sizeof(size), 1, f);
	fread(&real_size, sizeof(real_size), 1, f);
	fread(buffer, size, 1, f);
	fclose(f);

	decompress(buffer, size, real_size);
	memcpy(&arc, buffer, real_size);

	check(strncmp(arc.signature, "AIV+", 4), ERR_ARCLIST, "Invalid archive signature");
	for (int i = 0; i < arc.count; i++)
		printf("%s [%d bytes]\n", arc.filename[i], arc.size[i]);
}

void extract(struct cmd_data* data)
{
	printf("Extracting files from archive...\n");

	struct arc_header arc;
	unsigned int size;
	unsigned int real_size;
	unsigned char buffer[MAXN];

	FILE* f = fopen(data->arc_name, "rb+");
	fread(&size, sizeof(size), 1, f);
	fread(&real_size, sizeof(real_size), 1, f);
	fread(buffer, size, 1, f);
	decompress(buffer, size, real_size);
	memcpy(&arc, buffer, real_size);

	check(strncmp(arc.signature, "AIV+", 4), ERR_ARCEXTRACT, "Invalid archive signature");
	for (int i = 0; i < arc.count; i++) {
		printf("%s [%d bytes]\n", arc.filename[i], arc.size[i]);

		FILE* fw = fopen(arc.filename[i], "rb+");
		check((int) fw, ERR_ARCEXTRACT, "File exists. Please extract arhive in empty directory.");

		fw = fopen(arc.filename[i], "wb+");
		check(!fw, ERR_ARCEXTRACT, "Could not create file");

		int left = arc.size[i];
		while (left > 0) {
			fread(&size, sizeof(size), 1, f);
			fread(&real_size, sizeof(real_size), 1, f);
			fread(buffer, size, 1, f);
			left -= real_size;

			decompress(buffer, size, real_size);
			fwrite(buffer, 1, real_size, fw);
		}

		fclose(fw);
	}
	fclose(f);
}

void arc_cmd(struct cmd_data* data)
{
	switch (data->cmd) {
	case CMD_CREATE: {
		create(data);
		break;
	}
	case CMD_LIST: {
		list(data);
		break;
	}
	case CMD_EXTRACT: {
		extract(data);
		break;
	}
	}
}
