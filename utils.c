#include "utils.h"

FileHandle *openFileInfo[MAX_OPEN_FILES];
int openedFiles = 0;

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
	perror(COLOR_RED"No free block"COLOR_RESET);
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
		perror(COLOR_RED"No more FCBs"COLOR_RESET);
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
		addFcbToDirFcb(fs, fcb, dirFCB);
	return fcb;
}

void deleteFCB(FileSystemFAT *fs, FCB *fcb)
{
	int count = 0;
	int i = 0;

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

DirectoryEntry *getNextDirBlock(FileSystemFAT *fs, FCB *dirFcb)
{
	DirectoryEntry *de;
	int				idx;
	int				pre;

	pre = -1;
	idx = dirFcb->FATNextIndex;
	while (idx != -1)
	{
		de = (DirectoryEntry *) getBlockPointer(fs, idx);
		if (de->numFCBS < MAX_DIR_IN_MIN)
			return de;
		pre = idx;
		idx = fs->tableFAT[pre];
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


void addFcbToDirFcb(FileSystemFAT *fs, FCB *fcb, FCB *dirFCB)
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
				deMin->numFCBS++;
				deMin->FCBS[i] = fcb;
				break;
			}
			i++;
		}
	}
	else
	{
		de = getNextDirBlock(fs, dirFCB);
		if (de == NULL)
		{
			perror(COLOR_RED"No more directory blocks"COLOR_RESET);
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

FileHandle *newOpenFileInfo()
{
	int i = 0;
	FileHandle * ret;
	if (openedFiles >= MAX_OPEN_FILES)
	{
		perror(COLOR_RED"too much files opened"COLOR_RESET);
		return NULL;
	}

	ret = (FileHandle *) malloc(sizeof(FileHandle));

	while (i < MAX_OPEN_FILES)
	{
		if (openFileInfo[i] == NULL)
		{
			openFileInfo[i] = ret;
			openedFiles++;
			return ret;
		}
		i++;
	}
}

FileHandle *findOpenFileInfo(FCB *toFind)
{
	int i = 0;
	while (i < MAX_OPEN_FILES)
	{
		if (openFileInfo[i]->fcb == toFind)
			return openFileInfo[i];
		i++;
	}
	perror(COLOR_RED"file Not Found"COLOR_RESET);
}

void remOpenFileInfo(FileHandle *elem)
{
	int i = 0;
	while (i < MAX_OPEN_FILES)
	{
		if (openFileInfo[i] == elem)
		{
			openedFiles--;
			openFileInfo[i] = NULL;
			free(elem);
			return;
		}
		i++;
	}
	perror(COLOR_RED"file Not Found"COLOR_RESET);
}

void updateFilePointer(FileHandle *fileInfo)
{
	fileInfo->filePointer = fileInfo->fcb->filePointer;
}

FCB *createNewPath(char *path)
{
	return NULL;
}
