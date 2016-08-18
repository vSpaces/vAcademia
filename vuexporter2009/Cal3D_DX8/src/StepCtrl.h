// StepCtrl.h: interface for the StepCtrl class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_STEPCTRL_H__D4F69E62_013C_4CD4_BE7F_5A84F89BCC30__INCLUDED_)
#define AFX_STEPCTRL_H__D4F69E62_013C_4CD4_BE7F_5A84F89BCC30__INCLUDED_

#include "animation_action.h"	// Added by ClassView
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "vector.h"
class CalAnimationCycle;
class CalModel;

typedef WORD STEPTYPE;

#define ST_ROTONLY		0x0001
#define ST_STEPONLY		0x0002

// вектор поворота после шагов
// если не указан, то после прохода 
// модель не поворачивается
#define	SP_SVECTOR		1		
#define	SP_WEIGHT		2
#define	SP_SDELAY		4
#define	SP_EDELAY		8
// конечная точка
// если не указан, то модель просто поворачивается
// в конечное положение
#define	SP_EVECTOR	32		
#define	SP_SVLOOK	64
#define	SP_KEEPDIR	128		// сохранить текущее направление после поворота

#define	SP_KEEPX	256		// сохранить текущее направление после поворота
#define	SP_KEEPY	512		// сохранить текущее направление после поворота
#define	SP_KEEPZ	1024	// сохранить текущее направление после поворота

#define	RM_ROTATETODESTPOINT	1		// режим поворота в точку движения
#define	RM_ROTATETODESTVECTOR	2		// режим конечного поворота
#define	RM_FIRSTSTEP			4		// режим первого шага
#define	RM_MOVING				8		// режим движения
#define	RM_WAITFOOTDOWN			16		// повернулись по направлению и ждем опускания ноги

#define	AN_CYCLESTEP			1
#define	AN_FIRSTSTEP			2
#define	AN_CYCLEROTATION		4
#define	AN_STARTURNROTATION		8

typedef struct _StepHeader {
	DWORD		dw_Flags;
    D3DVECTOR	startDir;
	D3DVECTOR	endDir;
	WORD		pointNum;
	D3DVECTOR	*points;
} STEPHEADER;

typedef	struct	_STEPPARAMS{
	DWORD		dw_Flags;
	float		sf_StepLength;
	float		sf_Weight;
	float		sf_RotAngle;

	int	id_cycle_step_lr;
	int	id_cycle_step_rl;
	int	id_rest_on_r;
	int	id_rest_on_l;
	int	id_first_step_l;
	int	id_first_step_r;
	int	id_stay_at_dest;
	int	id_back_ccw;
	int	id_back_cw;
	int id_cycle_rotation;
	int id_bg_rleg_left;
	int	id_bg_lleg_right;	

//	DWORD		dw_AnimID;		// шаг
	//int			dw_RtnID;		// поворот
	int			dw_StayID;		// на месте
//	int			dw_FirstStepID;	// первый шаг

	DWORD		dw_StartDelay;
	DWORD		dw_EndDelay;
	STEPHEADER	*header;

	_STEPPARAMS::_STEPPARAMS()
	{
		id_cycle_step_lr = id_cycle_step_rl = id_rest_on_r = id_rest_on_l = id_first_step_l = \
		id_first_step_r = id_stay_at_dest = id_back_ccw = id_back_cw = id_cycle_rotation = \
		id_bg_rleg_left = id_bg_lleg_right = -1;
	}
	bool	useFirstStep(){ return (id_first_step_l != -1) && (id_first_step_r != -1);}
} STEPPARAMS;

typedef 	struct	_PATHPIECE
{
	float	m_LengthWeight;
	float	m_Length;
	int		curpoint;
	
	_PATHPIECE::_PATHPIECE()
	{
		m_LengthWeight = 0.0;
		m_Length = 0.0;
		curpoint = 0;
	}

} PATHPIECE;


//enum MOVESTATE  { MS_FSTEP, MS_GOING};

class StepCtrl  
{
public:
	bool DirectionIsRight(CalVector& normal, float& angle);
	void stROTATETODESTVECTOR();
	void setDuration( float dur);
	float duration;
	bool canBeCleared();
	void startGotoDest();
	void setParams(STEPPARAMS _params);
	void setRotation(CalVector axes, float rangle);
	void endStep();
	bool done();
	CalAnimationCycle* getAction();
	void setAction(CalAnimationCycle* pa);
	void beginStep(STEPPARAMS _params);
	int getID();
	void setID(int id);
	void stepIt( float deltaTime);
	void setStepVector(CalVector ats);
	StepCtrl(CalModel* model);
	virtual ~StepCtrl();

protected:
	int	m_iStepLength;
	float				curPhase;
	int					m_iAnimID;	//номер анимации
	DWORD				m_dwLastTick;	//последний отсчет времени
	bool				bDone;	//нужно удалить этот контроллер
	CalVector			axes;	//вектор шага
	CalAnimationCycle	*action;	//указатель на анимацию
	CalModel			*m_pModel;	//указатель на модель для ее трансформирования
	CalVector			axeRot;	//вектор шага
	float				angle;
	STEPTYPE			stepType;

	CalVector			sourcePos;
	int					curpoint;

	PATHPIECE			pathpiece;

	float				srcToDestAngle;	//угол между вертором направления и вектором движения в начале шага
	int					iUpdate;
	bool				bTakeRightWay;

public:
	void animDone(int id);
	void doAnimation( DWORD type, float param1, float param2);
	void clearAnimation( DWORD type, float param1 = 0.0);
	//MOVESTATE			moveState;
	STEPPARAMS			params;
	DWORD				dw_Mode;
	DWORD				dw_Animations;

	// Turn to
	float				m_accumulatedAngle;
};	

#endif // !defined(AFX_STEPCTRL_H__D4F69E62_013C_4CD4_BE7F_5A84F89BCC30__INCLUDED_)
