//****************************************************************************//
// saver.cpp                                                                  //
// Copyright (C) 2001, 2002 Bruno 'Beosil' Heidelberger                       //
//****************************************************************************//
// This library is free software; you can redistribute it and/or modify it    //
// under the terms of the GNU Lesser General Public License as published by   //
// the Free Software Foundation; either version 2.1 of the License, or (at    //
// your option) any later version.                                            //
//****************************************************************************//

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

//****************************************************************************//
// Includes                                                                   //
//****************************************************************************//

#include "saver.h"
#include "error.h"
#include "vector.h"
#include "quaternion.h"
#include "coremodel.h"
#include "coreskeleton.h"
#include "corebone.h"
#include "coreanimation.h"
#include "coretrack.h"
#include "corekeyframe.h"
#include "coremesh.h"
#include "coresubmesh.h"
#include "corematerial.h"
#include "FaceDesc.h"
#include "calshadowdata.h"
#include "coreanimationflags.h"

 /*****************************************************************************/
/** Constructs the saver instance.
  *
  * This function is the default constructor of the saver instance.
  *****************************************************************************/

CalSaver::CalSaver()
{
}

 /*****************************************************************************/
/** Destructs the saver instance.
  *
  * This function is the destructor of the saver instance.
  *****************************************************************************/

CalSaver::~CalSaver()
{
}

 /*****************************************************************************/
/** Saves a core animation instance.
  *
  * This function saves a core animation instance to a file.
  *
  * @param strFilename The name of the file to save the core animation instance
  *                    to.
  * @param pCoreAnimation A pointer to the core animation instance that should
  *                       be saved.
  *
  * @return One of the following values:
  *         \li \b true if successful
  *         \li \b false if an error happend
  *****************************************************************************/

bool CalSaver::saveCoreAnimation(const std::string& strFilename, CalCoreAnimation *pCoreAnimation)
{
  // open the file
  std::ofstream file;
  file.open(strFilename.c_str(), std::ios::out | std::ios::binary);
  if(!file)
  {
    CalError::setLastError(CalError::FILE_CREATION_FAILED, __FILE__, __LINE__, strFilename);
    return false;
  }

  char ANIMFILEMAGIC[4];

  ANIMFILEMAGIC[3] = 0;
  if( !pCoreAnimation->m_bRelative && !pCoreAnimation->m_bSaveFirst && !pCoreAnimation->m_bSaveLast){
	  memcpy( (void*)&ANIMFILEMAGIC, (void*)&Cal::ANIMATION_FILE_MAGIC, sizeof(Cal::ANIMATION_FILE_MAGIC));
  }else{
	  //
	  if( !pCoreAnimation->m_bRelative) ANIMFILEMAGIC[0] = 'D'; else ANIMFILEMAGIC[0] = 'E';
	  if( !pCoreAnimation->m_bSaveFirst) ANIMFILEMAGIC[1] = 'D'; else ANIMFILEMAGIC[1] = 'E';
	  if( !pCoreAnimation->m_bSaveLast) ANIMFILEMAGIC[2] = 'D'; else ANIMFILEMAGIC[2] = 'E';
  }
  // write magic tag
  file.write((char *)&ANIMFILEMAGIC, sizeof(Cal::ANIMATION_FILE_MAGIC));
  if(!file)
  {
    CalError::setLastError(CalError::FILE_WRITING_FAILED, __FILE__, __LINE__, strFilename);
    return false;
  }

  // write version info
  file.write((char *)&Cal::CURRENT_FILE_VERSION, sizeof(Cal::CURRENT_FILE_VERSION));
  if(!file)
  {
    CalError::setLastError(CalError::FILE_WRITING_FAILED, __FILE__, __LINE__, strFilename);
    return false;
  }

  DWORD flags;
  if( Cal::CURRENT_FILE_VERSION >= 1006)
  {
	  flags = pCoreAnimation->getFlags();
	  file.write((char *)&flags, sizeof(flags));
  }

  if( Cal::CURRENT_FILE_VERSION >= 1007)
  {
	  if( flags & CAL3D_COREANIM_KEEPFRAME)
	  {
		  float*	len;
		  pCoreAnimation->getFlagData(CAL3D_COREANIM_KEEPFRAME, (LPVOID*)&len);
		  file.write((char *)len, sizeof(float));
	  }
  }

  // write the duration of the core animation
  float duration;
  duration = pCoreAnimation->getDuration();

  file.write((char *)&duration, 4);
  if(!file)
  {
    CalError::setLastError(CalError::FILE_WRITING_FAILED, __FILE__, __LINE__, strFilename);
    return false;
  }

  // get core track list
  std::list<CalCoreTrack *>& listCoreTrack = pCoreAnimation->getListCoreTrack();

  // write the number of tracks
  int trackCount;
  trackCount = listCoreTrack.size();

  file.write((char *)&trackCount, 4);
  if(!file)
  {
    CalError::setLastError(CalError::FILE_WRITING_FAILED, __FILE__, __LINE__, strFilename);
    return 0;
  }

  // write all core bones
  std::list<CalCoreTrack *>::iterator iteratorCoreTrack;
  for(iteratorCoreTrack = listCoreTrack.begin(); iteratorCoreTrack != listCoreTrack.end(); ++iteratorCoreTrack)
  {
    // save core track
    if(!saveCoreTrack(file, strFilename, *iteratorCoreTrack))
    {
      return false;
    }
  }

  // explicitly close the file
  file.close();

  return true;
}

 /*****************************************************************************/
/** Saves a core bone instance.
  *
  * This function saves a core bone instance to a file stream.
  *
  * @param file The file stream to save the core bone instance to.
  * @param strFilename The name of the file stream.
  * @param pCoreBone A pointer to the core bone instance that should be saved.
  *
  * @return One of the following values:
  *         \li \b true if successful
  *         \li \b false if an error happend
  *****************************************************************************/

