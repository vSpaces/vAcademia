#pragma once

#include "../CommonRenderManagerHeader.h"

#include <vector3d.h>
#include "mutex.h"
#include "3DSoundFactor.h"
#include "VoipManClient.h"

class nrmViewport;
class nrm3DObject;
class nrmAudio;
class CNRManager;
class CSplinePoints;

struct T3DListenerSoundParams
{
	// коэффициент падения уровня громкости звука
	float _attenutedCoef;
	// коэффициент падения уровня громкости звука
	float _leftAttenutedCoef;
	// коэффициент падения уровня громкости звука
	float _rightAttenutedCoef;
	// минимальное расстояние, от 0 до minDist амплитуда звука не меняется
	float _minDist;
	// расстояние
	float _maxDist;
	// расстояние, на которое амплитуда звука падает в два раза
	float _attenunedDistCoef;
	// коэффициент падения уровня громкости звука, если аватара расположены спиной друг другу
	float _avDir2youCoef;
	// угловой коэффиицент для снижения резкости звука между "панами" наушниками
	float _angle_coef;

	T3DListenerSoundParams()
	{
		_attenutedCoef = 1.0f;
		_leftAttenutedCoef = 1.0f;
		_rightAttenutedCoef = 1.0f;
		_minDist = 300.0f;
		_maxDist = 3000.0f;
		_attenunedDistCoef = 300.0f;
		_avDir2youCoef = 0.7f;
		_angle_coef = 0.4f;
	}
};

struct TAudioParams
{
	bool bPlayingEnabled;
	int iMode;
	float fVolume;

	TAudioParams()
	{
		bPlayingEnabled = true;
		iMode = 1;
		fVolume = 1.0f;
	}
};

class nrm3DSoundManager : public voip::IVoip3DSoundCalculator
{
public:
	nrm3DSoundManager( CNRManager *pNRManager);
	virtual ~nrm3DSoundManager();

protected:
	typedef MP_VECTOR<nrmAudio *> nrmAudioVector;
	typedef MP_VECTOR<nrmAudio *>::iterator nrmAudioVectorIter;

public:
	void add_registry( nrmViewport *viewport);
	void un_registry( nrmViewport *viewport);

protected:
	__forceinline float getVolumeFromDistance( const float coef, const float minDist, const float maxDist, const float attenunedDistCoef, const float dist);
	__forceinline float getAngleOnVertical( CVector3D avPos, CVector3D pos, CVector3D dir);
	__forceinline float getAngleOnHorinz( CVector3D avPos, CVector3D pos, CVector3D dir);
	__forceinline BOOL calculate_params( CVector3D pos, CVector3D dir, CVector3D av_pos, CVector3D av_dir, C3DSoundFactor *p3DSoundFactor);

public:
	// рассчитываем параметры 3D звука
	// на входе: позиция и направление объекта
	// на выходе: позиция и направление объекта в системе координат viewport
	BOOL calc( CVector3D obj_pos, CVector3D obj_dir, ml3DPosition &vp_pos, ml3DPosition &vp_dir, ml3DPosition &cam_dir);
	BOOL calc( double adFactorDist, double adMinDist, CVector3D obj_pos, CVector3D obj_dir, C3DSoundFactor *p3DSoundFactor);
	BOOL calc( double adFactorDist, double adMinDist, CSplinePoints *pSplinePoints, C3DSoundFactor *p3DSoundFactor);

public:
	nrmAudioVectorIter FindSound(nrmAudio* sound);
	void UpdateSound(nrmAudio* sound);
	void RemoveSound(nrmAudio* sound);
	void update();
	void SetAudioMode(const wchar_t* sType, int iMode);
	void SetAudioVolume(const wchar_t* sType, float fVolume);
	int GetAudioType(const wchar_t* sType);
	int GetAudioMode( int iType);
	float GetAudioVolume( int iType);
	void SetPlayingEnabled( int iType, bool bEnabled);
	bool GetPlayingEnabled( int iType);

protected:
	MP_VECTOR<nrmViewport*> m_viewportList;
	T3DListenerSoundParams m_t3dListenerSoundParams;	
	nrmAudioVector m_3dSounds;
	CMutex m_mutexUpdate3DObjects;
	int m_iUpdateNeed;
	CNRManager *m_pNRManager;

private:
	typedef MP_MAP<int, TAudioParams*> AudioParamsMap;
	typedef MP_MAP<int, TAudioParams*>::iterator AudioParamsMapIt;
	AudioParamsMap audioParamsMap;

	// реализация IVoipSoundManager
public:
	bool calculatePosition( voip::IVoipCharacter* pMyIVoipCharacter, voip::IVoipCharacter* pIVoipCharacter, ml3DPosition &userPos);
};
