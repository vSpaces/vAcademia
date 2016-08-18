#include "stdafx.h"
#include "nrm3DSoundManager.h"
#include "nrmViewport.h"
#include "nrmCamera.h"
#include "nrm3DObject.h"
#include <algorithm>
#include "NRManager.h"
#include "MapManager.h"
#include "GlobalSingletonStorage.h"
#include "nrmAudio.h"
#include "SplinePoints.h"

#define PI 3.14159265358979

nrm3DSoundManager::nrm3DSoundManager( CNRManager *pNRManager):
	MP_VECTOR_INIT(m_viewportList),
	MP_VECTOR_INIT(m_3dSounds),
	MP_MAP_INIT(audioParamsMap)
{
	m_iUpdateNeed = 0;
	m_pNRManager = pNRManager;
	TAudioParams *audioNoneParams = MP_NEW(TAudioParams);
	audioParamsMap.insert( std::map<int, TAudioParams*>::value_type( SND_NONE, audioNoneParams));
	TAudioParams *audioBackMusicParams = MP_NEW(TAudioParams);
	audioParamsMap.insert( std::map<int, TAudioParams*>::value_type( SND_BACK_MUSIC, audioBackMusicParams));
	TAudioParams *audioPointParams = MP_NEW(TAudioParams);
	audioParamsMap.insert( std::map<int, TAudioParams*>::value_type( SND_POINT, audioPointParams));
	TAudioParams *audioSplineParams = MP_NEW(TAudioParams);
	audioParamsMap.insert( std::map<int, TAudioParams*>::value_type( SND_SPLINE, audioSplineParams));
}

nrm3DSoundManager::~nrm3DSoundManager()
{
	m_viewportList.clear();	
	m_3dSounds.clear();
	AudioParamsMapIt it = audioParamsMap.begin(),
						itend = audioParamsMap.end();
	for ( ; it != itend; it++)
	{
		TAudioParams *audioParams = it->second;
		MP_DELETE( audioParams);
	}
	audioParamsMap.clear();
}

//////////////////////////////////////////////////////////////////////////

void nrm3DSoundManager::add_registry( nrmViewport *viewport)
{	
	m_viewportList.push_back( viewport);
}

void nrm3DSoundManager::un_registry( nrmViewport* /*viewport*/)
{
	std::vector<nrmViewport*>::iterator	vit = m_viewportList.begin(),
										vend = m_viewportList.end();
	for( ;vit != vend; vit++)
	{
//		nrmViewport *_view = *vit;
/*		if( _view->piviewport == viewport->piviewport)
		{				
			viewportList.erase( vit);			
		}*/
	}	
}

//////////////////////////////////////////////////////////////////////////

float nrm3DSoundManager::getVolumeFromDistance( const float coef, const float minDist, const float maxDist, const float attenunedDistCoef, const float dist)
{		
	if ( dist >= maxDist)
	{
		return 0.0f;
	}
	float attenutedCoef = coef;
	float attenutedDist = dist - minDist;
	if ( attenutedDist >= attenunedDistCoef)
	{
		float count = attenutedDist / attenunedDistCoef;
		while ( count > 1.0f)
		{
			attenutedCoef *= 0.5f;
			attenutedDist -= attenunedDistCoef;
			count--;
		}		
		attenutedCoef *= ( 1.0f - attenutedDist / ( 2.0f * attenunedDistCoef));
	}
	else if ( attenutedDist > 0.0f)
	{
		attenutedCoef *= ( 1.0f - attenutedDist / ( 2.0f * attenunedDistCoef));
	}
	return attenutedCoef;
}

float nrm3DSoundManager::getAngleOnVertical( CVector3D pos, CVector3D avPos, CVector3D dir)
{
	CVector3D avDir = pos - avPos;		
	CVector3D dirXZ = dir;
	CVector3D avDirXZ = avDir;
	dirXZ.y = 0.0f;
	avDirXZ.y = 0.0f;
	float angle = dirXZ.GetAngle( avDirXZ);

	CVector3D y = CVector3D( 0.0f, 1.0f, 0.0f); //dirXZ % avDirXZ;	
	CVector3D dirXYZ = dirXZ + CVector3D( 0.0f, 1.0f, 0.0f);
	CVector3D norm = dirXYZ % y;
	double arg = norm.Dot( avDirXZ) / ( norm.GetLength() * avDirXZ.GetLength());
	if ( arg < 0.0f)
		angle = -angle;
	return angle;
	// вниз - угол положительный
}

