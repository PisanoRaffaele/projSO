#pragma once

typedef struct FileSystem {
	char *diskBuffer;
	int diskSize;
	int blockSize;
	int blockCount;
} FileSystem;

typedef struct FCB {
	char *fileName;
	char permissions[3];
} FCB;

typedef struct OpenFileInfo {
	FCB *fcb;
	int filePointer;
} OpenFileInfo;

typedef struct OpenFileRef {
	OpenFileInfo *openFileInfo;
} OpenFileRef;
