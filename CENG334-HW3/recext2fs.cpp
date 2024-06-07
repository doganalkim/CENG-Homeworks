#include "ext2fs.h"
#include "ext2fs_print.h"
#include "identifier.h"
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <iostream>
#include <unordered_map>
#include <set>

#define KB 1024
#define S(x) sizeof(x)
#define ULI unsigned long long int

uint8_t *Identifier = NULL; 
struct ext2_super_block SB;
struct ext2_inode root_inode;  // this is the root inode
char *img;
int BlockSize = 1024, NumbOfGd = 0, FirstGdBlock = 0, FirstSBPtr = -1, NumbOfPtrs = 0, firstGdOffset = 0, img_fd, lastGroupBlockCount;

// imzalar
void dfs(struct ext2_inode *in, int level, std::string name);
void mark_inode_as_used(int bm, int inode_numb);
void print_bitmap(int bm, int ind);
void mark_block_as_used(unsigned int bid);
void mark_blocks_pointed_by_inode(struct ext2_inode inode);
bool is_data_block(int bid);
bool is_dir_entry_block(int bid);

void read_super_block(){
    if(lseek(img_fd,KB,SEEK_SET) == -1){ 
        perror("Failed to seek to superblock");
        close(img_fd);
        exit(1);
    }
    read(img_fd,&SB,S(SB)); // we read and save the super block
    BlockSize = BlockSize << SB.log_block_size;
    NumbOfGd = (SB.block_count +  SB.blocks_per_group - 1 ) / SB.blocks_per_group; // number of Groups
    FirstSBPtr = ( 2 * KB + BlockSize - 1 ) / BlockSize;
    FirstGdBlock = (2 * KB + BlockSize - 1) / BlockSize;
    firstGdOffset = FirstGdBlock * BlockSize;
    NumbOfPtrs = BlockSize / 4; // this is the number of pointers per block
    lastGroupBlockCount = SB.blocks_per_group;
    if( (SB.block_count % SB.blocks_per_group) ) lastGroupBlockCount % SB.blocks_per_group;
    return;
}

void read_first_gd(){
    struct ext2_block_group_descriptor gd;
    if(lseek(img_fd, firstGdOffset ,SEEK_SET) == -1){
        perror("Failed to seek group descriptor");
        close(img_fd);
        exit(1);
    }
    read(img_fd,&gd,S(gd));                                                     // we got the first group descriptor, root must be located here
    for(int j = 0; j <  SB.first_inode ; j++) mark_inode_as_used(gd.inode_bitmap,j+1);             // mark the first 10 inodes as used
    print_bitmap(gd.inode_bitmap,0);
    if(lseek(img_fd,gd.inode_table * BlockSize + SB.inode_size,SEEK_SET) == -1){ //  find the root inode
        perror("Failed to seek to root inode");
        close(img_fd);
        exit(1);
    } 
    read(img_fd,&root_inode, S(root_inode) );
}

struct ext2_inode  inode_finder_helper(int inode_id){   // given the inode, find its block. Return pointor to the inode
    struct ext2_block_group_descriptor gd;
    int gn =  inode_id  / SB.inodes_per_group  ;        // get the corresponding group number
    int offset = firstGdOffset + gn * S(gd ); 
    lseek(img_fd, offset  , SEEK_SET );                 // given the inode number, go to the related group descriptor
                                                        // the first KB is for Bootblock, the second is for the Super Block
    read(img_fd,&gd,S(gd));                             // we have found the  first group descriptor 
    lseek(img_fd, gd.inode_table * BlockSize + SB.inode_size * ( (inode_id-1)   %  SB.inodes_per_group )  , SEEK_SET); // go to the inode table
    struct ext2_inode in;
    read(img_fd, &in, S(struct ext2_inode) );
    return in;
}

void name_return(int &cur_loc, uint8_t &len, char *name){
    lseek(img_fd, cur_loc + 8, SEEK_SET); //  lseek for the name
    memset(name,0,S(char) * len + 1); // initalize with 0
    read(img_fd,name,len * S(char)); // read the name
}

