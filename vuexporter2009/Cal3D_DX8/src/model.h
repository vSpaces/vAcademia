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

//****************************************************************************//
// Includes                                                                   //
//****************************************************************************//

#include "global.h"
#include "vector.h"
#include "quaternion.h"
#include "stepctrl.h"
//#include "facedesc.h"
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
class StepCtrl;
class CFace;
//class CMpeg4FDP;
//struct FACEExpression;
//struct EM_PACK;

#include "lookcontroller.h"
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
		initHand=0; bEnable=false; idClavicle=-1; idUpper=-1; idLower=-1; idHand=-1;
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
  std::vector<CalMesh *> m_vectorMesh;
  POINTCTRL	pointctrl;	// данные для указания на что-либо
  bool						externBuffers;

// constructors/destructor
public: 
	CalModel();
	virtual ~CalModel();
	std::vector<ACTIONTYPE>	actiontype;
	std::vector<int>		actionid;
	D3DMATRIX*				pmExternTransform;
	CMpeg4FDP*				m_pFAPU;
	
// member functions
public:
	float get_bip_height();
	bool natura3d_pos_changed(CalVector& pos, CalQuaternion& quat, bool& relative);
	void useExternTransform(D3DXMATRIX* pm);
	bool setExtertBuffers(LPVOID, LPVOID);
	void useExtertBuffers(bool use=true){ externBuffers = use;}
	void generateEdges();
	void insert_EmPack(Cal3DComString name, FACEExpression** vector, int count);
	bool get_bbox_by_bones(CalVector *max, CalVector *min, std::vector<Cal3DComString>& names);
	bool setFAP(CMpeg4FDP* am_pFAPU);
	void stockFloor();
	void adjustBoneLock(CalAnimation*	anim);
	void setFloor(float floor);
	float m_floorLevel;
	bool WasMoved();
	void getCenterMatrix(D3DXMATRIX *pm);
	void getMovingMatrix(D3DXMATRIX *pm);
	void getRestoreMatrix(D3DXMATRIX *pm);
	void getTransformMatrix(D3DXMATRIX *pm);
	void	get_worldTranslation(CalVector& trn);
	void	get_worldRotation(CalQuaternion& rtn);
	void destroy_geometry();

	void setPickMode(EPICKMODE pm){ pickmode=pm;};
	EPICKMODE	getPickMode(){ return pickmode;}
	inline void RemoveExternTransform() { pmExternTransform = NULL;};
	inline void SetExternTransform( D3DMATRIX *pextrn){ pmExternTransform = pextrn;};
	bool updatePointCtrl( float deltaTime);
	CalVector getDirectionAbsolute(DWORD id, CalVector axes=CalVector(0, 1, 0));
	void PointAt(POINTATPARAMS params);
	bool NoSkel( bool abSetNoSkel=false);
	CalVector transformVector(CalVector v);
	void rotate3d(CalVector axes, float angle, CALOPTYPE type);
	void scale3d(float x, float y, float z);
	int getFaceCount();
	int getEdgesCount();
	bool m_bRootActive;
	bool m_bRecalcRootTM;
	void move3d(float x, float y, float z);
	void detachModel();
	bool getBoundingBox(CalVector* max, CalVector* min);
	bool getBBoxes(std::vector<CalVector>& vmax, std::vector<CalVector>& vmin);
	void attachToModel(CalModel* pOwner=NULL, LPSTR name=NULL, CalVector trn=CalVector(0,0,0), CalQuaternion rtn=CalQuaternion(0,0,0,1));
	int getVertCount();
	/*char* GetFaceTextureName();
	void FreeFaceTextureName(char* apName);
	char* GetFaceNewTextureName();
	void FreeFaceNewTextureName(char* apName);*/
	int lookAt(LOOKATPARAMS params);
	void setDoneFlag(ACTIONTYPE type, int id);
	bool m_bStepDone;
	CalVector getRootDirection();
	CalVector getDirection();
	CalVector getRootPosition( bool buseextern=false);
	int getDoneAnimationID();
	bool doneAction(ACTIONTYPE	*type, int	*value);
	DWORD	fvf;
	DWORD	stride;
	

	void attachVB(IDirect3DDevice8 *pD3D8);
	void checkCoreAnimations();
	void setScale( float scale);
	void saveRotPoint();
	void setMoveType(CALMOVETYPE type);
	void setRotType(CALROTTYPE type);
	void setFace(WORD l1, WORD l2, float phase);
	WORD dwLex1, dwLex2;
	float fFacePhase;
	void updateSkin(CalAnimationMorph* ap_Face = NULL);
	CFace* getFace();
	void setFace(CFace *fd);
	void setDuration(int animID, float duration);
	void clearStep();
	void endStep();
	StepCtrl* m_pStepCtrl;
	bool step(STEPPARAMS params);

	CalLookController	lookctrl;
	// Пермещение модели и масштаб
	bool	m_bLocalTrans;
	CalVector		vTranslation;		// перемещение модели
	CalQuaternion	qRotation;	// поворот модели
	CalVector vLastRotPoint;
	CalVector vScale3D;
	D3DXMATRIX	mMoving, mCoreTransform;	// матрица трансформации, матрица изменения скелета

	bool bLastRotPointDef;
	CALROTTYPE	eRotType;
	CALMOVETYPE eMoveType;
	CalVector	rotPoint;
	float	m_Scale;
	bool	m_bClearCycle;
	void Move(CalVector v, CALOPTYPE type = CAL_AFTER);
	void Rotate(CalVector axes, float angle, CALOPTYPE type = CAL_AFTER);
	void	getTransMatrix(D3DMATRIX*	pMatrix);

	bool attachMesh(int coreMeshId);
	bool create(CalCoreModel *pCoreModel);
	void destroy();
	bool detachMesh(int coreMeshId);
	CalCoreModel *getCoreModel();
	CalMesh *getMesh(int coreMeshId);
	inline CalMixer *getMixer(){ return m_pMixer;}
	inline CalPhysique *getPhysique(){ return m_pPhysique;}
	inline CalRenderer *getRenderer(){ return m_pRenderer;}
	inline CalSkeleton *getSkeleton(){ return m_pSkeleton;}
	inline CalSpringSystem *getSpringSystem(){ return m_pSpringSystem;}
	inline Cal::UserData getUserData(){ return m_userData;}
	std::vector<CalMesh *>& getVectorMesh(){ return m_vectorMesh;}
	void setLodLevel(float lodLevel);
	void setMaterialSet(int setId);
	void setUserData(Cal::UserData userData);
	bool update(float deltaTime, bool updateSkin = true);
	bool contain(std::string& strBuffer);

	// Skeleton binding
	void bindSkeleton(CalSkeleton* apSkeleton);
	void unbindSkeleton();
	CalSkeleton* unbinded_self_skeleton;

	CalModel*		m_pOwner;
	CalModel*		m_pSlaver;
	std::map<Cal3DComString, EM_PACK>& get_mapEmotions(){ return m_mapEmotions;}
	pTransformFunction	m_pfExternTransform;
	LPVOID				m_pExternClass;
	bool				relative_pos_changed;
	bool				is_character(){ return __is_character;}

private:
	bool m_bNoSkel;
	bool m_bPrevUpdateSkin;
	bool edges;
	EPICKMODE	pickmode;
	std::map<Cal3DComString, EM_PACK> m_mapEmotions;
	CalVector	bboxMin, bboxMax;
	bool		bboxIsRight;
	bool		__is_character;
};

#endif

//****************************************************************************//
