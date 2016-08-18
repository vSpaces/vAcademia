
#ifndef _OMS_MI_H_
#define _OMS_MI_H_

#include "std_define.h"
#include <vector>

#ifndef NULL
#define NULL 0
#endif

namespace oms{

struct omsUserEvent{
	enum{
		KEYDOWN		=0x0100,
		KEYUP		=0x0101,
		GAMEPAD_KEYDOWN	=0x0102,
		GAMEPAD_KEYUP =0x0103,
		MOUSEMOVE	=0x0200,
		LBUTTONDOWN	=0x0201,
		LBUTTONUP	=0x0202,
		LBUTTONDBLCLK =0x0203,
		RBUTTONDOWN	=0x0204,
		RBUTTONUP	=0x0205,
		RBUTTONDBLCLK =0x0206,
		MBUTTONDOWN =0x0207,
		MBUTTONUP   =0x0208,
		MBUTTONDBLCLK =0x0209,
		MOUSEWHEEL	=0x020A,
		INTERNAL	=0x3D00,
		GETDEBUGINFO	=0x3E00,
		SYNCH		=0x3FF9,
		CUSTOM		=0x3FFA,
		RELOADSCENE	=0x3FFB,
		SETSTATE	=0x3FFC,
		GETSTATE	=0x3FFD,
		SETMODE		=0x3FFE,
		EXECSCRIPT	=0x3FFF
	};
	enum{
		KEYCONTROL	= 0x8,
		KEYSHIFT	= 0x4,
	};
	unsigned short ev;
	//union{
		struct {
			unsigned short mouse_x,mouse_y;
		}mousexy;
		unsigned short* script;
	//};
	unsigned int check_sum;
	unsigned int key_code;
	//
	omsUserEvent* pNext;
	//
	typedef unsigned char UpdateID;
	UpdateID update_id; // идентификатор update-а sceneManager-а, на котором было создано событие

