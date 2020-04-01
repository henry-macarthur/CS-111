// NAME: Derek Vance, Henry MacArthur
// EMAIL: dvance@g.ucla.edu, henrymac16@gmail.com
// ID: 604970765, 705096169
/*
	TODO:
	recursivley scan each of the indirect blocks and print the directories
	do the indirect things

*/
#include <stdlib.h>
#include <stdio.h>
#include "ext2_fs.h"
#include <fcntl.h>
#include <unistd.h>
#include <time.h>

const int SUPER_BLOCK_LOCATION = 1024;
int img_fd;
int num_groups;
struct ext2_super_block super_block_data;
struct ext2_group_desc group_data;

__u32 block_size;

int scan_directory_entries(int parent_id, __uint32_t ptr, int sz);
void scan_single(int block_num, int inode_number, int start, char type);
void scan_double(int block_num, int inode_number, int start, char type);
void scan_triple(int block_num, int inode_number, int start, char type);
void pread_error(int amount);

void process_inodes(int group_number, int i);

void time_formatter(time_t time)
{
	struct tm *ptm = gmtime(&time); 
	printf("%02d/%02d/%02d %02d:%02d:%02d,", ptm->tm_mon + 1, ptm->tm_mday, ptm->tm_year % 100, ptm->tm_hour, ptm->tm_min, ptm->tm_sec);
}

void pread_error(int amount)
{
	if (amount == -1) {
		fprintf(stderr, "ERROR: pread failed\n");
		exit(1);
	}
}

void super_block_function()
{
	//char super_block_buffer[]

	int amount_read = pread(img_fd, &super_block_data, sizeof(struct ext2_super_block), SUPER_BLOCK_LOCATION);
	pread_error(amount_read);
	block_size = EXT2_MIN_BLOCK_SIZE << super_block_data.s_log_block_size;
	printf("SUPERBLOCK,%u,%u,%u,%u,%u,%u,%u\n", super_block_data.s_blocks_count, super_block_data.s_inodes_count, block_size, 
		super_block_data.s_inode_size, super_block_data.s_blocks_per_group, super_block_data.s_inodes_per_group,
		super_block_data.s_first_ino);
}

//oid process_free_blocks
void group_summary(int group_number)
{
	int start_index;
	//base the offest off of block_size
	if(block_size == 1024) //is the third block
		start_index = 2;
	else
		start_index = 1;
	//if it does not fit into one group, is the next one contiguous
	int base_offset =  (block_size * start_index);
	int index_offset = sizeof(group_data) * (group_number); //how many blocks from the base
	int total_offset = base_offset + index_offset; //start at  blocks + index* size of a group block?

	int amount_read = pread(img_fd, &group_data, sizeof(group_data), total_offset);
	pread_error(amount_read);

	int blocks_in_group;
	int inodes_in_group;
	 //what if it is not full?
	//can do this later, but if it is the last one, this will not be full
	if((group_number + 1) == num_groups) //last block, might not be full
	{
		//get total blocks - how many we have looked at
		blocks_in_group = super_block_data.s_blocks_count - (super_block_data.s_blocks_per_group * group_number);
		inodes_in_group = super_block_data.s_inodes_count - (super_block_data.s_inodes_per_group * group_number);
	}
	else
	{
		blocks_in_group = super_block_data.s_blocks_per_group;
		inodes_in_group = super_block_data.s_inodes_per_group;
	}

	printf("GROUP,%u,%u,%u,%u,%u,%u,%u,%u\n", group_number, blocks_in_group, inodes_in_group, group_data.bg_free_blocks_count, group_data.bg_free_inodes_count, 
	group_data.bg_block_bitmap, group_data.bg_inode_bitmap, group_data.bg_inode_table);

	int bitmap_id = group_data.bg_block_bitmap; //this is the block id of the bitmap
	//process the bitmap
	//scan free block bitmap 1 means allocated, 0 means free
	//calculate offest, and do a pread
	char map[2048]; 
	int bitmap_offset = (block_size * (bitmap_id)); //should be the proper offset
	amount_read = pread(img_fd, map, block_size, bitmap_offset);
	pread_error(amount_read);
	//write(1, map, 12);
	//map should now contain all of the bitmap
	//loop through whole bitmap or just what is free
	int i = 0;
	for(; i < blocks_in_group / 8; i++) //each map[i] = a byte, need to break down byte into 8 bits
	{
		char cur = map[i];
		int j = 0;
		int mask = 1;
		for(; j < 8; j++)
		{
			int free = cur & mask;
			if(free == 0) 
			{ // do i use first data block or no?
				printf("BFREE,%d\n", ((group_number * super_block_data.s_blocks_per_group) + super_block_data.s_first_data_block) + (i*8) + j);
			}
			cur >>= 1;
		}
	}
	//DO NOT INTERPRETE TOO MANY BITS
	////
	int inode_bitmap_id = group_data.bg_inode_bitmap;
	int inode_bitmap_offset = block_size * inode_bitmap_id;
	amount_read = pread(img_fd, map, block_size, inode_bitmap_offset);
	pread_error(amount_read);

	i = 0;
	for(; i < inodes_in_group / 8; i++)//super_block_data.s_inodes_per_group / 8; i++)
	{
		char cur = map[i];
		int j = 0;
		int mask = 1;
		for(; j < 8; j++)
		{
			int free = cur & mask;
			if(!free)
			{
				printf("IFREE,%d\n", (group_number * super_block_data.s_inodes_per_group) + i*8 + j + 1);
			}
			else
			{
				//not free inode, has been allocated
				//printf("%d\n", (group_number * super_block_data.s_inodes_per_group) + i*8 + j + 1);
				process_inodes(group_number, (group_number * super_block_data.s_inodes_per_group) + i*8 + j + 1);

			}
			cur >>= 1;
		}
	}
	//process_inodes(group_number);
	//locate inode table
	
}

