
#include "StdAfx.h"
#include "MotionRecognizer.h"
#include "XMLList.h"
#include "PlatformDependent.h"

_SAvatarID::_SAvatarID():
	MP_STRING_INIT(pathPart),
	MP_STRING_INIT(motionFrom)
{
	id = 0;
	pathPart = "";
	motionFrom = "";
}

_SAvatarID::_SAvatarID(const _SAvatarID& other):
	MP_STRING_INIT(pathPart),
	MP_STRING_INIT(motionFrom)
{	
	id = other.id;
	pathPart = other.pathPart;
	motionFrom = other.motionFrom;
}

CMotionRecognizer::CMotionRecognizer():
		MP_MAP_INIT(m_avatarsList)
{
	Init();
}

void CMotionRecognizer::Init()
{
	std::wstring fileName = GetApplicationRootDirectory();
	fileName += L"avatars_ids.xml";

	std::vector<std::string> fields;
	fields.push_back("id");
	fields.push_back("motionfrom");

	CXMLList avatars(fileName, fields, true);

	std::string value, idValue;
	while (avatars.GetNextValue(value))
	{
		if (avatars.GetOptionalFieldValue("id", idValue))
		{
			SAvatarID* avatarID = MP_NEW(SAvatarID);
			avatarID->pathPart = value;
			avatarID->id = rtl_atoi(idValue.c_str());

			std::string motionFrom;
			if (avatars.GetOptionalFieldValue("motionFrom", motionFrom))
			{
				avatarID->motionFrom = motionFrom;
			}

			m_avatarsList.push_back(avatarID);
		}
	}
}

CMotionRecognizer* CMotionRecognizer::GetInstance()
{
	static CMotionRecognizer* obj = NULL;

	if (!obj)
	{
		obj = new CMotionRecognizer();
	}

	return obj;
}

void CMotionRecognizer::CorrectMotionPathIfNeeded(std::string& path)
{
	MP_VECTOR<SAvatarID *>::const_iterator it = m_avatarsList.begin();
	MP_VECTOR<SAvatarID *>::const_iterator itEnd = m_avatarsList.end();
	while (it != itEnd)
	{
		if ((*it)->motionFrom.size() > 0)
		{		
			StringReplace(path, "\\" + (*it)->pathPart + "\\", "\\" + (*it)->motionFrom + "\\");
			StringReplace(path, "/" + (*it)->pathPart + "/", "/" + (*it)->motionFrom + "/");		
		}
		it++;
	}
}

int CMotionRecognizer::GetIDByPath(const std::string& path)const
{
	MP_VECTOR<SAvatarID *>::const_iterator it = m_avatarsList.begin();
	MP_VECTOR<SAvatarID *>::const_iterator itEnd = m_avatarsList.end();
	while (it != itEnd)
	{
		if (path.find((*it)->pathPart) != -1)
		{
			return (*it)->id;
		}
		it++;
	}

	return 0;
}

CMotionRecognizer::~CMotionRecognizer()
{
}

