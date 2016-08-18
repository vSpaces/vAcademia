#pragma once


namespace rmml {

/**
 * Класс видео RMML
 */

class mlRMMLVideo :
					public mlRMMLElement,
					public mlJSClass,
					public mlRMMLLoadable,
					public mlRMMLVisible,
					public mlRMMLContinuous,
					public mlRMMLPButton,
					public mlIVideo,
					public moIVideo
{
public:
	mlRMMLVideo(void);
	void destroy() { MP_DELETE_THIS }
	~mlRMMLVideo(void);
MLJSCLASS_DECL
private:
	enum {
		JSPROP_windowed = 1
	};
	bool windowed;
	
// реализация mlRMMLElement
MLRMMLELEMENT_IMPL
	mlresult CreateMedia(omsContext* amcx);
	mlresult Load();
	mlRMMLElement* Duplicate(mlRMMLElement* apNewParent);
	mlresult Update(const __int64 alTime);
	bool Freeze();
	bool Unfreeze();

// реализация  mlILoadable
MLILOADABLE_IMPL

// реализация  mlIVisible
MLIVISIBLE_IMPL

// реализация  mlIContinuous
MLICONTINUOUS_IMPL

// реализация  mlIButton
MLIBUTTON_IMPL

// реализация mlIVideo
public:
	mlIVideo* GetIVideo(){ return this; }
	bool IsWindowed(){ return windowed; }

// реализация moIVideo
public:
	void WindowedChanged();
};

}