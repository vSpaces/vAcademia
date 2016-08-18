
#pragma once

#include "CommonEngineHeader.h"

#include "IPlugin.h"
#include "AnimationObject.h"
#include "CommonObject.h"
#include "RotationPack.h"
#include "Vector3D.h"
#include "Color4.h"
#include "IAttachParams.h"
#include <vector>

class CLODGroup;
class CPhysicsObject;

class C3DObject : public CCommonObject, public CBaseChangesInformer, public IChangesListener
{
public:
	C3DObject();
	~C3DObject();
	
	void SetZOffset(const float zOffset);
	__forceinline float GetZOffset()const
	{
		return m_zOffset;
	}

	void AttachTo(C3DObject* const parent, const char* alpcBoneName);

	void AttachTo(C3DObject* const parent);
	__forceinline C3DObject* GetParent()const
	{
		return m_parent;
	}

	void SetPhysics(const bool physics, CModel* const model = NULL);
	bool IsPhysicsObject()const;
	int GetPhysicsObjID()const;

	void SetLODGroup(CLODGroup* const lodGroup);
	__forceinline CLODGroup* GetLODGroup()const
	{
		return m_lodGroup;
	}

	void SetCoords(const float x, const float y, const float z);
	void SetCoords(const CVector3D& coords);
	void GetCoords(float* const x, float* const y, float* const z) const;
	void GetLocalCoords(float* const x, float* const y, float* const z) const;
	CVector3D GetLocalCoords() const;
	CVector3D GetCoords() const;

	void Move(const float deltaX, const float deltaY, const float deltaZ);
	
	void GetCenterCoords(float* const x, float* const y, float* const z);

	void GetScale(float* const scaleX, float* const scaleY, float* const scaleZ)const;
	float GetScale()const;
	CVector3D GetScale3D()const;
	
	void SetScale(const float scaleX, const float scaleY, const float scaleZ);
	void SetScale(const CVector3D& scale);
	
	void SetRotation(CRotationPack* const rotation);
	CRotationPack* GetRotation();
	CRotationPack* GetLocalRotation();

	void SetVisible(const bool isVisible);

	__forceinline bool IsVisible()const
	{
		return m_isVisible;
	}

	__forceinline bool IsInCameraSpace() const
	{
		return m_isInCameraSpace;
	}

	__forceinline void SetVisibleNow(const bool isVisibleNow)
	{
		m_isVisibleNow = ((isVisibleNow) && (m_isVisible));
	}

	void SetInCameraSpace(bool isInCameraSpace);

	__forceinline bool IsVisibleNow()const
	{
		if (m_parent)
		{
			return ((m_parent->IsVisible()) && (m_isVisibleNow));
		}

		return m_isVisibleNow;
	}

	__forceinline float GetDistSq()const
	{
		return m_distSq;
	}

	void SetLODNumber(const unsigned char lodNum);
	__forceinline unsigned char GetLODNumber()const
	{
		return m_lodNum;
	}

	void SetAnimation(CAnimationObject* const animation);
	__forceinline CAnimationObject* GetAnimation()const
	{
		return m_animation;
	}

	void SetLocalCoords(const float x, const float y, const float z);

	void SetBoundingBox(const float x, const float y, const float z, const float sizeX, const float sizeY, 
		const float sizeZ);
	void GetOOBoundingBox(CVector3D& origin, CVector3D*& ooCorners);
	void GetBoundingBox(float& minX, float& minY, float& minZ, float& maxX, float& maxY, float& maxZ)const;
	void GetInitialAABB(float& minX, float& minY, float& minZ, float& maxX, float& maxY, float& maxZ)const;
	bool IsInBoundingBox(const float x, const float y, const float z);
	bool IsInBoundingBox(const float x, const float y, const float z, const float eps);
	bool IsBoundingBoxInFrustum();
	bool IsShadowBoundingBoxInFrustum();

	float CalculateDistSq();

	void SetPlugin(IPlugin* const plugin);
	__forceinline IPlugin* GetPlugin()const
	{
		return m_plugin;
	}

