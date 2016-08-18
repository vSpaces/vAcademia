#ifndef _MUMBLE_VOLUME_TALKING_CONTROLLER_H
#define _MUMBLE_VOLUME_TALKING_CONTROLLER_H

//#include "murmur_pch.h"
//#include "mumble_pch.hpp"
//#include "SingletonTemplate.h"
#define MAX_VOLUME 3000;

class CVolumeTalkingController// : public Singleton<CVolumeTalkingController>
{
protected:
	CVolumeTalkingController(){};
	//friend class Singleton<CVolumeTalkingController>;

public:
	double AddUserVolume( QString asName, void *apBuffer, bool aTypeIsShort, int aiFrameSize, double &aModeCurrentTopInputLevel);
	double AddUserVolume( QString asName, double aLevel);
	double GetUserVolume( QString &asName);
	bool UserIsHaveMaxVolume( QString asName);

	static CVolumeTalkingController *GetVolumeController();
	static void Destroy();

private:
	QHash<QString, double> qhUserVolume;
	QMutex mutex;
	//QString qUserSpeak;
};

#endif
