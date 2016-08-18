#if _MSC_VER > 1000
#pragma once
#endif

#ifndef _mlRMMLCamera_H_
#define _mlRMMLCamera_H_

namespace rmml {

/**
 * Класс камеры для 3D-сцен RMML
 */

class mlRMMLCamera :	
					public mlRMMLElement,
					public mlJSClass,
					public mlRMMLLoadable,
					public mlRMML3DObject,
					public mlICamera,
					public moICamera
{
	bool mbMLElsCreated;
public:
	mlRMMLCamera(void);
	void destroy() { MP_DELETE_THIS }
	~mlRMMLCamera(void);
MLJSCLASS_DECL
private:
	enum {
		JSPROP_fov = 1,
		JSPROP_dest,
		JSPROP_linkTo,
		JSPROP_maxLinkDistance,
		JSPROP_fixedPosition,
		JSPROP_up,
		JSPROP_destOculus,
		EVID_onTrackFilePlayed
	};
	unsigned int fov;
	bool fixedPosition;
public:
	ml3DPosition dest;
	ml3DPosition destOculus;
	ml3DPosition up;
	JSString* linkTo;
	mlRMMLElement* mp3DOlinkTo;
	double maxLinkDistance;

	void SetLinkTo(JSString* ajssName);

	void SetMaterialTiling(int, float) {};
	void EnableShadows(bool isEnabled){};
	void ChangeCollisionLevel(int level){};

private:
	struct mlJS3DDest: public mlI3DPosition{
		mlRMMLCamera* mpCamera;
		JSObject* mjsoDest;
		mlJS3DDest(mlRMMLCamera* apCamera);
		// mlI3DPosition
		void SetPos_x(double adNewVal){ mpCamera->dest.x=adNewVal; mpCamera->DestChanged(); }
		void SetPos_y(double adNewVal){ mpCamera->dest.y=adNewVal; mpCamera->DestChanged(); }
		void SetPos_z(double adNewVal){ mpCamera->dest.z=adNewVal; mpCamera->DestChanged(); }
		ml3DPosition GetPos(){ return mpCamera->dest; }
		void SetPos(ml3DPosition &pos){ mpCamera->dest=pos; }
	};
	mlJS3DDest* mpJSDest;
	struct mlJS3DDestOculus: public mlI3DPosition{
		mlRMMLCamera* mpCamera;
		JSObject* mjsoDestOculus;
		mlJS3DDestOculus(mlRMMLCamera* apCamera);
		// mlI3DPosition
		void SetPos_x(double adNewVal){ mpCamera->destOculus.x=adNewVal; mpCamera->DestOculusChanged(); }
		void SetPos_y(double adNewVal){ mpCamera->destOculus.y=adNewVal; mpCamera->DestOculusChanged(); }
		void SetPos_z(double adNewVal){ mpCamera->destOculus.z=adNewVal; mpCamera->DestOculusChanged(); }
		ml3DPosition GetPos(){ return mpCamera->destOculus; }
		void SetPos(ml3DPosition &pos){ mpCamera->destOculus=pos; }
	};	
	mlJS3DDestOculus* mpJSDestOculus;
	struct mlJS3DUp: public mlI3DPosition{
		mlRMMLCamera* mpCamera;
		JSObject* mjsoUp;
		mlJS3DUp(mlRMMLCamera* apCamera);
		// mlI3DPosition
		void SetPos_x(double adNewVal){ mpCamera->up.x=adNewVal; mpCamera->UpChanged(); }
		void SetPos_y(double adNewVal){ mpCamera->up.y=adNewVal; mpCamera->UpChanged(); }
		void SetPos_z(double adNewVal){ mpCamera->up.z=adNewVal; mpCamera->UpChanged(); }
		ml3DPosition GetPos(){ return mpCamera->up; }
		void SetPos(ml3DPosition &pos){ mpCamera->up=pos; }
	};
	mlJS3DUp* mpJSUp;

	void CreateJSDest();
	void CreateJSDestOculus();
	void CreateJSUp();

// реализация mlRMMLElement
MLRMMLELEMENT_IMPL
	mlresult CreateMedia(omsContext* amcx);
	mlresult Load();
	mlRMMLElement* Duplicate(mlRMMLElement* apNewParent);

// реализация  mlILoadable
MLILOADABLE_IMPL

// реализация  mlI3DObject
MLI3DOBJECT_IMPL
	bool GetBillboard(){ return false; }
	bool GetUnpickable(){ return false; }

// реализация  moI3DObject
	void BillboardChanged(){}
	void UnpickableChanged(){}

// реализация mlICamera
	mlICamera* GetICamera(){ return this; }
	int GetFOV(){ return fov; }
	void SetFOV(int aiFOV){ fov=aiFOV; }
	bool GetFixedPosition() { return fixedPosition; }
	ml3DPosition GetDest(){ return dest; }
	ml3DPosition GetDestOculus(){ return destOculus; }
	ml3DPosition GetUp(){ return up; }
	void SetDest(ml3DPosition aposDest){ dest=aposDest; }
	mlMedia* GetLinkTo();
	double GetMaxLinkDistance(){ return maxLinkDistance; }

// moICamera
	void SetNearPlane(float nearPlane);
	void FOVChanged();
	void DestChanged();
	void DestOculusChanged();
	void UpChanged();
	void ResetDestOculus();
	void LinkToChanged();
	void FixedPositionChanged();
	void MaxLinkDistanceChanged();
	float GetZLevel();
	int  destMoveTo(ml3DPosition aPos3D, int aiDuaration, bool abCheckCollisions);
	void  Rotate(int aiDuration, int aiTime);
	void  Move(int aiDuration, int aiTime);
	void  ClearOffset();
	void CorrectPosition();
	bool HasOffset();
	bool IsFullyLoaded() {return false;}
	bool IsIntersectedWithOtherObjects() {return false;}
	void PlayTrackFile(const wchar_t* apwcFilePath);
	void LevelChanged(int /*level*/) {};
	int GetCurrentLevel() { return 0; }
	void onTrackFilePlayed();

protected:
	JSFUNC_DECL(destMoveTo)
	JSFUNC_DECL(setNearPlane)
	JSFUNC_DECL(getZLevel)
	JSFUNC_DECL(rotate)
	JSFUNC_DECL(move)
	JSFUNC_DECL(clearOffset)
	JSFUNC_DECL(hasOffset)
	JSFUNC_DECL(correctPosition)
	JSFUNC_DECL(playTrackFile)
	JSFUNC_DECL(resetDestOculus)
};

}

#endif
