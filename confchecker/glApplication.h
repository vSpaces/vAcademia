
#pragma once

#include <string>

class CGLApplication
{
public:
	CGLApplication(std::string name, unsigned int width, unsigned int height, bool isVSyncEnabled);
	~CGLApplication();

	int WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow);

	virtual bool DrawScene() = 0;
	virtual void OnLoading() = 0;

	HDC GetHDC()const;
	HWND GetHWND()const;
	HGLRC GetHGLRC()const;

private:
	bool SetupPixelFormat();
	void InitGL();
	
	HDC   m_hdc; 
	HWND  m_hwnd;
	HGLRC m_glRC;

	std::string m_name;
	bool m_isVSyncEnabled;
	unsigned int m_width;
	unsigned int m_height;
};