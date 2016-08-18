#pragma once

#define		RES_TYPE_SKELETON		2
#define		RES_TYPE_MESH			2
#define		RES_TYPE_BMF			2	
#define		RES_TYPE_TEXTURE		1
#define		RES_TYPE_SOUND			0

namespace	resources
{
	enum	RESOURCE_STATE {RS_UNDEFINED, RS_LOADING, RS_PAUSED, RS_LOADED, RS_NOTEXISTS, RS_NOTACCEPTED, RS_NOTREAD};
};

struct ifile;

struct IAsynchResource
{
	// открывает
	virtual void 	OpenAsynch(char* file_name) = 0;
	// читает следующие n байт
	virtual int	ReadNext(int nBytes) = 0;
	// возвращает режим загрузки
	virtual	resources::RESOURCE_STATE GetLoadingState() = 0;
	// приоритеты загрузки
	virtual	void	SetPriority(float afPriority) = 0;
	virtual	float	GetPriority() = 0;
	// главные приоритеты загрузки (приоритеты по типу ресурса)
	virtual	void	SetMainPriority(unsigned int	adwPriority) = 0;
	virtual	unsigned int	GetMainPriority() = 0;
	// номер зоны видимости
	virtual	void	SetZoneNum(unsigned int	zoneNum) = 0;
	virtual	unsigned int	GetZoneNum() = 0;
	// lod
	virtual	void	SetLOD(unsigned int lod) = 0;
	virtual	unsigned int	GetLOD() = 0;
	// приоритет из скрипта
	virtual	void	SetScriptPriority(unsigned char priority) = 0;
	virtual	unsigned char	GetScriptPriority() = 0;
	// текстура/модель
	virtual	void	SetExactObject(void* obj) = 0;
	virtual	void*	GetExactObject() = 0;
	// процент загрузки
	virtual	float	GetLoadedPercent() = 0;
	virtual unsigned int	GetBytesLoaded() = 0;
	virtual ifile*	GetIFile() = 0;
	// вернуть размер ресурса
	virtual unsigned int GetSize() = 0;
	// установить соответствие с объектами
	virtual void	AddObject(void* vs, float koef = 1.0f) = 0; 
	virtual void 	GetObject(int num, void** vs, float* koef) = 0;

	virtual int	GetObjectCount() = 0;

	virtual void 	SetLoadedState() = 0;

    virtual void	SetFileName(const char* file_name) = 0;
	virtual char*	GetFileName() = 0;
	virtual bool	IsLocalFileExists() = 0;
	//virtual bool	IsRemoteFileExists() = 0;

	virtual void	Pause() = 0;
	virtual bool	IsPause() = 0;
	virtual void	Continue() = 0;

	virtual void	Download() = 0;
	virtual void	Unpack() = 0;
	virtual void	Release() = 0;

	virtual void	SetImportantSlideStatus(bool isImportant) = 0;
	virtual bool	IsImportantSlide() = 0;

	virtual void	MustDestroySelf() = 0;
	virtual bool	IsMustDestroySelf() = 0;
};