float nrm3DSoundManager::getAngleOnHorinz( CVector3D pos, CVector3D avPos, CVector3D dir)
{
	CVector3D avDir = pos - avPos;	
	CVector3D dirXY = dir;
	CVector3D avDirXY = avDir;
	dirXY.z = 0.0f;
	avDirXY.z = 0.0f;
	float angle = dirXY.GetAngle( avDirXY);

	CVector3D z = CVector3D( 0.0f, 0.0f, 1.0f); //dirXY % avDirXY;
	CVector3D dirXYZ = dirXY + CVector3D( 0.0f, 0.0f, 1.0f);
	CVector3D norm = dirXYZ % z;
	double arg = norm.Dot( avDirXY) / ( norm.GetLength() * avDirXY.GetLength());
	if ( arg < 0.0f)
		angle = -angle;
	return angle;
	// вправо - угол положительный
}

//////////////////////////////////////////////////////////////////////////

BOOL nrm3DSoundManager::calc( CVector3D obj_pos, CVector3D obj_dir, ml3DPosition &vp_pos, ml3DPosition &vp_dir, ml3DPosition &cam_dir)
{
	CVector3D pos = obj_pos;	
	CVector3D dir = obj_dir;	

	float max_coef = 0.001f;	
	// определение viewport, по которому надо рассчитывать параметры
	std::vector<nrmViewport*>::iterator	vit = m_viewportList.begin(),
										vend = m_viewportList.end();
	nrmViewport *viewPort = NULL;	
	for ( ;vit != vend; vit++)
	{		
		nrmViewport *_view = *vit;
		if ( _view == NULL)
			break;		
		nrmCamera* camera = _view->GetCamera();
		if ( camera == NULL)
			continue;		
		ml3DPosition ml_cam_dir = camera->GetDest();

		CVector3D obj_pos;
		ml3DPosition ml_cam_pos = camera->GetPos();

		CVector3D cam_pos = CVector3D((float)ml_cam_pos.x, (float)ml_cam_pos.y, (float)ml_cam_pos.z);
		//icollisionobject* my_obj = _view->getMyAvatar();
		//if ( my_obj != NULL)
		//	obj_pos = my_obj->get_translation_absolute();
		//else		
			obj_pos = cam_pos;
		CVector3D dist = pos - obj_pos;
		CVector3D cam_dir__ = CVector3D((float)ml_cam_dir.x, (float)ml_cam_dir.y, (float)ml_cam_dir.z);
		cam_dir__ = cam_dir__ - cam_pos;

		// угол между вектором направлении камеры и вектор, начало которого позиция камеры, а конец другой источник звука
		float angle = getAngleOnHorinz( pos, cam_pos, cam_dir__);
		float angleBetweenDir = cam_dir__.GetAngle( dir);

		float attenutedCoef = getVolumeFromDistance( m_t3dListenerSoundParams._attenutedCoef, m_t3dListenerSoundParams._minDist, 
													m_t3dListenerSoundParams._maxDist, m_t3dListenerSoundParams._attenunedDistCoef, dist.GetLength());

		// угол
		if ( angle > 0.0f && angle <= PI / 2)
		{			
			if ( angleBetweenDir > 0.0f && angleBetweenDir <= PI / 2)
				attenutedCoef *= m_t3dListenerSoundParams._avDir2youCoef;
		}
		else if ( angle > PI / 2 && angle < PI)
		{
			attenutedCoef *= 0.8f;			
			if ( angleBetweenDir > PI / 2 && angleBetweenDir <= PI)
				attenutedCoef *= m_t3dListenerSoundParams._avDir2youCoef;
		}
		else if ( angle < 0.0f && angle >= -PI / 2)
		{			
			if ( angleBetweenDir > 0.0f && angleBetweenDir <= PI / 2)
				attenutedCoef *= m_t3dListenerSoundParams._avDir2youCoef;
		}
		else if ( angle < -PI / 2 && angle > -PI)
		{
			attenutedCoef *= 0.8f;			
			if ( angleBetweenDir > PI / 2 && angleBetweenDir <= PI)
				attenutedCoef *= m_t3dListenerSoundParams._avDir2youCoef;
		}

		if ( max_coef < attenutedCoef)
		{
			max_coef = attenutedCoef;
			viewPort = _view;
		}
	}

	if ( viewPort != NULL)
	{
		CVector3D obj_pos;
		//icollisionobject* my_obj = viewPort->getMyAvatar();
		ml3DPosition ml_cam_pos = viewPort->GetCamera()->GetPos();
		CVector3D cam_pos = CVector3D( (float) ml_cam_pos.x, (float) ml_cam_pos.y, (float) ml_cam_pos.z);
		//if ( my_obj != NULL)
		//	obj_pos = my_obj->get_translation_absolute();
		//else		
			obj_pos = cam_pos;		
		CVector3D dist = pos - obj_pos;		

		ml3DPosition ml_cam_dir = viewPort->GetCamera()->GetDest();

		CVector3D cam_dir__ = CVector3D( (float) ml_cam_dir.x, (float) ml_cam_dir.y, (float) ml_cam_dir.z);
		cam_dir__ -= cam_pos;

		vp_pos.x = dist.x;
		vp_pos.y = dist.y;
		vp_pos.z = dist.z;

		vp_dir.x = dir.x;
		vp_dir.y = dir.y;
		vp_dir.z = dir.z;

		cam_dir.x = cam_dir__.x;
		cam_dir.y = cam_dir__.y;
		cam_dir.z = cam_dir__.z;
		
		return TRUE;
	}
	
	return FALSE;
}

