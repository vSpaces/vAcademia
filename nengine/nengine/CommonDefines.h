#pragma once

// errors
#define	ERR_OBJECT_NOT_FOUND	-1

// limits
#define MAX_ALPHA_POLYGONS		10000
#define	MAX_SHADER_COUNT		50
#define	MAX_PATH_LENGTH			50
#define MAX_HEIGHTMAPS			32
#define MAX_PARTICLES			2000
#define MAX_TEXTURES			500
#define MAX_MODELS				50
#define MAX_SLOTS				5

// other
#define BLACK_INDEX				0

struct fvec
{
	float x0, y0;
	float dx, dy;
	float len;
};

// some definitions for DDS files
#pragma pack (push, 1)

struct DDS_PIXELFORMAT
{
        unsigned long dwSize;
        unsigned long dwFlags;
        unsigned long dwFourCC;
        unsigned long dwRGBBitCount;
        unsigned long dwRBitMask;
        unsigned long dwGBitMask;
        unsigned long dwBBitMask;
        unsigned long dwABitMask;
};

struct DDS_HEADER
{
        unsigned long dwSize;
        unsigned long dwFlags;
        unsigned long dwHeight;
        unsigned long dwWidth;
        unsigned long dwPitchOrLinearSize;
        unsigned long dwDepth;
        unsigned long dwMipMapCount;
        unsigned long dwReserved1[11];
        DDS_PIXELFORMAT ddspf;
        unsigned long dwCaps1;
        unsigned long dwCaps2;
        unsigned long dwReserved2[3];
};

#pragma	pack (pop)

enum
{
	// bit flags for header
	DDS_CAPS	    = 0x00000001,
	DDS_HEIGHT	    = 0x00000002,
	DDS_WIDTH	    = 0x00000004,
	DDS_PITCH	    = 0x00000008,
	DDS_PIXELFORMAT = 0x00001000,
	DDS_MIPMAPCOUNT = 0x00020000,
	DDS_LINEARSIZE  = 0x00080000,
	DDS_DEPTH	    = 0x00800000,

	// flags for pixel formats
	DDS_ALPHA_PIXELS = 0x00000001,
	DDS_ALPHA        = 0x00000002,
	DDS_FOURCC	     = 0x00000004,
	DDS_RGB	         = 0x00000040,
    DDS_RGBA         = 0x00000041,

	// flags for complex caps
	DDS_COMPLEX	   = 0x00000008,
	DDS_TEXTURE	   = 0x00001000,
	DDS_MIPMAP	   = 0x00400000,

	// flags for cubemaps
	DDS_CUBEMAP	          = 0x00000200,
	DDS_CUBEMAP_POSITIVEX = 0x00000400,
	DDS_CUBEMAP_NEGATIVEX = 0x00000800,
	DDS_CUBEMAP_POSITIVEY = 0x00001000,
	DDS_CUBEMAP_NEGATIVEY = 0x00002000,
	DDS_CUBEMAP_POSITIVEZ = 0x00004000,
	DDS_CUBEMAP_NEGATIVEZ = 0x00008000,
	DDS_VOLUME		      = 0x00200000
};