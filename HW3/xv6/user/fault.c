#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"
#include "fcntl.h"

#define assert(x) if (x) {} else { \
   printf(1, "%s: %d ", __FILE__, __LINE__); \
   printf(1, "assert failed (%s)\n", # x); \
   printf(1, "TEST FAILED\n"); \
   exit(); \
}

int
main(int argc, char *argv[])
{
  int* i = 0;
  int a = 99;
  int *b = &a;
  /*char *arg;
  int fd = open("tmp", O_WRONLY|O_CREATE);
  
  assert(fd != -1);
  printf(1, "File allocated successfully!");*/
  
  printf(1, "\nContent in b: %x",b);
  printf(1, "\nContent in a : %d", *b);
  printf(1, "\nContent in i : %x", *i);

  /*arg = (char*) 0x0;
  printf(1, "\nwrite with 0x00 returns : %d",(write(fd, arg, 10)));*/

  /* within null page */
  /*arg = (char*) 0x400;
  printf(1, "\nwrite with 0x400 returns : %d",(write(fd, arg, 1024)));
  //assert(write(fd, arg, 1024) == -1);*/

  /* spanning null page and code */
  /*arg = (char*) 0xfff;
  printf(1, "\nwrite with 0xfff returns : %d",(write(fd, arg, 2)));
  //assert(write(fd, arg, 2) == -1);*/
  exit();
}
