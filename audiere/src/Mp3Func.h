#pragma once

#include "audiere.h"


class CMp3Func
{
public:
	CMp3Func(void);
	~CMp3Func(void);
	static int check_aid(const unsigned char *header);
	static int is_syncword_mp123(const void *const headerptr);
	static int fread(void *buf, size_t size, size_t num, audiere::FilePtr file);
	static int fskip(audiere::FilePtr fp, long offset, int whence);
	static int fseek(audiere::FilePtr fp, long offset, int whence);
	static double getFileSize(audiere::FilePtr fp);
};
