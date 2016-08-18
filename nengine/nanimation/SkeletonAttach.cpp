
#include "StdAfx.h"
#include "SkeletonAttach.h"
#include "Math3D.h"

CSkeletonAttach::CSkeletonAttach( C3DObject *aParent, C3DObject *aChild, const char* alpcBoneName) 
	: CBaseAttachParams( aParent, aChild)
	, MP_STRING_INIT( m_boneName)
	, m_pBone( NULL)
	, m_subscribedToUpdates( false)
	, m_boneEvaluated( false)
{
	if( alpcBoneName != NULL)
		m_boneName = alpcBoneName;
}

CSkeletonAttach::~CSkeletonAttach()
{
	if( m_subscribedToUpdates)
	{
		if( GetParent()->GetAnimation())
		{
			GetParent()->GetAnimation()->SetAsyncUpdateListener( NULL);
		}
		m_subscribedToUpdates = false;
	}
}

void CSkeletonAttach::CalculateChildCoords( CVector3D& aPosition)
{
	CVector3D		bonePositionAbsolute;
	CQuaternion		boneRotationAbsolute;

	GetBoneTransformAbsolute( bonePositionAbsolute, boneRotationAbsolute);
	aPosition = Math3D::ObjectToWorld( GetChild()->GetLocalCoords(), bonePositionAbsolute, boneRotationAbsolute);
}

void CSkeletonAttach::CalculateChildRotation( CQuaternion& aRotation)
{
	CVector3D		bonePositionAbsolute;
	CQuaternion		boneRotationAbsolute;

	GetBoneTransformAbsolute( bonePositionAbsolute, boneRotationAbsolute);
	aRotation = Math3D::ObjectToWorld( GetChild()->GetLocalRotation()->GetAsDXQuaternion(), boneRotationAbsolute);
}

CVector3D	CSkeletonAttach::PositionAbsoluteToChild( const CVector3D& aPosition)
{
	CVector3D		bonePositionAbsolute;
	CQuaternion		boneRotationAbsolute;

	GetBoneTransformAbsolute( bonePositionAbsolute, boneRotationAbsolute);
	return Math3D::WorldToObject( aPosition, bonePositionAbsolute, boneRotationAbsolute);
}

CQuaternion CSkeletonAttach::RotationAbsoluteToChild( const CQuaternion& aRotation)
{
	CVector3D		bonePositionAbsolute;
	CQuaternion		boneRotationAbsolute;

	GetBoneTransformAbsolute( bonePositionAbsolute, boneRotationAbsolute);
	return Math3D::WorldToObject( aRotation, boneRotationAbsolute);
}

bool	CSkeletonAttach::GetBoneTransformAbsolute( CVector3D& aPosition, CQuaternion& aRotation)
{
	if( m_boneEvaluated)
	{
		aPosition = m_bonePositionAbsolute;
		aRotation = m_boneRotationAbsolute;
	}
	else
	{
		if( !m_subscribedToUpdates)
		{
			if( GetParent()->GetAnimation())
			{
				GetParent()->GetAnimation()->SetAsyncUpdateListener( this);
				m_subscribedToUpdates = true;
			}
		}
	}
	return m_boneEvaluated;
}

void CSkeletonAttach::OnAsynchUpdateComplete( )
{
	m_boneEvaluated = false;

	if( !m_subscribedToUpdates)
	{
		// Parent object doesn`t has animation object
		return;
	}

	CSkeletonAnimationObject* pAnimationObject = (CSkeletonAnimationObject*)GetParent()->GetAnimation();
	if( !pAnimationObject)
		return;

	CalModel* pCalModel = pAnimationObject->GetCalModel();
	if( !pCalModel)
		return;

	CalSkeleton* pSkeleton = pCalModel->getSkeleton();
	if( !pSkeleton)
		return;

	CalBone* pBone = pSkeleton->getBone( m_boneName.c_str());
	if( !pBone)
		return;

	CalVector		bonePosition = pBone->getTranslationAbsolute();
	CalQuaternion	boneRotation = pBone->getRotationAbsolute();

	CVector3D		bonePositionNe( bonePosition.x, bonePosition.y, bonePosition.z);
	CQuaternion		boneRotationNe( boneRotation.x, boneRotation.y, boneRotation.z, boneRotation.w);

	CRotationPack*	parentRotationAbsolute = GetParent()->GetRotation();

	m_bonePositionAbsolute = Math3D::ObjectToWorld( bonePositionNe
										, GetParent()->GetCoords()
										, parentRotationAbsolute->GetAsDXQuaternion());

	m_boneRotationAbsolute = Math3D::ObjectToWorld( boneRotationNe
										, parentRotationAbsolute->GetAsDXQuaternion());

	m_boneEvaluated = true;
}