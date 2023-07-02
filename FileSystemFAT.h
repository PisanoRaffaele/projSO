#ifndef FILESYSTEMFAT_H
#define FILESYSTEMFAT_H

#include "FS_Structs.h"
#include "utils.h"

FileHandle *createFile(FileSystemFAT *fs, char *path, char *name, mode_type mode);
void eraseFile(FileSystemFAT *fs, char *path, char *name);
int close(FileHandle *fh);

off_t fs_seek(int fd, off_t offset, int whence);
int fs_read(FileHandle *fd, void *buf, size_t count);
int fs_write(int fd, const void *buf, size_t count);

void createDirectory(FileSystemFAT *fs, char *path);
void eraseDirectory(FileSystemFAT *fs, char *path);
void listDirectory(FileSystemFAT *fs, char *path);
void changeDirectory(FileSystemFAT *fs, char *name, char *fileDir, char *newPath);

void printOpenFileInfo();

#endif
