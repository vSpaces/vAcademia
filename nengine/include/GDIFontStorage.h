#ifndef     __GDIFontStorage__
#define     __GDIFontStorage__

#include <windows.h>
#include <string>

class  GDIFontStorage
{
public:
    GDIFontStorage();
    virtual ~GDIFontStorage();

    HFONT GetFont( const char* face, int height, bool bold, bool italic);
    
protected:
	HFONT FindExistedFont( const char* face, int height, bool bold, bool italic);
	HFONT CreateFont( const char* face, int height, bool bold, bool italic);
	int FindFreeStorageIndex();
	void ReleaseFont( int index);

	#define	MAX_FONTS_IN_STORAGE	20

	typedef struct _FONT_DESC
	{
		std::string face;
		int height;
		bool bold;
		bool italic;
		int refCount;
		HFONT hFont;
	} FONT_DESC;

	FONT_DESC	fonts[MAX_FONTS_IN_STORAGE];
};


#endif // __GDIFontStorage__