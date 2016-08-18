// WeightVertex.cpp: implementation of the CWeightVertex class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "WeightVertex.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CWeightVertex::CWeightVertex()
{
	init();
}

CWeightVertex::CWeightVertex( INode *node, float weight)
{
	init();
	create( node, weight);
}

CWeightVertex::CWeightVertex( TCHAR *name, float weight)
{
	init();
	create( name, weight);
}

CWeightVertex::~CWeightVertex()
{
	clear();
}

//////////////////////////////////////////////////////////////////////////

void CWeightVertex::init()
{
	nodeName = NULL;
	node = NULL;
}

void CWeightVertex::create( INode *node, float weight)
{
	this->node = node;
	this->weight = weight;
}

void CWeightVertex::create( TCHAR *name, float weight)
{
	if ( name != NULL)
	{
		int size = strlen( name);
		if ( size > 0)
		{
			nodeName = new TCHAR[ size];
			nodeName = ( TCHAR*) malloc(( size+1) * sizeof( TCHAR));
			strcpy( nodeName, name);
		}
	}
	this->weight = weight;
}

void CWeightVertex::clear()
{
	if ( nodeName != NULL)
	{
		free( nodeName);
		nodeName = NULL;
	}
	weight = 0.0f;
}

INode *CWeightVertex::getNode()
{
	return node;
}

TCHAR *CWeightVertex::getNodeName()
{
	if ( node != NULL)
		return node->GetName();
	return nodeName;
}

float CWeightVertex::getWeight()
{
	return weight;
}
