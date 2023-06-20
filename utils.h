#ifndef UTILS_H
#define UTILS_H

#include "FS_Structs.h"

char *getDataBlock(FileSystemFAT *fs);

int getBlockIdx(FileSystemFAT *fs, char *block_ptr);

char *getBlockPointer(FileSystemFAT *fs, int idx);

void removeBit(char *bitMap, int index);

void setBit(char *bitMap, int index);

FCB *createFCB(FileSystemFAT *fs, char *fileName, mode_type mode, int32_t isDirectory);

void deleteFCB(FileSystemFAT *fs, FCB *fcb);

void addFcbToDirectory(FileSystemFAT *fs, FCB *fcb, FCB *directory);

FCB *createNewPath(char *path);

#endif
