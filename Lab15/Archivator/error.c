#include <stdio.h>
#include <stdlib.h>
#include "error.h"

void check(int cond, int err, char *str1)
{
  if(cond) {
    switch(err) {
      case ERR_WRONGARGS: {
        printf("Program was terminated with error [%s]\n", str1);
        printf("Program usage:\n");
        printf("  arc --file data.arc --create a.txt b.bin c.bmp\n");
        printf("  arc --file data.arc --extract\n");
        printf("  arc --file data.arc --list\n");
        break;
      }
      case ERR_ARCCREATE: {
        printf("Fatal error during creating archive\n");
        printf("Program was terminated with error [%s]\n", str1);
        break;
      }
      case ERR_ARCLIST: {
        printf("Fatal error during listing archive\n");
        printf("Program was terminated with error [%s]\n", str1);
        break;
      }
      case ERR_ARCEXTRACT: {
        printf("Fatal error during extracting archive\n");
        printf("Program was terminated with error [%s]\n", str1);
        break;
      }
    }
    exit(err);
  }
}
