
#include "StdAfx.h"
#include <math.h>
#include <float.h>
#include <assert.h>
#include "Consts.h"
#include "LODGroup.h"
#include "3DObject.h"
#include "userdata.h"
#include "PhysicsObject.h"
#include "TrimeshConstructData.h"
#include "GlobalSingletonStorage.h"
#include "SkeletonAnimationObject.h"
#include "HierarchyAttach.h"
#include "SkeletonAttach.h"

C3DObject::C3DObject():
	m_isNeededToSwitchOffPathes(false),
	m_isLODDistanceCanBeChanged(true),
	m_isNeededToRenderShadows(false),
	MP_VECTOR_INIT(m_physicsObjects),
	m_bbColor(255, 255, 255, 255),	
	m_isBoundingBoxVisible(false),
	m_isBoundingBoxSet(false),	
	m_isControlled(false),
	m_absRotation(NULL),	
	m_isDynamic(false), 
	m_realSizeZ(0.0f),
	m_animation(NULL),
	m_isVisible(true),
	m_isInCameraSpace(false),
	m_livePhazeNum(0),
	m_isFrozen(false),
	m_lodGroup(NULL),
	m_physics(false),
	m_zOffset(0.0f),
	m_centerX(0.0f),
	m_centerY(0.0f),
	m_centerZ(0.0f),
	m_plugin(NULL),
	m_parent(NULL),
	m_scaleX(1.0f),
	m_scaleY(1.0f),
	m_scaleZ(1.0f),
	m_distSq(0.0f),
	m_sizeX(0.0f),
	m_sizeY(0.0f),
	m_sizeZ(0.0f),
	m_lodNum(0),
	m_live(0),
	m_x(0.0f),
	m_y(0.0f),
	m_z(0.0f),
	m_group(NULL),
	m_attachController(NULL)
{
	m_rotatedCenter.Set(0.0f, 0.0f, 0.0f);
}

//
// Расстояние до объекта
//

float C3DObject::CalculateDistSq()
{
	float minX = m_centerX - m_sizeX;
	float minY = m_centerY - m_sizeY;
	float maxX = m_centerX + m_sizeX;
	float maxY = m_centerY + m_sizeY;

	GetCoords(&m_absX, &m_absY, &m_absZ);

	if (m_group)
	{
		float groupX, groupY, groupZ;
		m_group->GetCoords(&groupX, &groupY, &groupZ);
		m_distSq = GetDistanceToPoint(groupX, groupY, groupZ);
		return m_distSq;
	}
    	
	m_distSq = GetDistanceToPoint(m_absX, m_absY, m_absZ);

	float newDistSq = GetDistanceToPoint(m_absX + minX, m_absY + minY, m_absZ);
	if (newDistSq < m_distSq)
	{
		m_distSq = newDistSq;
	}
    	
	newDistSq = GetDistanceToPoint(m_absX + maxX, m_absY + minY, m_absZ);
	if (newDistSq < m_distSq)
	{
		m_distSq = newDistSq;
	}

	newDistSq = GetDistanceToPoint(m_absX + maxX, m_absY + minY, m_absZ);
	if (newDistSq < m_distSq)
	{
		m_distSq = newDistSq;
	}

	newDistSq = GetDistanceToPoint(m_absX + minX, m_absY + maxY, m_absZ);
	if (newDistSq < m_distSq)
	{
		m_distSq = newDistSq;
	}

	newDistSq = GetDistanceToPoint(m_absX + maxX, m_absY + maxY, m_absZ);
	if (newDistSq < m_distSq)
	{
		m_distSq = newDistSq;
	}

	/*float x, y, z;
	(*iter)->GetCoords(&x, &y, &z);

	float minX, minY, minZ, maxX, maxY, maxZ;
			(*iter)->GetBoundingBox(minX, minY, minZ, maxX, maxY, maxZ);
			minX += x;
			maxX += x;
			minY += y;
			maxY += y;
			minZ += z;
			maxZ += z;*/

	const float koef = 1.4f;

	bool isCameraInObject = g->cm.IsCameraInObject(minX * koef + m_absX, minY * koef + m_absY, maxX * koef + m_absX, maxY * koef + m_absY);

	if (isCameraInObject)
	{
		m_distSq = 1;
	}

	return m_distSq;
}

float C3DObject::GetDistanceToPoint(float x, float y, float z) const
{
	if (m_isInCameraSpace) {
		return x*x + y*y + z*z;
	} else {
		return g->cm.GetPointDistSq(x, y, z);
	}
}

//
// Координаты объекта
//

void C3DObject::Move(const float deltaX, const float deltaY, const float deltaZ)
{
	if (!m_isControlled)
	{
		float x, y, z;
		GetCoords(&x, &y, &z);
		SetCoords(x + deltaX, y + deltaY, z + deltaZ);
	}
	else if (m_physicsObjects.size() > 0)
	{		
		m_physicsObjects[0]->SetVelocityToMove(deltaX, deltaY, deltaZ);
	}
}

