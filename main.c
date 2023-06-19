#include "FileSystemFAT.h"
#include "utils.h"

OpenFileInfo	*openFiles[MAX_OPEN_FILES];
int				numOpenFiles = 0;

FileSystemFAT *FS_init()
{
	int				i;
	char			*block;
	FileSystemFAT	*FAT;
	char			*disk;

	disk = (char *)mmap(NULL, DISK_SIZE, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	if (disk == MAP_FAILED)
	{
		perror("map failed");
		return NULL;
	}
	FAT = (FileSystemFAT *) disk;

	FAT->diskSize = DISK_SIZE;
	FAT->blockSize = BLOCK_SIZE;
	FAT->blockNum = DATA_BLOCKS;
	FAT->numFCBS = 0;

	for (i = 0; i < DATA_BLOCKS; i++)
		FAT->tableFAT[i] = -1;

	for (i = 0; i < MAX_FCBS; i++)
		FAT->fcbList[i] = 0;

	for (i = 0; i < MAX_OPEN_FILES; i++)
		openFiles[i] = 0;

	FCB *root = (FCB *)&FAT->diskBuffer[0];

	strcpy(root->fileName, ROOT_DIR_NAME);

	root->permissions = R;
	root->isDirectory = 1;
	root->fileBlockCount = 1;
	root->nextBlockIndex = -1;

	DirectoryEntryMin *rootDir = (DirectoryEntryMin *)root->data;
	rootDir->numFCBS = 0;
	rootDir->isLast = 1;

	setBit(FAT->bitMap, 0);

	FAT->rootFCB = root;

	return FAT;
}

int main() {
	FileSystemFAT *f = FS_init();
	// uintptr_t f;
	printFS(*f, "bitMap");

	char *block_ptr = getDataBlock(f);
	int idx = getBlockIdx(f, block_ptr);

	block_ptr = getBlockPointer(f, idx);

	idx = getBlockIdx(f, block_ptr);

	printFS(*f, "bitMap");

	releaseDataBlock(f, idx);

	printFS(*f, "bitMap");
}

