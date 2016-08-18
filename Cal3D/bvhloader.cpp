//****************************************************************************//
// loader.cpp                                                                 //
// Copyright (C) 2001, 2002 Bruno 'Beosil' Heidelberger                       //
//****************************************************************************//
// This library is free software; you can redistribute it and/or modify it    //
// under the terms of the GNU Lesser General Public License as published by   //
// the Free Software Foundation; either version 2.1 of the License, or (at    //
// your option) any later version.                                            //
//****************************************************************************//

#include "StdAfx.h"
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

//****************************************************************************//
// Includes                                                                   //
//****************************************************************************//

#include "bvhloader.h"
#include "bvhParser.h"
#include "bvhFigure.h"
#include "coreanimation.h"
#include "corebvhanimation.h"
#include "corekeyframe.h"
#include "coretrack.h"
#include "quaternion.h"
#include "vector.h"
#include <algorithm>
#include "memory_leak.h"

/*****************************************************************************/
/** Constructs the loader instance.
  *
  * This function is the default constructor of the loader instance.
  *****************************************************************************/

const float CalBvhLoader::m_pi = 3.1415926535897f;

CalBvhLoader::CalBvhLoader()
{
	
}

 /*****************************************************************************/
/** Destructs the loader instance.
  *
  * This function is the destructor of the loader instance.
  *****************************************************************************/

CalBvhLoader::~CalBvhLoader()
{
}

CalCoreAnimation*	CalBvhLoader::loadBVH(void *apData, DWORD adwSize, int& errorCode)
{
	ArticulatedFigure figure;
	bool ok = true;
	BVHParser parser(apData, adwSize, figure);
	parser.parse(ok, errorCode);
	if (!ok)	return NULL;
	//
	if (figure.mRootNodes.size() == 0)
	{
		errorCode = -1;
		return NULL;
	}
	//
	CalCoreBvhAnimation*	pCoreAnimation = MP_NEW( CalCoreBvhAnimation);
	pCoreAnimation->m_version = 1009;
	pCoreAnimation->setRelMode(false);
	pCoreAnimation->saveFState(false);
	pCoreAnimation->saveLState(false);
	if( figure.getNumOfFrames() != 1)
		pCoreAnimation->setDuration((float)((figure.getNumOfFrames()-1) * figure.getFrameTime()));
	else
		pCoreAnimation->setDuration((float)(figure.getFrameTime()));

	pCoreAnimation->setBipUpPosition( (float)fabs(figure.getMinY() - figure.getInitialOffset()[1]));

	for (unsigned int i=0; i<figure.mRootNodes.size(); i++)
	{
		/*char buf[10];
		itoa( figure.mRootNodes.size(), buf, 10);
		MessageBox( 0, buf, "1", MB_OK);

		itoa( (long)figure.mRootNodes[i], buf, 10);
		MessageBox( 0, buf, "2", MB_OK);
		*/

		ArticulatedFigure::Node* pNode = figure.mRootNodes[i];
		addCoreTracks(pNode, &figure, pCoreAnimation, NULL);
	}
	pCoreAnimation->setBVH(true);
	return pCoreAnimation;
}

std::string	bonesNamesMap[] = 
{	"hip", "bip",
	"abdomen", "pelvis",
	"chest", "spine",
	"neckdummy", "",
	"neck", "neck",
	"head", "head",
	"figurehair", "",
	"lcollar", "l clavicle",
	"leftcollar", "l clavicle",
	"lshldr", "l upperarm",
	"leftuparm", "l upperarm",
	"lforearm", "l forearm",
	"leftlowarm", "l forearm",
	"lhand", "l hand",
	"lefthand", "l hand",
	"rcollar", "r clavicle",
	"rightcollar", "r clavicle",	
	"rshldr", "r upperarm",
	"rightuparm", "r upperarm",
	"rforearm", "r forearm",
	"rightlowarm", "r forearm",
	"rhand", "r hand",
	"righthand", "r hand",
	"lthigh", "l thigh",
	"leftupleg", "l thigh",
	"lshin", "l calf",
	"leftlowleg", "l calf",
	"lfoot", "l foot",
	"leftfoot", "l foot",
	"rthigh", "r thigh",
	"rightupleg", "r thigh",
	"rshin", "r calf",
	"rightlowleg", "r calf",
	"rfoot", "r foot",
	"rightfoot", "r foot"
};