BOOL nrm3DSoundManager::calculate_params( CVector3D pos, CVector3D dir, CVector3D av_pos, CVector3D av_dir, C3DSoundFactor *p3DSoundFactor)
{
	float max_coef = 0.001f;
	float attenutedCoef = 0.0f;

	CVector3D dist = pos - av_pos;

	// коэффициент падения уровня громкости звука
	float leftAttenutedCoef = m_t3dListenerSoundParams._leftAttenutedCoef;
	// коэффициент падения уровня громкости звука
	float rightAttenutedCoef = m_t3dListenerSoundParams._rightAttenutedCoef;

	attenutedCoef = getVolumeFromDistance( p3DSoundFactor->_attenutedCoef, p3DSoundFactor->_minDist,
		p3DSoundFactor->_maxDist, p3DSoundFactor->_attenunedDistCoef, dist.GetLength());

	if ( attenutedCoef < max_coef)
		return FALSE;

	// угол между вектором направлении камеры и вектором, начало которого позиция камеры (или аватара), а конец другой источник звука
	float angle = getAngleOnHorinz( pos, av_pos, av_dir);
	float angleBetweenDir = 0.0;
	if ( dir.x != 0.0f && dir.y != 0.0f && dir.z != 0.0f)
		angleBetweenDir = av_dir.GetAngle( dir);

	float angle_coef = p3DSoundFactor->_angle_coef;

	// угол
	if ( angle > 0.0f && angle <= PI / 2)
	{
		leftAttenutedCoef *= (float) ( 1.0f - 2 * angle / PI);
		angle_coef = (float) ( 0.5 * angle / PI );
		if ( angleBetweenDir > 0.0f && angleBetweenDir <= PI / 2)
			attenutedCoef *= p3DSoundFactor->_avDir2youCoef;
	}
	else if ( angle > PI / 2 && angle < PI)
	{
		attenutedCoef *= 0.8f;
		leftAttenutedCoef *= (float) ( 2 * angle / PI - 1.0f);
		angle_coef += (float) ( 0.5 * ( 0.5f - angle / PI));
		if ( angleBetweenDir > PI / 2 && angleBetweenDir <= PI)
			attenutedCoef *= p3DSoundFactor->_avDir2youCoef;
	}
	else if ( angle < 0.0f && angle >= -PI / 2)
	{
		rightAttenutedCoef *= (float) ( 1.0f + 2 * angle / PI);
		angle_coef -= (float) ( 0.5 * angle / PI);
		if ( angleBetweenDir > 0.0f && angleBetweenDir <= PI / 2)
			attenutedCoef *= p3DSoundFactor->_avDir2youCoef;
	}
	else if ( angle < -PI / 2 && angle > -PI)
	{
		attenutedCoef *= 0.8f;
		rightAttenutedCoef *= (float) ( -2 * angle / PI - 1.0f);
		angle_coef -= (float) ( 0.5 * ( 0.5f + angle / PI));
		if ( angleBetweenDir > PI / 2 && angleBetweenDir <= PI)
			attenutedCoef *= p3DSoundFactor->_avDir2youCoef;
	}	

	p3DSoundFactor->_attenutedCoef = attenutedCoef;
	p3DSoundFactor->_leftAttenutedCoef = leftAttenutedCoef;
	p3DSoundFactor->_rightAttenutedCoef = rightAttenutedCoef;
	return TRUE;
}

