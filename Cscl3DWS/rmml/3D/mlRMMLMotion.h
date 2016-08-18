#if _MSC_VER > 1000
#pragma once
#endif

#ifndef _mlRMMLMotion_H_
#define _mlRMMLMotion_H_

#include "mlRMMLMotionAOs.h"

namespace rmml {

#define EVNM_onSet L"onSet"
#define EVNM_onDone L"onDone"

/**
 * Класс движений 3D-объектов и персонажей RMML
 */

class mlRMMLMotion :	
					public mlRMMLElement,
					public mlJSClass,
					public mlRMMLLoadable,
					public mlIMotion,
					public moIMotion
{
friend class mlRMML3DObject;
public:
	mlRMMLMotion(void);
	void destroy() { MP_DELETE_THIS }
	~mlRMMLMotion(void);
MLJSCLASS_DECL
private:
	enum {
		JSPROP_inverse = 1,
		JSPROP_duration,
		JSPROP_primary,
		JSPROP_tracks,
		EVID_onSet,
		EVID_onDone
	};
	enum {
		EVSM_onSet = (1 << 0),
		EVSM_onDone = (1 << 1)
	};
	bool inverse;
	unsigned int duration;
	bool primary;
	mlMotionTracks* tracks;

	JSFUNC_DECL(moveTo)
	JSFUNC_DECL(rotateTo)

// реализация mlRMMLElement
MLRMMLELEMENT_IMPL
	EventSpec* GetEventSpec2(){ return _EventSpec; } 
	mlresult CreateMedia(omsContext* amcx);
	mlresult Load();
	mlRMMLElement* Duplicate(mlRMMLElement* apNewParent);

// реализация  mlILoadable
MLILOADABLE_IMPL

// реализация  mlIMotion
	mlIMotion* GetIMotion(){ return this; } 
	unsigned int GetDuration(){ return duration; }
	void SetDuration(unsigned int aiDuration);
	bool GetInverse(){ return inverse; }
	bool GetPrimary(){ return primary; }
	// События
	mlRMMLElement* GetElem_mlRMMLMotion(){ return this; }
	void onSet();
	void onDone();

// реализация  moIMotion
	void DurationChanged();
	void InverseChanged();
	void PrimaryChanged();
	mlMedia* GetObject(){ return NULL; }
	ml3DPosition GetFramePos(const wchar_t* apwcTrack, unsigned int auFrame); 
	void SetFramePos(const wchar_t* apwcTrack, unsigned int auFrame, ml3DPosition &pos);
	ml3DScale GetFrameScale(const wchar_t* apwcTrack, unsigned int auFrame);
	void SetFrameScale(const wchar_t* apwcTrack, unsigned int auFrame, ml3DScale &scl);
	ml3DRotation GetFrameRotation(const wchar_t* apwcTrack, unsigned int auFrame);
	void SetFrameRotation(const wchar_t* apwcTrack, unsigned int auFrame, ml3DRotation &rot);
	ml3DPosition GetFramePos(int aiTrack, unsigned int auFrame); 
	void SetFramePos(int aiTrack, unsigned int auFrame, ml3DPosition &pos);
	ml3DScale GetFrameScale(int aiTrack, unsigned int auFrame);
	void SetFrameScale(int aiTrack, unsigned int auFrame, ml3DScale &scl);
	ml3DRotation GetFrameRotation(int aiTrack, unsigned int auFrame);
	void SetFrameRotation(int aiTrack, unsigned int auFrame, ml3DRotation &rot);
	bool moveTo(ml3DPosition& aPos3D);
	bool rotateTo(ml3DRotation& aRot3D);
};

}

#endif
