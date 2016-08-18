// StepCtrl.cpp: implementation of the StepCtrl class.
//
//////////////////////////////////////////////////////////////////////

#include "global.h"
#include "StepCtrl.h"
#include "coreanimation.h"
#include "animation.h"
#include "coremodel.h"
#include "model.h"
#include "Mmsystem.h"
#include "skeleton.h"
#include "mixer.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
#define		RTNCW		1	<<	8
#define		RTNCCW		2	<<	8
#define		RTNMOREHALF	4	<<	8
#define		RTNHALFTAIL	8	<<	8
#define		RTNSIMPLE	16	<<	8


StepCtrl::StepCtrl(CalModel* model)
{
	m_iStepLength = m_iAnimID = 0;
	angle = 0;
	axes = CalVector(0, 0, 0);
	axeRot = CalVector(0, 0, 0);
	m_pModel = model;
	bDone = false;
	stepType = ST_ROTONLY;
	iUpdate = -1;
	dw_Animations = 0;
	duration = 1.0;
}

StepCtrl::~StepCtrl()
{

}

void StepCtrl::setStepVector(CalVector ats)
{
	axes = ats;
	m_iStepLength = sqrt(ats.x*ats.x+ats.y*ats.y+ats.z*ats.z);
	if(m_iStepLength != 0)
	{
		axes.x /= m_iStepLength;
		axes.y /= m_iStepLength;
		axes.z /= m_iStepLength;
	}	
	if( m_iStepLength == 0)
		stepType &= (~ST_STEPONLY);
	else
		stepType |= ST_STEPONLY;
}