// Устанавливает локальные координаты объекта
void C3DObject::SetLocalCoords(const float x, const float y, const float z)
{
	/*
	if (m_physics)
	{
	if (m_parent)
	{
	float parentX = 0.0f, parentY = 0.0f, parentZ = 0.0f;
	m_parent->GetCoords(&parentX, &parentY, &parentZ);
	m_physicsObjects[0]->SetCoords(x + parentX, y + parentY, z + parentZ + m_isBoundingBoxSet * m_sizeZ);			
	}
	else
	{
	m_physicsObjects[0]->SetCoords(x, y, z + m_isBoundingBoxSet * m_sizeZ);
	}		
	}
	else
	{
	m_x = x;
	m_y = y;
	m_z = z;
	}
	*/

	if (m_physics)
	{
		if( m_parent)
		{
			m_x = x;
			m_y = y;
			m_z = z;

			CVector3D physicsCoords = GetCoords( );
			m_physicsObjects[0]->SetCoords( physicsCoords.x, physicsCoords.y, physicsCoords.z);
		}
		else
		{
			m_physicsObjects[0]->SetCoords(x, y, z + m_isBoundingBoxSet * m_sizeZ);
		}
	}
	else
	{
		m_x = x;
		m_y = y;
		m_z = z;
	}

	if (m_lodGroup)
	{
		m_lodGroup->NeedToUpdate();
	}
}

// Устанавливает глобальные координаты объекта.
// При наличии родителя, объект должен все равно оказаться в указанных координатах.
void C3DObject::SetCoords(const float x, const float y, const float z)
{
	/*m_x = x;
	m_y = y;
	m_z = z;

	if (m_parent)
	{
#pragma warning(push)
#pragma warning(disable : 4239)
		CVector3D& parentCoords = m_parent->GetCoords();
#pragma warning(pop)

		m_x -= parentCoords.x;
		m_y -= parentCoords.y;
		m_z -= parentCoords.z;

		assert( false);	// не понял где у нас вызываются установка абсолютных координат при наличии парента
	}*/

	if (m_parent)
	{
		CVector3D absCoords( x, y, z);
		CVector3D childCoords = m_attachController->PositionAbsoluteToChild( absCoords);
		SetLocalCoords( childCoords.x, childCoords.y, childCoords.z);		
	}
	else
	{
		SetLocalCoords( x, y, z);
	}
}

// см.
// void C3DObject::SetCoords(const float x, const float y, const float z)
void C3DObject::SetCoords(const CVector3D& coords)
{
	SetCoords(coords.x, coords.y, coords.z);
}


CVector3D C3DObject::GetLocalCoords() const
{
	CVector3D result;
	GetLocalCoords(&result.x, &result.y, &result.z);

	return result;
}

// Должна вернуть локальные координаты объекта
void C3DObject::GetLocalCoords(float* const x, float* const y, float* const z) const
{
	/*if ((!m_physics) || (!m_parent))
	{
		if (m_physics)
		{
			m_physicsObjects[0]->GetCoords(x, y, z);
			*z -= m_isBoundingBoxSet * m_sizeZ;
		}
		else
		{
			*x = m_x;
			*y = m_y;
			*z = m_z;
		}
	}
	else
	{
		GetCoords(x, y, z);
		float parentX, parentY, parentZ;
		m_parent->GetCoords(&parentX, &parentY, &parentZ);
		(*x) = (*x) - parentX;
		(*y) = (*y) - parentY;
		(*z) = (*z) - parentZ;
	}*/

	if( m_physics)
	{
		if( m_parent)
		{
			*x = m_x;
			*y = m_y;
			*z = m_z;
		}
		else
		{
			m_physicsObjects[0]->GetCoords(x, y, z);
			*z -= m_isBoundingBoxSet * m_sizeZ;
		}

		if (GetAnimation())
		{
			*x += GetAnimation()->GetOffset().x;
			*y += GetAnimation()->GetOffset().y;
			*z -= GetAnimation()->GetOffset().z;
		}

		return;
	}

	*x = m_x;
	*y = m_y;
	*z = m_z;
}

// Должна вернуть абсолютные координаты объекта
void C3DObject::GetCoords(float* const x, float* const y, float* const z) const
{
	/*if (m_parent)
	{
		m_parent->GetCoords(x, y, z);
	}

	if (m_physics)
	{
		m_physicsObjects[0]->GetCoords(x, y, z);
		*z -= m_isBoundingBoxSet * m_sizeZ;

		if (GetAnimation())
		{
			*x += GetAnimation()->GetOffset().x;
			*y += GetAnimation()->GetOffset().y;
		}
	}
	else
	{
		*x = m_x;
		*y = m_y;	
		*z = m_z;
	}

	if ((m_parent) && (!m_physics))
	{
		float parentX, parentY, parentZ;
		m_parent->GetCoords(&parentX, &parentY, &parentZ);
		*x = *x + parentX;
		*y = *y + parentY;
		*z = *z + parentZ;
	}*/

	// Абсолютные координаты физики хранятся в ней самой
	// Если есть иерархия, вернем позицию с учетом родителя
	if ( m_parent)
	{
		CVector3D positionAbsolute = m_attachController->GetAbsChildCoords();
		*x = positionAbsolute.x;
		*y = positionAbsolute.y;	
		*z = positionAbsolute.z;
		return;
	}

	// Если нет иерарзии просто вернем координаты объекта
	GetLocalCoords( x, y, z);
}

