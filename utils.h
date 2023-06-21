#ifndef UTILS_H
#define UTILS_H

#include "FS_Structs.h"

char *getDataBlock(FileSystemFAT *fs);


int getBlockIdx(FileSystemFAT *fs, char *block_ptr);

char *getBlockPointer(FileSystemFAT *fs, int idx);


void removeBit(char *bitMap, int index);

void setBit(char *bitMap, int index);


FCB *createFCB(FileSystemFAT *fs, FCB *fcbDir, char *fileName, mode_type mode, int32_t isDirectory);

void deleteFCB(FileSystemFAT *fs, FCB *fcb);

DirectoryEntry *getNextDirBlock(FileSystemFAT *fs, FCB *dirFcb);

FileEntry *getNextDataBlock(FileSystemFAT *fs, FCB *fileFcb);


void addFcbToDirFcb(FileSystemFAT *fs, FCB *fcb, FCB *dirFCB);

FCB *findFCB(void *c_dir, char *name, int isMin);


FileHandle *newOpenFileInfo();

FileHandle *findOpenFileInfo(FCB *toFind);

void remOpenFileInfo(FileHandle *elem);

void updateFilePointer(FileHandle *fileInfo);


FCB *createNewPath(char *path);

#endif