bool CalSaver::saveCoreBones(std::ofstream& file, const std::string& strFilename, CalCoreBone *pCoreBone)
{
  if(!file)
  {
    CalError::setLastError(CalError::INVALID_HANDLE, __FILE__, __LINE__, strFilename);
    return false;
  }

  // write the name of the bone
  int len;
  Cal3DComString	boneName = pCoreBone->getName();
  len = boneName.GetLength() + 1;
  file.write((char *)&len, 4);
  file.write(pCoreBone->getName(), len);
  if(!file)
  {
    CalError::setLastError(CalError::FILE_WRITING_FAILED, __FILE__, __LINE__, strFilename);
    return false;
  }

  // write the translation of the bone
  const CalVector& translation = pCoreBone->getTranslation();
  file.write((char *)&translation[0], 4);
  file.write((char *)&translation[1], 4);
  file.write((char *)&translation[2], 4);

  // write the rotation of the bone
  const CalQuaternion& rotation = pCoreBone->getRotation();
  file.write((char *)&rotation[0], 4);
  file.write((char *)&rotation[1], 4);
  file.write((char *)&rotation[2], 4);
  file.write((char *)&rotation[3], 4);

  // write the translation of the bone
  const CalVector& translationBoneSpace = pCoreBone->getTranslationBoneSpace();
  file.write((char *)&translationBoneSpace[0], 4);
  file.write((char *)&translationBoneSpace[1], 4);
  file.write((char *)&translationBoneSpace[2], 4);

  // write the rotation of the bone
  const CalQuaternion& rotationBoneSpace = pCoreBone->getRotationBoneSpace();
  file.write((char *)&rotationBoneSpace[0], 4);
  file.write((char *)&rotationBoneSpace[1], 4);
  file.write((char *)&rotationBoneSpace[2], 4);
  file.write((char *)&rotationBoneSpace[3], 4);

  // write the parent bone id
  int parentId;
  parentId = pCoreBone->getParentId();
  file.write((char *)&parentId, 4);

  // check if an error happend
  if(!file)
  {
    CalError::setLastError(CalError::FILE_WRITING_FAILED, __FILE__, __LINE__, strFilename);
    return false;
  }

  // get children list
  std::list<int>& listChildId = pCoreBone->getListChildId();

  // write the number of children
  int childCount;
  childCount = listChildId.size();

  file.write((char *)&childCount, 4);
  if(!file)
  {
    CalError::setLastError(CalError::FILE_WRITING_FAILED, __FILE__, __LINE__, strFilename);
    return false;
  }
    
  // write all children ids
  std::list<int>::iterator iteratorChildId;
  for(iteratorChildId = listChildId.begin(); iteratorChildId != listChildId.end(); ++iteratorChildId)
  {
    // get child id
    int childId;
    childId = *iteratorChildId;

    // write child id
    file.write((char *)&childId, 4); 
    if(!file)
    {
      CalError::setLastError(CalError::FILE_WRITING_FAILED, __FILE__, __LINE__, strFilename);
      return false;
    }
  }

  return true;
}

 /*****************************************************************************/
/** Saves a core keyframe instance.
  *
  * This function saves a core keyframe instance to a file stream.
  *
  * @param file The file stream to save the core keyframe instance to.
  * @param strFilename The name of the file stream.
  * @param pCoreKeyframe A pointer to the core keyframe instance that should be
  *                      saved.
  *
  * @return One of the following values:
  *         \li \b true if successful
  *         \li \b false if an error happend
  *****************************************************************************/

bool CalSaver::saveCoreKeyframe(std::ofstream& file, const std::string& strFilename, CalCoreKeyframe *pCoreKeyframe)
{
  if(!file)
  {
    CalError::setLastError(CalError::INVALID_HANDLE, __FILE__, __LINE__, strFilename);
    return false;
  }

  // write the time of the keyframe
  float time;
  time = pCoreKeyframe->getTime();
  file.write((char *)&time, 4);

  // write the translation of the keyframe
  const CalVector& translation = pCoreKeyframe->getTranslation();
  file.write((char *)&translation[0], 4);
  file.write((char *)&translation[1], 4);
  file.write((char *)&translation[2], 4);

  // write the rotation of the keyframe
  const CalQuaternion& rotation = pCoreKeyframe->getRotation();
  file.write((char *)&rotation[0], 4);
  file.write((char *)&rotation[1], 4);
  file.write((char *)&rotation[2], 4);
  file.write((char *)&rotation[3], 4);

  // check if an error happend
  if(!file)
  {
    CalError::setLastError(CalError::FILE_WRITING_FAILED, __FILE__, __LINE__, strFilename);
    return false;
  }

  return true;
}

 /*****************************************************************************/
/** Saves a core material instance.
  *
  * This function saves a core material instance to a file.
  *
  * @param strFilename The name of the file to save the core material instance
  *                    to.
  * @param pCoreMaterial A pointer to the core material instance that should
  *                      be saved.
  *
  * @return One of the following values:
  *         \li \b true if successful
  *         \li \b false if an error happens
  *****************************************************************************/
