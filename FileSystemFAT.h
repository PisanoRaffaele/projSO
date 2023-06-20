#ifndef FILESYSTEMFAT_H
#define FILESYSTEMFAT_H

#include "FS_Structs.h"

FileHandle *createFile(FileSystemFAT *fs, char *path, char *name, mode_type mode);
void eraseFile(char *fileName);
int close(int fd);

off_t seek(int fd, off_t offset, int whence);
int read(int fd, void *buf, size_t count);
int write(int fd, const void *buf, size_t count);

void createDirectory(FileSystemFAT *fs, char *path, char *name);
void eraseDirectory(char *directoryName);
void listDirectory(char *directoryName);
void changeDirectory(char *directoryName);

#endif
