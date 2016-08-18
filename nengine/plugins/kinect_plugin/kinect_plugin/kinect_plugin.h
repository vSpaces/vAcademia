#pragma once

#define KINECT_DECL		__declspec(dllimport)

KINECT_DECL	IKinect* CreateKinectObject();