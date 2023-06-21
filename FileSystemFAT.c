#include "FileSystemFAT.h"
#include "utils.h"

int pathIsValid(char *path) {
	int count = 0;

	if (!path)
	{
		printf(COLOR_RED"Null path\n"COLOR_RESET);
		return 0;
	}
    for (int i = 0; path[i] != '\0'; i++)
	{
        if (path[i] == '.' || count >= MAX_FILE_NAME_LENGTH || path[i] == ' ')
		{
			printf(COLOR_RED"Invalid path\n"COLOR_RESET);
			return 0;
		}
		if (path[i] == '/')
			count = 0;
		else
			count++;
    }
	return 1;
}

int nameIsValid(char *name) {
	int countDot = 0;
	int count = 0;
	int i;

	if (!name)
	{
		printf(COLOR_RED"Null path\n"COLOR_RESET);
		return 0;
	}
	for (i = 0; name[i] != '\0'; i++)
	{
		if (name[i] == '.')
			countDot++;
		else if (name[i] == '/')
		{
			printf(COLOR_RED"Invalid File Name\n"COLOR_RESET);
			return 0;
		}
		else if (name[i] != ' ')
			count++;
	}
	if (countDot > 1 ||  i >= MAX_FILE_NAME_LENGTH || count == 0)
	{
		printf(COLOR_RED"Invalid File Name\n"COLOR_RESET);
		return 0;
	}
	return 1;
}

FileHandle *createFile(FileSystemFAT *fs, char *path, char *name, mode_type mode)
{
	int32_t 			i;
	int32_t 			count;
	char				*path_copy;
	char				*pathSegment;
	DirectoryEntry		*currentDir;
	DirectoryEntryMin	*firstDir;
	FCB					*currentFCB;
	FCB					*scanningFCB;
	FCB					*retFCB;
	FileHandle 			*ret;

	if (!nameIsValid(name))
		return NULL;

	if (!pathIsValid(path))
		return NULL;

	path_copy = strdup(path);
	pathSegment = strtok(path_copy, "/");

	if (strcmp(pathSegment, ROOT_DIR_NAME) != 0)
	{
		printf("%s",pathSegment);
		printf(COLOR_RED"\npath doesn't start with "COLOR_RESET);
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

	if (currentFCB == fs->rootFCB)
		retFCB = findFCB((void *)currentFCB->data, name, 1);
	else
		retFCB = findFCB((void *)currentFCB->data, name, 0);

	if(retFCB == NULL)
	{
		retFCB = createFCB(fs, currentFCB, name, mode, 0);
		ret = newOpenFileInfo();
		if(!ret)
			return NULL;
		ret->fileSystem = fs;
		ret->fcb = retFCB;
		ret->filePointer = 0;
		return ret;
	}
	else
	{
		ret = findOpenFileInfo(retFCB);
		updateFilePointer(ret);
		return ret;
	}
}

void eraseFile(char *fileName)
{

}

void createDirectory(FileSystemFAT *fs, char *path, char *dirName)
{
	if (!pathIsValid(path))
		return;
	if (!pathIsValid(dirName))
		return;
	FCB *parent = createNewPath(path);
	if (parent == NULL)
	{
		printf(COLOR_RED"Parent problem\n"COLOR_RESET);
		return;
	}
	// (if esiste return)
	FCB *newDir = createFCB(fs, parent, dirName, R, 1);
	if (newDir == NULL)
	{
		printf(COLOR_RED"New dir problem\n"COLOR_RESET);
		return;
	}


}