void StepCtrl::stepIt( float deltaTime)
{
	float dPhase = deltaTime / duration;

	if( dw_Mode&RTNSIMPLE) return;

	// Ќачальный режим поворота по направлению движени€
	if(( dw_Mode == RM_ROTATETODESTPOINT)||( dw_Mode == RM_ROTATETODESTVECTOR)){
		if( stepType & ST_ROTONLY){
			//TRACE("stepIt -> [ dw_Mode == RM_ROTATETODESTPOINT || dw_Mode == RM_ROTATETODESTVECTOR]\n")

			// ƒобавл€емый угол поворота
			float _angle = angle*(/*nextPhase-curPhase*/dPhase);
			float curWeight = pathpiece.m_LengthWeight;
			pathpiece.m_LengthWeight += _angle/srcToDestAngle;

			// ѕоследние полповорота
			if( pathpiece.m_LengthWeight+_angle/srcToDestAngle >= 1.0 && pathpiece.m_LengthWeight  < 1.0){
				if(dw_Mode == RM_ROTATETODESTVECTOR){
					if( params.id_stay_at_dest > -1){
						float drt = (1-curWeight)*srcToDestAngle*duration/angle;
						//m_pModel->getMixer()->setAnimDuration(params.id_stay_at_dest, drt);
						//m_pModel->getMixer()->executeAction(params.id_stay_at_dest, drt, 0.0);
						//m_pModel->getMixer()->executeAction(params.id_stay_at_dest, drt);
						m_pModel->getMixer()->setAction(params.id_stay_at_dest, 1000*m_pModel->getCoreModel()->getCoreAnimation(params.id_stay_at_dest)->getDuration());
					}
				}
			}
			

			//	attain the required direction
			if( pathpiece.m_LengthWeight >= 1){

				// align rotation to correct value ( to remove possible ERRORS)
				_angle = (1-curWeight)*srcToDestAngle;
				if( srcToDestAngle < 0) _angle *= -1;
				m_pModel->Rotate(axeRot, _angle);

				// shut down the rotation animation cycle
				//m_pModel->getMixer()->clearCycle(m_iAnimID, 0.0);
				clearAnimation( AN_CYCLEROTATION);


				if( dw_Mode == RM_ROTATETODESTPOINT)
				{
					startGotoDest();
					return;
				}else
				{
					// stay in the last pose
					if( params.id_stay_at_dest > -1)
					{
						float drt = (1-curWeight)*srcToDestAngle*duration/angle;
						//m_pModel->getMixer()->setAnimDuration(params.id_stay_at_dest, drt);
						m_pModel->getMixer()->setAction(params.id_stay_at_dest, 1000*m_pModel->getCoreModel()->getCoreAnimation(params.id_stay_at_dest)->getDuration());
					}
//char buf[255]; memset( &buf, 0, 255);
//sprintf( buf, "bDone = true {1}\n");OutputDebugString( buf);
					bDone = true;
				}
				curPhase = 0;
				return;
			}
			m_pModel->Rotate(axeRot, _angle);
		}
	}

	if( dw_Mode & RM_MOVING){

		float steplen = dPhase*m_iStepLength;
			CalVector	stepVector = axes*steplen;
			//TRACE("stepIt -> [dw_Mode == RM_MOVING]\n")

			if( params.header->pointNum != 0){

				// decrease first step in FOUR times
				if( dw_Mode & RM_FIRSTSTEP)	stepVector /= 4.0f;

				float		curWeight	= pathpiece.m_LengthWeight;

				if( pathpiece.m_Length != 0.0)	pathpiece.m_LengthWeight += steplen / pathpiece.m_Length;
				else	pathpiece.m_LengthWeight = 1.0;

				float		m_mangle;
				CalVector	m_mnormal;

				// если у нас неправильное направление
				// если у нас не выбрано правильное направление в рез-те блендинга
				// если у нас не определен флаг идти по направлению
				// если у нас не определен флаг повернутьс€ по направлению
				if( (params.header->pointNum > 0) && bTakeRightWay == false && !DirectionIsRight( m_mnormal, m_mangle))
				{
					// определ€ем пройденное рассто€ние по проекции
					CalVector	m_dirLine = CalVector(params.header->points[ pathpiece.curpoint].x,
														params.header->points[ pathpiece.curpoint].y,
														params.header->points[ pathpiece.curpoint].z);
								m_dirLine -= sourcePos;
					CalVector	m_curLine = m_pModel->getRootPosition();
								m_curLine -= sourcePos;
					m_dirLine.z = m_curLine.z = 0.0f;
					if( m_curLine.length() != 0.0) {
						float		m_angle = m_dirLine.getAngle( m_curLine);
						float		m_realLength = m_curLine.length()*cos( m_angle)/m_dirLine.length();
						if ( m_realLength < 1.0)
						{
							m_mnormal.x = m_mnormal.y = 0.0;// 081002
							m_pModel->Rotate(m_mnormal, m_mangle*fabs(m_realLength));
							pathpiece.m_LengthWeight = m_realLength;
						}else{
							pathpiece.m_LengthWeight = m_realLength = 1.0;
						}
					}
				}

				// ѕоследние полшага
				if( pathpiece.m_LengthWeight+steplen/pathpiece.m_Length >= 1.0 && pathpiece.m_LengthWeight  < 1.0)
				{
					if(params.header->pointNum != 0)
					{
						if( !(params.header->dw_Flags & SP_EVECTOR))
						{
							// ¬ конце нет поворота
 							if( params.id_stay_at_dest > -1)
							{
								float drt = (pathpiece.m_Length*(1-curWeight))/m_iStepLength*duration+0.1;
								//m_pModel->getMixer()->setAnimDuration(params.id_stay_at_dest, drt);
								m_pModel->getMixer()->setAction(params.id_stay_at_dest, 1000*m_pModel->getCoreModel()->getCoreAnimation(params.id_stay_at_dest)->getDuration());
								clearAnimation( AN_CYCLESTEP, drt);
								//m_pModel->getMixer()->clearCycle(m_iAnimID, drt-0.2);
							}
						}
					}
				}

				// we attained the destination
				if( pathpiece.m_LengthWeight >= 1.0)
				{
					if( pathpiece.m_LengthWeight > 1.0)
						stepVector *= (1-curWeight)/(pathpiece.m_LengthWeight-curWeight);

					if(params.header->pointNum != 0)
					{
						// если дошли до последней точки
						pathpiece.curpoint++;
						if( pathpiece.curpoint >= params.header->pointNum)
						{
							if( dw_Mode & RM_FIRSTSTEP)
								//m_pModel->getMixer()->clearAction(params.dw_FirstStepID);
								clearAnimation( AN_FIRSTSTEP);
							else
								//m_pModel->getMixer()->clearCycle(m_iAnimID, 0.0f);
								clearAnimation( AN_CYCLESTEP);

							m_iAnimID = -1;
							//	if we need END VECTOR rotation
							if( params.header->dw_Flags & SP_EVECTOR)
							{
								// align translation to correct value ( to remove possible ERRORS)
								CalVector	lastStep = CalVector( params.header->points[ pathpiece.curpoint - 1]);
								lastStep -= m_pModel->getSkeleton()->getRootPoint();
								lastStep.z = m_pModel->vTranslation.z;
								m_pModel->setMoveType( CMT_ABSOLUTE);
								m_pModel->Move( lastStep);
								m_pModel->setMoveType( CMT_RELATIVE);
								stROTATETODESTVECTOR();
								return;
							}else{
								if( params.id_stay_at_dest > -1)
								{
									float drt = (pathpiece.m_Length*(1-curWeight))/m_iStepLength*duration+0.1;
									//m_pModel->getMixer()->setAnimDuration(params.id_stay_at_dest, drt);
									m_pModel->getMixer()->setAction(params.id_stay_at_dest, 1000*m_pModel->getCoreModel()->getCoreAnimation(params.id_stay_at_dest)->getDuration());
								}
//char buf[255]; memset( &buf, 0, 255);
//sprintf( buf, "bDone = true {2}\n");OutputDebugString( buf);
								bDone=true;
							}
						}else{
							// если идем по кривой из более чем 2-х точек и конца ее не достигли
							bTakeRightWay = false;
							sourcePos = CalVector(params.header->points[ pathpiece.curpoint-1].x,
													params.header->points[ pathpiece.curpoint-1].y,
													params.header->points[ pathpiece.curpoint-1].z);
							//pathpiece.curpoint++;
							pathpiece.m_LengthWeight = 0.0;
							pathpiece.m_Length = CalVector(params.header->points[ pathpiece.curpoint].x-sourcePos.x,
													params.header->points[ pathpiece.curpoint].y-sourcePos.y,
													params.header->points[ pathpiece.curpoint].z-sourcePos.z
												).length();
						}
					}
				}
			}
/*if( m_pModel->getCoreModel()->getName().compare("boy") == 0){
char buf[255]; memset( &buf, 0, 255);
sprintf( buf, "Cal Move (%.2f, %.2f, %.2f)\n", stepVector.x, stepVector.y, stepVector.z);OutputDebugString( buf);
}*/
			m_pModel->Move( stepVector);
	}

}