bool CalSaver::saveCoreMaterialOld(const std::string& strFilename, CalCoreMaterial *pCoreMaterial)
{
	/*if ( strFilename.find(".xml")!=std::string::npos)
	{
	return saveCoreMaterialXML(strFilename, pCoreMaterial);
	}*/
	// open the file
	std::ofstream file;
	file.open(strFilename.c_str(), std::ios::out | std::ios::binary);
	if(!file)
	{
		CalError::setLastError(CalError::FILE_CREATION_FAILED, __FILE__, __LINE__, strFilename);
		return false;
	}

	// write magic tag
	file.write((char *)&Cal::MATERIAL_FILE_MAGIC, sizeof(Cal::MATERIAL_FILE_MAGIC));
	if(!file)
	{
		CalError::setLastError(CalError::FILE_WRITING_FAILED, __FILE__, __LINE__, strFilename);
		return false;
	}

	// write version info
	file.write((char *)&Cal::CURRENT_FILE_VERSION, sizeof(Cal::CURRENT_FILE_VERSION));
	if(!file)
	{
		CalError::setLastError(CalError::FILE_WRITING_FAILED, __FILE__, __LINE__, strFilename);
		return false;
	}

	// write the ambient color
	CalCoreMaterial::Color ambientColor;
	ambientColor = pCoreMaterial->getAmbientColor();
	file.write((char *)&ambientColor.red, 1);
	file.write((char *)&ambientColor.green, 1);
	file.write((char *)&ambientColor.blue, 1);
	file.write((char *)&ambientColor.alpha, 1);

	// write the diffuse color
	CalCoreMaterial::Color diffusetColor;
	diffusetColor = pCoreMaterial->getDiffuseColor();
	file.write((char *)&diffusetColor.red, 1);
	file.write((char *)&diffusetColor.green, 1);
	file.write((char *)&diffusetColor.blue, 1);
	file.write((char *)&diffusetColor.alpha, 1);

	// write the specular color
	CalCoreMaterial::Color specularColor;
	specularColor = pCoreMaterial->getSpecularColor();
	file.write((char *)&specularColor.red, 1);
	file.write((char *)&specularColor.green, 1);
	file.write((char *)&specularColor.blue, 1);
	file.write((char *)&specularColor.alpha, 1);

	// write the specular color
	CalCoreMaterial::Color emissiveColor;
	emissiveColor = pCoreMaterial->getEmissiveColor();
	file.write((char *)&emissiveColor.red, 1);
	file.write((char *)&emissiveColor.green, 1);
	file.write((char *)&emissiveColor.blue, 1);
	file.write((char *)&emissiveColor.alpha, 1);

	// write the shininess factor
	float shininess;
	shininess = pCoreMaterial->getShininess();
	file.write((char *)&shininess, 4);

	// check if an error happend
	if(!file)
	{
		CalError::setLastError(CalError::FILE_WRITING_FAILED, __FILE__, __LINE__, strFilename);
		return false;
	}

	// get the map vector
	std::vector<CalCoreMaterial::Map>& vectorMap = pCoreMaterial->getVectorMap();

	// write the number of maps
	int mapCount;
	mapCount = vectorMap.size();
	file.write((char *)&mapCount, 4);

	if(!file)
	{
		CalError::setLastError(CalError::FILE_WRITING_FAILED, __FILE__, __LINE__, strFilename);
		return false;
	}

	// write all maps
	int mapId;
	for(mapId = 0; mapId < mapCount; mapId++)
	{
		CalCoreMaterial::Map& map = vectorMap[mapId];

		// write the filename of the map
		int len;
		len = map.strFilename.GetLength() + 1;
		file.write((char *)&len, 4);
		file.write(map.strFilename.GetBuffer(), len-1);
		//file.write(".tga", 4);
		file.write("\0", 1);
		if (Cal::LIBRARY_VERSION >= 1002) {
			file.write((char *)&map.type, sizeof(map.type));
		}

		// check if an error happend
		if(!file)
		{
			CalError::setLastError(CalError::FILE_WRITING_FAILED, __FILE__, __LINE__, strFilename);
			return false;
		}
	}

	// explicitly close the file
	file.close();

	return true;
}

bool CalSaver::saveCoreMaterial(const std::string& strFilename, CalCoreMaterial *pCoreMaterial)
{
	/*if ( strFilename.find(".xml")!=std::string::npos)
	{
		return saveCoreMaterialXML(strFilename, pCoreMaterial);
	}*/
  // open the file
  FILE* file_ru = fopen(strFilename.c_str(), "wb");
  
/*  std::ofstream file;
  file.open(strFilename.c_str(), std::ios::out | std::ios::binary);
  if(!file)
  {
    CalError::setLastError(CalError::FILE_CREATION_FAILED, __FILE__, __LINE__, strFilename);
    return false;
  }
*/
  if(!file_ru)
  {
	  CalError::setLastError(CalError::FILE_CREATION_FAILED, __FILE__, __LINE__, strFilename);
	  return false;
  }

  // write magic tag
  //file.write((char *)&Cal::MATERIAL_FILE_MAGIC, sizeof(Cal::MATERIAL_FILE_MAGIC));
  //if(!file)
  //{
  //  CalError::setLastError(CalError::FILE_WRITING_FAILED, __FILE__, __LINE__, strFilename);
  //  return false;
  //}

  // write version info
  //file.write((char *)&Cal::CURRENT_FILE_VERSION, sizeof(Cal::CURRENT_FILE_VERSION));
  //if(!file)
  //{
  //  CalError::setLastError(CalError::FILE_WRITING_FAILED, __FILE__, __LINE__, strFilename);
  //  return false;
  //}

  // write the ambient color
  CalCoreMaterial::Color ambientColor;
  ambientColor = pCoreMaterial->getAmbientColor();
 /* file.write((char *)&ambientColor.red, 1);
  file.write((char *)&ambientColor.green, 1);
  file.write((char *)&ambientColor.blue, 1);
  file.write((char *)&ambientColor.alpha, 1);
*/
//	fwrite((char *)&ambientColor.red, 1, 1, file_ru);
//	fwrite((char *)&ambientColor.green, 1, 1, file_ru);
//	fwrite((char *)&ambientColor.blue, 1, 1, file_ru);
//	fwrite((char *)&ambientColor.alpha, 1, 1, file_ru);

	// амбиент и дифус в движке складываются, поэтому занулим амбиент
	ambientColor.red = 0;
	ambientColor.green = 0;
	ambientColor.blue = 0;
	ambientColor.alpha = 0;
  	fwrite((char *)&ambientColor.red, 1, 1, file_ru);
  	fwrite((char *)&ambientColor.green, 1, 1, file_ru);
  	fwrite((char *)&ambientColor.blue, 1, 1, file_ru);
  	fwrite((char *)&ambientColor.alpha, 1, 1, file_ru);

  // write the diffuse color
  CalCoreMaterial::Color diffusetColor;
  diffusetColor = pCoreMaterial->getDiffuseColor();
  /*file.write((char *)&diffusetColor.red, 1);
  file.write((char *)&diffusetColor.green, 1);
  file.write((char *)&diffusetColor.blue, 1);
  file.write((char *)&diffusetColor.alpha, 1);
*/
  fwrite((char *)&diffusetColor.red, 1, 1, file_ru);
  fwrite((char *)&diffusetColor.green, 1, 1, file_ru);
  fwrite((char *)&diffusetColor.blue, 1, 1, file_ru);
  fwrite((char *)&diffusetColor.alpha, 1, 1, file_ru);

  // write the specular color
  //CalCoreMaterial::Color specularColor;
  //specularColor = pCoreMaterial->getSpecularColor();
  //file.write((char *)&specularColor.red, 1);
  //file.write((char *)&specularColor.green, 1);
  //file.write((char *)&specularColor.blue, 1);
  //file.write((char *)&specularColor.alpha, 1);

  // write the specular color
  //CalCoreMaterial::Color emissiveColor;
  //emissiveColor = pCoreMaterial->getEmissiveColor();
  //file.write((char *)&emissiveColor.red, 1);
  //file.write((char *)&emissiveColor.green, 1);
  //file.write((char *)&emissiveColor.blue, 1);
  //file.write((char *)&emissiveColor.alpha, 1);

  // write the shininess factor
  //float shininess;
  //shininess = pCoreMaterial->getShininess();
  //file.write((char *)&shininess, 4);

  // check if an error happend
  /*if(!file)
  {
    CalError::setLastError(CalError::FILE_WRITING_FAILED, __FILE__, __LINE__, strFilename);
    return false;
  }*/

  if(!file_ru)
  {
	  CalError::setLastError(CalError::FILE_WRITING_FAILED, __FILE__, __LINE__, strFilename);
	  return false;
  }

  // get the map vector
  std::vector<CalCoreMaterial::Map>& vectorMap = pCoreMaterial->getVectorMap();

  // write the number of maps
  int mapCount;
  mapCount = vectorMap.size();
  //file.write((char *)&mapCount, 4);

  fwrite((char *)&mapCount, 4, 1, file_ru);

  /*if(!file)
  {
    CalError::setLastError(CalError::FILE_WRITING_FAILED, __FILE__, __LINE__, strFilename);
    return false;
  }*/

  if(!file_ru)
  {
	  CalError::setLastError(CalError::FILE_WRITING_FAILED, __FILE__, __LINE__, strFilename);
	  return false;
  }

  // write all maps
  int mapId;
  for(mapId = 0; mapId < mapCount; mapId++)
  {
    CalCoreMaterial::Map& map = vectorMap[mapId];

    // write the filename of the map
    int len;
    len = map.strFilename.GetLength() + 1;
    //file.write((char *)&len, 1);

	fwrite((char *)&len, 1, 1, file_ru);

    //file.write(map.strFilename.GetBuffer(), len-1);

	fwrite(map.strFilename.GetBuffer(), 1, len-1, file_ru);

	//file.write(".tga", 4);
	//file.write("\0", 1);

	fwrite("\0", 1, 1, file_ru);

	if (Cal::LIBRARY_VERSION >= 1002) {
		//file.write((char *)&map.type, sizeof(map.type));
		//file.write((char *)&map.type, 1);

		fwrite((char *)&map.type, 1, 1, file_ru);
	}

    // check if an error happend
    /*if(!file)
    {
      CalError::setLastError(CalError::FILE_WRITING_FAILED, __FILE__, __LINE__, strFilename);
      return false;
    }*/

	if(!file_ru)
	{
		CalError::setLastError(CalError::FILE_WRITING_FAILED, __FILE__, __LINE__, strFilename);
		return false;
	}
  }

  // explicitly close the file
  //file.close();

  fclose(file_ru);

  return true;
}

