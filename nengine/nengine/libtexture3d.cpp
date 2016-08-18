

#ifdef	_WIN32
    #pragma	warning (disable:4786)		
#endif

#include "StdAfx.h"
#include "LibTexture3D.h"
#include "Data.h"
#include <stdio.h>
#include <string.h>
#include "CommonDefines.h"
#include "cal3d/memory_leak.h"

Data* GetFile(const wchar_t* fileName)
{
	MP_NEW_P(data, Data, fileName);
    return data;
}

unsigned CreateTexture3D (bool mipmap, const wchar_t* fileName)
{
    Data* data = GetFile(fileName);

    if (data == NULL)
	{
        return 0;
	}

    unsigned texture = CreateTexture3D (mipmap, data);

    MP_DELETE(data);

    return texture;
}

unsigned CreateTexture3D(bool mipmap, Data* data)
{
    DDS_HEADER ddsd;
    char filecode[4];

    data->SeekAbs(0);
    data->GetBytes ( filecode, 4 );

    if (strncmp(filecode, "DDS ", 4) != 0)
	{
        return 0;
	}

    data->GetBytes(&ddsd, sizeof(ddsd));

    if ((ddsd.ddspf.dwFlags & DDS_FOURCC) != 0)
	{
        return 0;
	}

    if ((ddsd.dwFlags & DDS_DEPTH) == 0)
	{
        return 0;
	}

    int numComponents = 0;

    if ( ddsd.ddspf.dwFlags & DDS_ALPHA_PIXELS )
	{
        numComponents = 4;
	}
    else                                                
	{
        numComponents = 3;
	}

    int bytesPerLine = ddsd.dwWidth * numComponents;
    int width = ddsd.dwWidth;
    int height = ddsd.dwHeight;
    int depth = ddsd.dwDepth;
    int rowsCount = depth * height;

    if ((bytesPerLine & 3) != 0)                      
	{
        bytesPerLine += 4 - (bytesPerLine & 3);
	}

    byte* buf = MP_NEW_ARR(byte, bytesPerLine);
    byte* image = MP_NEW_ARR(byte, width * height * depth * numComponents);
    byte* dest = image;

    for (int i = 0; i < rowsCount; i++)
    {
        data->GetBytes(buf, bytesPerLine);

        byte* src = buf;

        for (register int j = 0; j < (int)ddsd.dwWidth; j++ )
        {
            dest[0] = src[2];
            dest[1] = src[1];
            dest[2] = src[0];

            if (numComponents == 4)
			{
                dest[3] = src[3];
			}

            dest += numComponents;
            src += numComponents;
        }
    }

    unsigned texture;

    GLFUNC(glGenTextures)(1, &texture);
    GLFUNC(glBindTexture)(GL_TEXTURE_3D_EXT, texture);

    GLFUNC(glTexImage3DEXT)(GL_TEXTURE_3D_EXT, 0, numComponents, width, height, depth, 0,
                      numComponents == 3 ? GL_RGB : GL_RGBA, GL_UNSIGNED_BYTE, image);

    GLFUNC(glTexParameteri)(GL_TEXTURE_3D_EXT, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    GLFUNC(glTexParameteri)(GL_TEXTURE_3D_EXT, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    GLFUNC(glTexParameteri)(GL_TEXTURE_3D_EXT, GL_TEXTURE_WRAP_S, GL_REPEAT);
    GLFUNC(glTexParameteri)(GL_TEXTURE_3D_EXT, GL_TEXTURE_WRAP_T, GL_REPEAT);
    GLFUNC(glTexParameteri)(GL_TEXTURE_3D_EXT, GL_TEXTURE_WRAP_R, GL_REPEAT);
	GLFUNC(glTexParameteri)(GL_TEXTURE_3D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 8);

                   
    if (mipmap)
    {
        GLFUNC(glTexParameteri)(GL_TEXTURE_3D_EXT, GL_GENERATE_MIPMAP_SGIS, GL_TRUE);
        GLFUNC(glTexParameteri)(GL_TEXTURE_3D_EXT, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    }

	MP_DELETE_ARR(image);
    MP_DELETE_ARR(buf);

    return texture;
}

