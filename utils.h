#ifndef UTILS_H
#define UTILS_H

#include "FS_Structs.h"

char *getDataBlock(FileSystemFAT *fs);

int getBlockIdx(FileSystemFAT *fs, char *block_ptr);

char *getBlockPointer(FileSystemFAT *fs, int idx);

void releaseDataBlock(FileSystemFAT *fs, int index);

void setBit(char *bitMap, int index);

void printFCB(FCB fcb);

void printFS(FileSystemFAT fs, const char* option);

#endif
