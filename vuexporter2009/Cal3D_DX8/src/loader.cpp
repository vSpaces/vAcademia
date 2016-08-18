//****************************************************************************//
// loader.cpp                                                                 //
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

#include "loader.h"
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
#include "facedesc.h"
#include "mpeg4fdp.h"
#include "memfile.h"
#include "coreanimationflags.h"

/*****************************************************************************/
/** Constructs the loader instance.
  *
  * This function is the default constructor of the loader instance.
  *****************************************************************************/

CalLoader::CalLoader()
{
}

 /*****************************************************************************/
/** Destructs the loader instance.
  *
  * This function is the destructor of the loader instance.
  *****************************************************************************/

CalLoader::~CalLoader()
{
}

 /*****************************************************************************/
/** Loads a core animation instance.
  *
  * This function loads a core animation instance from a file.
  *
  * @param strFilename The name of the file to load the core animation instance
  *                    from.
  *
  * @return One of the following values:
  *         \li a pointer to the core animation
  *         \li \b 0 if an error happend
  *****************************************************************************/

CalCoreAnimation *CalLoader::loadCoreAnimation(const std::string& strFilename)
{
  // open the file
  std::ifstream file;
  file.open(strFilename.c_str(), std::ios::in | std::ios::binary);
  if(!file)
  {
    CalError::setLastError(CalError::FILE_NOT_FOUND, __FILE__, __LINE__, strFilename);
    return 0;
  }

  // check if this is a valid file
  char magic[4];
  bool	m_bRelative=false;
  bool	m_bSaveFirst=false;
  bool	m_bSaveLast=false;

  file.read((char *)&magic, sizeof(magic));
  if(!file || (memcmp(&magic, Cal::ANIMATION_FILE_MAGIC, 4) != 0))
  {
	if(memcmp(&magic, Cal::RELANIMATION_FILE_MAGIC, 4) != 0)
	{
	  bool bError=false;
	  if( magic[0] == 'D') m_bRelative=false; else
		  if( magic[0] == 'E') m_bRelative=true; else
			  bError = true;
	  if( magic[1] == 'D') m_bSaveFirst=false; else
		  if( magic[1] == 'E') m_bSaveFirst=true; else
			  bError = true;
	  if( magic[2] == 'D') m_bSaveLast=false; else
		  if( magic[2] == 'E') m_bSaveLast=true; else
			  bError = true;
	  if( bError) return 0;
	}else
		m_bRelative = true;
  }

  // check if the version is compatible with the library
  unsigned int version;
  file.read((char *)&version, sizeof(version));
  if(!file || (version < Cal::EARLIEST_COMPATIBLE_FILE_VERSION) || (version > Cal::CURRENT_FILE_VERSION))
  {
    CalError::setLastError(CalError::INCOMPATIBLE_FILE_VERSION, __FILE__, __LINE__, strFilename);
    return 0;
  }

  // allocate a new core animation instance
  CalCoreAnimation *pCoreAnimation;
  pCoreAnimation = new CalCoreAnimation();
  if(pCoreAnimation == 0)
  {
    CalError::setLastError(CalError::MEMORY_ALLOCATION_FAILED, __FILE__, __LINE__);
    return 0;
  }

  // create the core animation instance
  if(!pCoreAnimation->create())
  {
    delete pCoreAnimation;
    return 0;
  }

  // get the duration of the core animation
  float duration;
  file.read((char *)&duration, 4);
  if(!file)
  {
    CalError::setLastError(CalError::INVALID_FILE_FORMAT, __FILE__, __LINE__, strFilename);
    pCoreAnimation->destroy();
    delete pCoreAnimation;
    return 0;
  }
  pCoreAnimation->setRelMode( m_bRelative);
  pCoreAnimation->saveFState( m_bSaveFirst);
  pCoreAnimation->saveLState( m_bSaveLast);

  // check for a valid duration
  if(duration <= 0.0f)
  {
    CalError::setLastError(CalError::INVALID_ANIMATION_DURATION, __FILE__, __LINE__, strFilename);
    pCoreAnimation->destroy();
    delete pCoreAnimation;
    return 0;
  }

  // set the duration in the core animation instance
  pCoreAnimation->setDuration(duration);

  // read the number of tracks
  int trackCount;
  file.read((char *)&trackCount, 4);
  if(!file || (trackCount <= 0))
  {
    CalError::setLastError(CalError::INVALID_FILE_FORMAT, __FILE__, __LINE__, strFilename);
    return 0;
  }

  // load all core bones
  int trackId;
  for(trackId = 0; trackId < trackCount; trackId++)
  {
    // load the core track
    CalCoreTrack *pCoreTrack;
    pCoreTrack = loadCoreTrack(file, strFilename);
    if(pCoreTrack == 0)
    {
      pCoreAnimation->destroy();
      delete pCoreAnimation;
      return 0;
    }

    // add the core track to the core animation instance
    pCoreAnimation->addCoreTrack(pCoreTrack);
  }

  // explicitly close the file
  file.close();

  return pCoreAnimation;
}

void CalLoader::deleteCoreAnimation(CalCoreAnimation* animation)
{
	if( animation)
	{
		delete animation;
	}
}

 /*****************************************************************************/
/** Loads a core bone instance.
  *
  * This function loads a core bone instance from a file stream.
  *
  * @param file The file stream to load the core bone instance from.
  * @param strFilename The name of the file stream.
  *
  * @return One of the following values:
  *         \li a pointer to the core bone
  *         \li \b 0 if an error happend
  *****************************************************************************/

CalCoreBone *CalLoader::loadCoreBones(std::ifstream& file, const std::string& strFilename)
{
  if(!file)
  {
    CalError::setLastError(CalError::INVALID_HANDLE, __FILE__, __LINE__, strFilename);
    return 0;
  }

  // get the name length of the bone
  int len;
  file.read((char *)&len, 4);
  if(len < 1)
  {
    CalError::setLastError(CalError::INVALID_FILE_FORMAT, __FILE__, __LINE__, strFilename);
    return 0;
  }

  // read the name of the bone
  char *strBuffer;
  strBuffer = new char[len];
  file.read(strBuffer, len);
  Cal3DComString strName;
  strName = strBuffer;
  delete [] strBuffer;

  // get the translation of the bone
  float tx, ty, tz;
  file.read((char *)&tx, 4);
  file.read((char *)&ty, 4);
  file.read((char *)&tz, 4);

  // get the rotation of the bone
  float rx, ry, rz, rw;
  file.read((char *)&rx, 4);
  file.read((char *)&ry, 4);
  file.read((char *)&rz, 4);
  file.read((char *)&rw, 4);

  // get the bone space translation of the bone
  float txBoneSpace, tyBoneSpace, tzBoneSpace;
  file.read((char *)&txBoneSpace, 4);
  file.read((char *)&tyBoneSpace, 4);
  file.read((char *)&tzBoneSpace, 4);

  // get the bone space rotation of the bone
  float rxBoneSpace, ryBoneSpace, rzBoneSpace, rwBoneSpace;
  file.read((char *)&rxBoneSpace, 4);
  file.read((char *)&ryBoneSpace, 4);
  file.read((char *)&rzBoneSpace, 4);
  file.read((char *)&rwBoneSpace, 4);

  // get the parent bone id
  int parentId;
  file.read((char *)&parentId, 4);

  // check if an error happend
  if(!file)
  {
    CalError::setLastError(CalError::INVALID_FILE_FORMAT, __FILE__, __LINE__, strFilename);
    return 0;
  }

  // allocate a new core bone instance
  CalCoreBone *pCoreBone;
  pCoreBone = new CalCoreBone();
  if(pCoreBone == 0)
  {
    CalError::setLastError(CalError::MEMORY_ALLOCATION_FAILED, __FILE__, __LINE__);
    return 0;
  }

  // create the core bone instance
  if(!pCoreBone->create(strName))
  {
    delete pCoreBone;
    return 0;
  }

  // set the parent of the bone
  pCoreBone->setParentId(parentId);

  // set all attributes of the bone
  pCoreBone->setTranslation(CalVector(tx, ty, tz));
  pCoreBone->setRotation(CalQuaternion(rx, ry, rz, rw));
  pCoreBone->setTranslationBoneSpace(CalVector(txBoneSpace, tyBoneSpace, tzBoneSpace));
  pCoreBone->setRotationBoneSpace(CalQuaternion(rxBoneSpace, ryBoneSpace, rzBoneSpace, rwBoneSpace));

  // read the number of children
  int childCount;
  file.read((char *)&childCount, 4);
  if(!file || (childCount < 0))
  {
    pCoreBone->destroy();
    delete pCoreBone;
    CalError::setLastError(CalError::INVALID_FILE_FORMAT, __FILE__, __LINE__, strFilename);
    return 0;
  }

  // load all children ids
  for(; childCount > 0; childCount--)
  {
    int childId;
    file.read((char *)&childId, 4);
    if(!file || (childId < 0))
    {
      pCoreBone->destroy();
      delete pCoreBone;
      CalError::setLastError(CalError::INVALID_FILE_FORMAT, __FILE__, __LINE__, strFilename);
      return 0;
    }

    pCoreBone->addChildId(childId);
  }

  return pCoreBone;
}

 /*****************************************************************************/
/** Loads a core keyframe instance.
  *
  * This function loads a core keyframe instance from a file stream.
  *
  * @param file The file stream to load the core keyframe instance from.
  * @param strFilename The name of the file stream.
  *
  * @return One of the following values:
  *         \li a pointer to the core keyframe
  *         \li \b 0 if an error happend
  *****************************************************************************/

CalCoreKeyframe *CalLoader::loadCoreKeyframe(std::ifstream& file, const std::string& strFilename)
{
  if(!file)
  {
    CalError::setLastError(CalError::INVALID_HANDLE, __FILE__, __LINE__, strFilename);
    return 0;
  }

  // get the time of the keyframe
  float time;
  file.read((char *)&time, 4);

  // get the translation of the bone
  float tx, ty, tz;
  file.read((char *)&tx, 4);
  file.read((char *)&ty, 4);
  file.read((char *)&tz, 4);

  // get the rotation of the bone
  float rx, ry, rz, rw;
  file.read((char *)&rx, 4);
  file.read((char *)&ry, 4);
  file.read((char *)&rz, 4);
  file.read((char *)&rw, 4);

  // check if an error happend
  if(!file)
  {
    CalError::setLastError(CalError::INVALID_FILE_FORMAT, __FILE__, __LINE__, strFilename);
    return 0;
  }

  // allocate a new core keyframe instance
  CalCoreKeyframe *pCoreKeyframe;
  pCoreKeyframe = new CalCoreKeyframe();
  if(pCoreKeyframe == 0)
  {
    CalError::setLastError(CalError::MEMORY_ALLOCATION_FAILED, __FILE__, __LINE__);
    return 0;
  }

  // create the core keyframe instance
  if(!pCoreKeyframe->create())
  {
    delete pCoreKeyframe;
    return 0;
  }

  // set all attributes of the keyframe
  pCoreKeyframe->setTime(time);
  pCoreKeyframe->setTranslation(CalVector(tx, ty, tz));
  pCoreKeyframe->setRotation(CalQuaternion(rx, ry, rz, rw));

  return pCoreKeyframe;
}

 /*****************************************************************************/
/** Loads a core material instance.
  *
  * This function loads a core material instance from a file.
  *
  * @param strFilename The name of the file to load the core material instance
  *                    from.
  *
  * @return One of the following values:
  *         \li a pointer to the core material
  *         \li \b 0 if an error happend
  *****************************************************************************/

CalCoreMaterial *CalLoader::loadCoreMaterial(const std::string& strFilename)
{
  // open the file
  std::ifstream file;
  file.open(strFilename.c_str(), std::ios::in | std::ios::binary);
  if(!file)
  {
    CalError::setLastError(CalError::FILE_NOT_FOUND, __FILE__, __LINE__, strFilename);
    return 0;
  }

  // check if this is a valid file
  unsigned int magic;
  file.read((char *)&magic, sizeof(magic));
  if(!file || (memcmp(&magic, Cal::MATERIAL_FILE_MAGIC, 4) != 0))
  {
    CalError::setLastError(CalError::INVALID_FILE_FORMAT, __FILE__, __LINE__, strFilename);
    return 0;
  }

  // check if the version is compatible with the library
  unsigned int version;
  file.read((char *)&version, sizeof(version));
  if(!file || (version < Cal::EARLIEST_COMPATIBLE_FILE_VERSION) || (version > Cal::CURRENT_FILE_VERSION))
  {
    CalError::setLastError(CalError::INCOMPATIBLE_FILE_VERSION, __FILE__, __LINE__, strFilename);
    return 0;
  }

  // allocate a new core material instance
  CalCoreMaterial *pCoreMaterial;
  pCoreMaterial = new CalCoreMaterial();
  if(pCoreMaterial == 0)
  {
    CalError::setLastError(CalError::MEMORY_ALLOCATION_FAILED, __FILE__, __LINE__);
    return 0;
  }

  // create the core material instance
  if(!pCoreMaterial->create())
  {
    delete pCoreMaterial;
    return 0;
  }

  // get the ambient color of the core material
  CalCoreMaterial::Color ambientColor;
  file.read((char *)&ambientColor.red, 1);
  file.read((char *)&ambientColor.green, 1);
  file.read((char *)&ambientColor.blue, 1);
  file.read((char *)&ambientColor.alpha, 1);

  // get the diffuse color of the core material
  CalCoreMaterial::Color diffuseColor;
  file.read((char *)&diffuseColor.red, 1);
  file.read((char *)&diffuseColor.green, 1);
  file.read((char *)&diffuseColor.blue, 1);
  file.read((char *)&diffuseColor.alpha, 1);

  // get the specular color of the core material
  CalCoreMaterial::Color specularColor;
  file.read((char *)&specularColor.red, 1);
  file.read((char *)&specularColor.green, 1);
  file.read((char *)&specularColor.blue, 1);
  file.read((char *)&specularColor.alpha, 1);

  // get the emissive color of the core material
  CalCoreMaterial::Color emissiveColor;
  if( version >= 1000)
  {
	  file.read((char *)&emissiveColor.red, 1);
	  file.read((char *)&emissiveColor.green, 1);
	  file.read((char *)&emissiveColor.blue, 1);
	  file.read((char *)&emissiveColor.alpha, 1);
  }

  // get the shininess factor of the core material
  float shininess;
  file.read((char *)&shininess, 4);

  // check if an error happend
  if(!file)
  {
    CalError::setLastError(CalError::INVALID_FILE_FORMAT, __FILE__, __LINE__, strFilename);
    pCoreMaterial->destroy();
    delete pCoreMaterial;
    return 0;
  }

  // set the colors and the shininess
  pCoreMaterial->setAmbientColor(ambientColor);
  pCoreMaterial->setDiffuseColor(diffuseColor);
  pCoreMaterial->setSpecularColor(specularColor);
	if( version >= 1000)
	pCoreMaterial->setEmissiveColor(emissiveColor);
  pCoreMaterial->setShininess(shininess);

  // read the number of maps
  int mapCount;
  file.read((char *)&mapCount, 4);
  if(!file)
  {
    CalError::setLastError(CalError::INVALID_FILE_FORMAT, __FILE__, __LINE__, strFilename);
    return 0;
  }

  // reserve memory for all the material data
  if(!pCoreMaterial->reserve(mapCount))
  {
    CalError::setLastError(CalError::MEMORY_ALLOCATION_FAILED, __FILE__, __LINE__, strFilename);
    pCoreMaterial->destroy();
    delete pCoreMaterial;
    return 0;
  }

  // load all maps
  int mapId;
  for(mapId = 0; mapId < mapCount; mapId++)
  {
    CalCoreMaterial::Map map;

    // get the filename length of the map
    int len;
    file.read((char *)&len, 4);
    if(len < 1)
    {
      CalError::setLastError(CalError::INVALID_FILE_FORMAT, __FILE__, __LINE__, strFilename);
      pCoreMaterial->destroy();
      delete pCoreMaterial;
      return 0;
    }

    // read the filename of the map
    char *strBuffer;
    strBuffer = new char[len];
    file.read(strBuffer, len);
    map.strFilename = strBuffer;
    delete [] strBuffer;

	if (version >= 1002)
		file.read((char *)&map.type, sizeof(map.type));

    // initialize the user data
    map.userData = 0;

    // check if an error happend
    if(!file)
    {
      CalError::setLastError(CalError::INVALID_FILE_FORMAT, __FILE__, __LINE__, strFilename);
      pCoreMaterial->destroy();
      delete pCoreMaterial;
      return 0;
    }

    // set map in the core material instance
    pCoreMaterial->setMap(mapId, map);
  }

  // explicitly close the file
  file.close();

  return pCoreMaterial;
}

 /*****************************************************************************/
