// FaceDesc.cpp: implementation of the CFaceDesc class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#ifdef CAL3D_EXPORTER
#include "cal3d_max_exporter.h"
#endif

#define D3D_OVERLOADS
#include "FaceDesc.h"
#include "renderer.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//class CalError;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFaceDesc::CFaceDesc()
{
	wNum = 0;
}

CFaceDesc::~CFaceDesc()
{
	wIndexes.erase(wIndexes.begin(), wIndexes.end());
	wDeltas.erase(wDeltas.begin(), wDeltas.end());
}

bool CFaceDesc::Create(LPSTR fileName)
{
	// open the file
	std::ifstream file;

	if(!file)
		return 0;

	DWORD fileLen;
	char* pBuf;

	FILE* fil = fopen((LPCSTR)fileName, "r");
	DWORD endf;
	endf = fseek(fil, 0, SEEK_END);
	if(!endf){
		fpos_t pos;
		fgetpos(fil, &pos);
		fileLen = pos;
	}else return 0;
	fclose(fil);

	file.open(fileName, std::ios::in | std::ios::binary);
	
	pBuf = (char*)malloc(fileLen);
	file.read(pBuf, fileLen);

	char curCh;
	Cal3DComString st = "";
	DWORD counter = 0;
	WORD wBuffer[6];
	float fl = 0.0;
	int	  i1 = 0;
	curCh = ' ';
	int vNum = 0;
	int i = 0;
	while(i < 6)
	{
		curCh = pBuf[counter];
		if((curCh != ',')&(curCh != ';')&(curCh != ' ')&(curCh != (char)10)&(curCh != (char)13)&(counter < fileLen))
		{
			st += curCh;
		} else
		{	
			if(st != "")
			{
				i1 = atoi(st);
				wBuffer[i] = (WORD)i1;
			  i++;
			  st = "";
			}
		};
		counter++;
	};

	wNum = wBuffer[0];
	vNum = wBuffer[1];
	w1 = wBuffer[2];
	w2 = wBuffer[3];
	w3 = wBuffer[4];
	w4 = wBuffer[5];

	i = 0;
	while(i < vNum)
	{
		curCh = pBuf[counter];
		if((curCh != ',')&(curCh != ';')&(curCh != ' ')&(curCh != (char)10)&(curCh != (char)13)&(counter < fileLen))
		{
			st += curCh;
		} else
		{	
			if(st != "")
			{
			  i1 = atoi(st);
			  wIndexes.push_back((WORD)i1);
			  i++;
			  st = "";
			}
		};
		counter++;
	};

	D3DVECTOR	vector;
	WORD vcnt = 0;

	st = "";
	int	iFigCount=0;
	while (iFigCount < 3*wNum*vNum)
	{
		curCh = pBuf[counter];
		if((curCh != ',')&(curCh != ';')&(curCh != (char)10)&(curCh != (char)13))
		{
			st += curCh;
		} else
		{	
			if(st != "")
			{
			  iFigCount++;
			  fl = atof(st);
			  memcpy((void*)((DWORD)&vector+vcnt*sizeof(float)), (void*)&fl, sizeof(float));
			  vcnt++;
			  if(vcnt == 3){
				  vcnt = 0;
				  wDeltas.push_back(vector);
			  }
			  st = "";
			}
		};
		counter++;
	};

	char	buffer[256];
	ZeroMemory( &buffer, 256);
	BYTE	bFileLength=0;
	while(counter < fileLen){
		if( pBuf[ counter] != 44 && pBuf[ counter] != 10 && pBuf[ counter] != 13){
			buffer[ bFileLength] = pBuf[ counter];
			bFileLength++;
		}
		counter++;
	}
	header.bFileName = bFileLength;
	header.pFileName  = (char*)malloc( bFileLength+1);
	ZeroMemory(header.pFileName, bFileLength+1);
	memcpy( header.pFileName, &buffer, bFileLength);
	return true;
}

void CFaceDesc::AddPair(WORD iMaxIndex, WORD iCal3dIndex, WORD iSubmesh)
{
	//??
	for(int k=0; k < wIndexes.size(); k++){
		WORD nextIndex = wIndexes[k];
		nextIndex--;
		if( nextIndex == iMaxIndex){
			 int iiii=0;
		}
	}
	WORD iMax, iCal, iSub;
	bool	bPresent=false;
	for(int i=0; i<aMaxIndexes.size(); i++){
		iMax = aMaxIndexes[i];
		iCal = aCalIndexes[i];
		iSub = aSubMeshes[i];
		if((iMax == iMaxIndex)&(iCal == iCal3dIndex)&(iSub == iSubmesh))
			bPresent = true;
	}

	if(!bPresent){
		aMaxIndexes.push_back(iMaxIndex);
		aCalIndexes.push_back(iCal3dIndex);
		aSubMeshes.push_back(iSubmesh);
	}
}

