#include "stdafx.h"
#include "SkeletalViewer.h"
#include "kinect_plugin.h"



/*****************************************************
	плагин возвращает ссылку на интерфейс
	через который можно  заинициализировать кинект
	запросить координаты скелета
	и разинициализировать кинект и  удалить плагин
******************************************************/

IKinect* CreateKinectObject()
{
	CSkeletalViewerApp*	pSkeletalViewerApp = new CSkeletalViewerApp();
	return pSkeletalViewerApp;
}