/** Loads a core mesh instance.
  *
  * This function loads a core mesh instance from a file.
  *
  * @param strFilename The name of the file to load the core mesh instance from.
  *
  * @return One of the following values:
  *         \li a pointer to the core mesh
  *         \li \b 0 if an error happend
  *****************************************************************************/

CalCoreMesh *CalLoader::loadCoreMesh(const std::string& strFilename)
{
  // open the file
  std::ifstream file;
  file.open(strFilename.c_str(), std::ios::in | std::ios::binary);
  if(!file)
  {
    CalError::setLastError(CalError::FILE_NOT_FOUND, __FILE__, __LINE__, strFilename);
    return 0;
  }

  // allocate a new core mesh instance
  CalCoreMesh *pCoreMesh;
  pCoreMesh = new CalCoreMesh();
  if(pCoreMesh == 0)
  {
    CalError::setLastError(CalError::MEMORY_ALLOCATION_FAILED, __FILE__, __LINE__);
    return 0;
  }
  // create the core mesh instance
  if(!pCoreMesh->create())
  {
    delete pCoreMesh;
    return 0;
  }

  // check if this is a valid file
  unsigned int magic;
  file.read((char *)&magic, sizeof(magic));
  if(!file || (memcmp(&magic, Cal::MESH_FILE_MAGIC, 4) != 0))
  {
	if(!file || (memcmp(&magic, Cal::MESH_ALONE_FILE_MAGIC, 4) != 0))
	{
		CalError::setLastError(CalError::INVALID_FILE_FORMAT, __FILE__, __LINE__, strFilename);
		return 0;
	}else
		pCoreMesh->NoSkel( true);
  }

  // check if the version is compatible with the library
  unsigned int version;
  file.read((char *)&version, sizeof(version));
  if(!file || (version < Cal::EARLIEST_COMPATIBLE_FILE_VERSION) || (version > Cal::CURRENT_FILE_VERSION))
  {
    CalError::setLastError(CalError::INCOMPATIBLE_FILE_VERSION, __FILE__, __LINE__, strFilename);
    return 0;
  }

  // get the number of submeshes
  int submeshCount;
  file.read((char *)&submeshCount, 4);

  // check if an error happend
  if(!file)
  {
    CalError::setLastError(CalError::INVALID_FILE_FORMAT, __FILE__, __LINE__, strFilename);
    return 0;
  }

  // load all core submeshes
  int submeshId;
  for(submeshId = 0; submeshId < submeshCount; submeshId++)
  {
    // load the core submesh
    CalCoreSubmesh *pCoreSubmesh;
    pCoreSubmesh = loadCoreSubmesh(file, strFilename, pCoreMesh->NoSkel());

    if(pCoreSubmesh == 0)
    {
      pCoreMesh->destroy();
      delete pCoreMesh;
      return 0;
    }

	if( pCoreMesh->NoSkel())
		pCoreSubmesh->NoSkel( true);

    // add the core submesh to the core mesh instance
    pCoreMesh->addCoreSubmesh(pCoreSubmesh);
  }

  // explicitly close the file
  file.close();

  return pCoreMesh;
}

 /*****************************************************************************/
/** Loads a core skeleton instance.
  *
  * This function loads a core skeleton instance from a file.
  *
  * @param strFilename The name of the file to load the core skeleton instance
  *                    from.
  *
  * @return One of the following values:
  *         \li a pointer to the core skeleton
  *         \li \b 0 if an error happend
  *****************************************************************************/

CalCoreSkeleton *CalLoader::loadCoreSkeleton(const std::string& strFilename)
{
  // open the file
  std::ifstream file;
  file.open(strFilename.c_str(), std::ios::in | std::ios::binary);
  if(!file)
  {
    CalError::setLastError(CalError::FILE_NOT_FOUND, __FILE__, __LINE__, strFilename);
    return 0;
  }

  // check if this is a valid file
  unsigned int magic;
  file.read((char *)&magic, sizeof(magic));
  if(!file || (memcmp(&magic, Cal::SKELETON_FILE_MAGIC, 4) != 0))
  {
    CalError::setLastError(CalError::INVALID_FILE_FORMAT, __FILE__, __LINE__, strFilename);
    return 0;
  }

  // check if the version is compatible with the library
  unsigned int version;
  file.read((char *)&version, sizeof(version));
  if(!file || (version < Cal::EARLIEST_COMPATIBLE_FILE_VERSION) || (version > Cal::CURRENT_FILE_VERSION))
  {
    CalError::setLastError(CalError::INCOMPATIBLE_FILE_VERSION, __FILE__, __LINE__, strFilename);
    return 0;
  }

  // read the number of bones
  int boneCount;
  file.read((char *)&boneCount, 4);
  if(!file || (boneCount <= 0))
  {
    CalError::setLastError(CalError::INVALID_FILE_FORMAT, __FILE__, __LINE__, strFilename);
    return 0;
  }

  // allocate a new core skeleton instance
  CalCoreSkeleton *pCoreSkeleton;
  pCoreSkeleton = new CalCoreSkeleton();
  if(pCoreSkeleton == 0)
  {
    CalError::setLastError(CalError::MEMORY_ALLOCATION_FAILED, __FILE__, __LINE__);
    return 0;
  }

  // create the core skeleton instance
  if(!pCoreSkeleton->create())
  {
    delete pCoreSkeleton;
    return 0;
  }

  // load all core bones
  int boneId;
  for(boneId = 0; boneId < boneCount; boneId++)
  {
    // load the core bone
    CalCoreBone *pCoreBone;
    pCoreBone = loadCoreBones(file, strFilename);
    if(pCoreBone == 0)
    {
      pCoreSkeleton->destroy();
      delete pCoreSkeleton;
      return 0;
    }

    // set the core skeleton of the core bone instance
    pCoreBone->setCoreSkeleton(pCoreSkeleton);

    // add the core bone to the core skeleton instance
    pCoreSkeleton->addCoreBone(pCoreBone);
  }

  // explicitly close the file
  file.close();

  // calculate state of the core skeleton
  pCoreSkeleton->calculateState();

  return pCoreSkeleton;
}

 /*****************************************************************************/
/** Loads a core submesh instance.
  *
  * This function loads a core submesh instance from a file stream.
  *
  * @param file The file stream to load the core submesh instance from.
  * @param strFilename The name of the file stream.
  *
  * @return One of the following values:
  *         \li a pointer to the core submesh
  *         \li \b 0 if an error happend
  *****************************************************************************/

CalCoreSubmesh *CalLoader::loadCoreSubmesh(std::ifstream& file, const std::string& strFilename, bool abNoSkel)
{
  if(!file)
  {
    CalError::setLastError(CalError::INVALID_HANDLE, __FILE__, __LINE__, strFilename);
    return 0;
  }

  // get the material thread id of the submesh
  int coreMaterialThreadId;
  file.read((char *)&coreMaterialThreadId, 4);

  // get the number of vertices, faces, level-of-details and springs
  int vertexCount;
  file.read((char *)&vertexCount, 4);
  int faceCount;
  file.read((char *)&faceCount, 4);
  int lodCount;
  file.read((char *)&lodCount, 4);
  int springCount;
  file.read((char *)&springCount, 4);

  // get the number of texture coordinates per vertex
  int textureCoordinateCount;
  file.read((char *)&textureCoordinateCount, 4);

  // check if an error happend
  if(!file)
  {
    CalError::setLastError(CalError::INVALID_FILE_FORMAT, __FILE__, __LINE__, strFilename);
    return 0;
  }

  // allocate a new core submesh instance
  CalCoreSubmesh *pCoreSubmesh;
  pCoreSubmesh = new CalCoreSubmesh();
  if(pCoreSubmesh == 0)
  {
    CalError::setLastError(CalError::MEMORY_ALLOCATION_FAILED, __FILE__, __LINE__);
    return 0;
  }

  // create the core submesh instance
  if(!pCoreSubmesh->create())
  {
    delete pCoreSubmesh;
    return 0;
  }

  // set the LOD step count
  pCoreSubmesh->setLodCount(lodCount);

  // set the core material id
  pCoreSubmesh->setCoreMaterialThreadId(coreMaterialThreadId);

  // reserve memory for all the submesh data
  if(!pCoreSubmesh->reserve(vertexCount, textureCoordinateCount, faceCount, springCount))
  {
    CalError::setLastError(CalError::MEMORY_ALLOCATION_FAILED, __FILE__, __LINE__, strFilename);
    pCoreSubmesh->destroy();
    delete pCoreSubmesh;
    return 0;
  }

  // load all vertices and their influences
  int vertexId;
  for(vertexId = 0; vertexId < vertexCount; vertexId++)
  {
    CalCoreSubmesh::Vertex vertex;

    // load data of the vertex
    file.read((char *)&vertex.position.x, 4);
    file.read((char *)&vertex.position.y, 4);
    file.read((char *)&vertex.position.z, 4);
    file.read((char *)&vertex.normal.x, 4);
    file.read((char *)&vertex.normal.y, 4);
    file.read((char *)&vertex.normal.z, 4);
    file.read((char *)&vertex.collapseId, 4);
    file.read((char *)&vertex.faceCollapseCount, 4);

    // check if an error happend
    if(!file)
    {
      CalError::setLastError(CalError::INVALID_FILE_FORMAT, __FILE__, __LINE__, strFilename);
      pCoreSubmesh->destroy();
      delete pCoreSubmesh;
      return 0;
    }

    // load all texture coordinates of the vertex
    int textureCoordinateId;
    for(textureCoordinateId = 0; textureCoordinateId < textureCoordinateCount; textureCoordinateId++)
    {
      CalCoreSubmesh::TxCoor textureCoordinate;

      // load data of the influence
      file.read((char *)&textureCoordinate.u, 4);
      file.read((char *)&textureCoordinate.v, 4);

      // check if an error happend
      if(!file)
      {
        CalError::setLastError(CalError::INVALID_FILE_FORMAT, __FILE__, __LINE__, strFilename);
        pCoreSubmesh->destroy();
        delete pCoreSubmesh;
        return 0;
      }

      // set texture coordinate in the core submesh instance
      pCoreSubmesh->setTextureCoordinate(vertexId, textureCoordinateId, textureCoordinate);
    }


	if( !abNoSkel){
		// get the number of influences
		int influenceCount;
		file.read((char *)&influenceCount, 4);

		// check if an error happend
		if(!file)
		{
		  CalError::setLastError(CalError::INVALID_FILE_FORMAT, __FILE__, __LINE__, strFilename);
		  pCoreSubmesh->destroy();
		  delete pCoreSubmesh;
		  return 0;
		}

		// reserve memory for the influences in the vertex
		vertex.vectorInfluence.reserve(influenceCount);
		vertex.vectorInfluence.resize(influenceCount);

		// load all influences of the vertex
		int influenceId;
		for(influenceId = 0; influenceId < influenceCount; influenceId++)
		{
		  // load data of the influence
		  file.read((char *)&vertex.vectorInfluence[influenceId].boneId, 4);
		  file.read((char *)&vertex.vectorInfluence[influenceId].weight, 4);

		  // check if an error happend
		  if(!file)
		  {
			CalError::setLastError(CalError::INVALID_FILE_FORMAT, __FILE__, __LINE__, strFilename);
			pCoreSubmesh->destroy();
			delete pCoreSubmesh;
			return 0;
		  }
		}
	}

    // set vertex in the core submesh instance
    pCoreSubmesh->setVertex(vertexId, vertex);

    // load the physical property of the vertex if there are springs in the core submesh
    if(springCount > 0)
    {
      CalCoreSubmesh::PhysicalProperty physicalProperty;

      // load data of the physical property
      file.read((char *)&physicalProperty.weight, 4);

      // check if an error happend
      if(!file)
      {
        CalError::setLastError(CalError::INVALID_FILE_FORMAT, __FILE__, __LINE__, strFilename);
        pCoreSubmesh->destroy();
        delete pCoreSubmesh;
        return 0;
      }

      // set the physical property in the core submesh instance
      pCoreSubmesh->setPhysicalProperty(vertexId, physicalProperty);
    }
  }

  // load all springs
  int springId;
  for(springId = 0; springId < springCount; springId++)
  {
    CalCoreSubmesh::Spring spring;

    // load data of the spring
    file.read((char *)&spring.vertexId[0], 4);
    file.read((char *)&spring.vertexId[1], 4);
    file.read((char *)&spring.springCoefficient, 4);
    file.read((char *)&spring.idleLength, 4);

    // check if an error happend
    if(!file)
    {
      CalError::setLastError(CalError::INVALID_FILE_FORMAT, __FILE__, __LINE__, strFilename);
      pCoreSubmesh->destroy();
      delete pCoreSubmesh;
      return 0;
    }

    // set spring in the core submesh instance
    pCoreSubmesh->setSpring(springId, spring);
  }

  // load all faces
  int faceId;
  for(faceId = 0; faceId < faceCount; faceId++)
  {
    CalCoreSubmesh::Face face;

    // load data of the face
    file.read((char *)&face.vertexId[0], sizeof(face.vertexId[0]));
    file.read((char *)&face.vertexId[1], sizeof(face.vertexId[1]));
    file.read((char *)&face.vertexId[2], sizeof(face.vertexId[2]));


    // check if an error happend
    if(!file)
    {
      CalError::setLastError(CalError::INVALID_FILE_FORMAT, __FILE__, __LINE__, strFilename);
      pCoreSubmesh->destroy();
      delete pCoreSubmesh;
      return 0;
    }

    // set face in the core submesh instance
    pCoreSubmesh->setFace(faceId, face);
  }

  return pCoreSubmesh;
}

 /*****************************************************************************/
