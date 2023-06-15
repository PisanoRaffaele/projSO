#pragma once

#include "mode.h"

#define MAX_OPEN_FILES 10

#define DATA_BLOCKS 4096
#define DATA_BLOCK_SIZE 1024
#define DISK_DATA_SIZE 1024 * 4096
#define MAX_FCBS 4096
#define DISK_SIZE 1024 * 4096 + 4096 + 4096 + 1024

#define MAX_FILE_NAME_LENGTH 8

typedef struct FileSystemFAT {
	int diskSize; // 4 bytes
	int blockSize; // 4 bytes
	int blockCount; // 4 bytes
	char info[DATA_BLOCK_SIZE - 4 - 4 - 4]; // (padding) 1012 bytes

	int tableFAT[DATA_BLOCKS]; // 4096 * 4 bytes = 16384 bytes
	FCB_FAT *fcbList[MAX_FCBS]; // 4096 * 4 bytes = 16384 bytes
	char diskBuffer[DISK_DATA_SIZE]; // 1024 * 4096 bytes = 4194304 bytes
} FileSystemFAT;


// 1024 bytes
typedef struct FCB_FAT {
	char fileName[MAX_FILE_NAME_LENGTH]; // 8 bytes
	int permissions; // 4 bytes
	int isDirectory; // 4 bytes (0 = false, 1 = true)
	int fileBlockCount; // 4 bytes
	int nextBlockIndex; // 4 bytes
	char data[DATA_BLOCK_SIZE - 8 - 4 - 4 - 4 - 4]; // 1000 bytes
} FCB_FAT;

// 1024 bytes
typedef struct DirectoryEntry_FAT {
	int numFCBS; // 4 bytes
	int isLast; // 4 bytes
	FCB_FAT *FCBS[( DATA_BLOCK_SIZE - 8 ) / 8]; // 127 pointers * 8 bytes each = 1016 bytes
} DirectoryEntry_FAT;

// 1024 bytes
typedef struct FileEntry_FAT {
	char data[DATA_BLOCK_SIZE]; // 1024 bytes
} FileEntry_FAT;



typedef struct OpenFileInfo {
	FCB_FAT *fcb;
	int filePointer;
} OpenFileInfo;

typedef struct OpenFileRef {
	OpenFileInfo *openFileInfo;
} OpenFileRef;



void createFile(char *fileName);
void eraseFile(char *fileName);
int open(const char *pathname, int flags, mode_t mode);
int close(int fd);

off_t seek(int fd, off_t offset, int whence);
ssize_t read(int fd, void *buf, size_t count);
ssize_t write(int fd, const void *buf, size_t count);

void createDirectory(char *directoryName);
void eraseDirectory(char *directoryName);
void listDirectory(char *directoryName);
void changeDirectory(char *directoryName);


