#pragma comment(linker, "/STACK:64000000")
#include "arc.h"
#include "cmd.h"

void main(int argc, char *argv[])
{
  struct cmd_data data;

  cmd_get(argc, argv, &data);
  arc_cmd(&data);
}
