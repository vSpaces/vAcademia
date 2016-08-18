
#pragma once

#include "GlobalInterfaceStorage.h"
#include "Polygons2DAccum.h"
#include "VBOObject.h"
#include "Color3.h"
#include "IOculusRiftSupport.h"

class CSpriteProcessor/* : public ISpriteProcessor*/
{
friend class CGlobalSingletonStorage;

public:
	// put sprite without alpha-blending 
	void PutNormalSprite(int x, int y, int texX1, int texY1, int texX2, int texY2, std::string fileName, CColor3& color, int angle=0, bool tiling=false);
	// put sprite without alpha-blending  
	void PutNormalSprite(int x, int y, int texX1, int texY1, int texX2, int texY2, int fl, CColor3& color, int angle=0, bool tiling=false);

	// put sprite without alpha-blending 
	void PutNormalSprite(int x, int y, int x2, int y2, int texX1, int texY1, int texX2, int texY2, std::string fileName, CColor3& color, int angle=0, bool tiling=false);
	// put sprite without alpha-blending 
	void PutNormalSprite(int x, int y, int x2, int y2, int texX1, int texY1, int texX2, int texY2, int fl, CColor3& color, int angle=0, bool tiling=false);

	// USEFUL
	void PutSprite(int x, int y, int x2, int y2, int texX1, int texY1, int texX2, int texY2, int textureID, CColor3& color, CPolygons2DAccum* accum, int angle = 0, int tiling = false, bool is_transparent = false, unsigned char alpha = 255);

	//void PutNormalSpriteWithRotation(int x, int y, int texX1, int texY1, int texX2, int texY2, int fl, int angle);
	//void PutNormalSpriteWithRotation(int x, int y, int texX1, int texY1, int texX2, int texY2, std::string fileName, int angle);

	// put sprite with alpha-blending
	void PutAlphaSprite(int x, int y, int texX1, int texY1, int texX2, int texY2, int fl, unsigned char sprAlpha, CColor3& color, int angle=0, bool tiling=false);
	//
	// put sprite with alpha-blending
	void PutAlphaSprite(int x, int y, int texX1, int texY1, int texX2, int texY2, std::string fileName, BYTE sprAlpha, CColor3& color, int angle=0, bool tiling=false);

	// put sprite with alpha-blending
	void PutAlphaSprite(int x, int y, int x2, int y2, int texX1, int texY1, int texX2, int texY2, int fl, unsigned char sprAlpha, CColor3& color, int angle=0, bool tiling=false);
	// put sprite with alpha-blending
	void PutAlphaSprite(int x, int y, int x2, int y2, int texX1, int texY1, int texX2, int texY2, std::string fileName, BYTE sprAlpha, CColor3& color, int angle=0, bool tiling=false);


	void PutAlphaSprite(int x, int y, int x2, int y2, int texX1, int texY1, int texX2, int texY2, int textureID, unsigned char alpha, CColor3& color, CPolygons2DAccum* accum, int angle=0, bool tiling=false);

	void PutNormalSprite(int x, int y, int texX1, int texY1, int texX2, int texY2, int fl, CColor3& color, CPolygons2DAccum* accum, int angle=0, bool tiling=false);
	void PutAlphaSprite(int x, int y, int texX1, int texY1, int texX2, int texY2, int fl, unsigned char alpha, CColor3& color, CPolygons2DAccum* accum, int angle=0, bool tiling=false);

	//void PutAlphaSpriteWithRotation(int x, int y, int texX1, int texY1, int texX2, int texY2, int fl, unsigned char sprAlpha, float angle);
	//void PutAlphaSpriteWithRotation(int x, int y, int texX1, int texY1, int texX2, int texY2, std::string fileName, unsigned char sprAlpha, float angle);

	// put colored sprite
	void PutOneColorQuad(float x1, float y1, float x2, float y2, unsigned char r, unsigned char g, unsigned char b, unsigned char a);
	void PutOneColorQuad(int x, int y, int xx, int yy, BYTE r, BYTE _g, BYTE b, BYTE a, CPolygons2DAccum* accum);

	void PutSplittedAlphaSprite(int x, int y, int texX1, int texY1, int texX2, int texY2, int fl, BYTE sprAlpha, int blockSize);
	void PutSplittedAlphaSprite(int x, int y, int texX1, int texY1, int texX2, int texY2, std::string name, BYTE sprAlpha, int blockSize);

	void PutAddSprite(int x, int y, int _x2, int _y2, int texX1, int texY1, int texX2, int texY2, int fl, BYTE alpha, CColor3& color);

	CBaseBufferObject* MakeSplittedSpriteInVBO(int width, int height, int blockSize);
	
	void CreateOculusDistortionMesh(IOculusRiftSupport::DistortionMesh* mesh, CShader* shader);
	void PutOculusDistortionMesh(int texId);

	// get OpenGL (float) x for given screen x
	float GetOpenGLX(int x)const;
	// get OpenGL (float) y for given screen y
	float GetOpenGLY(int y)const;

	

private:

	CSpriteProcessor();
	CSpriteProcessor(const CSpriteProcessor&);
	CSpriteProcessor& operator=(const CSpriteProcessor&);
	~CSpriteProcessor();

	float m_sinArray[361];
	float m_cosArray[361];
	
	IBaseBufferObject* m_distortionMesh;
};
