// Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2
//
// $RCSfile: mlRMMLQT.h,v $
// $Revision: 1.4 $
// $State: Exp $
// $Locker:  $
//
// last change: $Date: 2009/07/16 13:32:57 $
//              $Author: linda $
//
// Copyright (C) 2004 РМЦ
//
// Этот файл является частью ОМС (Открытая мультимедиа система)
//
//////////////////////////////////////////////////////////////////////

#ifndef __rmml_QuickTime_Included__
#define __rmml_QuickTime_Included__

#include "mlRMMLIQT.h"
#include <string>

namespace rmml
{
  struct moIQT;

#define EVNM_enterHotSpot    L"enterHotSpot"
#define EVNM_leaveHotSpot    L"leaveHotSpot"
#define EVNM_triggerHotSpot  L"triggerHotSpot"

#define EVNM_enterNode       L"enterNode"
#define EVNM_leaveNode       L"leaveNode"

  /** Класс для поддержки Flash */
  class mlRMMLQT : 
                    public mlRMMLElement,
                    public mlJSClass,
                    public mlRMMLLoadable,
                    public mlRMMLVisible,
                    public mlRMMLContinuous,
                    public mlIQT
  {
  public:
    mlRMMLQT();
	void destroy() { MP_DELETE_THIS }
    virtual ~mlRMMLQT();

    mlRMMLElement* GetElem_mlRMMLQT();

  // реализация mlJSClass
    MLJSCLASS_DECL

  // реализация mlMedia
    virtual void* GetInterface(long iid);

  // реализация mlRMMLElement
    MLRMMLELEMENT_IMPL
	virtual EventSpec* GetEventSpec2(){ return _EventSpec; } 
    virtual mlresult CreateMedia(omsContext* amcx);
    virtual mlresult Load();
    virtual mlRMMLElement* Duplicate(mlRMMLElement* apNewParent);

  // реализация mlILoadable
    MLILOADABLE_IMPL

  // реализация mlIVisible
    MLIVISIBLE_IMPL

  // реализация  mlRMMLContinuous
    MLICONTINUOUS_IMPL

  protected:
    enum {
      JSPROP_nodeID,
      JSPROP_hotSpotID,
      JSPROP_nodeType,
      JSPROP_panAngle,
      JSPROP_tiltAngle,
      JSPROP_fieldOfView,

      EVID_enterHotSpot,
      EVID_leaveHotSpot,
      EVID_triggerHotSpot,

      EVID_enterNode,
      EVID_leaveNode,
	  EVID_updateTime
    };

  public:
    std::wstring getHotType(hotspot hotSpotID = -1);
//    void triggerHotSport(hotsport hotSportID);
  protected:
    bool change_Node(nodeid nodeID_);
    void change_panAngle(double panAngle_);
    void change_tiltAngle(double tiltAngle_);
    void change_fieldOfView(double fieldOfView_);

// вызов соответствующих функция скриптовой части
    virtual void enterHotSport(hotspot hotSportID_);
    virtual void leaveHotSport(hotspot hotSportID_);
    virtual bool triggerHotSport(hotspot hotSportID_);

    virtual void enterNode(nodeid nodeID_);
    virtual void leaveNode(nodeid nodeID_);


// функции которые вызывается из mme, что-бы установить в скриптовой части
// правильные значения свойств, наследуются от интерфейса mlIQT
    virtual void set_Node(nodeid nodeID_);
    virtual void set_hotSpot(hotspot hotSpotID_);
    virtual void set_panAngle(double panAngle_);
    virtual void set_tiltAngle(double tiltAngle_);
    virtual void set_fieldOfView(double fieldOfView_);

  protected:
    moIQT *getQuickTime();

  protected:
    nodeid
      nodeID;       // идентификатор текущего узла
    MP_WSTRING
      nodeType;     // тип текущего узла
    hotspot
      hotSpotID;    // идентификатор, активной зоны
                    // над которой сейчас находиться курсор
    jsdouble
      panAngle,     // поворот по горизонтали в градусах
      tiltAngle,    // поворот по вертикале в градусах
      fieldOfView;  // приближение в "разах"
  };
}

#endif // __rmml_QuickTime_Included__