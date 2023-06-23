#include "prints.h"
#include "utils.h"

void printFCB(FCB *fcb) {
    printf("   FCB: ");
    printf("	fileName: %s | ", fcb->fileName);
    printf("permissions: %d | ", fcb->permissions);
    printf("isDirectory: %d | ", fcb->isDirectory);
    printf("BlockCount: %d | ", fcb->BlockCount);
    printf("FATNextIndex: %d | ", fcb->FATNextIndex);
    printf("data: ");
	for (int i = 0; i < sizeof(fcb->data); i++) {
		if (fcb->data[i] != 0)
			printf("%02X ", (unsigned char)fcb->data[i]);
	}
}

void printDirectoryTree(FileSystemFAT *fs, FCB *dirFCB)
{
	DirectoryEntry		*de;
	DirectoryEntryMin	*deMin;
	int					i;
	int					count;

	if (dirFCB == NULL || dirFCB->isDirectory == 0)
	{
		printf(RED "printDirectoryTree: dirFCB is NULL or not a directory\n"RESET);
		return;
	}

	if(strcmp(dirFCB->fileName, ROOT_DIR_NAME) == 0)
		printf(BOLDGREEN "%s\n[" RESET, dirFCB->fileName);

	deMin = (DirectoryEntryMin *)dirFCB->data;
	i = 0;
	count = 0;
	while (count < deMin->numFCBS)
	{
		if (deMin->FCBS[i] == NULL)
		{
			i++;
			continue;
		}
		else if (deMin->FCBS[i]->isDirectory == 1)
		{
			printf(BOLDMAGENTA"	%s\n	["RESET, deMin->FCBS[i]->fileName);
			if (count == deMin->numFCBS - 1)
				printDirectoryTree(fs, deMin->FCBS[i]);
			printf(BOLDMAGENTA"\n	]\n"RESET);
		}
		else
			printf("%s - ", deMin->FCBS[i]->fileName);
		i++;
		count++;
	}
	int h = 0;
	de = getDirBlock(fs, dirFCB, ++h);
	while (de != NULL)
	{
		i = 0;
		count = 0;
		while (count < de->numFCBS)
		{
			if (de->FCBS[i] == NULL)
			{
				i++;
				continue;
			}
			else if (de->FCBS[i]->isDirectory == 1)
			{
				printf(CYAN "%s\n"RESET, de->FCBS[i]->fileName);
				if (count == de->numFCBS - 1)
					printDirectoryTree(fs, de->FCBS[i]);
			}
			else
				printf("%s ", de->FCBS[i]->fileName);
			i++;
			count++;
		}
		de = getDirBlock(fs, dirFCB, ++h);
	}
}

void printFS(FileSystemFAT *fs, const char* option) {
    printf(CYAN "FileSystemFAT:\n" RESET);
    printf(CYAN "diskSize:" RESET " %d bytes, %d MB\n", fs->diskSize, fs->diskSize / 1000000);
    printf(CYAN "blockSize:" RESET " %d bytes\n", fs->blockSize);
    printf(CYAN "blockNum:" RESET " %d\n", fs->blockNum);
	printf(CYAN "numFCBS:" RESET" %d\n", fs->numFCBS);

    if (strcmp(option, "bitMap") == 0) {

        printf(CYAN "bitMap: \n" RESET);
        for (int i = 0; i < sizeof(fs->bitMap); i++) {
            unsigned char byte = fs->bitMap[i];
            for (int j = 0; j < 8; j++) {
                int bit = (byte >> j) & 1;
				if (bit == 1)
					printf(GREEN "%d" RESET, bit);
				else
                	printf("%d", bit);
            }
        }
        printf("\n");
    } else if (strcmp(option, "tableFAT") == 0) {

		printf(CYAN "tableFAT: \n" RESET);
		for (int i = 0; i < sizeof(fs->tableFAT) / sizeof(fs->tableFAT[0]); i++) {
			printf("%d[%d], ", i, fs->tableFAT[i]);
		}
		printf("\n");
	} else if (strcmp(option, "fcbList") == 0) {
		int count = 0;
        printf(CYAN "fcbList: \n" RESET);
        for (int i = 0; count < fs->numFCBS; i++) {
			if(fs->fcbList[i] != 0)
			{
				count++;
				printf("[");
				printFCB((FCB *)fs->fcbList[i]);
				printf("]\n");
			}
			else
			{
				if(!i)
					printf(" [empty]\n");
			}
        }
        printf("\n");
    } else if (strcmp(option, "diskBuffer") == 0) {

		printf(CYAN "diskBuffer (1/512): \n" RESET);

		int smallSize = DISK_DATA_SIZE / 512;

		for (int i = 0; i < smallSize; i++) {
			printf("%02X ", fs->diskBuffer[i] & 0xFF); // Stampa il byte in esadecimale

			// Verifica se il conteggio raggiunge un multiplo della dimensione del blocco
			if ((i + 1) % (BLOCK_SIZE) == 0) {
				printf(BLUE "\n|block|\n" RESET); // Stampa una barra verticale colorata
			}
		}

		printf("\n");
	} else if (strcmp(option, "directoryTree") == 0) {
		printf(CYAN "directoryTree: \n" RESET);
		printDirectoryTree(fs, fs->rootFCB);
		printf(BOLDGREEN "]" RESET);
		printf("\n");
	} else {
        printf("option = {'bitMap', 'tableFAT', 'fcbList', 'diskBuffer'}.\n");
    }
    printf("\n");
}


