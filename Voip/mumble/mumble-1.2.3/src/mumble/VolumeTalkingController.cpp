
#include "mumble_pch.hpp"
#include "VolumeTalkingController.h"
//#include "Global.h"
//#include "Time.h"

static CVolumeTalkingController *pVolumeTalkingController = NULL;

void CVolumeTalkingController::Destroy()
{
	delete pVolumeTalkingController;
	pVolumeTalkingController = NULL;
	//FreeInst();
}

CVolumeTalkingController *CVolumeTalkingController::GetVolumeController()
{
	if( pVolumeTalkingController == NULL)
		pVolumeTalkingController = new CVolumeTalkingController();
	return pVolumeTalkingController;
	//return Instance();
}

double CVolumeTalkingController::GetUserVolume( QString &asName)
{
	mutex.lock();
	QHashIterator<QString, double> i(qhUserVolume);
	while (i.hasNext()) {
		i.next();
		if( i.key() == asName)
		{
			mutex.unlock();
			return i.value();
		}	
	}
	mutex.unlock();
	return 0;
}

double CVolumeTalkingController::AddUserVolume( QString asName, void *apBuffer, bool aTypeIsShort, int aiFrameSize, double &aModeCurrentTopInputLevel)
{
	short sampleValue = 0;
	long calculatedMedian = 0;
	long calculatedDisperse = 0;
	for( int i=0; i < aiFrameSize; i++)
	{
		
		if( aTypeIsShort)
		{
			sampleValue = static_cast<float>(((short*)(apBuffer))[i]);
		}
		else
		{
			float sampleValueFloat = ((float*)(apBuffer))[i];
			float adjBuf = sampleValueFloat * 1;
			sampleValue = static_cast<short>(32768.f * (adjBuf < -1.0f ? -1.0f : (adjBuf > 1.0f ? 1.0f : adjBuf)));
		}
		calculatedMedian += sampleValue;

		if( sampleValue < 0)
			sampleValue = -sampleValue;

		if( calculatedDisperse < sampleValue)
			calculatedDisperse = sampleValue;
	}

	if( calculatedDisperse > aModeCurrentTopInputLevel)
		aModeCurrentTopInputLevel = calculatedDisperse;

	double currentInputLevel = GetUserVolume( asName);

	currentInputLevel = (currentInputLevel + ((double)calculatedDisperse - (double)calculatedMedian/aiFrameSize) / aModeCurrentTopInputLevel) / 2;

	AddUserVolume( asName, currentInputLevel);
	return currentInputLevel;
}

double CVolumeTalkingController::AddUserVolume( QString asName, double aLevel)
{
	mutex.lock();
	qhUserVolume[ asName] = aLevel;
	mutex.unlock();
	return aLevel;
}

bool CVolumeTalkingController::UserIsHaveMaxVolume( QString asName)
{
	double userVolume = 0;
	mutex.lock();
	QHashIterator<QString, double> i(qhUserVolume);
	while (i.hasNext()) {
		i.next();
		if( i.key() == asName)
		{
			userVolume = i.value();
			break;
		}	
	}
	if( userVolume == 0)
	{
		mutex.unlock();
		return false;
	}
	QHashIterator<QString, double> iter(qhUserVolume);
	while (iter.hasNext()) {
		iter.next();
		if( iter.key() != asName)
		{
			if( userVolume < iter.value())
			{				
				//qUserSpeak = iter.key();
				//qWarning() << "CVolumeTalkingController::UserIsHaveMaxVolume user with max volume ==" <<  iter.key();
				mutex.unlock();
				return false;
			}
		}	
	}
	//qUserSpeak = "my";
	//qWarning() << "CVolumeTalkingController::UserIsHaveMaxVolume my user with max volume";
	mutex.unlock();
	return true;
}

/*void CVolumeTalkingController::ClearSpeekStatus()
{
	qUserSpeak = "";
	iUserSpeaking = 0;
}*/