//****************************************************************************//
// model.h                                                                    //
// Copyright (C) 2001, 2002 Bruno 'Beosil' Heidelberger                       //
//****************************************************************************//
// This library is free software; you can redistribute it and/or modify it    //
// under the terms of the GNU Lesser General Public License as published by   //
// the Free Software Foundation; either version 2.1 of the License, or (at    //
// your option) any later version.                                            //
//****************************************************************************//

#ifndef CAL_MODEL_H
#define CAL_MODEL_H

#include "CommonCal3DHeader.h"

//****************************************************************************//
// Includes                                                                   //
//****************************************************************************//

#include "global.h"
#include "vector.h"
#include "quaternion.h"
#include "mpeg4fdp.h"

//****************************************************************************//
// Forward declarations                                                       //
//****************************************************************************//

class CalCoreModel;
class CalSkeleton;
class CalMixer;
class CalPhysique;
class CalSpringSystem;
class CalAnimationMorph;
class CalRenderer;
class CalMesh;
class CMpeg4FDP;
//struct FACEExpression;
class CFace;
class CalPointController;
//class CMpeg4FDP;
//struct FACEExpression;
//struct EM_PACK;

#include "LookController.h"
#include "VisemeController.h"
//#include "D3dx8math.h"
//class CalLookController;

class CalModel;

enum ACTIONTYPE  {AT_ACTION, AT_CYCLE, AT_STEP, AT_SET, AT_NONE, 
					AT_SAY, AT_LOOK, AT_SHADOW, AT_LINK, AT_SHOW, 
					AT_FLIP, AT_IDLE, AT_SYNCHACTION, AT_SETMAP, AT_MOVE3D,
					AT_SYNCHANIM, AT_MORPH, AT_ROTATE3D, AT_SCALE3D, AT_POINTAT,
					AT_LOAD, AT_RENDLL, AT_CAMERA, AT_STEPSOUNDR, AT_STEPSOUNDL};

enum	EPICKMODE 		{PM_HITBOXES, PM_FACES};

#define	RIGHTHAND	1	// point by right hand
#define	LEFTHAND	2	// point by left hand

enum EPOINTFLAGS {PF_CAMERA, PF_TARGET, PF_POINT};

typedef struct _POINTATPARAMS
{
    void* m_pTarget;		// на какой объект показать
    D3DVECTOR	m_Point;	// показать на точку
	DWORD		m_dwHand;	// какой рукой показывать 1 - правая, 2 - левая
	EPOINTFLAGS	pointflags;	// флаг, куда показать
	DWORD		multiplier;

	_POINTATPARAMS(){
		m_pTarget = NULL; m_Point.x = 0.0f; m_Point.y = 0.0f; m_Point.z = 0.0f; m_dwHand = 0;
		multiplier = 1;
	}
} POINTATPARAMS;

/*struct _LOOKCONTROLLER{
	bool		b_Enable;
	int			i_CurAnimId;
	int			i_NextAnimId;
	CalBone*	pHeadBone;
	CalBone*	pLookBone;
	CalModel*	pLookTrg;
} LOOKCONTROLLER;*/

typedef struct _POINTCTRL{
	DWORD		initHand;	// 0-не было инициализации, 1 - на правую руку, 2 - на левую руку
	bool		bEnable;
	DWORD		idClavicle;	// номер кости плеча ( родитель для руки)			3dsmax BoneName=Clavicle
	DWORD		idUpper;	// номер кости плеча руки							3dsmax BoneName=UpperArm
	DWORD		idLower;	// номер кости предплечья руки						3dsmax BoneName=Forearm
	DWORD		idHand;		// номер кости кисти								3dsmax BoneName=Hand
	CalVector	target;
	CalModel*	p_target;
	DWORD		multiplier;
	float		accWeight;

	_POINTCTRL()
	{
		initHand=0; bEnable=false; idClavicle=0xFFFFFFFF; idUpper=0xFFFFFFFF; idLower=0xFFFFFFFF; idHand=0xFFFFFFFF;
		target = CalVector( 0, 0, 0);
		p_target = NULL; multiplier=1;
		accWeight = 0;
	}
} POINTCTRL;

//****************************************************************************//
// Class declaration                                                          //
//****************************************************************************//

 /*****************************************************************************/
/** The model class.
  *****************************************************************************/

#define MULTMATRIX(mr, m1,m2)	D3DXMatrixMultiply(mr, m1, m2);

typedef int (*pTransformFunction)(CalVector&,CalQuaternion&,LPVOID data);  

class CAL3D_API CalModel
{
// member variables
protected:
  CalCoreModel*		m_pCoreModel;
  CalSkeleton*		m_pSkeleton;
  CalMixer*			m_pMixer;
  CalPhysique*		m_pPhysique;
  CalSpringSystem*	m_pSpringSystem;
  CalRenderer*		m_pRenderer;
  Cal::UserData		m_userData;
  CFace*			m_pFace;
  MP_VECTOR<CalMesh *> m_vectorMesh;
  POINTCTRL	pointctrl;	// данные для указания на что-либо
  bool						externBuffers;

// constructors/destructor
public: 
	CalModel();
	virtual ~CalModel();
	MP_VECTOR<ACTIONTYPE>	actiontype;
	MP_VECTOR<int>		actionid;
	D3DMATRIX*				pmExternTransform;
	CMpeg4FDP*				m_pFAPU;
	
// member functions
public:
	void SetLookController(CalLookController* lookController);
	void SetPointController(CalPointController* pointController);
	void SetVisemeController(CalVisemeController* visemeController);

