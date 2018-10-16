#include "fs.hpp"

struct superblock Sb[1];
struct memory_management mem_ctrl[1];
struct inode_table inodes[1];
struct control ctrl[1];
struct file_table ft[1];
struct fd_table fd[1];

uint lowbit( uint x )
{
	return x&(-x);
}

void init_bitmap( struct bitmap *btmp )
{
	memset( btmp->bit, 0, sizeof(btmp->bit) );
	btmp->handle = 0;
	btmp->size = 2048;
}

int query_bitmap( struct bitmap *btmp )
{
	int i, j;
	for( i = btmp->handle; i < btmp->size/32; i ++ ){
		for( j = 0; j < 32; j ++ ){
			if( ( btmp->bit[i] &  (1<<j) ) != 0 ) continue;
			else{
				btmp->bit[i] |= ( 1 << j );
				btmp->handle = i;
				ret = j+i*32;
				return ret;
			}
		}
	}
	for( i = 0; i < btmp->handle; i ++ ){
		for( j = 0; j < 32; j ++ ){
			if( ( btmp->bit[i] &  (1<<j) ) != 0 ) continue;
			else{
				btmp->bit[i] |= ( 1 << j );
				btmp->handle = i;
				ret = j+i*32;
				return ret;
			}
		}
	}
	printf("no space\n");
	exit(1);
}

void update_bitmap( struct bitmap *btmp, int x )
{
	if( btmp->bit[ x/32 ] & ( 1 << (x&31) ) ) 
		btmp->bit[ x/32 ] ^= ( 1 << (x&31) );
}

void fs_read( void *buffer, int length, int tar )
{
	int i;
	int bsize = Sb->block;
	int tot = length;
	for( i = tar/bsize; i < (tar+length)/bsize+1; i ++ ){
		fseek( mem_ctrl->block[i], SEEK_SET, ( i==tar/bsize )?tar%bsize:0 );
		int len;
		if( i == tar/size ){
			len = bsize-tar%bsize;
		}
		else if( i == (tar+length)/bsize ){
			len = (tar+length)%bsize;
			if( len == 0 ) len += bsize;
		}
		else{
			len = bsize;
		}
		fread( buffer, 1, len, mem_ctrl->block[i] );
		buffer += len;
	}
}

void fs_write( void *buffer, int length, int tar )
{
	int i;
	int bsize = Sb->block;
	int tot = length;
	for( i = tar/bsize; i < (tar+length)/bsize+1; i ++ ){
		fseek( mem_ctrl->block[i], SEEK_SET, ( i==tar/bsize )?tar%bsize:0 );
		int len;
		if( i == tar/size ){
			len = bsize-tar%bsize;
		}
		else if( i == (tar+length)/bsize ){
			len = (tar+length)%bsize;
			if( len == 0 ) len += bsize;
		}
		else{
			len = bsize;
		}
		fwrite( buffer, 1, len, mem_ctrl->block[i] );
		buffer += len;
	}
}

void read_inode( char *buf, int inode )
{
	fs_read( buf, sizeof(struct inode_entry), Sb->inode_table_offset+Sb->inode_size*inode );
}

void mount( int first_used )
{
	if( first_used == 1 ){
		int i;
		for( i = 0; i < 2048; i ++ ){
			char s[8];
			sprintf( s, "dir/%d", i);
			mem_ctrl->block[i] = fopen( s, "wb+" );
		}
		Sb->block_size = 4096;
		Sb->block_number = 2048;
		
		Sb->inode_number = 1000;
		Sb->inode_table_offset = Sb->block_size*2;
		Sb->inode_size = sizeof(struct inode_entry);
		
		Sb->indirect_inode_number = 4000;
		Sb->indirect_inode_table_offset = Sb->block_size*60;
		Sb->indirect_inode_size = sizeof(struct indirect_inode_entry);
		
		Sb->data_number = 1500;
		Sb->data_offset = Sb->block_size*300;
		Sb->data_size = 4096;
		
	}
	else{
	}
}

void unmount(  )
{
	
}

//big bug 


FILE* block_number_to_block( int b )
{
	return mem_ctrl->block[b];
}

