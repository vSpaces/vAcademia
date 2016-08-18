#pragma once

#define MAXVOLUME (( WORD) 0xFFFF)

class CSoundVolumeController
{
public:
	CSoundVolumeController();
	virtual ~CSoundVolumeController();

protected:
	enum{ MIC_VOLUME, PLAY_VOLUME};
	//
	BOOL setVolume( int type, DWORD inout, DWORD ComponentType, DWORD dwVol);
	//
	BOOL getVolume( int type, DWORD inout, DWORD ComponentType, DWORD &dwVol);

public:
	// установка уровня громкости записи микрофона
	BOOL setMicVolume( float coef);
	// получения уровня громкости записи микрофона
	float getMicVolume();

	// установка уровня громкости записи
	BOOL setRecordVolume( float coef);
	// получения уровня громкости записи
	float getRecordVolume();

	// установка уровня громкости воспроизведения наушниками
	BOOL setHeadPhoneVolume( float coef);
	// получения уровня громкости воспроизведения наушниками
	float getHeadPhoneVolume();

	// установка уровня громкости воспроизведения
	BOOL setPlayVolume( float coef);
	// получения уровня громкости воспроизведения
	float getPlayVolume();

	void restore();

private:
	// текуший коэффициент уровня громкости записи микрофона
	float micVolumeCoef;
	// предыдущий коэффициент уровня громкости записи микрофона
	float micVolumeCoefSaver;
	// текуший коэффициент уровня громкости записи
	float recordVolumeCoef;
	// предыдущий коэффициент уровня громкости записи
	float recordVolumeCoefSaver;

	// текуший коэффициент уровня громкости воспроизведения наушников
	float headPhoneVolumeCoef;
	// предыдущий коэффициент уровня громкости воспроизведения наушников
	float headPhoneVolumeCoefSaver;

	// текуший коэффициент уровня громкости воспроизведения
	float playVolumeCoef;
	// предыдущий коэффициент уровня громкости воспроизведения
	float playVolumeCoefSaver;

};
