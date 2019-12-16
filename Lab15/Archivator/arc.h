#ifndef ARC_H
#define ARC_H
#include "cmd.h"
#pragma pack(push, 1)


struct arc_header {
  char signature[4];
  int count;
  int size[100];
  char filename[100][200];
};

#pragma pack(pop)

void arc_cmd(struct cmd_data *data);

#endif
