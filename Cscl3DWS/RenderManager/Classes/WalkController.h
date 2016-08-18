
#pragma once

#include "../CommonRenderManagerHeader.h"

#include <vector.h>
#include "3DObject.h"
#include "BSpline.h"
#include "PathLine.h"
#include "icontroller.h"
#include "MOtion.h"

//	Moving
#define WME_START_GO_FORWARD		1
#define WME_START_GO_BACKWARD		2
#define WME_START_L_STRAFE			4
#define WME_START_R_STRAFE			8
#define WME_END_GO_FORWARD			16
#define WME_END_GO_BACKWARD			32
#define WME_END_L_STRAFE			64
#define WME_END_R_STRAFE			128
#define WME_GO_FORWARD				256
#define WME_GO_BACKWARD				512
#define WME_L_STRAFE				1024
#define WME_R_STRAFE				2048
#define WME_START_GO_LEFT			4096
#define WME_START_GO_RIGHT			8192
#define WME_START_GO_BACK_LEFT		16384
#define WME_START_GO_BACK_RIGHT		32768
#define WME_SET_PATH				65536
#define WME_INITIALIZE				131072
#define WME_SET_PATHPERCENT			262144
#define WME_STOP					524288
#define WME_GOTO_POINT				1048576
#define WME_START_R_RTN				2097152
#define WME_POINT_TURN				8388608
#define WME_CHANGE_MOTIONS_START	33554432
#define WME_CHANGE_MOTIONS_END		67108864

//	Rotating
#define WME_END_R_RTN			(67108864 * 8)
#define WME_START_L_RTN			(67108864 * 4)
#define WME_END_L_RTN			(67108864 * 16)

#define WME_PATHDONE			1
#define WME_AFTER_TURN			16777216


#define NEW_PATH_METHOD			-1

/************************************************************************/
/*	Motions ID. Used as indexes in motions array
/*
/* MID_STGF		0	//	start go forward
/* MID_GF		1	//	go forward
/* MID_GF		2	//	end	go forward
/* MID_STGB		3	//	start go backward
/* MID_GB		4	//	go backward
/* MID_GB		5	//	end backward
/* MID_SL		6	//	strafe	left
/* MID_SR		7	//	strafe right
/************************************************************************/

enum	WALK_MOTIONS_IDs	{	MID_STGF, MID_GF, MID_ENGF,		// forward
								MID_STGB, MID_GB, MID_ENGB,		// backward
								MID_SL,							// strafe left
								MID_SR, 						// strafe right
								MID_LRT,  MID_ENLRT,			// rotate left
								MID_RRT,  MID_ENRRT,			// rotate right
								MID_MAINIDLE};

#define WALK_MOTIONS_COUNT		13 + 2 /*for sounds*/

#define WME_GET_PATHPERCENT		1
#define WME_GET_PREVPERCENT		2

// Magic values
#define MIN_TURN_VECTOR_LENGTH	0.1f
#define	WME_FIRST_TRANSTIME		250.0f
#define	WME_LAST_TRANSTIME		250.0f

// Button states
#define LEFT_KEY_PRESSED		2
#define RIGHT_KEY_PRESSED		4
#define UP_KEY_PRESSED			8
#define DOWN_KEY_PRESSED		16
#define LESS_KEY_PRESSED		32
#define MORE_KEY_PRESSED		64

#define LEFT_FORWARD_MODE		1
#define RIGHT_FORWARD_MODE		2
#define LEFT_BACKWARD_MODE		3
#define RIGHT_BACKWARD_MODE		4

//#define	TEST_NON_CHARACTER_MOVING

/************************************************************************/
/*  При движении Bip скорость определяется формулами:
/*	WME_START_GO_FORWARD	y(x) = x;
/*	WME_GO_FORWARD			y(x) = velocity;
/*	WME_END_GO_FORWARD		y(x) = -x+velocity;
/************************************************************************/

/************************************************************************/
/* Event handlers delegates
/************************************************************************/
//typedef	BOOL	(*lpWalkCallback)(LPVOID /*target*/, DWORD /*message*/, LPVOID /*data*/);

class nrmCharacter;

class CWalkController : public IController
{
public:
	CWalkController();
	virtual ~CWalkController();
	
public:
	void	Update(unsigned int dwTime);
	void	HandleEvent(unsigned int evID, void* data);
	void	ClearAllActions();

	bool	IsCharacterFrozen();
	void	Unfreeze();
	void	Freeze();

	void	GetLastGoToCoords(float& x1, float& y1, float& z1, float& x2, float& y2, float& z2);

