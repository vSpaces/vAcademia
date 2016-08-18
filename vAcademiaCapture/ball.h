
// { fd501041-8ebe-11ce-8183-00aa00577da1 }
DEFINE_GUID(CLSID_vAcademiaCapture,
0xfd641041, 0x8eba, 0x1ace, 0x91, 0x86, 0x01, 0xba, 0x10, 0x56, 0x8d, 0xa3);

class CVAcademiaCapture
{
public:
    CVAcademiaCapture(HWND hWnd, int iImageWidth = 320, int iImageHeight = 240);
	~CVAcademiaCapture();

    int GetImageWidth() { return m_iImageWidth ;}
    int GetImageHeight() { return m_iImageHeight ;}

	bool GetImage(unsigned char* data, unsigned int size);

private:
	bool CreateGlobalMemoryObjectIfNeeded(unsigned int size);
	void DestroyGlobalMemoryObjectIfNeeded();
	unsigned char* GetGlobalMemoryArrayPointer(unsigned int size);
	void GetAllSizes(unsigned int& width, unsigned int& height, unsigned int& sizeRGB, unsigned int& sizeRGBA);
	void ConvertRGBToRGBA(unsigned char* destData, unsigned int destSize,
							 unsigned char* srcData, unsigned int srcSize);
	void FreeGlobalMemoryArrayPointer(unsigned char* arrayPointer);

    int m_iImageHeight;     // The image height
    int m_iImageWidth;      // The image width
      
	HWND m_hWnd;

	HANDLE m_globalMemoryObject;
}; 
