#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>

#include "fs.h"
#include "types.h"
#define T_DIR  1   // Directory
#define T_FILE 2   // File
#define dirsize BSIZE/sizeof(struct dirent)

struct stat fs;
struct superblock *supb;
struct dinode *first_in;
struct dinode *this;
void* fs_img;

int* bitmap; // the use of bitmap
uint* inode_map; // used inode
int* data_usage; // used data block
int* inode_ref_cnt; // how many times the inode is referred
int* visited; // in traverse, mark as traversed
// if inode is used in inode_map, but not referred in the dir, collect it

int mode; // repair or not

void print_error(char* err){
    if (mode==0){
        fprintf(stderr, "%s\n", err);
        exit(1);
    }
}

void init(char* arg){
    int fsfd;
    if (mode){
        fsfd = open(arg, O_RDWR);
    } else{
        fsfd = open(arg, O_RDONLY);
    }

    if (fsfd < 0){
        print_error("image not found.");
    }
    // struct allocation
    fstat(fsfd, &fs);

    if (mode){
        fs_img = mmap(NULL,fs.st_size, PROT_READ|PROT_WRITE,MAP_SHARED,fsfd,0);
    } else{
        fs_img = mmap(NULL,fs.st_size,PROT_READ,MAP_PRIVATE,fsfd,0);
    }
    supb = (struct superblock*)(fs_img+BSIZE);
    first_in = (struct dinode*)(fs_img+2*BSIZE);
    this = first_in + 2;
    // check3
    if ((first_in + 1)->type != 1){
        print_error("ERROR: root directory does not exist.");
    } // check its parent
    close(fsfd);
}

void check1(struct dinode* current) {
    if (current->type < 0 || current->type > 3){
        print_error("ERROR: bad inode.");
    }
}

void check257addr(uint addr, int type){
    // check2 direct address
    if (addr < 0 || addr > 1023){
        print_error("ERROR: bad direct address in inode.");
    }
    // check5
    if (bitmap[addr] == 0){
        print_error("ERROR: address used by inode but marked free in bitmap.");
    }
    // check7,12
    if (addr != 0 && data_usage[addr] >= 1) {
        if (type == 1)
            print_error("ERROR: directory appears more than once in file system.");
        else
            print_error("ERROR: direct address used more than once.");
    }
    if(addr != 0)
        data_usage[addr] = 1;
}

/**
 * check that the address(block) in the inode is in range,
 * and every block is referred by unique inode,
 * and if the block is marked as used in bitmap
 * @param current checked inode
 */
void check2_5_7(struct dinode* current){
    int tp = current->type;
    for (int i = 0; i < NDIRECT + 1; ++i) { //todo: if +1 correct
        check257addr(current->addrs[i],tp);
    }
    uint* indirect_block = fs_img + current->addrs[NDIRECT]*BSIZE; // todo: check fsimg is the correct
    for (int j = 0; j < NINDIRECT; ++j) {
        if (*indirect_block < 0 || *indirect_block >1023)
            print_error("ERROR: bad indirect address in inode.");
        if (data_usage[*indirect_block] == 1 && *indirect_block != 0)
            print_error("ERROR: indirect address used more than once.");
        if(bitmap[*indirect_block] == 0){
            print_error("ERROR: address used by inode but marked free in bitmap.");
        }
        if (*indirect_block != 0)
            data_usage[*indirect_block] = 1;
        indirect_block++;
    }
}

