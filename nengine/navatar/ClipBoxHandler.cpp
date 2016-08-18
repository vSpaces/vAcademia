
#include "StdAfx.h"
#include "MessageObject.h"
#include "XMLAttributes.h"
#include "ClipBoxHandler.h"
#include "SkeletonAnimationObject.h"

CClipBoxHandler::CClipBoxHandler()
{
}

void CClipBoxHandler::SetClipBox(int type, std::string& valueID, std::string& valueOrt, 
								 std::string& valuePlaneStart, std::string& valuePlaneEnd, std::string& valueOrt2,
								 std::string& valuePlaneStart2, std::string& valuePlaneEnd2, std::string& valueOrt3,
								 std::string& valuePlaneStart3, std::string& valuePlaneEnd3)
{
	CSkeletonAnimationObject* sao = (CSkeletonAnimationObject *)m_obj3d->GetAnimation();
	if (sao)
	{
		int iValueID = rtl_atoi(valueID.c_str()) - 1;
		if (iValueID<0 || iValueID>=CLIPPING_VOLUME_COUNT )
		{
			return;
		}
		int iValueOrt = rtl_atoi(valueOrt.c_str());
		int iValuePlaneStart = rtl_atoi(valuePlaneStart.c_str());
		int iValuePlaneEnd = rtl_atoi(valuePlaneEnd.c_str());

		int iValueOrt2 = rtl_atoi(valueOrt2.c_str());
		int iValuePlaneStart2 = rtl_atoi(valuePlaneStart2.c_str());
		int iValuePlaneEnd2 = rtl_atoi(valuePlaneEnd2.c_str());

		int iValueOrt3 = rtl_atoi(valueOrt3.c_str());
		int iValuePlaneStart3 = rtl_atoi(valuePlaneStart3.c_str());
		int iValuePlaneEnd3 = rtl_atoi(valuePlaneEnd3.c_str());
		sao->SetClippingVolume(type, iValueID, iValueOrt, (float)iValuePlaneStart, (float)iValuePlaneEnd,
			iValueOrt2, (float)iValuePlaneStart2, (float)iValuePlaneEnd2,
			iValueOrt3, (float)iValuePlaneStart3, (float)iValuePlaneEnd3);
		m_clipVolume[iValueID].isEnabled = true;
		m_clipVolume[iValueID].coordID = iValueOrt;
		m_clipVolume[iValueID].coordID2 = iValueOrt2;
		m_clipVolume[iValueID].coordID3 = iValueOrt3;
		m_clipVolume[iValueID].endValue = (float)iValuePlaneEnd;
		m_clipVolume[iValueID].endValue2 = (float)iValuePlaneEnd2;
		m_clipVolume[iValueID].endValue3 = (float)iValuePlaneEnd3;
		m_clipVolume[iValueID].startValue = (float)iValuePlaneStart;
		m_clipVolume[iValueID].startValue2 = (float)iValuePlaneStart2;
		m_clipVolume[iValueID].startValue3 = (float)iValuePlaneStart3;
	}
}
void CClipBoxHandler::DisableClipBox(int iValueID)
{
	m_clipVolume[iValueID].isEnabled = false;
	CSkeletonAnimationObject* sao = (CSkeletonAnimationObject *)m_obj3d->GetAnimation();
	if (sao)
	{
		sao->DisableClippingVolume(iValueID);
	}

}
void CClipBoxHandler::DisableClipBox(std::string& valueID)
{
	DisableClipBox(rtl_atoi(valueID.c_str()) - 1);
}

void CClipBoxHandler::ProcessMessage(std::string& command, IMessageObject* message)
{
	int argumentCount = message->GetArgumentCount();
	
	if ("set_clip_box" == command)
	{
		//assert(argumentCount == 8);
		std::string valueID = message->GetNextString();
		std::string valueOrt = message->GetNextString();
		std::string valuePlaneStart = message->GetNextString();
		std::string valuePlaneEnd = message->GetNextString();

		std::string valueOrt2 = message->GetNextString();
		std::string valuePlaneStart2 = message->GetNextString();
		std::string valuePlaneEnd2 = message->GetNextString();

		std::string valueOrt3 = "-1";
		std::string valuePlaneStart3 = "";
		std::string valuePlaneEnd3 = "";

		if (argumentCount == 11)
		{
			valueOrt3 = message->GetNextString();
			valuePlaneStart3 = message->GetNextString();
			valuePlaneEnd3 = message->GetNextString();
		}

		SetClipBox((valueID == "9") ? CLIPPING_VOLUME_TYPE_SPHERE : CLIPPING_VOLUME_TYPE_BOX,
			valueID,  valueOrt,  valuePlaneStart,  valuePlaneEnd, valueOrt2, valuePlaneStart2, valuePlaneEnd2,
			valueOrt3, valuePlaneStart3, valuePlaneEnd3);
	}
	else if ("set_clip_box_disable" == command)
	{
		assert(argumentCount == 2);
		std::string valueID = message->GetNextString();
		DisableClipBox(valueID);
	}
}

void CClipBoxHandler::SetObject3D(C3DObject* obj3d)
{
	m_obj3d = obj3d;
}

void CClipBoxHandler::DisableAll()
{
	int i;
	for (i = 0; i < CLIPPING_VOLUME_COUNT; i++)
	{
		DisableClipBox(i);
	}
}

CClipBoxHandler::~CClipBoxHandler()
{
}