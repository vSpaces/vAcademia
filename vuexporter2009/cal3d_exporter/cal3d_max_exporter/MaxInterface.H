//----------------------------------------------------------------------------//
// MaxInterface.h                                                             //
// Copyright (C) 2001, 2002 Bruno 'Beosil' Heidelberger                       //
//----------------------------------------------------------------------------//
// This program is free software; you can redistribute it and/or modify it    //
// under the terms of the GNU General Public License as published by the Free //
// Software Foundation; either version 2 of the License, or (at your option)  //
// any later version.                                                         //
//----------------------------------------------------------------------------//

#ifndef MAX_INTERFACE_H
#define MAX_INTERFACE_H

//----------------------------------------------------------------------------//
// Includes                                                                   //
//----------------------------------------------------------------------------//

#include "BaseInterface.h"

//----------------------------------------------------------------------------//
// Defines                                                                    //
//----------------------------------------------------------------------------//

// this is the class for all biped controllers except the root and the footsteps
#define BIPSLAVE_CONTROL_CLASS_ID Class_ID(0x9154,0)
// this is the class for the center of mass, biped root controller ("Bip01")
#define BIPBODY_CONTROL_CLASS_ID  Class_ID(0x9156,0) 

//----------------------------------------------------------------------------//
// Forward declarations                                                       //
//----------------------------------------------------------------------------//

class CBaseNode;
class CMaxNode;
class CBaseMaterial;

//----------------------------------------------------------------------------//
// Class declaration                                                          //
//----------------------------------------------------------------------------//

class CMaxInterface : public CBaseInterface
{
// misc
protected:
	static DWORD WINAPI ProgressFunction(LPVOID arg);

// member variables
protected:
	ExpInterface *m_pExpInterface;
	Interface *m_pInterface;

// constructors/destructor
public:
	CMaxInterface();
	virtual ~CMaxInterface();

// member functions
public:
	bool Create(ExpInterface *pExpInterface, Interface *pInterface);
	int GetCurrentFrame();
	float GetCurrentTime();
	int GetEndFrame();
	int GetFps();
	CWnd *GetMainWnd();
	CBaseMaterial *GetMaterial(int materialId);
	int GetMaterialCount();
	CBaseMesh *GetMesh(CBaseNode *pNode);
	CBaseNode *GetNode(const CComString& strName);
	int GetSelectedNodeCount();
	CBaseNode *GetSelectedNode(int nodeId);
	int GetStartFrame();
	void GetTranslationAndRotation(CBaseNode *pNode, CBaseNode *pParentNode, float time, CalVector& translation, CalQuaternion& rotation);
	void GetTranslationAndRotationBoneSpace(CBaseNode *pNode, float time, CalVector& translation, CalQuaternion& rotation);
	bool IsBone(CBaseNode *pNode);
	bool IsDummy(CBaseNode *pNode);
	bool IsMesh(CBaseNode *pNode);
	void SetProgressInfo(int percentage);
	void StartProgressInfo(const CComString& strText);
	void StopProgressInfo();
	Point3 GetAmbient();

protected:
	virtual Matrix3 GetNodeTM(CBaseNode *pNode, float time);
	bool IsBipedBone(INode *pNode);
};

#endif

//----------------------------------------------------------------------------//
