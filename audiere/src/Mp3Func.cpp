#include "stdafx.h"
#include ".\mp3func.h"
#include <string.h>
#include "debug.h"

using namespace audiere;

CMp3Func::CMp3Func(void)
{
}

CMp3Func::~CMp3Func(void)
{
}

int CMp3Func::check_aid(const unsigned char *header)
{
	return 0 == memcmp(header, "AiD\1", 4);
}

int CMp3Func::is_syncword_mp123(const void *const headerptr)
{
	const unsigned char *const p = (const unsigned char *const)headerptr;
	static const char abl2[16] =
	{ 0, 7, 7, 7, 0, 7, 0, 0, 0, 0, 0, 8, 8, 8, 8, 8 };

	if ((p[0] & 0xFF) != 0xFF)
		return 0;       /* first 8 bits must be '1' */
	if ((p[1] & 0xE0) != 0xE0)
		return 0;       /* next 3 bits are also */
	if ((p[1] & 0x18) == 0x08)
		return 0;       /* no MPEG-1, -2 or -2.5 */        
	if ((p[1] & 0x06) == 0x00)
		return 0;       /* no Layer I, II and III */
#ifndef USE_LAYER_1
	if ((p[1] & 0x06) == 0x03*2)
		return 0; /* layer1 is not supported */
#endif
#ifndef USE_LAYER_2
	if ((p[1] & 0x06) == 0x02*2)
		return 0; /* layer1 is not supported */
#endif
	/*if (!(((p[1] & 0x06) == 0x03*2 && input_format == sf_mp1)
		|| ((p[1] & 0x06) == 0x02*2 && input_format == sf_mp2)
		|| ((p[1] & 0x06) == 0x01*2 && input_format == sf_mp3)))
		return 0; /* imcompatible layer with input file format */
	if ((p[2] & 0xF0) == 0xF0)
		return 0;       /* bad bitrate */
	if ((p[2] & 0x0C) == 0x0C)
		return 0;       /* no sample frequency with (32,44.1,48)/(1,2,4)     */
	if ((p[1] & 0x18) == 0x18 && (p[1] & 0x06) == 0x04
		&& abl2[p[2] >> 4] & (1 << (p[3] >> 6)))
		return 0;
	if ((p[3] & 3) == 2)
		return 0;       /* reserved enphasis mode */
	return 1;
}

int CMp3Func::fread(void *buf, size_t size, size_t num, FilePtr file)
{
	int readBytes = file->read(buf, num);
	return readBytes;
}

int CMp3Func::fseek(FilePtr fp, long offset, int whence)
{
	bool ret;
	switch (whence)
	{
	case SEEK_SET:
		ret = fp->seek(offset, File::BEGIN);
		break;
	case SEEK_END:
		ret = fp->seek(offset, File::END);
		break;
	case SEEK_CUR:
		ret = fp->seek(offset, File::CURRENT);
		break;
	default:
		ret = false;
		break;
	}
	if (ret)
        return 0;
	else
		return -1;
}

int CMp3Func::fskip(FilePtr fp, long offset, int whence)
{
#ifndef PIPE_BUF
	char    buffer[4096];
#else
	char    buffer[PIPE_BUF];
#endif
	int     read;

	if (0 == CMp3Func::fseek(fp, offset, whence))
		return 0;

	if (whence != SEEK_CUR || offset < 0) 
	{
		ADR_LOG("fskip problem: Mostly the return status of functions is not evaluate so it is more secure to polute <stderr>.\n");
		return -1;
	}

	while (offset > 0) {
		read = offset > sizeof(buffer) ? sizeof(buffer) : offset;
		if ((read = CMp3Func::fread(buffer, 1, read, fp)) <= 0)
			return -1;
		offset -= read;
	}

	return 0;
}

double CMp3Func::getFileSize(FilePtr fp)
{
	int curPos = fp->tell();
	fp->seek(0, File::BEGIN);
	int start = fp->tell();

	fp->seek(0, File::END);
	int end = fp->tell();

	fp->seek(curPos, File::BEGIN);
	
	if (end >= start)
		return end - start;
    return -1.0;
}