/** Loads a core track instance.
  *
  * This function loads a core track instance from a file stream.
  *
  * @param file The file stream to load the core track instance from.
  * @param strFilename The name of the file stream.
  *
  * @return One of the following values:
  *         \li a pointer to the core track
  *         \li \b 0 if an error happend
  *****************************************************************************/

CalCoreTrack *CalLoader::loadCoreTrack(std::ifstream& file, const std::string& strFilename)
{
  if(!file)
  {
    CalError::setLastError(CalError::INVALID_HANDLE, __FILE__, __LINE__, strFilename);
    return 0;
  }

  // read the bone id
  int coreBoneId;
  file.read((char *)&coreBoneId, 4);
  if(!file || (coreBoneId < 0 && Cal::CURRENT_FILE_VERSION < 1003))
  {
    CalError::setLastError(CalError::INVALID_FILE_FORMAT, __FILE__, __LINE__, strFilename);
    return 0;
  }

  // allocate a new core track instance
  CalCoreTrack *pCoreTrack;
  pCoreTrack = new CalCoreTrack();
  if(pCoreTrack == 0)
  {
    CalError::setLastError(CalError::MEMORY_ALLOCATION_FAILED, __FILE__, __LINE__);
    return 0;
  }

  if( file_version > 1000)
  {
	int	ns;
	file.read((char *)&ns, sizeof(int));
	Cal3DComString	name;
	if( ns > 0)
	{
	  file.read((char *)name.GetBufferSetLength(ns), ns);
	  pCoreTrack->setCoreBoneName(name.GetBuffer());
	}	
  }

  // create the core track instance
  if(!pCoreTrack->create())
  {
    delete pCoreTrack;
    return 0;
  }

  // link the core track to the appropriate core bone instance
  pCoreTrack->setCoreBoneId(coreBoneId);

  // read the number of keyframes
  int keyframeCount;
  file.read((char *)&keyframeCount, 4);
  if(!file || (keyframeCount <= 0))
  {
    CalError::setLastError(CalError::INVALID_FILE_FORMAT, __FILE__, __LINE__, strFilename);
    return 0;
  }

  // load all core keyframes
  int keyframeId;
  for(keyframeId = 0; keyframeId < keyframeCount; keyframeId++)
  {
    // load the core keyframe
    CalCoreKeyframe *pCoreKeyframe;
    pCoreKeyframe = loadCoreKeyframe(file, strFilename);
    if(pCoreKeyframe == 0)
    {
      pCoreTrack->destroy();
      delete pCoreTrack;
      return 0;
    }

    // add the core keyframe to the core track instance
    pCoreTrack->addCoreKeyframe(pCoreKeyframe);
  }

  return pCoreTrack;
}

CFace* CalLoader::loadFace(const std::string& strFilename)
{
	CFace	*pFace = NULL;

	// open the file
	std::ifstream file;
	file.open(strFilename.c_str(), std::ios::in | std::ios::binary);
	if(!file){
		CalError::setLastError(CalError::FILE_NOT_FOUND, __FILE__, __LINE__, strFilename);
		return 0;
	}

	// check if this is a valid file
	unsigned int magic;
	file.read((char *)&magic, sizeof(magic));
	if(!file || (memcmp(&magic, Cal::FACE_FILE_MAGIC, 4) != 0)){
		CalError::setLastError(CalError::INVALID_FILE_FORMAT, __FILE__, __LINE__, strFilename);
		return 0;
	}

	pFace = new CFace();
	file.read((char *)&(pFace->header), sizeof(pFace->header));

	DWORD dwRealSmc=0;
	pFace->aSubmeshNum.reserve(pFace->header.dwSubmeshCount);
	pFace->aVertNum.reserve(pFace->header.dwSubmeshCount);
	pFace->aVertIndexes.reserve(pFace->header.dwSubmeshCount);
	pFace->aDeltas.reserve(pFace->header.dwSubmeshCount);
	//pFace->header.

	for(int ism=0; ism<pFace->header.dwSubmeshCount; ism++){
		WORD	dwSubmeshVertNum=0;
		file.read((char *)&(dwSubmeshVertNum), sizeof(WORD));
		if(dwSubmeshVertNum == 0) continue;
		
		pFace->aSubmeshNum.push_back(ism);
		dwRealSmc++;

		WORD*	paVertIndexes = new WORD[dwSubmeshVertNum];
		D3DXVECTOR3*	paDeltas= new D3DXVECTOR3[dwSubmeshVertNum*pFace->header.lNum];

		file.read((char *)paVertIndexes, sizeof(WORD)*dwSubmeshVertNum);
		file.read((char *)paDeltas, sizeof(D3DXVECTOR3)*dwSubmeshVertNum*pFace->header.lNum);

		pFace->aVertNum.push_back(dwSubmeshVertNum);
		pFace->aVertIndexes.push_back(paVertIndexes);
		pFace->aDeltas.push_back(paDeltas);
	}
	pFace->header.dwSubmeshCount = dwRealSmc;

	file.read((char*)&(pFace->header.bFileName), sizeof(BYTE));
	if( pFace->header.bFileName > 0){
		pFace->header.pFileName = (char*)malloc(pFace->header.bFileName+1);
		ZeroMemory(pFace->header.pFileName, pFace->header.bFileName+1);
		file.read((char*)pFace->header.pFileName, pFace->header.bFileName);
	}
	return pFace;
}
//****************************************************************************//


// HANDLE functions

 /*****************************************************************************/
/** Loads a core animation instance.
  *
  * This function loads a core animation instance from a file.
  *
  * @param strFilename The name of the file to load the core animation instance
  *                    from.
  *
  * @return One of the following values:
  *         \li a pointer to the core animation
  *         \li \b 0 if an error happend
  *****************************************************************************/

CalCoreAnimation *CalLoader::loadCoreAnimation(HANDLE hFile)
{
// check if this is a valid file
  char magic[4];
  bool	m_bRelative=false;
  bool	m_bSaveFirst=false;
  bool	m_bSaveLast=false;

  ReadFile(hFile, (void*)&magic, sizeof(magic), &nBytesRead, NULL);
  int i=GetLastError();
  if(!hFile || (memcmp(&magic, Cal::ANIMATION_FILE_MAGIC, 4) != 0))
  {
	if(memcmp(&magic, Cal::RELANIMATION_FILE_MAGIC, 4) != 0)
	{
	  bool bError=false;
	  if( magic[1] == 'D') m_bRelative=false; else
		  if( magic[0] == 'E') m_bRelative=true; else
			  bError = true;
	  if( magic[0] == 'D') m_bSaveFirst=false; else
		  if( magic[1] == 'E') m_bSaveFirst=true; else
			  bError = true;
	  if( magic[2] == 'D') m_bSaveLast=false; else
		  if( magic[2] == 'E') m_bSaveLast=true; else
			  bError = true;
	  if( bError) return 0;
	}else
		m_bRelative = true;
  }

  // check if the version is compatible with the library
  unsigned int version;
  ReadFile(hFile, (void*)&version, sizeof(version), &nBytesRead, NULL);
  if(!hFile || (version < Cal::EARLIEST_COMPATIBLE_FILE_VERSION) || (version > Cal::CURRENT_FILE_VERSION))
  {
    //CalError::setLastError(CalError::INCOMPATIBLE_FILE_VERSION, __FILE__, __LINE__, strFilename);
    return 0;
  }

  // allocate a new core animation instance
  CalCoreAnimation *pCoreAnimation;
  pCoreAnimation = new CalCoreAnimation();
  if(pCoreAnimation == 0)
  {
    //CalError::setLastError(CalError::MEMORY_ALLOCATION_FAILED, __FILE__, __LINE__);
    return 0;
  }

  // create the core animation instance
  if(!pCoreAnimation->create())
  {
    delete pCoreAnimation;
    return 0;
  }

  pCoreAnimation->setRelMode( m_bRelative);
  pCoreAnimation->saveFState( m_bSaveFirst);
  pCoreAnimation->saveLState( m_bSaveLast);

  // get the duration of the core animation
  float duration;
  ReadFile(hFile, (void*)&duration, 4, &nBytesRead, NULL);
  if(!hFile)
  {
    //CalError::setLastError(CalError::INVALID_FILE_FORMAT, __FILE__, __LINE__, strFilename);
    pCoreAnimation->destroy();
    delete pCoreAnimation;
    return 0;
  }

  // check for a valid duration
  if(duration <= 0.0f)
  {
    //CalError::setLastError(CalError::INVALID_ANIMATION_DURATION, __FILE__, __LINE__, strFilename);
    pCoreAnimation->destroy();
    delete pCoreAnimation;
    return 0;
  }

  // set the duration in the core animation instance
  pCoreAnimation->setDuration(duration);

  // read the number of tracks
  int trackCount;
  ReadFile(hFile, (void*)&trackCount, 4, &nBytesRead, NULL);
  if(!hFile || (trackCount <= 0))
  {
    //CalError::setLastError(CalError::INVALID_FILE_FORMAT, __FILE__, __LINE__, strFilename);
    return 0;
  }

  // load all core bones
  int trackId;
  for(trackId = 0; trackId < trackCount; trackId++)
  {
    // load the core track
    CalCoreTrack *pCoreTrack;
    pCoreTrack = loadCoreTrack(hFile);
    if(pCoreTrack == 0)
    {
      pCoreAnimation->destroy();
      delete pCoreAnimation;
      return 0;
    }

    // add the core track to the core animation instance
    pCoreAnimation->addCoreTrack(pCoreTrack);
  }

   return pCoreAnimation;
}

 /*****************************************************************************/
/** Loads a core bone instance.
  *
  * This function loads a core bone instance from a file stream.
  *
  * @param file The file stream to load the core bone instance from.
  * @param strFilename The name of the file stream.
  *
  * @return One of the following values:
  *         \li a pointer to the core bone
  *         \li \b 0 if an error happend
  *****************************************************************************/

CalCoreBone *CalLoader::loadCoreBones(HANDLE hFile)
{
  // get the name length of the bone
  int len;
  ReadFile(hFile, (void*)&len, 4, &nBytesRead, NULL);
  if(len < 1)
  {
    //CalError::setLastError(CalError::INVALID_FILE_FORMAT, __FILE__, __LINE__, strFilename);
    return 0;
  }

  // read the name of the bone
  char *strBuffer;
  strBuffer = new char[len];
  ReadFile(hFile, (void*)strBuffer, len, &nBytesRead, NULL);
  Cal3DComString strName;
  strName = strBuffer;
  delete [] strBuffer;

  // get the translation of the bone
  float tx, ty, tz;
  ReadFile(hFile, (void*)&tx, 4, &nBytesRead, NULL);
  ReadFile(hFile, (void*)&ty, 4, &nBytesRead, NULL);
  ReadFile(hFile, (void*)&tz, 4, &nBytesRead, NULL);

  // get the rotation of the bone
  float rx, ry, rz, rw;
  ReadFile(hFile, (void*)&rx, 4, &nBytesRead, NULL);
  ReadFile(hFile, (void*)&ry, 4, &nBytesRead, NULL);
  ReadFile(hFile, (void*)&rz, 4, &nBytesRead, NULL);
  ReadFile(hFile, (void*)&rw, 4, &nBytesRead, NULL);

  // get the bone space translation of the bone
  float txBoneSpace, tyBoneSpace, tzBoneSpace;
  ReadFile(hFile, (void*)&txBoneSpace, 4, &nBytesRead, NULL);
  ReadFile(hFile, (void*)&tyBoneSpace, 4, &nBytesRead, NULL);
  ReadFile(hFile, (void*)&tzBoneSpace, 4, &nBytesRead, NULL);

  // get the bone space rotation of the bone
  float rxBoneSpace, ryBoneSpace, rzBoneSpace, rwBoneSpace;
  ReadFile(hFile, (void*)&rxBoneSpace, 4, &nBytesRead, NULL);
  ReadFile(hFile, (void*)&ryBoneSpace, 4, &nBytesRead, NULL);
  ReadFile(hFile, (void*)&rzBoneSpace, 4, &nBytesRead, NULL);
  ReadFile(hFile, (void*)&rwBoneSpace, 4, &nBytesRead, NULL);

  // get the parent bone id
  int parentId;
  ReadFile(hFile, (void*)&parentId, 4, &nBytesRead, NULL);

  // check if an error happend
  if(!hFile)
  {
    //CalError::setLastError(CalError::INVALID_FILE_FORMAT, __FILE__, __LINE__, strFilename);
    return 0;
  }

  // allocate a new core bone instance
  CalCoreBone *pCoreBone;
  pCoreBone = new CalCoreBone();
  if(pCoreBone == 0)
  {
    //CalError::setLastError(CalError::MEMORY_ALLOCATION_FAILED, __FILE__, __LINE__);
    return 0;
  }

  // create the core bone instance
  if(!pCoreBone->create(strName))
  {
    delete pCoreBone;
    return 0;
  }

  // set the parent of the bone
  pCoreBone->setParentId(parentId);

  // set all attributes of the bone
  pCoreBone->setTranslation(CalVector(tx, ty, tz));
  pCoreBone->setRotation(CalQuaternion(rx, ry, rz, rw));
  pCoreBone->setTranslationBoneSpace(CalVector(txBoneSpace, tyBoneSpace, tzBoneSpace));
  pCoreBone->setRotationBoneSpace(CalQuaternion(rxBoneSpace, ryBoneSpace, rzBoneSpace, rwBoneSpace));

  // read the number of children
  int childCount;
  ReadFile(hFile, (void*)&childCount, 4, &nBytesRead, NULL);
  if(!hFile || (childCount < 0))
  {
    pCoreBone->destroy();
    delete pCoreBone;
    //CalError::setLastError(CalError::INVALID_FILE_FORMAT, __FILE__, __LINE__, strFilename);
    return 0;
  }

  // load all children ids
  for(; childCount > 0; childCount--)
  {
    int childId;
    ReadFile(hFile, (void*)&childId, 4, &nBytesRead, NULL);
    if(!hFile || (childId < 0))
    {
      pCoreBone->destroy();
      delete pCoreBone;
      //CalError::setLastError(CalError::INVALID_FILE_FORMAT, __FILE__, __LINE__, strFilename);
      return 0;
    }

    pCoreBone->addChildId(childId);
  }
  return pCoreBone;
}

 /*****************************************************************************/
