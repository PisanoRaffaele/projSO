#include "FileSystemFAT.h"
#include "prints.h"

openFileInfo *ofiTable[MAX_OPEN_FILES];
int openedFiles = 0;

void printOpenFileInfo()
{
	printf(CYAN "Open Files: \n" RESET);
	printf("numOpenFiles: %d\n", openedFiles);

	for (int i = 0; i < MAX_OPEN_FILES; i++)
	{
		if (ofiTable[i] != NULL)
			printf("fileInfo[%d]: %s\n", i, ofiTable[i]->fcb->fileName);
	}
}


FileHandle *createFile(FileSystemFAT *fs, char *path, char *name, mode_type mode)
{
	FCB				*currentFCB;
	FCB				*retFCB;
	openFileInfo	*ofi;
	FileHandle		*ret;

	if (!nameIsValid(name))
		return NULL;

	currentFCB = createDirectoryWrapped(fs, path);

	retFCB = findFCB(fs, currentFCB, name);

	if(retFCB == NULL)
	{
		ofi = newOpenFileInfo(ofiTable, &openedFiles);
		if(!ofi)
			return NULL;
		ret = newFileHandle(ofi, mode);
		if(!ret)
		{
			remOpenFileInfo(ofiTable, &openedFiles, ofi);
			return NULL;
		}
		retFCB = createFCB(fs, currentFCB, name, 0);
		if (!retFCB)
		{
			remOpenFileInfo(ofiTable, &openedFiles, ofi);
			free(ret);
			return NULL;
		}
		ofi->fileSystem = fs;
		ofi->fcb = retFCB;
		return ret;
	}
	else
	{
		ofi = findOpenFileInfo(ofiTable, retFCB);
		if (!ofi)
		{
			ofi = newOpenFileInfo(ofiTable, &openedFiles);
			if(!ofi)
				return NULL;
			ofi->fileSystem = fs;
			ofi->fcb = retFCB;
			ret = newFileHandle(ofi, mode);
			if(!ret)
			{
				remOpenFileInfo(ofiTable, &openedFiles, ofi);
				return NULL;
			}
			return ret;
		}
		ret = newFileHandle(ofi, mode);
		if(!ret)
			return NULL;
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

	if (findOpenFileInfo(ofiTable, currentFCB))
	{
		printf(RED"\nCan't delete opened File\n"RESET);
		return;
	}

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

void changeDirectory(FileSystemFAT *fs, char *name, char *fileDir, char *newPath)
{
	FCB					*currentFCB;
	FCB					*oldFCB;
	FCB					*newDirFCB;
	char				*path_copy;
	char				*pathSegment;

	if (!pathIsValid(fileDir))
		return;

	if (!nameIsValid(name))
		return NULL;

	path_copy = strdup(fileDir);
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

	newDirFCB = createDirectoryWrapped(fs, newPath);
	addToDir(fs, currentFCB, newDirFCB);
}


int close(FileHandle *fh)
{
	int ret;
	fh->info->numFileHandle = fh->info->numFileHandle - 1;
	if (fh->info->numFileHandle == 0)
		ret = remOpenFileInfo(ofiTable, &openedFiles, fh->info);
	if (!ret)
	{
		fh->info->numFileHandle = fh->info->numFileHandle + 1;
		return -1;
	}
	free(fh);
	return 0;
}


int fs_read(FileHandle *fd, void *buf, size_t count)
{
	
}