	void SetFreeze(const bool isFrozen);
	__forceinline bool IsFrozen()const 
	{
		return m_isFrozen;
	}

	void SetControlledObjectStatus(const bool isControlled);
	__forceinline bool IsControlledObject()const
	{
		return m_isControlled;
	}

	void FreeResources();

	void OnChanged(int eventID);

	void SetBoundingBoxVisible(const bool isVisible);
	__forceinline bool IsBoundingBoxVisible()const 
	{
		return m_isBoundingBoxVisible;
	}

	void SetBoundingBoxColor(const unsigned char r, const unsigned char g, const unsigned char b, const unsigned char a);
	CColor4* GetBoundingBoxColor()const;

	CVector3D GetSize()const;

	__forceinline float GetHeight()const
	{
		return m_realSizeZ;
	}

	void SetPivotObject(C3DObject* const group);

	__forceinline void SetNeedSwitchOffPathes(bool const isNeeded)
	{
		m_isNeededToSwitchOffPathes = isNeeded;
	}

	__forceinline bool IsNeedToSwitchOffPathes()const
	{
		return m_isNeededToSwitchOffPathes;
	}

	void SetNeedRenderShadowsStatus(const bool isNeeded);
	__forceinline bool IsNeededToRenderShadows()const
	{
		return m_isNeededToRenderShadows;
	}

	void SetLODDistanceCanBeChangedStatus(const bool isLODDistanceCanBeChanged);
	__forceinline bool IsLODDistanceCanBeChanged()const
	{
		return m_isLODDistanceCanBeChanged;
	}

	__forceinline void SaveVisibility()
	{
		m_savedVisible = m_isVisible;
	}

	__forceinline void RestoreVisibility()
	{
		m_isVisible = m_savedVisible;
	}

	__forceinline void GetSizes(float& sizeX, float& sizeY, float& sizeZ)
	{
		sizeX = m_sizeX;
		sizeY = m_sizeY;
		sizeZ = m_sizeZ;
	}

	bool IsCollidedWithOtherObjects();	

private:
	void SetPhysicsImpl(const bool physics, CModel* const model = NULL);
	void SetPhysicsFromModel(CModel* const model);
	void DetachChild(C3DObject* attachedChild);
	void AttachToImpl(C3DObject* const parent, CBaseAttachParams*	apAttachController, IAsyncUpdateListener* apIAsyncUpdateListener);
	float GetDistanceToPoint(float x, float y, float z) const;

	float m_x, m_y, m_z;
	float m_absX, m_absY, m_absZ;
	float m_scaleX, m_scaleY, m_scaleZ;	

	float m_zOffset;
	float m_realSizeZ;

	float m_ooSizeX;
	float m_ooSizeY;
	float m_ooSizeZ;

	bool m_isControlled;

	IPlugin* m_plugin;

	CLODGroup* m_lodGroup;

	CRotationPack m_rotation;
	CRotationPack* m_absRotation;

	CAnimationObject* m_animation;

	C3DObject*			m_parent;
	CBaseAttachParams*	m_attachController;

	CVector3D m_ooCorners[8];

	bool m_isDynamic;
	bool m_isVisible;
	bool m_isInCameraSpace;
	bool m_savedVisible;
	bool m_isVisibleNow;
	bool m_physics;
	bool m_isNeededToSwitchOffPathes;

	float m_distSq;

	unsigned char m_lodNum;
	unsigned char m_live;
	unsigned char m_livePhazeNum;

	MP_VECTOR<CPhysicsObject *> m_physicsObjects;

	float m_centerX, m_centerY, m_centerZ;
	float m_sizeX, m_sizeY, m_sizeZ;
	
	CColor4 m_bbColor;
	bool m_isFrozen;
	bool m_isBoundingBoxSet;
	bool m_isBoundingBoxVisible;
	bool m_isNeededToRenderShadows;
	bool m_isLODDistanceCanBeChanged;

	C3DObject* m_group;

	CVector3D m_rotatedCenter;
};