	__forceinline bool IsSharedSkeleton()const
	{
		return (binded_target_model != NULL);
	}

	float get_bip_height();
	bool basisChanged(CalVector& pos, CalQuaternion& quat, bool& relative);
	void useExternTransform(D3DXMATRIX* pm);
	bool setExtertBuffers(LPVOID, LPVOID);
	void useExtertBuffers(bool use=true){ externBuffers = use;}
	void generateEdges();
	void insert_EmPack(std::string name, FACEExpression** vector, int count);
	bool get_bbox_by_bones(CalVector *max, CalVector *min, std::vector<std::string>& names);
	bool setFAP(CMpeg4FDP* am_pFAPU);

	void getInFileTranslation(CalVector& trn);
	void getInFileRotation(CalQuaternion& rtn);
	void destroy_geometry();

	void setPickMode(EPICKMODE pm){ pickmode=pm;};
	EPICKMODE	getPickMode(){ return pickmode;}
	__forceinline void RemoveExternTransform() { pmExternTransform = NULL;};
	__forceinline void SetExternTransform( D3DMATRIX *pextrn){ pmExternTransform = pextrn;};

	void PointAt(POINTATPARAMS params);
	bool NoSkel( bool abSetNoSkel=false);
	CalVector transformVector(CalVector v);
	int getFaceCount();
	int getEdgesCount();
	void detachModel();
	bool getBoundingBox(CalVector* max, CalVector* min);
	bool getBBoxes(std::vector<CalVector>& vmax, std::vector<CalVector>& vmin);
	void attachToModel(CalModel* pOwner=NULL, LPSTR name=NULL, CalVector trn=CalVector(0,0,0), CalQuaternion rtn=CalQuaternion(0,0,0,1));
	int getVertCount();
	int lookAt(LOOKATPARAMS params);
	void setDoneFlag(ACTIONTYPE type, int id);
	bool m_bStepDone;

	int getDoneAnimationID();
	bool doneAction(ACTIONTYPE	*type, int	*value);
	DWORD	fvf;
	DWORD	stride;
	

	void attachVB(IDirect3DDevice8 *pD3D8);
	void checkCoreAnimations();
	void setScale( float scale);

	void setFace(WORD l1, WORD l2, float phase);
	WORD dwLex1, dwLex2;
	float fFacePhase;
	void updateSkin(CalAnimationMorph* ap_Face = NULL);
	CFace* getFace();
	void setFace(CFace *fd);
	void setDuration(int animID, float duration);

	// Пермещение модели и масштаб
	bool	m_bLocalTrans;

	bool bLastRotPointDef;

	float	m_Scale;
	
	bool attachMesh(int coreMeshId);
	bool create(CalCoreModel *pCoreModel);
	void destroy();
	bool detachMesh(int coreMeshId);
	CalCoreModel *getCoreModel();
	CalMesh *getMesh(int coreMeshId);
	__forceinline CalMixer *getMixer(){ return m_pMixer;}
	__forceinline CalPhysique *getPhysique(){ return m_pPhysique;}
	__forceinline CalRenderer *getRenderer(){ return m_pRenderer;}
	__forceinline CalSkeleton *getSkeleton(){ return m_pSkeleton;}
	__forceinline CalSpringSystem *getSpringSystem(){ return m_pSpringSystem;}
	__forceinline Cal::UserData getUserData(){ return m_userData;}
	std::vector<CalMesh *>& getVectorMesh(){ return m_vectorMesh;}
	void setLodLevel(float lodLevel);
	void setMaterialSet(int setId);
	void setUserData(Cal::UserData userData);
	bool update(float deltaTime, bool isModelVisible = true);
	void needUpdateWhenVisible();

	void deleteAnimations(CalCoreAnimation* coreAnimation);

	// Skeleton binding
	void bindToModelSkeleton(CalModel* apModel);
	void unbindSkeleton();

	// Freezing
	void freeze();
	void unFreeze();
	bool isFrozen();

	CalModel*		m_pOwner;
	CalModel*		m_pSlaver;
	std::map<MP_STRING, EM_PACK>& get_mapEmotions(){ return m_mapEmotions;}
	pTransformFunction	m_pfExternTransform;
	LPVOID				m_pExternClass;
	bool				relative_pos_changed;
	bool				is_character(){ return __is_character;}

private:	
	bool updateLookAtCtrl( float deltaTime);

	// binding
	void addBindedModel(CalModel* aModel);
	void removeBindedModel(CalModel* aModel);
	CalSkeleton* unbinded_self_skeleton;
	CalModel*    binded_target_model;
	MP_VECTOR<CalModel*> binded_models;

	CalPointController* m_pointController;

private:
	bool m_bNoSkel;
	bool m_bPrevModelVisible;
	bool m_bNeedUpdateWhenVisible;
	bool m_bFrozen;
	bool edges;
	EPICKMODE	pickmode;
	MP_MAP<MP_STRING, EM_PACK> m_mapEmotions;
	CalVector	bboxMin, bboxMax;
	bool		bboxIsRight;
	bool		__is_character;

	CalLookController*		m_lookController;
	CalVisemeController*	m_visemeController;
};

#endif

//****************************************************************************//
