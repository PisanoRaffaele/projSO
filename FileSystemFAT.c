#include "FileSystemFAT.h"
#include "utils.h"

int pathIsValid(char *path) {
	int count = 0;

	if (!path)
	{
		printf("Null path\n");
		return 0;
	}
    for (int i = 0; path[i] != '\0'; i++)
	{
        if (path[i] == '.' || count >= MAX_FILE_NAME_LENGTH)
		{
			printf("Invalid path\n");
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
		printf("Null path\n");
		return 0;
	}
	for (i = 0; name[i] != '\0'; i++)
	{
		if (name[i] == '.')
			countDot++;
		else if (name[i] == '/')
		{
			printf("Invalid File Name\n");
			return 0;
		}
		else if (name[i] != ' ')
			count++;
	}
	if (countDot > 1 ||  i >= MAX_FILE_NAME_LENGTH || count == 0)
	{
		printf("Invalid File Name\n");
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

	if (!nameIsValid(name))
		return NULL;

	if (!pathIsValid(path))
		return NULL;

	if (path == "")
	{
		retFCB = createFCB(fs, name, mode, 0);
		FileHandle *ret = (FileHandle *)malloc(sizeof(FileHandle));
		ret->fileSystem = fs;
		ret->fcb = retFCB;
		ret->filePointer = 0;
		return ret;
	}

	path_copy = strdup(path);
	pathSegment = strtok(path_copy, "/");

	currentFCB = fs->rootFCB;
	if (pathSegment != NULL)
		firstDir = (DirectoryEntryMin *)currentFCB->data;
		if (!firstDir->numFCBS == 0)
		{
			i = 0;
			count = 0;
			while (count < firstDir->numFCBS)
			{
				scanningFCB = firstDir->FCBS[i++];
				if (scanningFCB == NULL)
					continue;
				else if (strcmp(scanningFCB->fileName, pathSegment) == 0)
				{
					currentFCB = scanningFCB;
					break;
				}
				else
					count++;
			}
		}
	while(pathSegment != NULL)
	{
		i = 0;
		count = 0;
		while(count < firstDir->numFCBS)
		{
			scanningFCB = firstDir->FCBS[i];
			if (scanningFCB == NULL)
			{
				i++;
				continue;
			}
			else if (strcmp(scanningFCB->fileName, pathSegment) == 0)
			{
				currentFCB = scanningFCB;
				currentDir = currentFCB->data;
				pathSegment = strtok(NULL, "/");
				break;
			}
			else
			{
				i++;
				count++;
			}
		}
		if (count == currentDir->numFCBS)
		{
			printf("Path does not exist\n");
			return NULL;
			//create Directory

		}
	}






	// i = 0;
	// while (i < MAX_FCBS && i < fs->numFCBS)
	// {
	// 	if (fs->fcbList[i] == NULL)
	// 		continue;
	// 	if (strcmp(fs->fcbList[i]->fileName, pathname) == 0)
	// 	{
	// 		printf("File already exists\n");
	// 		return NULL;
	// 	}
	// 	i++;
	// }
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
		printf("Parent problem\n");
		return;
	}
	FCB *newDir = createFCB(fs, dirName, R, 1);
	if (newDir == NULL)
	{
		printf("New dir problem\n");
		return;
	}


}
