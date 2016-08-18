
#include "StdAfx.h"
#include "XMLAttributes.h"
#include "HelperFunctions.h"

void SaveAttribute(TiXmlElement* node, const std::string& atrName, float atr)
{
	std::string str1 = FloatToStr(atr);
	node->SetAttribute(atrName.c_str(), str1.c_str());
}

void SaveAttribute(TiXmlElement* node, std::string atrName, std::string atr)
{
	node->SetAttribute(atrName.c_str(), atr.c_str());
}