bool CalSaver::saveCoreMaterialXML(const std::string& strFilename, CalCoreMaterial *pCoreMaterial)
{
  // open the file
  std::ofstream file;
  file.open(strFilename.c_str(), std::ios::out);
  if(!file)
  {
    CalError::setLastError(CalError::FILE_CREATION_FAILED, __FILE__, __LINE__, strFilename);
    return false;
  }

  char	buffer[255];
  sprintf(buffer, "<material>\n");
  // write title
  file.write(buffer, strlen(buffer));

  CalCoreMaterial::Color ambientColor;
  ambientColor = pCoreMaterial->getAmbientColor();
  CalCoreMaterial::Color diffuseColor;
  diffuseColor = pCoreMaterial->getDiffuseColor();
  CalCoreMaterial::Color specularColor;
  specularColor = pCoreMaterial->getSpecularColor();
  CalCoreMaterial::Color emissiveColor;
  emissiveColor = pCoreMaterial->getEmissiveColor();
  float shininess;
  shininess = pCoreMaterial->getShininess();

  sprintf(buffer, "  <ambient r=\"%.2f\" g=\"%.2f\" b=\"%.2f\">\n", ambientColor.red/255.0f, ambientColor.green/255.0f, ambientColor.blue/255.0f);
  file.write(buffer, strlen(buffer));
  sprintf(buffer, "  <diffuse r=\"%.2f\" g=\"%.2f\" b=\"%.2f\" a=\"%.2f\">\n", diffuseColor.red/255.0f, diffuseColor.green/255.0f, diffuseColor.blue/255.0f, diffuseColor.alpha/255.0f);
  file.write(buffer, strlen(buffer));
  sprintf(buffer, "  <specular r=\"%.2f\" g=\"%.2f\" b=\"%.2f\">\n", specularColor.red/255.0f, specularColor.green/255.0f, specularColor.blue/255.0f);
  file.write(buffer, strlen(buffer));
  sprintf(buffer, "  <emissive r=\"%.2f\" g=\"%.2f\" b=\"%.2f\">\n", emissiveColor.red/255.0f, emissiveColor.green/255.0f, emissiveColor.blue/255.0f);
  file.write(buffer, strlen(buffer));

  // get the map vector
  std::vector<CalCoreMaterial::Map>& vectorMap = pCoreMaterial->getVectorMap();

  // write the number of maps
  int mapCount;
  mapCount = vectorMap.size();

  if( mapCount > 0)
  {
	  sprintf(buffer, "  <maps>\n");
	  file.write(buffer, strlen(buffer));
	  // write all maps
	  int mapId;
	  for(mapId = 0; mapId < mapCount; mapId++)
	  {
		CalCoreMaterial::Map& map = vectorMap[mapId];

		std::string	mapType = "";
		switch(map.type) {
		case Cal::DIFFUSE_TEXTURE:
			mapType = "diffuse";
			break;
		case Cal::SPECULAR_TEXTURE:
			mapType = "specular";
			break;
		case Cal::SPECULAR_TEXTURE+1:
			mapType = "lightmap";
			break;
		default:
			mapType = "diffuse";
		}

		sprintf(buffer, "    <%s src=\"%s\">\n", mapType.c_str(), map.strFilename.GetBuffer());
		file.write(buffer, strlen(buffer));
	  }
	  sprintf(buffer, "  </maps>\n");
	  file.write(buffer, strlen(buffer));
  }

  sprintf(buffer, "</material>");
  // write title
  file.write(buffer, strlen(buffer));
  // explicitly close the file
  file.close();

  return true;
}

 /*****************************************************************************/
/** Saves a core mesh instance.
  *
  * This function saves a core mesh instance to a file.
  *
  * @param strFilename The name of the file to save the core mesh instance to.
  * @param pCoreMesh A pointer to the core mesh instance that should be saved.
  *
  * @return One of the following values:
  *         \li \b true if successful
  *         \li \b false if an error happend
  *****************************************************************************/

