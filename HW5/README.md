Project 5

Team Members:
	Kirthanaa Raghuraman
	Manish Bansal

xv6 - Small file optimization

In this part of the project, we added a small file optimization to the xv6 file system, where small files of size 52 bytes or less are stored in the inode of the file itself in place of the direct and indirect pointers. 

The modifications were mainlu done in kernel/fs.c in readi(), writei() and trunci() fucntions.
In readi(), the file is directly read from the inode's address pointers if the file size is less than 52 bytes.
In writei(), the file is written to the address blocks if the file is less than 52 bytes.
In trunci(), the blocks corresponding to the direct and indirect pointers are not removed if the files are of the type T_SMALLFILE.

Also, if the file is of type T_SMALLFILE, and a user is trying to read/write more than the file size, only the maximum number of bytes (either the file size or 52 bytes) is allowed to be read/written respectively.

fscheck - File system checker
In this part, we wrote a c program that checks for integrity and several other checks on a file system such as proper formatting of inods, structure of root directory, allocation in inode and data bitmaps and so on. 
