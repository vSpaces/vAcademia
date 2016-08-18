//----------------------------------------------------------------------------//
// BaseInterface.h                                                            //
// Copyright (C) 2001, 2002 Bruno 'Beosil' Heidelberger                       //
//----------------------------------------------------------------------------//
// This program is free software; you can redistribute it and/or modify it    //
// under the terms of the GNU General Public License as published by the Free //
// Software Foundation; either version 2 of the License, or (at your option)  //
// any later version.                                                         //
//----------------------------------------------------------------------------//

#ifndef BASE_INTERFACE_H
#define BASE_INTERFACE_H

//----------------------------------------------------------------------------//
// Forward declarations                                                       //
//----------------------------------------------------------------------------//

class CWnd;
class CBaseNode;
class CBaseMesh;
class CBaseMaterial;

//----------------------------------------------------------------------------//
// Class declaration                                                          //
//----------------------------------------------------------------------------//

class CBaseInterface
{
// constructors/destructor
protected:
	CBaseInterface();
	virtual ~CBaseInterface();

// member functions
public:
#ifdef MAX3DS_BUILD
	virtual Matrix3 GetNodeTM(CBaseNode *pNode, float time) = 0;
	virtual Point3 GetAmbient() = 0;
#endif
	virtual int GetCurrentFrame() = 0;
	virtual float GetCurrentTime() = 0;
	virtual int GetEndFrame() = 0;
	virtual int GetFps() = 0;
	virtual CWnd *GetMainWnd() = 0;
	virtual CBaseMaterial *GetMaterial(int materialId) = 0;
	virtual int GetMaterialCount() = 0;
	virtual CBaseMesh *GetMesh(CBaseNode *pNode) = 0;
#ifdef MAX3DS_BUILD
	virtual CBaseNode *GetNode(const Cal3DComString& strName) = 0;
#else
	virtual CBaseNode *GetNode(const std::string& strName) = 0;
#endif
	virtual CBaseNode *GetSelectedNode(int nodeId) = 0;
	virtual int GetSelectedNodeCount() = 0;
	virtual int GetStartFrame() = 0;
	virtual void GetScale(CBaseNode *pNode, float time, CalVector& scale) = 0;
	virtual void GetTranslationAndRotation(CBaseNode *pNode, CBaseNode *pParentNode, float time, CalVector& translation, CalQuaternion& rotation) = 0;
	virtual void GetTranslationAndRotationBoneSpace(CBaseNode *pNode, float time, CalVector& translation, CalQuaternion& rotation) = 0;
	virtual bool IsBone(CBaseNode *pNode) = 0;
	virtual bool IsDummy(CBaseNode *pNode) = 0;
	virtual bool IsMesh(CBaseNode *pNode) = 0;
	virtual void SetProgressInfo(int percentage) = 0;
#ifdef MAX3DS_BUILD
	virtual void StartProgressInfo(const Cal3DComString& strText) = 0;
#else
	virtual void StartProgressInfo(const std::string& strText) = 0;
#endif
	virtual void StopProgressInfo() = 0;
	virtual void RedrawViews() = 0;
};

#endif

//----------------------------------------------------------------------------//