void StepCtrl::setID(int id)
{
	m_iAnimID = id;
}

int StepCtrl::getID()
{
	return m_iAnimID;
}

void StepCtrl::beginStep(STEPPARAMS _params)
{
	bDone = false;
	dw_Animations = 0;
//{char buf[255]; memset( &buf, 0, 255);sprintf( buf, "BEGIN STEP=0\n", dw_Animations);OutputDebugString( buf);}
	setParams( _params);
	m_pModel->setRotType( CRT_ROOT);
	m_pModel->setMoveType( CMT_RELATIVE);
	m_dwLastTick = timeGetTime();
	curPhase = 0;
}

void StepCtrl::setAction(CalAnimationCycle *pa)
{
	action = pa;
}

CalAnimationCycle* StepCtrl::getAction()
{
	return action;
}

bool StepCtrl::done()
{
	return bDone;
}

void StepCtrl::endStep()
{
	bDone = true;
}

void StepCtrl::setRotation(CalVector axes, float rangle)
{
	axeRot = axes;
	angle = rangle;
	if( angle == 0)
		stepType &= (~ST_ROTONLY);
	else
		stepType |= ST_ROTONLY;

	if( stepType & ST_ROTONLY){
		m_pModel->saveRotPoint();
	}
}

void StepCtrl::setParams(STEPPARAMS _params)
{
	params = _params;

/*if( m_pModel->getCoreModel()->getName().compare("girl") == 0)
	int iii=0;*/

	if( !(params.dw_Flags & SP_SDELAY))
		params.dw_StartDelay = 0.0;
	if( !(params.dw_Flags & SP_EDELAY))
		params.dw_EndDelay = 0.0;
	if( !(params.dw_Flags & SP_WEIGHT))
		params.sf_Weight = 1.0;

	// определ€ем угол между текущим направлением модели
	// и направлением движени€

	// ≈сли поворачиваемс€ перед движением
	if(( params.header->dw_Flags & SP_SVECTOR)||(  params.header->dw_Flags & SP_SVLOOK)){
		CalVector	deltaVector;

		CalVector	modelPosition = m_pModel->getRootPosition();
		for( int i=0; i < params.header->pointNum; i++){
			if( params.header->dw_Flags & SP_KEEPX)
				params.header->points[i].x = modelPosition.x;
			if( params.header->dw_Flags & SP_KEEPY)
				params.header->points[i].y = modelPosition.y;
			if( params.header->dw_Flags & SP_KEEPZ)
				params.header->points[i].z = modelPosition.z;
		}


		if( params.header->dw_Flags & SP_SVECTOR)
		{
			// ¬ектор поворота задан
			deltaVector = CalVector(params.header->startDir.x, params.header->startDir.y, 0);
			deltaVector.z = m_pModel->getDirection().z;
		}else
		{
			// ¬ектор поворота идет по направлению
			if( params.header->pointNum != 0)
			{
				D3DVECTOR	*points = params.header->points;
				deltaVector = CalVector(points[0].x, points[0].y, points[0].z) - m_pModel->getRootPosition();
				if( deltaVector.length() == 0)
				{
					// ??
				}
				deltaVector.z = m_pModel->getDirection().z;
			}else
			{
				deltaVector = CalVector(0, 1, 0);
			}
		}

		if(( params.header->dw_Flags & SP_EVECTOR) &&( params.header->dw_Flags & SP_KEEPDIR) && !(dw_Mode&RTNSIMPLE))
		{
			CalVector endVector = m_pModel->getDirection();
			params.header->endDir.x = endVector.x;
			params.header->endDir.y = endVector.y;
			params.header->endDir.z = endVector.z;
		}

		if( deltaVector.length() == 0)
		{
			setRotation( CalVector(0, 0, 1), 0);
			// —разу идем по пр€мой
			startGotoDest();
			return;
		}

		// If Normal direction is UP then person:
		//	1. Stay at right foot
		//	2. Start going from left foot
		//	-------------------------------------
		// If Normal direction is DOWN then person:
		//	1. Stay at left foot
		//	2. Start going from right foot
		float _angle = m_pModel->getDirection().getAngle( deltaVector);
		if( _angle != 0.0){
			srcToDestAngle = _angle;
			CalVector	normal = m_pModel->getDirection().getNormal( deltaVector);
			normal.x = normal.y = 0.0;

			if( srcToDestAngle > 0)
				setRotation( normal, params.sf_RotAngle);// 081002
			else
				setRotation( normal, -params.sf_RotAngle);// 081002


			// ¬ыключаем движение
			setStepVector( CalVector(0.0f, 0.0f, 0.0f));
			// ”станавливаем номер анимации дл€ поворота
			//setID( params.dw_RtnID);
			// ”станавливаем режим шага
			dw_Mode = RM_ROTATETODESTPOINT;
			// —четчик движени€
			pathpiece.m_LengthWeight = 0.0f;
			// ѕараметры анимации на поворот
			m_accumulatedAngle = 0.0;
			doAnimation(AN_CYCLEROTATION | (normal.z>0?RTNCCW:RTNCW) | (srcToDestAngle>1.57?RTNMOREHALF:0), params.sf_Weight, params.dw_StartDelay);
			return;
		}else{
			params.header->dw_Flags &= ~SP_SVECTOR;
			params.header->dw_Flags &= ~SP_SVLOOK;
		}
	}
	setRotation( CalVector(0, 0, 1), 0);
	// —разу идем по пр€мой
	startGotoDest();
}