/** Loads a core keyframe instance.
  *
  * This function loads a core keyframe instance from a file stream.
  *
  * @param file The file stream to load the core keyframe instance from.
  * @param strFilename The name of the file stream.
  *
  * @return One of the following values:
  *         \li a pointer to the core keyframe
  *         \li \b 0 if an error happend
  *****************************************************************************/

CalCoreKeyframe *CalLoader::loadCoreKeyframe(HANDLE hFile)
{
  // get the time of the keyframe
  float time;
  ReadFile(hFile, (void*)&time, 4, &nBytesRead, NULL);

  // get the translation of the bone
  float tx, ty, tz;
  ReadFile(hFile, (void*)&tx, 4, &nBytesRead, NULL);
  ReadFile(hFile, (void*)&ty, 4, &nBytesRead, NULL);
  ReadFile(hFile, (void*)&tz, 4, &nBytesRead, NULL);

  // get the rotation of the bone
  float rx, ry, rz, rw;
  ReadFile(hFile, (void*)&rx, 4, &nBytesRead, NULL);
  ReadFile(hFile, (void*)&ry, 4, &nBytesRead, NULL);
  ReadFile(hFile, (void*)&rz, 4, &nBytesRead, NULL);
  ReadFile(hFile, (void*)&rw, 4, &nBytesRead, NULL);

  // check if an error happend
  if(!hFile)
  {
    //CalError::setLastError(CalError::INVALID_FILE_FORMAT, __FILE__, __LINE__, strFilename);
    return 0;
  }

  // allocate a new core keyframe instance
  CalCoreKeyframe *pCoreKeyframe;
  pCoreKeyframe = new CalCoreKeyframe();
  if(pCoreKeyframe == 0)
  {
    //CalError::setLastError(CalError::MEMORY_ALLOCATION_FAILED, __FILE__, __LINE__);
    return 0;
  }

  // create the core keyframe instance
  if(!pCoreKeyframe->create())
  {
    delete pCoreKeyframe;
    return 0;
  }

  // set all attributes of the keyframe
  pCoreKeyframe->setTime(time);
  pCoreKeyframe->setTranslation(CalVector(tx, ty, tz));
  pCoreKeyframe->setRotation(CalQuaternion(rx, ry, rz, rw));

  return pCoreKeyframe;
}

 /*****************************************************************************/
/** Loads a core material instance.
  *
  * This function loads a core material instance from a file.
  *
  * @param strFilename The name of the file to load the core material instance
  *                    from.
  *
  * @return One of the following values:
  *         \li a pointer to the core material
  *         \li \b 0 if an error happend
  *****************************************************************************/

CalCoreMaterial *CalLoader::loadCoreMaterial(HANDLE hFile)
{
  // check if this is a valid file
  unsigned int magic;
  ReadFile(hFile, (void*)&magic, sizeof(magic), &nBytesRead, NULL);
  if(!hFile || (memcmp(&magic, Cal::MATERIAL_FILE_MAGIC, 4) != 0))
  {
    //CalError::setLastError(CalError::INVALID_FILE_FORMAT, __FILE__, __LINE__, strFilename);
    return 0;
  }

  // check if the version is compatible with the library
  unsigned int version;
  ReadFile(hFile, (void*)&version, sizeof(version), &nBytesRead, NULL);
  if(!hFile || (version < Cal::EARLIEST_COMPATIBLE_FILE_VERSION) || (version > Cal::CURRENT_FILE_VERSION))
  {
    //CalError::setLastError(CalError::INCOMPATIBLE_FILE_VERSION, __FILE__, __LINE__, strFilename);
    return 0;
  }

  // allocate a new core material instance
  CalCoreMaterial *pCoreMaterial;
  pCoreMaterial = new CalCoreMaterial();
  if(pCoreMaterial == 0)
  {
    //CalError::setLastError(CalError::MEMORY_ALLOCATION_FAILED, __FILE__, __LINE__);
    return 0;
  }

  // create the core material instance
  if(!pCoreMaterial->create())
  {
    delete pCoreMaterial;
    return 0;
  }

  // get the ambient color of the core material
  CalCoreMaterial::Color ambientColor;
  ReadFile(hFile, (void*)&ambientColor.red, 1, &nBytesRead, NULL);
  ReadFile(hFile, (void*)&ambientColor.green, 1, &nBytesRead, NULL);
  ReadFile(hFile, (void*)&ambientColor.blue, 1, &nBytesRead, NULL);
  ReadFile(hFile, (void*)&ambientColor.alpha, 1, &nBytesRead, NULL);

  // get the diffuse color of the core material
  CalCoreMaterial::Color diffuseColor;
  ReadFile(hFile, (void*)&diffuseColor.red, 1, &nBytesRead, NULL);
  ReadFile(hFile, (void*)&diffuseColor.green, 1, &nBytesRead, NULL);
  ReadFile(hFile, (void*)&diffuseColor.blue, 1, &nBytesRead, NULL);
  ReadFile(hFile, (void*)&diffuseColor.alpha, 1, &nBytesRead, NULL);

  // get the specular color of the core material
  CalCoreMaterial::Color specularColor;
  ReadFile(hFile, (void*)&specularColor.red, 1, &nBytesRead, NULL);
  ReadFile(hFile, (void*)&specularColor.green, 1, &nBytesRead, NULL);
  ReadFile(hFile, (void*)&specularColor.blue, 1, &nBytesRead, NULL);
  ReadFile(hFile, (void*)&specularColor.alpha, 1, &nBytesRead, NULL);

  // get the specular color of the core material
  CalCoreMaterial::Color emissiveColor;
  ReadFile(hFile, (void*)&emissiveColor.red, 1, &nBytesRead, NULL);
  ReadFile(hFile, (void*)&emissiveColor.green, 1, &nBytesRead, NULL);
  ReadFile(hFile, (void*)&emissiveColor.blue, 1, &nBytesRead, NULL);
  ReadFile(hFile, (void*)&emissiveColor.alpha, 1, &nBytesRead, NULL);

  // get the shininess factor of the core material
  float shininess;
  ReadFile(hFile, (void*)&shininess, 4, &nBytesRead, NULL);

  // check if an error happend
  if(!hFile)
  {
    //CalError::setLastError(CalError::INVALID_FILE_FORMAT, __FILE__, __LINE__, strFilename);
    pCoreMaterial->destroy();
    delete pCoreMaterial;
    return 0;
  }

  // set the colors and the shininess
  pCoreMaterial->setAmbientColor(ambientColor);
  pCoreMaterial->setDiffuseColor(diffuseColor);
  pCoreMaterial->setSpecularColor(specularColor);
  pCoreMaterial->setEmissiveColor(emissiveColor);
  pCoreMaterial->setShininess(shininess);

  // read the number of maps
  int mapCount;
  ReadFile(hFile, (void*)&mapCount, 4, &nBytesRead, NULL);
  if(!hFile)
  {
    //CalError::setLastError(CalError::INVALID_FILE_FORMAT, __FILE__, __LINE__, strFilename);
    return 0;
  }

  // reserve memory for all the material data
  if(!pCoreMaterial->reserve(mapCount))
  {
    //CalError::setLastError(CalError::MEMORY_ALLOCATION_FAILED, __FILE__, __LINE__, strFilename);
    pCoreMaterial->destroy();
    delete pCoreMaterial;
    return 0;
  }

  // load all maps
  int mapId;
  for(mapId = 0; mapId < mapCount; mapId++)
  {
    CalCoreMaterial::Map map;

    // get the filename length of the map
    int len;
    ReadFile(hFile, (void*)&len, 4, &nBytesRead, NULL);
    if(len < 1)
    {
      //CalError::setLastError(CalError::INVALID_FILE_FORMAT, __FILE__, __LINE__, strFilename);
      pCoreMaterial->destroy();
      delete pCoreMaterial;
      return 0;
    }

    // read the filename of the map
    char *strBuffer;
    strBuffer = new char[len];
    ReadFile(hFile, (void*)strBuffer, len, &nBytesRead, NULL);
    map.strFilename = strBuffer;
    delete [] strBuffer;

	if (version >= 1002)
		ReadFile(hFile, (LPVOID)&map.type, sizeof(map.type), &nBytesRead, NULL);

    // initialize the user data
    map.userData = 0;

    // check if an error happend
    if(!hFile)
    {
      //CalError::setLastError(CalError::INVALID_FILE_FORMAT, __FILE__, __LINE__, strFilename);
      pCoreMaterial->destroy();
      delete pCoreMaterial;
      return 0;
    }

    // set map in the core material instance
    pCoreMaterial->setMap(mapId, map);
  }

  // explicitly close the file
  //file.close();

  return pCoreMaterial;
  //return NULL;
}

 /*****************************************************************************/
/** Loads a core mesh instance.
  *
  * This function loads a core mesh instance from a file.
  *
  * @param strFilename The name of the file to load the core mesh instance from.
  *
  * @return One of the following values:
  *         \li a pointer to the core mesh
  *         \li \b 0 if an error happend
  *****************************************************************************/

CalCoreMesh *CalLoader::loadCoreMesh(HANDLE hFile)
{
  // allocate a new core mesh instance
  CalCoreMesh *pCoreMesh;
  pCoreMesh = new CalCoreMesh();
  if(pCoreMesh == 0)
  {
    CalError::setLastError(CalError::MEMORY_ALLOCATION_FAILED, __FILE__, __LINE__);
    return 0;
  }

  // create the core mesh instance
  if(!pCoreMesh->create())
  {
    delete pCoreMesh;
    return 0;
  }

  // check if this is a valid file
  unsigned int magic;
  ReadFile(hFile, (void*)&magic, sizeof(magic), &nBytesRead, NULL);
  if(!hFile || (memcmp(&magic, Cal::MESH_FILE_MAGIC, 4) != 0))
  {
	if(!hFile || (memcmp(&magic, Cal::MESH_ALONE_FILE_MAGIC, 4) != 0))
	{
		//CalError::setLastError(CalError::INVALID_FILE_FORMAT, __FILE__, __LINE__, strFilename);
		return 0;
	}else
		pCoreMesh->NoSkel();
  }

  // check if the version is compatible with the library
  unsigned int version;
  ReadFile(hFile, (void*)&version, sizeof(version), &nBytesRead, NULL);
  if(!hFile || (version < Cal::EARLIEST_COMPATIBLE_FILE_VERSION) || (version > Cal::CURRENT_FILE_VERSION))
  {
    //CalError::setLastError(CalError::INCOMPATIBLE_FILE_VERSION, __FILE__, __LINE__, strFilename);
    return 0;
  }

  // get the number of submeshes
  int submeshCount;
  ReadFile(hFile, (void*)&submeshCount, 4, &nBytesRead, NULL);

  // check if an error happend
  if(!hFile)
  {
    //CalError::setLastError(CalError::INVALID_FILE_FORMAT, __FILE__, __LINE__, strFilename);
    return 0;
  }

  // load all core submeshes
  int submeshId;
  for(submeshId = 0; submeshId < submeshCount; submeshId++)
  {
    // load the core submesh
    CalCoreSubmesh *pCoreSubmesh;
    pCoreSubmesh = loadCoreSubmesh(hFile, pCoreMesh->NoSkel());
    if(pCoreSubmesh == 0)
    {
      pCoreMesh->destroy();
      delete pCoreMesh;
      return 0;
    }
	if( pCoreMesh->NoSkel())
		pCoreSubmesh->NoSkel( true);

    // add the core submesh to the core mesh instance
    pCoreMesh->addCoreSubmesh(pCoreSubmesh);
  }

  return pCoreMesh;
}

 /*****************************************************************************/
/** Loads a core skeleton instance.
  *
  * This function loads a core skeleton instance from a file.
  *
  * @param strFilename The name of the file to load the core skeleton instance
  *                    from.
  *
  * @return One of the following values:
  *         \li a pointer to the core skeleton
  *         \li \b 0 if an error happend
  *****************************************************************************/

CalCoreSkeleton *CalLoader::loadCoreSkeleton(HANDLE hFile)
{
  // check if this is a valid file
  unsigned int magic;
  ReadFile(hFile, (void*)&magic, sizeof(magic), &nBytesRead, NULL);
  if(!hFile || (memcmp(&magic, Cal::SKELETON_FILE_MAGIC, 4) != 0))
  {
    //CalError::setLastError(CalError::INVALID_FILE_FORMAT, __FILE__, __LINE__, strFilename);
    return 0;
  }

  // check if the version is compatible with the library
  unsigned int version;
  ReadFile(hFile, (void*)&version, sizeof(version), &nBytesRead, NULL);
  if(!hFile || (version < Cal::EARLIEST_COMPATIBLE_FILE_VERSION) || (version > Cal::CURRENT_FILE_VERSION))
  {
    //CalError::setLastError(CalError::INCOMPATIBLE_FILE_VERSION, __FILE__, __LINE__, strFilename);
    return 0;
  }

  // read the number of bones
  int boneCount;
  ReadFile(hFile, (void*)&boneCount, 4, &nBytesRead, NULL);
  if(!hFile || (boneCount <= 0))
  {
    //CalError::setLastError(CalError::INVALID_FILE_FORMAT, __FILE__, __LINE__, strFilename);
    return 0;
  }

  // allocate a new core skeleton instance
  CalCoreSkeleton *pCoreSkeleton;
  pCoreSkeleton = new CalCoreSkeleton();
  if(pCoreSkeleton == 0)
  {
    CalError::setLastError(CalError::MEMORY_ALLOCATION_FAILED, __FILE__, __LINE__);
    return 0;
  }

  // create the core skeleton instance
  if(!pCoreSkeleton->create())
  {
    delete pCoreSkeleton;
    return 0;
  }

  // load all core bones
  int boneId;
  for(boneId = 0; boneId < boneCount; boneId++)
  {
    // load the core bone
    CalCoreBone *pCoreBone;
    pCoreBone = loadCoreBones(hFile);
    if(pCoreBone == 0)
    {
      pCoreSkeleton->destroy();
      delete pCoreSkeleton;
      return 0;
    }

    // set the core skeleton of the core bone instance
    pCoreBone->setCoreSkeleton(pCoreSkeleton);

    // add the core bone to the core skeleton instance
    pCoreSkeleton->addCoreBone(pCoreBone);
  }

  // calculate state of the core skeleton
  pCoreSkeleton->calculateState();

  return pCoreSkeleton;
}

 /*****************************************************************************/
