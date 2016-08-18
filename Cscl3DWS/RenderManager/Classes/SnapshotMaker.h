
#pragma once

typedef struct _SSnapshotParams
{
	std::string imageName;
	std::string fileName;
	bool toSystemMemory;
	bool isPostEffectEnabled;
	bool isMiniMap;
	bool isNoDynamicObjects;
	bool isNo2D;
	bool isOnly2D;
	bool useBackgroundColor;
	float colorR, colorG, colorB;
	bool isShadowsDisabled;
	bool isTransparentBackground;
	bool isDisableTransparent;

	_SSnapshotParams()
	{
		isPostEffectEnabled = false;
		toSystemMemory = false;
		isMiniMap = false;
		isNoDynamicObjects = false;
		isNo2D = false;
		isOnly2D = false;
		useBackgroundColor = false;
		isShadowsDisabled = false;
		isTransparentBackground = false;
		colorR = 1.0f;
		colorG = 1.0f;
		colorB = 1.0f;
		isDisableTransparent = false;
	}
} SSnapshotParams;

bool MakeSnapshotImpl(mlMedia** appVisibles, CSprite*& sprite, SSnapshotParams& params);