void StepCtrl::startGotoDest()
{
	//TRACE("startGotoDest\n")

	if( params.header->pointNum == 0)
	{
		if( params.header->dw_Flags & SP_EVECTOR)
		{
			stROTATETODESTVECTOR();
			return;
		}
//char buf[255]; memset( &buf, 0, 255);
//sprintf( buf, "bDone = true {3}\n");OutputDebugString( buf);
		bDone = true;
		return;
	}
	pathpiece.curpoint		= 0;
	bTakeRightWay	= false;
	dw_Mode			= RM_MOVING;
	// ¬ыключаем поворот при ходьбе
	setRotation( CalVector(0, 0, 1), 0);
	if( params.header->pointNum < 1){
		// »дем пр€мо без остановки
		setStepVector( CalVector( 0, params.sf_StepLength, 0));
	}else{
		CalVector	currentPoint	= m_pModel->getRootPosition();	
					sourcePos		= currentPoint;
		
		CalVector	deltaVector = CalVector(params.header->points[pathpiece.curpoint].x, 
											params.header->points[pathpiece.curpoint].y, 
											params.header->points[pathpiece.curpoint].z) - currentPoint;
		deltaVector.z = 0;

		pathpiece.m_Length = deltaVector.length();
		deltaVector.normalize();
		CalVector	stepVector = m_pModel->getRootDirection();

		stepVector.z = 0; // 081002
		stepVector.normalize();
		stepVector *= params.sf_StepLength;
		setStepVector( stepVector);
	}
	pathpiece.m_LengthWeight = 0.0f;
	//	setID( params.dw_AnimID);

	float	delay = 0.0;
	if( params.useFirstStep())
	{
		dw_Mode |= RM_FIRSTSTEP;
		doAnimation( AN_FIRSTSTEP, 0.0, 0.0);
	}else
	{
		doAnimation( AN_CYCLESTEP, params.sf_Weight, delay);
	}
	m_dwLastTick = timeGetTime();
}

