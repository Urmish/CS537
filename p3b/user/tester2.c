// Do not modify this file. It will be replaced by the grading scripts
// when checking your project.

#include "types.h"
#include "stat.h"
#include "user.h"
//#include "defs.h"
#include "param.h"
#define PGSIZE 4096
int
main(int argc, char *argv[])
{
  printf(1, "%s", "** Placeholder program for grading scripts **\n");
  int *p=NULL;
  p = (int *)(USERTOP - 2*PGSIZE + 100);
  *p = 10;
  printf(1,"\nTester2 - p is %d",*p);
  *p = 10;
  printf(1,"\nTester2 - p is %d",*p);
  p = (int *)(USERTOP - 5*PGSIZE + 100);
  *p = 10;
  printf(1,"\nTester2 - p is ",*p);
  exit();
}