// Должна вернуть абсолютные координаты объекта
CVector3D C3DObject::GetCoords()const
{
	CVector3D result;
	GetCoords(&result.x, &result.y, &result.z);

	return result;
}

//
// Масштаб и размер
//

void C3DObject::SetScale(const float scaleX, const float scaleY, const float scaleZ)
{
	m_scaleX = scaleX;
	m_scaleY = scaleY;
	m_scaleZ = scaleZ;

	if (m_lodGroup)
	{
		m_lodGroup->OnChanged(0);
	}
}

void C3DObject::SetScale(const CVector3D& scale)
{
	SetScale(scale.x, scale.y, scale.z);	
}

float C3DObject::GetScale()const
{
	return m_scaleX;
}

void C3DObject::GetScale(float* const scaleX, float* const scaleY, float* const scaleZ)const
{
	assert(scaleX);
	assert(scaleY);
	assert(scaleZ);

	/*if (m_parent)
	{
		m_parent->GetScale(scaleX, scaleY, scaleZ);

		(*scaleX) *= m_scaleX;
		(*scaleY) *= m_scaleY;
		(*scaleZ) *= m_scaleZ;

		return;
	}*/

	*scaleX = m_scaleX;
	*scaleY = m_scaleY;
	*scaleZ = m_scaleZ;
}

CVector3D C3DObject::GetScale3D()const
{
	CVector3D result;
	GetScale(&result.x, &result.y, &result.z);

	return result;
}

CVector3D C3DObject::GetSize()const
{
	CVector3D scale = GetScale3D();

	return CVector3D(m_sizeX * scale.x * 2.0f, m_sizeY * scale.y * 2.0f, m_sizeZ * scale.z * 2.0f);
}

//
// Поворот
//

void C3DObject::SetRotation(CRotationPack* const rotation)
{
	if (rotation == NULL)
	{
		m_rotation.Clear();
				
		return;
	}

	if ((m_physics) && (m_isDynamic))
	{
		CQuaternion quat = rotation->GetAsDXQuaternion();
		m_physicsObjects[0]->GetRotationPack()->SetAsDXQuaternion(quat.x, quat.y, quat.z, quat.w);
		rotation->GetRotating(&quat.x, &quat.y, &quat.z, &quat.w);
		m_physicsObjects[0]->SetRotation(quat.x, quat.y, quat.z, quat.w);
	}
	else
	{
#pragma warning(push)
#pragma warning(disable : 4239)
		CQuaternion& quat = rotation->GetAsDXQuaternion();
#pragma warning(pop)
		m_rotation.SetAsDXQuaternion(quat.x, quat.y, quat.z, quat.w);

		if (GetLODGroup())
		{
			GetLODGroup()->UpdateBoundingBox(this);
		}
	}
}

CRotationPack* C3DObject::GetLocalRotation()
{
	if (m_isDynamic)
	{
		if (m_physicsObjects.size() > 0)
		{
			return m_physicsObjects[0]->GetRotationPack();
		}
		else
		{			
			return &m_rotation;
		}
	}

	return &m_rotation;
}

CRotationPack* C3DObject::GetRotation()
{
	if (m_parent)
	{
		/*CQuaternion parentRotation;
		m_parent->GetRotation()->GetAsDXQuaternion(&parentRotation.x, &parentRotation.y, &parentRotation.z, &parentRotation.w);
				
		CQuaternion localRotation = GetLocalRotation()->GetAsDXQuaternion();
		localRotation *= parentRotation;
		if (!m_absRotation)
		{
			m_absRotation = MP_NEW(CRotationPack);
		}
		
		m_absRotation->SetAsDXQuaternion(localRotation.x, localRotation.y, localRotation.z, localRotation.w);
        m_absRotation->SetEmptyRotation(m_rotation.IsEmptyRotation());
		return m_absRotation;*/

		if (!m_absRotation)
		{
			m_absRotation = MP_NEW(CRotationPack);
		}

		CQuaternion		quat = m_attachController->GetAbsChildRotation();
		m_absRotation->SetAsDXQuaternion( quat.x, quat.y, quat.z, quat.w);
		return m_absRotation;
	}


	if ((m_physics) && (m_isDynamic))
	{
		if (!m_absRotation)
		{
			m_absRotation = MP_NEW(CRotationPack);		
		}
		CQuaternion q(0, 0, 1, 0); 
		if (GetAnimation())
		{
			CSkeletonAnimationObject* sao = (CSkeletonAnimationObject *)GetAnimation();
			q *= sao->GetAnimationRotation();
		}
		q *= m_physicsObjects[0]->GetRotationPack()->GetAsDXQuaternion();

		m_absRotation->SetAsDXQuaternion(q.x, q.y, q.z, q.w);
		m_absRotation->SetEmptyRotation(m_rotation.IsEmptyRotation());

		return m_absRotation;
	}

	return GetLocalRotation();
}

