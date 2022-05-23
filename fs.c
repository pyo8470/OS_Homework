#include "fs.h"
#include "disk.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
void FileSysInit(void) {
    // create and open the virtual(logical) disk to manage
    DevCreateDisk();
    DevOpenDisk();

    char *pBuf = (char *)malloc(BLOCK_SIZE); // allocated Block size Memory
    memset(pBuf, 0, BLOCK_SIZE);             //  initialized to 0
    free(pBuf);
}
// set Bytemap Byte to 1
void SetInodeBytemap(int inodeno) {
    char *pBuf = (char *)malloc(BLOCK_SIZE); // allocated Block size Memory
    DevReadBlock(INODE_BYTEMAP_BLOCK_NUM, pBuf);
    pBuf[inodeno] = 1;
    DevWriteBlock(INODE_BYTEMAP_BLOCK_NUM, pBuf);
    free(pBuf);
}
// set Bytemap Byte to 0
void ResetInodeBytemap(int inodeno) {
    char *pBuf = (char *)malloc(BLOCK_SIZE); // allocated Block size Memory
    DevReadBlock(INODE_BYTEMAP_BLOCK_NUM, pBuf);
    pBuf[inodeno] = 0;
    DevWriteBlock(INODE_BYTEMAP_BLOCK_NUM, pBuf);
    free(pBuf);
}

void SetBlockBytemap(int blkno) {
    char *pBuf = (char *)malloc(BLOCK_SIZE); // allocated Block size Memory
    DevReadBlock(BLOCK_BYTEMAP_BLOCK_NUM, pBuf);
    pBuf[blkno] = 1;
    DevWriteBlock(BLOCK_BYTEMAP_BLOCK_NUM, pBuf);
    free(pBuf);
}

void ResetBlockBytemap(int blkno) {
    char *pBuf = (char *)malloc(BLOCK_SIZE); // allocated Block size Memory
    DevReadBlock(BLOCK_BYTEMAP_BLOCK_NUM, pBuf);
    pBuf[blkno] = 0;
    DevWriteBlock(BLOCK_BYTEMAP_BLOCK_NUM, pBuf);
    free(pBuf);
}
// 16 Inode per Block
void PutInode(int inodeno, Inode *pInode) {
    char *pBuf = (char *)malloc(BLOCK_SIZE); // allocated Block size Memory
    int BlockNum = (inodeno / INODE_PER_INODELIST_BLOCK) +
                   INODELIST_BLOCK_FIRST; // location of blocknum of inode
    DevReadBlock(BlockNum, pBuf);
    int inode_offset = inodeno % INODE_PER_INODELIST_BLOCK;

    memcpy(pBuf + (inode_offset * sizeof(Inode)), pInode, sizeof(Inode));

    DevWriteBlock(BlockNum, pBuf);
    free(pBuf);
}

void GetInode(int inodeno, Inode *pInode) {
    char *pBuf = (char *)malloc(BLOCK_SIZE); // allocated Block size Memory
    int BlockNum = (inodeno / INODE_PER_INODELIST_BLOCK) +
                   INODELIST_BLOCK_FIRST; // location of blocknum of inode
    DevReadBlock(BlockNum, pBuf);

    int inode_offset = inodeno % INODE_PER_INODELIST_BLOCK;

    memcpy(pInode, pBuf + inode_offset * sizeof(Inode), sizeof(Inode));
    // pBuf(first) + inode_offset(Byte)*32Byte ->
    // Corresponding inode
    free(pBuf);
}

int GetFreeInodeNum(void) {
    char *pBuf = (char *)malloc(BLOCK_SIZE); // allocated Block size Memory
    DevReadBlock(INODE_BYTEMAP_BLOCK_NUM, pBuf);
    int free_index = 0;
    // loop num of Inode
    for (int i = 0; i < INODE_PER_INODELIST_BLOCK * INODELIST_BLOCKS; i++) {
        if (pBuf[i] == 0) {
            free_index = i;
            free(pBuf);
            return free_index;
        }
    }
}

int GetFreeBlockNum(void) {

    char *pBuf = (char *)malloc(BLOCK_SIZE); // allocated Block size Memory
    DevReadBlock(BLOCK_BYTEMAP_BLOCK_NUM, pBuf);
    int free_index = 0;
    // loop num of Block
    for (int i = 0; i < BLOCK_SIZE; i++) {
        if (pBuf[i] == 0) {
            free_index = i;
            free(pBuf);
            return free_index;
        }
    }
    return -1;
}

void PutIndirectBlockEntry(int blkno, int index, int number) {
    int entry_num = BLOCK_SIZE / 4;
    int *pBuf = (int *)malloc(
        sizeof(int) * entry_num); // allocated Block size Memory(entry -> 4Byte)
    DevReadBlock(blkno, (char *)pBuf);
    pBuf[index] = number;
    DevWriteBlock(blkno, (char *)pBuf);
    free(pBuf);
}

void RemoveIndirectBlockEntry(int blkno, int index) {
    int entry_num = BLOCK_SIZE / 4;
    int *pBuf = (int *)malloc(sizeof(int) * entry_num);
    DevReadBlock(blkno, (char *)pBuf);
    pBuf[index] = INVALID_ENTRY;
    DevWriteBlock(blkno, (char *)pBuf);
    free(pBuf);
}
int GetIndirectBlockEntry(int blkno, int index) {
    int entry_value = 0;
    int entry_num = BLOCK_SIZE / 4;
    int *pBuf = (int *)malloc(sizeof(int) * entry_num);
    DevReadBlock(blkno, (char *)pBuf);
    entry_value = pBuf[index];
    free(pBuf);
    return entry_value;
}

void PutDirEntry(int blkno, int index, DirEntry *pEntry) {
    int entry_num = BLOCK_SIZE / 32;
    DirEntry *Entry = (DirEntry *)malloc(entry_num * sizeof(DirEntry));
    DevReadBlock(blkno, (char *)Entry);

    memcpy(&Entry[index], pEntry, sizeof(DirEntry));

    DevWriteBlock(blkno, (char *)Entry);
    free(Entry);
}

int GetDirEntry(int blkno, int index, DirEntry *pEntry) {
    int return_V = 1;
    int entry_num = BLOCK_SIZE / sizeof(DirEntry);
    DirEntry *Entry = (DirEntry *)malloc(sizeof(DirEntry) * entry_num);
    DevReadBlock(blkno, (char *)Entry);

    if (Entry[index].inodeNum == INVALID_ENTRY) {
        return_V = -1;
    }
    memcpy(pEntry, &Entry[index], sizeof(DirEntry));

    free(Entry);
    return return_V;
}

void RemoveDirEntry(int blkno, int index) {
    int entry_num = BLOCK_SIZE / sizeof(DirEntry);
    DirEntry *Entry = (DirEntry *)malloc(sizeof(DirEntry) * entry_num);
    DevReadBlock(blkno, (char *)Entry);
    Entry[index].inodeNum = INVALID_ENTRY;
    DevWriteBlock(blkno, (char *)Entry);
    free(Entry);
}
