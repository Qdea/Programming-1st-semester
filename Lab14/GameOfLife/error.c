#include <stdio.h>
#include <stdlib.h>
#include "error.h"

void check(int cond, int err, char* str1)
{
	if (cond) {
		switch (err) {
		case ERR_OPENTOREAD: {
			printf("Can not open file to read: %s\n", str1);
			break;
		}
		case ERR_OPENTOWRITE: {
			printf("Can not open file to write: %s\n", str1);
			break;
		}
		case ERR_MONOCHROME: {
			printf("Image should be monochrome\n");
			break;
		}
		}
		exit(err);
	}
}
