#pragma once

#include "FileSystem.h"

typedef struct FileSystem_FAT {
	FileSystem base;
	int *startIndexes;
	int *tableFAT;
} FileSystem_FAT;

typedef struct FCB_FAT {
	FCB base;
	int startIndex;
} FCB_FAT;


typedef struct Directory_FAT {
	FCB_FAT base;

} Directory_FAT;

typedef struct DirectoryEntry_FAT {
	
} DirectoryEntry_FAT;