/** Loads a core submesh instance.
  *
  * This function loads a core submesh instance from a file stream.
  *
  * @param file The file stream to load the core submesh instance from.
  * @param strFilename The name of the file stream.
  *
  * @return One of the following values:
  *         \li a pointer to the core submesh
  *         \li \b 0 if an error happend
  *****************************************************************************/

CalCoreSubmesh *CalLoader::loadCoreSubmesh(HANDLE hFile, bool abNoSkel)
{
  // get the material thread id of the submesh
  int coreMaterialThreadId;
  ReadFile(hFile, (void*)&coreMaterialThreadId, 4, &nBytesRead, NULL);

  // get the number of vertices, faces, level-of-details and springs
  int vertexCount;
  ReadFile(hFile, (void*)&vertexCount, 4, &nBytesRead, NULL);
  int faceCount;
  ReadFile(hFile, (void*)&faceCount, 4, &nBytesRead, NULL);
  int lodCount;
  ReadFile(hFile, (void*)&lodCount, 4, &nBytesRead, NULL);
  int springCount;
  ReadFile(hFile, (void*)&springCount, 4, &nBytesRead, NULL);

  // get the number of texture coordinates per vertex
  int textureCoordinateCount;
  ReadFile(hFile, (void*)&textureCoordinateCount, 4, &nBytesRead, NULL);

  // check if an error happend
  if(!hFile)
  {
    //CalError::setLastError(CalError::INVALID_FILE_FORMAT, __FILE__, __LINE__, strFilename);
    return 0;
  }

  // allocate a new core submesh instance
  CalCoreSubmesh *pCoreSubmesh;
  pCoreSubmesh = new CalCoreSubmesh();
  if(pCoreSubmesh == 0)
  {
    CalError::setLastError(CalError::MEMORY_ALLOCATION_FAILED, __FILE__, __LINE__);
    return 0;
  }

  // create the core submesh instance
  if(!pCoreSubmesh->create())
  {
    delete pCoreSubmesh;
    return 0;
  }

  // set the LOD step count
  pCoreSubmesh->setLodCount(lodCount);

  // set the core material id
  pCoreSubmesh->setCoreMaterialThreadId(coreMaterialThreadId);

  // reserve memory for all the submesh data
  if(!pCoreSubmesh->reserve(vertexCount, textureCoordinateCount, faceCount, springCount))
  {
    //CalError::setLastError(CalError::MEMORY_ALLOCATION_FAILED, __FILE__, __LINE__, strFilename);
    pCoreSubmesh->destroy();
    delete pCoreSubmesh;
    return 0;
  }

  // load all vertices and their influences
  int vertexId;
  for(vertexId = 0; vertexId < vertexCount; vertexId++)
  {
    CalCoreSubmesh::Vertex vertex;

    // load data of the vertex
    ReadFile(hFile, (void*)&vertex.position.x, 4, &nBytesRead, NULL);
    ReadFile(hFile, (void*)&vertex.position.y, 4, &nBytesRead, NULL);
    ReadFile(hFile, (void*)&vertex.position.z, 4, &nBytesRead, NULL);
    ReadFile(hFile, (void*)&vertex.normal.x, 4, &nBytesRead, NULL);
    ReadFile(hFile, (void*)&vertex.normal.y, 4, &nBytesRead, NULL);
    ReadFile(hFile, (void*)&vertex.normal.z, 4, &nBytesRead, NULL);
    ReadFile(hFile, (void*)&vertex.collapseId, 4, &nBytesRead, NULL);
    ReadFile(hFile, (void*)&vertex.faceCollapseCount, 4, &nBytesRead, NULL);

    // check if an error happend
    if(!hFile)
    {
      //CalError::setLastError(CalError::INVALID_FILE_FORMAT, __FILE__, __LINE__, strFilename);
      pCoreSubmesh->destroy();
      delete pCoreSubmesh;
      return 0;
    }

    // load all texture coordinates of the vertex
    int textureCoordinateId;
    for(textureCoordinateId = 0; textureCoordinateId < textureCoordinateCount; textureCoordinateId++)
    {
      CalCoreSubmesh::TxCoor textureCoordinate;

      // load data of the influence
      ReadFile(hFile, (void*)&textureCoordinate.u, 4, &nBytesRead, NULL);
      ReadFile(hFile, (void*)&textureCoordinate.v, 4, &nBytesRead, NULL);

      // check if an error happend
      if(!hFile)
      {
        //CalError::setLastError(CalError::INVALID_FILE_FORMAT, __FILE__, __LINE__, strFilename);
        pCoreSubmesh->destroy();
        delete pCoreSubmesh;
        return 0;
      }

      // set texture coordinate in the core submesh instance
      pCoreSubmesh->setTextureCoordinate(vertexId, textureCoordinateId, textureCoordinate);
    }

	if( !abNoSkel){
		// get the number of influences
		int influenceCount;
		ReadFile(hFile, (void*)&influenceCount, 4, &nBytesRead, NULL);

		// check if an error happend
		if(!hFile)
		{
		  //CalError::setLastError(CalError::INVALID_FILE_FORMAT, __FILE__, __LINE__, strFilename);
		  pCoreSubmesh->destroy();
		  delete pCoreSubmesh;
		  return 0;
		}

		// reserve memory for the influences in the vertex
		vertex.vectorInfluence.reserve(influenceCount);
		vertex.vectorInfluence.resize(influenceCount);

		// load all influences of the vertex
		int influenceId;
		for(influenceId = 0; influenceId < influenceCount; influenceId++)
		{
		  // load data of the influence
		  ReadFile(hFile, (void*)&vertex.vectorInfluence[influenceId].boneId, 4, &nBytesRead, NULL);
		  ReadFile(hFile, (void*)&vertex.vectorInfluence[influenceId].weight, 4, &nBytesRead, NULL);

		  // check if an error happend
		  if(!hFile)
		  {
			//CalError::setLastError(CalError::INVALID_FILE_FORMAT, __FILE__, __LINE__, strFilename);
			pCoreSubmesh->destroy();
			delete pCoreSubmesh;
			return 0;
		  }
		}
	}

    // set vertex in the core submesh instance
    pCoreSubmesh->setVertex(vertexId, vertex);

    // load the physical property of the vertex if there are springs in the core submesh
    if(springCount > 0)
    {
      CalCoreSubmesh::PhysicalProperty physicalProperty;

      // load data of the physical property
      ReadFile(hFile, (void*)&physicalProperty.weight, 4, &nBytesRead, NULL);

      // check if an error happend
      if(!hFile)
      {
        //CalError::setLastError(CalError::INVALID_FILE_FORMAT, __FILE__, __LINE__, strFilename);
        pCoreSubmesh->destroy();
        delete pCoreSubmesh;
        return 0;
      }

      // set the physical property in the core submesh instance
      pCoreSubmesh->setPhysicalProperty(vertexId, physicalProperty);
    }
  }

  // load all springs
  int springId;
  for(springId = 0; springId < springCount; springId++)
  {
    CalCoreSubmesh::Spring spring;

    // load data of the spring
    ReadFile(hFile, (void*)&spring.vertexId[0], 4, &nBytesRead, NULL);
    ReadFile(hFile, (void*)&spring.vertexId[1], 4, &nBytesRead, NULL);
    ReadFile(hFile, (void*)&spring.springCoefficient, 4, &nBytesRead, NULL);
    ReadFile(hFile, (void*)&spring.idleLength, 4, &nBytesRead, NULL);

    // check if an error happend
    if(!hFile)
    {
      //CalError::setLastError(CalError::INVALID_FILE_FORMAT, __FILE__, __LINE__, strFilename);
      pCoreSubmesh->destroy();
      delete pCoreSubmesh;
      return 0;
    }

    // set spring in the core submesh instance
    pCoreSubmesh->setSpring(springId, spring);
  }

  // load all faces
  int faceId;
  for(faceId = 0; faceId < faceCount; faceId++)
  {
    CalCoreSubmesh::Face face;

    // load data of the face
    ReadFile(hFile, (void*)&face.vertexId[0], sizeof(face.vertexId[0]), &nBytesRead, NULL);
    ReadFile(hFile, (void*)&face.vertexId[1], sizeof(face.vertexId[1]), &nBytesRead, NULL);
    ReadFile(hFile, (void*)&face.vertexId[2], sizeof(face.vertexId[2]), &nBytesRead, NULL);


    // check if an error happend
    if(!hFile)
    {
      //CalError::setLastError(CalError::INVALID_FILE_FORMAT, __FILE__, __LINE__, strFilename);
      pCoreSubmesh->destroy();
      delete pCoreSubmesh;
      return 0;
    }

    // set face in the core submesh instance
    pCoreSubmesh->setFace(faceId, face);
  }

  return pCoreSubmesh;
}

 /*****************************************************************************/
/** Loads a core track instance.
  *
  * This function loads a core track instance from a file stream.
  *
  * @param file The file stream to load the core track instance from.
  * @param strFilename The name of the file stream.
  *
  * @return One of the following values:
  *         \li a pointer to the core track
  *         \li \b 0 if an error happend
  *****************************************************************************/

CalCoreTrack *CalLoader::loadCoreTrack(HANDLE hFile)
{
  // read the bone id
  int coreBoneId;
  ReadFile(hFile, (void*)&coreBoneId, 4, &nBytesRead, NULL);
  if(!hFile || (coreBoneId < 0 && Cal::CURRENT_FILE_VERSION < 1003))
  {
    //CalError::setLastError(CalError::INVALID_FILE_FORMAT, __FILE__, __LINE__, strFilename);
    return 0;
  }

  // allocate a new core track instance
  CalCoreTrack *pCoreTrack;
  pCoreTrack = new CalCoreTrack();
  if(pCoreTrack == 0)
  {
    CalError::setLastError(CalError::MEMORY_ALLOCATION_FAILED, __FILE__, __LINE__);
    return 0;
  }

  // create the core track instance
  if(!pCoreTrack->create())
  {
    delete pCoreTrack;
    return 0;
  }

  // link the core track to the appropriate core bone instance
  pCoreTrack->setCoreBoneId(coreBoneId);

  // read the number of keyframes
  int keyframeCount;
  ReadFile(hFile, (void*)&keyframeCount, 4, &nBytesRead, NULL);
  if(!hFile || (keyframeCount <= 0))
  {
    //CalError::setLastError(CalError::INVALID_FILE_FORMAT, __FILE__, __LINE__, strFilename);
    return 0;
  }

  // load all core keyframes
  int keyframeId;
  for(keyframeId = 0; keyframeId < keyframeCount; keyframeId++)
  {
    // load the core keyframe
    CalCoreKeyframe *pCoreKeyframe;
    pCoreKeyframe = loadCoreKeyframe(hFile);
    if(pCoreKeyframe == 0)
    {
      pCoreTrack->destroy();
      delete pCoreTrack;
      return 0;
    }

    // add the core keyframe to the core track instance
    pCoreTrack->addCoreKeyframe(pCoreKeyframe);
  }

  return pCoreTrack;
}

CFace* CalLoader::loadFace(HANDLE hFile)
{
	CFace	*pFace = NULL;

	// check if this is a valid file
	unsigned int magic;
	ReadFile(hFile, (void*)&magic, sizeof(magic), &nBytesRead, NULL);
	if(!hFile || (memcmp(&magic, Cal::FACE_FILE_MAGIC, 4) != 0)){
		//CalError::setLastError(CalError::INVALID_FILE_FORMAT, __FILE__, __LINE__, strFilename);
		return 0;
	}

	pFace = new CFace();
	ReadFile(hFile, (void*)&(pFace->header), sizeof(pFace->header), &nBytesRead, NULL);

	DWORD dwRealSmc=0;
	pFace->aSubmeshNum.reserve(pFace->header.dwSubmeshCount);
	pFace->aVertNum.reserve(pFace->header.dwSubmeshCount);
	pFace->aVertIndexes.reserve(pFace->header.dwSubmeshCount);
	pFace->aDeltas.reserve(pFace->header.dwSubmeshCount);
	//pFace->header.

	for(int ism=0; ism<pFace->header.dwSubmeshCount; ism++){
		WORD	dwSubmeshVertNum=0;
		ReadFile(hFile, (void*)&(dwSubmeshVertNum), sizeof(WORD), &nBytesRead, NULL);
		if(dwSubmeshVertNum == 0) continue;
		
		pFace->aSubmeshNum.push_back(ism);
		dwRealSmc++;

		WORD*	paVertIndexes = new WORD[dwSubmeshVertNum];
		D3DXVECTOR3*	paDeltas= new D3DXVECTOR3[dwSubmeshVertNum*pFace->header.lNum];

		ReadFile(hFile, (void*)paVertIndexes, sizeof(WORD)*dwSubmeshVertNum, &nBytesRead, NULL);
		ReadFile(hFile, (void*)paDeltas, sizeof(D3DVECTOR)*dwSubmeshVertNum*pFace->header.lNum, &nBytesRead, NULL);

		pFace->aVertNum.push_back(dwSubmeshVertNum);
		pFace->aVertIndexes.push_back(paVertIndexes);
		pFace->aDeltas.push_back(paDeltas);
	}
	pFace->header.dwSubmeshCount = dwRealSmc;

	ReadFile(hFile, (void*)&(pFace->header.bFileName), sizeof(BYTE), &nBytesRead, NULL);
	if( pFace->header.bFileName > 0){
		pFace->header.pFileName = (char*)malloc(pFace->header.bFileName+1);
		ZeroMemory(pFace->header.pFileName, pFace->header.bFileName+1);
		ReadFile(hFile, (void*)pFace->header.pFileName, pFace->header.bFileName, &nBytesRead, NULL);
	}
	
	return pFace;
}


