#include<cstdio>
#include<cstring>
#include<cstdlib>
#include<algorithm>

struct queue
{
	int st, ed, q[2048];
};

struct bitmap
{
	uint bit[2048/32];
	int handle;
	int size;
};

struct superblock
{
	int start_before;
	
	int block_size;
	int block_number;
	
	int inode_number;
	int inode_table_offset;
	int inode_size;
	
	int indirect_inode_number;
	int indirect_inode_table_offset;
	int indirect_inode_size;
	
	int data_number;
	int data_offset;
	int data_size;
	
	int root_inode;
	int now_inode;
	//struct queue inode_q[1], data_q[1];
	struct bitmap inode_bitmap[1], indirect_inode_bitmap[1], \
	data_bitmap[1];
	
};

struct memory_management
{
	FILE *block[2048];
};

struct inode_entry
{
	int file_size;
	int size;
	int size_in1;
	int size_in2;
	int type;// 0 file	1 dir	
	int refcnt;
	int block[16];
	int indirect1[8];
	int indirect2[8];
};

struct indirect_inode_entry
{
	int size;
	int type;
	int block[16];
};

struct inode_table
{
	struct inode_entry *table;
};

struct directory_entry
{
	char name[16];
	int inode_number;
};// ???

struct directory
{
	int entry_number;
	struct directory_entry *dir_table;
};

struct file_table_entry
{
	int inode_num, file_cursor, refcnt;
};

struct file_table
{
	struct file_table_entry table[1024];
};

struct fd_table_entry
{
	int fd, index;
};

struct fd_table
{
	int num;
	struct fd_table_entry table[1024];
};