void process_inodes(int group_number, int i)
{
	int inode_table_id = group_data.bg_inode_table;
	int inode_offset = inode_table_id * block_size;
	//int num_inodes = super_block_data.s_inodes_per_group;
	struct ext2_inode current_inode;
	// int i = 0;
	// for(; i < num_inodes; i++)
	// {
		

		int amount_read = pread(img_fd, &current_inode, sizeof(struct ext2_inode), inode_offset + (i - (group_number * super_block_data.s_inodes_per_group + 1)) *sizeof(struct ext2_inode));
		pread_error(amount_read);
		// if(i == 335)
		// {
		// 	printf("AAAA \n");
		// }
		//read the current inode from the inode table
		char type = '?';
		if((current_inode.i_mode & 0xF000 ) ==  0x8000) //file
		{
			type = 'f';
		}
		else if((current_inode.i_mode & 0xF000) == 0x4000) //dir
		{
			type = 'd';
		}
		else if((current_inode.i_mode & 0xF000) == 0xA000)
		{
			type = 's';
		}	
		if(current_inode.i_mode != 0 && current_inode.i_links_count != 0)
		{
			printf("INODE,%d,%c,%o,%d,%d,%d,", i,type, 
			current_inode.i_mode & 0xfff, current_inode.i_uid, current_inode.i_gid, current_inode.i_links_count);
			time_formatter(current_inode.i_ctime);
			time_formatter(current_inode.i_mtime);
			time_formatter(current_inode.i_atime);
			
		

			//int max = current_inode.i_blocks/(2<<super_block_data.s_log_block_size);
			printf("%d,%d", current_inode.i_size, current_inode.i_blocks);//current_inode.i_blocks);
			int k;

			if(type == 'd' || type == 'f' || (type == 's' && current_inode.i_size >= 60))
			{
				for(k = 0; k < 15; k++)
				{
					printf(",%d", current_inode.i_block[k]);
				}
			}
			printf("\n");
			// if(type != 'd')
			// 	max = 0;
			for(k = 0; k < 12; k++)
			{
				// int j = 0;
				// for(; j < sizeof(struct ext_dir_entry))
				if(current_inode.i_block[k] != 0 && type == 'd')
				{
					scan_directory_entries(i, current_inode.i_block[k], 0);
				}
			}

		//scan direct entries

		//scan indirect endtries, 12th index
			//for each of these i think if the file type is a directory, print it as a directory entry 
			int single = current_inode.i_block[12];
			int double_indirect = current_inode.i_block[13];
			int triple_indirect = current_inode.i_block[14];
			if(single != 0)
			{
				scan_single(single, i, 12, type);
			}
			if(double_indirect != 0)
			{
				scan_double(double_indirect, i, 12, type);
			}
			if(triple_indirect != 0)
			{
				scan_triple(triple_indirect, i, 12, type);
			}
		}
	//}
}