int index_to_block_number( int inode_index, int o )
{
	int tot_bnum = (ie->file_size+Sb->block_size-1)/Sb->block_size;
	int s[10], cnt = 0;
	int to = o;
	char buf[1024];
	fs_read( buffer, sizeof(struct inode_entry), \
	inode_table_offset+\
	sizeof(struct inode_entry)*inode_index );
	struct inode_entry e = buf;
	
	if( to > 16+8*16 ){
		int tt = to-8-16*8;
		char buffer[1024];
		fs_read( buffer, sizeof(struct indirect_inode_entry), \
		indirect_inode_table_offset+\
		sizeof(struct indirect_inode_entry)*e->indirect2[tt/256] );
		struct indirect_inode_entry ie = buffer;
		tt %= 256;
		
		int tp = ie->block[tt/16];
		fs_read( buffer, sizeof(struct indirect_inode_entry), \
		indirect_inode_table_offset+\
		sizeof(struct indirect_inode_entry)*tp );
		ie = buffer;
		tt %= 16;
		
		tp = ie->block[tt];	
		
		return tp;
	}
	else if( to > 16 ){
		int tt = to-16;
		char buffer[1024];
		fs_read( buffer, sizeof(struct indirect_inode_entry), \
		indirect_inode_table_offset+\
		sizeof(struct indirect_inode_entry)*e->indirect1[tt/16] );
		struct indirect_inode_entry ie = buffer;
		tt %= 16;
		
		int tp = ie->block[tt];
		return tp;
	}
	else{
		return e->block[to];
	}
}

void pre_indirect_inode_to_block_number( int inode_index, int o, int *a, int *b )
{
	int tot_bnum = (ie->file_size+Sb->block_size-1)/Sb->block_size;
	int s[10], cnt = 0;
	int to = o;
	char buf[1024];
	fs_read( buffer, sizeof(struct inode_entry), \
	inode_table_offset+\
	sizeof(struct inode_entry)*inode_index );
	struct inode_entry e = buf;
	
	if( to > 16+8*16 ){
		int tt = to-8-16*8;
		char buffer[1024];
		(*a) = e->indirect2[tt/256];
		fs_read( buffer, sizeof(struct indirect_inode_entry), \
		indirect_inode_table_offset+\
		sizeof(struct indirect_inode_entry)*e->indirect2[tt/256] );
		struct indirect_inode_entry ie = buffer;
		tt %= 256;
		
		(*b) = ie->block[tt/16];
		
		return;
	}
	else if( to > 16 ){
		int tt = to-16;
		(*a) = e->indirect1[tt/16];
		return ;
	}
	else{
		return;
	}
}

void reback( int inode )
{
	void buf[1024];
	read_inode( buf, inode );
	struct inode_entry *ie = buf;
	for( int i = 0; i < ie->file_size/Sb->block_size+1; i ++ ){
		int bnum = index_to_block_number( inode, i );
		int a = -1, b = -1;
		pre_indirect_inode_to_block_number( inode, i, &a, &b );
		update_bitmap( Sb->data_bitmap, bnum );
		if( a != -1 ) update_bitmap(  Sb->indirect_inode_bitmap, a );
		if( b != -1 ) update_bitmap(  Sb->indirect_inode_bitmap, b );
	}
	update( Sb->inode_bitmap, inode );
}

FILE* inode_to_block( int inode_index, int offset )
{
	int o = offset/Sb->block_size;
	int b = index_to_block_number( inode_index, o );
	return block_number_to_block( b );
}

int lookup( char* name, int inode_dir )
{
	int i, j;
	char buffer[1024];
	fs_read( buffer, sizeof(struct inode_entry), \
	inode_table_offset+sizeof(struct inode_entry)*inode_index );
	struct inode_entry *ie = buffer;
	char dir_buf[8192];
	struct directory dir[1];
	int res = -1;
	for( i = 0; i < ie->file_size/Sb->block_size+1; i ++ ){
		int id = index_to_block_number( inode_dir, i );
		fs_read( dir_buf, Sb->block_size, \
		id*Sb->block_size );
		void *ptr = dir_buf;
		if( i == 0 ){
			dir->entry_number = *((int*)ptr);
			ptr += sizeof(int);
			dir->dir_table = ptr;
		}
		else{
			dir->dir_table = ptr;
		}
		for( j = 0; ptr-dir_buf < Sb->block_size; \
		j ++, ptr += sizeof( struct directory_entry ) ){
			if( strcmp( name, dir->dir_table[j]->name ) == 0 ){
				res = dir->dir_table[j]->inode_number;
				break;
			}
		}
		if( res != -1 ) break;
	}
	return res;
}

