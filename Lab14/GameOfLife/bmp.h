#ifndef BMP_H
#define BMP_H
#pragma pack(push, 1)

struct bmp_header
{
	unsigned char type[2]; // Код 4D42h — Буквы 'BM'
	unsigned int size; // Размер файла в байтах
	unsigned char reserved[4]; // Резервные 4 байта
	unsigned int offset; // Смещение, где начинается изображение
};

struct bmp_info
{
	unsigned int size; // Размер заголовка BITMAP в байтах (40)
	int width; // Ширина изображения в пикселях
	int height; // Высота изображения в пикселях
	unsigned short planes; // Число плоскостей (не используется) = 1
	unsigned short bit_count; // Бит/пиксель
	unsigned int compression; // Тип сжатия (обычно не используется)
	unsigned int size_image; // Размер сжатого изображения в байтах
	int x_dpm; // Горизонтальное расширение, пиксель/метр
	int y_dmp; // Вертикальное расширение, пиксель/метр
	unsigned int clr_used; //	Кол-во используемых цветов (очень редко)
	unsigned int clr_important; // Кол-во "важных" цветов (не используется)
};

struct bmp
{
	struct bmp_header header; // заголовок файла
	struct bmp_info info; // bitmap информация
	unsigned char* palette; // палитра
	unsigned char* data; // данные (пиксели)
};

#pragma pack(pop)

void bmp_read(struct bmp* image, char* filename); // чтение из файла
void bmp_write(struct bmp* image, char* filename); // сохранение в файл
void bmp_clone(struct bmp* src, struct bmp* dest); // создание копии
int bmp_cmp(struct bmp* src, struct bmp* dest); // сравнение картинок
int bmp_get(struct bmp* image, int x, int y); // получить пиксель в координатах x, y
void bmp_set(struct bmp* image, int x, int y, int val); // установить значение пикселя в координатах x, y равное val
void bmp_free(struct bmp* image); // освободить память
#endif
