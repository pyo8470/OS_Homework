#ifndef __FILESYSTEM_H__
#define __FILESYSTEM_H__

#include "disk.h"
#define INVALID_ENTRY (-1)
// ------- Caution -----------------------------------------
#define FS_DISK_CAPACITY (BLOCK_SIZE * 512) // 512 block

#define NUM_OF_DIRECT_BLOCK_PTR (4) /* direct block pointer의 개수 */
#define MAX_NAME_LEN (28)

#define FILESYS_INFO_BLOCK (0)         /* file system info block no. */
#define INODE_BYTEMAP_BLOCK_NUM (1)    /* inode bytemap block no. */
#define BLOCK_BYTEMAP_BLOCK_NUM (2)    /* block bytemap block no. */
#define INODELIST_BLOCK_FIRST (3)      /* the first block no. of inode list */
#define INODELIST_BLOCKS (4)           /* the number of blocks in inode list */
#define INODE_PER_INODELIST_BLOCK (16) /* the number of Inode in Block */
// ----------------------------------------------------------
// DirEntry size : 32byte;
typedef struct __dirEntry {
    char name[MAX_NAME_LEN]; // file name
    int inodeNum;
} DirEntry;
//
// Inode size : 32Bytes
typedef struct _Inode {
    int allocBlocks;                          // 0
    int size;                                 // 1
    int type;                                 // 2
    int dirBlockPtr[NUM_OF_DIRECT_BLOCK_PTR]; // block pointers 3,4,5,6
    int indirectBlockPtr;                     // indirect block pointer 7
} Inode;

void FileSysInit(void);
void SetInodeBytemap(int inodeno);
void ResetInodeBytemap(int inodeno);
void SetBlockBytemap(int blkno);
void ResetBlockBytemap(int blkno);
void PutInode(int inodeno, Inode *pInode);
void GetInode(int inodeno, Inode *pInode);
int GetFreeInodeNum(void);
int GetFreeBlockNum(void);
void PutIndirectBlockEntry(int blkno, int index, int number);
void RemoveIndirectBlockEntry(int blkno, int index);
int GetIndirectBlockEntry(int blkno, int index);
void PutDirEntry(int blkno, int index, DirEntry *pEntry);
int GetDirEntry(int blkno, int index, DirEntry *pEntry);
void RemoveDirEntry(int blkno, int index);

#endif /* FILESYSTEM_H_ */
