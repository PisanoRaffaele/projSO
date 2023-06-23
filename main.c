#include "FileSystemFAT.h"
#include "prints.h"
#include <stdbool.h>

void reverse(char str[], int length)
{
    int start = 0;
    int end = length - 1;
    while (start < end) {
        char temp = str[start];
        str[start] = str[end];
        str[end] = temp;
        end--;
        start++;
    }
}
// Implementation of citoa()
char* citoa(int num, char* str, int base)
{
    int i = 0;
    bool isNegative = false;

    /* Handle 0 explicitly, otherwise empty string is
     * printed for 0 */
    if (num == 0) {
        str[i++] = '0';
        str[i] = '\0';
        return str;
    }

    // In standard itoa(), negative numbers are handled
    // only with base 10. Otherwise numbers are
    // considered unsigned.
    if (num < 0 && base == 10) {
        isNegative = true;
        num = -num;
    }

    // Process individual digits
    while (num != 0) {
        int rem = num % base;
        str[i++] = (rem > 9) ? (rem - 10) + 'a' : rem + '0';
        num = num / base;
    }

    // If number is negative, append '-'
    if (isNegative)
        str[i++] = '-';

    str[i] = '\0'; // Append string terminator

    // Reverse the string
    reverse(str, i);

    return str;
}

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

	char *name = calloc(10, 1);

	name[0] = 'c';
	for (int i = 0; i < 520; i++)
	{
		citoa(i, &name[1], 10);
		close(createFile(fs, "$ROOT$/user/prova/", name, R));

	}

	//fh = createFile(fs, "$ROOT$/user/prova/", "prova.text", R);

    //printOpenFileInfo();
	//printFS(fs, "bitMap");

	printFS(fs, "directoryTree");

    //printFS(fs, "tableFAT");

    //printFS(fs, "fcbList");

}