//
// Разное
//

void C3DObject::SetPlugin(IPlugin* const plugin)
{
	m_plugin = plugin;
}

void C3DObject::OnChanged(int eventID)
{
	OnAfterChanges(eventID);
}

void C3DObject::SetBoundingBoxVisible(const bool isVisible)
{
	m_isBoundingBoxVisible = isVisible;
}

void C3DObject::SetFreeze(const bool isFrozen)
{
	m_isFrozen = isFrozen;

	if (GetAnimation())
	{
		GetAnimation()->SetFreeze(isFrozen);
	}
}

void C3DObject::SetZOffset(const float zOffset)
{
	m_zOffset = zOffset;
}

void C3DObject::AttachTo(C3DObject* const parent, const char* alpcBoneName)
{
	IAsyncUpdateListener* aAsyncUpdateListener = NULL;
	CBaseAttachParams* attachController = NULL;
	if( parent)
	{
		MP_NEW_P3( foo, CSkeletonAttach, parent, this, alpcBoneName);
		attachController = foo;

		if( parent->GetAnimation())
			aAsyncUpdateListener = foo;
	}
	AttachToImpl( parent, attachController, aAsyncUpdateListener);
}

void C3DObject::AttachTo(C3DObject* const parent)
{
	CBaseAttachParams* attachController = NULL;
	if( parent)
	{
		MP_NEW_P2( foo, CHierarchyAttach, parent, this);
		attachController = foo;
	}
	AttachToImpl( parent, attachController, NULL);
}

void C3DObject::AttachToImpl(C3DObject* const parent, CBaseAttachParams* apAttachController, IAsyncUpdateListener* /*apIAsyncUpdateListener*/)
{
	CVector3D positionAbsoluteBeforeAttach = GetCoords();

	m_parent = parent;

	if( m_attachController)
	{
		MP_DELETE( m_attachController);
		m_attachController = NULL;
	}

	m_attachController = apAttachController;

	// После аттача координаты объекта не должны меняться.
	// Абсолютный поворот сейчас поменяется. Это нужно доделывать реализуя функцию RotationAbsoluteToChild в контроллерах аттача (CHierarchyAttach и др.)
	// после чего делать SetRotation с предыдущим поворотом
	SetCoords(positionAbsoluteBeforeAttach);
}

void C3DObject::SetLODGroup(CLODGroup* const lodGroup)
{
	assert(lodGroup);

	m_lodGroup = lodGroup;
	lodGroup->Add3DObject(this);
	lodGroup->AddChangesListener(this);
}

void C3DObject::SetControlledObjectStatus(const bool isControlled)
{
	m_isControlled = isControlled;
}

void C3DObject::SetVisible(const bool isVisible)
{
	m_isVisible = isVisible;
	if (!isVisible)
	{
		m_isVisibleNow = false;
	}

	m_isNeededToRenderShadows = false;

	if (m_lodGroup)
	{
		m_lodGroup->NeedToUpdate();
	}	
}

void C3DObject::SetInCameraSpace(bool isInCameraSpace)
{
	m_isInCameraSpace = isInCameraSpace;
}

void C3DObject::SetLODNumber(const unsigned char lodNum)
{
	m_lodNum = lodNum;
}

void C3DObject::SetAnimation(CAnimationObject* const animation)
{
	m_animation = animation;

	if (m_animation)
	{
		m_animation->SetObject3D(this);

		SetFreeze(m_isFrozen);
	}
}

void C3DObject::FreeResources()
{
	if (m_isControlled)
	{
		g->lw.WriteLn("Attempt to kill my avatar!!!");

		if (g->ne.GetWorkMode() != WORK_MODE_DESTROY)
		{
			return;
		}
	}

	if (GetLODGroup())
	{
		GetLODGroup()->DeleteChangesListener(this);
		GetLODGroup()->OnDeleted(this);
	}

	//if (!GetAnimation())	
	{
		std::vector<CPhysicsObject *>::iterator it = m_physicsObjects.begin();
		std::vector<CPhysicsObject *>::iterator itEnd = m_physicsObjects.end();
		
		for ( ; it != itEnd; it++)
		{
			(*it)->Destroy();
			g->phom.DeleteObject((*it)->GetID());
		}

		m_physicsObjects.clear();
		m_physics = false;
	}

	if (m_animation)
	{
		g->skom.DeleteObject(((CSkeletonAnimationObject*)m_animation)->GetID());		
		m_animation = NULL;
	}

	if (m_absRotation)
	{
		MP_DELETE(m_absRotation);
		m_absRotation = NULL;
	}

	g->pf.OnObjectDeleted(GetID());
}

//
// Физика
//

bool C3DObject::IsPhysicsObject()const
{
	return m_physics;
}