	omsUserEvent(){
		ev = 0;
		script = NULL;
		pNext = NULL;
		check_sum = 0;
		update_id = 0;
	}
	// Win32 constructor
	omsUserEvent(unsigned int auMsg, unsigned int awParam, long alParam){
		ev = (unsigned short)auMsg;
		mousexy.mouse_x = 0;
		mousexy.mouse_y = 0;
		if(ev >= MOUSEMOVE && ev <= MOUSEWHEEL){
			mousexy.mouse_x=(unsigned short)(alParam & 0xffff);
			mousexy.mouse_y=(unsigned short)(alParam >> 16);
		}else if(ev == KEYDOWN || ev == KEYUP){
			mousexy.mouse_x = (unsigned short)(alParam & 0xffff);  
			mousexy.mouse_y = (unsigned short)(alParam >> 16); 
		}
		script = NULL;
		key_code=awParam;
		pNext=NULL;
		check_sum = 0;
		update_id = 0;
	}
	// ExecScript constructor
	omsUserEvent(unsigned short* apwcScript, int aiLength=-1){
		ev=EXECSCRIPT;
		script=apwcScript;
		if(aiLength<0){
			while(*apwcScript++);
			key_code=(unsigned short)(apwcScript - script - 1);
		}else 
			key_code=(unsigned short)aiLength;
		pNext=NULL;
		update_id = 0;
	}
	// CallEvent constructor
	omsUserEvent::omsUserEvent(unsigned short auEvent, unsigned short auDataID){
		ev=0x8000 | auEvent;
		key_code=auDataID;
		pNext=NULL;
		update_id = 0;
	}
	// SetMode constructor
	omsUserEvent::omsUserEvent(unsigned char aMode){
		ev=SETMODE;
		script = NULL;
		key_code=aMode;
		pNext=NULL;
		update_id = 0;
	}
};

#define MI_WITH_ALL_MOUSEMOVE_STORE 1

struct omsIInput
{
	virtual void AddEvent(omsUserEvent &ev, bool abIntoBegin=false)=0;
	virtual void AddCustomEvent(unsigned short auiID, unsigned char* apData, unsigned int aiDataLength)=0;
	virtual void AddExecScript(const unsigned short* apwcScript, int aiLength)=0;
	virtual void AddCallEvent(unsigned short auEventID, unsigned short auObjectID, 
								unsigned char* apData, unsigned short auDataLength, 
								unsigned long aulGlobalTime)=0;
	virtual void AddGetStateEvent()=0;
	virtual void AddSetStateEvent(unsigned char* apData, unsigned long alSize)=0;
	virtual void AddInternalEvent(unsigned char achEventCode, bool abProp, 
									unsigned char* apData, unsigned short auDataLength, bool abIntoBegin=false)=0;
	virtual omsUserEvent GetEvent()=0;
	virtual omsUserEvent GetNextCallEvent()=0;
//	virtual unsigned char* GetCallEventData(omsUserEvent &ev, unsigned short &auLength)=0;
	virtual void FreeEventData(omsUserEvent &ev)=0;
	virtual void AddGetDebugInfoEvent(unsigned char acInfoType,
                                    void* hwndDebugger,
                                    unsigned char* apData,
                                    unsigned int auDataLength) = 0;
	virtual void SetStopper()=0;
	virtual void AddSyncData( unsigned char* aData, unsigned int aDataSize, unsigned int aCheckSum)=0;
	virtual void RemoveAllSynchEvents()=0;
	// установить режим пропускания событий MouseMove
	virtual void SetSkipMouseMovesMode(bool abMode)=0;
	virtual bool GetSkipMouseMovesMode()=0;
	virtual unsigned char IncrementUpdateID()=0;
};

/**
 * Базовый класс очереди сообщений от пользователя.
 * Используется менеджером сцен
 */
template<class Locker,typename Mut> 
class omsInput: public omsIInput{
	omsUserEvent* mpFirst;
	omsUserEvent** mppLastNext;
	omsUserEvent** mpStopper;
	bool mbSkipMouseMovesMode;
	omsUserEvent::UpdateID mUpdateID; // идентификатор текущего update-а sceneManager-а
protected:
	Mut mMutex;
public:
	virtual void* AllocMem(unsigned int)=0;
	virtual void FreeMem(void* apBlock){}
	omsInput(){
		mpFirst = NULL;
		mppLastNext = NULL;
		mpStopper = NULL;
		mbSkipMouseMovesMode = true;
		mUpdateID = 0;
	}
	~omsInput(){
		while(mpFirst){
			omsUserEvent* pEvent = mpFirst;
			mpFirst = pEvent->pNext;
			MP_DELETE( pEvent);
		}		
	}
// реализация omsIInput
	void SetStopper(){
		if(mpStopper == NULL)
			mpStopper = mppLastNext;
	}
	// копирует буфер координат из старой записи события MouseMove в новую,
	// добавляя в него координаты старого события
	void CopyMouseMoveCoordBuffer(omsUserEvent &newEv, omsUserEvent &oldEv){
		// (координаты события в буфере не храним. Храним только координаты пропущенных событий)
		// если в *pEvent уже был буфер координат, то надо его перенести в ev и добавить в него координаты из ev
		// (старшее слово key_code - длина буфера (в байтах), младшее - количество записанных в нем пар координат)
		// 1) определить, сколько пар координат надо будет хранить в буфере
		int iNeedToStore = 1; // нужно сохранить как минимум координаты пропускаемого события
		int iOldBufSize = 0;
		// 1.1) если был буфер в старом событии, то прибавить количество содержащихся в нем координат
		if(oldEv.script != NULL){
			iNeedToStore += ((oldEv.key_code) & 0xFFFF);
			if(iNeedToStore > 300){ // если уже слишком много пропущено, то не будем больше сохранять
				newEv.script = oldEv.script;
				newEv.key_code = oldEv.key_code;
				return;
			}
			iOldBufSize = (((oldEv.key_code) >> 16) & 0xFFFF);
		}
		// 2) если надо перевыделять память под буфер, то сделать это, скопировать в него содержимое старого буфера и освободить старый буфер
		#define SIZE_OF_COORD_PAIR ((sizeof(short)*2))
		if(iNeedToStore > iOldBufSize/((int)SIZE_OF_COORD_PAIR)){ // надо перевыделять, если имеющийся буфер мал
			int iNewBufferSize = (iNeedToStore * 2) * SIZE_OF_COORD_PAIR;
			newEv.script = (unsigned short *)MP_NEW_ARR(unsigned char, iNewBufferSize);
			if(oldEv.script != NULL){
				memcpy(newEv.script, oldEv.script, iOldBufSize);
				MP_DELETE_ARR(oldEv.script);
			}
			// сохраняем размер буфера
			newEv.key_code = (newEv.key_code & 0xFFFF) | (iNewBufferSize << 16);
		}else{
			// если перевыделять не надо, то просто копируем
			newEv.script = oldEv.script;
			newEv.key_code = oldEv.key_code;
		}
		oldEv.script = NULL;
		// сохраняем количество пар
		newEv.key_code = (newEv.key_code & 0xFFFF0000) | iNeedToStore;
		// 3) сохранить в буфере координаты старого события (oldEv)
		int iLastPosIdx = (iNeedToStore - 1) * 2;
		newEv.script[iLastPosIdx] = oldEv.mousexy.mouse_x;
		newEv.script[iLastPosIdx + 1] = oldEv.mousexy.mouse_y;
	}

