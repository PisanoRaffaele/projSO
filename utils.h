#ifndef UTILS_H
#define UTILS_H

#include "FS_Structs.h"

char *getDataBlock(FileSystemFAT *fs);


int getBlockIdx(FileSystemFAT *fs, char *block_ptr);

char *getBlockPointer(FileSystemFAT *fs, int idx);


void removeBit(char *bitMap, int index);

void setBit(char *bitMap, int index);

void removeFatIndex(FileSystemFAT *fs, FCB *dirFCB);


FCB *createFCB(FileSystemFAT *fs, FCB *fcbDir, char *fileName, mode_type mode, int32_t isDirectory);

int deleteFCB(FileSystemFAT *fs, FCB *fcb);

FCB *findFCB(void *c_dir, char *name, int isMin);

void addToDir(FileSystemFAT *fs, FCB *fcb, FCB *dirFCB);

void removeFromDir(FileSystemFAT *fs, FCB *dirFCB, FCB *fcb);



DirectoryEntry *getDirBlock(FileSystemFAT *fs, FCB *dirFcb, int deep);

DirectoryEntry *createDirBlock(FileSystemFAT *fs, FCB *dirFcb, int deep);

FileEntry *getNextDataBlock(FileSystemFAT *fs, FCB *fileFcb);



FileHandle *newOpenFileInfo(FileHandle **openFileInfo, int *openedFiles);

FileHandle *findOpenFileInfo(FileHandle **openFileInfo, FCB *toFind);

int remOpenFileInfo(FileHandle **openFileInfo, int *openedFiles, FileHandle *elem);

void updateFilePointer(FileHandle *fileInfo);



int pathIsValid(char *path);

int nameIsValid(char *name);

#endif
