// Do not modify this file. It will be replaced by the grading scripts
// when checking your project.

#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"

int
main(int argc, char *argv[])
{
  char buffer[513];
  printf(1, "%s", "** Placeholder program for grading scripts **\n");
  int fd = open("lokesh.txt", O_CREATE | O_RDWR | O_MIRRORED);
  printf(1, "\ntester.c : fd is %d\n", fd);
  printf (1, "\ntester.c : value returned by write is %d\n",write(fd, "Hello!", 6));
close(fd);
  fd = open("lokesh.txt", O_RDWR );
  printf (1, "\ntester.c : value returned by read is %d\n",read(fd, buffer, 6));
  printf (1, "\ntester.c: Data read back is *%s*\n", buffer);
  exit();
}