void C3DObject::SetPhysicsFromModel(CModel* const model)
{
	if (m_physicsObjects.size() != 0)
	{
		return;
	}

	CPhysicsObject* physObj = NULL;
	CRotationPack* physObjRotate = NULL;	

	std::wstring physObjName = GetName();
	physObjName += L"_physObj_";

	for (unsigned int primitiveID = 0; primitiveID < model->GetPrimitivesCount(); primitiveID++)
	{
		int objectID = g->phom.AddObject(physObjName + IntToWStr(primitiveID));
		physObj = g->phom.GetObjectPointer(objectID);
		physObj->SetObject3D(this);

		SPrimitiveDescription* primitive = model->GetPrimitive(primitiveID);

		if (primitive->type == 1)
		{
			physObj->SetType(PHYSICS_OBJECT_TYPE_STATIC_BOX);
		}
		else if (primitive->type == 2)
		{
			physObj->SetType(PHYSICS_OBJECT_TYPE_STATIC_SPHERE);
		}

		physObj->GetPrimitive()->SetInitialDimensions(primitive->lengthX * primitive->scale.x * m_scaleX, primitive->lengthY * primitive->scale.y * m_scaleY, primitive->lengthZ * primitive->scale.z * m_scaleZ, 1.0f);

		CQuaternion objQuat = GetRotation()->GetAsDXQuaternion();
		CQuaternion primitiveQuat = primitive->rotation;
		CVector3D	primitiveTranslation = primitive->translation;

		primitiveTranslation *= objQuat;
		physObj->GetPrimitive()->SetInitialCoords(m_x + primitiveTranslation.x * m_scaleX, 
			m_y + primitiveTranslation.y * m_scaleY, m_z + primitiveTranslation.z * m_scaleZ);

		physObjRotate = physObj->GetRotationPack();		

		if  (physObj->Init())
		{
			m_physicsObjects.push_back(physObj);

			CQuaternion quat = primitiveQuat;
			quat *= objQuat;

			physObjRotate->SetAsDXQuaternion(quat.x, quat.y, quat.z, quat.w);
			physObjRotate->GetRotating(&quat.x, &quat.y, &quat.z, &quat.w);
			physObj->SetRotation(quat.x, quat.y, quat.z, quat.w);
			physObj->SetCollisionCatBits(1);
			physObj->SetCollisionColBits(2);
		}
	}
}

void C3DObject::SetPhysics(const bool physics, CModel* const model)
{
	CVector3D pos = GetCoords();
	SetPhysicsImpl( physics, model);
	SetCoords(pos);
}

