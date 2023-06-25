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
	FCB			*currentFCB;
	FCB			*retFCB;
	FileHandle	*ret;

	if (!nameIsValid(name))
		return NULL;

	currentFCB = createDirectoryWrapped(fs, path);

	retFCB = findFCB(fs, currentFCB, name);

	if(retFCB == NULL)
	{
		ret = newOpenFileInfo(openFileInfo, &openedFiles);
		if(!ret)
			return NULL;
		retFCB = createFCB(fs, currentFCB, name, 0);
		ret->permissions = mode;
		ret->fileSystem = fs;
		ret->fcb = retFCB;
		ret->filePointer = 0;
		return ret;
	}
	else
	{
		ret = findOpenFileInfo(openFileInfo, retFCB);
		updateFileInfo(ret, mode);
		return ret;
	}
}

void eraseFile(FileSystemFAT *fs, char *path, char *name)
{
	char	*path_copy;
	char	*pathSegment;
	FCB		*currentFCB;
	FCB		*oldFCB;

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

	while(pathSegment != NULL)
	{
		oldFCB = currentFCB;
		currentFCB = findFCB(fs, oldFCB, pathSegment);
		if (currentFCB == NULL)
		{
			printf(PURPLE"\nDirectory not found\n"RESET);
			return;
		}
		pathSegment = strtok(NULL, "/");
	}

	oldFCB = currentFCB;
	currentFCB = findFCB(fs, oldFCB, name);

	removeFromDir(fs, oldFCB, currentFCB);
	deleteFCB(fs, currentFCB);
}

void createDirectory(FileSystemFAT *fs, char *path)
{
	createDirectoryWrapped(fs, path);
}

void eraseDirectory(FileSystemFAT *fs, char *path)
{
	char				*path_copy;
	char				*pathSegment;
	DirectoryEntryMin	*dirEntry;
	FCB					*oldFCB;
	FCB					*currentFCB;

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
	if (pathSegment == NULL)
	{
		printf(RED"\nCan't delete root FCB\n"RESET);
		return;
	}
	while(pathSegment != NULL)
	{
		oldFCB = currentFCB;
		currentFCB = findFCB(fs, oldFCB, pathSegment);
		if (currentFCB == NULL)
		{
			printf(PURPLE"\nDirectory not found\n"RESET);
			return;
		}
		pathSegment = strtok(NULL, "/");
	}

	dirEntry = (DirectoryEntryMin *)currentFCB->data;
	if (dirEntry->numFCBS > 0)
	{
		printf(PURPLE"Directory not empty"RESET);
		return;
	}
	removeFromDir(fs, oldFCB, currentFCB);
	deleteFCB(fs, currentFCB);
}

void listDirectory(FileSystemFAT *fs, char *path)
{
	FCB					*currentFCB;
	FCB					*oldFCB;
	FCB					*scanningFCB;
	DirectoryEntry		*de;
	DirectoryEntryMin	*deMin;
	char				*path_copy;
	char				*pathSegment;
	int					i;
	int					count;
	int					block;

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
	while(pathSegment != NULL)
	{
		oldFCB = currentFCB;
		currentFCB = findFCB(fs, oldFCB, pathSegment);
		if (currentFCB == NULL)
		{
			printf(RED"\nDirectory not found\n"RESET);
			return;
		}
		pathSegment = strtok(NULL, "/");
	}

	if (currentFCB->isDirectory == 0)
	{
		printf(RED "%s is a file path\n"RESET, path);
		return;
	}
	deMin = (DirectoryEntryMin *)currentFCB->data;
	i = 0;
	count = 0;
	while (count < deMin->numFCBS)
	{
		scanningFCB = deMin->FCBS[i++];
		if (scanningFCB == NULL)
			continue;
		else if (scanningFCB->isDirectory == 1)
			printf(BOLDMAGENTA" %s/ "RESET, scanningFCB->fileName);
		else
			printf(" %s ", scanningFCB->fileName);
		count++;
	}
	block = 1;
	while (currentFCB->BlockCount < block)
	{
		de = getDirBlock(fs, currentFCB, block);
		i = 0;
		count = 0;
		while (count < de->numFCBS)
		{
			scanningFCB = de->FCBS[i++];
			if (scanningFCB == NULL)
				continue;
			else if (scanningFCB->isDirectory == 1)
				printf(CYAN " %s/"RESET, scanningFCB->fileName);
			else
				printf(" %s ", scanningFCB->fileName);
			count++;
		}
	}
}

void changeDirectory(FileSystemFAT *fs, char *path, char *newPath)
{
	// ? 
}


int close(FileHandle *fh)
{
	return remOpenFileInfo(openFileInfo, &openedFiles, fh);
}
