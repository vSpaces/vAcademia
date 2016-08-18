
#include "StdAfx.h"
#include "BVHParser.h"
#include <cctype> //tolower
#include <algorithm> //transform
#include "memory_leak.h"

//Make the code a little more readable
typedef ArticulatedFigure::Node            Node;
typedef ArticulatedFigure::ParamMappingArr ParamMappingArr;

BVHParser::BVHParser( string &inFilename, ArticulatedFigure &inFigure, bool &outOK ) :
                mInputStream(inFilename.c_str(), ios_base::in ),
                mArticulatedFigure( inFigure ),
				MP_VECTOR_INIT(mLinearNodeList)
{
    outOK = mInputStream.good();
	buffer = NULL;
	buffer_size = 0;
	buffer_index = 0;
}

BVHParser::BVHParser( void *apData, DWORD adwSize, ArticulatedFigure &inFigure): 
	mArticulatedFigure( inFigure ),
	MP_VECTOR_INIT(mLinearNodeList)
{
	if( adwSize > 0)
	{
		buffer = MP_NEW_ARR( char, adwSize);
		memcpy( buffer, apData, adwSize);
	}
	buffer_index = 0;
}

BVHParser::~BVHParser()
{
    if( mInputStream.is_open() )
        mInputStream.close();
	if( buffer)
		MP_DELETE_ARR( buffer);
}

#define CHECK_STR(str_,cmpStr_,errStr_)    { \
                                                string bla_ = (str_); \
                                                if( bla_ != string(cmpStr_) )\
                                                { cout << "ERROR:" << (errStr_) << " (" << bla_ << ")" <<endl; break;}\
                                             }

#define CHECK_TOKEN(cmpStr_, errStr_ ) CHECK_STR( getNextToken(), cmpStr_, errStr_ )

void
BVHParser::parse(bool &outParseSuccesfull, int& errorCode )
{
	locale loc1 ( "English_Britain" );

	errorCode = 0;

    string token;
    bool   readOK = false;

    outParseSuccesfull = false;

	Point3d curOffset(0,0,0);
    
    do {
        //------- Read Start Token ----------
        CHECK_TOKEN( "hierarchy", "invalid BVH start token" );

        //------ Read Hierarchy -----------
        CHECK_TOKEN( "root", "expected 'ROOT' token" );

        readHierarchy( NULL, false, readOK, curOffset ); if( !readOK ) break;

        //------ Read Frames ---------
        CHECK_TOKEN( "motion", "expected 'MOTION' token" );
        readFrames( readOK ); if( !readOK ) break;

		//------ Calculate mass center of the body --------
		mArticulatedFigure.mBodyCenter = mArticulatedFigure.mTotalOffset / (float)mArticulatedFigure.mNodesCounter;

		mArticulatedFigure.mMaxOffsetDistance = max( mArticulatedFigure.mMaxOffset.x,
													mArticulatedFigure.mMaxOffset.y);
		mArticulatedFigure.mMaxOffsetDistance = max( mArticulatedFigure.mMaxOffsetDistance,
														mArticulatedFigure.mMaxOffset.z);
        outParseSuccesfull = true;
    } while(0);  

	if (!outParseSuccesfull)
	{
		errorCode = 1;
	}
}

void 
BVHParser::readFrames( bool &outParseSuccesfull )
{
    outParseSuccesfull = false;

	setlocale( LC_NUMERIC, "English_Britain" );
    do {
        //------------ Read # of frames ---------
        CHECK_TOKEN( "frames:", "expected 'Frames:' token" );

        int numFrames = 0;
        numFrames = rtl_atoi( getNextToken().c_str() );

        if( !(numFrames > 0) )
        {
            cout << "ERROR: invalid number of frames: (" << numFrames << ")" << endl;
            break;
        }

        //------------ Read FPS ---------
        CHECK_TOKEN( "frame", "expected 'frame' token" );
        CHECK_TOKEN( "time:", "expected 'time:' token" );
        double fps = 0;

        fps = atof( getNextToken().c_str() );
        if (!(fps > 0))
        {
            cout << "ERROR: invalid fps value: (" << fps << ")" << endl;
            break;
        }

        //------------ Optimization - reserve space in vectors ---------------
        for (ArticulatedFigure::NodePtrListIt it = mLinearNodeList.begin(); 
                                             it != mLinearNodeList.end(); it++)
        {
            Node *n = *it;
            
            n->pOffsets.reserve( numFrames );
            n->pRotations.reserve( numFrames );
        }

        //------------ Read movement vectors --------
        mArticulatedFigure.setRuntimeParamters( numFrames, fps );

        bool readMotionsOK = true;
        for (int i = 0; i < numFrames && readMotionsOK; i++)
        {
            ArticulatedFigure::NodePtrListIt it = mLinearNodeList.begin();
            for( ; it != mLinearNodeList.end() && readMotionsOK; it++ )
            {
                Node *n = *it;
                ParamMappingArr &pMap = n->pChannelMapping;

                readNodeMotions( n, pMap, readMotionsOK );
            }
        }

        //if there was a problem reading the motion parameters for a single frame
        if( !readMotionsOK ) break;

        outParseSuccesfull = true;
    } while(0);
	setlocale( LC_NUMERIC, "Russian" );
}

