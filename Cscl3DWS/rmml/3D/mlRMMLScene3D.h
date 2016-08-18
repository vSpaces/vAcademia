#if _MSC_VER > 1000
#pragma once
#endif

#ifndef _mlRMMLScene3D_H_
#define _mlRMMLScene3D_H_

namespace rmml {

class mlRMMLCloud;

/**
 * Класс 3D-сцен RMML
 */

class mlRMMLScene3D :	
					public mlRMMLElement,
					public mlJSClass,
					public mlIScene3D
{
public:
	mlRMMLScene3D(void);
	void destroy() { MP_DELETE_THIS }
	~mlRMMLScene3D(void);
MLJSCLASS_DECL
private:
	enum {
		JSPROP_useMapManager
	};
	bool useMapManager;

//	JSFUNC_DECL(setEditableBoundsColor)	// установить цвет boundbox-а всех редектируемых объектов и показать их, если цвет непрозрачный

// реализация mlRMMLElement
MLRMMLELEMENT_IMPL
	mlresult CreateMedia(omsContext* amcx);
	mlRMMLElement* Duplicate(mlRMMLElement* apNewParent);
	bool Freeze(){ 
		if(!mlRMMLElement::Freeze()) return false; 
		return FreezeChildren(); 
	}
	bool Unfreeze(){ 
		if(!mlRMMLElement::Unfreeze()) return false; 
		return UnfreezeChildren(); 
	}
	virtual void ClearRefs(){ ClearIntersections(); }
	mlIScene3D* GetIScene3D(){ return this; }

private:
	bool mbClearingIntersections;
//	mlColor mEditableBBoxColor;	// цвет boundingbox-а по умолчанию для редактируемых объектов
//	mlColor mBBoxColor; // цвет boundingbox-а по умолчанию для нередактируемых объектов
	typedef std::vector<mlRMMLCloud*> CloudVector;
	CloudVector mvVisibleClouds; // список всех видимых cloud-ов 3D-объектов

//	void ResetEditableBBoxColor();
//	bool IsEditableObject(mlRMMLElement* apElem);
public:
	bool ClearingIntersections(){ return mbClearingIntersections; }
	void ClearIntersections();
	// получить цвет по умолчанию для bounding-box-а заданного объекта
//	mlColor GetDefaultBoundingBoxColor(mlRMMLElement* apElem);

	// Зарегистрировать видимый cloud
	bool RegisterVisibleCloud(mlRMMLCloud* apCloud);
	// Разрегистрировать cloud
	void UnregisterCloud(mlRMMLCloud* apCloud);
	// Поискать кнопку в cloud-е, который под мышкой
	mlRMMLElement* GetCloudButtonUnderMouse(mlPoint &aXY, mlButtonUnderMouse &aBUM, mlRMMLCloud* &pCloud);

// Реализация mlIScene3D
	bool GetUseMapManager(){ return useMapManager; }

// Реализация moIScene3D
	void UseMapManagerChanged();

};

}

#endif
