#ifndef ERROR_H
#define ERROR_H

enum {
	ERR_OPENTOREAD = 1,
	ERR_OPENTOWRITE = 2,
	ERR_MONOCHROME = 3
};

void check(int cond, int err, char* str1);

#endif