void C3DObject::SetPhysicsImpl(const bool physics, CModel* const model)
{
	if (model)
	{
		SetPhysicsFromModel(model);
		return;
	}

	if ((m_physics) && (physics))
	{
		return;
	}

	if (!physics)
	{
		std::vector<CPhysicsObject *>::iterator it = m_physicsObjects.begin();
		std::vector<CPhysicsObject *>::iterator itEnd = m_physicsObjects.end();
		
		for ( ; it != itEnd; it++)
		{
			(*it)->Destroy();
			g->phom.DeleteObject(g->phom.GetObjectNumber((*it)->GetName()));
		}

		m_physicsObjects.clear();
		m_isDynamic = false;
		m_physics = false;

		return;
	}	

	int modelID;

	CQuaternion quat = m_rotation.GetAsDXQuaternion();

	if ( ((int)GetUserData(USER_DATA_LEVEL) == LEVEL_GROUND) || ((int)GetUserData(USER_DATA_LEVEL) == LEVEL_SEA) ||
		((int)GetUserData(USER_DATA_LEVEL) == LEVEL_CAMERA_COLLISION_TOO) || ((int)GetUserData(USER_DATA_LEVEL) == LEVEL_CAMERA_COLLISION_ONLY)
		 || ((int)GetUserData(USER_DATA_LEVEL) == LEVEL_COLLISION_MESH))
	if (m_lodGroup->GetLodLevel(0))
	{
		modelID = m_lodGroup->GetLodLevel(0)->GetModelID();
		CModel* _model = g->mm.GetObjectPointer(modelID);
		
		if ((!quat.IsEmptyRotation()) && (!m_rotation.IsEmptyRotation()))
		if (_model->GetLoadingState() == MODEL_LOADED)
		{			
			m_rotation.SetEmptyRotation(true);

			m_rotation.SetRotationModel(_model);

			m_rotation.ApplyRotationToVertexs();

			m_lodGroup->UpdateBoundingBox(this);
		}	
	}


	m_isDynamic = (GetAnimation() != NULL);
	
	if ((int)GetUserData(USER_DATA_LEVEL) == LEVEL_CAMERA_COLLISION_ONLY)
	{
		SetVisible(false);
	}

	if (((int)GetUserData(USER_DATA_LEVEL) != LEVEL_GROUND) && (physics) && (GetLODGroup()->GetPhysicsType() == PHYSICS_OBJECT_TYPE_STATIC_TRIMESH))
	{
		g->lw.WriteLn("[ERROR!] Level must be have 1 value per lodgroup ", GetName());
		return;
	}

	if (((int)GetUserData(USER_DATA_LEVEL) >= LEVEL_NO_COLLISION) && ((int)GetUserData(USER_DATA_LEVEL) != LEVEL_CAMERA_COLLISION_ONLY)
		&& ((int)GetUserData(USER_DATA_LEVEL) != LEVEL_SPHERE_COLLISION) && ((int)GetUserData(USER_DATA_LEVEL) != LEVEL_COLLISION_MESH))
	{
		return;
	}

	if ((physics) && (m_physicsObjects.size() == 0))
	{
		int physicObjectID = g->phom.AddObject(this->GetName());
		CPhysicsObject* physObj = g->phom.GetObjectPointer(physicObjectID);
		m_physicsObjects.push_back(physObj);

		if (m_lodGroup->GetLodLevel(0))
		{
			modelID = m_lodGroup->GetLodLevel(0)->GetModelID();

			if (GetAnimation())
			{
				modelID = GetAnimation()->GetModelID();
			}

			float scale = (m_lodGroup->GetLodLevel(0)) ? m_lodGroup->GetLodLevel(0)->size : 1.0f;

			CModel* model = g->mm.GetObjectPointer(modelID);
			if ((int)GetUserData(USER_DATA_LEVEL) == LEVEL_SPHERE_COLLISION)
			{
				physObj->SetType(PHYSICS_OBJECT_TYPE_STATIC_SPHERE);
			}
			else if (((int)GetUserData(USER_DATA_LEVEL) != LEVEL_GROUND) && ((int)GetUserData(USER_DATA_LEVEL) != LEVEL_CAMERA_COLLISION_ONLY)
				&& ((int)GetUserData(USER_DATA_LEVEL) != LEVEL_COLLISION_MESH))
			{
				physObj->SetType(m_lodGroup->GetPhysicsType());
			}
			else if ((int)GetUserData(USER_DATA_LEVEL) != LEVEL_COLLISION_MESH)
			{
				GetLODGroup()->SetPhysicsType(PHYSICS_OBJECT_TYPE_STATIC_TRIMESH);
				physObj->SetType(PHYSICS_OBJECT_TYPE_STATIC_TRIMESH);			
				physObj->GetPrimitive()->SetUserData(model);
			}
			else
			{
				GetLODGroup()->SetPhysicsType(PHYSICS_OBJECT_TYPE_STATIC_COLLISION_MESH);
				physObj->SetType(PHYSICS_OBJECT_TYPE_STATIC_COLLISION_MESH);			
				physObj->GetPrimitive()->SetUserData(model);
			}
		
			physObj->GetPrimitive()->SetInitialCoords(m_x, m_y, m_z);
		
			if (GetAnimation())
			{
				GetAnimation()->Update(GetAnimation()->GetUpdateTime() + 1, 0);
				model = GetAnimation()->GetModel();
				physObj->GetPrimitive()->SetInitialDimensions(ANIM_DIMENSIONS, 1.0f);
			}
			else
			{
				float sizeX = model->GetModelLengthX() * scale * m_scaleX;
				float sizeY = model->GetModelLengthY() * scale * m_scaleY;
				float sizeZ = model->GetModelLengthZ() * scale * m_scaleZ;

				if (sizeX < MIN_BIG_OBJECT_SIZE)
				{
					sizeX *= COLLISION_CORRELATION;
				}

				if (sizeY < MIN_BIG_OBJECT_SIZE)
				{
					sizeY *= COLLISION_CORRELATION;
				}

				if (sizeZ < MIN_BIG_OBJECT_SIZE)
				{
					sizeZ *= COLLISION_CORRELATION;
				}

				if ((int)GetUserData(USER_DATA_LEVEL) == LEVEL_SPHERE_COLLISION)
				if (sizeY > sizeX)
				{
					sizeX = sizeY;
				}

				physObj->GetPrimitive()->SetInitialDimensions(sizeX, sizeY, sizeZ, 1.0f);
			}	
		}
		
		physObj->SetObject3D(this);
		bool has_geom = physObj->Init();
		if (!has_geom)
		{
			m_physics = false;
			return;
		}
		
		physObj->GetRotationPack()->SetAsDXQuaternion(quat.x, quat.y, quat.z, quat.w);
	
		m_rotation.GetRotating(&quat.x, &quat.y, &quat.z, &quat.w);
		physObj->SetRotation(quat.x, quat.y, quat.z, quat.w);

		if (GetAnimation())
		{
			physObj->SetCollisionColBits(m_lodGroup->GetCollisionBits());
			physObj->SetCollisionCatBits(m_lodGroup->GetCollisionBits());
		}
		else
		{
			physObj->SetCollisionCatBits(1);
			physObj->SetCollisionColBits(2);
		}
		
		physObj->Update();
	}

	m_physics = physics;
}

int C3DObject::GetPhysicsObjID()const
{
	if (m_physicsObjects.size() > 0)
	{
		return m_physicsObjects[0]->GetID();
	}
	else
	{
		return -1;
	}
 }

void C3DObject::SetPivotObject(C3DObject* const group)
{
	m_group = group;
}

//
// Bounding box
//

void C3DObject::SetBoundingBoxColor(const unsigned char r, const unsigned char g, const unsigned char b, const unsigned char a)
{
	m_bbColor.r = r;
	m_bbColor.g = g;
	m_bbColor.b = b;
	m_bbColor.a = a;
}

CColor4* C3DObject::GetBoundingBoxColor()const 
{
	return (CColor4 *)&m_bbColor;
}

