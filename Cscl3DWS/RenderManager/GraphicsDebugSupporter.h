
#pragma once

#include "CommonRenderManagerHeader.h"

class CGraphicsDebugSupporter
{
public:
	CGraphicsDebugSupporter();
	~CGraphicsDebugSupporter();

	void SaveObjectVisibilityList();
	void ToogleGroundVisibility();
	void ReloadMasks();

	void EnableTextures();
	void DisableTextures();

	void ShowTexturesList();

	void SaveGoTo();
	void ReplayGoTo();
	void MeasureAllPathesTime();
	 
	void GoToSegment(CVector3D* vBegin, CVector3D* vEnd);

	void CopyCameraToClipboard();
	void PlayCameraTrack();

	void SaveAvatarsTextures();

	void SaveObjectsComparisonPoint();
	void CompareObjectsWithSavedPoint();

	void MakeUrlToClipboard();

private:
	void CreatePassageway(CVector3D* vBegin, CVector3D* vEnd);

	bool m_isGroundVisible;

	MP_MAP<MP_WSTRING, int> m_existingModels;
	MP_MAP<MP_WSTRING, int> m_existingSAO;
};