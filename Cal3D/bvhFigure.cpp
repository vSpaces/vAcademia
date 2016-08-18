#include "StdAfx.h"

#include "bvhFigure.h"

#include <iostream>
#include <fstream>
#include <string>
#include "memory_leak.h"
using namespace std;

#define DEG2RAD 0.0174532925
#define RAD2DEG 57.2957795

extern float mat_diffuse[];
extern float mat_diffuse_tr[];

ArticulatedFigure::Node::Node( string &inName, Point3d &inPos ): 
	MP_STRING_INIT(pNodeName),
	MP_VECTOR_INIT(pOffsets),
	MP_VECTOR_INIT(matrices),
	MP_VECTOR_INIT(pRotations),
	MP_VECTOR_INIT(pChildren),
	MP_VECTOR_INIT(pFilteredOffsets),
	MP_VECTOR_INIT(pFilteredRotations),
	MP_VECTOR_INIT(pOffsetDiff),
	MP_VECTOR_INIT(pRotationDiff),
	pNumChannels(0),
	offset(inPos),
	rotation(0,0,0,1)
{
	pNodeName = inName;
}

ArticulatedFigure::Node::~Node()
{
    NodePtrListIt it = pChildren.begin();
    for( ; it != pChildren.end(); it++ )
    {
        Node *ptr = *it;
        MP_DELETE( ptr);
    }
}

ArticulatedFigure::Node::Node( const Node &inRhs ):
	MP_STRING_INIT(pNodeName),
	MP_VECTOR_INIT(pOffsets),
	MP_VECTOR_INIT(matrices),
	MP_VECTOR_INIT(pRotations),
	MP_VECTOR_INIT(pChildren),
	MP_VECTOR_INIT(pFilteredOffsets),
	MP_VECTOR_INIT(pFilteredRotations),
	MP_VECTOR_INIT(pOffsetDiff),
	MP_VECTOR_INIT(pRotationDiff)
{
    pNodeName    = inRhs.pNodeName;
    pNumChannels = inRhs.pNumChannels;
    offset		 = inRhs.offset;
	positionAbsolute = inRhs.positionAbsolute;
    pOffsets     = inRhs.pOffsets;
    pRotations   = inRhs.pRotations;
    pChildren    = inRhs.pChildren;
}
								   
ArticulatedFigure::ArticulatedFigure(void):
	MP_VECTOR_INIT(mRootNodes)
{
    mFrameTime  = -1;
    mNumFrames	= -1;
	mIsFiltered	= false;
	mIsCoupled = false;

	mMaxOffset[0] = mMaxOffset[1] = mMaxOffset[2] = 0;
	mMinOffset[0] = mMinOffset[1] = mMinOffset[2] = 100;
	mTotalOffset[0] = mTotalOffset[1] = mTotalOffset[2] = 0;
	mBodyCenter[0] = mBodyCenter[1] = mBodyCenter[2] = 0;
	mNodesCounter = 0;
}

ArticulatedFigure::~ArticulatedFigure(void)
{
    NodePtrListIt it = mRootNodes.begin();
    for( ; it != mRootNodes.end(); it++ )
    {
        Node *ptr = *it;
        MP_DELETE( ptr);
    }
}

void 
ArticulatedFigure::setRuntimeParamters( int inNumFrames, double inFrameTime )
{
    mFrameTime = inFrameTime;
	if (mNumFrames > 0)
		//mNumFrames = min(mNumFrames,inNumFrames);
		mNumFrames = max(mNumFrames,inNumFrames);
	else
		mNumFrames = inNumFrames;
}

Point3d ArticulatedFigure::getInitialOffset()
{
	if (mRootNodes.size() == 0 || mRootNodes[0]->pOffsets.size() == 0){
		Point3d initialOffset(0,0,0);
		return initialOffset;
	}
	//NOTE: I assume everything is set according to the first body
	return mRootNodes[0]->pOffsets[0];
}

double ArticulatedFigure::getMinY()
{
	Point3d minPoint = mMinOffset + getInitialOffset();
	return minPoint[1];
}

Point3d ArticulatedFigure::getBodyCenter()
{ 
	Point3d midPoint = mBodyCenter + getInitialOffset();
	return midPoint; 
}

void ArticulatedFigure::toggleCoupled()
{
	mIsCoupled = !mIsCoupled;
}

