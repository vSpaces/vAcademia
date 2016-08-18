
#pragma once

#define GLEW_STATIC

#include "GLApplication.h"

class CGLApp : public CGLApplication
{
public:
	CGLApp(std::string name, unsigned int width, unsigned int height);

	bool DrawScene();
	void OnLoading();

private:
	void Pack();
	void Unpack();	
};