//****************************************************************************//
// coreanimation.h                                                            //
// Copyright (C) 2001, 2002 Bruno 'Beosil' Heidelberger                       //
//****************************************************************************//
// This library is free software; you can redistribute it and/or modify it    //
// under the terms of the GNU Lesser General Public License as published by   //
// the Free Software Foundation; either version 2.1 of the License, or (at    //
// your option) any later version.                                            //
//****************************************************************************//

#ifndef CAL_COREANIMATION_H
#define CAL_COREANIMATION_H

//****************************************************************************//
// Includes                                                                   //
//****************************************************************************//

#include "global.h"
#include "vector.h"

//****************************************************************************//
// Forward declarations                                                       //
//****************************************************************************//

class CalCoreTrack;
class CalSkeleton;
class CalCoreSkeleton;
class CalCoreKeyframe;
struct CalAnimationCallback;

//****************************************************************************//
// Class declaration                                                          //
//****************************************************************************//

 /*****************************************************************************/
/** The core animation class.
  *****************************************************************************/
typedef std::pair<CalCoreTrack *, CalCoreTrack *> TRACKPAIR;

class CAL3D_API CalCoreAnimation
{
// member variables
protected:
  float m_duration;
  MP_LIST<CalCoreTrack *> m_listCoreTrack;

// constructors/destructor
public:
  CalCoreAnimation();
	virtual ~CalCoreAnimation();

// member functions	
public:
	virtual	void initialize(CalSkeleton* skeleton);
	float getStayedBipHeight(CalCoreSkeleton* m_pCoreSkeleton);
	void getFlagData(DWORD flag, LPVOID* ppdata);
	void setFlagData(DWORD flag, LPVOID pdata);
	void KeepFrame();
	bool IsCommon();
	CalCoreKeyframe* getBegPointer();
	void addTrackKeyframe(DWORD id, CalVector trn, CalQuaternion rtn, float time);
	void addCoreTrack(DWORD id);	
	void setLock(const std::string& strName);
	std::string getName()const;
	float getKeepLastTime();
	void setKeepLastTime(DWORD ms, float rnd);
	bool containBoneFrom(CalCoreAnimation* pCheckAnim, std::list<TRACKPAIR>*	tracksout = NULL);
	void saveLState(bool bSave);
	void saveFState(bool bSave);	
	void setRelMode( bool bRelMode);
	bool addCoreTrack(CalCoreTrack *pCoreTrack);
	bool removeCoreTrack(DWORD coreBoneId);
	bool create();
	void destroy();
	CalCoreTrack *getCoreTrack(int coreBoneId);
	CalCoreTrack *getCoreTrack(const char* trackName);

	// Длительности, получаемые из файла
	float getDuration();
	void setDuration(float duration);

	std::list<CalCoreTrack *>& getListCoreTrack();
	bool FallOnSkel(CalCoreSkeleton *pCoreSkel);
	void setFlags(DWORD val){	dwFlags = val;}
	void setFlag(DWORD val){	dwFlags |= val;}
	void removeFlag(DWORD val){	dwFlags &= ~val;}
	DWORD getFlags(){	return dwFlags;}

	int RemoveFakeFrames( );
	void setBVH(bool isBVH);
	bool isBVH();

	struct CallbackRecord
	{
		CalAnimationCallback *callback;
		float                 min_interval;
	};
	std::vector<CallbackRecord>& getCallbackList() { return m_listCallbacks; }
	void registerCallback(CalAnimationCallback *callback,float min_interval);
	void removeCallback(CalAnimationCallback *callback);
  
	unsigned int		m_version;
	bool		m_bRelative;
	bool		m_bSaveFirst;
	bool		m_bSaveLast;
	DWORD		m_dwKeepLast;
	float		m_fKeepRnd;
	MP_STRING	m_strLock;

private:
	MP_VECTOR<CallbackRecord> m_listCallbacks;
	//CRITICAL_SECTION	cs;
	DWORD				dwFlags;
	float				frameMoveLength;
	bool				m_isBVH;
};

#endif

//****************************************************************************//