bool CalSaver::saveCoreMesh(const std::string& strFilename, CalCoreMesh *pCoreMesh)
{
  // open the file
  std::ofstream file;
  file.open(strFilename.c_str(), std::ios::out | std::ios::binary);
  if(!file)
  {
    CalError::setLastError(CalError::FILE_CREATION_FAILED, __FILE__, __LINE__, strFilename);
    return false;
  }

  // write magic tag
  if( !pCoreMesh->NoSkel())
	file.write((char *)&Cal::MESH_FILE_MAGIC, sizeof(Cal::MESH_FILE_MAGIC));
  else
	  file.write((char *)&Cal::MESH_ALONE_FILE_MAGIC, sizeof(Cal::MESH_FILE_MAGIC));
  if(!file)
  {
    CalError::setLastError(CalError::FILE_WRITING_FAILED, __FILE__, __LINE__, strFilename);
    return false;
  }

  // write version info
  file.write((char *)&Cal::CURRENT_FILE_VERSION, sizeof(Cal::CURRENT_FILE_VERSION));
  if(!file)
  {
    CalError::setLastError(CalError::FILE_WRITING_FAILED, __FILE__, __LINE__, strFilename);
    return false;
  }

  if( Cal::CURRENT_FILE_VERSION >= 1005)
  {
	// write mesh translation and rotation
	CalVector trn = pCoreMesh->get_worldTranslation();
	CalQuaternion rtn = pCoreMesh->get_worldRotation();
	file.write((char *)&trn, sizeof(trn));
	file.write((char *)&rtn, sizeof(rtn));
  }

  // get the submesh vector
  std::vector<CalCoreSubmesh *>& vectorCoreSubmesh = pCoreMesh->getVectorCoreSubmesh();

  // write the number of submeshes
  int submeshCount;
  submeshCount = vectorCoreSubmesh.size();

  file.write((char *)&submeshCount, 4);
  if(!file)
  {
    CalError::setLastError(CalError::FILE_WRITING_FAILED, __FILE__, __LINE__, strFilename);
    return false;
  }

  // write all core submeshes
  int submeshId;
  for(submeshId = 0; submeshId < submeshCount; submeshId++)
  {
	  if( pCoreMesh->NoSkel())
		vectorCoreSubmesh[submeshId]->NoSkel( true);
    // write the core submesh
    if(!saveCoreSubmesh(file, strFilename, vectorCoreSubmesh[submeshId]))
    {
      return false;
    }
  }

  // save shadow data
  if( Cal::CURRENT_FILE_VERSION >= 1007)
  {
	  int edgesCount = 0;
	  if( pCoreMesh->getShadowData())
	  {
		  CalShadowData*	pShadowData = pCoreMesh->getShadowData();
		  std::vector<ShadowEdge>& edges = pShadowData->edges;
		  std::vector<ShadowEdge>::iterator	it;
		  edgesCount = edges.size();
		  file.write((char *)&edgesCount, 4);
		  for (it=edges.begin();it!=edges.end();it++)
		  {
			  file.write((char *)&*it, sizeof(ShadowEdge));
		  }
	  }else
	  {
		  file.write((char *)&edgesCount, 4);		  
	  }
  }

  // explicitly close the file
  file.close();

  return true;
}

 /*****************************************************************************/
/** Saves a core skeleton instance.
  *
  * This function saves a core skeleton instance to a file.
  *
  * @param strFilename The name of the file to save the core skeleton instance
  *                    to.
  * @param pCoreSkeleton A pointer to the core skeleton instance that should be
  *                      saved.
  *
  * @return One of the following values:
  *         \li \b true if successful
  *         \li \b false if an error happend
  *****************************************************************************/

bool CalSaver::saveCoreSkeleton(const std::string& strFilename, CalCoreSkeleton *pCoreSkeleton)
{
  // open the file
  std::ofstream file;
  file.open(strFilename.c_str(), std::ios::out | std::ios::binary);
  if(!file)
  {
    CalError::setLastError(CalError::FILE_CREATION_FAILED, __FILE__, __LINE__, strFilename);
    return false;
  }

  // write magic tag
  file.write((char *)&Cal::SKELETON_FILE_MAGIC, sizeof(Cal::SKELETON_FILE_MAGIC));
  if(!file)
  {
    CalError::setLastError(CalError::FILE_WRITING_FAILED, __FILE__, __LINE__, strFilename);
    return false;
  }

  // write version info
  file.write((char *)&Cal::CURRENT_FILE_VERSION, sizeof(Cal::CURRENT_FILE_VERSION));
  if(!file)
  {
    CalError::setLastError(CalError::FILE_WRITING_FAILED, __FILE__, __LINE__, strFilename);
    return false;
  }

  // write the number of bones
  int boneCount;
  boneCount = pCoreSkeleton->getVectorCoreBone().size();

  file.write((char *)&boneCount, 4);
  if(!file)
  {
    CalError::setLastError(CalError::FILE_WRITING_FAILED, __FILE__, __LINE__, strFilename);
    return false;
  }

  // write all core bones
  int boneId;
  for(boneId = 0; boneId < boneCount; boneId++)
  {
    // write the core bone
    if(!saveCoreBones(file, strFilename, pCoreSkeleton->getCoreBone(boneId)))
    {
      return false;
    }
  }

  // explicitly close the file
  file.close();

  return true;
}

 /*****************************************************************************/
/** Saves a core submesh instance.
  *
  * This function saves a core submesh instance to a file stream.
  *
  * @param file The file stream to save the core submesh instance to.
  * @param strFilename The name of the file stream.
  * @param pCoreSubmesh A pointer to the core submesh instance that should be
  *                     saved.
  *
  * @return One of the following values:
  *         \li \b true if successful
  *         \li \b false if an error happend
  *****************************************************************************/

