#ifndef FILESYSTEMFAT_H
#define FILESYSTEMFAT_H

#include "FS_Structs.h"

FileSystemFAT *FS_init();

void createFile(const char *pathname, mode_type mode);
void eraseFile(char *fileName);
int close(int fd);

off_t seek(int fd, off_t offset, int whence);
ssize_t read(int fd, void *buf, size_t count);
ssize_t write(int fd, const void *buf, size_t count);

void createDirectory(char *directoryName);
void eraseDirectory(char *directoryName);
void listDirectory(char *directoryName);
void changeDirectory(char *directoryName);

#endif
