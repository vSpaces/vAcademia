
// { fd501041-8ebe-11ce-8183-00aa00577da1 }
DEFINE_GUID(CLSID_vAcademiaAudioCapture,
0xfd641041, 0x8eba, 0x1ace, 0x91, 0x86, 0x01, 0xba, 0x10, 0x56, 0x8d, 0xa4);

class CVAcademiaCapture
{
public:
    CVAcademiaCapture(HWND hWnd);
	~CVAcademiaCapture();

private:
	bool CreateGlobalMemoryObjectIfNeeded(unsigned int size);
	void DestroyGlobalMemoryObjectIfNeeded();
	unsigned char* GetGlobalMemoryArrayPointer(unsigned int size);
	
	void FreeGlobalMemoryArrayPointer(unsigned char* arrayPointer);

	HWND m_hWnd;

	HANDLE m_globalMemoryObject;
}; 