CFace* CalLoader::loadFace(void *apData, DWORD adwSize)
{
	CalMemFile mf;
	CFace	*pFace = NULL;

	mf.init( apData, adwSize);

	// check if this is a valid file
	unsigned int magic;
	nBytesRead = mf.read((void*)&magic, sizeof(magic));
	//ReadFile(hFile, (void*)&magic, sizeof(magic), &nBytesRead, NULL);
	if((nBytesRead == 0) || (memcmp(&magic, Cal::FACE_FILE_MAGIC, 4) != 0)){
		//CalError::setLastError(CalError::INVALID_FILE_FORMAT, __FILE__, __LINE__, strFilename);
		return 0;
	}

	pFace = new CFace();
	//ReadFile(hFile, , &nBytesRead, NULL);
	nBytesRead = mf.read((void*)&(pFace->header), sizeof(pFace->header));

	DWORD dwRealSmc=0;
	pFace->aSubmeshNum.reserve(pFace->header.dwSubmeshCount);
	pFace->aVertNum.reserve(pFace->header.dwSubmeshCount);
	pFace->aVertIndexes.reserve(pFace->header.dwSubmeshCount);
	pFace->aDeltas.reserve(pFace->header.dwSubmeshCount);
	//pFace->header.

	for(int ism=0; ism<pFace->header.dwSubmeshCount; ism++){
		WORD	dwSubmeshVertNum=0;
		//ReadFile(hFile, (void*)&(dwSubmeshVertNum), sizeof(WORD), &nBytesRead, NULL);
		nBytesRead = mf.read((void*)&(dwSubmeshVertNum), sizeof(WORD));
		if(dwSubmeshVertNum == 0) continue;
		
		pFace->aSubmeshNum.push_back(ism);
		dwRealSmc++;

		WORD*	paVertIndexes = new WORD[dwSubmeshVertNum];
		D3DXVECTOR3*	paDeltas= new D3DXVECTOR3[dwSubmeshVertNum*pFace->header.lNum];

		//ReadFile(hFile, (void*)paVertIndexes, sizeof(WORD)*dwSubmeshVertNum, &nBytesRead, NULL);
		nBytesRead = mf.read((void*)paVertIndexes, sizeof(WORD)*dwSubmeshVertNum);
		//ReadFile(hFile, (void*)paDeltas, sizeof(D3DVECTOR)*dwSubmeshVertNum*pFace->header.lNum, &nBytesRead, NULL);
		nBytesRead = mf.read((void*)paDeltas, sizeof(D3DVECTOR)*dwSubmeshVertNum*pFace->header.lNum);

		pFace->aVertNum.push_back(dwSubmeshVertNum);
		pFace->aVertIndexes.push_back(paVertIndexes);
		pFace->aDeltas.push_back(paDeltas);
	}
	pFace->header.dwSubmeshCount = dwRealSmc;

	//ReadFile(hFile, (void*)&(pFace->header.bFileName), sizeof(BYTE), &nBytesRead, NULL);
	nBytesRead = mf.read((void*)&(pFace->header.bFileName), sizeof(BYTE));
	if( pFace->header.bFileName > 0){
		pFace->header.pFileName = (char*)malloc(pFace->header.bFileName+1);
		ZeroMemory(pFace->header.pFileName, pFace->header.bFileName+1);
		//ReadFile(hFile, (void*)pFace->header.pFileName, pFace->header.bFileName, &nBytesRead, NULL);
		nBytesRead = mf.read((void*)pFace->header.pFileName, pFace->header.bFileName);
	}
	
	return pFace;
}

CalCoreAnimation *CalLoader::loadCoreAnimation(void *apData, DWORD adwSize)
{
	CalMemFile mf;
	CFace	*pFace = NULL;

	mf.init( apData, adwSize);

  // check if this is a valid file
  char magic[4];
  bool	m_bRelative=false;
  bool	m_bSaveFirst=false;
  bool	m_bSaveLast=false;

  mf.read((char *)&magic, sizeof(magic));
  if(mf.eof() || (memcmp(&magic, Cal::ANIMATION_FILE_MAGIC, 4) != 0))
  {
	if(memcmp(&magic, Cal::RELANIMATION_FILE_MAGIC, 4) != 0)
	{
	  bool bError=false;
	  if( magic[0] == 'D') m_bRelative=false; else
		  if( magic[0] == 'E') m_bRelative=true; else
			  bError = true;
	  if( magic[1] == 'D') m_bSaveFirst=false; else
		  if( magic[1] == 'E') m_bSaveFirst=true; else
			  bError = true;
	  if( magic[2] == 'D') m_bSaveLast=false; else
		  if( magic[2] == 'E') m_bSaveLast=true; else
			  bError = true;
	  if( bError) return 0;
	}else
		m_bRelative = true;
  }

  // check if the version is compatible with the library
  unsigned int version;
  mf.read((char *)&version, sizeof(version));
  file_version = version;
  if(mf.eof() || (version < Cal::EARLIEST_COMPATIBLE_FILE_VERSION) || (version > Cal::CURRENT_FILE_VERSION))
  {
	file_version = version;
    CalError::setLastError(CalError::INCOMPATIBLE_FILE_VERSION, __FILE__, __LINE__, "Error load CoreAnimation");
    return 0;
  }

  // allocate a new core animation instance
  CalCoreAnimation *pCoreAnimation;
  pCoreAnimation = new CalCoreAnimation();
  if(pCoreAnimation == 0)
  {
    CalError::setLastError(CalError::MEMORY_ALLOCATION_FAILED, __FILE__, __LINE__);
    return 0;
  }

  // create the core animation instance
  if(!pCoreAnimation->create())
  {
    delete pCoreAnimation;
    return 0;
  }
  pCoreAnimation->m_version = file_version;

  DWORD flags=0;
  if( file_version >= 1006)
  {
	  mf.read((char *)&flags, sizeof(flags));
	  pCoreAnimation->setFlags(flags);
  }

  if( file_version >= 1007)
  {
	  if( flags & CAL3D_COREANIM_KEEPFRAME)
	  {
		  float	len;
		  mf.read((char *)&len, sizeof(float));
		  pCoreAnimation->setFlag(CAL3D_COREANIM_KEEPFRAME);
		  pCoreAnimation->setFlagData(CAL3D_COREANIM_KEEPFRAME, &len);
	  }
  }

  // get the duration of the core animation
  float duration;
  mf.read((char *)&duration, 4);
  if(mf.eof())
  {
    CalError::setLastError(CalError::INVALID_FILE_FORMAT, __FILE__, __LINE__, "Error load CoreAnimation");
    pCoreAnimation->destroy();
    delete pCoreAnimation;
    return 0;
  }

  pCoreAnimation->setRelMode( m_bRelative);
  pCoreAnimation->saveFState( m_bSaveFirst);
  pCoreAnimation->saveLState( m_bSaveLast);

  // check for a valid duration
  if(duration <= 0.0f)
  {
    CalError::setLastError(CalError::INVALID_ANIMATION_DURATION, __FILE__, __LINE__, "Error load CoreAnimation");
    pCoreAnimation->destroy();
    delete pCoreAnimation;
    return 0;
  }

  // set the duration in the core animation instance
  pCoreAnimation->setDuration(duration);

  // read the number of tracks
  int trackCount;
  mf.read((char *)&trackCount, 4);
  if(mf.eof() || (trackCount <= 0))
  {
    CalError::setLastError(CalError::INVALID_FILE_FORMAT, __FILE__, __LINE__, "Error load CoreAnimation");
    return 0;
  }

  // load all core bones
  int trackId;
  for(trackId = 0; trackId < trackCount; trackId++)
  {
    // load the core track
    CalCoreTrack *pCoreTrack;
    pCoreTrack = loadCoreTrack(mf);
    if(pCoreTrack == 0)
    {
      pCoreAnimation->destroy();
      delete pCoreAnimation;
      return 0;
    }

    // add the core track to the core animation instance
    pCoreAnimation->addCoreTrack(pCoreTrack);
  }

  return pCoreAnimation;
}

CalCoreTrack *CalLoader::loadCoreTrack(CalMemFile &mf)
{
  if(mf.eof())
  {
    CalError::setLastError(CalError::INVALID_HANDLE, __FILE__, __LINE__, "Error load CoreTrack");
    return 0;
  }

  // read the bone id
  int coreBoneId;
  mf.read((char *)&coreBoneId, 4);
  if(mf.eof() || (coreBoneId < 0 && Cal::CURRENT_FILE_VERSION < 1003))
  {
    CalError::setLastError(CalError::INVALID_FILE_FORMAT, __FILE__, __LINE__, "Error load CoreTrack");
    return 0;
  }

  // allocate a new core track instance
  CalCoreTrack *pCoreTrack;
  pCoreTrack = new CalCoreTrack();
  if(pCoreTrack == 0)
  {
    CalError::setLastError(CalError::MEMORY_ALLOCATION_FAILED, __FILE__, __LINE__);
    return 0;
  }

  // create the core track instance
  if(!pCoreTrack->create())
  {
    delete pCoreTrack;
    return 0;
  }

  int	ns;
  if( file_version > 1000)
  {
	mf.read((char *)&ns, sizeof(int));
	Cal3DComString	name;
	if( ns > 0)
	{
	  mf.read((char *)name.GetBufferSetLength(ns), ns);
	  pCoreTrack->setCoreBoneName(name.GetBuffer());
	}	
  }

  if( file_version > 1008)
  {
	  if( ns > 0)
	  {
		float	boneLength;
		mf.read((char *)&boneLength, sizeof(float));
		pCoreTrack->m_coreCommonBoneLength = boneLength;
	  }
  }

  // link the core track to the appropriate core bone instance
  pCoreTrack->setCoreBoneId(coreBoneId);

  // read the number of keyframes
  int keyframeCount;
  mf.read((char *)&keyframeCount, 4);
  if(mf.eof() || (keyframeCount <= 0))
  {
    CalError::setLastError(CalError::INVALID_FILE_FORMAT, __FILE__, __LINE__, "Error load CoreTrack");
    return 0;
  }

  // load all core keyframes
  int keyframeId;
  for(keyframeId = 0; keyframeId < keyframeCount; keyframeId++)
  {
    // load the core keyframe
    CalCoreKeyframe *pCoreKeyframe;
    pCoreKeyframe = loadCoreKeyframe(mf);
    if(pCoreKeyframe == 0)
    {
      pCoreTrack->destroy();
      delete pCoreTrack;
      return 0;
    }

    // add the core keyframe to the core track instance
    pCoreTrack->addCoreKeyframe(pCoreKeyframe);
  }

  return pCoreTrack;
}

CalCoreKeyframe *CalLoader::loadCoreKeyframe(CalMemFile &mf)
{
  if(mf.eof())
  {
    CalError::setLastError(CalError::INVALID_HANDLE, __FILE__, __LINE__, "Error load CoreKeyframe");
    return 0;
  }

  // get the time of the keyframe
  float time;
  mf.read((char *)&time, 4);

  // get the translation of the bone
  float tx, ty, tz;
  mf.read((char *)&tx, 4);
  mf.read((char *)&ty, 4);
  mf.read((char *)&tz, 4);

  // get the rotation of the bone
  float rx, ry, rz, rw;
  mf.read((char *)&rx, 4);
  mf.read((char *)&ry, 4);
  mf.read((char *)&rz, 4);
  mf.read((char *)&rw, 4);

  // check if an error happend
  if(mf.eof())
  {
    CalError::setLastError(CalError::INVALID_FILE_FORMAT, __FILE__, __LINE__, "Error load CoreKeyframe");
    return 0;
  }

  // allocate a new core keyframe instance
  CalCoreKeyframe *pCoreKeyframe;
  pCoreKeyframe = new CalCoreKeyframe();
  if(pCoreKeyframe == 0)
  {
    CalError::setLastError(CalError::MEMORY_ALLOCATION_FAILED, __FILE__, __LINE__);
    return 0;
  }

  // create the core keyframe instance
  if(!pCoreKeyframe->create())
  {
    delete pCoreKeyframe;
    return 0;
  }

  // set all attributes of the keyframe
  pCoreKeyframe->setTime(time);
  pCoreKeyframe->setTranslation(CalVector(tx, ty, tz));
  pCoreKeyframe->setRotation(CalQuaternion(rx, ry, rz, rw));

  return pCoreKeyframe;
}

CalCoreSkeleton *CalLoader::loadCoreSkeleton(void *apData, DWORD adwSize)
{
	CalMemFile mf;
	CFace	*pFace = NULL;

	mf.init( apData, adwSize);

  // check if this is a valid file
  unsigned int magic;
  mf.read((char *)&magic, sizeof(magic));
  if(mf.eof() || (memcmp(&magic, Cal::SKELETON_FILE_MAGIC, 4) != 0))
  {
    CalError::setLastError(CalError::INVALID_FILE_FORMAT, __FILE__, __LINE__, "Error load CoreSkeleton");
    return 0;
  }

  // check if the version is compatible with the library
  unsigned int version;
  mf.read((char *)&version, sizeof(version));
  if(mf.eof() || (version < Cal::EARLIEST_COMPATIBLE_FILE_VERSION) || (version > Cal::CURRENT_FILE_VERSION))
  {
    CalError::setLastError(CalError::INCOMPATIBLE_FILE_VERSION, __FILE__, __LINE__, "Error load CoreSkeleton");
    return 0;
  }

  // read the number of bones
  int boneCount;
  mf.read((char *)&boneCount, 4);
  if(mf.eof() || (boneCount <= 0))
  {
    CalError::setLastError(CalError::INVALID_FILE_FORMAT, __FILE__, __LINE__, "Error load CoreSkeleton");
    return 0;
  }

  // allocate a new core skeleton instance
  CalCoreSkeleton *pCoreSkeleton;
  pCoreSkeleton = new CalCoreSkeleton();
  if(pCoreSkeleton == 0)
  {
    CalError::setLastError(CalError::MEMORY_ALLOCATION_FAILED, __FILE__, __LINE__);
    return 0;
  }

  // create the core skeleton instance
  if(!pCoreSkeleton->create())
  {
    delete pCoreSkeleton;
    return 0;
  }

  // load all core bones
  int boneId;
  for(boneId = 0; boneId < boneCount; boneId++)
  {
    // load the core bone
    CalCoreBone *pCoreBone;
    pCoreBone = loadCoreBones(mf);
    if(pCoreBone == 0)
    {
      pCoreSkeleton->destroy();
      delete pCoreSkeleton;
      return 0;
    }

    // set the core skeleton of the core bone instance
    pCoreBone->setCoreSkeleton(pCoreSkeleton);

    // add the core bone to the core skeleton instance
    pCoreSkeleton->addCoreBone(pCoreBone);
  }

  // calculate state of the core skeleton
  pCoreSkeleton->calculateState();

  return pCoreSkeleton;
}