// return -1 if visit a file, positive or zero if visit a dir
int dir_traverse(int inum, int parent_inum){
    // check this dir's function
    struct dinode* node = first_in+inum;
    check1(node);
    // check 10
    if (node->type == 0)
        print_error("ERROR: inode referred to in directory but marked free.");
    inode_ref_cnt[inum]++; // mark as visited
    if (node->type == T_FILE && visited[inum] > 0){ // check non-visited file
        return -1;
    }
    check2_5_7(node);
    visited[inum]=1; // todo: whether this should be 1

    // further traverse
    if (node->type == T_DIR){
        int parent = 0, self = 0; // .. and .
        // check12
        if (inode_ref_cnt[inum] > 1){
            print_error("ERROR: directory appears more than once in file system.");
        }

        // lop the 12 direct node
        for (int dino = 0; dino < NDIRECT; ++dino) {
            if (node->addrs[dino] == 0)
                continue;
            struct dirent* dir = (struct dirent*)(fs_img + (BSIZE*(node->addrs[dino])));
            // check dir
            for (int i = 0; i < dirsize; i++) {
                if ((dir+i)->inum <= 0)
                    continue;
                if (strcmp((dir+i)->name,"..") == 0){
                    parent = 1;
                    if ((dir+i)->inum != parent_inum)
                        print_error(inum == 1 ? "ERROR: root directory does not exist.": "ERROR: parent directory mismatch.");
                    continue;
                }
                if (strcmp((dir+i)->name,".") == 0) {
                    self = 1;
                    continue;
                }
                int result = dir_traverse((dir+i)->inum,inum);
                if (result == 0 || result == 1)
                    print_error("ERROR: directory not properly formatted.");
            }
        }
        // for indirect node
        uint* indirect_block = fs_img + node->addrs[NDIRECT]*BSIZE;
        for (int j = 0; j < NINDIRECT; ++j) {
            uint temp = *(indirect_block+j);
            if (temp == 0) continue;
            struct dirent* in_dir = (struct dirent*)(fs_img + BSIZE*temp);
            // check dir
            for (int i = 0; i < dirsize; i++) {
                if ((in_dir+i)->inum <= 0)
                    continue;
                if (strcmp((in_dir+i)->name,"..") == 0){
                    parent = 1;
                    if ((in_dir+i)->inum != parent_inum)
                        print_error(parent_inum == 1 ? "ERROR: root directory does not exist.": "ERROR: parent directory mismatch.");
                    continue;
                }
                if (strcmp((in_dir+i)->name,".") == 0) {
                    self = 1;
                    continue;
                }
                int result = dir_traverse((in_dir+i)->inum,inum);
                if (result == 0 || result == 1)
                    print_error("ERROR: directory not properly formatted.");
            }
        }
        return parent + self;
    }
    return -1;
}

ushort find_block(){
    struct dinode* root = first_in + 1;
    for (int i = 0; i < NDIRECT; ++i) {
        if (root->addrs[i] ==0)
            continue;
        struct dirent* node = (struct dirent*)(fs_img + root->addrs[i]*BSIZE);
        for (int j = 0; j < dirsize; ++j) {
            if((node+j)->inum == 0)
                continue;
            if (strcmp((node+j)->name,"lost_found") == 0)
                return (node+j)->inum;
        }
    }
    uint* indirect_block = fs_img + root->addrs[NDIRECT]*BSIZE;
    for (int j = 0; j < NINDIRECT; ++j) {
        uint temp = *(indirect_block + j);
        if (temp == 0) continue;
        struct dirent *in_dir = (struct dirent *) (fs_img + BSIZE * temp);
        // check dir
        for (int k = 0; k < dirsize; ++k) {
            if ((in_dir + j)->inum == 0)
                continue;
            if (strcmp((in_dir + k)->name, "lost_found") == 0)
                return (in_dir + k)->inum;
        }
    }
}

void repair_inode(struct dinode* l_f, ushort nodeid, ushort inum){
    int finished = 0;
    for (int i = 0; i < NDIRECT && (!finished); ++i) {
        if (l_f->addrs[i] ==0)
            continue;
        struct dirent* node = (struct dirent*)(fs_img + l_f->addrs[i]*BSIZE);
        for (int j = 0; j < dirsize; ++j) {
            if((node+j)->inum == 0){
                strcpy((node+j)->name,"miss"); // todo: add the name???
                (node+j)->inum = nodeid;
                finished = 1;
                break;
            }
        }
    }

    if (!finished){
        uint* indirect_block = fs_img + l_f->addrs[NDIRECT]*BSIZE;
        for (int i = 0; i < NINDIRECT && (!finished); ++i) {
            uint temp = *(indirect_block + i);
            if (temp == 0) continue;
            struct dirent *in_dir = (struct dirent *) (fs_img + BSIZE * temp);
            for (int j = 0; j < dirsize; ++j) {
                if((in_dir+j)->inum == 0){
                    strcpy((in_dir+j)->name,"miss"); // todo: add the name???
                    (in_dir+j)->inum = nodeid;
                    finished = 1;
                    break;
                }
            }
        }
    }

    for (int i = 0; i < NDIRECT; ++i) {
        if ((first_in+nodeid)->addrs[i] ==0)
            continue;
        struct dirent* miss_node = (struct dirent*)(fs_img + (first_in+nodeid)->addrs[i]*BSIZE);
        for (int j = 0; j < dirsize; ++j) {
            if ((miss_node+j)->inum == 0) continue;
            if(strcmp((miss_node+j)->name,"..") == 0){
                (miss_node+j)->inum = inum;
                return;
            }
        }
    }

    uint* indi_b = fs_img + (first_in+nodeid)->addrs[NDIRECT]*BSIZE;
    for (int i = 0; i < NINDIRECT; ++i) {
        uint temp = *(indi_b + i);
        if (temp == 0) continue;
        struct dirent *in_dir = (struct dirent *) (fs_img + BSIZE * temp);
        for (int j = 0; j < dirsize; ++j) {
            if ((in_dir+j)->inum == 0) continue;
            if(strcmp((in_dir+j)->name,"..") == 0){
                (in_dir+j)->inum = inum;
                return;
            }
        }
    }

}

