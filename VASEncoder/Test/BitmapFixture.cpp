#include ".\bitmapfixture.h"

CBitmapFixture::CBitmapFixture(LPWSTR alpwFileName)
{
	dimensions.cx = 0;
	dimensions.cy = 0;
	bpp = 0;
	lpBits = NULL;

	IPicture* pIPicture = NULL;
	CoInitialize( NULL);
	OleLoadPicturePath( alpwFileName, NULL, 0, 0, IID_IPicture, (void**)&pIPicture);
	if( pIPicture)
	{
		HDC hdc;
		HANDLE hBitmap = 0;
		pIPicture->get_Handle( (OLE_HANDLE*)&hBitmap);
		pIPicture->get_Width( (OLE_YSIZE_HIMETRIC*)&dimensions.cx);
		pIPicture->get_Height( (OLE_XSIZE_HIMETRIC*)&dimensions.cy);

		CClientDC dc(NULL);
		dc.HIMETRICtoLP(&dimensions);

		pIPicture->get_CurDC( &hdc);

		if( hBitmap != 0)
		{
			hdc=GetDC(NULL);
			BITMAPINFO bmpInfo;
			ZeroMemory(&bmpInfo,sizeof(BITMAPINFO));
			bmpInfo.bmiHeader.biSize=sizeof(BITMAPINFOHEADER);
			GetDIBits(hdc,(HBITMAP)hBitmap,0,0,NULL,&bmpInfo,DIB_RGB_COLORS); 
			if(bmpInfo.bmiHeader.biSizeImage<=0)
				bmpInfo.bmiHeader.biSizeImage=bmpInfo.bmiHeader.biWidth*abs(bmpInfo.bmiHeader.biHeight)*(bmpInfo.bmiHeader.biBitCount+7)/8;

			lpBits = malloc(bmpInfo.bmiHeader.biSizeImage);
			bmpInfo.bmiHeader.biCompression=BI_RGB;
			GetDIBits(hdc,(HBITMAP)hBitmap,0,bmpInfo.bmiHeader.biHeight,lpBits, &bmpInfo, DIB_RGB_COLORS);       

			bpp = bmpInfo.bmiHeader.biBitCount;
		}
		pIPicture->Release();
	}
	CoUninitialize();
}

CBitmapFixture::~CBitmapFixture(void)
{
}