CalCoreBone *CalLoader::loadCoreBones(CalMemFile &mf)
{
  if(mf.eof())
  {
    CalError::setLastError(CalError::INVALID_HANDLE, __FILE__, __LINE__, "Error load CoreBone");
    return 0;
  }

  // get the name length of the bone
  int len;
  mf.read((char *)&len, 4);
  if(len < 1)
  {
    CalError::setLastError(CalError::INVALID_FILE_FORMAT, __FILE__, __LINE__, "Error load CoreBone");
    return 0;
  }

  // read the name of the bone
  char *strBuffer;
  strBuffer = new char[len];
  mf.read(strBuffer, len);
  Cal3DComString strName;
  strName = strBuffer;
  delete [] strBuffer;

  // get the translation of the bone
  float tx, ty, tz;
  mf.read((char *)&tx, 4);
  mf.read((char *)&ty, 4);
  mf.read((char *)&tz, 4);

  // get the rotation of the bone
  float rx, ry, rz, rw;
  mf.read((char *)&rx, 4);
  mf.read((char *)&ry, 4);
  mf.read((char *)&rz, 4);
  mf.read((char *)&rw, 4);

  // get the bone space translation of the bone
  float txBoneSpace, tyBoneSpace, tzBoneSpace;
  mf.read((char *)&txBoneSpace, 4);
  mf.read((char *)&tyBoneSpace, 4);
  mf.read((char *)&tzBoneSpace, 4);

  // get the bone space rotation of the bone
  float rxBoneSpace, ryBoneSpace, rzBoneSpace, rwBoneSpace;
  mf.read((char *)&rxBoneSpace, 4);
  mf.read((char *)&ryBoneSpace, 4);
  mf.read((char *)&rzBoneSpace, 4);
  mf.read((char *)&rwBoneSpace, 4);

  // get the parent bone id
  int parentId;
  mf.read((char *)&parentId, 4);

  // check if an error happend
  if(mf.eof())
  {
    CalError::setLastError(CalError::INVALID_FILE_FORMAT, __FILE__, __LINE__, "Error load CoreBone");
    return 0;
  }

  // allocate a new core bone instance
  CalCoreBone *pCoreBone;
  pCoreBone = new CalCoreBone();
  if(pCoreBone == 0)
  {
    CalError::setLastError(CalError::MEMORY_ALLOCATION_FAILED, __FILE__, __LINE__);
    return 0;
  }

  // create the core bone instance
  if(!pCoreBone->create(strName))
  {
    delete pCoreBone;
    return 0;
  }

  // set the parent of the bone
  pCoreBone->setParentId(parentId);

  // set all attributes of the bone
  pCoreBone->setTranslation(CalVector(tx, ty, tz));
  pCoreBone->setRotation(CalQuaternion(rx, ry, rz, rw));
  pCoreBone->setTranslationBoneSpace(CalVector(txBoneSpace, tyBoneSpace, tzBoneSpace));
  pCoreBone->setRotationBoneSpace(CalQuaternion(rxBoneSpace, ryBoneSpace, rzBoneSpace, rwBoneSpace));

  // read the number of children
  int childCount;
  mf.read((char *)&childCount, 4);
  if(/*mf.eof() || */(childCount < 0))
  {
    pCoreBone->destroy();
    delete pCoreBone;
    CalError::setLastError(CalError::INVALID_FILE_FORMAT, __FILE__, __LINE__, "Error load CoreBone");
    return 0;
  }

  // load all children ids
  for(; childCount > 0; childCount--)
  {
    int childId;
    mf.read((char *)&childId, 4);
    if(/*mf.eof() || */(childId < 0))
    {
      pCoreBone->destroy();
      delete pCoreBone;
      CalError::setLastError(CalError::INVALID_FILE_FORMAT, __FILE__, __LINE__, "Error load CoreBone");
      return 0;
    }

    pCoreBone->addChildId(childId);
  }

  return pCoreBone;
}

CalCoreMesh *CalLoader::loadCoreMesh(void *apData, DWORD adwSize)
{
	CalMemFile mf;
	CFace	*pFace = NULL;

	mf.init( apData, adwSize);

  // allocate a new core mesh instance
  CalCoreMesh *pCoreMesh;
  pCoreMesh = new CalCoreMesh();
  if(pCoreMesh == 0)
  {
    CalError::setLastError(CalError::MEMORY_ALLOCATION_FAILED, __FILE__, __LINE__);
    return 0;
  }
  // create the core mesh instance
  if(!pCoreMesh->create())
  {
    delete pCoreMesh;
    return 0;
  }

  // check if this is a valid file
  unsigned int magic;
  mf.read((char *)&magic, sizeof(magic));
  if(mf.eof() || (memcmp(&magic, Cal::MESH_FILE_MAGIC, 4) != 0))
  {
	if(mf.eof() || (memcmp(&magic, Cal::MESH_ALONE_FILE_MAGIC, 4) != 0))
	{
		CalError::setLastError(CalError::INVALID_FILE_FORMAT, __FILE__, __LINE__, "Error load Coremesh");
		return 0;
	}else
		pCoreMesh->NoSkel( true);
  }

  // check if the version is compatible with the library
  unsigned int version;
  mf.read((char *)&version, sizeof(version));
  if(mf.eof() || (version < Cal::EARLIEST_COMPATIBLE_FILE_VERSION) || (version > Cal::CURRENT_FILE_VERSION))
  {
    CalError::setLastError(CalError::INCOMPATIBLE_FILE_VERSION, __FILE__, __LINE__, "Error load Coremesh");
    return 0;
  }
  file_version = version;

  if( version >= 1005)
  {
	// write mesh translation and rotation
	CalVector trn = pCoreMesh->get_worldTranslation();
	CalQuaternion rtn = pCoreMesh->get_worldRotation();
	mf.read((char *)&trn, sizeof(trn));
	mf.read((char *)&rtn, sizeof(rtn));
	pCoreMesh->set_worldTranslation(trn);
	pCoreMesh->set_worldRotation(rtn);
  }

  // get the number of submeshes
  int submeshCount;
  mf.read((char *)&submeshCount, 4);

  // check if an error happend
  if(mf.eof())
  {
    CalError::setLastError(CalError::INVALID_FILE_FORMAT, __FILE__, __LINE__, "Error load CoreMesh");
    return 0;
  }

  // load all core submeshes
  int submeshId;
  for(submeshId = 0; submeshId < submeshCount; submeshId++)
  {
    // load the core submesh
    CalCoreSubmesh *pCoreSubmesh;
    pCoreSubmesh = loadCoreSubmesh(mf, pCoreMesh->NoSkel());

    if(pCoreSubmesh == 0)
    {
      pCoreMesh->destroy();
      delete pCoreMesh;
      return 0;
    }

	if( pCoreMesh->NoSkel())
		pCoreSubmesh->NoSkel( true);

    // add the core submesh to the core mesh instance
    pCoreMesh->addCoreSubmesh(pCoreSubmesh);
  }

  // load shadow data
  if( version>=1004 && version<1007)
  {
		int	edgesCount;
		mf.read((char *)&edgesCount, 4);
  }

  // load shadow data
  if( file_version>=1007)
  {
		int	edgesCount;
		mf.read((char *)&edgesCount, 4);
		if( edgesCount < 10000)
		if( edgesCount > 0)
		{
			CalShadowData*	sd = new CalShadowData();
			for( int i=0; i<edgesCount; i++)
			{
				ShadowEdge	edge;
				mf.read((char *)&edge, sizeof(ShadowEdge));
				sd->edges.push_back(edge);
			}
			pCoreMesh->setShadowData(sd);
		}
  }

  return pCoreMesh;
}

CalCoreSubmesh *CalLoader::loadCoreSubmesh(CalMemFile &mf, bool abNoSkel)
{
  if(mf.eof())
  {
    CalError::setLastError(CalError::INVALID_HANDLE, __FILE__, __LINE__, "Error load CoreSubmesh");
    return 0;
  }

  // get the material thread id of the submesh
  int coreMaterialThreadId;
  mf.read((char *)&coreMaterialThreadId, 4);

  // get the number of vertices, faces, level-of-details and springs
  int vertexCount;
  mf.read((char *)&vertexCount, 4);
  int faceCount;
  mf.read((char *)&faceCount, 4);
  int lodCount;
  mf.read((char *)&lodCount, 4);
  int springCount;
  mf.read((char *)&springCount, 4);

  // get the number of texture coordinates per vertex
  int textureCoordinateCount;
  mf.read((char *)&textureCoordinateCount, 4);

  // check if an error happend
  if(mf.eof())
  {
    CalError::setLastError(CalError::INVALID_FILE_FORMAT, __FILE__, __LINE__, "Error load CoreSubmesh");
    return 0;
  }

  // allocate a new core submesh instance
  CalCoreSubmesh *pCoreSubmesh;
  pCoreSubmesh = new CalCoreSubmesh();
  if(pCoreSubmesh == 0)
  {
    CalError::setLastError(CalError::MEMORY_ALLOCATION_FAILED, __FILE__, __LINE__);
    return 0;
  }

  // create the core submesh instance
  if(!pCoreSubmesh->create())
  {
    delete pCoreSubmesh;
    return 0;
  }

  // set the LOD step count
  pCoreSubmesh->setLodCount(lodCount);

  // set the core material id
  pCoreSubmesh->setCoreMaterialThreadId(coreMaterialThreadId);

  // reserve memory for all the submesh data
  if(!pCoreSubmesh->reserve(vertexCount, textureCoordinateCount, faceCount, springCount))
  {
    CalError::setLastError(CalError::MEMORY_ALLOCATION_FAILED, __FILE__, __LINE__, "Error load CoreSubmesh");
    pCoreSubmesh->destroy();
    delete pCoreSubmesh;
    return 0;
  }

  // load all vertices and their influences
  int vertexId;
  for(vertexId = 0; vertexId < vertexCount; vertexId++)
  {
    CalCoreSubmesh::Vertex vertex;

    // load data of the vertex
    mf.read((char *)&vertex.position.x, 4);
    mf.read((char *)&vertex.position.y, 4);
    mf.read((char *)&vertex.position.z, 4);
    mf.read((char *)&vertex.normal.x, 4);
    mf.read((char *)&vertex.normal.y, 4);
    mf.read((char *)&vertex.normal.z, 4);
    mf.read((char *)&vertex.collapseId, 4);
    mf.read((char *)&vertex.faceCollapseCount, 4);

    // check if an error happend
    if(mf.eof())
    {
      CalError::setLastError(CalError::INVALID_FILE_FORMAT, __FILE__, __LINE__, "Error load CoreSubmesh");
      pCoreSubmesh->destroy();
      delete pCoreSubmesh;
      return 0;
    }

    // load all texture coordinates of the vertex
    int textureCoordinateId;
    for(textureCoordinateId = 0; textureCoordinateId < textureCoordinateCount; textureCoordinateId++)
    {
      CalCoreSubmesh::TxCoor textureCoordinate;

      // load data of the influence
      mf.read((char *)&textureCoordinate.u, 4);
      mf.read((char *)&textureCoordinate.v, 4);
	  textureCoordinate.v = 1-textureCoordinate.v;

      // check if an error happend
      if(mf.eof())
      {
        CalError::setLastError(CalError::INVALID_FILE_FORMAT, __FILE__, __LINE__, "Error load CoreSubmesh");
        pCoreSubmesh->destroy();
        delete pCoreSubmesh;
        return 0;
      }

      // set texture coordinate in the core submesh instance
      pCoreSubmesh->setTextureCoordinate(vertexId, textureCoordinateId, textureCoordinate);
    }


	if( !abNoSkel){
		// get the number of influences
		int influenceCount;
		mf.read((char *)&influenceCount, 4);

		// check if an error happend
		if(mf.eof())
		{
		  CalError::setLastError(CalError::INVALID_FILE_FORMAT, __FILE__, __LINE__, "Error load CoreSubmesh");
		  pCoreSubmesh->destroy();
		  delete pCoreSubmesh;
		  return 0;
		}

		// reserve memory for the influences in the vertex
		vertex.vectorInfluence.reserve(influenceCount);
		vertex.vectorInfluence.resize(influenceCount);

		// load all influences of the vertex
		int influenceId;
		for(influenceId = 0; influenceId < influenceCount; influenceId++)
		{
		  // load data of the influence
		  mf.read((char *)&vertex.vectorInfluence[influenceId].boneId, 4);
		  mf.read((char *)&vertex.vectorInfluence[influenceId].weight, 4);

		  // check if an error happend
		  if(mf.eof())
		  {
			CalError::setLastError(CalError::INVALID_FILE_FORMAT, __FILE__, __LINE__, "Error load CoreSubmesh");
			pCoreSubmesh->destroy();
			delete pCoreSubmesh;
			return 0;
		  }
		}
	}

    // set vertex in the core submesh instance
    pCoreSubmesh->setVertex(vertexId, vertex);

    // load the physical property of the vertex if there are springs in the core submesh
    if(springCount > 0)
    {
      CalCoreSubmesh::PhysicalProperty physicalProperty;

      // load data of the physical property
      mf.read((char *)&physicalProperty.weight, 4);

      // check if an error happend
      if(mf.eof())
      {
        CalError::setLastError(CalError::INVALID_FILE_FORMAT, __FILE__, __LINE__, "Error load CoreSubmesh");
        pCoreSubmesh->destroy();
        delete pCoreSubmesh;
        return 0;
      }

      // set the physical property in the core submesh instance
      pCoreSubmesh->setPhysicalProperty(vertexId, physicalProperty);
    }
  }

  // load all springs
  int springId;
  for(springId = 0; springId < springCount; springId++)
  {
    CalCoreSubmesh::Spring spring;

    // load data of the spring
    mf.read((char *)&spring.vertexId[0], 4);
    mf.read((char *)&spring.vertexId[1], 4);
    mf.read((char *)&spring.springCoefficient, 4);
    mf.read((char *)&spring.idleLength, 4);

    // check if an error happend
    if(mf.eof())
    {
      CalError::setLastError(CalError::INVALID_FILE_FORMAT, __FILE__, __LINE__, "Error load CoreSubmesh");
      pCoreSubmesh->destroy();
      delete pCoreSubmesh;
      return 0;
    }

    // set spring in the core submesh instance
    pCoreSubmesh->setSpring(springId, spring);
  }

  // load all faces
  int faceId;
  for(faceId = 0; faceId < faceCount; faceId++)
  {
    CalCoreSubmesh::Face face;

    // load data of the face
    mf.read((char *)&face.vertexId[0], sizeof(face.vertexId[0]));
    mf.read((char *)&face.vertexId[1], sizeof(face.vertexId[1]));
    mf.read((char *)&face.vertexId[2], sizeof(face.vertexId[2]));


    // check if an error happend
    if(mf.eof())
    {
      CalError::setLastError(CalError::INVALID_FILE_FORMAT, __FILE__, __LINE__, "Error load CoreSubmesh");
      pCoreSubmesh->destroy();
      delete pCoreSubmesh;
      return 0;
    }

    // set face in the core submesh instance
    pCoreSubmesh->setFace(faceId, face);
  }

  return pCoreSubmesh;
}

