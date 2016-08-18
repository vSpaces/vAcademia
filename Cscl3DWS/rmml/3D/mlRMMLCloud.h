#if _MSC_VER > 1000
#pragma once
#endif

#ifndef _mlRMMLCloud_H_
#define _mlRMMLCloud_H_

namespace rmml {

/**
 * Класс 2D-'облачков' для 3D сцен 
 */

/*
	content 		ref 	ссылка на композицию с облаком
	footnote 		string 	имя картинки сноски (в композиции, на которую ссылается content)
	align 			string 	выравнивание относительно точки привязки (“left”, “top;left”…)
	moveOutOfScreen bool 	выводить за пределы экрана или нет
	allowOverlap	bool 	разрешать перекрывание с другими облаками
	sortOnDepth 	bool 	сортировать по глубине
	position 		pos3D 	координаты привязки (относительно объекта attachedTo или абсолютные)
	attachedTo 		ref 	объект, к которому привязано облако
*/

#define SET_GET_POS_C2(C) \
	void SetPos_##C(double aiNewVal){ \
		CreateJSPos(); pos.C=aiNewVal; \
		PosChanged(pos); \
	} \
	double GetPos_##C(){ return GetPos().C; }

class mlRMMLCloud :	
					public mlRMMLElement,
					public mlJSClass,
					public mlRMMLLoadable,
					// public mlRMMLVisible,
					public mlICloud,
					public moICloud,
					public mlIVisibleNotifyListener
{
	bool mbInverse;
	JSString* mjssContent;
	bool mbMLElsCreated;
public:
	mlRMMLCloud(void);
	void destroy() { MP_DELETE_THIS }
	~mlRMMLCloud(void);
MLJSCLASS_DECL
private:
	enum {
		JSPROP_inverse = 1,
		JSPROP_visible,
		JSPROP_maxVisibleDistance,
		JSPROP_duration,
		JSPROP_content,
		JSPROP_footnote,
		JSPROP_align,
		JSPROP_moveOutOfScreen,
		JSPROP_allowOverlap,
		JSPROP_sortOnDepth,
		JSPROP_position,
		JSPROP_attachedTo,
		JSPROP_marginX,
		JSPROP_marginY,
		JSPROP_viewVec
	};
	bool visible;
	double duration;
	double maxVisibleDistance;
	mlRMMLComposition* content;
	JSString* footnote;
	JSString* align;
	bool moveOutOfScreen;
	bool allowOverlap;
	bool sortOnDepth;
	ml3DPosition pos;
	JSString* attachedTo;
	mlRMMLElement* mp3DOattachedTo;
	int marginX, marginY;
	ml3DPosition viewVec;

	JSFUNC_DECL(attachTo)
	JSFUNC_DECL(setSpecParameters)

private:
	struct mlJS3DPos: public mlI3DPosition{
		mlRMMLCloud* mpCloud;
		JSObject* mjsoPos;
		mlJS3DPos(mlRMMLCloud* apCloud);
		// mlI3DPosition
		void SetPos_x(double adNewVal){ mpCloud->SetPos_x(adNewVal); }
		void SetPos_y(double adNewVal){ mpCloud->SetPos_y(adNewVal); }
		void SetPos_z(double adNewVal){ mpCloud->SetPos_z(adNewVal); }
		ml3DPosition GetPos(){ return mpCloud->GetPos(); }
		void SetPos(ml3DPosition &aPos){ mpCloud->SetPos(aPos); }
	};
	mlJS3DPos* mpJSPos;
	SET_GET_POS_C2(x)
	SET_GET_POS_C2(y)
	SET_GET_POS_C2(z)

	void CreateJSPos();

	void SetAttachedTo(JSString* ajssName);

	mlRMMLScene3D* mpScene3DRegistered;	// 3D-сцена, в которой зарегистрирован cloud
	// Зарегистрировать/разрегистрировать cloud в 3D-сцене, если он сам и его контент являются видимыми/невидимым
	void UpdateRegistration();
	// Разрегистрировать cloud из 3D-сцены, если он там был зарегистрирован
	void Unregister();
	// Получить 3D-сцену, которая является одним из родителей cloud-а
	mlRMMLScene3D* Get3DScene();
	// Возвращает true, если и сам cloud и его контент являеются видимыми
	bool IsVisible();

// реализация mlRMMLElement
MLRMMLELEMENT_IMPL
	mlresult CreateMedia(omsContext* amcx);
	mlresult Load();
	mlRMMLElement* Duplicate(mlRMMLElement* apNewParent);

// реализация  mlILoadable
MLILOADABLE_IMPL

// реализация  mlIVisible
//MLIVISIBLE_IMPL

// реализация mlICloud
public:
	mlICloud* GetICloud(){ return this; } 
	mlMedia* GetText();
	mlMedia* GetNicknameText();
	bool GetVisibility(){ return visible; }
	mlMedia* GetContent();
	mlMedia* GetFootnote();
	const wchar_t* GetAlign();
	bool GetMoveOutOfScreen();
	bool GetAllowOverlap();
	bool GetSortOnDepth();
	mlMedia* GetAttachedTo();
	int GetMarginX();
	int GetMarginY();
	double GetMaxVisibleDistance();

// реализация moICloud
public:
	// Свойства
	moICloud* GetmoICloud();
	void VisibleChanged();
	void ContentChanged();
	void FootnoteChanged();
	void MarginXChanged();
	void MarginYChanged();
	void AlignChanged();
	void MoveOutOfScreenChanged();
	void AllowOverlapChanged();
	void SortOnDepthChanged();
	void MaxVisibleDistanceChanged();
	void ViewVecChanged(ml3DPosition &viewVec);
	ml3DPosition GetPos();
	void SetPos(ml3DPosition &aPos);
	void PosChanged(ml3DPosition &pos); // изменилось положение 
	void AttachedToChanged(mlMedia* pmlMedia);
	void SetSpecParameters(int adeltaNoChangePos, int ashiftAtAvatarPosZ);

	// реализация mlIVisibleNotifyListener
public:
	// изменилось визуальное представление видимого элемента
	virtual void VisualRepresentationChanged(mlMedia* map);
	// видимый элемент уничтожился и больше от него уведомлений не будет
	virtual void VisibleDestroyed(mlMedia* map);
};

}

#endif