void
BVHParser::readNodeMotions( ArticulatedFigure::Node *inNodePtr, 
                    ArticulatedFigure::ParamMappingArr &inArr, bool &outParseOK )
{
    float xRot = 0, yRot = 0, zRot = 0;
    float xPos = 0, yPos = 0, zPos = 0;

    outParseOK = false;

    int numChannels = inNodePtr->pNumChannels;

    if( numChannels == 0 )
    {
        outParseOK = true; //nothing to read
    }
    else
    {
        do {
            
            if( !mInputStream.good() ) break;

            //MAP channels to parameters
            bool err = false;
            for( int i = 0; i < numChannels && !err; i++ )
            {
                string token = getNextToken();
                float num = (float)atof( token.c_str() );

                switch( inArr.pArr[i] )
                {
                case XPOS:
                    xPos = num;
                    break;
                case YPOS:
                    yPos = num;
                    break;
                case ZPOS:
                    zPos = num;
                    break;
                case XROT:
                    xRot = num;
                    break;
                case YROT:
                    yRot = num;
                    break;
                case ZROT:
                    zRot = num;
                    break;
                default:
                    cout << "ERROR: invalid value @ position " << i << endl;
                    err = true;
                    break;
                }

            }

            if( err ) break;

            //Finally, set parameters...
			if (inNodePtr->pOffsets.size() == 0)
			{
		        inNodePtr->pOffsetDiff.push_back( Point3d(0, 0, 0) );
	            inNodePtr->pRotationDiff.push_back( Point3d(0, 0, 0) );
			}
			else
			{
				inNodePtr->pOffsetDiff.push_back(Point3d(xPos, yPos, zPos)-inNodePtr->pOffsets[inNodePtr->pOffsets.size()-1]);
				inNodePtr->pRotationDiff.push_back(Point3d(xRot, yRot, zRot)-inNodePtr->pRotations[inNodePtr->pRotations.size()-1]);
			}
			inNodePtr->pOffsets.push_back( Point3d(xPos, yPos, zPos) );
            inNodePtr->pRotations.push_back( Point3d(xRot, yRot, zRot) );
            
            outParseOK = true;
        } while(0);
    }

}