BOOL nrm3DSoundManager::calc( double adFactorDist, double adMinDist, CVector3D obj_pos, CVector3D obj_dir, C3DSoundFactor *p3DSoundFactor)
{
	CVector3D pos = obj_pos;	
	CVector3D dir = obj_dir;	

	// определение viewport, по которому надо рассчитывать параметры
	std::vector<nrmViewport*>::iterator	vit = m_viewportList.begin(),
										vend = m_viewportList.end();
	p3DSoundFactor->calc( adFactorDist, adMinDist);

	ATLASSERT( m_pNRManager->GetContext());
	if ( m_pNRManager->GetContext() == NULL)
		return FALSE;

	CMapManager *pMapMan = (CMapManager*) m_pNRManager->GetContext()->mpMapMan;
	ATLASSERT( pMapMan);
	if ( pMapMan == NULL)
		return FALSE;

	C3DObject* pMyAv = pMapMan->GetAvatar();
	ATLASSERT( pMyAv);
	if ( pMyAv == NULL)
		return FALSE;
		
	CVector3D av_dir = CVector3D( 1.0f, 0.0f, 0.0f);
	CRotationPack *rot_pack = pMyAv->GetRotation();
	CQuaternion qrot = rot_pack->GetAsDXQuaternion();	
	av_dir *= qrot;

	CCamera3D *pCamera = g->cm.GetActiveCamera();
	ATLASSERT( pCamera);
	if ( !pCamera)
		return FALSE;
	CVector3D cam_pos = pCamera->GetEyePosition();
	CVector3D cam_dir__ = pCamera->GetLookAt();
	cam_dir__ -= cam_pos;
	av_dir = cam_dir__;	
		
	CVector3D av_pos = pMyAv->GetCoords();

	if ( !calculate_params( pos, dir, av_pos, av_dir, p3DSoundFactor))
		return FALSE;
		
	return TRUE;
}

#define ABS(x) ((x) < 0 ? (-(x)) : (x))      /**< Absolute integer value. */