void inode_finder(int bid,int level){                         // given the block id, go to that node
    int blockLocation = bid * BlockSize;                      // arrange the block location as blocknumber * BlockSize
    lseek(img_fd, blockLocation  ,SEEK_SET);                  // go to the block
    int offset = 0;
    while(offset < BlockSize){
        int cur_location = blockLocation + offset;
        lseek(img_fd, cur_location, SEEK_SET);                // lseak for the current directory entry
        struct ext2_dir_entry de;
        read(img_fd,&de,S(de));                               // read the current dir entry
        offset +=  de.length;
        if(de.inode == 0) break;                              // invalid inode pointed by directory entry 
        char name[de.name_length + 1];
        name_return(cur_location,de.name_length,name);
        struct ext2_inode in = inode_finder_helper(de.inode); // find the inode correspoind the inode number
        if(strncmp(name,".",de.name_length) != 0 && strncmp(name,"..",de.name_length) != 0 )   dfs(&in,level + 1,name); // call dfs recursively
    }
}

void handle_pointers(int bid,int level, int pointer_type){
    // to do NOT SURE IF IT IS WORKING OR NOT SINCE THERE IS NO TESTCASE FOR IT
    int  blockLocation = bid*BlockSize;
    unsigned int block_no;
    int offset = 0;
    for(int i = 0; i < NumbOfPtrs ; i++){
        int cur_location =  blockLocation + offset;
        lseek( img_fd, cur_location, SEEK_SET);
        read(img_fd, &block_no, S(unsigned int)); // now, we have read the block no
        offset += S(unsigned int);  
        if( block_no  == 0) continue;
        if(pointer_type == 0) inode_finder(block_no,level );                       // single indirect
        if(pointer_type) handle_pointers(block_no, level, pointer_type - 1);  // double indirect  and triple indirect
    }
}

// main function for depth first search
void dfs(struct ext2_inode *in, int level, std::string name ){
    for(int i = 0; i < level ; i++) std::cout  << "-";
    std::cout << " " << name;
    if( (in->mode & 0xf000) != EXT2_I_DTYPE ){ // the inode does not contain a directory, it is a file
        std::cout <<  std::endl;
        return;
    }
    std::cout << "/" << std::endl;
                                                // here, this means the inode contains dir entries
    for(int i = 0; i < 12 ; i++){               // direct block handling
        if(in->direct_blocks[i] == 0) continue; // no pointer for here
        int bid = in->direct_blocks[i];
        inode_finder(bid, level); 
    }
    if( in ->  single_indirect != 0 ) {
        handle_pointers(in -> single_indirect,level, 0);
    }
    if( in -> double_indirect != 0 ){
        handle_pointers( in -> double_indirect,level, 1 );
    }
    if( in -> triple_indirect != 0 ) {
        handle_pointers( in -> triple_indirect, level ,2 );
    }
    return;
}

void print_bitmap(int bm, int ind){
    lseek(img_fd,BlockSize * bm , SEEK_SET ); // we reach the block containing bitmap
    uint8_t marker = 1, val;
    int numbOfBytes = (SB.inodes_per_group + 7) / 8;
    for(int i = 0; i < numbOfBytes  ; i++){
        read(img_fd, &val, S(uint8_t));
        //std:: cout << "i:" << i << " val:" << (int)val <<  "!" << std::endl;
    }
}

void mark_inode_as_used(int bm, int inode_numb){
    int offset = (inode_numb-1)/8;
    uint8_t marker = 1 << ((inode_numb - 1) % 8), val;
    lseek(img_fd,BlockSize * bm + offset , SEEK_SET ); // we reach the block containing bitmap
    read(img_fd, &val, S(val) );
    val = val | marker;
    lseek(img_fd,BlockSize * bm + offset , SEEK_SET ); // we reach the block containing bitmap
    //val = 0;
    write(img_fd,&val,1);
}

