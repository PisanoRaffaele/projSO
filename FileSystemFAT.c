#include "FileSystemFAT.h"


OpenFileInfo *openFiles[MAX_OPEN_FILES];
int numOpenFiles = 0;

FileSystemFAT * FS_init()
{
	int i;

	char *disk = (char *)mmap(NULL, DISK_SIZE, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	if (disk == MAP_FAILED)
	{
		printf("Error: mmap failed\n");
		return;
	}

	char *block = getBlock(disk, 0);
	if (!block)
	{
		printf("can't get block\n");
		return;
	}

	FileSystemFAT FAT = *(FileSystemFAT *) block;

	FAT.diskSize = DISK_SIZE;
	FAT.blockSize = DATA_BLOCK_SIZE;
	FAT.blockCount = DATA_BLOCKS;

	for (i = 0; i < DATA_BLOCKS; i++)
		FAT.tableFAT[i] = -1;

	for (i = 0; i < MAX_FCBS; i++)
		FAT.fcbList[i] = 0;

	for (i = 0; i < MAX_OPEN_FILES; i++)
		openFiles[i] = 0;

	FCB *root = (FCB *)FAT->diskBuffer[0];

	root->fileName = "$$ROOT$$";
	root->permissions = S_IRWXU;
	root->isDirectory = 1;
	root->fileBlockCount = 1;
	root->nextBlockIndex = -1;

	DirectoryEntryMin *rootDir = (DirectoryEntryMin *)root->data;
	rootDir->numFCBS = 0;
	rootDir->isLast = 1;

	setBit(G_FAT->bitMap, 0);

	FAT.rootFCB = root;

	return FAT;
}

char *getBlock(char *disk, int blockIndex)
{
	if (blockIndex < 0 || blockIndex * DATA_BLOCK_SIZE >= DISK_SIZE)
		return NULL;
	return disk + blockIndex * DATA_BLOCK_SIZE;
}

void setBit(char *bitMap, int index)
{
	int byteIndex = index / 8;
	int bitIndex = index % 8;

	bitMap[byteIndex] |= 1 << bitIndex;
}

void createFile(char *fileName)
{

}