int path_to_inode_number( char *name, int dir_inode )
{
	int n = strlen(name), i, j, k, fl = 0;
	if( n == 0 ){
		return Sb->now_inode;
	}
	for( i = 0; i < n; i ++ ){
		if( s[i] == '/' ){
			fl = 1;
			break;
		}
	}
	if( !fl ){
		return lookup( name, dir_inode );
	}
	else{
		char buf[32];
		for( i = 0; i < n; i ++ ){
			if( s[i] == '/' ) break;
		}
		strcpy( buf, name, i ); buf[i] = '\0';
		int tmp = lookup( buf, dir_inode );
		name += i+1;
		return path_to_inode_number( name, tmp );
	}
}

int generalpath_to_inode_number( char *name )
{
	if( name[0] == '/' ){
		return path_to_inode_number( name+1, Sb->root_inode );
	}
	else return path_to_inode_number( name, Sb->now_inode );
}

void add_one_block( struct inode_entry *ie )
{
	int num = ie->file_size/Sb->block_size;
	if( num < 16 ){
		ie->block[num] = query_bitmap( Sb->data_bitmap );
	}
	else if( num < 16+8*16 ){
		int to = num-8;
		if( to%16 == 0 ){
			int no = query_bitmap( Sb->indirect_inode_bitmap );
			char buf[1024];
			fs_read( buf, sizeof( struct indirect_inode_entry ), \
			sizeof( struct indirect_inode_entry )*no);
			
			struct indirect_inode_entry *iie = buf;
			iie->block[to%16] = query_bitmap( Sb->data_bitmap );
			ie->indirect1[to/16] = no;
			
			fs_write( buf, sizeof( struct indirect_inode_entry ), \
			sizeof( struct indirect_inode_entry )*no);
		}
		else{
			int no = ie->indirect1[to/16];
			char buf[1024];
			fs_read( buf, sizeof( struct indirect_inode_entry ), \
			sizeof( struct indirect_inode_entry )*no);
			
			struct indirect_inode_entry *iie = buf;
			iie->block[to%16] = query_bitmap( Sb->data_bitmap );
			
			fs_write( buf, sizeof( struct indirect_inode_entry ), \
			sizeof( struct indirect_inode_entry )*no);
		}
	}
	else{
		int to = num-16-8*16;
		if( to%256 == 0 ){
			int no1 = query_bitmap( Sb->indirect_inode_bitmap );
			int no2 = query_bitmap( Sb->indirect_inode_bitmap );
			char buf1[1024], buf2[1024];
			fs_read( buf1, sizeof( struct indirect_inode_entry ), \
			sizeof( struct indirect_inode_entry )*no1);
			fs_read( buf2, sizeof( struct indirect_inode_entry ), \
			sizeof( struct indirect_inode_entry )*no2);
			
			struct indirect_inode_entry *iie1 = buf1, *iie2 = buf2;
			ie->indirect2[to/256] = no1;
			iie1->block[0] = no2;
			iie2->block[0] = query_bitmap( Sb->data_bitmap );
			
			fs_write( buf1, sizeof( struct indirect_inode_entry ), \
			sizeof( struct indirect_inode_entry )*no1);
			fs_write( buf2, sizeof( struct indirect_inode_entry ), \
			sizeof( struct indirect_inode_entry )*no2);
		}
		else if( to%16 == 0 ){
			int no1 = ie->indirect2[to/256];
			int no2 = query_bitmap( Sb->indirect_inode_bitmap );
			char buf1[1024], buf2[1024];
			fs_read( buf1, sizeof( struct indirect_inode_entry ), \
			sizeof( struct indirect_inode_entry )*no1);
			fs_read( buf2, sizeof( struct indirect_inode_entry ), \
			sizeof( struct indirect_inode_entry )*no2);
			
			struct indirect_inode_entry *iie1 = buf1, *iie2 = buf2;
			iie1->block[(to/16)%16] = no2;
			iie2->block[0] = query_bitmap( Sb->data_bitmap );
			
			fs_write( buf1, sizeof( struct indirect_inode_entry ), \
			sizeof( struct indirect_inode_entry )*no1);
			fs_write( buf2, sizeof( struct indirect_inode_entry ), \
			sizeof( struct indirect_inode_entry )*no2);
		}
		else{
			int no1 = ie->indirect2[to/256];
			char buf1[1024], buf2[1024];
			fs_read( buf1, sizeof( struct indirect_inode_entry ), \
			sizeof( struct indirect_inode_entry )*no1);
			
			struct indirect_inode_entry *iie1 = buf1, *iie2 = buf2;
			int no2 = iie1->block[(to/16)%16];
			fs_read( buf2, sizeof( struct indirect_inode_entry ), \
			sizeof( struct indirect_inode_entry )*no2);
			
			iie2->block[0] = query_bitmap( Sb->data_bitmap );
			
			fs_write( buf1, sizeof( struct indirect_inode_entry ), \
			sizeof( struct indirect_inode_entry )*no1);
			fs_write( buf2, sizeof( struct indirect_inode_entry ), \
			sizeof( struct indirect_inode_entry )*no2);
		}
	}
}

