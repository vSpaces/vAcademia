
#include "StdAfx.h"
#include "SkeletonAnimationObject.h"
#include "GlobalSingletonStorage.h"
#include "KinectVideoTexture.h"

void CSkeletonAnimationObject::DumpKinectInfoToLog()
{
	if (m_kinectController)
	{
		m_kinectController->DumpKinectInfoToLog();
	}
}

void CSkeletonAnimationObject::SetKinectSeatedMode(const bool isSeated)
{
	if (m_kinectController)
	{
		m_kinectController->SetSeatedMode(isSeated);
	}
}

void CSkeletonAnimationObject::EnableKinect()
{
	if (m_kinectController)
	{
		m_kinectController->Enable();
		LookAt(NULL, 0, 0);
	}
}

void CSkeletonAnimationObject::DisableKinect()
{
	if (m_kinectController)
	{
		m_kinectController->Disable();
	}
}

IKinect* CSkeletonAnimationObject::GetKinectObject()
{
	return m_kinectObj;
}

void CSkeletonAnimationObject::SetKinectObject(IKinect* kinect)
{
	m_kinectObj = kinect;
	MP_NEW_V(m_kinectController, CKinectController, this);
}

bool CSkeletonAnimationObject::IsControlledByKinect()
{
	if( !m_kinectObj)
		return false;

	return true;
}

//  Возвращает размер данных текущего бинарного состояния кинекта
unsigned int CSkeletonAnimationObject::GetKinectSynchDataSize()
{
	if (!m_kinectController)
	{
		return 0;
	}

	if (m_kinectEmulator)
	{
		return 0;
	}

	if (!m_kinectController->IsEnabled())
	{		
		return 0;
	}

	int size;
	m_kinectController->GetSynchData(NULL, &size);
	return size;
}

// Должен заполнить готовый буфер data данными. size - размер буфера. Чтобы его узнать - надо вызвать GetKinectSynchDataSize
bool  CSkeletonAnimationObject::GetKinectSynchData(const unsigned char* data, int& size)
{
	if (m_kinectEmulator)
	{		
		return false;
	}

	if (!m_kinectController->IsEnabled())
	{
		size = 0;		
		return true;
	}

	return m_kinectController->GetSynchData((unsigned char*)data, &size);	
}

// Установить скелету нужное состояние
bool  CSkeletonAnimationObject::SetKinectSynchData(const unsigned char* data, unsigned int size)
{
	if (size == 0)
	{
		if (m_kinectEmulator)
		{
			m_kinectEmulator->ClearData();
			m_kinectController->Disable();
		}

		return true;
	}

	g->lw.WriteLn("SetKinectSynchData: ", m_obj3d->GetName(), " size: ", size);

	if (!m_kinectEmulator)
	{
		m_kinectEmulator = MP_NEW(CKinectEmulator);				
	}
	if (!m_kinectObj)
	{
		SetKinectObject(m_kinectEmulator);
	}
	PatchKinectStateIfNeeded(data, size);
	m_kinectEmulator->SetData((void*)data, size);		
	m_kinectController->SetSeatedMode(m_kinectEmulator->IsSeated());
	m_kinectController->Enable();	

	/*if (m_obj3d)
	{
		g->lw.WriteLn("SetKinectSynchData ", m_obj3d->GetName(), " size = ", size);
		g->lw.WriteLn("IsFoundSkeleton ", m_kinectEmulator->IsFoundSkeleton());
		g->lw.WriteLn("IsSeated ", m_kinectEmulator->IsSeated());
		g->lw.WriteLn("IsTwoSkeletonsFound ", m_kinectEmulator->IsTwoSkeletonsFound());
		g->lw.WriteLn("GetHeadTrackingQuality ", m_kinectEmulator->GetHeadTrackingQuality());
		g->lw.WriteLn("GetLeftArmTrackingQuality ", m_kinectEmulator->GetLeftArmTrackingQuality());
		g->lw.WriteLn("GetRightArmTrackingQuality ", m_kinectEmulator->GetRightArmTrackingQuality());
		g->lw.WriteLn("GetLeftFootTrackingQuality ", m_kinectEmulator->GetLeftFootTrackingQuality());
		g->lw.WriteLn("GetRightFootTrackingQuality ", m_kinectEmulator->GetRightFootTrackingQuality());
	}*/
	return true;
}