	void	GoPath(CVector3D* path, int pointCount);
	void	SetLastPathLine(CPathLine* pathLine);

public:
	void	EnableImmediateMotionRemoveMode( bool abEnable);
	void	SetCharacter(nrmCharacter* rmCharacter);
	float	get_duration(int id);
	void	add_motion(WALK_MOTIONS_IDs id, int mID);
	void	set_step_length(float sl);
	void	set_rotation_speed(float rot);

#ifdef NEW_PATH_METHOD
	CPathLine*	get_path();
#else
	CBSpline*	get_path();
#endif

protected:
	// Направление определяется тремя составляющими:
	// - направление вперед-назад
	DWORD					m_bfDirection;
	// - направление вправо-влево
	DWORD					m_lrDirection;
	// - направление поворота
	DWORD					m_rotDirection;

#ifdef NEW_PATH_METHOD
	CPathLine*				m_path;
	CPathLine*				m_internalPath;
#else
	CBSpline*				m_path;
	CBSpline*				m_internalPath;
#endif

	// Engine motions ID's
	MP_VECTOR<int>		m_motionIDs;	
	// motions objects
	MP_VECTOR<CMotion*>	m_motions;	

	// персонаж останавивается по достижению конца пути, а не по команде пользователя.
	bool					m_isStoppedOnArrived;	

	bool					m_isAfterTurn;
	float					m_stepLength;
	float					m_rotSpeed;
	float					m_lastPercent;
	// текущий процент перемещения по пути
	float					m_pathPercent;	
	CalVector				m_turnPoint;
	CalVector				m_afterTurnPoint;
	C3DObject*				m_character;
	nrmCharacter*			m_rmCharacter;

private:
	void	initialize();
	void	handle_message(DWORD message);
	void	clear_old_actions();

	void	change_motions_start();
	void	change_motions_end();

	// обработка сообщений
	void	rotate_avatar(int addAngle, bool isSaveAngle);
	void	start_go_forward(int addAngle, bool isSaveAngle = true);
	void	end_go_forward();
	void	start_go_backward(int addAngle, bool isSaveAngle = true);
	void	end_go_backward();
	void	start_strafe_left();
	void	end_strafe_left();
	void	start_strafe_right();
	void	end_strafe_right();
	void	go_to_point(void* data);
	void	point_turn(void* data);
	void	set_path(void* data);
	void	start_left_rotation();
	void	end_left_rotation();
	void	start_right_rotation();
	void	end_right_rotation();

	// действия с locked bones
	void	set_action_with_locked_bones(int cal3dID, DWORD time, std::string bone);
	void	remove_action_with_locked_bones(int cal3dID);

	// обработка update
	void	update_locked_bones();
	void	update_point_rotating(DWORD dwTime, float frameStepLength);
	void	update_spline_moving(DWORD dwTime, float	frameStepLength, float moveLength, bool& removePathOnThisStep);
	void	update_strafe_moving(DWORD dwTime, float frameStepLength);
	void	update_forward_backward_moving(DWORD dwTime, float frameStepLength);
	void	update_rotating(DWORD dwTime);

	// cal3d wrappers
	void	execute_action(int id, float delayIn, float delayOut);
	void	set_action( int id, float time=0.0);
	void	clear_action( int id, float time=0.0);
	void	clear_cycle( int id, float time=0.0);
	bool	blend_cycle(int id, float weight, float delay, bool asynch=false);
		
	// состояние движения в направлении
	int		m_lrMotionState, m_bfMotionState;
	// состояние поворотных движений
	int		m_rotatingState;
	
	// время движения в направлении
	DWORD	m_lrMoveTick, m_lrLastMoveTick;
	DWORD	m_bfMoveTick, m_bfLastMoveTick;

	float	m_mainDuration;
	float	m_lockShift;

	bool m_isInitialized;
	
	DWORD m_stopStartTime;

	bool m_isStopStart;
	bool m_isClearingNeeded;
	bool m_isBoneLocked;
	bool m_isFirstBoneLockedFrame;
	bool m_isRotatedBeforePath;

	bool m_isCollision;

	bool m_isMovementChanged;

	MP_STRING m_lockedBoneName;

	int m_addAngle;
	int m_specialMode;
	DWORD m_buttonState;

	CRotationPack m_characterRotation;

	CVector3D m_lockDelta;

	CVector3D m_lastGoToStartPos;
	CVector3D m_lastGoToEndPos;
	int m_lastPathPointCount;

	//CPathFinder m_foundPath;

	bool m_isRotatingLeft, m_isRotatingRight;

	CalVector m_lockedBoneTranslation;
	bool		m_bImmediateMotionRemove;

	CPathLine* m_lastPathLine;
};