BOOL nrm3DSoundManager::calc( double adFactorDist, double adMinDist, CSplinePoints *pSplinePoints, C3DSoundFactor *p3DSoundFactor)
{
	// определение viewport, по которому надо рассчитывать параметры
	//std::vector<nrmViewport*>::iterator	vit = m_viewportList.begin(),
	//										vend = m_viewportList.end();
	if ( adFactorDist > 1.0)
		p3DSoundFactor->calc( adFactorDist, adMinDist);

	ATLASSERT( m_pNRManager->GetContext());
	if ( m_pNRManager->GetContext() == NULL)
		return FALSE;

	CMapManager *pMapMan = (CMapManager*) m_pNRManager->GetContext()->mpMapMan;
	ATLASSERT( pMapMan);
	if ( pMapMan == NULL)
		return FALSE;

	C3DObject* pMyAv = pMapMan->GetAvatar();
	//ATLASSERT( pMyAv);
	if ( pMyAv == NULL)
		return FALSE;

	CVector3D av_dir = CVector3D( 1.0f, 0.0f, 0.0f);
	CRotationPack *rot_pack = pMyAv->GetRotation();
	CQuaternion qrot = rot_pack->GetAsDXQuaternion();	
	av_dir *= qrot;

	float attenutedCoef = 0.0f;

	// коэффициент падения уровня громкости звука
	float leftAttenutedCoef = m_t3dListenerSoundParams._leftAttenutedCoef;
	// коэффициент падения уровня громкости звука
	float rightAttenutedCoef = m_t3dListenerSoundParams._rightAttenutedCoef;

	CCamera3D *pCamera = g->cm.GetActiveCamera();
	ATLASSERT( pCamera);
	if ( !pCamera)
		return FALSE;
	CVector3D cam_pos = pCamera->GetEyePosition();
	CVector3D cam_dir__ = pCamera->GetLookAt();
	cam_dir__ -= cam_pos;
	av_dir = cam_dir__;	
	
	CVector3D av_pos = pMyAv->GetCoords();
	av_pos.z = 0;

	CVector3D point_dir;

	// определяем сколько точек спалайна попадает в радиус слышимости
	BoundaryPoints *boundaryPoints = pSplinePoints->getBoundaryPoints( av_pos, p3DSoundFactor->_maxDist);

	if ( !boundaryPoints->bSmallRegion)
	{
		if ( boundaryPoints->boundarySplinePoints.size() == 0)
		{
			return FALSE;
		}

		if ( boundaryPoints->boundarySplinePoints.size() == 1)
		{
			// всего одна точка в радиусе слышимости
			CVector3D dir = CVector3D( 0.0f, 0.0f, 0.0f);
			if ( !calculate_params( boundaryPoints->minPoint, dir, av_pos, av_dir, p3DSoundFactor))
				return FALSE;
			return TRUE;
		}

		CVector3D minPoint2;
		CSplinePoints::m_splinePointsIter it = boundaryPoints->boundarySplinePoints.begin(),
											itend = boundaryPoints->boundarySplinePoints.end();

		if ( boundaryPoints->boundarySplinePoints.size() > 2)
		{
			float angle1 = getAngleOnHorinz( boundaryPoints->minPoint, av_pos, av_dir);		

			for ( ; it != itend; it++)
			{
				CVector3D point = *it;
				float angle2 = getAngleOnHorinz( point, av_pos, av_dir);
				if ( angle2 + angle1 < ABS( angle1) + ABS( angle2))
				{
					if ( minPoint2.GetLengthSq() == 0)
						minPoint2 = point;
					else if ( minPoint2.GetLengthSq() > point.GetLengthSq() && minPoint2 != boundaryPoints->minPoint)
						minPoint2 = point;
				}
			}
			if ( minPoint2 == boundaryPoints->minPoint || minPoint2.GetLengthSq() == 0)
			{
				it = boundaryPoints->boundarySplinePoints.begin();
				for ( ; it != itend; it++)
				{
					CVector3D point = *it;
					if ( minPoint2.GetLengthSq() == 0)
						minPoint2 = point;
					else if ( minPoint2.GetLengthSq() > point.GetLengthSq())
						minPoint2 = point;
				}
			}
		}
		else
		{
			// всего две точки в радиусе слышимости
			minPoint2 = *it;
			if ( minPoint2 == boundaryPoints->minPoint)
			{
				it++;
				minPoint2 = *it;
			}
		}
		
		BOOL bSucc1 = TRUE;
		BOOL bSucc2 = TRUE;		
		
		CVector3D dir = CVector3D( 0.0f, 0.0f, 0.0f);
		C3DSoundFactor t3DSoundFactor1 = *p3DSoundFactor;
		// расчет параметров звуков для первой точек
		bSucc1 = calculate_params( boundaryPoints->minPoint, dir, av_pos, av_dir, &t3DSoundFactor1);		

		if ( !bSucc1 && !bSucc2)
			return FALSE;

		if ( !bSucc2)
		{		
			p3DSoundFactor->_attenutedCoef = t3DSoundFactor1._attenutedCoef;
			p3DSoundFactor->_leftAttenutedCoef = t3DSoundFactor1._leftAttenutedCoef;
			p3DSoundFactor->_rightAttenutedCoef = t3DSoundFactor1._rightAttenutedCoef;
			return TRUE;
		}
				
		C3DSoundFactor t3DSoundFactor2 = *p3DSoundFactor;
		// расчет параметров звуков для второй точек
		bSucc2 = calculate_params( minPoint2, dir, av_pos, av_dir, &t3DSoundFactor2);

		if ( !bSucc1 && !bSucc2)
			return FALSE;

		if ( !bSucc1)
		{		
			p3DSoundFactor->_attenutedCoef = t3DSoundFactor2._attenutedCoef;
			p3DSoundFactor->_leftAttenutedCoef = t3DSoundFactor2._leftAttenutedCoef;
			p3DSoundFactor->_rightAttenutedCoef = t3DSoundFactor2._rightAttenutedCoef;
			return TRUE;
		}

		if ( !bSucc2)
		{		
			p3DSoundFactor->_attenutedCoef = t3DSoundFactor1._attenutedCoef;
			p3DSoundFactor->_leftAttenutedCoef = t3DSoundFactor1._leftAttenutedCoef;
			p3DSoundFactor->_rightAttenutedCoef = t3DSoundFactor1._rightAttenutedCoef;
			return TRUE;
		}

		p3DSoundFactor->_attenutedCoef = __max( t3DSoundFactor1._attenutedCoef, t3DSoundFactor2._attenutedCoef);
		float coef = (t3DSoundFactor1._attenutedCoef + t3DSoundFactor2._attenutedCoef) / p3DSoundFactor->_attenutedCoef;

		float left1 = t3DSoundFactor1._leftAttenutedCoef;
		float left2 = t3DSoundFactor2._leftAttenutedCoef;
		float right1 = t3DSoundFactor1._rightAttenutedCoef;
		float right2 = t3DSoundFactor2._rightAttenutedCoef;

		p3DSoundFactor->_leftAttenutedCoef = ( left1 * t3DSoundFactor1._attenutedCoef + left2 * t3DSoundFactor2._attenutedCoef) * coef;
		if ( p3DSoundFactor->_leftAttenutedCoef > 1.0f)
			p3DSoundFactor->_leftAttenutedCoef = 1.0f;
		p3DSoundFactor->_rightAttenutedCoef = ( right1 * t3DSoundFactor1._attenutedCoef + right2 * t3DSoundFactor2._attenutedCoef) * coef;
		if ( p3DSoundFactor->_rightAttenutedCoef > 1.0f)
			p3DSoundFactor->_rightAttenutedCoef = 1.0f;
	}
	else
	{

	}

	return TRUE;
}

