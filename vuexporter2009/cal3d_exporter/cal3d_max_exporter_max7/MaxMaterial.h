//----------------------------------------------------------------------------//
// MaxMaterial.h                                                              //
// Copyright (C) 2001, 2002 Bruno 'Beosil' Heidelberger                       //
//----------------------------------------------------------------------------//
// This program is free software; you can redistribute it and/or modify it    //
// under the terms of the GNU General Public License as published by the Free //
// Software Foundation; either version 2 of the License, or (at your option)  //
// any later version.                                                         //
//----------------------------------------------------------------------------//

#ifndef MAX_MATERIAL_H
#define MAX_MATERIAL_H

//----------------------------------------------------------------------------//
// Includes                                                                   //
//----------------------------------------------------------------------------//

#include "BaseMaterial.h"

//----------------------------------------------------------------------------//
// Class declaration                                                          //
//----------------------------------------------------------------------------//

class CMaxMaterial : public CBaseMaterial
{
// member variables
protected:
	StdMat *m_pIStdMat;

	CalCoreMaterial *m_coreMaterial;
// constructors/destructor
public:
	CMaxMaterial();
	virtual ~CMaxMaterial();

// member functions
public:
	int GetMaxMapType(int mapId);
	void SaveMaps();
	void SaveMaps(LPCTSTR lpOthersPath, LPCTSTR lpCommonPath, LPCTSTR lpOthersPathServer = "", LPCTSTR lpCommonPathServer = "");
	bool Create(StdMat *pIStdMat, CalCoreMaterial* coreMaterial= NULL);
	void GetAmbientColor(float *pColor);
	void GetDiffuseColor(float *pColor);
	void GetEmissiveColor(float *pColor);
	int GetMapCount();
	std::string GetMapFilename(int mapId);
	std::string GetMapFilename(Texmap *pTexMap);
	std::string GetName();
	void GetSpecularColor(float *pColor);
	float GetShininess();

protected:
	void SaveMap(Texmap *pTexMap, CString& strFilename);
	bool SaveMap2(Texmap *pTexMap, CString& strFilename);
	void CorrectPathSlashes(CString& objectPath);
};

#endif

//----------------------------------------------------------------------------//
