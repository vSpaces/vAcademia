// WeightVertex.h: interface for the CWeightVertex class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WEIGHTVERTEX_H__5540E5DD_97DA_4F11_A4CB_CCE4749172EF__INCLUDED_)
#define AFX_WEIGHTVERTEX_H__5540E5DD_97DA_4F11_A4CB_CCE4749172EF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class INode;

class CWeightVertex  
{
public:
	CWeightVertex();
	CWeightVertex( INode *node, float weight);
	CWeightVertex( TCHAR *name, float weight);
	virtual ~CWeightVertex();

public:
	void init();
	void create( INode *node, float weight);
	void create( TCHAR *name, float weight);
	void clear();
	INode *getNode();
	TCHAR *getNodeName();
	float getWeight();

protected:	
	INode *node;
	TCHAR *nodeName;
	float weight;

};

#endif // !defined(AFX_WEIGHTVERTEX_H__5540E5DD_97DA_4F11_A4CB_CCE4749172EF__INCLUDED_)
