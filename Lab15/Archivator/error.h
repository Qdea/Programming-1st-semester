#ifndef ERROR_H
#define ERROR_H

enum {
  ERR_WRONGARGS = 1,
  ERR_ARCCREATE = 2,
  ERR_ARCLIST = 3,
  ERR_ARCEXTRACT = 4
};

void check(int cond, int err, char* str1);

#endif
