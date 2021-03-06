Project 3:

Part a : Null pointer Dereferencing

In xv6, by default null pointer dereferencing does not throw an error since the pages are loaded in virtual memory from address 0. Here, we modify xv6 to load the process into virtual memory by leaving the first page empty. 

To do that, I modified the following files:
  1. exec.c - modified the initial address to where the process will be loaded to PGSIZE
  2. vm.c - in the function copyuvm(), modified the function to skip copying the first page and to start from the next page.
  3. syscall.c - modified the function argptr(). Here, it fetches the arguments that serve as a pointer from the stack. Here, I added a check to see if the pointer being read has a value ranging from 0 to PGSIZE, it will return -1. This is done to ensure that any invalid addresses lying in the first page are caught before dereferencing and the programe exits gracefully.


Part b : Shared memory

In this part, I implemented shared memory where 4 pages are allocated in physical memory and mapped to the last 4 pages in the virtual address space from the bottom. A process can access the shared memory using the shmem_access(page_number) system call and also get the number processes sharing each page using the shmem_count(page_number) system call.

Modifications had to be done in several places to allow the shared pages to be accessed by several processes : 
  1. vm.c - modified fucntions freevm()
  2. exec.c
  3. proc.h - modified the process structure
  4. proc.c - modified allocproc(), fork(), exit()
  5. syscall.c - modified  fetchint() and fetchstr() to handle arguments of system calls containing references to shared pages 	
