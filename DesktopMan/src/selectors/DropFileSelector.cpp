#include "StdAfx.h"
#include "../../include/selectors/DesktopFrame.h"
#include "../../include/IDesktopMan.h"
#include "../../include/selectors/DropFileSelector.h"

#include "../../Include/TextureLoader.h"

#include <WindowsX.h>

CDropFileSelector::CDropFileSelector(oms::omsContext* apContext): MP_WSTRING_INIT(m_path)
{
	context = apContext;
	currentFrame = NULL;
	m_path = L"";
}

CDropFileSelector::~CDropFileSelector()
{
	ClearCapturedImage();
}

RECT CDropFileSelector::GetCurrentSelectingRect()
{
	void* imgBits = NULL;
	BITMAPINFO bmi;

	FILE* fl = NULL;
	int resCode = _wfopen_s(&fl, m_path.c_str(), L"rb");

	if (fl && resCode==0)
	{
		fseek(fl, 0, 2);
		int size = ftell(fl);

		byte* data;

		if ( size > 4*1024*1024)
		{
			wchar_t wcsLanguage[100] = L"";

			if( context->mpApp != NULL)
				context->mpApp->GetLanguage((wchar_t*)wcsLanguage, 99);

			if (wcscmp(wcsLanguage, L"eng"))
			{
				MessageBox( NULL, "Изображение имеет слишкой большой размер, максимум 4 MБ", "Ошибка", MB_OK);
			}
			else
			{
				MessageBox( NULL, "The image is too large. Size limit for images is 4 MB", "Error", MB_OK);
			}

			fclose(fl);
			CRect rect(0,0,0,0); 
			return rect;
		}

		fseek(fl, 0, 0);
		data = MP_NEW_ARR( byte, size+1);
		memset(data, 0, size);
		data[size] = 0;
		int cnt = fread(data, size, 1, fl);
		fclose(fl);

		CTextureLoader  textureLoader;

		USES_CONVERSION;
		textureLoader.LoadFromMemory( (void*)data, size, W2A(m_path.c_str()));
		int channelCount = textureLoader.GetChannelCount();
		int dataSize = textureLoader.GetWidth() * textureLoader.GetHeight() * channelCount;
		imgBits = MP_NEW_ARR( unsigned char, dataSize);
		memcpy(imgBits, textureLoader.GetData(), dataSize);


		HDC hdc = GetDC(0);
		ZeroMemory(&bmi,sizeof(BITMAPINFO));
		bmi.bmiHeader.biSize=sizeof(BITMAPINFOHEADER);
		bmi.bmiHeader.biHeight=textureLoader.GetHeight();;
		bmi.bmiHeader.biWidth=textureLoader.GetWidth();
		bmi.bmiHeader.biSizeImage=dataSize;

		MP_NEW_V2( currentFrame, CDesktopFrame, imgBits, bmi);

		CRect rect(0,0,bmi.bmiHeader.biHeight,bmi.bmiHeader.biWidth);

		return rect; 
	}
	else
	{
		CRect rect(0,0,0,0); 
		return rect;
	}
	
}

void CDropFileSelector::SetFilePath(std::wstring path)
{
	m_path = path;
}


// возвращает последний полученный кадр
IDesktopFrame* CDropFileSelector::GetCurrentFrame()
{
	return currentFrame;
}

void CDropFileSelector::ClearCapturedImage()
{
	if( currentFrame != NULL)
	{
		MP_DELETE( currentFrame);
		currentFrame = NULL;
	}}