void  C3DObject::GetBoundingBox(float& minX, float& minY, float& minZ, float& maxX, float& maxY, float& maxZ)const
{
	minX = m_centerX - m_sizeX;
	minY = m_centerY - m_sizeY;
	minZ = m_centerZ - m_sizeZ;
	maxX = m_centerX + m_sizeX;
	maxY = m_centerY + m_sizeY;
	maxZ = m_centerZ + m_sizeZ;
}

void  C3DObject::GetInitialAABB(float& minX, float& minY, float& minZ, float& maxX, float& maxY, float& maxZ)const
{
	minX = m_centerX - m_ooSizeX;
	minY = m_centerY - m_ooSizeY;
	minZ = m_centerZ - m_ooSizeZ;
	maxX = m_centerX + m_ooSizeX;
	maxY = m_centerY + m_ooSizeY;
	maxZ = m_centerZ + m_ooSizeZ;
}

bool C3DObject::IsInBoundingBox(const float x, const float y, const float z)
{
	float myX, myY, myZ;
	GetCoords(&myX, &myY, &myZ);
	return ((x - myX >= m_centerX - m_sizeX) && (x - myX <= m_centerX + m_sizeX) &&
		(y - myY >= m_centerY - m_sizeY) && (y - myY <= m_centerY + m_sizeY) &&
		(z - myZ >= m_centerZ - m_sizeZ) && (z - myZ <= m_centerZ + m_sizeZ));
}

bool C3DObject::IsInBoundingBox(const float x, const float y, const float z, const float eps)
{
	/*float myX, myY, myZ;
	GetCoords(&myX, &myY, &myZ);*/
	return ((x - m_absX >= m_rotatedCenter.x - m_sizeX - eps) && (x - m_absX <= m_rotatedCenter.x + m_sizeX + eps) &&
		(y - m_absY >= m_rotatedCenter.y - m_sizeY - eps) && (y - m_absY <= m_rotatedCenter.y + m_sizeY + eps) &&
		(z - m_absZ >= m_rotatedCenter.z - m_sizeZ - eps) && (z - m_absZ <= m_rotatedCenter.z + m_sizeZ + eps));
}

void C3DObject::GetOOBoundingBox(CVector3D& origin, CVector3D*& ooCorners)
{
	origin.Set(m_absX + m_rotatedCenter.x, m_absY + m_rotatedCenter.y, m_absZ + m_rotatedCenter.z);
	ooCorners = &m_ooCorners[0];	
}

bool C3DObject::IsBoundingBoxInFrustum()
{
	if (m_isInCameraSpace) {
		return true;
	}

	if (g->cm.IsPointInFrustum(m_absX, m_absY, m_absZ))
	{
		return true;
	}	

	if (g->cm.IsBoundingBoxInFrustum(m_absX + m_rotatedCenter.x, m_absY + m_rotatedCenter.y, m_absZ + m_rotatedCenter.z, &m_ooCorners[0]))
	{
		return true;
	}

	float cameraX, cameraY, cameraZ;
	g->cm.GetCameraEye(&cameraX, &cameraY, &cameraZ);

	bool isVisible = (((int)GetUserData(USER_DATA_LEVEL)%2 == 1) && (cameraX >= m_centerX - m_sizeX) && 
		(cameraX <= m_centerX + m_sizeX) && (cameraY >= m_centerY - m_sizeY) && (cameraY <= m_centerY + m_sizeY));

	return isVisible;
}

bool C3DObject::IsShadowBoundingBoxInFrustum()
{
	if (g->rs.GetBool(CAVE_SUPPORT))
	{
		return true;
	}
	/*bool isVisible = false;

	for (int _x = -1; _x <= 1; _x += 2)
	for (int _y = -1; _y <= 1; _y += 2)
	for (int _z = -1; _z <= 1; _z += 2)
	if (g->cm.IsPointInFrustum(m_absX + m_centerX - m_sizeZ * SHADOW_BOUNDING_BOX_OFFSET + _x * (m_sizeX * SHADOW_BOUNDING_BOX_SCALE_XY), 
		m_absY + m_centerY - m_sizeZ * SHADOW_BOUNDING_BOX_OFFSET + _y * (m_sizeY * SHADOW_BOUNDING_BOX_SCALE_XY), 
		m_absZ + m_centerZ + _z * m_sizeZ * SHADOW_BOUNDING_BOX_SCALE_Z))
	{
		isVisible = true;
	}*/

	if (g->cm.IsBoundingBoxInFrustum(m_absX + m_centerX - m_sizeZ * SHADOW_BOUNDING_BOX_OFFSET, 
		m_absY + m_centerY - m_sizeZ * SHADOW_BOUNDING_BOX_OFFSET, m_absZ + m_centerZ - m_sizeZ * (SHADOW_BOUNDING_BOX_SCALE_Z - 1.0f), 
		m_sizeX * SHADOW_BOUNDING_BOX_SCALE_XY, m_sizeY * SHADOW_BOUNDING_BOX_SCALE_XY, 
		m_sizeZ * SHADOW_BOUNDING_BOX_SCALE_Z))
	{
		return true;
	}

	return false;
}