void 
BVHParser::readHierarchy( ArticulatedFigure::Node *inParent, bool isEnd, bool &outParseSuccesfull, Point3d curOffset )
{
    outParseSuccesfull = false;
    string hierarchyName(""), token;
    ArticulatedFigure::Node *curNode = NULL;

    do {

        //----------- Read Hierarchy Name and '{' --------------
        token = getNextTokenPreserveCase();

        if( token !=  string("{") )
        {
            hierarchyName = token;
            token = getNextToken();
        }

        CHECK_STR( token, "{", "expected '{' at start of hierarchy" );

        //----------- Read Offset --------------
        CHECK_TOKEN( "offset", "expected 'OFFSET'" );

        float x, y, z;
        x = (float)atof(getNextToken().c_str());
        y = (float)atof(getNextToken().c_str());
        z = (float)atof(getNextToken().c_str());

		Point3d offset(x,y,z);

        if( inParent == NULL )
        {
            MP_NEW_P2( theNode, Node, hierarchyName, offset );
            mArticulatedFigure.mRootNodes.push_back(theNode);

            curNode = theNode;
            mLinearNodeList.push_back( theNode );
        }
        else
        {
            MP_NEW_P2( theNode, Node, hierarchyName, offset );
            inParent->pChildren.push_back( theNode );
			theNode->positionAbsolute = inParent->positionAbsolute + offset;
			if( inParent->pChildren.size() == 1)
			{
				inParent->direction = offset;
				if( inParent->direction.length() > 0)
				{
					CalVector baseAxe(1, 0, 0);
					inParent->rotation = CalQuaternion( inParent->direction.getNormal(baseAxe), 
														inParent->direction.getAngle(baseAxe));
				}
			}
            curNode = theNode;
            mLinearNodeList.push_back( theNode );
        }

		//---------- update offset details ------------
		mArticulatedFigure.mNodesCounter++;
		curOffset += offset;
		mArticulatedFigure.mTotalOffset += curOffset;
		mArticulatedFigure.mMaxOffset[0] = abs(curOffset[0]) > mArticulatedFigure.mMaxOffset[0]? 
			abs(curOffset[0]) : mArticulatedFigure.mMaxOffset[0];
		mArticulatedFigure.mMaxOffset[1] = abs(curOffset[1]) > mArticulatedFigure.mMaxOffset[1]? 
			abs(curOffset[1]) : mArticulatedFigure.mMaxOffset[1];
		mArticulatedFigure.mMaxOffset[2] = abs(curOffset[2]) > mArticulatedFigure.mMaxOffset[2]? 
			abs(curOffset[2]) : mArticulatedFigure.mMaxOffset[2];

		float xm = curOffset[0];//min(curOffset[0], mArticulatedFigure.mTotalOffset[0]);
		float ym = curOffset[1];//min(curOffset[1], mArticulatedFigure.mTotalOffset[1]);
		float zm = curOffset[2];//min(curOffset[2], mArticulatedFigure.mTotalOffset[2]);
		mArticulatedFigure.mMinOffset[0] = xm < mArticulatedFigure.mMinOffset[0]? 
			xm : mArticulatedFigure.mMinOffset[0];
		mArticulatedFigure.mMinOffset[1] = ym < mArticulatedFigure.mMinOffset[1]? 
			ym : mArticulatedFigure.mMinOffset[1];
		mArticulatedFigure.mMinOffset[2] = zm < mArticulatedFigure.mMinOffset[2]? 
			zm : mArticulatedFigure.mMinOffset[2];
        //----------- Read channels --------------
        if( !isEnd )
        {
            CHECK_TOKEN( "channels", "expected 'CHANNELS'" );
            int numChannels;
            numChannels = rtl_atoi( getNextToken().c_str() );

            if( numChannels != 6 && numChannels != 3)
            {
                cout << "ERROR: can only accept 3 or 6 channel BVH file got " << numChannels
                    << " channels instead" << endl;
                break;
            }

            curNode->setNumChannels( numChannels );

            //-------- Read off channel name tokens ------
            bool plcmntError = false;
            ParamMappingArr &plcmnt = curNode->pChannelMapping;
            for( int i = 0; i < numChannels; i++ )
            {
                token = getNextToken();

                if( token == string("xposition"))
                    plcmnt.pArr[i] = XPOS;
                else if( token == string("yposition"))
                    plcmnt.pArr[i] = YPOS;
                else if( token == string("zposition"))
                    plcmnt.pArr[i] = ZPOS;
                else if( token == string("xrotation"))
                    plcmnt.pArr[i] = XROT;
                else if( token == string("yrotation"))
                    plcmnt.pArr[i] = YROT;
                else if( token == string("zrotation"))
                    plcmnt.pArr[i] = ZROT;
                else
                {
                    cout  << "ERROR: invalid channel name (" << token << ")" << endl;
                    plcmntError = true;
                    break;
                }
            }

            if( plcmntError ) break;

            //-------- End site or joint? ------
       
            token = getNextToken();

            if( token != string("end") && token != string("joint"))
            {
                cout << "ERROR: was expecting End or JOINT, got \"" << token
                    << "\" instead" << endl;
                break;
            }

            bool readOK = false;

            readHierarchy( curNode, token == string("end"), readOK, curOffset ); 
			if( !readOK ) break;
        }

        bool nextJointReadOK = false;
        while(1)
        {
            token = getNextToken();
            
            bool isJoint = (token == string("joint"));

            if( !isJoint && token != string("}") )
            {
                cout << "ERROR: was expecting JOINT or } , got \"" << token
                    << "\" instead" << endl;
                break;
            }

            if( isJoint )
            {
                bool readOK = false;

                readHierarchy( curNode, false, readOK, curOffset ); 
				if(!readOK ) break;
            }
            else // read "}"
            {
                nextJointReadOK = true;
                break;
            }
        }

        if( !nextJointReadOK ) break;

        outParseSuccesfull = true;
    } while(0);
}

#undef CHECK_STR
#undef CHECK_TOKEN

bool
BVHParser::hasMoreTokens()
{
    return mInputStream.good();
}

string
BVHParser::getNextToken()
{
    string ret = getNextTokenPreserveCase();
	//convert to lowercase
    transform( ret.begin(),ret.end(), ret.begin(), tolower);
    return ret;
}

string 
BVHParser::getNextTokenPreserveCase()
{
    string ret;
	if( !buffer)
		mInputStream >> ret;
	else
	{
		while (true)
		{
			if( buffer_index == buffer_size)	break;
			char	ch = buffer[ buffer_index];

			typedef ctype<char> _Ctype;
			locale	alocale;
			const _Ctype& _Ctype_fac = _USE(alocale, _Ctype);
			buffer_index++;
			if ( _Ctype_fac.is( ctype<char>::space, ch))
			{
				if( ret.length()!=0)
					break;
				continue;
			}
			//if( ch == 10)	continue;
			//if( ch == 9)	continue;
			//if( ch == 13)	break;
			//if( ch == 32)	break;
			ret += ch;
		}
	}
    return ret;
}
