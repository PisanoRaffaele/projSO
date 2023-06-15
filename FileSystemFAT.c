#include "FileSystem.h"
#include "FileSystemFAT.h"

#define MAX_OPEN_FILES 10
#define DIM_DISK_BLOCKS 1024

OpenFileInfo *openFiles[MAX_OPEN_FILES];
int numOpenFiles = 0;

FileSystem_FAT FAT;

Directory_FAT rootDir;

