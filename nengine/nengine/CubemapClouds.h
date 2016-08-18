
#pragma once

#include "Vector3D.h"
#include "Clouds.h"
#include <string>

#define CUBEMAP_FACE_COUNT	6

#define CUBEMAP_MODE		1
#define SIX_TEXTURES_MODE	2

class CCubemapClouds : public IClouds
{
public:
	CCubemapClouds();
	~CCubemapClouds();

	void Destroy();

	void SetColor(unsigned char r, unsigned char g, unsigned char b, unsigned char a);

	void SetFov(int fov);
	int GetFov()const;

	void SetCubemapTextures(std::string leftTextureName, std::string frontTextureName, 
		std::string rightTextureName, std::string backTextureName, 
		std::string topTextureName, std::string bottomTextureName);

	void SetCubemap(std::string cubemapName);

	void Draw();

private:
	void RenderCubemap();
	void RenderSixTextures();

	void SetTexture(int orderID, std::string name);

	void DrawBackFace(CVector3D& size);
	void DrawFrontFace(CVector3D& size);
	void DrawLeftFace(CVector3D& size);
	void DrawRightFace(CVector3D& size);
	void DrawTopFace(CVector3D& size);
	void DrawBottomFace(CVector3D& size);

	int m_fov;
	int m_mode;

	int m_cubemapID;
	int m_textureIDs[CUBEMAP_FACE_COUNT];

	unsigned char m_r, m_g, m_b, m_a;
};