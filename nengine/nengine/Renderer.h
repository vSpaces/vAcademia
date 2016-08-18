
#pragma once

#define MODE_RENDER_UNKNOWN		0
#define MODE_RENDER_SOLID		1
#define MODE_RENDER_TRANSPARENT	2
#define MODE_RENDER_ALPHATEST	4
#define MODE_RENDER_ALL			8

#define MODE_INDEX_SOLID					0
#define MODE_INDEX_TRANSPARENT				1
#define MODE_INDEX_ALPHATEST				2
#define MODE_INDEX_ALL						3
#define MODE_INDEX_SOLID_TRANSPARENT		4		
#define MODE_INDEX_TRANSPARENT_ALPHATEST	5
#define MODE_INDEX_SOLID_ALPHATEST			6

#define MODE_INDEX_COUNT					7

class IRenderer
{
public:
	IRenderer(void);
	~IRenderer(void);

	virtual void SetMode(int mode) = 0;

	virtual void RenderModel(int num, unsigned short* fs, float* vx, float* tx, 
		float* normals, int* textureNumbers, int fsCount, int primType) = 0;

	virtual void RenderModel(int num, unsigned short* fs, float* vx, int* txLevelPointers, 
		int txLevelCount, float* normals, int* textureNumbers, int fsCount, int primType) = 0;
};