void	CalBvhLoader::addCoreTracks(ArticulatedFigure::Node* pNode, 
									ArticulatedFigure* figure, 
									CalCoreAnimation* pCoreAnimation, 
									ArticulatedFigure::Node* pParentNode)
									//CalCoreTrack* pParentCalTrack)
{
	assert(pNode);
	assert(pCoreAnimation);
	
	CalCoreTrack* pCoreTrack = MP_NEW( CalCoreTrack);

	std::string	nodeName = pNode->pNodeName;
	// by Vladimir
	//if ( nodeName.size() > 0)
		transform(nodeName.begin(), nodeName.end(), nodeName.begin(), tolower);

	int i;
	for (i = 0; i < sizeof(bonesNamesMap) / sizeof(bonesNamesMap[0]); i += 2)
	{
		if (nodeName == bonesNamesMap[i])
		{
			nodeName = bonesNamesMap[i+1];
			break;
		}
	}

	//std::map<float, CalCoreKeyframe*> keyframes;
	//std::map<float, CalCoreKeyframe*>::iterator iter;

	MatrixVec::iterator	itParentMatrices;
	if (pParentNode)
	{
		//keyframes = pParentCalTrack->getMapCoreKeyframe();
		//iter = keyframes.begin();
		itParentMatrices = pParentNode->matrices.begin();
	}

	pNode->matrices.reserve( figure->getNumOfFrames());
	pCoreTrack->setCoreBoneName((LPSTR)nodeName.c_str());
	for (i = 0; i < figure->getNumOfFrames(); i++)
	{
		CalCoreKeyframe* pKeyFrame = MP_NEW( CalCoreKeyframe);

		CalVector position(0, 0, 0);
		CalVector direction(0, 0, 0);
		CalQuaternion rotation(0, 0, 0, 1);
				
		if (pParentNode)
		{
			direction.set(pNode->direction.x, pNode->direction.y, pNode->direction.z);
			position.set(pNode->offset.x, pNode->offset.y, pNode->offset.z);
		}
		else
		{
			CalVector boneOffset = pNode->pOffsets[i];
			//position.set(pNode->pOffsets[i].x, pNode->pOffsets[i].y, pNode->pOffsets[i].z);
			position.set(pNode->pOffsets[i].x, -pNode->pOffsets[i].z, pNode->pOffsets[i].y);
			direction = position;
		}

		D3DXMATRIX	mOut, mx, my, mz;

		D3DXMatrixIdentity(&mOut);

		D3DXQUATERNION qOut;

		if (i < (int)pNode->pRotations.size())
		{
			CalVector euler = pNode->pRotations[i];
			
			D3DXMatrixRotationX(&mx, to_rad(euler.x));
			D3DXMatrixRotationY(&my, to_rad(euler.y));
			D3DXMatrixRotationZ(&mz, to_rad(euler.z));

			for( int k = sizeof(pNode->pChannelMapping.pArr) / sizeof(pNode->pChannelMapping.pArr[0])-1; k>=0; k--)
			{
				if( pNode->pChannelMapping.pArr[k] == XROT)
				{
					D3DXMatrixMultiply(&mOut, &mOut, &mx);
				}
				else	if( pNode->pChannelMapping.pArr[k] == YROT)
				{
					D3DXMatrixMultiply(&mOut, &mOut, &my);
				}
				else	if( pNode->pChannelMapping.pArr[k] == ZROT)
				{
					D3DXMatrixMultiply(&mOut, &mOut, &mz);
				}
			}
		}

		mOut._41 = position.x;
		mOut._42 = position.y;
		mOut._43 = position.z;

		if (pParentNode)
		{
			//D3DXMATRIX matrix = (*iter).second->getMatrix();
			D3DXMATRIX matrix = *itParentMatrices;
			D3DXMatrixMultiply(&mOut, &mOut, &matrix);
			//pKeyFrame->setParentKeyFrame((*iter).second);
			itParentMatrices++;
		}

		float vals[4][4] = {
			{mOut._11, mOut._12, mOut._13, mOut._14},
			{mOut._21, mOut._22, mOut._23, mOut._24},
			{mOut._31, mOut._32, mOut._33, mOut._34},
			{mOut._41, mOut._42, mOut._43, mOut._44}
		};
		rotation = CalQuaternion(vals);
		rotation.conjugate();
		//pKeyFrame->setMatrix(mOut);
		
		float time = i * (float)figure->getFrameTime();
		pKeyFrame->setRotation(rotation);
		pKeyFrame->setTranslation(direction);
		pKeyFrame->setTime(time);

		pNode->matrices.push_back(mOut);

		pCoreTrack->addCoreKeyframe(pKeyFrame);
	}

	if( nodeName != "")
		pCoreAnimation->addCoreTrack(pCoreTrack);
	else
	{
		pCoreTrack->destroy();
		MP_DELETE( pCoreTrack);
	}

	for (unsigned int j = 0; j < pNode->pChildren.size(); j++)
	{
		addCoreTracks(pNode->pChildren[j], figure, pCoreAnimation, pNode);
	}
}