nrm3DSoundManager::nrmAudioVectorIter nrm3DSoundManager::FindSound(nrmAudio* sound)
{
	nrmAudioVectorIter iter = find(m_3dSounds.begin(), m_3dSounds.end(), sound);	
	return iter;
}

void nrm3DSoundManager::UpdateSound(nrmAudio* sound)
{	
	nrmAudioVectorIter	vit = FindSound(sound),
							vend = m_3dSounds.end();
	if (vit == vend)
	{
		m_3dSounds.push_back( sound);
	}
}

void nrm3DSoundManager::RemoveSound(nrmAudio* sound)
{
	nrmAudioVectorIter	vit = FindSound( sound),
								vend = m_3dSounds.end();
	if ( vit != vend)
	{
		m_3dSounds.erase( vit);
	}	
}

void nrm3DSoundManager::update()
{
	//if ( m_iUpdateNeed < 2)
	//{
	//	m_iUpdateNeed++;
	//	return;
	//}

	nrmAudioVectorIter	vit = m_3dSounds.begin(),
						vend = m_3dSounds.end();
	for( ; vit != vend; vit++)
	{				
		nrmAudio *sound = *vit;
		if (sound)
		{
			sound->UpdateParams();
		}
	}
	m_iUpdateNeed = 0;
}

