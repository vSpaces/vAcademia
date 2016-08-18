#pragma once

#include "CommonRenderTargetHeader.h"

#include <vector>
#include <string>
#include "Shader.h"
#include "CommonObject.h"

class CFilter : public CCommonObject
{
public:
	CFilter();
	~CFilter();

	// Must be called in FilterPalette
	bool Load(std::string fileName);
	
	void Bind(unsigned int width, unsigned int height);
	void Unbind();

	void SetSourceImage(int slotID, int textureID);

	int SetFloatArgument(std::string argumentName, float value);
	void SetFloatArgument(int argumentID, float value);

	unsigned int GetFloatID(std::string& name);
	unsigned int GetImageID(std::string& name);

private:
	bool MakeFragmentShader(void* _func);
	bool MakeVertexShader(void* _func);

	unsigned int m_textureSlots[8];
	unsigned int m_lastWidth, m_lastHeight;

	int m_textureStepLocation;

	unsigned char m_usedSlotCount;

	CShader* m_shader;

	MP_STRING m_fragmentShaderSource;
	MP_STRING m_vertexShaderSource;

	MP_MAP<MP_STRING, int> m_floatMap;
	MP_MAP<MP_STRING, int> m_imageMap;

	MP_VECTOR<int> m_floats;
	MP_VECTOR<MP_STRING> m_floatNames;
};