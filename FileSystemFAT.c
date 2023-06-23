#include "FileSystemFAT.h"
#include "prints.h"

FileHandle *openFileInfo[MAX_OPEN_FILES];
int openedFiles = 0;

void printOpenFileInfo()
{
	printf(CYAN "Open Files: \n" RESET);
	printf("numOpenFiles: %d\n", openedFiles);

	for (int i = 0; i < MAX_OPEN_FILES; i++)
	{
		if (openFileInfo[i] != NULL)
			printf("fileInfo[%d]: %s\n", i, openFileInfo[i]->fcb->fileName);
	}
}


FileHandle *createFile(FileSystemFAT *fs, char *path, char *name, mode_type mode)
{
	FCB					*currentFCB;
	FCB					*retFCB;
	FileHandle 			*ret;

	if (!nameIsValid(name))
		return NULL;
	currentFCB = createDirectory(fs, path);
	if (currentFCB == fs->rootFCB)
		retFCB = findFCB((void *)currentFCB->data, name, 1);
	else
		retFCB = findFCB((void *)currentFCB->data, name, 0);

	if(retFCB == NULL)
	{
		ret = newOpenFileInfo(openFileInfo, &openedFiles);
		if(!ret)
			return NULL;
		retFCB = createFCB(fs, currentFCB, name, mode, 0);
		ret->fileSystem = fs;
		ret->fcb = retFCB;
		ret->filePointer = 0;
		return ret;
	}
	else
	{
		ret = findOpenFileInfo(openFileInfo, retFCB);
		updateFilePointer(ret);
		return ret;
	}
}

void eraseFile(FileSystemFAT *fs, char *path, char *name)
{
	char				*path_copy;
	char				*pathSegment;
	DirectoryEntry		*currentDir;
	DirectoryEntryMin	*firstDir;
	FCB					*currentFCB;
	FCB					*scanningFCB;

	if (!nameIsValid(name))
		return NULL;

	if (!pathIsValid(path))
		return;

	path_copy = strdup(path);
	pathSegment = strtok(path_copy, "/");

	if (strcmp(pathSegment, ROOT_DIR_NAME) != 0)
	{
		printf("%s",pathSegment);
		printf(RED"\npath doesn't start with "RESET);
		printf(ROOT_DIR_NAME);
		printf("\n");
		return NULL;
	}
	currentFCB = fs->rootFCB;

	pathSegment = strtok(NULL, "/");
	if (pathSegment != NULL)
	{
		firstDir = (DirectoryEntryMin *)currentFCB->data;
		if (!firstDir->numFCBS == 0)
			scanningFCB = findFCB((void *)firstDir, pathSegment, 1);
		if (scanningFCB == NULL)
		{
			printf(CYAN"\nDirectory not found\n"RESET);
			return;
		}
		pathSegment = strtok(NULL, "/");
	}
	while(pathSegment != NULL)
	{
		scanningFCB = NULL;
		currentDir = (DirectoryEntry *) currentFCB->data;
		if (!currentDir->numFCBS == 0)
			scanningFCB = findFCB((void *)currentDir, pathSegment, 0);
		if (scanningFCB == NULL)
		{
			printf(PURPLE"\nDirectory not found\n"RESET);
			return;
		}
		pathSegment = strtok(NULL, "/");
	}

	//elimina File


}

FCB *createDirectory(FileSystemFAT *fs, char *path)
{
	char				*path_copy;
	char				*pathSegment;
	DirectoryEntry		*currentDir;
	DirectoryEntryMin	*firstDir;
	FCB					*currentFCB;
	FCB					*scanningFCB;

	if (!pathIsValid(path))
		return NULL;

	path_copy = strdup(path);
	pathSegment = strtok(path_copy, "/");

	if (strcmp(pathSegment, ROOT_DIR_NAME) != 0)
	{
		printf("%s",pathSegment);
		printf(RED"\npath doesn't start with "RESET);
		printf(ROOT_DIR_NAME);
		printf("\n");
		return NULL;
	}
	currentFCB = fs->rootFCB;

	pathSegment = strtok(NULL, "/");
	if (pathSegment != NULL)
	{
		firstDir = (DirectoryEntryMin *)currentFCB->data;
		if (!firstDir->numFCBS == 0)
			scanningFCB = findFCB((void *)firstDir, pathSegment, 1);
		if (scanningFCB == NULL)
			scanningFCB = createFCB(fs, currentFCB, pathSegment, R, 1);
		currentFCB = scanningFCB;
		pathSegment = strtok(NULL, "/");
	}
	while(pathSegment != NULL)
	{
		scanningFCB = NULL;
		currentDir = (DirectoryEntry *) currentFCB->data;
		if (!currentDir->numFCBS == 0)
			scanningFCB = findFCB((void *)currentDir, pathSegment, 0);
		if (scanningFCB == NULL)
			scanningFCB = createFCB(fs, currentFCB, pathSegment, R, 1);
		currentFCB = scanningFCB;
		pathSegment = strtok(NULL, "/");
	}
	return currentFCB;
}

int close(FileHandle *fh)
{
	return remOpenFileInfo(openFileInfo, &openedFiles, fh);
}

void eraseDirectory(FileSystemFAT *fs, char *path)
{
	char				*path_copy;
	char				*pathSegment;
	DirectoryEntry		*currentDir;
	DirectoryEntryMin	*firstDir;
	FCB					*currentFCB;
	FCB					*scanningFCB;


	if (!pathIsValid(path))
		return;

	path_copy = strdup(path);
	pathSegment = strtok(path_copy, "/");

	if (strcmp(pathSegment, ROOT_DIR_NAME) != 0)
	{
		printf("%s",pathSegment);
		printf(RED"\npath doesn't start with "RESET);
		printf(ROOT_DIR_NAME);
		printf("\n");
		return NULL;
	}
	currentFCB = fs->rootFCB;

	pathSegment = strtok(NULL, "/");
	if (pathSegment != NULL)
	{
		firstDir = (DirectoryEntryMin *)currentFCB->data;
		if (!firstDir->numFCBS == 0)
			scanningFCB = findFCB((void *)firstDir, pathSegment, 1);
		if (scanningFCB == NULL)
		{
			printf(CYAN"\nDirectory not found\n"RESET);
			return;
		}
		pathSegment = strtok(NULL, "/");
	}
	while(pathSegment != NULL)
	{
		//ogni volta torna MIN?
		currentDir = (DirectoryEntry *) scanningFCB->data;
		if (!currentDir->numFCBS == 0)
			scanningFCB = findFCB((void *)currentDir, pathSegment, 0);
		if (scanningFCB == NULL)
		{
			printf(PURPLE"\nDirectory not found\n"RESET);
			return;
		}
		pathSegment = strtok(NULL, "/");
	}

	firstDir = (DirectoryEntryMin *)scanningFCB->data;
	if (firstDir->numFCBS > 0)
	{
		printf(PURPLE"Directory not empty"RESET);
		return;
	}
	removeFromDir(fs, currentDir, scanningFCB);
	deleteFCB(fs, scanningFCB);
}