void scan_triple(int block_num, int inode_number, int start, char type)
{
	int offset = block_num * block_size;
	__u32 buff[2048]; 
	int amount_read = pread(img_fd, &buff, block_size, offset);
	pread_error(amount_read);

	int i;
	for(i = 0; (unsigned long)i < block_size/sizeof(__u32); i++)
	{
		if(buff[i] != 0)
		{
			int logical_offset = (256 * 256 * (i + 1)) + 256  + start;
			printf("INDIRECT,%d,3,%d,%d,%u\n", inode_number, logical_offset, block_num, buff[i]);
			scan_double(buff[i], inode_number, logical_offset - 256, type);
		}
	}
}

void scan_double(int block_num, int inode_number, int start, char type)
{

	int offset = block_num * block_size; 
	__u32 buff[2048];
	int amount_read = pread(img_fd, &buff, block_size, offset);
	pread_error(amount_read);

	int i = 0;
	for(; (unsigned long)i < block_size/sizeof(__u32); i++)
	{
		if(buff[i] != 0)
		{
			int logical_offset = (256 * (i + 1)) + start;
			printf("INDIRECT,%d,2,%d,%d,%u\n", inode_number, logical_offset, block_num, buff[i]);
			scan_single(buff[i], inode_number, logical_offset, type);
		}
	}
}

void scan_single(int block_num, int inode_number, int start, char type) //13th entry 
{
	int offset = block_size * block_num;
	__u32 buff[2048];
	int amount_read = pread(img_fd, &buff, block_size, offset);
	pread_error(amount_read);
	//read the indirection block 
	int i = 0;
	for(i = 0; (unsigned long)i < block_size/sizeof(__u32); i++)
	{
		if(buff[i] != 0) //good!
		{
			//scan_directory_entries(inode_number, buff[i])
			//start = 12;
			printf("INDIRECT,%d,1,%d,%d,%u\n", inode_number, i + start, block_num, buff[i]); //256^level + (level - 1)*256
			//256^level * (i -1) + (level -1)*256
			if(type == 'd')
			{
				scan_directory_entries(inode_number, buff[i], i + start);
			}
		}
	}
}

int scan_directory_entries(int parent_id, __uint32_t ptr, int sz)
{
	//need to do a pread
	int current_read = 0;
	int logical_byte_offset = sz;
	int offset = ptr * block_size;
	while(current_read < (int)block_size) //loop through the whole thing!
	{
		struct ext2_dir_entry current_directory;
		int amount_read = pread(img_fd, &current_directory, sizeof(struct ext2_dir_entry), offset);
		pread_error(amount_read);

		int inode_number = current_directory.inode;
		int entry_length = current_directory.rec_len;
		int name_length = current_directory.name_len;
		//char * name = current_directory.name; //when we print this out add 'name';

		if(current_directory.inode != 0)
		{
			printf("DIRENT,%d,%d,%d,%d,%d,'", parent_id, logical_byte_offset, inode_number, entry_length, name_length);
			printf("%.*s", name_length, current_directory.name);
			printf("'\n");
		}

		offset += entry_length;
		logical_byte_offset += entry_length;
		current_read += entry_length;
	}

	return 0;
	
}

int main(int argc, char ** argv)
{

	if(argc != 2)
	{
		fprintf(stderr, "invalid number of arguments\n");
		exit(1);
	}
	else
	{
		//printf("%s \n", argv[1]);
	}

	char * file = argv[1];

	img_fd = open(file, O_RDONLY);
	//printf("%d\n", img_fd);
	super_block_function(); //super_block_data will now be initialized so I can use it
	num_groups = (super_block_data.s_blocks_count -1)/ super_block_data.s_blocks_per_group+1;
	int i = 0;
	for(; i < num_groups; i++)
	{
		//loop through each block
		group_summary(i);

	}
	//read superblock data = pread()
	return 0;
}