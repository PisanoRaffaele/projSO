#include "FileSystemFAT.h"
#include "utils.h"
#include "prints.h"

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

	FCB *root = createFCB(FAT, NULL, ROOT_DIR_NAME, R, 1);

	FAT->rootFCB = root;

	DirectoryEntryMin *rootDir = (DirectoryEntryMin *)root->data;
	rootDir->numFCBS = 0;
	rootDir->FCBS[0] = NULL;

	return FAT;
}

int main() {
	FileSystemFAT *fs = FS_init();

	//FileHandle *fh = createFile(fs, "$ROOT$/user/prova/", "prova.text", R);

	printFS(fs, "bitMap");

	printFS(fs, "fcbList");


	//fh = createFile(fs, "$ROOT$/user/prova/", "prova.text", R);

	printFS(fs, "bitMap");

	printFS(fs, "fcbList");

}

