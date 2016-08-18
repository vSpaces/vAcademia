// FaceDesc.h: interface for the CFaceDesc class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FACEDESC_H__F5B20328_7ACC_4D7B_B96D_065FF010E3B1__INCLUDED_)
#define AFX_FACEDESC_H__F5B20328_7ACC_4D7B_B96D_065FF010E3B1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <fstream>
#include <string>
#include <vector>
#include <list>
#include <stack>
#include <map>
#include <set>
#include <sstream>
#include <algorithm>
#include <D3dx8math.h>
#include "coreanimation.h"
#include "model.h"

typedef struct faceFileDesc{
	WORD	lNum;	//число лексем
	WORD	vNum;	//число вершин
	WORD	aBase[4];	//массив номеров базовых вершин
	WORD	aBaseSubMeshes[4];	//массив номеров базовых вершин
	WORD	dwSubmeshCount;	//число SubMesh
	BYTE	bFileName;
	char*	pFileName;

	faceFileDesc::faceFileDesc(){
		pFileName = NULL;
		bFileName = 0;
	}
} FACEFILEDESC;

class CalSaver;
class CalCoreAnimation;

class CFaceDesc  
{
public:
	friend class CalSaver;
	void prepare();
	void AddPair(WORD , WORD, WORD );
	bool Create(LPSTR fileName);
	CFaceDesc();
	virtual ~CFaceDesc();
public:
	WORD GetSubmeshVertCount(WORD smn);
	WORD dwSubMeshCount;
	void SetSubmeshCount(WORD wSmc);
	FACEFILEDESC	header;
	//исходные данные
	std::vector<WORD>	wIndexes;	//номера вершин для движения( в максовой сцене)
	std::vector<D3DXVECTOR3>	wDeltas;	//коэффициенты векторного произведения

	//вектора соответствия номеров вершин
	std::vector<WORD>	aMaxIndexes;	//коэффициенты векторного произведения
	std::vector<WORD>	aCalIndexes;	//коэффициенты векторного произведения
	std::vector<WORD>	aSubMeshes;	//номера SubMesh

protected:
	WORD	wNum;	//число лексем
	WORD	vNum;	//число вершин
	WORD	w1, w2, w3, w4;	//номера базовых вершин( в максовой сцене)
};

//namespace nsCalFace {

class CFace : public CalCoreAnimation
{
public:
	friend class CalSaver;
	CFace();
	virtual ~CFace();
public:
	bool init(CalModel* ap_Model);
	void calcVectors();
	FACEFILEDESC* getHeader();
	FACEFILEDESC	header;

	//базовые векторы
	D3DXVECTOR3	base;
	D3DXVECTOR3	vi;
	D3DXVECTOR3	vj;
	D3DXVECTOR3	vk;

	//вектора соответствия номеров вершин
	std::vector<WORD>	aSubmeshNum;	//номера SubMesh
	std::vector<WORD>	aVertNum;		//число вершин
	std::vector<WORD*>	aVertIndexes;	//номера вершин
	std::vector<D3DXVECTOR3*>	aDeltas;	//коэффициенты векторного произведения
};
//}

#endif // !defined(AFX_FACEDESC_H__F5B20328_7ACC_4D7B_B96D_065FF010E3B1__INCLUDED_)
