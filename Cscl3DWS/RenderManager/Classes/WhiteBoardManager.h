
#pragma once

#include "../CommonRenderManagerHeader.h"

#include "nrmImage.h"
#include "MapManager.h"

class CWhiteBoardManager
{
public:
	CWhiteBoardManager();
	~CWhiteBoardManager();

	bool CreateWhiteBoard(C3DObject* obj, nrmImage* img, int& textureID, int auID);
	void OnDeleteWhiteboard(nrmImage* img);

	unsigned int GetHideDistance()const;

	void SetMapManager(CMapManager* mapManager);

	void PrintDebugInfo();

private:
	void DeleteWBImage(nrmImage* img);
	void AddWBImage(nrmImage* img);

	void DetectWhiteBoardSizeIfNeeded();

	int m_count;

	bool m_isInited;

	unsigned int m_scaleKoef;
	unsigned int m_hideDistance;

	MP_VECTOR<nrmImage *> m_wbImages;
};