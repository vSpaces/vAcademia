//****************************************************************************//
// corematerial.h                                                             //
// Copyright (C) 2001, 2002 Bruno 'Beosil' Heidelberger                       //
//****************************************************************************//
// This library is free software; you can redistribute it and/or modify it    //
// under the terms of the GNU Lesser General Public License as published by   //
// the Free Software Foundation; either version 2.1 of the License, or (at    //
// your option) any later version.                                            //
//****************************************************************************//

#ifndef CAL_COREMATERIAL_H
#define CAL_COREMATERIAL_H

//****************************************************************************//
// Includes                                                                   //
//****************************************************************************//

#include "global.h"

//****************************************************************************//
// Class declaration                                                          //
//****************************************************************************//

 /*****************************************************************************/
/** The core material class.
  *****************************************************************************/

class CAL3D_API CalCoreMaterial
{
// misc
public:
  /// The core material Color.
  typedef struct
  {
    unsigned char red;
    unsigned char green;
    unsigned char blue;
    unsigned char alpha;
  } Color;

  /// The core material Map.
  typedef struct _Map
  {
    Cal3DComString strFilename; 
	int			type;
    Cal::UserData userData;
	_Map(){ type=1;}
	_Map(const _Map& m){ *this=m;}
	void operator=(const _Map& m){ strFilename=m.strFilename; userData = m.userData; type = m.type;}
  } Map;

// member variables
protected:
  Color m_ambientColor;
  Color m_diffuseColor;
  Color m_specularColor;
  Color m_emissiveColor;
  float m_shininess;
  std::vector<Map> m_vectorMap;
  Cal::UserData m_userData;
  Cal3DComString	m_shaderPath;

// constructors/destructor
public:
	CalCoreMaterial();
	virtual ~CalCoreMaterial();
	
// member functions
public:
  bool create();
  void destroy();
  Color& getAmbientColor();
  Color& getDiffuseColor();
  Color& getSpecularColor();
  Color& getEmissiveColor();
  int getMapCount();
  Cal3DComString getMapFilename(int mapId);
  Cal::UserData getMapUserData(int mapId);
  float getShininess();
  Cal::UserData getUserData();
  std::vector<Map>& getVectorMap();
  bool reserve(int mapCount);
  void setAmbientColor(const Color& ambientColor);
  void setDiffuseColor(const Color& diffuseColor);
  void setEmissiveColor(const CalCoreMaterial::Color& emissiveColor);
  bool setMap(int mapId, const Map& map);
  bool setMapUserData(int mapId, Cal::UserData userData);
  void setShininess(float shininess);
  void setSpecularColor(const Color& specularColor);
  void setUserData(Cal::UserData userData);
  void setShaderPath(LPCTSTR alpShaderPath);
  LPCTSTR getShaderPath();
  CalCoreMaterial*	clone();

  void operator=(const CalCoreMaterial& m)
  {
	  m_ambientColor=m.m_ambientColor;
	  m_diffuseColor=m.m_diffuseColor;
	  m_specularColor=m.m_specularColor;
	  m_emissiveColor=m.m_emissiveColor;
	  m_shininess=m.m_shininess;
	  m_vectorMap.clear();
	  for( unsigned int i=0; i<m.m_vectorMap.size(); i++)
		  m_vectorMap.push_back(m.m_vectorMap[i]);
	  m_userData = m.m_userData;
  }
};

#endif

//****************************************************************************//
