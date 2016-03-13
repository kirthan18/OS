#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"
#include "fcntl.h"

#define PGSIZE 4096

#define assert(x) if (x) {} else { \
   printf(1, "%s: %d ", __FILE__, __LINE__); \
   printf(1, "assert failed (%s)\n", # x); \
   printf(1, "TEST FAILED\n"); \
   exit(); \
}

void
test_failed()
{
	printf(1, "TEST FAILED\n");
	exit();
}

void
test_passed()
{
 printf(1, "TEST PASSED\n");
 exit();
}

int
main(int argc, char *argv[])
{
  /*int* i = 0;
  int a = 99;
  int *b = &a;
  char *arg;
  int fd = open("tmp", O_WRONLY|O_CREATE);
  
  assert(fd != -1);
  printf(1, "File allocated successfully!");
  
  printf(1, "\nContent in b: %x",b);
  printf(1, "\nContent in a : %d", *b);
  printf(1, "\nContent in i : %x", *i);

  arg = (char*) 0x0;
  printf(1, "\nwrite with 0x00 returns : %d",(write(fd, arg, 10)));

  
  arg = (char*) 0x400;
  printf(1, "\nwrite with 0x400 returns : %d",(write(fd, arg, 1024)));
  //assert(write(fd, arg, 1024) == -1);

  
  arg = (char*) 0x1100;
  printf(1, "\nwrite with 0x1100 returns : %d",(write(fd, arg, 2)));
  //assert(write(fd, arg, 2) == -1);

  printf(1,"\nShared mem count of page 0 : %d", shmem_count(0));
  printf(1,"\nShared mem count of page 1 : %d", shmem_count(1));
  printf(1,"\nShared mem count of page 2 : %d", shmem_count(2));
  printf(1,"\nShared mem count of page 3 : %d", shmem_count(3));
  printf(1,"\nShared mem count of page 4 : %d", shmem_count(4));
  printf(1,"\nShared mem count of page -1 : %d", shmem_count(-1));*/
  char *ptr;
  int i;
	char arr[6] = "CS537";
  int * phy_ptr = NULL;

	int pid = fork();
	if (pid < 0) {
		test_failed();
	}	
	else if (pid == 0) {
		ptr = shmem_access(3);
                printf(1, "\nMemory allocated : %p", ptr);
		if (ptr == NULL) {
			test_failed();
		}
		phy_ptr = get_base_addr_shmem();
		for (i = 0; i < 5; i++) {
			*(ptr+i) = arr[i];
                        printf(1, "\nWrote %c at %p", *(ptr+i), ptr+i);
		}
		printf(1, "\nPHYSICAL ADDRESS for CHILD = %p\n", *(char *)phy_ptr);
		exit();
	}
	else {
		wait();

		ptr = shmem_access(3);
		if (ptr == NULL) {
			test_failed();
		}
                printf(1, "\nMemory obtained : %p", ptr);
		phy_ptr = get_base_addr_shmem();
		printf(1, "\nPHYSICAL ADDRESS for PARENT = %p\n", *(char *)phy_ptr);
		for (i = 0; i < 4*PGSIZE; i++) {	
                        //printf(1, "\nRead %c at %p", *(ptr+i), ptr+i);	
			if (*(phy_ptr+i) == arr[0] || *(phy_ptr+i) == arr[1] || *(phy_ptr+i) == arr[2] || *(phy_ptr+i) == arr[3] ||
			*(phy_ptr+i) == arr[4]) {
				printf(1, "\nSUCCESS!");
			}
		}/*
                 ptr = shmem_access(2);
		if (ptr == NULL) {
			test_failed();
		}
                printf(1, "\nMemory obtained : %p", ptr);
		
		for (i = 0; i < PGSIZE; i++) {	
                        //printf(1, "\nRead %c at %p", *(ptr+i), ptr+i);	
			if (*(ptr+i) == arr[0] || *(ptr+i) == arr[1] || *(ptr+i) == arr[2] || *(ptr+i) == arr[3] ||
			*(ptr+i) == arr[4]) {
				printf(1, "\nSUCCESS!");
			}
		}
		ptr = shmem_access(1);
		if (ptr == NULL) {
			test_failed();
		}
                printf(1, "\nMemory obtained : %p", ptr);
		
		for (i = 0; i < PGSIZE; i++) {	
                        //printf(1, "\nRead %c at %p", *(ptr+i), ptr+i);	
			if (*(ptr+i) == arr[0] || *(ptr+i) == arr[1] || *(ptr+i) == arr[2] || *(ptr+i) == arr[3] ||
			*(ptr+i) == arr[4]) {
				printf(1, "\nSUCCESS!");
			}
		}
		ptr = shmem_access(0);
		if (ptr == NULL) {
			test_failed();
		}
                printf(1, "\nMemory obtained : %p", ptr);
		
		for (i = 0; i < PGSIZE; i++) {	
                        //printf(1, "\nRead %c at %p", *(ptr+i), ptr+i);	
			if (*(ptr+i) == arr[0] || *(ptr+i) == arr[1] || *(ptr+i) == arr[2] || *(ptr+i) == arr[3] ||
			*(ptr+i) == arr[4]) {
				printf(1,"\nSUCCESS!");
			}
		}
*/


                
	}
	
	test_passed();
  exit();
}



