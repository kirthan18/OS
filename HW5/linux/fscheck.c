#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>

// Block 0 is unused.
// Block 1 is super block.
// Inodes start at block 2.
#define ROOTINO 1  // root i-number
#define BSIZE 512  // block size

// File system super block
struct superblock {
  uint size;         // Size of file system image (blocks)
  uint nblocks;      // Number of data blocks
  uint ninodes;      // Number of inodes.
};


int main(int argc, char **argv)
{
	//char *file_image;
	//char buf[BSIZE];

	//int return_value;

	struct superblock *sb;
	struct stat sbuf;

	/*if(argc != 1)
	{
		fprintf(stderr, "Invalid number of arguments!");
		return 1;
	}*/

	//TODO - change this to read the input received as a command line argument
	int fd = open("fs.img", O_RDONLY);
	
	if(fd < 0)
	{
		fprintf(stderr, "image not found\n");
		return 1;
	}

	void *img_ptr = mmap(NULL, sbuf.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
	if(img_ptr == MAP_FAILED)
	{
		fprintf(stderr, "Error mapping file image using mmap");
		return 1;
	}

	/*return_value = read(fd, buf, BSIZE); //sector 0
	if(return_value != BSIZE){
		fprintf(stderr, "Error reading sector 0\n");
		return 1;
	}

	return_value = read(fd, buf, BSIZE); //sector 1
	if(return_value != BSIZE){
		fprintf(stderr, "Error reading sector 1\n");
		return 1;
	}

	sb = (struct superblock*)buf;*/

	sb = (struct superblock*) (img_ptr + BSIZE);

	printf("Size : %d\nNum blocks : %d\nNum inodes : %d\n", sb->size, sb->nblocks, sb->ninodes);

	return 0;
}