void CFaceDesc::prepare()
{
	header.lNum = wNum;
	header.vNum = 0;//aMaxIndexes.size();
	for(int j=0; j<wIndexes.size(); j++){
		for(int k=0; k<aMaxIndexes.size(); k++){
			if(wIndexes[j]-1 == aMaxIndexes[k])
				header.vNum++;
		}
	}
	int i=0;
	WORD _w1, _w2, _w3, _w4;
	_w1 = w1-1;
	_w2 = w2-1;
	_w3 = w3-1;
	_w4 = w4-1;
	for(i=0; i<aMaxIndexes.size(); i++){
		if(aMaxIndexes[i] == _w1){
			w1 = aCalIndexes[i];
			header.aBaseSubMeshes[0] = aSubMeshes[i];
		}
		if(aMaxIndexes[i] == _w2){
			w2 = aCalIndexes[i];
			header.aBaseSubMeshes[1] = aSubMeshes[i];
		}
		if(aMaxIndexes[i] == _w3){
			w3 = aCalIndexes[i];
			header.aBaseSubMeshes[2] = aSubMeshes[i];
		}
		if(aMaxIndexes[i] == _w4){
			w4 = aCalIndexes[i];
			header.aBaseSubMeshes[3] = aSubMeshes[i];
		}
	}
	header.aBase[0] = w1;
	header.aBase[1] = w2;
	header.aBase[2] = w3;
	header.aBase[3] = w4;
	header.dwSubmeshCount = dwSubMeshCount;
}

void CFaceDesc::SetSubmeshCount(WORD wSmc)
{
	dwSubMeshCount = wSmc;
}

WORD CFaceDesc::GetSubmeshVertCount(WORD smn)
{
	WORD wret=0;

	for(int i=0; i<wIndexes.size(); i++){
		  WORD wNextIndex = wIndexes[i]-1;
		  for(int j=0; j < aMaxIndexes.size(); j++){
			  WORD ism = aSubMeshes[j];
			  WORD dwMaxIndexes = aMaxIndexes[j];
			  if((wNextIndex == dwMaxIndexes)&(ism == smn)){
				  wret++;
			  }
		  }
	 }
	return wret;
}


CFace::CFace()
{
	header.pFileName=NULL;
}

CFace::~CFace()
{
	int i=0;
	for(i=0; i<aVertIndexes.size(); i++)
		delete aVertIndexes[i];
	for(i=0; i<aDeltas.size(); i++)
		delete aDeltas[i];
	aVertNum.erase(aVertNum.begin(), aVertNum.end());
	aSubmeshNum.erase(aSubmeshNum.begin(), aSubmeshNum.end());
	aVertIndexes.erase(aVertIndexes.begin(), aVertIndexes.end());
	aDeltas.erase(aDeltas.begin(), aDeltas.end());

	if((header.pFileName != NULL)&&(header.bFileName != 0)) free(header.pFileName);
}

FACEFILEDESC* CFace::getHeader()
{
	return &header;
}

void CFace::calcVectors()
{
	vi = vi - base;
	vj = vj - base;
	vk = vk - base;
}

bool CFace::init(CalModel *ap_Model)
{
  CalRenderer *pCalRenderer;
  pCalRenderer = ap_Model->getRenderer();

  DWORD meshCount = pCalRenderer->getMeshCount();

  /*FACEFILEDESC*	header = NULL;
  if(m_pFace != NULL)
	  header = m_pFace->getHeader();*/

  // render all meshes of the model
  int meshId;
  for(meshId = 0; meshId < meshCount; meshId++){
    int submeshCount;
    submeshCount = pCalRenderer->getSubmeshCount(meshId);

    int submeshId;
    for(submeshId = 0; submeshId < submeshCount; submeshId++)
    {
      // select mesh and submesh for further data access
      if(pCalRenderer->selectMeshSubmesh(meshId, submeshId))
      {
		float* meshVertices;
		int vertexCount;
        vertexCount = pCalRenderer->getVertList((float**)&meshVertices);

		if( meshVertices == NULL)
			return false;

		//получаем координаты базовых точек face`а модели
		if(this != NULL){
			if(header.aBaseSubMeshes[0] == submeshId){
				if( pCalRenderer->useVB()){
					base.x = (((D3DVERTEX*)meshVertices)[header.aBase[0]]).x;
					base.y = (((D3DVERTEX*)meshVertices)[header.aBase[0]]).y;
					base.z = (((D3DVERTEX*)meshVertices)[header.aBase[0]]).z;
				}else{
					base = ((D3DVECTOR*)meshVertices)[header.aBase[0]];
				}
			}
			if(header.aBaseSubMeshes[1] == submeshId){
				if( pCalRenderer->useVB()){
					vi.x = (((D3DVERTEX*)meshVertices)[header.aBase[1]]).x;
					vi.y = (((D3DVERTEX*)meshVertices)[header.aBase[1]]).y;
					vi.z = (((D3DVERTEX*)meshVertices)[header.aBase[1]]).z;
				}else{
					vi = ((D3DVECTOR*)meshVertices)[header.aBase[1]];
				}
			}
			if(header.aBaseSubMeshes[2] == submeshId){
				if( pCalRenderer->useVB()){
					vj.x = (((D3DVERTEX*)meshVertices)[header.aBase[2]]).x;
					vj.y = (((D3DVERTEX*)meshVertices)[header.aBase[2]]).y;
					vj.z = (((D3DVERTEX*)meshVertices)[header.aBase[2]]).z;
				}else{
					vj = ((D3DVECTOR*)meshVertices)[header.aBase[2]];
				}
			}
			if(header.aBaseSubMeshes[3] == submeshId){
				if( pCalRenderer->useVB()){
					vk.x = (((D3DVERTEX*)meshVertices)[header.aBase[3]]).x;
					vk.y = (((D3DVERTEX*)meshVertices)[header.aBase[3]]).y;
					vk.z = (((D3DVERTEX*)meshVertices)[header.aBase[3]]).z;
				}else{
					vk = ((D3DVECTOR*)meshVertices)[header.aBase[3]];
				}
			}
		}
	  }
	}
  }
  return true;
}
