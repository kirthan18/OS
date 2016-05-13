#include<stdio.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<unistd.h>
#include<assert.h>
#include<sys/mman.h>
#include<stdlib.h>
#include <string.h>

//#include<stdint.h>

// Block 0 is unused.
// Block 1 is super block.
// Inodes start at block 2.

#define ROOTINO 1  // root i-number
#define BSIZE 512  // block size
#define NDIRECT 12
#define NINDIRECT (BSIZE / sizeof(uint))

// File system super block
struct superblock {
  uint size;         // Size of file system image (blocks)
  uint nblocks;      // Number of data blocks
  uint ninodes;      // Number of inodes.
};

// On-disk inode structure
struct dinode {
  short type;           // File type
  short major;          // Major device number (T_DEV only)
  short minor;          // Minor device number (T_DEV only)
  short nlink;          // Number of links to inode in file system
  uint size;            // Size of file (bytes)
  uint addrs[NDIRECT+1];   // Data block addresses
};

#define IPB           (BSIZE / sizeof(struct dinode))

// Directory is a file containing a sequence of dirent structures.
#define DIRSIZ 14
struct dirent {
  ushort inum;
  char name[DIRSIZ];
};

// superblock | inode table | bitmap(data) | data blocks
// some gaps are also there

void check_valid_inode_type(void *);
void check_valid_root_inode(void *);
void check_valid_inode_address(void *);
void check_directory_format(void *);
struct dirent * get_dir_entry(void *, uint, uint);
void check_block_and_bitmap_consistency(void *);
void check_inode_and_directory_entry_consistency(void *);
void check_ref_count_directory(void *);

