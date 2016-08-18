#pragma once

namespace rmml {

#define EVNM_onPress L"onPress"
#define EVNM_onRelease L"onRelease"
#define EVNM_onReleaseOutside L"onReleaseOutside"
#define EVNM_onRollOver L"onRollOver"
#define EVNM_onRollOut L"onRollOut"
#define EVNM_onKeyDown L"onKeyDown"
#define EVNM_onKeyUp L"onKeyUp"
#define EVNM_onSetFocus L"onSetFocus"
#define EVNM_onKillFocus L"onKillFocus"
#define EVNM_onAltPress L"onAltPress"
#define EVNM_onAltRelease L"onAltRelease"
#define EVNM_onAltReleaseOutside L"onAltReleaseOutside"

/**
 * RMML-подкласс для RMML-классов, которые могут быть активными зонами
 * (Composition, Image, Animation, ...)
 * (не путать с прототипами JavaScript!)
 */

class RMML_NO_VTABLE mlRMMLPButton: 
									//public mlJSClassProto,
									public mlIButton,
									public moIButton
{
friend class mlRMMLElement;
public:
	mlRMMLPButton(void);
	~mlRMMLPButton(void);
MLJSPROTO_DECL
private:
	enum {JSPROP0_mlRMMLPButton=TIDB_mlRMMLPButton-1,
		JSPROP_enabled,
		JSPROP_useHandCursor,
		JSPROP_hitArea,
		JSPROP_modal,
		TIDE_mlRMMLPButton
	};
public:
	enum { EVID_mlRMMLPButton=TIDE_mlRMMLPButton-1,
		EVID_onPress,
		EVID_onRelease,
		EVID_onReleaseOutside,
		EVID_onRollOver,
		EVID_onRollOut,
		EVID_onKeyDown,
		EVID_onKeyUp,
		EVID_onSetFocus,
		EVID_onKillFocus,
		EVID_onAltPress,		// альтернативное нажатие на кнопку (double click, right mouse button, middle mouse button)
		EVID_onAltRelease,		// альтернативное отпускание кнопки (right mouse button, middle mouse button)
		EVID_onAltReleaseOutside, // альтернативное отпускание вне кнопки (right mouse button, middle mouse button)
		TEVIDE_mlRMMLPButton
	};
protected:
//"enabled" type="xs:boolean"/>
//"useHandCursor" type="xs:boolean" default="true"/>
//"hitArea" type="hitAreaType"/>
	bool enabled;
	unsigned int useHandCursor;
	class mlHitArea{
		struct HARegion{
			HARegion* mpNext;
			HARegion(){mpNext=NULL;}
			virtual mlresult Parse(wchar_t* &pwc){ return ML_ERROR_NOT_IMPLEMENTED; }
			virtual bool IsPointIn(mlPoint &aPnt){ return false; }
			virtual mlRect GetBounds()=0;
			virtual void Destroy() = 0;
		};
		struct HARect: public HARegion{
			int miX,miY,miW,miH;
			mlresult Parse(wchar_t* &pwc);
			bool IsPointIn(mlPoint &aPnt);
			mlRect GetBounds(){
				mlRect rc; 
				rc.left=miX; rc.top=miY;
				rc.right=miX+miW; rc.bottom=miY+miH;
				return rc;
			}
			void Destroy() { MP_DELETE_THIS; };
		};
		struct HAEllipse: public HARegion{
			int miX,miY,miW,miH;
//			mlresult Parse(wchar_t* &pwc);
//			bool IsPointIn(mlPoint &aPnt);
			mlRect GetBounds(){
				mlRect rc; 
				rc.left=miX; rc.top=miY;
				rc.right=miX+miW; rc.bottom=miY+miH;
				return rc;
			}
			void Destroy() { MP_DELETE_THIS; };
		};
		struct HAPoly: public HARegion{
			struct PolyPoint{
				mlPoint mPoint;
				PolyPoint* mpNext;
				PolyPoint(){mpNext=NULL;}
			};
			PolyPoint* mpPoints;
			HAPoly(){ mpPoints=NULL; }
			void Clear(){
				while(mpPoints){ 
					PolyPoint* pToDel=mpPoints; 
					mpPoints=mpPoints->mpNext; 
					 MP_DELETE( pToDel);
				} 
			}
			~HAPoly(){Clear();}
			mlresult Parse(wchar_t* &pwc);
			bool IsPointIn(mlPoint &aPnt);
			mlRect GetBounds();
			void Destroy() { MP_DELETE_THIS; };
		};
		struct HAMask: public HARegion{
			mlRMMLElement* mpMLEl;
			HAMask(){mpMLEl=NULL;}
//			mlresult Parse(wchar_t* &pwc);
//			bool IsPointIn(mlPoint &aPnt);
			mlRect GetBounds(){
				mlRect rc; ML_INIT_RECT(rc);
				if(mpMLEl!=NULL){
					ML_ERROR(mpMLEl->mcx, "HAMask::GetBounds");
				}
				// ??
				return rc;
			}
			void Destroy() { MP_DELETE_THIS; };
		};
		HARegion* mpFirst;
	public:
		mlHitArea(){ mpFirst=NULL; }
		~mlHitArea(){ Clear(); }
		void Clear(){
			while(mpFirst){ 
				HARegion* pToDel=mpFirst; 
				mpFirst=mpFirst->mpNext; 
				pToDel->mpNext=NULL;
				pToDel->Destroy();				
			} 
		}
		mlresult Parse(wchar_t* apwc);
		mlString toString();
		bool IsDefined(){ return mpFirst!=NULL; }
		bool IsPointIn(mlPoint &aPnt){
			for(HARegion* pReg=mpFirst; pReg; pReg=pReg->mpNext){
				if(pReg->IsPointIn(aPnt)) return true;
			}
			return false;
		}
		mlRect GetBounds(){
			mlRect rcRet;
			for(HARegion* pReg=mpFirst; pReg; pReg=pReg->mpNext){
				mlRect rc=pReg->GetBounds();
				if(pReg==mpFirst){
					rcRet=rc;
				}else{
					if(rc.left < rcRet.left) rcRet.left=rc.left;
					if(rc.top < rcRet.top) rcRet.top=rc.top;
					if(rc.right > rcRet.right) rcRet.right=rc.right;
					if(rc.bottom > rcRet.bottom) rcRet.bottom=rc.bottom;
				}
			}
			return rcRet;
		}
	} hitArea;
//<!-- hitArea value notation:
//	rectangular shape - [x, y, w, h]
//	circular shape - (x ,y ,r )
//	ellipse shape - (x ,y ,w ,h )
//  polyshape - {(x,y),(x,y), ...}
//	reference to mask shape - name of the rmml-element
//	concatenation of shapes - shape1, shape2, ...
//-->
	bool mbHitAreaIsBoundingBox;
	short modal;
	// =true, если хотя бы для одного из клавиатурных собыий 
	// зарегистрирован хотя бы один слушатель
	bool mbKeyEvListenersIsSet;

	JSFUNC_DECL(isActivated) // есть хотя бы один обработчик на onPress/onRollOver/...
	JSFUNC_DECL(captureMouse) // захватить мышь (все обработчики мыши будут вызываться только у этого объекта)
	JSFUNC_DECL(releaseMouse) // отпустить мышь

protected:
	virtual mlRMMLElement* GetElem_mlRMMLPButton()=0;
	virtual mlRMMLElement* GetParent_mlRMMLPButton()=0;
	virtual void OnPress(mlPoint apntXY){}
	virtual void OnRelease(){}
	virtual void OnAltPress(mlPoint apntXY){}
	virtual void OnAltRelease(){}

protected:
	bool mbMouseWasIn;
//	mlRect mBounds; // в координатах родителя
	void ModalChanged();
public:
//	void GenerateEvents(char aidEvent);
	//bool CanHandleEvent(char aidEvent){
	//	if(aidEvent < TEVIDE_mlRMMLPButton && aidEvent > EVID_mlRMMLPButton){
	//		if(!GetAbsEnabled()) return false;
	//	}
	//	//if(aidEvent < mlRMMLVisible::TEVIDE_mlRMMLVisible 
	//	//	&& aidEvent > mlRMMLVisible::EVID_mlRMMLVisible){
	//	//	if(!GetAbsEnabled()) return false;
	//	//	GenerateEvents(aidEvent);
	//	//}
	//	return true;
	//}
	mlRect GetBounds(){
		if(hitArea.IsDefined()){
			return hitArea.GetBounds();
		}
		mlRect rc; rc.left=rc.right=0;
		return rc;
	}
	int GetModal(){ return modal; }
	bool Button_IsPointIn(mlPoint &aPnt);
	unsigned int UseHandCursor(){ return useHandCursor; }
	void UseHandCursorChanged();
	bool CanHandleEvent(char aidEvent, bool abAllEvents, bool &bNeedToHandle);
	void HandleEvent(char aidEvent);
	void Button_ParentChanged();
	// поддержка синхронизации
	static void But_GetData4Synch(JSContext* cx, char aidEvent, mlSynchData &avSynchData);
	static void But_SetData4Synch(JSContext* cx, char aidEvent, mlSynchData &avSynchData);
//	static void But_SetData4Synch(char aidEvent, unsigned char* apSynchData, int &aiIdx);
	// есть ли обработчики клавиатурного ввода?
	bool HandlesKeyEvents(){ return mbKeyEvListenersIsSet; }
	// установлен по крайней мере один обработчик кнопок ввода
	void KeyEvListenersIsSet(){ mbKeyEvListenersIsSet = true; }

// реализация  mlIButton
public:
	unsigned int GetKeyCode();
	unsigned int GetKeyStates();
	unsigned int GetKeyScanCode();
	unsigned int GetKeyUnicode();
	bool GetAbsEnabled();
	// События
	//void onPress(mlRMMLElement* apMLEl);
	//void onRelease(mlRMMLElement* apMLEl);
	//void onReleaseOutside(mlRMMLElement* apMLEl);
	//void onRollOver(mlRMMLElement* apMLEl);
	//void onRollOut(mlRMMLElement* apMLEl);

// реализация  moIButton
public:
	moIButton* GetmoIButton();
	// Свойства
	void AbsEnabledChanged();
	// События
	void onSetFocus();
	void onKillFocus();
	void onKeyDown();
	void onKeyUp();

};

#define MLIBUTTON_IMPL \
public: \
	mlRMMLElement* GetElem_mlRMMLPButton(){ return this; } \
	mlRMMLElement* GetParent_mlRMMLPButton(){ return mpParent; } \
	mlIButton* GetIButton(){ return this; } \
	mlRMMLPButton* GetButton(){ return this; }
	//void OnPress(){ mlRMMLPButton::onPress(this); } \
	//void OnRelease(){ mlRMMLPButton::onRelease(this); } \
	//void OnReleaseOutside(){ mlRMMLPButton::onReleaseOutside(this); } \
	//void OnRollOver(){ mlRMMLPButton::onRollOver(this); } \
	//void OnRollOut(){ mlRMMLPButton::onRollOut(this); } 

}