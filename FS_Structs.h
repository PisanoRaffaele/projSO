#ifndef FS_STRUCTS_H
#define FS_STRUCTS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <stdint.h>
#include <errno.h>

#define DATA_BLOCKS 1024
#define BLOCK_SIZE 4096 // bytes

#define META_DATA_SIZE 4096 * 2  // 4 + 4 + 4 + 4 + 8 + 128 + 8000 + 40 = 8192 bytes
#define META_DATA_BLOCKS 2

#define DISK_DATA_SIZE 1024 * 4096
#define DISK_SIZE 4096 * 2 + 1024 * 4096 // META_DATA_SIZE + DISK_DATA_SIZE
#define DISK_BLOCKS (4096 * 2 + 1024 * 4096) / 4096 // DISK_SIZE / BLOCK_SIZE

#define MAX_OPEN_FILES 10
#define MAX_FCBS 1000
#define MAX_FILE_NAME_LENGTH 8

#define ROOT_DIR_NAME "$ROOT$"

#define COLOR_RED     "\x1b[31m"
#define COLOR_BLUE     "\x1b[34m"
#define COLOR_GREEN    "\x1b[32m"
#define COLOR_CYAN     "\x1b[36m"
#define COLOR_RESET   "\x1b[0m"

#define EX_W_R 7
#define EX_W 6
#define EX_R 5
#define EX 4
#define W_R 3
#define W 2
#define R 1

typedef unsigned int mode_type;
//typedef unsigned int size_t;
//typedef int ssize_t;
//typedef int off_t;

// 4096 bytes
typedef struct FCB{
	char fileName[MAX_FILE_NAME_LENGTH]; // 8 bytes
	int32_t permissions; // 4 bytes
	int32_t isDirectory; // 4 bytes (0 = false, 1 = true)
	int32_t fileBlockCount; // 4 bytes
	int32_t nextBlockIndex; // 4 bytes
	char data[BLOCK_SIZE - 24]; // 4072 bytes
} FCB;

typedef struct FileSystemFAT {
	int32_t tableFAT[DATA_BLOCKS]; // 1024 * 4 bytes = 4096 bytes

	int32_t diskSize; // 4 bytes
	int32_t blockSize; // 4 bytes
	int32_t blockNum; // 4 bytes
	int32_t numFCBS; // 4 bytes
	FCB *rootFCB; // 8 bytes
	char bitMap[DATA_BLOCKS / 8]; // 128 bytes -> 1024 bits

	FCB *fcbList[MAX_FCBS]; // 1000 * 8 bytes = 8000 bytes

	char info_padding[40]; // 4096 * 2 - (4 + 4 + 4 + 4 + 8 + 128 + 8000) = 40 bytes (padding)

	char diskBuffer[DISK_DATA_SIZE]; // 1024 * 4096 bytes = 4194304 bytes
} FileSystemFAT;

// 4096 bytes
typedef struct DirectoryEntry {
	int32_t numFCBS; // 4 bytes
	int32_t isLast; // 4 bytes (0 = false, 1 = true)
	FCB *FCBS[( BLOCK_SIZE - 8 ) / 8]; // 511 pointers * 8 bytes each = 4088 bytes
} DirectoryEntry;

// 4096 bytes
typedef struct FileEntry {
	char data[BLOCK_SIZE]; // 4096 bytes
} FileEntry;

// 4072 bytes
typedef struct DirectoryEntryMin {
	int32_t numFCBS; // 4 bytes
	int32_t isLast; // 4 bytes
	uint64_t FCBS[( BLOCK_SIZE - 24 - 8 ) / 8]; // 508 pointers * 8 bytes each = 4064 bytes
} DirectoryEntryMin;

// 4072 bytes
typedef struct FileEntryMin {
	char data[BLOCK_SIZE - 24]; // 4072 bytes
} FileEntryMin;



typedef struct FileHandle {
	int32_t filePointer; // 4 bytes
	FCB *fcb; // 8 bytes
} FileHandle;



typedef struct OpenFileInfo {
	FCB *fcb;
	int filePointer;
} OpenFileInfo;

typedef struct OpenFileRef {
	OpenFileInfo *openFileInfo;
} OpenFileRef;


#endif
