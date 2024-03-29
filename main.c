#include "FileSystemFAT.h"
#include "prints.h"

FileSystemFAT *FS_init()
{
	int				i;
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

	FCB *root = createFCB(FAT, NULL, ROOT_DIR_NAME, 1);

	FAT->rootFCB = root;

	DirectoryEntryMin *rootDir = (DirectoryEntryMin *)root->data;
	rootDir->numFCBS = 0;
	rootDir->FCBS[0] = NULL;

    initOpenFileInfo();

	return FAT;
}


void printAll(FileSystemFAT *fs)
{
    printFS(fs, "all");
}

void testCreateEraseFile(FileSystemFAT *fs)
{
    FileHandle *fh;

    fh = createFile(fs, "$ROOT$/user/prova/", "prova.txt", R);
    printOpenFileInfo();
    eraseFile(fs, "$ROOT$/user/prova", "prova.txt");
    close(fh);
    eraseFile(fs, "$ROOT$/user/prova", "prova.txt");
    printFS(fs, "directoryTree");

    for (int i = 0; i < 510; i++)
    {
        int ret;
        char str[10];
        snprintf(str, sizeof(str), "%d.txt", i);
        fh = createFile(fs, "$ROOT$/filebox/", str, R);
        if (fh == NULL)
            printf(RED "Error creating file\n" RESET);

        ret = close(fh);
        if (ret)
            printf(RED "Error closing file\n" RESET);
    }
    printOpenFileInfo();
    printFS(fs, "directoryTree");
    printFS(fs, "bitMap");

    for (int i = 0; i < 510; i++)
    {
        char str[10];
        snprintf(str, sizeof(str), "%d.txt", i);
        eraseFile(fs, "$ROOT$/filebox/", str);
    }

    printOpenFileInfo();
    printFS(fs, "fcbList");
}

void testWriteRead(FileSystemFAT *fs)
{
    FileHandle *fh;
    char str[9001];
    memset(str, 'p', sizeof(str));
    str[sizeof(str) - 1] = '\0';

    fh = createFile(fs, "$ROOT$/user/prova/", "prova.txt", W_R);
    printf("%s\n", fh->info->fcb->fileName);
    printf("offset: %d\n", fh->offset);
    printf("mode: %d\n", fh->permissions);

    fs_write(fh, str, 10);

    printFileContent(fs, fh->info->fcb);

    printf("offset: %d\n", fh->offset);

    fs_seek(fh, 0, SEEK_SET);

    char buf[9001];
    memset(buf, 0, sizeof(buf));
    fs_read(fh, buf, sizeof(buf));

    printf("read buffer1: %s\n", buf);

    fs_seek(fh, 1, SEEK_CUR);
    fs_seek(fh, 0, SEEK_CUR);
    memset(buf, 0, sizeof(buf));
    fs_read(fh, buf, 1);
    printf("read buffer2: %s\n", buf);

    fs_seek(fh, 1, SEEK_END);
    fs_seek(fh, 0, SEEK_END);
    memset(buf, 0, sizeof(buf));
    fs_read(fh, buf, 1);
    printf("read buffer3: %s\n", buf);

    fs_seek(fh, 8999, SEEK_SET);
    printf("offset: %d\n", fh->offset);
    memset(buf, 0, sizeof(buf));
    fs_read(fh, buf, 3);
    printf("read buffer4: %s\n", buf);

    printFS(fs, "fcbList");
    printFS(fs, "bitMap");

    close(fh);
}

void testDir(FileSystemFAT *fs)
{
    createDirectory(fs, "$ROOT$/user/");
    createDirectory(fs, "$ROOT$/user/prova/prova2/");
    createDirectory(fs, "$ROOT$/fr/");

    listDirectory(fs, "$ROOT$/");

    printFS(fs, "directoryTree");
    changeDirectory(fs, "prova2", "$ROOT$/user/prova/", "$ROOT$/user/");
    printFS(fs, "directoryTree");

    eraseDirectory(fs, "$ROOT$/user/prova/prova2/");
    eraseDirectory(fs, "$ROOT$/user/prova2/");
    eraseDirectory(fs, "$ROOT$/user/");

    for (int i = 0; i < 510; i++)
    {
        char str[35];
        snprintf(str, sizeof(str), "$ROOT$/filebox/1/2/3/4/%d", i);
        createDirectory(fs, str);
    }

    listDirectory(fs, "$ROOT$/filebox/1/2/3/4/");

    for (int i = 0; i < 510; i++)
    {
        char str[35];
        snprintf(str, sizeof(str), "$ROOT$/filebox/1/2/3/4/%d", i);
        eraseDirectory(fs, str);
    }
    printFS(fs, "directoryTree");
}

void testAll (FileSystemFAT *fs) {
    FileHandle *fh;
    char str[9001];
    char buf[9001];

    fh = createFile(fs, "$ROOT$/user/prova/", "prova.txt", W_R);
    printf("%s | offset: %d | mode: %d\n", fh->info->fcb->fileName, fh->offset, fh->permissions);
    memset(str, 'p', sizeof(str));
    str[sizeof(str) - 1] = '\0';
    fs_write(fh, str, 10);
    printFileContent(fs, fh->info->fcb);
    printf("offset: %d\n", fh->offset);
    fs_seek(fh, 0, SEEK_SET);
    memset(buf, 0, sizeof(buf));
    fs_read(fh, buf, sizeof(buf));
    printf("read buffer1: %s\n", buf);

    printOpenFileInfo();
    eraseFile(fs, "$ROOT$/user/prova", "prova.txt");
    close(fh);
    eraseFile(fs, "$ROOT$/user/prova", "prova.txt");

    createDirectory(fs, "$ROOT$/fr/");
    listDirectory(fs, "$ROOT$/");
    eraseDirectory(fs, "$ROOT$/user/");
    changeDirectory(fs, "prova", "$ROOT$/user/", "$ROOT$/fr/");
    eraseDirectory(fs, "$ROOT$/user/");

    printFS(fs, "bitMap");
    printFS(fs, "fcbList");
    printFS(fs, "directoryTree");

}



int main(int argc, char **argv) {

    if (argc < 2) {
        printf(RED"\nMissing option:\n\n"RESET);
        printf("    1: print all the fs\n");
        printf("    2: test Create/Erase File\n");
        printf("    3: test write/read\n");
        printf("    4: test dir\n");
        printf("\n");
        return 1;
    }
    else if (argc > 2) {
        printf(RED"Too many arguments\n"RESET);
        return 1;
    }

    FileSystemFAT *fs = FS_init();

    switch (atoi(argv[1])) {
        case 1:
            printAll(fs);
            break;
        case 2:
            testCreateEraseFile(fs);
            break;
        case 3:
            testWriteRead(fs);
            break;
        case 4:
            testDir(fs);
            break;
        case 5:
            testAll(fs);
            break;
        default:
            printf(RED"Invalid option\n\n"RESET);
            printf("    1) printAll\n");
            printf("    2: test CreateFile\n");
            printf("    3: test write\n");
            printf("    4: test dir\n");
            printf("    5: test all\n");
            printf("\n");
            break;
    }
}
