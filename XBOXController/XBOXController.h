#pragma once

#ifdef XBOXCONTROLLER_EXPORTS
#define XBOXCONTROLLER_API __declspec(dllexport)
#else
#define XBOXCONTROLLER_API __declspec(dllimport)
#endif

#include "IXBOXController.h"

class CXBOXController : public IXBOXController
{
private:
    XINPUT_STATE _controllerState;
    int _controllerNum;
public:
    CXBOXController(int playerNumber);
    XINPUT_STATE GetState() override;
    bool IsConnected() override;
    void Vibrate(int leftVal = 0, int rightVal = 0) override;
};


extern "C" XBOXCONTROLLER_API IXBOXController* NewController();