	void AddEvent(omsUserEvent &ev, bool abIntoBegin=false){
		Locker lock_it(mMutex);
		if(!abIntoBegin){
			ev.update_id = mUpdateID;
		}
		omsUserEvent* pEvent=NULL;
		if(ev.ev==omsUserEvent::MOUSEMOVE && mbSkipMouseMovesMode){
			// все MouseMove-ы в любом случае сохраняем, 
			// чтобы потом выдать координаты перемещения мыши вместе с последним событием
			//StoreMouseMove(ev);
			pEvent=mpFirst;
			while(pEvent){
				if(pEvent->ev==omsUserEvent::MOUSEMOVE){
					break;
				}
				pEvent=pEvent->pNext;
			}
		}else if(abIntoBegin){
			// ставим в начало очереди
			if(mpFirst!=NULL){
				MP_NEW_P( pEvNew, omsUserEvent, ev);
				pEvNew->pNext=mpFirst;
				mpFirst=pEvNew;
				return;
			}
		}
		if(pEvent){
			if(ev.ev == omsUserEvent::MOUSEMOVE && pEvent->ev == omsUserEvent::MOUSEMOVE){
				CopyMouseMoveCoordBuffer(ev, *pEvent);
			}
			ev.pNext=pEvent->pNext;
			*pEvent=ev;
		}else{
			MP_NEW_P( pEvNew, omsUserEvent, ev);
			pEvNew->pNext = NULL;
			if(mpFirst==NULL){
				mpFirst=pEvNew;
			}else{
				*mppLastNext=pEvNew;
			}
			mppLastNext=&(pEvNew->pNext);
		}
	}
	void AddCustomEvent(unsigned short auCustomEventID, unsigned char* apData, unsigned int auDataLength){
		omsUserEvent ev;
		unsigned char* pData = MP_NEW_ARR(unsigned char, (auDataLength + sizeof(unsigned short) + 1) );
		ev.script = (unsigned short*)pData;
		*((unsigned short*)pData) = auCustomEventID; pData += sizeof(unsigned short);
		ev.key_code = (unsigned int)(auDataLength + sizeof(unsigned short));
		for(unsigned int ii = 0; ii < auDataLength; ii++)
			*pData++ = *apData++;
		ev.ev = omsUserEvent::CUSTOM;
		AddEvent(ev);
	}
	void AddExecScript(const unsigned short* apwcScript, int aiLength){
		if(aiLength<0){
			const unsigned short* pwc=apwcScript;
			while(*pwc++);
			aiLength=(int)(pwc - apwcScript - 1);
		}
		unsigned short* pwcScript=(unsigned short*)MP_NEW_ARR(unsigned char, ((aiLength+1)*sizeof(unsigned short)));
		unsigned short* pwc=pwcScript;
		for(int ii=0; ii<aiLength; ii++)
			*pwc++=*apwcScript++;
		*pwc=L'\0';
		AddEvent(omsUserEvent(pwcScript,aiLength));
	}
	void AddCallEvent(unsigned short auEventID, unsigned short auObjectID, 
						unsigned char* apData, unsigned short auDataLength, unsigned long aulGlobalTime){
		omsUserEvent ev;
		ev.ev=0x8000 | auObjectID;
		unsigned char* pData=MP_NEW_ARR(unsigned char, (auDataLength + sizeof(unsigned short) + sizeof(unsigned long)));
		ev.script=(unsigned short*)pData;
		*((unsigned short*)pData) = auEventID; pData += sizeof(unsigned short);
		*((unsigned long*)pData) = aulGlobalTime; pData += sizeof(unsigned long);
		ev.key_code=(unsigned int)auDataLength;
		for(int ii=0; ii<auDataLength; ii++)
			*pData++=*apData++;
		AddEvent(ev);
	}

