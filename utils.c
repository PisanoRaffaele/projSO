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

FCB *createFCB(FileSystemFAT *fs, char *fileName, mode_type mode, int32_t isDirectory)
{
	if (fs->numFCBS >= MAX_FCBS)
	{
		perror("No more FCBs");
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
	strncpy(fcb->fileName, fileName, 7);
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

	removeBit(fs, getBlockIdx(fs, (char *)fcb));
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
		while (count < deMin->numFCBS)
		{
			if (deMin->FCBS[i] != NULL) // 0 == NULL nei puntatori
				count++;
			else if (deMin->FCBS[i] == NULL)
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
			perror("No more directory blocks");
			return;
		}
		while (count < de->numFCBS)
		{
			if (de->FCBS[i] != NULL) //oppure 0?
				count++;
			else if (de->FCBS[i] == NULL)
			{
				de->numFCBS++;
				de->FCBS[i] = fcb;
				break;
			}
			i++;
		}
	}
}


FCB *createNewPath(char *path)
{
	return NULL;
}
