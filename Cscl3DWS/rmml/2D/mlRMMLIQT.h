// Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2
//
// $RCSfile: mlRMMLIQT.h,v $
// $Revision: 1.1 $
// $State: Exp $
// $Locker:  $
//
// last change: $Date: 2005/09/14 05:27:20 $
//              $Author: tandy $
//
// Copyright (C) 2004 РМЦ
//
// Этот файл является частью ОМС (Открытая мультимедиа система)
//
//////////////////////////////////////////////////////////////////////

#ifndef __rmml_IQuickTime_Included__
#define __rmml_IQuickTime_Included__

namespace rmml
{
  struct mlIQT
  {
    enum { id = 26, };

    typedef int hotspot;
    typedef int nodeid;

    virtual void set_Node(nodeid nodeID_)               = 0;
    virtual void set_hotSpot(hotspot hotSpotID_)        = 0;
    virtual void set_panAngle(double panAngle_)         = 0;
    virtual void set_tiltAngle(double tiltAngle_)       = 0;
    virtual void set_fieldOfView(double fieldOfView_)   = 0;
  };

  struct moIQT
  {
    enum { id = 26, };

    virtual long gotoNodeID(long node_id) = 0;
    virtual long getNodeID() const        = 0;

    virtual double panAngle(double pan_angle_) = 0;
    virtual double panAngle() const            = 0;

    virtual double tiltAngle(double tilt_angle_) = 0;
    virtual double tiltAngle() const             = 0;

    virtual double fieldOfView(double field_of_view_) = 0;
    virtual double fieldOfView() const                = 0;
  };
}

#endif // __rmml_IQuickTime_Included__