	void AddGetDebugInfoEvent(unsigned char acInfoType, void* hwndDebugger, 
							  unsigned char* apData, unsigned int auDataLength){
		omsUserEvent ev;
		ev.ev=omsUserEvent::GETDEBUGINFO+acInfoType;
		unsigned char* pData=MP_NEW_ARR(unsigned char, (auDataLength+sizeof(void*)));
		ev.script=(unsigned short*)pData;
		*((void**)pData)=hwndDebugger; pData+=sizeof(void*);
		ev.key_code=(unsigned int)auDataLength;
		for(unsigned int ii=0; ii<auDataLength; ii++)
			*pData++=*apData++;
		AddEvent(ev);
	}
	void AddGetStateEvent(){
		omsUserEvent ev;
		ev.ev=omsUserEvent::GETSTATE;
		AddEvent(ev,true);
	}
	void AddSetStateEvent(unsigned char* apData, unsigned long alDataLength){
		omsUserEvent ev;
		ev.ev=omsUserEvent::SETSTATE;
		unsigned char* pData=MP_NEW_ARR(unsigned char, alDataLength+1);
		ev.script=(unsigned short*)pData;
		ev.key_code=alDataLength;
		for(unsigned int ii=0; ii<alDataLength; ii++)
			*pData++=*apData++;
		AddEvent(ev);
	}
	void AddInternalEvent(unsigned char achEventCode, bool abProp, 
							unsigned char* apData, unsigned short auDataLength, bool abIntoBegin=false){
		omsUserEvent ev;
		ev.ev=omsUserEvent::INTERNAL | achEventCode;
		if(abProp) ev.ev|=0x80;
		ev.key_code=auDataLength;
		unsigned char* pData=MP_NEW_ARR(unsigned char, auDataLength+1);
		ev.script=(unsigned short*)pData;
		ev.key_code=auDataLength;
		for(int ii=0; ii<auDataLength; ii++)
			*pData++=*apData++;
		AddEvent(ev, abIntoBegin);
	}
	//
	omsUserEvent GetEvent(){
		Locker lock_it(mMutex);
		omsUserEvent Event;
		if(mpFirst!=NULL){
			omsUserEvent* pEvent = mpFirst;
			if (mpStopper != NULL && (&(pEvent->pNext) == mpStopper)){
				mpStopper = NULL;
				return Event;
			}
			Event = *pEvent;
			mpFirst = mpFirst->pNext;
			MP_DELETE( pEvent);
		}else{
			mpStopper = NULL;
		}
		return Event;
	}
	omsUserEvent GetNextCallEvent(){
		Locker lock_it(mMutex);
		omsUserEvent Event;
		omsUserEvent* pEvent = mpFirst;
		while(pEvent != NULL){
			if(pEvent->ev & 0x8000)
				return *pEvent;
			pEvent = pEvent->pNext;
		}
		return Event;
	}

	void FreeEventData(omsUserEvent &ev)
	{
		MP_DELETE_ARR(ev.script);
		return;
	}
	void AddSyncData( unsigned char* apData, unsigned int aDataSize, unsigned int aCheckSum)
	{
		omsUserEvent ev;
		unsigned char* pData = MP_NEW_ARR(unsigned char, aDataSize + 1);
		ev.script = (unsigned short*)pData;
		ev.key_code = (unsigned int)aDataSize;
		for(unsigned int ii = 0; ii < aDataSize; ii++)
			*pData++ = *apData++;
		ev.ev = omsUserEvent::SYNCH;
		ev.check_sum = aCheckSum;
		AddEvent(ev);
	}

	void AddObjectNotFoundData( unsigned int aObjectID, unsigned int aBornReality, unsigned int aRealityID)
	{
		omsUserEvent ev;
		unsigned char* pData = MP_NEW_ARR(unsigned char, aDataSize + 1);
		ev.script = (unsigned short*)pData;
		ev.key_code = (unsigned int)aDataSize;
		for(int ii = 0; ii < aDataSize; ii++)
			*pData++ = *apData++;
		ev.ev = omsUserEvent::SYNCH;
		AddEvent(ev);
	}

	void RemoveAllSynchEvents(){
		omsUserEvent* pEvent = mpFirst;
		omsUserEvent* *pp2Cur = &mpFirst;
		while(pEvent != NULL){
			if(pEvent->ev == omsUserEvent::SYNCH){
				if(mppLastNext == &(pEvent->pNext))
					mppLastNext = pp2Cur;
				*pp2Cur = pEvent->pNext;
				FreeEventData(*pEvent);
				MP_DELETE( pEvent);
			}else{
				pp2Cur = &(pEvent->pNext);
			}
			pEvent = *pp2Cur;
		}
		mpStopper = NULL;
	}
	void SetSkipMouseMovesMode(bool abMode){
		mbSkipMouseMovesMode = abMode;
	}
	bool GetSkipMouseMovesMode(){
		return mbSkipMouseMovesMode;
	}

	unsigned char IncrementUpdateID(){
		return ++mUpdateID;
	}
};

}

#endif