bool CalSaver::saveCoreSubmesh(std::ofstream& file, const std::string& strFilename, CalCoreSubmesh *pCoreSubmesh)
{
  if(!file)
  {
    CalError::setLastError(CalError::INVALID_HANDLE, __FILE__, __LINE__, strFilename);
    return false;
  }

  // write the core material thread id
  int coreMaterialThreadId;
  coreMaterialThreadId = pCoreSubmesh->getCoreMaterialThreadId();
  file.write((char *)&coreMaterialThreadId, 4);

  // check if an error happend
  if(!file)
  {
    CalError::setLastError(CalError::FILE_WRITING_FAILED, __FILE__, __LINE__, strFilename);
    return false;
  }

  // get the vertex, face, physical property and spring vector
  std::vector<CalCoreSubmesh::Vertex>& vectorVertex = pCoreSubmesh->getVectorVertex();
  std::vector<CalCoreSubmesh::Face>& vectorFace = pCoreSubmesh->getVectorFace();
  std::vector<CalCoreSubmesh::PhysicalProperty>& vectorPhysicalProperty = pCoreSubmesh->getVectorPhysicalProperty();
  std::vector<CalCoreSubmesh::Spring>& vectorSpring = pCoreSubmesh->getVectorSpring();

  // write the number of vertices, faces, level-of-details and springs
  int vertexCount;
  vertexCount = vectorVertex.size();
  file.write((char *)&vertexCount, 4);

  int faceCount;
  faceCount = vectorFace.size();
  file.write((char *)&faceCount, 4);

  int lodCount;
  lodCount = pCoreSubmesh->getLodCount();
  file.write((char *)&lodCount, 4);

  int springCount;
  springCount = pCoreSubmesh->getSpringCount();
  file.write((char *)&springCount, 4);

  // get the texture coordinate vector vector
  std::vector<std::vector<CalCoreSubmesh::TxCoor> >& vectorvectorTextureCoordinate = pCoreSubmesh->getVectorVectorTextureCoordinate();

  // write the number of texture coordinates per vertex
  int textureCoordinateCount;
  textureCoordinateCount = vectorvectorTextureCoordinate.size();
  file.write((char *)&textureCoordinateCount, 4);

  // check if an error happend
  if(!file)
  {
    CalError::setLastError(CalError::FILE_WRITING_FAILED, __FILE__, __LINE__, strFilename);
    return false;
  }

  // write all vertices
  int vertexId;
  for(vertexId = 0; vertexId < vertexCount; vertexId++)
  {
    CalCoreSubmesh::Vertex& vertex = vectorVertex[vertexId];

    // write the vertex data
    file.write((char *)&vertex.position.x, 4);
    file.write((char *)&vertex.position.y, 4);
    file.write((char *)&vertex.position.z, 4);
    file.write((char *)&vertex.normal.x, 4);
    file.write((char *)&vertex.normal.y, 4);
    file.write((char *)&vertex.normal.z, 4);
    file.write((char *)&vertex.collapseId, 4);
    file.write((char *)&vertex.faceCollapseCount, 4);

    // write all texture coordinates of this vertex
    int textureCoordinateId;
    for(textureCoordinateId = 0; textureCoordinateId < textureCoordinateCount; textureCoordinateId++)
    {
      CalCoreSubmesh::TxCoor& textureCoordinate = vectorvectorTextureCoordinate[textureCoordinateId][vertexId];

      // write the influence data
      file.write((char *)&textureCoordinate.u, 4);
      file.write((char *)&textureCoordinate.v, 4);

      // check if an error happend
      if(!file)
      {
        CalError::setLastError(CalError::FILE_WRITING_FAILED, __FILE__, __LINE__, strFilename);
        return false;
      }
    }

	if( !pCoreSubmesh->NoSkel()){
		// write the number of influences
		int influenceCount;
		influenceCount = vertex.vectorInfluence.size();
		file.write((char *)&influenceCount, 4);

		// check if an error happend
		if(!file)
		{
		  CalError::setLastError(CalError::FILE_WRITING_FAILED, __FILE__, __LINE__, strFilename);
		  return false;
		}

		 // write all influences of this vertex
		int influenceId;
		for(influenceId = 0; influenceId < influenceCount; influenceId++)
		{
		  CalCoreSubmesh::Influence& influence = vertex.vectorInfluence[influenceId];

		  // write the influence data
		  file.write((char *)&influence.boneId, 4);
		  file.write((char *)&influence.weight, 4);

		  // check if an error happend
		  if(!file)
		  {
			CalError::setLastError(CalError::FILE_WRITING_FAILED, __FILE__, __LINE__, strFilename);
			return false;
		  }
		}
	}

    // save the physical property of the vertex if there are springs in the core submesh
    if(springCount > 0)
    {
      // write the physical property of this vertex if there are springs in the core submesh
      CalCoreSubmesh::PhysicalProperty& physicalProperty = vectorPhysicalProperty[vertexId];

      // write the physical property data
      file.write((char *)&physicalProperty.weight, 4);

      // check if an error happend
      if(!file)
      {
        CalError::setLastError(CalError::FILE_WRITING_FAILED, __FILE__, __LINE__, strFilename);
        return false;
      }
    }
  }

  // write all springs
  int springId;
  for(springId = 0; springId < springCount; springId++)
  {
    CalCoreSubmesh::Spring& spring = vectorSpring[springId];

    // write the spring data
    file.write((char *)&spring.vertexId[0], 4);
    file.write((char *)&spring.vertexId[1], 4);
    file.write((char *)&spring.springCoefficient, 4);
    file.write((char *)&spring.idleLength, 4);

    // check if an error happend
    if(!file)
    {
      CalError::setLastError(CalError::FILE_WRITING_FAILED, __FILE__, __LINE__, strFilename);
      return false;
    }
  }

  // write all faces
  int faceId;
  for(faceId = 0; faceId < faceCount; faceId++)
  {
    CalCoreSubmesh::Face& face = vectorFace[faceId];

    // write the face data
    file.write((char *)&face.vertexId[0], sizeof(face.vertexId[0]));
    file.write((char *)&face.vertexId[1], sizeof(face.vertexId[1]));
    file.write((char *)&face.vertexId[2], sizeof(face.vertexId[2]));

    // check if an error happend
    if(!file)
    {
      CalError::setLastError(CalError::FILE_WRITING_FAILED, __FILE__, __LINE__, strFilename);
      return false;
    }
  }

  return true;
}

 /*****************************************************************************/
/** Saves a core track instance.
  *
  * This function saves a core track instance to a file stream.
  *
  * @param file The file stream to save the core track instance to.
  * @param strFilename The name of the file stream.
  * @param pCoreTrack A pointer to the core track instance that should be saved.
  *
  * @return One of the following values:
  *         \li \b true if successful
  *         \li \b false if an error happend
  *****************************************************************************/