void handle_group_inode_recovery(int inode_bm, int inode_table  ){
    int location = inode_table * BlockSize, offset = 0;
    for(int i = 0; i < SB.inodes_per_group ; i++){
        lseek(img_fd, location + offset, SEEK_SET);
        struct ext2_inode in;
        read(img_fd, &in, S(in) );
        if(in.deletion_time == 0 && (in.link_count != 0 || in.size != 0) ){    // means that the inode is currently being used, mark it as 1
            mark_inode_as_used(inode_bm,i+1);
            mark_blocks_pointed_by_inode(in);
        }
        offset += SB.inode_size ;
        //print_inode(&in , i+1);
    }
}

int numbOfBlocksForInodeBitmaps;

void inode_recovery(){
    int offset = 0;
    struct ext2_block_group_descriptor gd;
    for(int i = 0; i < NumbOfGd ; i++){ // do it for each group descriptor
        lseek(img_fd,firstGdOffset + offset,SEEK_SET);
        read(img_fd, &gd,S(gd));
        handle_group_inode_recovery(gd.inode_bitmap, gd.inode_table);
        numbOfBlocksForInodeBitmaps = (SB.inodes_per_group * SB.inode_size  + BlockSize - 1) / BlockSize;
        for(int j = 0; j <= numbOfBlocksForInodeBitmaps ; j ++) mark_block_as_used(gd.inode_bitmap+j);
        mark_block_as_used(gd.block_bitmap); mark_block_as_used(gd.inode_bitmap); mark_block_as_used(gd.inode_table);
        for(int j = i*SB.blocks_per_group + SB.first_data_block ; j < gd.block_bitmap; j++ ) mark_block_as_used(j);
        offset += S(gd);
    }
}

void print_block_bitmap(int bm){
    lseek(img_fd,BlockSize * bm , SEEK_SET ); // we reach the block containing bitmap
    uint8_t marker = 1, val;
    int numbOfBytes = (SB.blocks_per_group + 7) / 8;
    for(int i = 0; i < numbOfBytes  ; i++){
        read(img_fd, &val, S(uint8_t));
    }
}

void mark_block_as_used_helper(int bm, int ind){
    int offset = ind / 8;
    uint8_t marker = 1 << (ind % 8), val;
    lseek(img_fd, BlockSize*bm  + offset ,SEEK_SET);
    read(img_fd, &val,1);
    val = val | marker;
    lseek(img_fd, BlockSize*bm  + offset ,SEEK_SET);
    //val = 0;
    write(img_fd , &val , 1);
}

void mark_block_as_used(unsigned int bid){
    int group_no = (bid - SB.first_data_block) /SB.blocks_per_group;          // We have found the group no
    struct ext2_block_group_descriptor gd;
    lseek(img_fd, firstGdOffset + group_no*S(gd)  ,SEEK_SET);
    read(img_fd,&gd,S(gd));
    mark_block_as_used_helper(gd.block_bitmap, ( (bid-SB.first_data_block) % SB.blocks_per_group ) );
}

void mark_blocks_pointed_by_indirects(int bid, int pointer_type){
    unsigned int  offset = 0, val; 
    for(int i = 0 ; i < NumbOfPtrs ; i++){
        lseek(img_fd,bid*BlockSize + offset ,SEEK_SET);
        read(img_fd,&val,S(val));
        offset += S(val);
        if(!val) continue;
        mark_block_as_used(val);
        if(pointer_type){
            mark_blocks_pointed_by_indirects(val,pointer_type-1);
        }
    }
}

void mark_blocks_pointed_by_inode(struct ext2_inode inode){
    for(int i = 0; i < 12 ; i++){
        if(inode.direct_blocks[i]) mark_block_as_used(inode.direct_blocks[i]);
    }
    if(inode.single_indirect){
        mark_block_as_used(inode.single_indirect);
        mark_blocks_pointed_by_indirects(inode.single_indirect,0);
    }
    if(inode.double_indirect){
        mark_block_as_used(inode.double_indirect);
        mark_blocks_pointed_by_indirects(inode.double_indirect,1);
    }
    if(inode.triple_indirect){
        mark_block_as_used(inode.triple_indirect);
        mark_blocks_pointed_by_indirects(inode.triple_indirect,2);
    }
}