void nrm3DSoundManager::SetAudioMode(const wchar_t* sType, int iMode)
{	
	nrmAudioVectorIter	vit = m_3dSounds.begin(),
						vend = m_3dSounds.end();

	int iType = GetAudioType( sType);

	if ( ( iType & SND_BACK_MUSIC) != 0)
	{
		AudioParamsMapIt it = audioParamsMap.find( SND_BACK_MUSIC);
		if ( it != audioParamsMap.end())
			it->second->iMode = iMode;
	}
	if ( ( iType & SND_POINT) != 0)
	{
		AudioParamsMapIt it = audioParamsMap.find( SND_POINT);
		if ( it != audioParamsMap.end())
			it->second->iMode = iMode;
	}
	if ( ( iType & SND_SPLINE) != 0)
	{
		AudioParamsMapIt it = audioParamsMap.find( SND_SPLINE);
		if ( it != audioParamsMap.end())
			it->second->iMode = iMode;
	}
	
	for( ; vit != vend; vit++)
	{
		nrmAudio *sound = *vit;
		
		if ( sound)
		{
			if ( ( sound->GetEmitterType() & iType) != 0)
				sound->SetAudioMode( iMode);
		}
	}
}

void nrm3DSoundManager::SetAudioVolume(const wchar_t* sType, float fVolume)
{	
	nrmAudioVectorIter	vit = m_3dSounds.begin(),
						vend = m_3dSounds.end();

	int iType = GetAudioType( sType);

	if ( ( iType & SND_BACK_MUSIC) != 0)
	{
		AudioParamsMapIt it = audioParamsMap.find( SND_BACK_MUSIC);
		if ( it != audioParamsMap.end())
			it->second->fVolume = fVolume;
	}
	if ( ( iType & SND_POINT) != 0)
	{
		AudioParamsMapIt it = audioParamsMap.find( SND_POINT);
		if ( it != audioParamsMap.end())
			it->second->fVolume = fVolume;
	}
	if ( ( iType & SND_SPLINE) != 0)
	{
		AudioParamsMapIt it = audioParamsMap.find( SND_SPLINE);
		if ( it != audioParamsMap.end())
			it->second->fVolume = fVolume;
	}

	for( ; vit != vend; vit++)
	{
		nrmAudio *sound = *vit;

		if ( sound)
		{
			if ( ( sound->GetEmitterType() & iType) != 0)
				sound->SetAudioVolume( fVolume);
		}
	}
}

inline int string_to_type( std::string sType)
{
	if ( strcmpi( sType.c_str(), "background_music") == 0)
	{
		return SND_BACK_MUSIC;
	}
	if ( strcmpi( sType.c_str(), "spline") == 0)
	{
		return SND_SPLINE;
	}
	if ( strcmpi( sType.c_str(), "point") == 0)
	{
		return SND_POINT;
	}

	// по умолчанию обычный 2D-звук
	return SND_NONE;	
}

int nrm3DSoundManager::GetAudioType(const wchar_t* sType)
{
	USES_CONVERSION;
	std::string s = W2A(sType);
	int pos = s.find( "|");
	if ( pos == -1)
	{
		return string_to_type( s);
	}

	int iType = SND_NONE;

	while ( pos > -1)
	{
		std::string spos1 = s.substr( 0, pos);
		iType |= string_to_type( spos1);
		s = s.substr( pos + 1, s.length() - pos);		
		pos = s.find( '|');		
	}
	iType |= string_to_type( s);
	return iType;
}

int nrm3DSoundManager::GetAudioMode( int iType)
{
	AudioParamsMapIt it = audioParamsMap.find( iType);
	if ( it != audioParamsMap.end())
		return it->second->iMode;
	return 1;
}