int 
main(int argc, char * argv[]){

  if(argc != 2){
	fprintf(stderr, "image not found.\n");
    exit(1);
  }

  int fd = open(argv[1],O_RDONLY);
  if(fd < 0){
	fprintf(stderr, "image not found.\n");
    exit(1);
  }

  int rc;
  struct stat sbuf;
  rc = fstat(fd, &sbuf);
  assert(rc == 0);

  void * img_ptr = mmap(NULL, sbuf.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
  assert(img_ptr != MAP_FAILED);

  struct superblock *sb;
  sb = (struct superblock *) (img_ptr + BSIZE);
  //printf("%d %d %d\n", sb->size, sb->nblocks, sb->ninodes);

  check_valid_inode_type(img_ptr);
  check_valid_root_inode(img_ptr);
  check_valid_inode_address(img_ptr);
  // to check the presence of . and .. in every folder and correct redirection of .. to its actual parent.
  check_directory_format(img_ptr); // has to be after valid inode address and valid inode type
  // to check consistency between bitmap and data blocks of inodes.
  check_block_and_bitmap_consistency(img_ptr);
  check_inode_and_directory_entry_consistency(img_ptr);
  check_ref_count_directory(img_ptr);

  return 0;
}





void check_ref_count_directory(void * img_ptr){
  struct dinode * dip = (struct dinode *) (img_ptr + 2*BSIZE);
  struct superblock *sb  = (struct superblock *) (img_ptr + BSIZE);
  int i;
  int inode_in_dir_entry[sb->ninodes];
  //initialise the two maps to zeros.
  for(i=0 ; i<sb->ninodes ; i++){
	inode_in_dir_entry[i] = 0;
  }

  for(i=0 ; i<sb->ninodes ; i++, dip++){
	int type = dip->type;
    if(type != 1) {
	  continue;
	}
	int j;	
	for(j=0; j<=NDIRECT; j++){
	  if(j < NDIRECT){
		if(dip->addrs[j] == 0){ // unused direct pointer. Valid case
		  continue;	
		}
		//read the datablock for directory entries.
		struct dirent * dir_entry_ptr = (struct dirent *)(dip->addrs[j]*BSIZE+img_ptr);
		int max_dir_entry = BSIZE/sizeof(struct dirent);
		int k;		
		for(k=0; k<max_dir_entry; k++){
		  struct dirent * curr_dir_entry = dir_entry_ptr + k;
		  if(strcmp(curr_dir_entry->name,".")==0 || strcmp(curr_dir_entry->name,"..")==0)
			continue;
		  inode_in_dir_entry[curr_dir_entry->inum]++;
		}
	  }
	  if(j == NDIRECT){ //process indirect block and exit.
		if(dip->addrs[j] == 0){ // unused indirect pointer. Valid case
		  continue;	
		}
		//Read the indirect pointer page now.
		uint indirect_addr_page = dip->addrs[j];
		uint k;
		for(k=0; k<NINDIRECT; k++){
		  uint * block_addr_ptr = (uint *)(indirect_addr_page*BSIZE+img_ptr);
		  uint block_addr = *(k+block_addr_ptr);
	
		  struct dirent * dir_entry_ptr = (struct dirent *)(block_addr*BSIZE+img_ptr);
		  int l=0, max_dir_entry = BSIZE/sizeof(struct dirent);
		  for(l=0; l<max_dir_entry; l++){
		    struct dirent * curr_dir_entry = dir_entry_ptr + l;
		  	if(strcmp(curr_dir_entry->name,".")==0 || strcmp(curr_dir_entry->name,"..")==0)
			  continue;
			inode_in_dir_entry[curr_dir_entry->inum]++;
		  }  // end of for(l=0; l<max_dir_entry; l++)
		} //  end of for(k=0; k<NINDIRECT; k++)
	  } // end of if(j == NDIRECT)
	} //  end of for(j=0; j<=NDIRECT; j++)
  }//end of for(i=0 ; i<sb->ninodes ; i++, dip++)

  //checking the reference count for directories now.
  dip = (struct dinode *) (img_ptr + 2*BSIZE);
  for(i=0 ; i<sb->ninodes ; i++, dip++){
	int type = dip->type;
	if(type!=1 || i==1){
	  continue;
	}
	if(inode_in_dir_entry[i] != 1){
	  fprintf(stderr, "ERROR: directory appears more than once in file system.\n");
	  exit(1);
	}
  }
}

void check_inode_and_directory_entry_consistency(void * img_ptr){
  struct dinode * dip = (struct dinode *) (img_ptr + 2*BSIZE);
  struct superblock *sb  = (struct superblock *) (img_ptr + BSIZE);
  int i;
  int inode_use_map[sb->ninodes];
  int inode_in_dir_entry[sb->ninodes];
  //initialise the two maps to zeros.
  for(i=0 ; i<sb->ninodes ; i++){
	inode_use_map[i] = 0;
	inode_in_dir_entry[i] = 0;
  }

  for(i=0 ; i<sb->ninodes ; i++, dip++){
	int type = dip->type;
    if(type == 0) {
	  continue;
	}
    inode_use_map[i] = 1;
	if(type == 2 || type == 3)
		continue;
	int j;	
	for(j=0; j<=NDIRECT; j++){
	  if(j < NDIRECT){
		if(dip->addrs[j] == 0){ // unused direct pointer. Valid case
		  continue;	
		}
		//read the datablock for directory entries.
		struct dirent * dir_entry_ptr = (struct dirent *)(dip->addrs[j]*BSIZE+img_ptr);
		int max_dir_entry = BSIZE/sizeof(struct dirent);
		int k;		
		for(k=0; k<max_dir_entry; k++){
		  struct dirent * curr_dir_entry = dir_entry_ptr + k;
		  inode_in_dir_entry[curr_dir_entry->inum]++;
		}
	  }
	  if(j == NDIRECT){ //process indirect block and exit.
		if(dip->addrs[j] == 0){ // unused indirect pointer. Valid case
		  continue;	
		}
		//Read the indirect pointer page now.
		uint indirect_addr_page = dip->addrs[j];
		uint k;
		for(k=0; k<NINDIRECT; k++){
		  uint * block_addr_ptr = (uint *)(indirect_addr_page*BSIZE+img_ptr);
		  uint block_addr = *(k+block_addr_ptr);
	
		  struct dirent * dir_entry_ptr = (struct dirent *)(block_addr*BSIZE+img_ptr);
		  int l=0, max_dir_entry = BSIZE/sizeof(struct dirent);
		  for(l=0; l<max_dir_entry; l++){
		    struct dirent * curr_dir_entry = dir_entry_ptr + l;
			inode_in_dir_entry[curr_dir_entry->inum]++;
		  }  // end of for(l=0; l<max_dir_entry; l++)
		} //  end of for(k=0; k<NINDIRECT; k++)
	  } // end of if(j == NDIRECT)
	} //  end of for(j=0; j<=NDIRECT; j++)
  }//end of for(i=0 ; i<sb->ninodes ; i++, dip++)
  
  for(i=1 ; i<sb->ninodes ; i++){ // skipping i=0 because it can contain garbage
	if(inode_use_map[i] && !inode_in_dir_entry[i]){
	  fprintf(stderr, "ERROR: inode marked use but not found in a directory.\n");
	  exit(1);
	} else if(!inode_use_map[i] && inode_in_dir_entry[i]) {
	  fprintf(stderr, "ERROR: inode referred to in directory but marked free.\n");
	  exit(1);
	}
  }

  //checking the reference count for files now.
  dip = (struct dinode *) (img_ptr + 2*BSIZE);
  for(i=0 ; i<sb->ninodes ; i++, dip++){
	int type = dip->type;
	if(type!=2 && type!=3){
	  continue;
	}
	if(inode_in_dir_entry[i] != dip->nlink){
	  fprintf(stderr, "ERROR: bad reference count for file.\n");
	  exit(1);
	}
  }
}

void check_block_and_bitmap_consistency(void * img_ptr){
  struct dinode * dip = (struct dinode *) (img_ptr + 2*BSIZE);
  struct superblock *sb  = (struct superblock *) (img_ptr + BSIZE);
  int blocks_used_by_inode[sb->size];
  int i;
  //initialize blocks_used_by_inode to ones and zeros.
  for(i=0; i<sb->size; i++){
    if(i < (sb->ninodes/IPB + 4)){ // for file sytem data structure without data blocks
	  blocks_used_by_inode[i] = 1;
	} else {
      blocks_used_by_inode[i] = 0;
    }
  }

  // finding data blocks actually used by inodes.
  for(i=0 ; i<sb->ninodes ; i++, dip++){
	int j;	
	for(j=0; j<=NDIRECT; j++){
	  if(j < NDIRECT){
		if(dip->addrs[j] == 0){ // unused direct pointer. Valid case
		  continue;	
		}
		blocks_used_by_inode[dip->addrs[j]]++;
	  }
	  
	  if(j == NDIRECT){ //process indirect block and exit.
		if(dip->addrs[j] == 0){ // unused indirect pointer. Valid case
		  continue;	
		}
		//Read the indirect pointer page now.
		uint indirect_addr_page = dip->addrs[j];
		blocks_used_by_inode[dip->addrs[j]]++;
		uint k;
		for(k=0; k<NINDIRECT; k++){
		  uint * block_addr_ptr = (uint *)(indirect_addr_page*BSIZE+img_ptr);
		  uint block_addr = *(k+block_addr_ptr);
		  if(block_addr == 0)
			continue;
		  blocks_used_by_inode[block_addr]++;
		}
	  }
	}
  } // end of for(i=0 ; i<sb->ninodes ; i++, dip++){

  //checking for the correspondence in the data bitmap and blocks_used_by_inode.
  char * bitmap_addr = (char *) (img_ptr + (sb->ninodes/IPB + 3)*BSIZE);
  for(i=0 ; i<sb->size ; i++){
	int byte_idx = i/8;
	int bit_idx = i%8;	
	int data_bitmap_val_at_i = ((*(bitmap_addr+byte_idx)) >> bit_idx) & 0x1;
	if(blocks_used_by_inode[i] > 1){
	  fprintf(stderr, "ERROR: address used more than once.\n");
	  exit(1); 
	}
    if(data_bitmap_val_at_i && !blocks_used_by_inode[i]){ //desired case of consistency.
	  fprintf(stderr, "ERROR: bitmap marks block in use but it is not in use.\n");
	  exit(1);  
	}
    if(!data_bitmap_val_at_i && blocks_used_by_inode[i]){ //desired case of consistency.
	  fprintf(stderr, "ERROR: address used by inode but marked free in bitmap.\n");
	  exit(1);  
	}
  }
}

// This method will get the entry for a folder name in the folder identified by inode number.
struct dirent * get_dir_entry(void * img_ptr, uint parent_inode_num, uint search_inode_num){
  struct dinode * dip = (struct dinode *) (img_ptr + 2*BSIZE);
  struct dinode * curr_inode = dip + parent_inode_num;
  
  int j;
  for(j=0; j<=NDIRECT; j++){
	  if(j < NDIRECT){
		if(curr_inode->addrs[j] == 0){// empty pointer continue.
		  continue;	
		}
		//read the datablock for directory entries.
		struct dirent * dir_entry_ptr = (struct dirent *)(curr_inode->addrs[j]*BSIZE+img_ptr);
		int max_dir_entry = BSIZE/sizeof(struct dirent);
		int k;		
		for(k=0; k<max_dir_entry; k++){
		  struct dirent * curr_dir_entry = dir_entry_ptr + k;
		  if(strcmp(curr_dir_entry->name,".") != 0 && strcmp(curr_dir_entry->name,"..") != 0 && curr_dir_entry->inum == search_inode_num){
			return curr_dir_entry;
		  }
		}
	  } // end of if(j < NDIRECT)
	  
	  if(j == NDIRECT){ //process indirect block and exit.
		if(curr_inode->addrs[j] == 0){ // unused indirect pointer
		  continue;	
		}
		//Read the indirect pointer page now.
		uint indirect_addr_page = curr_inode->addrs[j];
		uint k;
		for(k=0; k<NINDIRECT; k++){
		  uint * block_addr_ptr = (uint *)(indirect_addr_page*BSIZE+img_ptr);
		  uint block_addr = *(k+block_addr_ptr);
	
		  struct dirent * dir_entry_ptr = (struct dirent *)(block_addr*BSIZE+img_ptr);
		  int l=0, max_dir_entry = BSIZE/sizeof(struct dirent);
		  for(l=0; l<max_dir_entry; l++){
		    struct dirent * curr_dir_entry = dir_entry_ptr + l;
		   	 if(strcmp(curr_dir_entry->name,".") != 0 && strcmp(curr_dir_entry->name,"..") != 0 && curr_dir_entry->inum == search_inode_num){
			  return curr_dir_entry;
		    }
		  }	  
		}
	  } // end of if(j == NDIRECT)
  }//end of level 1 for loop
  return NULL;  
}


void check_directory_format(void * img_ptr){
  struct dinode * dip = (struct dinode *) (img_ptr + 2*BSIZE);
  struct superblock *sb  = (struct superblock *) (img_ptr + BSIZE);
  int i;
  for(i=0 ; i<sb->ninodes ; i++, dip++){
	// check whether the inode is a directory or not.
	int type = dip->type;
    if(type != 1) {
	  continue;
	}
	int seen_dot = 0, seen_parent_dot = 0;
	int j;	
	for(j=0; j<=NDIRECT; j++){
	  if(j < NDIRECT){
		if(dip->addrs[j] == 0){ // unused direct pointer. Valid case
		  continue;	
		}
		//read the datablock for directory entries.
		struct dirent * dir_entry_ptr = (struct dirent *)(dip->addrs[j]*BSIZE+img_ptr);
		int max_dir_entry = BSIZE/sizeof(struct dirent);
		int k;		
		for(k=0; k<max_dir_entry; k++){
		  struct dirent * curr_dir_entry = dir_entry_ptr + k;
		  if(strcmp(curr_dir_entry->name,".") == 0){
			seen_dot = 1;
		  } else if (strcmp(curr_dir_entry->name,"..") == 0){
			seen_parent_dot = 1;
			struct dirent * parents_child_dir_entry = get_dir_entry(img_ptr, curr_dir_entry->inum, i);
			if(i!=1 && parents_child_dir_entry == NULL){	
			  fprintf(stderr, "ERROR: parent directory mismatch.\n");
			  exit(1);
			}
		  }
		  if(seen_dot && seen_parent_dot) // to better the performance
			break;
		}
	  }
	  
	  if(j == NDIRECT){ //process indirect block and exit.
		//printf("I am Inside the indirect code\n");
		if(dip->addrs[j] == 0){ // unused indirect pointer. Valid case
		  continue;	
		}
		//Read the indirect pointer page now.
		uint indirect_addr_page = dip->addrs[j];
		uint k;
		for(k=0; k<NINDIRECT; k++){
		  uint * block_addr_ptr = (uint *)(indirect_addr_page*BSIZE+img_ptr);
		  uint block_addr = *(k+block_addr_ptr);
	
		  struct dirent * dir_entry_ptr = (struct dirent *)(block_addr*BSIZE+img_ptr);
		  int l=0, max_dir_entry = BSIZE/sizeof(struct dirent);
		  for(l=0; l<max_dir_entry; l++){
		    struct dirent * curr_dir_entry = dir_entry_ptr + l;
		    if(strcmp(curr_dir_entry->name,".") == 0){
		      seen_dot = 1;
		    } else if (strcmp(curr_dir_entry->name,"..") == 0){
			  seen_parent_dot = 1;
			  struct dirent * parents_child_dir_entry = get_dir_entry(img_ptr, curr_dir_entry->inum, i);
			  if(i!=1 && parents_child_dir_entry == NULL){	
			    fprintf(stderr, "ERROR: parent directory mismatch.\n");
			    exit(1);
		 	  }
		    }
		    if(seen_dot && seen_parent_dot) // to better the performance
			  break;
		  }	
		  if(seen_dot && seen_parent_dot) // to better the performance
			break;	  
		}
		
	  }
	}
	if(seen_dot == 0 || seen_parent_dot == 0){ // to better the performance
      fprintf(stderr, "ERROR: directory not properly formatted.\n");
      exit(1);
	}
  }
  
}


void check_valid_inode_type(void * img_ptr){
  struct dinode * dip = (struct dinode *) (img_ptr + 2*BSIZE);
  struct superblock *sb  = (struct superblock *) (img_ptr + BSIZE);
  int i;
  for(i=0 ; i<sb->ninodes ; i++){
	int type = dip->type;
	// check for correct type of inode
    if(type < 0 || type > 3) {
	  fprintf(stderr, "ERROR: bad inode.\n");
      exit(1);
	}
	dip++;
  }
}


void check_valid_root_inode(void * img_ptr){
  struct dinode * dip = (struct dinode *) (img_ptr + 2*BSIZE);
  if((dip+1)->type != 1) {
	fprintf(stderr, "ERROR: root directory does not exist.\n");
    exit(1);
  }

  // to make sure that bot . and .. refers to itself for root.
  struct dinode * root_inode = dip + 1;
  int j;
  for(j=0; j<=NDIRECT; j++){
	  if(j < NDIRECT){
		if(root_inode->addrs[j] == 0){// empty pointer continue.
		  continue;	
		}
		//read the datablock for directory entries.
		struct dirent * dir_entry_ptr = (struct dirent *)(root_inode->addrs[j]*BSIZE+img_ptr);
		int max_dir_entry = BSIZE/sizeof(struct dirent);
		int k;		
		for(k=0; k<max_dir_entry; k++){
		  struct dirent * root_dir_entry = dir_entry_ptr + k;
		  if(strcmp(root_dir_entry->name,".") == 0 && root_dir_entry->inum != 1){
			fprintf(stderr, "ERROR: root directory does not exist.\n");
    		exit(1);
		  }
		  if(strcmp(root_dir_entry->name,"..") == 0 && root_dir_entry->inum != 1){
			fprintf(stderr, "ERROR: root directory does not exist.\n");
    		exit(1);
		  }
		}
	  } // end of if(j < NDIRECT)
	  
	  if(j == NDIRECT){ //process indirect block and exit.
		if(root_inode->addrs[j] == 0){ // unused indirect pointer
		  continue;	
		}
		//Read the indirect pointer page now.
		uint indirect_addr_page = root_inode->addrs[j];
		uint k;
		for(k=0; k<NINDIRECT; k++){
		  uint * block_addr_ptr = (uint *)(indirect_addr_page*BSIZE+img_ptr);
		  uint block_addr = *(k+block_addr_ptr);
	
		  struct dirent * dir_entry_ptr = (struct dirent *)(block_addr*BSIZE+img_ptr);
		  int l=0, max_dir_entry = BSIZE/sizeof(struct dirent);
		  for(l=0; l<max_dir_entry; l++){
		    struct dirent * root_dir_entry = dir_entry_ptr + l;
		    if(strcmp(root_dir_entry->name,".") == 0 && root_dir_entry->inum != 1){
			  fprintf(stderr, "ERROR: root directory does not exist.\n");
    		  exit(1);
		    }
		    if(strcmp(root_dir_entry->name,"..") == 0 && root_dir_entry->inum != 1){
		      fprintf(stderr, "ERROR: root directory does not exist.\n");
    		  exit(1);
		    }
		  }	  
		}
	  } // end of if(j == NDIRECT)
  }//end of level 1 for loop


}


void check_valid_inode_address(void * img_ptr){
  struct dinode * dip = (struct dinode *) (img_ptr + 2*BSIZE);
  struct superblock *sb  = (struct superblock *) (img_ptr + BSIZE);
  int i;
  for(i=0 ; i<sb->ninodes ; i++){
	// check for the validity of the data blocks in the direct and indirect pointers.

	int j;	
	for(j=0; j<=NDIRECT; j++){
	  if(j < NDIRECT){
		if(dip->addrs[j] == 0){ // unused direct pointer. Valid case
		  continue;	
		}
		int block_index = dip->addrs[j];
		if(block_index < (sb->ninodes/IPB+4) || block_index >= sb->size){
		  fprintf(stderr, "ERROR: bad address in inode.\n");
      	  exit(1);
		}	
	  }
	  
	  if(j == NDIRECT){ //process indirect block and exit.
		//printf("I am Inside the indirect code\n");
		if(dip->addrs[j] == 0){ // unused indirect pointer. Valid case
		  continue;	
		}
		//Read the indirect pointer page now.
		uint indirect_addr_page = dip->addrs[j];
		uint k;
		for(k=0; k<NINDIRECT; k++){
		  uint * block_addr_ptr = (uint *)(indirect_addr_page*BSIZE+img_ptr);
		  uint block_addr = *(k+block_addr_ptr);
		  if(block_addr == 0){ // unused direct pointer. Valid case
			  continue;	
		  }
		  int block_index = block_addr;
		  if(block_index < (sb->ninodes/IPB+4) || block_index >= sb->size){
			fprintf(stderr, "ERROR: bad address in inode.\n");
		  	exit(1);
		  }
		}
		
	  }
	}
	dip++;
  }
}