bool CalSaver::saveCoreTrack(std::ofstream& file, const std::string& strFilename, CalCoreTrack *pCoreTrack)
{
  if(!file)
  {
    CalError::setLastError(CalError::INVALID_HANDLE, __FILE__, __LINE__, strFilename);
    return false;
  }

  // write the bone id
  int coreBoneId;
  coreBoneId = pCoreTrack->getCoreBoneId();

  file.write((char *)&coreBoneId, 4);
  if(!file)
  {
    CalError::setLastError(CalError::FILE_WRITING_FAILED, __FILE__, __LINE__, strFilename);
    return false;
  }

  if( Cal::CURRENT_FILE_VERSION > 1000)
  {
	int	ns = pCoreTrack->getCoreBoneName().GetLength();
	file.write((char *)&ns, sizeof(int));
	if( ns > 0)	  file.write((char *)pCoreTrack->getCoreBoneName().GetBuffer(), ns);
  }

  if( Cal::CURRENT_FILE_VERSION > 1008)
  {
	  int	ns = pCoreTrack->getCoreBoneName().GetLength();
	  if( ns > 0)
	  {
		float	boneLength = pCoreTrack->m_coreCommonBoneLength;
		file.write((char *)&boneLength, sizeof(float));
	  }
  }

  // get core keyframe map
  std::map<float, CalCoreKeyframe *>& mapCoreKeyframe = pCoreTrack->getMapCoreKeyframe();

  // read the number of keyframes
  int keyframeCount;
  keyframeCount = mapCoreKeyframe.size();

  file.write((char *)&keyframeCount, 4);
  if(!file)
  {
    CalError::setLastError(CalError::FILE_WRITING_FAILED, __FILE__, __LINE__, strFilename);
    return false;
  }

  // save all core keyframes
  std::map<float, CalCoreKeyframe *>::iterator iteratorCoreKeyframe;
  for(iteratorCoreKeyframe = mapCoreKeyframe.begin(); iteratorCoreKeyframe != mapCoreKeyframe.end(); ++iteratorCoreKeyframe)
  {
    // save the core keyframe
    if(!saveCoreKeyframe(file, strFilename, iteratorCoreKeyframe->second))
    {
      return false;
    }
  }

  return true;
}

//****************************************************************************//

void CalSaver::saveFace(const std::string& strFilename, CFaceDesc *pFace)
{
// open the file
  std::ofstream file;
  file.open(strFilename.c_str(), std::ios::out | std::ios::binary);
  if(!file)
  {
    CalError::setLastError(CalError::FILE_CREATION_FAILED, __FILE__, __LINE__, strFilename);
    return;
  }

  // write magic tag
  file.write((char *)&Cal::FACE_FILE_MAGIC, sizeof(Cal::FACE_FILE_MAGIC));
  if(!file)
  {
    CalError::setLastError(CalError::FILE_WRITING_FAILED, __FILE__, __LINE__, strFilename);
    return;
  }

  file.write((char*)&(pFace->header), sizeof(pFace->header));

  WORD	wNextIndex=0;

  for(int ism=0; ism<pFace->header.dwSubmeshCount; ism++){

	  WORD iCorSubmeshVertCount=pFace->GetSubmeshVertCount(ism);
	  file.write((char*)&iCorSubmeshVertCount, sizeof(WORD));
	  if(iCorSubmeshVertCount == 0)	continue;

	  int i;
	  for(i=0; i<pFace->wIndexes.size(); i++){
		  wNextIndex = pFace->wIndexes[i]-1;
		  for(int j=0; j < pFace->aMaxIndexes.size(); j++){
			  if((wNextIndex == pFace->aMaxIndexes[j])&(ism == pFace->aSubMeshes[j])){
				WORD ind=pFace->aCalIndexes[j];
				file.write((char*)&ind, sizeof(WORD));
			  }
		  }
	  }

	  WORD	idnumperlex = pFace->wDeltas.size()/pFace->header.lNum;
	  for (int l=0; l < pFace->header.lNum; l++){
		  for(i=0; i<pFace->wIndexes.size(); i++){
			  wNextIndex = pFace->wIndexes[i]-1;
			  for(int j=0; j < pFace->aMaxIndexes.size(); j++){
				  if((wNextIndex == pFace->aMaxIndexes[j])&(ism == pFace->aSubMeshes[j])){
					D3DVECTOR v = pFace->wDeltas[i+l*idnumperlex];
					file.write((char*)&v, sizeof(D3DVECTOR));
				  }
			  }
		  }
	  }
  }
  file.write((char*)&(pFace->header.bFileName), sizeof(BYTE));
  if( pFace->header.bFileName > 0)	{
	file.write((char*)pFace->header.pFileName, pFace->header.bFileName);
	delete pFace->header.pFileName;
  }
  file.close();
}

bool CalSaver::saveConfig(const std::string& strFilename, CalCoreMesh *pCoreMesh)
{
  // получим имя файла конфигурации из имени файла модели
  std::string config;

  std::string::size_type pos;
  pos = strFilename.find_last_of(".");

  config = strFilename.substr(0, ++pos);
  config.append("cfg");

  // open the file
  std::ofstream file;
  file.open(config.c_str(), std::ios::out | std::ios::binary);
  if(!file)
  {
    CalError::setLastError(CalError::FILE_CREATION_FAILED, __FILE__, __LINE__, strFilename);
    return false;
  }

  // Сохраним данные о конфигурации модели //
  file.write( "scale=1.0\n\n", 11);				// масштаб модели
  if( !pCoreMesh->NoSkel())
	file.write( "skeleton=skel.csf\n\n", 19);	// если есть скелет, то пропишем его
  file.write( "mesh=mesh.cmf\n\n", 15);			// сама модель

  // get the submesh vector
  std::vector<CalCoreSubmesh *>& vectorCoreSubmesh = pCoreMesh->getVectorCoreSubmesh();
  int submeshCount;
  submeshCount = vectorCoreSubmesh.size();
  for( int i=1; i <= submeshCount; i++){										// материалы модели
	std::string	matname;
	matname.append("mtrl");

	char buf[10]; memset( (LPVOID)&buf, 0, 10);
	sprintf( (char*)&buf, "%i", i);

	matname.append( (char*)&buf);	// полное имя материала

	file.write( "material=", 9);
	file.write( (char*)matname.c_str(), matname.size());
	file.write( ".crf", 4);
	file.write( "\n\n", 2);
  }
  ///////////////////////////////////////////

  // explicitly close the file
  file.close();

  return true;
}