float nrm3DSoundManager::GetAudioVolume( int iType)
{
	AudioParamsMapIt it = audioParamsMap.find( iType);
	if ( it != audioParamsMap.end())
		return it->second->fVolume;
	return 1.0f;
}

void nrm3DSoundManager::SetPlayingEnabled( int iType, bool bEnabled)
{
	nrmAudioVectorIter	vit = m_3dSounds.begin(),
						vend = m_3dSounds.end();

	//int iType = GetAudioType( sType);

	if ( ( iType & SND_BACK_MUSIC) != 0)
	{
		AudioParamsMapIt it = audioParamsMap.find( SND_BACK_MUSIC);
		if ( it != audioParamsMap.end())
			it->second->bPlayingEnabled = bEnabled;
	}
	if ( ( iType & SND_POINT) != 0)
	{
		AudioParamsMapIt it = audioParamsMap.find( SND_POINT);
		if ( it != audioParamsMap.end())
			it->second->bPlayingEnabled = bEnabled;
	}
	if ( ( iType & SND_SPLINE) != 0)
	{
		AudioParamsMapIt it = audioParamsMap.find( SND_SPLINE);
		if ( it != audioParamsMap.end())
			it->second->bPlayingEnabled = bEnabled;
	}

	for( ; vit != vend; vit++)
	{
		nrmAudio *sound = *vit;

		if ( sound)
		{
			if ( ( sound->GetEmitterType() & iType) != 0)
				sound->SetPlayingEnabled( bEnabled);
		}
	}
}

bool nrm3DSoundManager::GetPlayingEnabled( int iType)
{
	AudioParamsMapIt it = audioParamsMap.find( iType);
	if ( it != audioParamsMap.end())
		return it->second->bPlayingEnabled;
	return 1;
}

#define MAX_NORM_DIST	10.0f
#define MAX_DIST		5000.0f

bool nrm3DSoundManager::calculatePosition( voip::IVoipCharacter* pMyIVoipCharacter, voip::IVoipCharacter* pIVoipCharacter, ml3DPosition &userPos)
{
	ml3DPosition pos = pIVoipCharacter->getAbsolutePosition();
	ml3DRotation rot = pIVoipCharacter->getAbsoluteRotation();

	ml3DPosition myPos = pMyIVoipCharacter->getAbsolutePosition();
	ml3DRotation myRot = pMyIVoipCharacter->getAbsoluteRotation();

	CVector3D vpos = CVector3D((float)pos.x, (float)pos.y, (float)pos.z);
	CQuaternion vrot = CQuaternion((float)rot.x, (float)rot.y, (float)rot.z, (float)rot.a);
	CVector3D vmyPos = CVector3D((float)myPos.x, (float)myPos.y, (float)myPos.z);
	CQuaternion vmyRot = CQuaternion((float)myRot.x, (float)myRot.y, (float)myRot.z, (float)myRot.a);
		
	CVector3D av_dir = CVector3D( 1.0f, 0.0f, 0.0f);
	CVector3D pos_vec = vpos - vmyPos;
	float dist = pos_vec.GetLength();
	float coef = MAX_NORM_DIST / MAX_DIST;
	
	CCamera3D *pCamera = g->cm.GetActiveCamera();
	ATLASSERT( pCamera);
	if ( !pCamera)
		return FALSE;
	CVector3D cam_pos = pCamera->GetEyePosition();	
	CVector3D cam_dir__ = pCamera->GetLookAt();
	cam_dir__ -= cam_pos;
	av_dir = cam_dir__;	

	float angleH = getAngleOnHorinz( vpos, vmyPos, av_dir);
	float angleV = getAngleOnVertical( vpos, vmyPos, av_dir);

	float cos_angleH = cos( angleH);
	float cos_angleV = cos( angleV);
	float sin_angleH = sin( angleH);
	float sin_angleV = sin( angleV);

	userPos.y = coef * dist * cos_angleH * abs( cos_angleV);
	userPos.x = coef * dist * sin_angleH * abs( cos_angleV);
	userPos.z = coef * dist * abs( cos_angleH) * -sin_angleV;

	return TRUE;
}