bool StepCtrl::canBeCleared()
{
	if( dw_Mode == RM_ROTATETODESTPOINT) return false;
	if( dw_Mode == RM_MOVING) return false;
	return true;
}

void StepCtrl::setDuration(float dur)
{
	duration = dur;
}

void StepCtrl::stROTATETODESTVECTOR()
{
	//TRACE("stROTATETODESTVECTOR\n")
	// определ€ем угол между текущим направлением модели
	// и направлением движени€
	CalVector	modelDirection2d = m_pModel->getDirection();
	modelDirection2d.z = 0.0;

	CalVector deltaVector( params.header->endDir.x, params.header->endDir.y, params.header->endDir.z);

	deltaVector.z = modelDirection2d.z;
	float _angle = modelDirection2d.getAngle( deltaVector);
		// »дем не по пр€мой, нужно вначале повернутьс€
	if( _angle != 0.0){
		CalVector normal = modelDirection2d.getNormal( deltaVector);
		srcToDestAngle = _angle;
		if( srcToDestAngle > 0)
			setRotation( normal, params.sf_RotAngle);
		else
			setRotation( normal, -params.sf_RotAngle);
		// ¬ыключаем движение
		setStepVector( CalVector(0.0f, 0.0f, 0.0f));
		// ”станавливаем номер анимации дл€ поворота
		//setID( params.dw_RtnID);
		// ”станавливаем режим шага
		dw_Mode = RM_ROTATETODESTVECTOR;
		// —четчик движени€
		pathpiece.m_LengthWeight = 0.0f;
		// ѕараметры анимации на поворот
		//if( params.header->dw_Flags == SP_EVECTOR)
		if( params.id_bg_rleg_left>-1 && params.id_bg_lleg_right>-1)
			doAnimation(AN_STARTURNROTATION | (normal.z>0?RTNCCW:RTNCW), params.sf_Weight, params.dw_StartDelay);
		else
			doAnimation( AN_CYCLEROTATION | RTNSIMPLE, params.sf_Weight, 0.0);
		m_dwLastTick = timeGetTime();
		return;
	}
//char buf[255]; memset( &buf, 0, 255);
//sprintf( buf, "bDone = true {4}\n");OutputDebugString( buf);
	bDone = true;
}

