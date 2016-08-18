
#include "StdAfx.h"
#include "BasePostEffect.h"
#include "GlobalSingletonStorage.h"
#include "cal3d/memory_leak.h"

CBasePostEffect::CBasePostEffect():
	m_shaderID(-1),
	m_shaderParams(NULL)	
{

}

void CBasePostEffect::LoadFromXML(std::wstring fileName)
{
	USES_CONVERSION;
	TiXmlDocument doc( W2A(fileName.c_str()));
	bool isLoaded = g->rl->LoadXml( (void*)&doc, fileName);
	if (!isLoaded)
	{
		g->lw.WriteLn("[ERROR] Loading post-effect ", fileName, " failed...");
		return;
	}

	TiXmlNode* object = doc.FirstChild("base_shader");

	if ((doc.NoChildren()) || (!object))
	{
		g->lw.WriteLn("[ERROR] Post-effect ", fileName, "has no base shader");
		return;
	}

	std::string shaderName;

	if (object->ToElement()->GetText())
	{
		shaderName = object->ToElement()->GetText();
	}
	else
	{
		g->lw.WriteLn("[ERROR] Post-effect ", fileName," has invalid shader");
		return;
	}

	m_shaderID = g->sm.GetObjectNumber(shaderName);
	if (m_shaderID == -1)
	{
		g->lw.WriteLn("[ERROR] Shader for post-effect ", fileName, " is not found!");
		return;
	}

	if (m_shaderParams)
	{
		MP_DELETE(m_shaderParams);
	}

	m_shaderParams = MP_NEW(CMaterial);
	m_shaderParams->LoadFromXML(fileName, false);

	OnLoadFromXML(fileName);
}

CBasePostEffect::~CBasePostEffect()
{
	if (m_shaderParams)
	{
		MP_DELETE(m_shaderParams);
	}
}