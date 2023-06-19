#include "utils.h"

char *getDataBlock(FileSystemFAT *fs)
{
	for (int i = 0; i < sizeof(fs->bitMap); i++) {
		unsigned char byte = fs->bitMap[i];
		for (int j = 0; j < 8; j++) {
			int bit = (byte >> j) & 1;
			if (bit == 0)
			{
				setBit(fs->bitMap, i * 8 + j);
				return fs->diskBuffer + (i * 8 + j) * fs->blockSize;
			}
		}
	}
	perror("No free block");
	return NULL;
}

void releaseDataBlock(FileSystemFAT *fs, int index)
{
	int byteIndex = index / 8;
	int bitIndex = index % 8;

	fs->bitMap[byteIndex] &= ~(1 << bitIndex);
}

int getBlockIdx(FileSystemFAT *fs, char *block_ptr)
{
	return (block_ptr - fs->diskBuffer) / fs->blockSize;
}

char *getBlockPointer(FileSystemFAT *fs, int idx)
{
	return fs->diskBuffer + idx * fs->blockSize;
}

void setBit(char *bitMap, int index)
{
	int byteIndex = index / 8;
	int bitIndex = index % 8;

	bitMap[byteIndex] |= 1 << bitIndex;
}

void printFCB(FCB fcb) {
    printf("	FCB:\n");
    printf("	fileName: %s\n", fcb.fileName);
    printf("	permissions: %d\n", fcb.permissions);
    printf("	isDirectory: %d\n", fcb.isDirectory);
    printf("	fileBlockCount: %d\n", fcb.fileBlockCount);
    printf("	nextBlockIndex: %d\n", fcb.nextBlockIndex);
    printf("	data: \n");
	for (int i = 0; i < sizeof(fcb.data); i++) {
		printf("%c", fcb.data[i]);
	}
}

void printFS(FileSystemFAT fs, const char* option) {
    printf(COLOR_CYAN "FileSystemFAT:\n" COLOR_RESET);
    printf(COLOR_CYAN "diskSize:" COLOR_RESET " %d bytes, %d MB\n", fs.diskSize, fs.diskSize / 1000000);
    printf(COLOR_CYAN"blockSize:" COLOR_RESET " %d bytes\n", fs.blockSize);
    printf(COLOR_CYAN"blockNum:" COLOR_RESET " %d\n", fs.blockNum);
	printf(COLOR_CYAN"numFCBS:" COLOR_RESET" %d\n", fs.numFCBS);
    printf(COLOR_CYAN"rootFCB: [\n"COLOR_RESET);
	printFCB(*(FCB *)fs.rootFCB);
	printf(COLOR_CYAN"]\n"COLOR_RESET);

    if (strcmp(option, "bitMap") == 0) {

        printf(COLOR_CYAN "bitMap: \n" COLOR_RESET);
        for (int i = 0; i < sizeof(fs.bitMap); i++) {
            unsigned char byte = fs.bitMap[i];
            for (int j = 0; j < 8; j++) {
                int bit = (byte >> j) & 1;
				if (bit == 1)
					printf(COLOR_GREEN "%d" COLOR_RESET, bit);
				else
                	printf("%d", bit);
            }
        }
        printf("\n");
    } else if (strcmp(option, "tableFAT") == 0) {

		printf(COLOR_CYAN "tableFAT: \n" COLOR_RESET);
		for (int i = 0; i < sizeof(fs.tableFAT) / sizeof(fs.tableFAT[0]); i++) {
			printf("%d[%d], ", i, fs.tableFAT[i]);
		}
		printf("\n");
	} else if (strcmp(option, "fcbList") == 0) {

        printf(COLOR_CYAN "fcbList: \n" COLOR_RESET);
        for (int i = 0; i < sizeof(fs.fcbList); i++) {
			if(fs.fcbList[i] != 0)
			{
				printf("[\n");
				printFCB(*(FCB *)fs.fcbList[i]);
				printf("]\n");
			}
			else
			{
				if(!i)
					printf(" [empty]\n");
				return;
			}
        }
        printf("\n");
    } else if (strcmp(option, "diskBuffer") == 0) {

		printf(COLOR_CYAN "diskBuffer (1/512): \n" COLOR_RESET);

		int smallSize = DISK_DATA_SIZE / 512;

		for (int i = 0; i < smallSize; i++) {
			printf("%02X ", fs.diskBuffer[i] & 0xFF); // Stampa il byte in esadecimale

			// Verifica se il conteggio raggiunge un multiplo della dimensione del blocco
			if ((i + 1) % (BLOCK_SIZE) == 0) {
				printf(COLOR_BLUE "\n|block|\n" COLOR_RESET); // Stampa una barra verticale colorata
			}
		}

		printf("\n");
	} else {
        printf("option = {'bitMap', 'tableFAT', 'fcbList', 'diskBuffer'}.\n");
    }
    printf("\n");
}