bool StepCtrl::DirectionIsRight(CalVector &normal, float &angle)
{
//	if( (params.header->dw_Flags && SP_SVECTOR) && params.header->pointNum < 2)
//		return true;
		// смотрим совпадение вектора шага и вектора от центра модели до точки назначени€
		CalVector	dStepVector;
		CalVector	currentPoint = m_pModel->getRootPosition();
		CalVector	deltaVector = CalVector(params.header->points[pathpiece.curpoint].x, 
										params.header->points[pathpiece.curpoint].y, 
										params.header->points[pathpiece.curpoint].z) - currentPoint;
		deltaVector.z = 0;
		//CalQuaternion	m_qRotation = m_pModel->qRotation;
		//m_qRotation.conjugate();
		dStepVector = m_pModel->getDirection();//getRootDirection();
		//dStepVector *= m_qRotation;
		dStepVector.z = 0;
		//dStepVector.invert();

		CalVector	m_normal;
		float		m_angle;

		m_angle = dStepVector.getAngle( deltaVector);
		if( m_angle == 0.0)	{
			pathpiece.m_LengthWeight = 0.0;
			pathpiece.m_Length = deltaVector.length();
			bTakeRightWay = true;
			return true;
		}
		m_normal = dStepVector.getNormal( deltaVector);

		normal = m_normal;
		angle = m_angle;
		return false;
}

void StepCtrl::doAnimation(DWORD type, float param1, float param2)
{
	int	animid = 0;
	int	steptype = 0;

//{char buf[255]; memset( &buf, 0, 255);sprintf( buf, "dw_Animations{1.1}=%d\n", dw_Animations);OutputDebugString( buf);}
	dw_Animations |= type;
//{char buf[255]; memset( &buf, 0, 255);sprintf( buf, "dw_Animations{1.2}=%d\n", dw_Animations);OutputDebugString( buf);}
	steptype = type&0x00FF;

	switch( steptype)
	{
	case AN_CYCLESTEP:
		{
			animid = params.id_cycle_step_lr;
			if( type&RTNCW){animid = params.id_cycle_step_rl;}
			m_pModel->getMixer()->executeStep( animid, param1, param2);
		}break;
	case AN_FIRSTSTEP:
		{
			int	fstepid = params.id_first_step_l;
			if( type&RTNCCW){animid = params.id_rest_on_r;}
			if( !(( params.header->dw_Flags & SP_SVECTOR)||(  params.header->dw_Flags & SP_SVLOOK)))
			{
				m_pModel->getMixer()->executeAction( fstepid, param1, param2);
				animid = fstepid;
			}
			else
				return;
		}break;
	case AN_CYCLEROTATION:
		{
			if( type&RTNSIMPLE)
			{
				animid = params.id_cycle_rotation;
				m_pModel->getMixer()->executeStep( animid, param1, param2);
			}
			else
			{
				if( type&RTNMOREHALF)
				{
					// rotate more then 90 grad.
					if( type&RTNCCW){animid = params.id_back_ccw;}
					else	animid = params.id_back_cw;

					dw_Mode |= RTNSIMPLE;
					m_pModel->getMixer()->executeAction( animid, 0.0, param2);
				}else
				{
					int	fstepid = params.id_first_step_l;
					if( type&RTNCCW){animid = params.id_rest_on_r;}
					else{animid = params.id_rest_on_l; fstepid = params.id_first_step_r;}

					// determine the MIX duration
					float rtndur = m_pModel->getCoreModel()->getCoreAnimation(animid)->getDuration();
					float	angk = srcToDestAngle / params.sf_RotAngle;
					m_pModel->getMixer()->setAction( animid, m_pModel->getCoreModel()->getCoreAnimation(animid)->getDuration()*1000.0f);
					m_pModel->getMixer()->executeAction( fstepid, angk*rtndur, param2);
				}
			}
		}break;
	case AN_STARTURNROTATION:
		{	// Start turn to dest vector
			int footrtnid = params.id_bg_lleg_right;
			animid = params.id_rest_on_l;
			if( type&RTNCCW)
			{
				animid = params.id_rest_on_r;
				footrtnid = params.id_bg_rleg_left;
			}
			//
			//float	angk = srcToDestAngle/1.57f;
			//if( angk > 1.0f)	angk = 1.0f;
			//
			//m_pModel->getMixer()->setAction( animid, m_pModel->getCoreModel()->getCoreAnimation(animid)->getDuration()*1000.0f);
			m_pModel->getMixer()->executeStep( animid, 1.0, 0.0);
			if( srcToDestAngle > 0.35)	// 20 degrees
				m_pModel->getMixer()->executeAction( footrtnid, 0.0, 0.0);

		}break;
	}

	setID( animid);
	setDuration( m_pModel->getCoreModel()->getCoreAnimation(animid)->getDuration());
}

