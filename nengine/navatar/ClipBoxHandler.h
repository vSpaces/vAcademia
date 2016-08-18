
#pragma once

#include "ClippingVolume.h"
#include "IMessageHandler.h"
#include "3DObject.h"
#include "TinyXML.h"

typedef struct _SMyClippingVolume
{
	bool isEnabled;
	int coordID, coordID2, coordID3;
	float endValue, endValue2, endValue3;
	float startValue, startValue2, startValue3;

	_SMyClippingVolume()
	{
		isEnabled = false;
		coordID3 = -1;
	}
} SMyClippingVolume;

class CClipBoxHandler
{
public:
	CClipBoxHandler();
	~CClipBoxHandler();

	void ProcessMessage(std::string& command, IMessageObject* message);
	void SetObject3D(C3DObject* obj3d);
	void DisableAll();

private:
	void SetClipBox(int type, std::string& valueID, std::string& valueOrt, std::string& valuePlaneStart, std::string& valuePlaneEnd,std::string& valueOrt2,std::string& valuePlaneStart2, std::string& valuePlaneEnd2, std::string& valueOrt3,std::string& valuePlaneStart3, std::string& valuePlaneEnd3);
	void DisableClipBox(std::string& valueID);
	void DisableClipBox(int valueID);

	SMyClippingVolume m_clipVolume[10/*CLIPPING_VOLUME_COUNT*/];
	C3DObject* m_obj3d;
};