void insert_dir( char *name, int inode_dir, int ins )
{
	char buf[32];
	memset( buf, '\0', sizeof(buf) );
	memcpy( buf, name, 16 );
	memcpy( buf+16, &ins, 4 )
	
	char buffer[4096], buffer2[4096];
	struct inode_entry *ie;
	fs_read( buffer, Sb->inode_size, Sb->inode_table_offset+Sb->inode_size*inode_dir );
	ie = buffer;
	
	int bnum = ie->block[0];
	
	fs_read( buffer2, Sb->block_size, Sb->data_offset+Sb->block_size*bnum );
	void *ptr = buffer2;
	int num = *((int*)ptr);
	if( sizeof(struct directory_entry) != 20 ) { puts("error"); exit(1); }
	fs_write( buf, sizeof(struct directory_entry), \
	Sb->data_offset+Sb->block_size*bnum+sizeof(int)+num*sizeof(struct directory_entry) );
}

int create_file( char *name, int size, int type )//type 0 file 1 dir
{
	int i, n;
	char buf[32];
	n = strlen(name);
	for( i = n-1; i >= 0; i -- ){
		if( name[i] == '/' )
			break;
	}
	strcpy( buf, name+i+1, n-i-1 );
	name[i] = '\0';
	n = strlen(name);
	int inode_id = generalpath_to_inode_number( name );
	if( inode_id == -1 ){
		return -1;
	}
	int new_inode = query_bitmap( Sb->inode_bitmap );
	insert_dir( buf, inode_id, new_inode );
	int bsize = Sb->block_size;
	int num = (size+bsize-1)/bsize;
	
	void buf[1024];
	fs_read( buf, sizeof( struct inode_entry ), \
	Sb->inode_table_offset+sizeof( struct inode_entry )*new_inode );
	struct inode_entry ie = buf;
	
	ie->file_size = 0;
	ie->type = type;
	ie->refcnt = 1;
	
	for( i = 0; i < num; i ++ ){
		add_one_block( ie );
	}
	
	fs_write( buf, sizeof( struct inode_entry ), \
	Sb->inode_table_offset+sizeof( struct inode_entry )*new_inode );

	
}

int open( char *name )
{
	int inode_id = generalpath_to_inode_number( name );
	if( inode_id == -1 ){
		inode_id = create_file( name, 4096, 0 );
		if( inode_id == -1 ) printf("create file failed\n");
	}
	int i, j;
	for( i = 0; i < 1024; i ++ ){
		if( ft->table[i]->refcnt == 0 ) continue;
		if( ft->table[i]->inode_num == inode_id ){
			printf("file has been opened\n");
			break;
		}
	}
	if( i == 1024 ){
		for( j = 0; j < 1024; j ++ ){
			if( ft->table[j]->refcnt == 0 ){
				break;
			}
		}
		if( j == 1024 ) printf("file table entry run out\n");
		else{
			ft->table[j]->refcnt ++;
			ft->table[j]->inode_num = inode_id;
			ft->table[j]->file_cursor = 0;
			fd->table[(++fd->num)%1024]->index = j;
			return fd->num;
		}
	}
	return 0;
}

int Read( int f, int length, char *buf )
{
	int ff = fd->table[f]->index;
	int inode_num = ft->table[ff]->inode_num;
	
	// no deal with bug
	char buffer[1024];
	fs_read( buffer, sizeof(struct inode_entry), \
	Sb->inode_table_offset+sizeof(struct inode_entry)*inode_num );
	struct inode_entry *ie = buffer;
	
	for( int i = ft->table[ff]->file_cursor/Sb->block_size; \
		i < min( ft->table[ff]->file_cursor+length, ie->file_size )\
		/Sb->block_size+1; i ++ ){
		int bnum = index_to_block_number( inode_num, i );
		fs_read( buf, Sb->block_size, \
		Sb->data_offset+bnum*Sb->block_size );
		buf += Sb->block_size;
	}
	ft->table[ff]->file_cursor += length;
	ft->table[ff]->file_cursor = min( ft->table[ff]->file_cursor, ie->file_size );
	return 0;
}

