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
	printf(RED"No free block"RESET);
	return NULL;
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

void removeBit(char *bitMap, int index)
{
	int byteIndex = index / 8;
	int bitIndex = index % 8;

	bitMap[byteIndex] &= ~(1 << bitIndex);
}

FCB *createFCB(FileSystemFAT *fs, FCB *dirFCB, char *fileName, mode_type mode, int32_t isDirectory)
{
	if (fs->numFCBS >= MAX_FCBS)
	{
		printf(RED"No more FCBs"RESET);
		return NULL;
	}

	FCB *fcb = (FCB *)getDataBlock(fs);
	if (fcb == NULL)
		return NULL;

	fs->numFCBS++;
	int i = 0;
	while (fs->fcbList[i] != NULL)
		i++;
	fs->fcbList[i] = fcb;
	fcb->permissions = mode;
	fcb->isDirectory = isDirectory;
	fcb->BlockCount = 1;
	fcb->FATNextIndex = -1;
	if (isDirectory)
		fcb->filePointer = -1;
	else
		fcb->filePointer = 0;
	strncpy(fcb->fileName, fileName, MAX_FILE_NAME_LENGTH - 1);
	if (dirFCB != NULL)
		addToDir(fs, fcb, dirFCB);
	return fcb;
}

int deleteFCB(FileSystemFAT *fs, FCB *fcb)
{
	int count = 0;
	int i = 0;
	DirectoryEntryMin *deMin;

	if (fcb->isDirectory)
	{
		deMin = (DirectoryEntryMin *)fcb->data;
		if (deMin->numFCBS > 0)
		{
			printf(PURPLE"Directory not empty"RESET);
			return -1;
		}
	}

	while (count < fs->numFCBS)
	{
		if (fs->fcbList[i] != NULL)
			count++;
		else if (fs->fcbList[i] == fcb)
			break;
		i++;
	}
	fs->numFCBS--;
	fs->fcbList[i] = NULL;

	removeBit(fs->bitMap, getBlockIdx(fs, (char *)fcb));

	return 0;
}

FileEntry *getNextDataBlock(FileSystemFAT *fs, FCB *fileFcb)
{
	FileEntry	*fe;
	int			idx;
	int			pre;

	pre = -1;
	idx = fileFcb->FATNextIndex;
	while (idx != -1)
	{
		fe = (FileEntry *) getBlockPointer(fs, idx);
		if(fe->data[BLOCK_SIZE - 1] != 0)
			return fe;
		pre = idx;
		idx = fs->tableFAT[pre];
	}
	fe = (FileEntry *) getDataBlock(fs);
	if (fe == NULL)
		return NULL;
	idx = getBlockIdx(fs, (char *)fe);
	if (pre == -1)
		fileFcb->FATNextIndex = idx;
	else
		fs->tableFAT[pre] = idx;
	fileFcb->BlockCount++;
	return fe;
}

DirectoryEntry *getDirBlock(FileSystemFAT *fs, FCB *dirFcb, int deep)
{
	int idx;
	if (deep == 0)
	{
		printf("deep <= 0\n");
		return NULL;
	}
	deep--;
	idx = dirFcb->FATNextIndex;
	while (idx != -1 && deep > 0)
	{
		idx = fs->tableFAT[idx];
		deep--;
	}
	if (idx != -1)
		return (DirectoryEntry *) getBlockPointer(fs, idx);
	else
		return NULL;
}

DirectoryEntry *createDirBlock(FileSystemFAT *fs, FCB *dirFcb, int deep)
{
	DirectoryEntry	*de;
	int				idx;
	int				pre;

	de = getDirBlock(fs, dirFcb, deep);
	if (de != NULL)
		return de;

	deep--;
	pre = -1;
	idx = dirFcb->FATNextIndex;
	while (idx != -1 && deep > 0)
	{
		pre = idx;
		idx = fs->tableFAT[idx];
		deep--;
	}
	de = (DirectoryEntry *) getDataBlock(fs);
	if (de == NULL)
		return NULL;
	idx = getBlockIdx(fs, (char *)de);
	if (pre == -1)
		dirFcb->FATNextIndex = idx;
	else
		fs->tableFAT[pre] = idx;
	dirFcb->BlockCount++;
	return de;
}

void removeFromDir(FileSystemFAT *fs, FCB *dirFCB, FCB *fcb)
{
	DirectoryEntryMin	*deMin;
	DirectoryEntry		*de;
	int					idx;
	int					i;
	int 				pre;

	i = 0;
	deMin = (DirectoryEntryMin *) &dirFCB->data;
	while (i < MAX_DIR_IN_BLOCK)
	{
		if (deMin->FCBS[i] == fcb)
		{
			deMin->numFCBS = deMin->numFCBS - 1;
			deMin->FCBS[i] = NULL;
			return;
		}
		i++;
	}
	idx = dirFCB->FATNextIndex;
	while (1)
	{
		de = (DirectoryEntry *) getBlockPointer(fs, idx);
		i = 0;
		while (i < MAX_DIR_IN_BLOCK)
		{
			if (de->FCBS[i] == fcb)
			{
				de->numFCBS = de->numFCBS - 1;
				de->FCBS[i] = NULL;
				dirFCB->BlockCount -= 1;
				if (de->numFCBS == 0)
				{
					removeFatIndex(fs, dirFCB);
					removeBit(fs->bitMap, getBlockIdx(fs, (char *)de));
				}
				return;
			}
			i++;
		}
	}
}

