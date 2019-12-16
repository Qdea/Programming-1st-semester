#ifndef CMD_H
#define CMD_H

enum {
  CMD_NONE = 0,
  CMD_CREATE = 1,
  CMD_EXTRACT = 2,
  CMD_LIST = 3
};

struct cmd_data {
  int cmd;
  int count;
  char arc_name[200];
  char files[100][200];
};

void cmd_get(int argc, char *argv[], struct cmd_data *data);

#endif