void C3DObject::SetBoundingBox(const float x, const float y, const float z, const float sizeX, const float sizeY, const float sizeZ)
{
#pragma warning(push)
#pragma warning(disable : 4239)
	CVector3D& scale = GetScale3D();
#pragma warning(pop)

	m_ooSizeX = sizeX * scale.x;
	m_ooSizeY = sizeY * scale.y;
	m_ooSizeZ = sizeZ * scale.z;

	float lastSizeZ = m_sizeZ;

	m_centerX = x * scale.x;
	m_centerY = y * scale.y;
	m_centerZ = z * scale.z;
	m_sizeX = sizeX * scale.x;
	m_sizeY = sizeY * scale.y;
	m_sizeZ = sizeZ * scale.z;

	if (!GetRotation())
	{
		return;
	}

	CQuaternion q = GetRotation()->GetAsDXQuaternion();
	q.Normalize();

	CVector3D tmp[8];
	int i = 0;
	for (int _x = -1; _x <= 1; _x += 2)
	for (int _y = -1; _y <= 1; _y += 2)
	for (int _z = -1; _z <= 1; _z += 2)
	{
		tmp[i].Set(_x * m_sizeX, _y * m_sizeY, _z * m_sizeZ);
		tmp[i] *= q;

		m_ooCorners[i].Set(_x * m_ooSizeX, _y * m_ooSizeY, _z * m_ooSizeZ);
		m_ooCorners[i] *= q;

		i++;
	}
		
	m_rotatedCenter.Set(m_centerX, m_centerY, m_centerZ);
	m_rotatedCenter *= q;

	float minX = FLT_MAX;
	float minY = FLT_MAX;
	float minZ = FLT_MAX;
	float maxX = FLT_MIN;
	float maxY = FLT_MIN;
	float maxZ = FLT_MIN;

	for (i = 0; i < 8; i++)
	{
		if (tmp[i].x < minX)
		{
			minX = tmp[i].x;
		}

		if (tmp[i].x > maxX)
		{
			maxX = tmp[i].x;
		}

		if (tmp[i].y < minY)
		{
			minY = tmp[i].y;
		}

		if (tmp[i].y > maxY)
		{
			maxY = tmp[i].y;
		}

		if (tmp[i].z < minZ)
		{
			minZ = tmp[i].z;
		}

		if (tmp[i].z > maxZ)
		{
			maxZ = tmp[i].z;
		}
	}

	m_sizeX = maxX;
	if (fabs(minX) > sizeX)
	{
		m_sizeX = fabs(minX);
	}

	m_sizeY = maxY;
	if (fabs(minY) > sizeY)
	{
		m_sizeY = fabs(minY);
	}

	m_sizeZ = maxZ;
	if (fabs(minZ) > sizeZ)
	{
		m_sizeZ = fabs(minZ);
	}

	m_realSizeZ = m_sizeZ * 2.0f;
	if (GetAnimation())
	{
		if (g->phom.GetControlledObject())
		if (g->phom.GetControlledObject()->GetHalfHeight() > 0)
		{
			m_sizeZ = g->phom.GetControlledObject()->GetHalfHeight();
		}
	}

	if (m_sizeX < MIN_SIZE)
	{
		m_sizeX = MIN_SIZE;
	}
	if (m_sizeY < MIN_SIZE)
	{
		m_sizeY = MIN_SIZE;
	}
	if (m_sizeZ < MIN_SIZE)
	{
		m_sizeZ = MIN_SIZE;
	}

	if ((GetAnimation()) && (!m_isBoundingBoxSet))
	{
		CVector3D physicsCoords = GetCoords();

		m_isBoundingBoxSet = true;

		if (m_physicsObjects.size() > 0)
		{
			m_physicsObjects[0]->SetCoords(physicsCoords.x, physicsCoords.y, physicsCoords.z + m_isBoundingBoxSet * m_sizeZ);
		}		
	}


	if (m_physics)
	if (lastSizeZ > MIN_SIZE)
	if (fabs(lastSizeZ - m_sizeZ) > MIN_Z_CHANGE)
	{
		if ((GetAnimation()) && (m_isControlled))
		{
			CPhysicsObject* obj = m_physicsObjects[0];
			float x, y, z;
			obj->GetCoords(&x, &y, &z);
			obj->SetCoords(x, y, z + (m_sizeZ - lastSizeZ));
		}
	}
}

void C3DObject::SetNeedRenderShadowsStatus(const bool isNeeded)
{
	m_isNeededToRenderShadows = isNeeded;
}

void C3DObject::SetLODDistanceCanBeChangedStatus(const bool isLODDistanceCanBeChanged)
{
	m_isLODDistanceCanBeChanged = isLODDistanceCanBeChanged;
}

bool C3DObject::IsCollidedWithOtherObjects()
{
	return g->phom.IsCollidedWithOtherObjects(m_physicsObjects);
}

C3DObject::~C3DObject()
{
	if (m_absRotation)
	{
		MP_DELETE(m_absRotation);
		m_absRotation = NULL;
	}
}