void StepCtrl::clearAnimation(DWORD type, float param1)
{
	int	animid = 0;

//{char buf[255]; memset( &buf, 0, 255);sprintf( buf, "dw_Animations{2.1}=%d\n", dw_Animations);OutputDebugString( buf);}
	dw_Animations &= ~type;
//{char buf[255]; memset( &buf, 0, 255);sprintf( buf, "dw_Animations{2.1}=%d\n", dw_Animations);OutputDebugString( buf);}

	switch( type)
	{
	case AN_CYCLESTEP:
		{
			m_pModel->getMixer()->clearCycle( params.id_cycle_step_lr, param1);
			m_pModel->getMixer()->clearCycle( params.id_cycle_step_rl, param1);
		}break;
	case AN_FIRSTSTEP:
		{
//OutputDebugString( "Clear first step\n");
			animid = params.id_first_step_l;
			m_pModel->getMixer()->clearAction(animid);
			animid = params.id_first_step_r;
			m_pModel->getMixer()->clearAction(animid);
		}break;
	case AN_CYCLEROTATION:
		{
//OutputDebugString( "Clear cycle rotation\n");
			animid = params.id_rest_on_r;
			//
			m_pModel->getMixer()->clearCycle( params.id_rest_on_r, 0.0);
			m_pModel->getMixer()->clearCycle( params.id_rest_on_l, 0.0);
			m_pModel->getMixer()->clearCycle( params.id_cycle_rotation, 0.0);
			//
			m_pModel->getMixer()->clearAction( params.id_rest_on_r);
			m_pModel->getMixer()->clearAction( params.id_rest_on_l);
			//
			//if( dw_Animations & AN_STARTURNROTATION){
				m_pModel->getMixer()->clearAction( params.id_bg_rleg_left);
				m_pModel->getMixer()->clearAction( params.id_bg_lleg_right);
{char buf[255]; memset( &buf, 0, 255);sprintf( buf, "dw_Animations{3.1}=%d\n", dw_Animations);OutputDebugString( buf);}
				dw_Animations &= ~AN_STARTURNROTATION;
{char buf[255]; memset( &buf, 0, 255);sprintf( buf, "dw_Animations{3.1}=%d\n", dw_Animations);OutputDebugString( buf);}
			//}
		}break;
			
	}
}


void StepCtrl::animDone(int id)
{
char buf[255]; memset( &buf, 0, 255);
sprintf( buf, "!! dw_Animations %i, id=%i, l=%i, r=%i\n", dw_Animations, id, params.id_first_step_l, params.id_first_step_r);OutputDebugString( buf);

	if( dw_Animations & AN_FIRSTSTEP)
	{
		if( id == params.id_first_step_l)
		{
if( m_pModel->getCoreModel()->getName() == "girl" == 0)
int iii=0;
			OutputDebugString( "Done 1 FIRST STEP\n");
			doAnimation( AN_CYCLESTEP | RTNCCW, 1.0, 0.0);
			m_pModel->setDoneFlag(AT_STEPSOUNDL, 0);
			clearAnimation( AN_FIRSTSTEP);
			dw_Mode = RM_MOVING;
		}else{
			if( id == params.id_first_step_r)
			{
if( m_pModel->getCoreModel()->getName() == "girl" == 0)
int iii=0;
				OutputDebugString( "Done 2 FIRST STEP\n");
				doAnimation( AN_CYCLESTEP | RTNCW, 1.0, 0.0);
				m_pModel->setDoneFlag(AT_STEPSOUNDR, 0);
				clearAnimation( AN_FIRSTSTEP);
				dw_Mode = RM_MOVING;
			}
		}
	}else
	if( dw_Animations & AN_CYCLEROTATION)
	{
		if( id == params.id_back_ccw){
			//doAnimation( AN_CYCLEROTATION | RTNCCW | RTNHALFTAIL, 1.0, 0.0);
			setParams( params);
			//dw_Mode &= ~RTNSIMPLE;
		}else{
			if( id == params.id_back_cw){
				//doAnimation( AN_CYCLEROTATION | RTNCW | RTNHALFTAIL, 1.0, 0.0);
				setParams( params);
				//dw_Mode &= ~RTNSIMPLE;
			}
		}
	}
}