bool CalSaver::saveFAPU(const std::string& strFilename, CMpeg4FDP *face)
{
  // open the file
  std::ofstream file;
  file.open(strFilename.c_str(), std::ios::out | std::ios::binary);
  if(!file)
  {
    CalError::setLastError(CalError::FILE_CREATION_FAILED, __FILE__, __LINE__, strFilename);
    return false;
  }

  // write magic tag
  file.write((char *)&Cal::FAPU_FILE_MAGIC, sizeof(Cal::FAPU_FILE_MAGIC));
  if(!file)
  {
    CalError::setLastError(CalError::FILE_WRITING_FAILED, __FILE__, __LINE__, strFilename);
    return false;
  }

  // write version info
  file.write((char *)&Cal::CURRENT_FILE_VERSION, sizeof(Cal::CURRENT_FILE_VERSION));
  if(!file)
  {
    CalError::setLastError(CalError::FILE_WRITING_FAILED, __FILE__, __LINE__, strFilename);
    return false;
  }

  // write FAPU parameters
  file.write((char *)&face->IRISD0, (DWORD)&face->n_points - (DWORD)&face->IRISD0);

  // write the control points num
  file.write((char *)&face->cp_num, sizeof(int));

	// write the control points num
	for( int ig=0; ig<MAX_CONTROL_GROUP; ig++)
	{
		for( int in=0; in<MAX_CONTROL_POINTS_INGROUP; in++)
		{
			if( !face->n_points[ig][in])	continue;

			control_point*	point = face->n_points[ig][in];
			
			// write MPEG4 group_id, in_group_id and neutral vector
			file.write((char *)&point->group_id, sizeof(int));
			file.write((char *)&point->id, sizeof(int));
			file.write((char *)&point->v_neutral, sizeof(CalVector));

			if( Cal::CURRENT_FILE_VERSION > 1010)
			{
				file.write((char *)&point->mesh_id, sizeof(int));
				file.write((char *)&point->submesh_id, sizeof(int));
				file.write((char *)&point->point_id, sizeof(int));
			}

			if( ig == 8 && in == 1)
				int ijh=0;
			// write influence points num
			int isize = 0;
			int ip=0;
			for( ip=0; ip<point->points.size(); ip++)
				if( point->points[ip].weight != 0.0)
					isize++;
			file.write((char *)&isize, sizeof(int));

			// write all points
			for( ip=0; ip<point->points.size(); ip++)
			{
				if( point->points[ip].weight == 0.0)	continue;
				file.write((char *)&point->points[ip].mesh_id, sizeof(int));
				file.write((char *)&point->points[ip].submesh_id, sizeof(int));
				file.write((char *)&point->points[ip].id, sizeof(int));
				file.write((char *)&point->points[ip].weight, sizeof(float));
				file.write((char *)&point->points[ip].parent_gid, sizeof(int));
				file.write((char *)&point->points[ip].parent_id, sizeof(int));
			}
		}
	}

  // check if an error happend
  if(!file)
  {
    CalError::setLastError(CalError::FILE_WRITING_FAILED, __FILE__, __LINE__, strFilename);
    return false;
  }

  // explicitly close the file
  file.close();

  return true;
}

bool CalSaver::saveFAP(const std::string& strFilename, FACEExpression** pexprs, int count)
{
	  // open the file
  std::ofstream file;
  file.open(strFilename.c_str(), std::ios::out | std::ios::binary);
  if(!file)
  {
    CalError::setLastError(CalError::FILE_CREATION_FAILED, __FILE__, __LINE__, strFilename);
    return false;
  }

  // write magic tag
  file.write((char *)&Cal::FAP_FILE_MAGIC, sizeof(Cal::FAP_FILE_MAGIC));
  if(!file)
  {
    CalError::setLastError(CalError::FILE_WRITING_FAILED, __FILE__, __LINE__, strFilename);
    return false;
  }

  // write version info
  file.write((char *)&Cal::CURRENT_FILE_VERSION, sizeof(Cal::CURRENT_FILE_VERSION));
  if(!file)
  {
    CalError::setLastError(CalError::FILE_WRITING_FAILED, __FILE__, __LINE__, strFilename);
    return false;
  }

  // write FAPU parameters
  file.write((char *)&count, sizeof(int));

	// write the control points num
	for( int ig=0; ig<count; ig++)
	{
		FACEExpression* exprs = *pexprs;
		// write name
		int	fl = exprs->name.GetLength();
		file.write((char *)&fl, sizeof(int));
		file.write((char *)exprs->name.GetBuffer(), exprs->name.GetLength());
		
		// write expression type
		file.write((char *)&exprs->type, sizeof(FACEExpression::FACE_TYPE));

		// write FACEExpression::FT_EXPRESSION parameters
		if( exprs->type == FACEExpression::FT_EXPRESSION)
		{
			int	fs = exprs->expression->faps.size();
			file.write((char *)&fs, sizeof(int));
			for( int i=0; i<fs; i++)
			{
				fap_on_face	fof = exprs->expression->faps[i];
				file.write((char *)&fof.fap_id, sizeof(int));
				file.write((char *)&fof.base_shift, sizeof(CalVector));
				file.write((char *)&fof.base_unit, sizeof(float));
				file.write((char *)&fof.group_id, sizeof(int));
				file.write((char *)&fof.subgroup_id, sizeof(int));
			}
		}

		// write FACEExpression::FT_VISEME parameters
		if( exprs->type == FACEExpression::FT_VISEME)
		{
			assert( exprs->viseme);
			if( Cal::CURRENT_FILE_VERSION > 1009)
			{
				// write viseme FDP parameters
				file.write((char *)&exprs->viseme->units, sizeof(exprs->viseme->units));
				file.write((char *)&exprs->viseme->iris, sizeof(exprs->viseme->iris));
				file.write((char *)&exprs->viseme->es, sizeof(exprs->viseme->es));
				file.write((char *)&exprs->viseme->ens, sizeof(exprs->viseme->ens));
				file.write((char *)&exprs->viseme->mns, sizeof(exprs->viseme->mns));
				file.write((char *)&exprs->viseme->mw, sizeof(exprs->viseme->mw));
			}

			int	ps = exprs->viseme->points.size();
			file.write((char *)&ps, sizeof(int));
			for( int i=0; i<ps; i++)
			{
				viseme_cp	bcp = exprs->viseme->points[i];
				file.write((char *)&bcp, sizeof(bcp));
			}
		}

		pexprs++;
	}

  // check if an error happend
  if(!file)
  {
    CalError::setLastError(CalError::FILE_WRITING_FAILED, __FILE__, __LINE__, strFilename);
    return false;
  }

  // explicitly close the file
  file.close();

  return true;
}