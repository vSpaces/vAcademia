// Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2
//
// $RCSfile: mlRMMLQT.cpp,v $
// $Revision: 1.6 $
// $State: Exp $
// $Locker:  $
//
// last change: $Date: 2010/04/09 07:17:32 $
//              $Author: tandy $ 
//
//////////////////////////////////////////////////////////////////////
#include "mlRMML.h"
#include "config/oms_config.h"
#include "2D/mlRMMLQT.h"
#include "config/prolog.h"

#if _MSC_VER <= 1200
double __cdecl _wtof(
        const wchar_t *ptr
        )
{
	char *cptr;
	int malloc_flag = 0;
	size_t len;
	double retval;
	while (iswspace(*ptr))
		ptr++;

	len = wcstombs(NULL, ptr, 0);
	if ((cptr = MP_NEW_ARR( char, len+1)) == NULL)
	{
		errno = ENOMEM;
		return 0.0;
	}
	// UNDONE: check for errors
	// Add one to len so as to null terminate cptr.
	wcstombs(cptr, ptr, len+1);
	retval = atof(cptr);
	MP_DELETE_ARR(cptr);

	return retval;
}
#endif

namespace rmml
{
  mlRMMLQT::mlRMMLQT()
    : nodeID(-1), hotSpotID(0),
      panAngle(0.0), tiltAngle(0.0), fieldOfView(0.0),
	  MP_WSTRING_INIT(nodeType)
  {
    mType     = MLMT_QT;
    mRMMLType = MLMT_QT;
  }

  mlRMMLQT::~mlRMMLQT()
  {
    omsContext* pContext = GPSM(mcx)->GetContext();
    ML_ASSERTION(mcx, pContext->mpRM!=NULL, "mlRMMLQT::~mlRMMLQT");
	if(!PMO_IS_NULL(mpMO))
	    pContext->mpRM->DestroyMO(this);
  }

  mlRMMLElement*
  mlRMMLQT::GetElem_mlRMMLQT()
  {
    return this;
  }

  void*
  mlRMMLQT::GetInterface(long iid)
  {
    if (iid == mlIQT::id)
      return static_cast<mlIQT*>(this);
    
    return 0;
  }

// реализация mlJSClass
//// JavaScript Variable Table
  JSPropertySpec mlRMMLQT::_JSPropertySpec[] = {
    JSPROP_RW(nodeID)
    JSPROP_RO(hotSpotID)
    JSPROP_RO(nodeType)
    JSPROP_RW(panAngle)
    JSPROP_RW(tiltAngle)
    JSPROP_RW(fieldOfView)
    { 0, 0, 0, 0, 0 }
  };

  /// JavaScript Function Table
  JSFunctionSpec mlRMMLQT::_JSFunctionSpec[] = {
    { 0, 0, 0, 0, 0 }
  };

  EventSpec mlRMMLQT::_EventSpec[] = {
    MLEVENT(enterHotSpot)
    MLEVENT(leaveHotSpot)
    MLEVENT(triggerHotSpot)

    MLEVENT(enterNode)
    MLEVENT(leaveNode)

    {0, 0, 0}
  };

//  ADDPROPFUNCSPECS_IMPL(mlRMMLQT)

  MLJSCLASS_IMPL_BEGIN(QT, mlRMMLQT, 1)
    MLJSCLASS_ADDPROTO(mlRMMLElement)
    MLJSCLASS_ADDPROTO(mlRMMLLoadable)
    MLJSCLASS_ADDPROTO(mlRMMLVisible)
    MLJSCLASS_ADDPROTO(mlRMMLContinuous)
//    MLJSCLASS_ADDPROTO(mlRMMLQT)
    MLJSCLASS_ADDPROPFUNC
    MLJSCLASS_ADDEVENTS
  MLJSCLASS_IMPL_END(mlRMMLQT)

/// JavaScript Set Property Wrapper
  JSBool
  mlRMMLQT::JSSetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp)
  {
    SET_PROPS_BEGIN(mlRMMLQT);
    SET_PROTO_PROP(mlRMMLElement);
    SET_PROTO_PROP(mlRMMLVisible);
    SET_PROTO_PROP(mlRMMLContinuous);
    default:
      switch(iID)
      {
        case JSPROP_nodeID: {
          nodeid new_node = priv->nodeID;
          ML_JSVAL_TO_INT(new_node, *vp);
            if (new_node != priv->nodeID)
              priv->change_Node(new_node);
        } break;

        case JSPROP_panAngle: {
            double new_panAngle = priv->panAngle;
            ML_JSVAL_TO_DOUBLE(new_panAngle, *vp);
            if (new_panAngle != priv->panAngle)
              priv->change_panAngle(new_panAngle);
        } break;

        case JSPROP_tiltAngle: {
            double new_tiltAngle = priv->tiltAngle;
            ML_JSVAL_TO_DOUBLE(new_tiltAngle, *vp);
            if (new_tiltAngle != priv->tiltAngle)
              priv->change_tiltAngle(new_tiltAngle);
        } break;

        case JSPROP_fieldOfView: {
            double new_fieldOfView = priv->fieldOfView;
            ML_JSVAL_TO_DOUBLE(new_fieldOfView, *vp);
            if (new_fieldOfView != priv->fieldOfView)
              priv->change_fieldOfView(new_fieldOfView);
        } break;
      }
    SET_PROPS_END;

    return JS_TRUE;
  }

  /// JavaScript Get Property Wrapper
  JSBool
  mlRMMLQT::JSGetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp)
  {
    GET_PROPS_BEGIN(mlRMMLQT);
    GET_PROTO_PROP(mlRMMLElement);
    GET_PROTO_PROP(mlRMMLVisible);
    GET_PROTO_PROP(mlRMMLContinuous);
    default:
      switch(iID)
      {
        case JSPROP_nodeID:
          *vp = INT_TO_JSVAL(priv->nodeID);
        break;

        case JSPROP_hotSpotID:
          *vp = INT_TO_JSVAL(priv->hotSpotID);
        break;

        case JSPROP_nodeType:
          *vp = STRING_TO_JSVAL(wr_JS_NewUCStringCopyZ(cx, priv->nodeType.c_str()));
          break;

        case JSPROP_panAngle:
          *vp = DOUBLE_TO_JSVAL(&priv->panAngle);
          break;

        case JSPROP_tiltAngle:
          *vp = DOUBLE_TO_JSVAL(&priv->tiltAngle);
          break;

        case JSPROP_fieldOfView:
          *vp = DOUBLE_TO_JSVAL(&priv->fieldOfView);
          break;
      }
    GET_PROPS_END;

    return JS_TRUE;
  }