void CSkeletonAnimationObject::PatchKinectStateIfNeeded(const unsigned char* data, unsigned int size)
{
	if (size >=SKELETON_SIZE)
	{
		m_kinectController->SetRotatedToBack(data[SKELETON_SIZE - 1] == 1);

		if (size > SKELETON_SIZE)
		{
			m_kinectController->SetSecondaryLastRightCoords((float*)data);
		}

		if (size > TWO_SKELETONS_SIZE)
		{
			CQuaternion q;
			float* _data = (float*)data;
			CVector3D start(_data[85], _data[86], _data[87]);
			CVector3D end(_data[88], _data[89], _data[90]);

			if ((start.x != 0.0f) && (start.y != 0.0f) && (start.z != 0.0f) &&
				(end.x != 0.0f) && (end.y != 0.0f) && (end.z != 0.0f))
			{
				m_kinectController->SetKinectDeskPoints(start, end, NULL);
			}
		}
	}
}

void CSkeletonAnimationObject::GetKinectTrackedParts(bool& isHeadTracked, bool& isLeftArmTracked,
													 bool& isRightArmTracked, bool& isLeftFootTracked,
													 bool& isRightFootTracked, bool& areTwoSkeletonsFound,
													 bool& areMarkersNotFound)
{
	if ((!m_kinectController) || (!m_kinectController->IsAnythingTracked()))
	{
		isHeadTracked = false;
		isLeftArmTracked = false;
		isRightArmTracked = false;
		isLeftFootTracked = false;
		isRightFootTracked = false;
		areTwoSkeletonsFound = false;
		areMarkersNotFound = false;
		return;
	}

	if (m_kinectController)
	{
		isHeadTracked = m_kinectController->IsHeadTracked();
		isLeftArmTracked = m_kinectController->IsLeftArmTracked();
		isRightArmTracked = m_kinectController->IsRightArmTracked();
		isLeftFootTracked = m_kinectController->IsLeftFootTracked();
		isRightFootTracked = m_kinectController->IsRightFootTracked();
		areTwoSkeletonsFound = m_kinectController->AreTwoSkeletonsFound();
		areMarkersNotFound = m_kinectController->AreMarkersNotFound();
	}
}

bool CSkeletonAnimationObject::IsKinectEnabled()
{
	if (m_kinectController)
	{
		return m_kinectController->IsEnabled();
	}

	return false;
}

IDynamicTexture* CSkeletonAnimationObject::GetVideoDynamicTexture()
{
	if (!m_kinectObj)
	{
		return NULL;
	}

	if (!m_kinectVideoTexture)
	{
		MP_NEW_V(m_kinectVideoTexture, CKinectVideoTexture, m_kinectObj);
	}

	return m_kinectVideoTexture;
}

void CSkeletonAnimationObject::DeleteDynamicVideoTexture()
{
	if (m_kinectVideoTexture)
	{
		if (!m_kinectVideoTexture->IsAnalyzeEnabled())
		{
			g->tm.RemoveDynamicTexture(m_kinectVideoTexture);			
			m_kinectVideoTexture = NULL;
		}
	}
}

void CSkeletonAnimationObject::SetKinectDeskPoints(CVector3D start, CVector3D end, C3DObject* obj)
{
	if (m_kinectController)
	{
		m_kinectController->SetKinectDeskPoints(start, end, obj);
	}
}

void CSkeletonAnimationObject::DisableKinectDesk()
{
	if (m_kinectController)
	{
		m_kinectController->DisableKinectDesk();
	}
}

void CSkeletonAnimationObject::SetMarkerType(const int markerType)
{
	if (m_kinectController)
	{
		m_kinectController->SetMarkerType(markerType);
	}
}

void CSkeletonAnimationObject::EnableFaceTracking()
{
	if (m_kinectController)
	{
		m_kinectController->EnableFaceTracking();
	}
}

void CSkeletonAnimationObject::DisableFaceTracking()
{
	if (m_kinectController)
	{
		m_kinectController->DisableFaceTracking();
	}
}

void CSkeletonAnimationObject::ReloadKinectSettings()
{
	if (m_kinectController)
	{
		m_kinectController->ReloadSettings();
	}
}