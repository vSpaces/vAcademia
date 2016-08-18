
#pragma once

#define GLEW_STATIC

#include "GLApplication.h"

class CGLApp : public CGLApplication
{
public:
	CGLApp(std::string name, unsigned int width, unsigned int height);

	bool DrawScene();
	void OnLoading();

	void SetLangID(int langID);

private:
	int m_langID;
	std::string m_strings[10][2];
};
