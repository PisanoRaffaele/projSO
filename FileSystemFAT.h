#ifndef FILESYSTEMFAT_H
#define FILESYSTEMFAT_H

#include "FS_Structs.h"
#include "utils.h"

FileHandle *createFile(FileSystemFAT *fs, char *path, char *name, mode_type mode);
void eraseFile(char *fileName);
int close(FileHandle *fh);

off_t seek(int fd, off_t offset, int whence);
int read(int fd, void *buf, size_t count);
int write(int fd, const void *buf, size_t count);

FCB *createDirectory(FileSystemFAT *fs, char *path);
void eraseDirectory(FileSystemFAT *fs, char *path);
void listDirectory(FileSystemFAT *fs, char *path);
void changeDirectory(FileSystemFAT *fs, char *path);

void printOpenFileInfo();

#endif
