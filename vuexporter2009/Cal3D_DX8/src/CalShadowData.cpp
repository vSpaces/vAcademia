// CalShadowData.cpp: implementation of the CalShadowData class.
//
//////////////////////////////////////////////////////////////////////

#include "CalShadowData.h"
#include "coremesh.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CalShadowData::CalShadowData()
{

}

CalShadowData::~CalShadowData()
{

}

bool CalShadowData::createOn(CalCoreMesh *ap_Mesh)
{
	int edgesCount=0, faceCount=0, verticesCount=0;

	ap_Mesh->generate_edges();
	edgesCount=ap_Mesh->getVectorEdge().size();

	edges.reserve(edgesCount);
	std::vector<CalCoreSubmesh::Edge>::iterator	edgeIt;
	std::vector<CalCoreSubmesh::Edge>	submeshEdges = ap_Mesh->getVectorEdge();
	for(edgeIt=submeshEdges.begin(); edgeIt!=submeshEdges.end(); edgeIt++)
	{
		edges.push_back(ShadowEdge(edgeIt->if1+faceCount, edgeIt->if2+faceCount, \
				edgeIt->iv1+verticesCount, edgeIt->iv2+verticesCount));
	}
	return true;
}


CalShadowData* CalShadowData::clone()
{
	CalShadowData* data = new CalShadowData();
	data->edges.assign(edges.begin(), edges.end());
	return data;
}