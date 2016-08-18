#if _MSC_VER > 1000
#pragma once
#endif

#ifndef _mlRMMLMotionAOs_H_
#define _mlRMMLMotionAOs_H_


namespace rmml {

class mlMotionTrackFrames;

/**
 * JS-класс для track-а элемента motion RMML
 */

class mlMotionTrack:	
					public mlJSClass
{
friend class mlMotionTracks;
friend class mlMotionTrackFrame;
	MP_WSTRING msBoneName;
	int miBoneNum;
	mlRMMLMotion* mpMotion;
	mlMotionTrackFrames* frames;
public:
	mlMotionTrack(void);
	void destroy() { MP_DELETE_THIS; }
	~mlMotionTrack(void);
MLJSCLASS_DECL2(mlMotionTrack)
private:
	enum {
		JSPROP_frames,
	};
};

/**
 * JS-класс для track-ов элемента motion RMML
 */

class mlMotionTracks:
					public mlJSClass
{
friend class mlRMMLMotion;
	mlRMMLMotion* mpMotion;
public:
	mlMotionTracks(void);
	void destroy() { MP_DELETE_THIS; }
	~mlMotionTracks(void);
MLJSCLASS_DECL2(mlMotionTracks)
private:
};

/**
 * JS-класс для кадра track-а элемента motion RMML
 */

class mlMotionTrackFrame:	
					public mlJSClass,
					public mlI3DPosition,
					public mlI3DScale,
					public mlI3DRotation
{
friend class mlMotionTrackFrames;
	mlPosition3D* _pos;
	mlScale3D* _scale;
	mlRotation3D* _rotation;
	unsigned int muIndex;
	mlMotionTrack* mpMotionTrack;
public:
	mlMotionTrackFrame(void);
	void destroy() { MP_DELETE_THIS; }
	~mlMotionTrackFrame(void);
MLJSCLASS_DECL2(mlMotionTrackFrame)
private:
	enum {
		JSPROP__pos,
		JSPROP__scale,
		JSPROP__rotation
	};
	void CreatePos();
	void CreateScale();
	void CreateRotation();
// реализация mlI3DPosition
public:
	void SetPos_x(double adNewVal){
		ml3DPosition pos = GetPos(); 
		pos.x = adNewVal;
		SetPos(pos);
	}
	void SetPos_y(double adNewVal){
		ml3DPosition pos = GetPos(); 
		pos.y = adNewVal;
		SetPos(pos);
	}
	void SetPos_z(double adNewVal){
		ml3DPosition pos = GetPos(); 
		pos.z = adNewVal;
		SetPos(pos);
	}
	ml3DPosition GetPos();
	void SetPos(ml3DPosition &pos);
// реализация mlI3DScale
public:
	void SetScl_x(double adNewVal){
		ml3DScale scl = GetScl(); 
		scl.x = adNewVal;
		SetScl(scl);
	}
	void SetScl_y(double adNewVal){
		ml3DScale scl = GetScl(); 
		scl.y = adNewVal;
		SetScl(scl);
	}
	void SetScl_z(double adNewVal){
		ml3DScale scl = GetScl(); 
		scl.z = adNewVal;
		SetScl(scl);
	}
	ml3DScale GetScl();
	void SetScl(ml3DScale &scl);
// реализация mlI3DRotation
public:
	void SetRot_x(double adNewVal){
		ml3DRotation rot = GetRot();
		rot.x = adNewVal;
		SetRot(rot);
	}
	void SetRot_y(double adNewVal){
		ml3DRotation rot = GetRot();
		rot.y = adNewVal;
		SetRot(rot);
	}
	void SetRot_z(double adNewVal){
		ml3DRotation rot = GetRot();
		rot.z = adNewVal;
		SetRot(rot);
	}
	void SetRot_a(double adNewVal){
		ml3DRotation rot = GetRot();
		rot.a = adNewVal;
		SetRot(rot);
	}
	ml3DRotation GetRot();
	void SetRot(ml3DRotation &aRot);
};

/**
 * JS-класс для кадров track-а элемента motion RMML
 */

class mlMotionTrackFrames:
					public mlJSClass
{
friend class mlRMMLMotion;
friend class mlMotionTrack;
	mlMotionTrack* mpMotionTrack;
public:
	mlMotionTrackFrames(void);
	void destroy() { MP_DELETE_THIS; }
	~mlMotionTrackFrames(void);
MLJSCLASS_DECL2(mlMotionTrackFrames)
private:
};

}

#endif // _mlRMMLMotionAOs_H_