#pragma once

class CRecordState
{
public:
	CRecordState();
	virtual ~CRecordState();

public:
	// установить состо€ние записи
	void setRecordState( BOOL bRecordState);
	// установить им€ файла, в который будет идти запись mp3-кадров
	void setFileName( wchar_t *sFileName);
	// возвратить им€ файлы
	wchar_t *getFileName();
	// возвратить состо€ние записи
	BOOL getRecordState();
	// установить состо€ние сформированности блока кадров (п€ть полных кадров по 1152)
	//void setBlockReady( BOOL bBlockReady);
	// возвратить состо€ние сформированности блока кадров (п€ть полных кадров по 1152)
	//BOOL getBlockReady();
	// установить состо€ние фонового звука
	void setBackgroundNoise( BOOL bBackgroundNoise);
	// получить состо€ние фонового звука (отсутсвие звука)
	BOOL getBackgroundNoise();
	// установить число каналов
	void setChannels( int channels);
	// возвратить число каналов
	int getChannels();
	// установить частоту
	void setSamples_per_sec( int samples_per_sec);
	// возвратить частоту
	int getSamples_per_sec();
	// установить число битов сэмлпа
	void setBits_per_sample( int bits_per_sample);
	// возвратить число битов сэмлпа
	int getBits_per_sample();

private:
	// состо€ние записи. ≈cли true, то позвол€ть записывать
	BOOL bRecordState;
	// им€ файла
	WCHAR sFileName[ MAX_PATH];
	//
	//BOOL bBlockReady;
	// true, то фоновый звук (шум)
	BOOL bBackgroundNoise;
	int channels;
	int samples_per_sec;
	int bits_per_sample;
};
