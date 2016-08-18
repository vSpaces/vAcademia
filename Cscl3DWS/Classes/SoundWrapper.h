// SoundWrapper.h: interface for the CSoundWrapper class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SOUNDWRAPPER_H__74ECC5D6_DE58_4CD7_BFFF_4390E6C13006__INCLUDED_)
#define AFX_SOUNDWRAPPER_H__74ECC5D6_DE58_4CD7_BFFF_4390E6C13006__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#include "Atlbase.h"
//#include "nrmanager.h"
#include "..\rm\rm.h"
#include "..\rm\rmext.h"
//#include "..\res\res.h"
#include <map>
#include <ComManRes.h>
//#include "..\ResManIntr.h"

class CSoundWrapper;
using namespace res;


class ATL_NO_VTABLE CResourceNotify : public IResourceNotify

{
	friend	class	CSoundWrapper;
public:
	CResourceNotify();
	virtual	~CResourceNotify();	

public:
	long onResourceNotify(IResource* a_pIResource, DWORD aNotifyCode, DWORD aNotifyServerCode, wchar_t *aErrorText);	
	void destroy();
	void setAsynchResource(void* aAsynch){};
	void setSoundWrapper( CSoundWrapper* apSoundWrapper);
	bool isNeedDestroyByComman(){return false;};

private:
	CSoundWrapper*	m_pSoundWrapper;
};

namespace res
{
	class resResourceManagerWrapper;
	//struct IResMan;
}

namespace sm
{
	struct smISoundManager;
	struct smISoundNotify;
}

class CSoundWrapper : public rm::rmISoundManager
{
	friend	class	CResourceNotify;
protected:
	sm::smISoundManager *mpSoundMan;
	oms::omsContext* mpOMSContext;
	IResMan* mpResMan;

	void Trace(const char* pszFormat, ...);
	void Trace(HRESULT aHR,const char* apszFun);

public:
	void setSoundManager( sm::smISoundManager *aSoundMan);
	void SetResourceMan( IResMan *aResMan);
	
	void setOMSContext(oms::omsContext* apOMSContext);
	CSoundWrapper();
	virtual ~CSoundWrapper();

	// rmISoundManager
	int		GetLexema(int aiPerson, int& aoiLexema);
	int		OpenSound( ifile *pfile, sm::smISoundNotify *apISoundNotify = NULL, bool bAttached = FALSE);
	int		OpenSound(const wchar_t* apwcURL, sm::smISoundNotify *apISoundNotify = NULL);
	int 	UpdateFileInfo( int	aiSound, unsigned long adwLoadedBytes, bool bLoaded);
	bool	PlaySound(int	aiSound, int aiPerson, bool loop);
	void	StopSound(int	aiSound);
	void	SoundOff();
	void	SoundOn(int id, char** ppGUID);
	bool	IsPlaying(int	aiSound);
	bool	CloseSound(int aiSound);
	bool	SetPosition(int	aiSound, unsigned long adwPos);
	bool	GetPosition(int	aiSound, unsigned long &adwPos);
	bool	GetFrequency(int aiSound, unsigned long &adwFrequency);
	bool	GetDuration(int	aiSound, unsigned long &adwDur);
	bool	GetLength(int aiSound, unsigned long &adwLength);
	bool	GetSize( int	aiSound, unsigned long &adwSize);
	bool	GetCurrentSize( int aiSound, unsigned long &adwSize);
	bool	GetVolume( int id, float &fVolume);
	bool	SetCooperativeLevel(unsigned long hWnd, unsigned long level);
	bool	Set3DSoundParams( int id, rmml::ml3DPosition dist, rmml::ml3DPosition dir, rmml::ml3DPosition vp_dir);
	bool	Set3DSoundParams( int id, float coef, float left_coef, float right_coef);
	bool	SetVolume( int id, float volume);
	bool	SetPitchShift( int id,  float shift);
	bool	UpdateAllSound( int playingState);

protected:

	typedef	MP_MAP<IResource*, res::resIAsynchResource*>	swAsynchMap;
	typedef	swAsynchMap::iterator						swAsynchMapIterator;

	//CComQIPtr<IResourceNotify>							m_pIResourceNotify;
	CResourceNotify m_pIResourceNotify;
	swAsynchMap		m_mapAsynchResources;

	void OnResourceNotify( IResource* apIResource, unsigned long adwNotifyCode, unsigned int aNotifyServerCode, wchar_t *aErrorText);
};

#endif // !defined(AFX_SOUNDWRAPPER_H__74ECC5D6_DE58_4CD7_BFFF_4390E6C13006__INCLUDED_)
