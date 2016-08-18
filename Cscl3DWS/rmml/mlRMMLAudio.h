#if _MSC_VER > 1000
#pragma once
#endif

#ifndef _mlRMMLAudio_H_
#define _mlRMMLAudio_H_

namespace rmml {

/**
 * Класс звука RMML
 */

class mlRMMLAudio :	
					public mlRMMLElement,
					public mlJSClass,
					public mlRMMLLoadable,
					public mlRMMLContinuous,
					public mlIAudible,
					public moIAudible
{
public:
	mlRMMLAudio(void);
	void destroy() { MP_DELETE_THIS }
	~mlRMMLAudio(void);
MLJSCLASS_DECL

private:
	enum{
		JSPROP_emitterType = 1, // тип источника
		JSPROP_splineSrc, //	путь к xml-файла, содержащего сплайн
		JSPROP_volume, //	громкость звука
		JSPROP_location //	локация, в которой играется звук (для 3D-объектов)
	};

	JSString *m_emitterType;
	JSString *m_splineSrc;
	int m_volume;
	JSString *m_locationID;
	MP_WSTRING msSplineSrcFull;

public:

// реализация mlRMMLElement
MLRMMLELEMENT_IMPL
	mlresult CreateMedia(omsContext* amcx);
	mlresult Load();
	mlRMMLElement* Duplicate(mlRMMLElement* apNewParent);
	bool Freeze();
	bool Unfreeze();
	
// реализация  mlILoadable
MLILOADABLE_IMPL

// реализация  mlIContinuous
MLICONTINUOUS_IMPL
	unsigned long GetCurPos();
	unsigned long GetRMCurPos();
	void PosChanged();

// реализация  mlIAudible
MLIAUDIBLE_IMPL
	bool EmitterTypeChanged();
	bool SplineSrcChanged();
	bool VolumeChanged();
	bool LocationChanged();
	const wchar_t* GetEmitterType();
	const wchar_t* GetSplineSrc();
	const int GetVolume();
	const wchar_t* GetLocation();
};

}

#endif
