//****************************************************************************//
// animcallback.h                                                             //
// Copyright (C) 2004  Keith Fulton <keith@paqrat.com>                        //
//****************************************************************************//
// This library is free software; you can redistribute it and/or modify it    //
// under the terms of the GNU Lesser General Public License as published by   //
// the Free Software Foundation; either version 2.1 of the License, or (at    //
// your option) any later version.                                            //
//****************************************************************************//

#ifndef CAL_ANIMCALLBACK_H
#define CAL_ANIMCALLBACK_H


#include "global.h"


class CalAnimation;
class CalModel;

struct CalAnimationCallback
{
    virtual ~CalAnimationCallback() {}
    virtual void AnimationUpdate(const float animTime, CalModel* const model, void* const userData) = 0;
    virtual void AnimationComplete(CalModel* const model, void* const userData) = 0;
    // oms_v_1.0_stub
    virtual void AnimationSet(CalModel* const model, void* const userData) = 0;
    // end of oms_v_1.0_stub
};


#endif

//****************************************************************************//
