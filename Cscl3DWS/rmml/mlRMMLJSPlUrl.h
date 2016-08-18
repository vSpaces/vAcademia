#ifndef _mlRMMLJSPlUrl_h__
#define _mlRMMLJSPlUrl_h__

namespace rmml{

#define URL_BUFFERS_SIZE	255
struct UrlProperties
{
	wchar_t	wcLocation[URL_BUFFERS_SIZE];
	wchar_t	wcPlace[URL_BUFFERS_SIZE];
	wchar_t	wcAction[URL_BUFFERS_SIZE];
	wchar_t	wcActionParam[URL_BUFFERS_SIZE];
	float	x;
	float	y;
	float	z;
	unsigned long zoneID;

	UrlProperties()
	{
		memset( &wcLocation, 0, sizeof(wcLocation));
		memset( &wcLocation, 0, sizeof(wcPlace));
		memset( &wcLocation, 0, sizeof(wcAction));
		memset( &wcLocation, 0, sizeof(wcActionParam));
		x = 0;
		y = 0;
		z = 0 ;
		zoneID = 0;
	}
};

}

#endif