void removeFatIndex(FileSystemFAT *fs, FCB *dirFCB)
{
	int idx;
	int pre;
	int prepre;

	pre = -1;
	prepre = -1;
	idx = dirFCB->FATNextIndex;
	while (idx != -1)
	{
		prepre = pre;
		pre = idx;
		idx = fs->tableFAT[idx];
	}
	if (prepre == -1)
		dirFCB->FATNextIndex = -1;
	else
		fs->tableFAT[prepre] = -1;
}

void addToDir(FileSystemFAT *fs, FCB *fcb, FCB *dirFCB)
{
	DirectoryEntryMin	*deMin;
	DirectoryEntry		*de;
	int					count;
	int					i;

	i = 0;
	count = 0;
	deMin = (DirectoryEntryMin *) &dirFCB->data;
	if (deMin->numFCBS < MAX_DIR_IN_MIN)
	{
		while (1)
		{
			if (deMin->FCBS[i] == NULL)
			{
				deMin->numFCBS = deMin->numFCBS + 1;
				deMin->FCBS[i] = fcb;
				break;
			}
			i++;
		}
	}
	else
	{
		int deep = 1;
		de = createDirBlock(fs, dirFCB, deep);
		while (de != NULL && de->numFCBS == MAX_DIR_IN_BLOCK)
			de = getDirBlock(fs, dirFCB, ++deep);
		if (de == NULL)
		{
			perror(RED"No more directory blocks"RESET);
			return;
		}
		while (1)
		{
			if (de->FCBS[i] == NULL)
			{
				de->numFCBS++;
				de->FCBS[i] = fcb;
				break;
			}
			i++;
		}
	}
}

FCB *findFCB(void *c_dir, char *name, int isMin)
{
	int					i;
	int					count;
	FCB					*scanning;
	DirectoryEntryMin	*dirMin;
	DirectoryEntry		*dir;

	i = 0;
	count = 0;
	if (isMin)
	{
		dirMin = (DirectoryEntryMin *) c_dir;
		while (count < dirMin->numFCBS)
		{
			scanning = dirMin->FCBS[i++];
			if (scanning == NULL)
				continue;
			else if (strcmp(scanning->fileName, name) == 0)
				return scanning;
			count++;
		}
	}
	else
	{
		dir = (DirectoryEntry *) c_dir;
		while (count < dir->numFCBS)
		{
			scanning = dir->FCBS[i++];
			if (scanning == NULL)
			{
				printf("scanning == NULL ");
			}
			else if (strcmp(scanning->fileName, name) == 0)
				return scanning;
			count++;
		}
	}
	return NULL;
}




FileHandle *newOpenFileInfo(FileHandle **openFileInfo, int *openedFiles)
{
	int i = 0;
	FileHandle *ret;

	if (*openedFiles >= MAX_OPEN_FILES)
	{
		printf(RED"too much files opened\n"RESET);
		return NULL;
	}

	ret = (FileHandle *) malloc(sizeof(FileHandle));

	while (i < MAX_OPEN_FILES)
	{
		if (openFileInfo[i] == NULL)
		{
			openFileInfo[i] = ret;
			*openedFiles = *openedFiles + 1;
			return ret;
		}
		i++;
	}
}


FileHandle *findOpenFileInfo(FileHandle **openFileInfo, FCB *toFind)
{
	int i = 0;
	while (i < MAX_OPEN_FILES)
	{
		if (openFileInfo[i]->fcb == toFind)
			return openFileInfo[i];
		i++;
	}
	perror(RED"file Not Found"RESET);
}

int remOpenFileInfo(FileHandle **openFileInfo, int *openedFiles, FileHandle *elem)
{
	int i = 0;
	while (i < MAX_OPEN_FILES)
	{
		if (openFileInfo[i] == elem)
		{
			*openedFiles = *openedFiles - 1;
			openFileInfo[i] = NULL;
			free(elem);
			return 0;
		}
		i++;
	}
	printf(RED"file Not Opened"RESET);
	return -1;
}


void updateFilePointer(FileHandle *fileInfo)
{
	fileInfo->filePointer = fileInfo->fcb->filePointer;
}



int pathIsValid(char *path)
{
	int count = 0;

	char *path_copy = strdup(path);
	char *pathSegment = strtok(path_copy, "/");

	if (pathSegment != NULL)
		pathSegment = strtok(NULL, "/");

	while (pathSegment != NULL)
	{
		if (strcmp(pathSegment, ROOT_DIR_NAME) == 0)
		{
			printf(RED"Invalid path\n"RESET);
			return 0;
		}
		pathSegment = strtok(NULL, "/");
	}


	if (!path)
	{
		printf(RED"Null path\n"RESET);
		return 0;
	}
    for (int i = 0; path[i] != '\0'; i++)
	{
        if (path[i] == '.' || count >= MAX_FILE_NAME_LENGTH || path[i] == ' ')
		{
			printf(RED"Invalid path\n"RESET);
			return 0;
		}
		if (path[i] == '/')
			count = 0;
		else
			count++;
    }
	return 1;
}

int nameIsValid(char *name)
{
	int countDot = 0;
	int count = 0;
	int i;

	if (!name)
	{
		printf(RED"Null path\n"RESET);
		return 0;
	}
	for (i = 0; name[i] != '\0'; i++)
	{
		if (name[i] == '.')
			countDot++;
		else if (name[i] == '/')
		{
			printf(RED"Invalid File Name\n"RESET);
			return 0;
		}
		else if (name[i] != ' ')
			count++;
	}
	if (countDot > 1 ||  i >= MAX_FILE_NAME_LENGTH || count == 0)
	{
		printf(RED"Invalid File Name\n"RESET);
		return 0;
	}
	return 1;
}