CalCoreMaterial *CalLoader::loadCoreMaterial(void *apData, DWORD adwSize)
{
	CalMemFile mf;
	CFace	*pFace = NULL;

	mf.init( apData, adwSize);

  // check if this is a valid file
  unsigned int magic;
  mf.read((char *)&magic, sizeof(magic));
  if(mf.eof() || (memcmp(&magic, Cal::MATERIAL_FILE_MAGIC, 4) != 0))
  {
    CalError::setLastError(CalError::INVALID_FILE_FORMAT, __FILE__, __LINE__, "Error load CoreMaterial");
    return 0;
  }

  // check if the version is compatible with the library
  unsigned int version;
  mf.read((char *)&version, sizeof(version));
  if(mf.eof() || (version < Cal::EARLIEST_COMPATIBLE_FILE_VERSION) || (version > Cal::CURRENT_FILE_VERSION))
  {
    CalError::setLastError(CalError::INCOMPATIBLE_FILE_VERSION, __FILE__, __LINE__, "Error load CoreMaterial");
    return 0;
  }

  // allocate a new core material instance
  CalCoreMaterial *pCoreMaterial;
  pCoreMaterial = new CalCoreMaterial();
  if(pCoreMaterial == 0)
  {
    CalError::setLastError(CalError::MEMORY_ALLOCATION_FAILED, __FILE__, __LINE__);
    return 0;
  }

  // create the core material instance
  if(!pCoreMaterial->create())
  {
    delete pCoreMaterial;
    return 0;
  }

  // get the ambient color of the core material
  CalCoreMaterial::Color ambientColor;
  mf.read((char *)&ambientColor.red, 1);
  mf.read((char *)&ambientColor.green, 1);
  mf.read((char *)&ambientColor.blue, 1);
  mf.read((char *)&ambientColor.alpha, 1);

  // get the diffuse color of the core material
  CalCoreMaterial::Color diffuseColor;
  mf.read((char *)&diffuseColor.red, 1);
  mf.read((char *)&diffuseColor.green, 1);
  mf.read((char *)&diffuseColor.blue, 1);
  mf.read((char *)&diffuseColor.alpha, 1);

  // get the specular color of the core material
  CalCoreMaterial::Color specularColor;
  mf.read((char *)&specularColor.red, 1);
  mf.read((char *)&specularColor.green, 1);
  mf.read((char *)&specularColor.blue, 1);
  mf.read((char *)&specularColor.alpha, 1);

  // get the emissive color of the core material
  CalCoreMaterial::Color emissiveColor;
  if( version >= 1000)
  {
	  mf.read((char *)&emissiveColor.red, 1);
	  mf.read((char *)&emissiveColor.green, 1);
	  mf.read((char *)&emissiveColor.blue, 1);
	  mf.read((char *)&emissiveColor.alpha, 1);
  }

  // get the shininess factor of the core material
  float shininess;
  mf.read((char *)&shininess, 4);
  shininess *= 100;

  // check if an error happend
  if(mf.eof())
  {
    CalError::setLastError(CalError::INVALID_FILE_FORMAT, __FILE__, __LINE__, "Error load CoreMaterial");
    pCoreMaterial->destroy();
    delete pCoreMaterial;
    return 0;
  }

  // set the colors and the shininess
  pCoreMaterial->setAmbientColor(ambientColor);
  pCoreMaterial->setDiffuseColor(diffuseColor);
  pCoreMaterial->setSpecularColor(specularColor);
  if( version >= 1000)
	pCoreMaterial->setEmissiveColor(emissiveColor);
  pCoreMaterial->setShininess(shininess);

  // read the number of maps
  int mapCount;
  mf.read((char *)&mapCount, 4);
  if(mf.eof())
  {
    CalError::setLastError(CalError::INVALID_FILE_FORMAT, __FILE__, __LINE__, "Error load CoreMaterial");
    return 0;
  }

  // reserve memory for all the material data
  if(!pCoreMaterial->reserve(mapCount))
  {
    CalError::setLastError(CalError::MEMORY_ALLOCATION_FAILED, __FILE__, __LINE__, "Error load CoreMaterial");
    pCoreMaterial->destroy();
    delete pCoreMaterial;
    return 0;
  }

  // load all maps
  int mapId;
  for(mapId = 0; mapId < mapCount; mapId++)
  {
    CalCoreMaterial::Map map;

    // get the filename length of the map
    int len;
    mf.read((char *)&len, 4);
    if(len < 1)
    {
      CalError::setLastError(CalError::INVALID_FILE_FORMAT, __FILE__, __LINE__, "Error load CoreMaterial");
      pCoreMaterial->destroy();
      delete pCoreMaterial;
      return 0;
    }

    // read the filename of the map
    char *strBuffer;
    strBuffer = new char[len];
    mf.read(strBuffer, len);
    map.strFilename = strBuffer;
    delete [] strBuffer;

	if (version >= 1002)
		mf.read(&map.type, sizeof(map.type));

    // initialize the user data
    map.userData = 0;

    // check if an error happend
    if(mf.eof())
    {
      CalError::setLastError(CalError::INVALID_FILE_FORMAT, __FILE__, __LINE__, "Error load CoreMaterial");
      pCoreMaterial->destroy();
      delete pCoreMaterial;
      return 0;
    }

    // set map in the core material instance
    pCoreMaterial->setMap(mapId, map);
  }

  return pCoreMaterial;
}

void CalLoader::deleteFAPU(CMpeg4FDP* pf)
{
	delete pf;
}

CMpeg4FDP *CalLoader::loadFAPU(void *apData, DWORD adwSize)
{
	CalMemFile file;
	CFace	*pFace = NULL;

	file.init( apData, adwSize);
	// check if this is a valid file
	unsigned int magic;
	file.read((char *)&magic, sizeof(magic));
	if(file.eof() || (memcmp(&magic, Cal::FAPU_FILE_MAGIC, 4) != 0))
	{
		CalError::setLastError(CalError::INVALID_FILE_FORMAT, __FILE__, __LINE__, "Error load CoreMaterial");
		return 0;
	}

	// check if the version is compatible with the library
	unsigned int version;
	file.read((char *)&version, sizeof(version));
	if(file.eof() || (version < Cal::EARLIEST_COMPATIBLE_FILE_VERSION) || (version > Cal::CURRENT_FILE_VERSION))
	{
		CalError::setLastError(CalError::INCOMPATIBLE_FILE_VERSION, __FILE__, __LINE__, "Error load CoreMaterial");
		return 0;
	}

	// read data
	CMpeg4FDP* face = new CMpeg4FDP();

	// read FAPU parameters
	file.read((char *)&face->IRISD0, sizeof(face->IRISD0));
	file.read((char *)&face->ES0, sizeof(face->ES0));
	file.read((char *)&face->ENS0, sizeof(face->ENS0));
	file.read((char *)&face->MNS0, sizeof(face->MNS0));
	file.read((char *)&face->MW0, sizeof(face->MW0));
	file.read((char *)&face->AU, sizeof(face->AU));

	face->IRISD0 = abs( face->IRISD0);
	face->ES0 = abs( face->ES0);
	face->ENS0 = abs( face->ENS0);
	face->MNS0 = abs( face->MNS0);
	face->MW0 = abs( face->MW0);
	face->AU = abs( face->AU);

	// read the control points num
	file.read((char *)&face->cp_num, sizeof(int));


	for( int ig=0; ig<face->cp_num; ig++)
	{
		control_point*	point = new control_point();

		// read MPEG4 group_id, in_group_id and neutral vector
		//file.read((char *)&point->group_id, (DWORD)&point->points - (DWORD)&point->group_id);
		// write MPEG4 group_id, in_group_id and neutral vector
		file.read((char *)&point->group_id, sizeof(int));
		file.read((char *)&point->id, sizeof(int));
		file.read((char *)&point->v_neutral, sizeof(CalVector));


		if( version > 1010)
		{
			file.read((char *)&point->mesh_id, sizeof(int));
			file.read((char *)&point->submesh_id, sizeof(int));
			file.read((char *)&point->point_id, sizeof(int));

			submesh_face_point	sp;
			sp.mesh_id = point->mesh_id;
			sp.submesh_id = point->submesh_id;
			sp.id = point->id;
			sp.parent_gid = point->group_id;
			sp.parent_id = point->id;
			face->control_points.push_back(sp);
		}

		point->v_current = point->v_neutral;

		// read influence points num
		int isize = 0;
		file.read((char *)&isize, sizeof(int));

		if( isize == 0)
		{
			submesh_face_point	sp;

			sp.mesh_id = point->mesh_id;
			sp.submesh_id = point->submesh_id;
			sp.id = point->point_id;
			sp.weight = 1.0f;
			sp.parent_gid = point->group_id;
			sp.parent_id = point->id;
			point->points.push_back( sp);
		}

		//point->points.reserve( isize + 10);

		// read all points
		for( int ip=0; ip<isize; ip++)
		{
			submesh_face_point	sp;
			file.read((char *)&sp.mesh_id, sizeof(int));
			file.read((char *)&sp.submesh_id, sizeof(int));
			file.read((char *)&sp.id, sizeof(int));
			file.read((char *)&sp.weight, sizeof(float));
			file.read((char *)&sp.parent_gid, sizeof(int));
			file.read((char *)&sp.parent_id, sizeof(int));

			sp.parent_gid = point->group_id;
			sp.parent_id = point->id;

			point->points.push_back( sp);
			//
			if( version <= 1010)
			{
				if( sp.weight == 1)
					face->control_points.push_back(sp);
			}
		}
		face->n_points[point->group_id][point->id] = point;
	}

	return face;
}

FACEExpressionsPack* CalLoader::loadFAP(void *apData, DWORD adwSize)
{
	FACEExpressionsPack*	pack = new FACEExpressionsPack();
	pack->pexpressions.reserve(100);
	FACEExpression*	faps[100];
	//pack->count = loadFAP(apData, adwSize, pack->pexpressions.begin());
	pack->count = loadFAP(apData, adwSize, (FACEExpression**)&faps);
	for( int i=0; i<pack->count; i++)
		pack->add_expression(faps[i]);
	return pack;
}

void CalLoader::deleteFAP(FACEExpressionsPack* pack)
{
	if( pack)
		delete pack;
}

int CalLoader::loadFAP(void *apData, DWORD adwSize, FACEExpression** vector)
{
  CalMemFile file;
  CFace	*pFace = NULL;

  file.init( apData, adwSize);

  // check if this is a valid file
	unsigned int magic;
	file.read((char *)&magic, sizeof(magic));
	if(file.eof() || (memcmp(&magic, Cal::FAP_FILE_MAGIC, 4) != 0))
	{
		CalError::setLastError(CalError::INVALID_FILE_FORMAT, __FILE__, __LINE__, "Error load CoreMaterial");
		return 0;
	}

	// check if the version is compatible with the library
	unsigned int version;
	file.read((char *)&version, sizeof(version));
	if(file.eof() || (version < Cal::EARLIEST_COMPATIBLE_FILE_VERSION) || (version > Cal::CURRENT_FILE_VERSION))
	{
		CalError::setLastError(CalError::INCOMPATIBLE_FILE_VERSION, __FILE__, __LINE__, "Error load CoreMaterial");
		return 0;
	}

  int retCount=0;
  // write FAPU parameters
  file.read((char *)&retCount, sizeof(int));

	// write the control points num
	for( int ig=0; ig<retCount; ig++)
	{
		FACEExpression*	fe;

		Cal3DComString	sname;
		// write name
		int	fl = 0;
		file.read((char *)&fl, sizeof(int));
		//sname.resize(fl+1, (char)0);
		file.read((char *)sname.GetBufferSetLength(fl+1), fl);

		//FACEExpression*	fe = new FaceExpression();
		
		FACEExpression::FACE_TYPE	type;
		// write expression type
		file.read((char *)&type, sizeof(FACEExpression::FACE_TYPE));

		// write FACEExpression::FT_EXPRESSION parameters
		if( type == FACEExpression::FT_EXPRESSION)
		{
			MPEGExpression*	expr = new MPEGExpression();
			fe = new FACEExpression(expr);

			int	fs = 0;
			file.read((char *)&fs, sizeof(int));
			expr->faps.resize(fs);

			for( int i=0; i<fs; i++)
			{
				fap_on_face	fof;
				file.read((char *)&fof.fap_id, sizeof(int));
				file.read((char *)&fof.base_shift, sizeof(CalVector));
				file.read((char *)&fof.base_unit, sizeof(float));
				file.read((char *)&fof.group_id, sizeof(int));
				file.read((char *)&fof.subgroup_id, sizeof(int));
				expr->faps[i] = fof;
			}
		}

		// write FACEExpression::FT_VISEME parameters
		if( type == FACEExpression::FT_VISEME)
		{
			MPEGViseme*	viseme = new MPEGViseme();
			fe = new FACEExpression(viseme);

			if( version > 1009)
			{
				// write viseme FDP parameters
				file.read((char *)&viseme->units, sizeof(viseme->units));
				file.read((char *)&viseme->iris, sizeof(viseme->iris));
				file.read((char *)&viseme->es, sizeof(viseme->es));
				file.read((char *)&viseme->ens, sizeof(viseme->ens));
				file.read((char *)&viseme->mns, sizeof(viseme->mns));
				file.read((char *)&viseme->mw, sizeof(viseme->mw));
			}

			int	ps = 0;
			file.read((char *)&ps, sizeof(int));
			viseme->points.resize(ps);
			for( int i=0; i<ps; i++)
			{
				viseme_cp	bcp;
				file.read((char *)&bcp, sizeof(bcp));
				viseme->points[i] = bcp;
			}
		}

		fe->name += sname;
		*vector = fe;
		vector++;
	}

  return retCount;
}