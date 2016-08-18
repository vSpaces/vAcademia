
#pragma once

class ILoadInfoListener
{
public:
	virtual void On2DTexturesAndCubemapsLoaded(int loadedCount, int allCount) = 0;
	virtual void OnMaterialsLoaded() = 0;
	virtual void OnModelsLoaded() = 0;
	virtual void OnAnimationsLoaded() = 0;
	virtual void OnGUILoaded() = 0;
	virtual void OnTextures3DLoaded() = 0;
	virtual void OnSoundLoaded(int loadedCount, int allCount) = 0;
	virtual void OnLODInfoLoaded() = 0;
	virtual void OnSpritesInfoLoaded() = 0;
	virtual void OnScenes2DLoaded() = 0;
	virtual void OnHeightmapsLoaded() = 0;
	virtual void OnTerrainsLoaded() = 0;
};