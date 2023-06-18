#pragma once

#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <stdint.h>
#include <errno.h>

#define DATA_BLOCKS 4096
#define DATA_BLOCK_SIZE 1024 //bytes

#define META_DATA_SIZE 33792 // 4096 * 4 + 2048 * 8 + 1024 bytes
#define META_DATA_BLOCKS 33 // (4096 * 4 + 2048 * 8 + 1024) / 1024

#define DISK_DATA_SIZE 4194304 // 1024 * 4096 bytes
#define DISK_SIZE 4228096 // 4194304 + 33792 bytes
#define DISK_BLOCKS 4129 // 4096 + (4096 * 4 + 2048 * 8 + 1024) / 1024

#define MAX_OPEN_FILES 10
#define MAX_FCBS 2048
#define MAX_FILE_NAME_LENGTH 8

// 1024 bytes
typedef struct FCB{
	char fileName[MAX_FILE_NAME_LENGTH]; // 8 bytes
	int32_t permissions; // 4 bytes
	int32_t isDirectory; // 4 bytes (0 = false, 1 = true)
	int32_t fileBlockCount; // 4 bytes
	int32_t nextBlockIndex; // 4 bytes
	char data[DATA_BLOCK_SIZE - 24]; // 1000 bytes
} FCB;

typedef struct FileSystemFAT {
	int32_t diskSize; // 4 bytes
	int32_t blockSize; // 4 bytes
	int32_t blockNum; // 4 bytes
	uint64_t rootFCB; // 8 bytes
	char bitMap[DATA_BLOCKS / 8]; // 512 bytes -> 4096 bits
	char info_padding[DATA_BLOCK_SIZE - 532]; // (padding) 492 bytes

	int32_t tableFAT[DATA_BLOCKS]; // 4096 * 4 bytes = 16384 bytes
	uint64_t fcbList[MAX_FCBS]; // 2048 * 8 bytes = 16384 bytes
	char diskBuffer[DISK_DATA_SIZE]; // 1024 * 4096 bytes = 4194304 bytes
} FileSystemFAT;

// 1024 bytes
typedef struct DirectoryEntry {
	int32_t numFCBS; // 4 bytes
	int32_t isLast; // 4 bytes (0 = false, 1 = true)
	uint64_t FCBS[( DATA_BLOCK_SIZE - 8 ) / 8]; // 127 pointers * 8 bytes each = 1016 bytes
} DirectoryEntry;

// 1024 bytes
typedef struct FileEntry {
	char data[DATA_BLOCK_SIZE]; // 1024 bytes
} FileEntry;

// 1000 bytes
typedef struct DirectoryEntryMin {
	int32_t numFCBS; // 4 bytes
	int32_t isLast; // 4 bytes
	uint64_t FCBS[( DATA_BLOCK_SIZE - 24 - 8 ) / 8]; // 124 pointers * 8 bytes each = 992 bytes
} DirectoryEntryMin;

// 1000 bytes
typedef struct FileEntryMin {
	char data[DATA_BLOCK_SIZE - 24]; // 1000 bytes
} FileEntryMin;




typedef struct OpenFileInfo {
	FCB *fcb;
	int filePointer;
} OpenFileInfo;

typedef struct OpenFileRef {
	OpenFileInfo *openFileInfo;
} OpenFileRef;


FileSystemFAT *FS_init();

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

char *getBlock(char *disk, int pos);