int main(int argc, char* argv[]){
    if (argc == 1){
        print_error("Usage: xcheck <file_system_image>");
    }
    if (argc == 3){
        mode = 1;
        init(argv[2]);
    } else{
        init(argv[1]);
    }
    char* bitmap_block_start = (char *) (fs_img + (supb->ninodes / IPB + 3) * BSIZE);

    bitmap = calloc(supb->size, sizeof(int)); // bitmap that the data block is marked as used
    inode_map = (uint*)calloc(supb->ninodes, sizeof(uint)); // that the inode is marked as used
    data_usage = calloc(supb->size, sizeof(int)); // data block marked as used
    inode_ref_cnt = calloc(supb->ninodes, sizeof(int));
    visited = calloc(supb->ninodes, sizeof(int));

    // construct a bitmap value array
    for (int i = 0; i < supb->size;) {
        for (int j = 0; j < 8 && i < supb->size; ++j) {
            bitmap[i] = ((*bitmap_block_start) & (1 << j)) > 0 ? 1 : 0;
            ++i;
        }
        bitmap_block_start++;
    }

    dir_traverse(1,1);

    struct dinode* l_and_f;
    ushort lf_inode;
    int need_repaired[supb->ninodes];
    if (mode){
        lf_inode = find_block();
        l_and_f = first_in + lf_inode;
        memset(need_repaired, 0, supb->ninodes* sizeof(int)); //todo: correct?
    }
    for (int i = 2; i < supb->ninodes; ++i) {
        check1(this);
        inode_map[i] = this->type > 0 ? 1 : 0; // todo: optimize
        if (inode_map[i] && visited[i] == 0){
            if (mode){
               need_repaired[i] = 1;
            }
            if (this->type == T_DIR){
                print_error("ERROR: inaccessible directory exists.");
            } else {
               print_error("ERROR: inode marked use but not found in a directory.");
            }
        }
        if (inode_map[i] && inode_ref_cnt[i] != this->nlink){
           print_error("ERROR: bad reference count for file.");
        }
        this++;
    }
   for (int i =  (supb->ninodes/IPB + supb->nblocks/BPB + 4); i < supb->size; i++){
        //check6
        if (bitmap[i] >= 1 && data_usage[i] == 0)
            print_error("ERROR: bitmap marks block in use but it is not in use.");
    }
    if (mode){
        for (int i = 0; i < supb->ninodes; ++i) {
            if (need_repaired[i] == 1){
                if ((first_in + i)->type == T_DIR){
                    struct dinode* temp = first_in + i;
                    for (int j = 0; j < NDIRECT; ++j) {
                        if (temp->addrs[j] ==0)
                            continue;
                        struct dirent* node = (struct dirent*)(fs_img + temp->addrs[j]*BSIZE);
                        for (int k = 0; k < dirsize; ++k) {
                            if((node+k)->inum == 0 || (node+k)->inum == i)
                                continue;
                            if(need_repaired[(node+k)->inum] == 1){
                                need_repaired[(node+k)->inum] = 0;
                            }
                        }
                    }
                    uint* indirect_block = fs_img + temp->addrs[NDIRECT]*BSIZE;
                    for (int j = 0; j < NINDIRECT; ++j) {
                        uint ind_tmep = *(indirect_block + j);
                        if (ind_tmep == 0) continue;
                        struct dirent *in_dir = (struct dirent *) (fs_img + BSIZE * ind_tmep);
                        // check dir
                        for (int k = 0; k < dirsize; ++k) {
                            if ((in_dir + k)->inum == 0 || (in_dir+k)->inum == i)
                                continue;
                            if(need_repaired[(in_dir+k)->inum] == 1){
                                need_repaired[(in_dir+k)->inum] = 0;
                            }
                        }
                    }
                }
            }
        }
        for (int l = 0; l < supb->ninodes; ++l) {
            if (need_repaired[l] == 1){
                repair_inode(l_and_f, l, lf_inode);
            }
        }
    }

    exit(0);
}