bool is_block_empty(int bid){
    lseek(img_fd, BlockSize*bid, SEEK_SET);
    int size = BlockSize / S(ULI);
    ULI arr[size];
    read(img_fd, arr, BlockSize);
    for(int i = 0; i < size ; i++){
        if(arr[i]) return false;
    }
    return true;
}

bool is_pointer_block(int bid){
    unsigned int blockNo, offset  = 0 , limit = SB.block_count - SB.first_data_block;
    while(offset < BlockSize){
        lseek(img_fd, bid*BlockSize + offset, SEEK_SET);
        offset += S(blockNo);
        read(img_fd, &blockNo, S(blockNo));
        if(blockNo > limit){
            return false;
        }
    }
    offset   = 0;
    while(offset < BlockSize){
        lseek(img_fd, bid*BlockSize + offset, SEEK_SET);
        offset += S(blockNo);
        read(img_fd, &blockNo, S(blockNo));
        if(blockNo)
            mark_block_as_used(blockNo);
    }
    return true;
}

void bitmap_recovery(){
    int i = 0;
    if(!SB.block_count) i++;
    for(; i <  SB.block_count; i++){  // do it for each block
        if(is_data_block(i)){ 
            //std::cout  << i  << " is a DATA block" << std::endl;
            mark_block_as_used(i); continue;
        }
        if(is_dir_entry_block(i)){
            //std::cout  << i  << " is an ENTRY block" << std::endl;
            mark_block_as_used(i);  continue;
        }
        if( is_block_empty(i))  {               // block contains no data -> no need to mark it as used
            //std::cout  << i  << " is a EMPTY block" << std::endl;
            continue;
        }
        if( is_pointer_block(i)){
            //std::cout  << i  << " is an POINTER block" << std::endl;
            mark_block_as_used(i);
            continue;
        }
        mark_block_as_used(i);                 // otherwise, mark the block as used 
    }
}

bool is_dir_entry_block(int bid){
    int location = bid*BlockSize;
    struct ext2_dir_entry de;
    int offset = 0;
    while(offset < BlockSize){
        lseek(img_fd, location + offset , SEEK_SET);
        read(img_fd, &de, S(de) );
        offset += de.length;
        if(de.inode > SB.inode_count || de.length < 8  ) return false;
    }
    return true;
}

bool is_data_block(int bid){
    lseek(img_fd, bid*BlockSize, SEEK_SET); 
    uint8_t val;
    for(int i = 0; i < 32 ; i++){
        read(img_fd, &val, 1);
        if(Identifier[i] != val) return false;
    }
    return true;
}

void pointer_recovery(){
    // to do
    int offset = 0;
    struct ext2_block_group_descriptor gd;
    for(int i = 0; i < NumbOfGd ; i++){ // do it for each group descriptor
        lseek(img_fd,firstGdOffset + offset,SEEK_SET);
        read(img_fd, &gd,S(gd));
        offset += S(gd);
    }
}

void arrange_padding_blockbitmap(){                                     // This function is for the arranging padding of the bitmaps
    if(SB.first_data_block) mark_block_as_used(SB.block_count);                    // This line means no need for padding
    if( lastGroupBlockCount == SB.blocks_per_group) return;
    int limit = SB.block_count - lastGroupBlockCount;                   // Start from the last block
    if(!SB.first_data_block) limit--;                                              // Last block is 2^n - 1 if arranger is equal to zero    
    for(int i = SB.block_count ; i > limit; i--) mark_block_as_used(i);
}

int main(int argc, char* argv[]){
    img = argv[1];
    Identifier = parse_identifier(argc,argv);
    img_fd = open(img,O_RDWR);
    read_super_block();
    read_first_gd();
    inode_recovery();                        // DONE
    bitmap_recovery();                       // CHECK THIS PART
    dfs(&root_inode,1,"root");               // DONE (?) 
    arrange_padding_blockbitmap();           // TODO
    //pointer_recovery();                    // TODO
    close(img_fd);
    return 0;
}