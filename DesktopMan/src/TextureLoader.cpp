#include "StdAfx.h"
#include "../include/TextureLoader.h"
//#include "HelperFunctions.h"
#include "../include/FreeImage.h"
#include "../include/TextureProc.h"
#ifdef VAS_ENCODER
#include "../../VASEncoder/vase.h"
#endif
#define MAX_DROP_IMAGE_SIZE		2000.0

CTextureLoader::CTextureLoader():
	m_dib(NULL),
	reverseRGB(FALSE)
{
	FreeImage_Initialise();
#ifdef VAS_ENCODER
	waveletEncoder = NULL;
#endif
	m_waveletsDIB = NULL;
}

#ifdef VAS_ENCODER
void CTextureLoader::InitNewCodec(void* taskManager, oms::omsContext* aContext)
{
	MP_NEW_V2( waveletEncoder, VASWaveletEncoder, taskManager, aContext);
}
#endif

bool CTextureLoader::LoadFromMemory(void* pictureData, unsigned int dataSize, std::string fileName)
{
	if (m_dib != NULL)
	{
        FreeImage_Unload((FIBITMAP *)m_dib);
		m_dib = NULL;
	}

	FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;

	fif = FreeImage_GetFileType(fileName.c_str(), 0);

	if (fif == FIF_UNKNOWN) 
	{
		fif = FreeImage_GetFIFFromFilename(fileName.c_str());
	}

	if (fif == FIF_UNKNOWN)
	{
		return false;
	}

	if (fif != FIF_JPEG && fif != FIF_PNG && fif != FIF_BMP && fif != FIF_GIF)
	{
		return false;
	}


	FreeImageIO io;
	io.read_proc = _ReadProc;
	io.write_proc = NULL;
	io.tell_proc = _TellProc;
	io.seek_proc = _SeekProc;

	SMemFile memFile;
	memFile.buffer = pictureData;
	memFile.size = dataSize;
	memFile.currentPosition = 0;

		m_dib = FreeImage_LoadFromHandle(fif, &io, (fi_handle)&memFile);


	if (fileName!= JPEG_DEFAULT_NAME)
	{
		if ( GetChannelCount() < 4)
		{
			void* convertedDib = FreeImage_ConvertTo32Bits((FIBITMAP *)m_dib);
			if (m_dib)
				FreeImage_Unload((FIBITMAP *)m_dib);
			m_dib = convertedDib;
		}
		
		int width = GetWidth();
		int height = GetHeight();

		double ratio =1;

		if ( width > MAX_DROP_IMAGE_SIZE || height > MAX_DROP_IMAGE_SIZE )
		{
			if (width > height)
				ratio = MAX_DROP_IMAGE_SIZE	/(double)width;
			else
				ratio = MAX_DROP_IMAGE_SIZE	/(double)height;

			int newWidth = (int)(ratio * width);
			int newHeight = (int)(ratio * height);
			Resize(newWidth,newHeight);
		}
	}

	OnLoaded(false);

	return true;
}

void CTextureLoader::Resize(int width, int height)
{
	if (m_dib != NULL)
	{
		FIBITMAP * partImage = (FIBITMAP *)m_dib;
		m_dib = FreeImage_Rescale((FIBITMAP *)m_dib, width, height, FILTER_BILINEAR);
		FreeImage_Unload(partImage);
	}
}

void CTextureLoader::OnLoaded(bool isBmp)
{
	int bpp = FreeImage_GetBPP((FIBITMAP *)m_dib);

	void* convertedDib = NULL;
	if ((bpp == 8 && !FreeImage_GetTransparencyCount((FIBITMAP *)m_dib)) || (bpp == 16))
	{
		convertedDib = FreeImage_ConvertTo24Bits((FIBITMAP *)m_dib);
	}
	else if ((bpp != 24) && (bpp != 32))
	{
		convertedDib = FreeImage_ConvertTo32Bits((FIBITMAP *)m_dib);
	}

	if (isBmp && bpp == 32)
	{
		convertedDib = FreeImage_ConvertTo24Bits((FIBITMAP *)m_dib);
	}

	if( convertedDib)
	{
		if (m_dib)
			FreeImage_Unload((FIBITMAP *)m_dib);
		m_dib = convertedDib;
	}

	//FreeImage_FlipVertical((FIBITMAP *)m_dib);

	reverseRGB = false;
}

void CTextureLoader::CleanUp()
{
	if (m_dib)
	{
		FreeImage_Unload((FIBITMAP *)m_dib);
		m_dib = NULL;
	}
	FreeImage_DeInitialise();
#ifdef VAS_ENCODER
	SAFE_DELETE(waveletEncoder);
#endif
}

unsigned char CTextureLoader::GetChannelCount()const
{
	if (!m_dib)
	{
		return 0;
	}
	
	switch (FreeImage_GetBPP((FIBITMAP *)m_dib))
	{
	case 32:
		return 4;

	case 24:
		return 3;

	case 16:
		return 3;

	case 8:
		return 1;
	}

	return 0;
}

unsigned int CTextureLoader::GetWidth()const
{
	if (!m_dib)
	{
		return 0;
	}

	return FreeImage_GetWidth((FIBITMAP *)m_dib);
}

unsigned int CTextureLoader::GetHeight()const
{
	if (!m_dib)
	{
		return 0;
	}

	return FreeImage_GetHeight((FIBITMAP *)m_dib);
}

void* CTextureLoader::GetData()const
{
	if (!m_dib)
	{
		return NULL;
	}	

	return FreeImage_GetBits((FIBITMAP *)m_dib);
}

unsigned int CTextureLoader::GetDataSize()const
{	
	//return GetWidth()*GetHeight()*GetChannelCount();
	BITMAPINFO* info = FreeImage_GetInfo((FIBITMAP*)m_dib);
	return FreeImage_GetDIBSize((FIBITMAP *)m_dib) - sizeof(*info);
}

bool CTextureLoader::IsDirectChannelOrder()const
{
	return reverseRGB ? (FreeImage_GetRedMask((FIBITMAP *)m_dib) & 0x00FF0000)!=0 : (!(FreeImage_GetRedMask((FIBITMAP *)m_dib) & 0x00FF0000)) != 0;
}

bool CTextureLoader::IsInverseChannelOrder()const
{
	return !IsDirectChannelOrder();
}

CTextureLoader::~CTextureLoader()
{
	CleanUp();
}
#ifdef VAS_ENCODER
bool CTextureLoader::LoadFromMemoryNewCodec(void* pictureData, unsigned int dataSize, CSize pictureSize, bool abEasyPacked)
{
	if (NULL == waveletEncoder)
		return false;
	m_waveletsDIB = waveletEncoder->Decode(pictureData, dataSize, pictureSize.cx, pictureSize.cy, 24, abEasyPacked);
	m_waveletsWidth = pictureSize.cx;
	m_waveletsHeight = pictureSize.cy;
	m_waveletsPictureSize = waveletEncoder->GetDecodedPictureSize();

	if (m_waveletsDIB == NULL)
		return false;
	return true;
}
#endif
unsigned int CTextureLoader::GetWaveletsWidth()const
{
	return m_waveletsWidth;
}
unsigned int CTextureLoader::GetWaveletsHeight()const
{
	return m_waveletsHeight;
}

void* CTextureLoader::GetWaveletsData()const
{
	return m_waveletsDIB;
}
unsigned int CTextureLoader::GetWaveletsDataSize()const
{
	return m_waveletsPictureSize;
}