// реализация mlRMMLElement
  mlresult
  mlRMMLQT::CreateMedia(omsContext* amcx)
  {
    ML_ASSERTION(mcx, amcx->mpRM!=NULL,"mlRMMLQT::CreateMedia");

    amcx->mpRM->CreateMO(this);

    if (PMO_IS_NULL(mpMO))
      return ML_ERROR_NOT_INITIALIZED;

    mpMO->SetMLMedia(this);

    return ML_OK;
  }

  mlresult
  mlRMMLQT::Load()
  {
    if (PMO_IS_NULL(mpMO))
      return ML_ERROR_NOT_INITIALIZED;

    int iOldType = mType;
    mType = MLMT_QT;
    if (!(mpMO->GetILoadable()->SrcChanged()))
      mType = iOldType;

    return ML_OK;
  }

  mlRMMLElement*
  mlRMMLQT::Duplicate(mlRMMLElement* apNewParent)
  {
    mlRMMLQT* pNewEL =
      (mlRMMLQT*)GET_RMMLEL(mcx, mlRMMLImage::newJSObject(mcx));
    pNewEL->SetParent(apNewParent);
    pNewEL->GetPropsAndChildrenCopyFrom(this);

    return pNewEL;
  }

  std::wstring
  mlRMMLQT::getHotType(hotspot hotSpotID /*= -1*/)
  {
    return std::wstring();
  }

  bool
  mlRMMLQT::change_Node(nodeid nodeID_)
  {
    moIQT *qt_ptr = getQuickTime();
    if (!qt_ptr)
      return false;

    set_Node(qt_ptr->gotoNodeID(nodeID_));
	return true;
  }

  void
  mlRMMLQT::change_panAngle(double panAngle_)
  {
    moIQT *qt_ptr = getQuickTime();
    if (!qt_ptr)
      return;

    panAngle = qt_ptr->panAngle(panAngle_);
  }

  void
  mlRMMLQT::change_tiltAngle(double tiltAngle_)
  {
    moIQT *qt_ptr = getQuickTime();
    if (!qt_ptr)
      return;

    tiltAngle = qt_ptr->tiltAngle(tiltAngle_);
  }

  void
  mlRMMLQT::change_fieldOfView(double fieldOfView_)
  {
    moIQT *qt_ptr = getQuickTime();
    if (!qt_ptr)
      return;

    fieldOfView = qt_ptr->fieldOfView(fieldOfView_);
  }

  void
  mlRMMLQT::enterHotSport(hotspot hotSportID_)
  {
    GetElem_mlRMMLQT()->CallListeners(EVID_enterHotSpot);
  }

  void
  mlRMMLQT::leaveHotSport(hotspot hotSportID_)
  {
    GetElem_mlRMMLQT()->CallListeners(EVID_leaveHotSpot);
  }

  bool
  mlRMMLQT::triggerHotSport(hotspot hotSportID_)
  {
    GetElem_mlRMMLQT()->CallListeners(EVID_triggerHotSpot);

    return true;
  }

  void
  mlRMMLQT::enterNode(nodeid nodeID_)
  {
    GetElem_mlRMMLQT()->CallListeners(EVID_enterNode);
  }

  void
  mlRMMLQT::leaveNode(nodeid nodeID_)
  {
    GetElem_mlRMMLQT()->CallListeners(EVID_leaveNode);
  }

  void
  mlRMMLQT::set_Node(nodeid nodeID_)
  {
    if (nodeID == nodeID_)
      return;

    if (nodeID != -1)
      leaveNode(nodeID_);

    nodeID = nodeID_;

    if (nodeID != -1)
      enterNode(nodeID_);
  }

  void
  mlRMMLQT::set_hotSpot(hotspot hotSpotID_)
  {
    if (hotSpotID == hotSpotID_)
      return;

    if (hotSpotID != 0)
      leaveHotSport(hotSpotID);

    hotSpotID = hotSpotID_;

    if (hotSpotID != -1)
      enterHotSport(hotSpotID);
  }

  void
  mlRMMLQT::set_panAngle(double panAngle_)
  {
    panAngle = panAngle_;
  }

  void
  mlRMMLQT::set_tiltAngle(double tiltAngle_)
  {
    tiltAngle = tiltAngle_;
  }
   
  void
  mlRMMLQT::set_fieldOfView(double fieldOfView_)
  {
    fieldOfView = fieldOfView_;
  }

  moIQT *
  mlRMMLQT::getQuickTime()
  {
    return reinterpret_cast<moIQT*>(mpMO->GetInterface(moIQT::id));
  }
}