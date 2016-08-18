
#pragma once

#include "TinyXML.h"

void SaveAttribute(TiXmlElement* node, const std::string& atrName, float atr);
void SaveAttribute(TiXmlElement* node, std::string atrName, std::string atr);