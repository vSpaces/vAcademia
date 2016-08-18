#pragma once

#include "CommonEngineHeader.h"
#include "IAttachParams.h"
#include "AnimationObject.h"

class CSkeletonAttach : public CBaseAttachParams
						, public IAsyncUpdateListener
{
public:
	CSkeletonAttach( C3DObject *aParent, C3DObject *aChild, const char* alpcBoneName);
	virtual ~CSkeletonAttach();

	CVector3D	PositionAbsoluteToChild( const CVector3D& aPosition);

	CQuaternion RotationAbsoluteToChild( const CQuaternion& aRotation);

	void OnAsynchUpdateComplete();

private:
	void CalculateChildCoords( CVector3D& aPosition);
	void CalculateChildRotation( CQuaternion& aRotation);

	bool GetBoneTransformAbsolute( CVector3D& aPosition, CQuaternion& aRotation);

	MP_STRING	m_boneName;
	CalBone		*m_pBone;
	CVector3D	m_bonePositionAbsolute;
	CQuaternion m_boneRotationAbsolute;
	bool		m_boneEvaluated;
	bool		m_subscribedToUpdates;
};