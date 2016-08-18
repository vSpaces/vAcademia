//----------------------------------------------------------------------------//
// MaterialCandidate.h                                                        //
// Copyright (C) 2001, 2002 Bruno 'Beosil' Heidelberger                       //
//----------------------------------------------------------------------------//
// This program is free software; you can redistribute it and/or modify it    //
// under the terms of the GNU General Public License as published by the Free //
// Software Foundation; either version 2 of the License, or (at your option)  //
// any later version.                                                         //
//----------------------------------------------------------------------------//

#ifndef MATERIAL_CANDIDATE_H
#define MATERIAL_CANDIDATE_H

//----------------------------------------------------------------------------//
// Forward declarations                                                       //
//----------------------------------------------------------------------------//

class CBaseMaterial;

//----------------------------------------------------------------------------//
// Class declaration                                                          //
//----------------------------------------------------------------------------//

class CMaterialCandidate
{
// misc
public:
	typedef struct _Map
	{
		std::string strFilename;
		int			type;

		bool	operator<(const _Map& mp);
	} Map;

// member variables
protected:
	CBaseMaterial *m_pMaterial;
	std::vector<Map> m_vectorMap;

// constructors/destructor
public:
	CMaterialCandidate();
	virtual ~CMaterialCandidate();

// member functions
public:
	bool bFlipped;
	bool Create(CBaseMaterial *pMaterial);
	std::string GetName();
	void GetAmbientColor(float *pColor);
	void GetDiffuseColor(float *pColor);
#ifdef MAX3DS_BUILD
	void GetEmissiveColor(float *pColor);
#endif
	CBaseMaterial *GetMaterial();
	float GetShininess();
	void GetSpecularColor(float *pColor);
	std::vector<Map>& GetVectorMap();
};

#endif

//----------------------------------------------------------------------------//
