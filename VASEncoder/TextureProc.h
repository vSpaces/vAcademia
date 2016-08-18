
#ifdef USE_FREE_IMAGE
typedef struct _SMemFile
{
	void* buffer;
	int currentPosition;
	int size;
} SMemFile;


inline unsigned _stdcall _ReadProc(void *buffer, unsigned size, unsigned count, fi_handle handle) 
{
	SMemFile* memFile = (SMemFile*)handle;

	int readSize = size * count;
	if (readSize + memFile->currentPosition > memFile->size)
	{
		readSize = memFile->size - memFile->currentPosition;
	}

	memcpy((unsigned char *)buffer, (unsigned char *)memFile->buffer + memFile->currentPosition, readSize);
	memFile->currentPosition += readSize;

	return readSize;
}

inline int _stdcall _SeekProc(fi_handle handle, long offset, int origin) 
{
	assert(origin != SEEK_END);
	SMemFile* memFile = (SMemFile*)handle;

	if (origin == SEEK_SET) 
	{
		memFile->currentPosition = offset;
	} 
	else 
	{
		memFile->currentPosition += offset;
	}

	return 0;
}

inline long _stdcall _TellProc(fi_handle handle) 
{
	SMemFile* memFile = (SMemFile*)handle;

	return memFile->currentPosition;
}

inline unsigned _stdcall _WriteProc(void *buffer, unsigned size, unsigned count, fi_handle handle) 
{   
	SMemFile* memFile = (SMemFile*)handle;

	int writeSize = size * count;
	if (writeSize + memFile->currentPosition > memFile->size)
	{
		writeSize = memFile->size - memFile->currentPosition;
	}

	memcpy((BYTE *)(memFile->buffer) + memFile->currentPosition, buffer, writeSize);
	memFile->currentPosition += writeSize;


	return count;
}

#endif