int Write( int f, int length, char *buf )
{
	int ff = fd->table[f]->index;
	int inode_num = ft->table[ff]->inode_num;
	
	char buffer[1024];
	fs_read( buffer, sizeof(struct inode_entry), \
	Sb->inode_table_offset+sizeof(struct inode_entry)*inode_num );
	struct inode_entry *ie = buffer;
	
	for( int i = ft->table[ff]->file_cursor/Sb->block_size; \
		i < min( ft->table[ff]->file_cursor+length, ie->file_size )\
		/Sb->block_size+1; i ++ ){
		int bnum = index_to_block_number( inode_num, i );
		fs_write( buf, Sb->block_size, \
		Sb->data_offset+bnum*Sb->block_size );
		buf += Sb->block_size;
	}
	ft->table[ff]->file_cursor += length;
	return 0;
}

int link( char *name, char *dest )
{
	int inode_dest = generalpath_to_inode_number( dest );
	int i, n;
	char buf[32];
	n = strlen(name);
	for( i = n-1; i >= 0; i -- ){
		if( name[i] == '/' )
			break;
	}
	strcpy( buf, name+i+1, n-i-1 );
	name[i] = '\0';
	int inode_id = generalpath_to_inode_number( name );
	insert_dir( buf, inode_id, inode_dest );
	
	void buffer[4096];
	fs_read( buffer, sizeof(struct inode_entry), \
	Sb->inode_table_offset+sizeof(struct inode_entry)*inode_dest );
	
	struct inode_entry *ie = buffer;
	ie->refcnt ++;
	
	fs_write( buffer, sizeof(struct inode_entry), \
	Sb->inode_table_offset+sizeof(struct inode_entry)*inode_dest );
}

int unlink( char *name )
{
	int inode_file = generalpath_to_inode_number( name );
	int i, n, j, k;
	char buf[32];
	memset( buf, '\0', sizeof(buf) );
	n = strlen(name);
	for( i = n-1; i >= 0; i -- ){
		if( name[i] == '/' )
			break;
	}
	strcpy( buf, name+i+1, n-i-1 );
	
	int inode_dir = generalpath_to_inode_number( name );
	char buffer[4096], buffer2[4096];
	struct inode_entry *ie;
	fs_read( buffer, Sb->inode_size, Sb->inode_table_offset+Sb->inode_size*inode_dir );
	ie = buffer;
	
	int bnum = ie->block[0];
	
	fs_read( buffer2, Sb->block_size, Sb->data_offset+Sb->block_size*bnum );
	void *ptr = buffer2;
	int num = *((int*)ptr);
	ptr += sizeof(int);
	for( j = 0; j < num; j ++ ){
		struct directory_entry *de = ptr;
		if( strcmp( de->name, buf ) == 0 ){
			break;
		}
		ptr += sizeof( struct directory_entry );
	}
	for( ; j < num-1; j ++ ){
		memcpy( ptr, ptr+sizeof( struct directory_entry ),\
		sizeof( struct directory_entry ) );
		ptr += sizeof( struct directory_entry );
	}
	ptr = buffer2;
	-- num;
	*((int*)ptr) = num;
	
	fs_write( buffer, sizeof(struct directory_entry), \
	Sb->data_offset+Sb->block_size*bnum+sizeof(int)+num*sizeof(struct directory_entry) );
	
	fs_read( buffer2, sizeof(struct inode_entry), Sb->inode_table_offset+Sb->inode_size*inode_file );
	ie = buffer2;
	ie->refcnt --;
	if( ie->refcnt != 0 )
		fs_write( buffer2, sizeof(struct inode_entry), Sb->inode_table_offset+Sb->inode_size*inode_file );
	else{
		reback( inode_file );
	}
}

void Rename( char *name, char *new_name )
{
	link( new_name, name );
	unlink( name );
}

void Delete( char *name )
{
	//only file
	unlink( name );
}

int reset_cursor( int f )
{
	int ff = fd->table[f]->index;
	ft->table[ff]->file_cursor = 0;
}

int main()
{
	
	return 0;
}