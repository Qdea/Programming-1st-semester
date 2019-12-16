#pragma comment(linker, "/STACK:64000000")
#define _CRT_SECURE_NO_WARNINGS
#include <string.h>
#include "error.h"
#include "cmd.h"

void cmd_get(int argc, char *argv[], struct cmd_data *data)
{
  data->cmd = CMD_NONE;
  data->count = 0;
  strcpy(data->arc_name, "");
  for(int i = 1; i < argc; i++) {
    if(!strcmp(argv[i], "--file")) {
      check(++i >= argc, ERR_WRONGARGS, "Expected filename for '--file' param");
      check(*argv[i] == '-', ERR_WRONGARGS, "Filename could not start with '-'");
      strcpy(data->arc_name, argv[i]);
    } else
    if(!strcmp(argv[i], "--create")) {
      check(data->cmd != CMD_NONE, ERR_WRONGARGS, "Expected one command only");
      data->cmd = CMD_CREATE;
      while(++i < argc) {
        check(*argv[i] == '-', ERR_WRONGARGS, "Filename could not start with '-'");
        check(data->count >= 100, ERR_WRONGARGS, "Could not archive more than 100 files");
        strcpy(data->files[data->count++], argv[i]);
      }
    } else
    if(!strcmp(argv[i], "--extract")) {
      check(data->cmd != CMD_NONE, ERR_WRONGARGS, "Expected one command only");
      data->cmd = CMD_EXTRACT;
    } else
    if(!strcmp(argv[i], "--list")) {
      check(data->cmd != CMD_NONE, ERR_WRONGARGS, "Expected one command only");
      data->cmd = CMD_LIST;
    } else {
      check(1, ERR_WRONGARGS, "Unknown command line parameter");
    }
  }
  int len = strlen(data->arc_name);
  check(data->cmd == CMD_NONE, ERR_WRONGARGS, "Please specify at least one command");
  check(!len, ERR_WRONGARGS, "You must specify filename of archive for any command");
  check(data->cmd == CMD_CREATE && data->count == 0, ERR_WRONGARGS, "Specify files to add them into archive");
  check(data->cmd == CMD_CREATE && len < 5, ERR_WRONGARGS, "Length of archive filename is too short");
  check(data->cmd == CMD_CREATE && strcmp(data->arc_name + len - 4, ".arc"), ERR_WRONGARGS, "Archive filename should end with '.arc'");
}
