#ifndef __ARTICULATED_FIGURE_H__
#define __ARTICULATED_FIGURE_H__

#include "bvhParams.h"

class ArticulatedFigure
{
	friend class BVHParser;
//--------------- Internal Class Definitions -----------
public:
    class Node;
    typedef std::vector<Node>     NodeList;
    typedef std::vector<Node *>   NodePtrList;
    typedef NodePtrList::iterator NodePtrListIt;

    //------------- Keeping track of Channel positions -----------
    class ParamMappingArr
    {
    public:
        ParamMappingArr()
        {
            for( int i = 0; i < 6; i++ ) pArr[i] = INVALID_PLCMNT;
        }

        ParamPlacement pArr[6];
    };

    //------------- The Basic Node Structure -----------
    class Node {
    public:
        Node( std::string &inName, CalVector &inPos );
        ~Node();
        Node( const Node &inNode );

        void setNumChannels( int inNumChannels ){ pNumChannels = inNumChannels; }

    public:
        std::string     pNodeName;
        int             pNumChannels;
        ParamMappingArr pChannelMapping;
        CalVector       offset;				// из файла
		CalVector       positionAbsolute;	// рассчитанная позиция
		CalVector       direction;			// рассчитанное направление
		CalQuaternion   rotation;			// повороты из файла
        PointVec        pOffsets;			// анимация офсетов
		MatrixVec		matrices;

        //VERY IMPORTANT: order to perform these rotations is:
        // Xrotation Zrotation Yrotation - according to BVH file format
        PointVec        pRotations;

        NodePtrList     pChildren; //child nodes

		// filtered data
		PointVec		pFilteredOffsets;
        PointVec		pFilteredRotations;

		//first derivative of the motions - calculated when figure is loaded.
        PointVec        pOffsetDiff;
		PointVec        pRotationDiff;
    };

//--------------- Public Methods -----------
public:
    ArticulatedFigure();
    ~ArticulatedFigure();

	double getMinY();
    void setRuntimeParamters( int inNumFrames, double inFrameTime );
	double getMaxOffsetDistance(){ return mMaxOffsetDistance; }
	double getJointRadius(){ return getMaxOffsetDistance()/50.0; }
	CalVector getInitialOffset();
	CalVector getBodyCenter();
	int getNumOfFrames(){ return mNumFrames; }
	bool isFiltered(){ return mIsFiltered; }
	double getFrameTime(){ return mFrameTime==0?1:mFrameTime; }
	void toggleCoupled();
	bool getCoupled(){ return mIsCoupled; }

//--------------- Storage -----------
    NodePtrList mRootNodes; //can be more than one root node!
protected:
    int         mNumFrames;
    double      mFrameTime;
	bool		mIsFiltered;
	bool		mIsCoupled;

	CalVector		mMaxOffset;
	CalVector		mMinOffset;
	double			mMaxOffsetDistance;
	CalVector		mTotalOffset;
	CalVector		mBodyCenter;
	int				mNodesCounter;
};

#endif //__ARTICULATED_FIGURE_H__

