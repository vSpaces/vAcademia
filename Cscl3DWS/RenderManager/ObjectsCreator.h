
#pragma once

#include <string>
#include "3DObject.h"
#include "UserData.h"
#include "IChangesListener.h"
#include <IAsynchResourcemanager.h>

bool CreateObject3D(std::string url, std::string _class, C3DObject** _obj, std::string name, int level = -1, void* nrmObj = NULL, IChangesListener* changesListener = NULL, bool isHighPriority = false);
void ReloadLODGroup(C3DObject* obj, std::string& newURL);
