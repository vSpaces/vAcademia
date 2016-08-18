
#include "StdAfx.h"
#include <TinyXML.h>
#include "CameraTrackController.h"
#include "GlobalSingletonStorage.h"

#define TIME_SHIFT	0

CCameraTrackController::CCameraTrackController():
	m_startTime(0),
	MP_VECTOR_INIT(m_frames),
	m_isPlaying(false)
{
}

bool CCameraTrackController::LoadTrack( void* apData, int auDataSize)
{
	TiXmlDocument doc;

	bool isLoaded = doc.LoadFile(apData, auDataSize);

	if (!isLoaded)
	{
		g->lw.WriteLn("[ERROR] Track loading failed");
		return false;
	}

	return LoadTrack( doc);
}

bool CCameraTrackController::LoadTrack(std::wstring fileName)
{
	USES_CONVERSION;
	TiXmlDocument doc( W2A(fileName.c_str()));
	bool isLoaded = g->rl->LoadXml( (void*)&doc, fileName);

	if (!isLoaded)
	{
		g->lw.WriteLn("[ERROR] Loading failed: ", fileName);
		return false;
	}

	return LoadTrack( doc);
}

bool CCameraTrackController::LoadTrack(TiXmlDocument& doc)
{
	TiXmlNode* props = doc.FirstChild();

	if ((props->NoChildren()) || (props == NULL))
	{
		return false;
	}

	TiXmlNode* prop = props->FirstChild();
			
	while (prop != NULL)
	{
		TiXmlElement* elem = prop->ToElement();

		if (!elem)
		{
			prop = props->IterateChildren(prop);
			continue;
		}

		if ((elem->Attribute("time")) && (elem->Attribute("usecurrentposition")))
		{
			unsigned int time = rtl_atoi(elem->Attribute("time"));

			MP_NEW_P10(cameraFrame, SCameraFrame, 0, 0, 0, 0, 0, 0, 0, 0, 0, time);
			m_frames.push_back(cameraFrame);

			prop = props->IterateChildren(prop);
			continue;
		}

		if ((!elem->Attribute("eyex")) ||
			(!elem->Attribute("eyey")) ||
			(!elem->Attribute("eyez")) ||
			(!elem->Attribute("lookx")) ||
			(!elem->Attribute("looky")) ||
			(!elem->Attribute("lookz")) ||
			(!elem->Attribute("time")))
		{
			prop = props->IterateChildren(prop);
			continue;
		}		

		float eyeX = StringToFloat(elem->Attribute("eyex"));
		float eyeY = StringToFloat(elem->Attribute("eyey"));
		float eyeZ = StringToFloat(elem->Attribute("eyez"));
		float lookX = StringToFloat(elem->Attribute("lookx"));
		float lookY = StringToFloat(elem->Attribute("looky"));
		float lookZ = StringToFloat(elem->Attribute("lookz"));
		unsigned int time = rtl_atoi(elem->Attribute("time"));

		float upX = 0.0f;
		float upY = 1.0f;
		float upZ = 0.0f;

		if (elem->Attribute("upx"))
		{
			upX = StringToFloat(elem->Attribute("upx"));
		}

		if (elem->Attribute("upy"))
		{
			upY = StringToFloat(elem->Attribute("upy"));
		}

		if (elem->Attribute("upz"))
		{
			upZ = StringToFloat(elem->Attribute("upz"));
		}

		MP_NEW_P10(cameraFrame, SCameraFrame, eyeX, eyeY, eyeZ, lookX, lookY, lookZ, upX, upY, upZ, time);
		m_frames.push_back(cameraFrame);

		prop = props->IterateChildren(prop);
	}

	return true;
}

void CCameraTrackController::StartTrack()
{
	m_isPlaying = true;
	m_startTime = g->tp.GetTickCount() - TIME_SHIFT;

	std::vector<SCameraFrame *>::iterator it = m_frames.begin();
	std::vector<SCameraFrame *>::iterator itEnd = m_frames.end();
	for ( ; it != itEnd; it++)
	{
		SCameraFrame* cameraFrame = (*it);
		if (cameraFrame->isCurrentCamera)
		{
			if (g->cm.GetActiveCamera())
			{
				CVector3D eye = g->cm.GetActiveCamera()->GetEyePosition();
				CVector3D lookAt = g->cm.GetActiveCamera()->GetLookAt();
				CVector3D up = g->cm.GetActiveCamera()->GetUp();

				cameraFrame->eyeX = eye.x;
				cameraFrame->eyeY = eye.y;
				cameraFrame->eyeZ = eye.z;
				cameraFrame->lookX = lookAt.x;
				cameraFrame->lookY = lookAt.y;
				cameraFrame->lookZ = lookAt.z;
				cameraFrame->upX = up.x;
				cameraFrame->upY = up.y;
				cameraFrame->upZ = up.z;
			}
		}
	}
}

int CCameraTrackController::GetCameraParams(CVector3D& eye, CVector3D& lookAt)
{
	unsigned int time = GetTrackTime();

	SCameraFrame* firstKeyFrame = NULL;
	SCameraFrame* secondKeyFrame = NULL;

	std::vector<SCameraFrame *>::iterator it = m_frames.begin();
	std::vector<SCameraFrame *>::iterator itEnd = m_frames.end();
	for ( ; it != itEnd; it++)
	{
		SCameraFrame* cameraFrame = (*it);
		if (cameraFrame->time <= time)
		{
			firstKeyFrame = cameraFrame;
		}
		else if (!secondKeyFrame)
		{
			secondKeyFrame = cameraFrame;
		}
	}

	if ((!firstKeyFrame) || (!secondKeyFrame))
	{
		if (m_isPlaying) {
			m_isPlaying = false;
			return PLAYING_ENDED;
		}
		else {
			return PLAYING_NOT_STARTED;
		}
	}

	unsigned int deltaTime = secondKeyFrame->time - firstKeyFrame->time;
	unsigned int timeShift = time - firstKeyFrame->time;
	
	float interpolationKoef = (deltaTime == 0) ? 1.0f : ((float)timeShift / (float)deltaTime);
	float invInterpolationKoef = 1.0f - interpolationKoef;

#define INTERPOLATE_KOEF(x) float x = firstKeyFrame->##x * invInterpolationKoef + secondKeyFrame->##x * interpolationKoef;

	INTERPOLATE_KOEF(eyeX)
	INTERPOLATE_KOEF(eyeY)
	INTERPOLATE_KOEF(eyeZ)
	INTERPOLATE_KOEF(lookX)
	INTERPOLATE_KOEF(lookY)
	INTERPOLATE_KOEF(lookZ)
	INTERPOLATE_KOEF(upX)
	INTERPOLATE_KOEF(upY)
	INTERPOLATE_KOEF(upZ)

	eye.x = eyeX;	
	eye.y = eyeY;
	eye.z = eyeZ;
	lookAt.x = lookX;	
	lookAt.y = lookY;
	lookAt.z = lookZ;	

	return PLAYING_OK;
}

unsigned int CCameraTrackController::GetTrackTime()
{
	return (unsigned int)(g->tp.GetTickCount() - m_startTime);
}

CCameraTrackController::~CCameraTrackController()
{
}