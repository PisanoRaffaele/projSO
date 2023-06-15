#include "FileSystemFAT.h"


OpenFileInfo *openFiles[MAX_OPEN_FILES];
int numOpenFiles = 0;

FileSystemFAT FAT;

FCB_FAT *rootFCB;

