
#pragma once

#include "CommonObject.h"
#include "Vector3D.h"
#include "Model.h"

class C3DObject;

#define TYPE_MORPHING						1
#define TYPE_SKELETON_ANIMATION				2

struct IAsyncUpdateListener
{
	virtual void OnAsynchUpdateComplete() = 0;
};

class CAnimationObject : public CCommonObject
{
public:
	CAnimationObject();
	virtual ~CAnimationObject();

	virtual void Update(const int deltaTime, const float distSq) = 0;
	virtual bool Load(const std::string& path, const bool isBlend = false) = 0;
	virtual bool IsUpdated() = 0;
	virtual int GetType()const = 0;
	virtual CVector3D GetOffset()const = 0;
	virtual void GetOffset(float* const x, float* const y, float* const z) = 0;
	virtual void OnPosChanged() = 0;
	virtual void SetFreeze(const bool isFrozen) = 0;
	virtual int GetUpdateTime()const = 0;
	//
	void SetAsyncUpdateListener( IAsyncUpdateListener *aIAsyncUpdateListener)
	{
		m_asyncUpdateListener = aIAsyncUpdateListener;
	}

	__forceinline int CAnimationObject::GetModelID()const
	{
		return m_modelID;
	}

	__forceinline CModel* CAnimationObject::GetModel()const
	{
		return m_model;
	}

	void	SetObject3D(C3DObject* const obj);
	__forceinline C3DObject* GetObject3D()
	{
		return m_obj3d;
	}

protected:
	unsigned int	m_modelID;
	CModel*			m_model;
	C3DObject*		m_obj3d;
	IAsyncUpdateListener *m_asyncUpdateListener;
};