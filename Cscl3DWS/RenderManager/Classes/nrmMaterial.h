#if !defined(AFX_nrmMaterial_H__A94D4F09_D0E9_480F_A889_757EF331EA0A__INCLUDED_)
#define AFX_nrmMaterial_H__A94D4F09_D0E9_480F_A889_757EF331EA0A__INCLUDED_

#include "../CommonRenderManagerHeader.h"

#include "nrmanager.h"

typedef struct _SMapDescr
{
	mlMedia* m_map;
	unsigned int m_materialID;
	MP_WSTRING m_index;

	_SMapDescr(mlMedia* map, unsigned int id, const wchar_t* index);	
} SMapDescr;

typedef	MP_VECTOR_DC<SMapDescr> ListeningMaps;

class nrmMaterial	: public moMedia,
					  public moIMaterial,
					  public mlIVisibleNotifyListener
{
public:
	nrmMaterial(nrm3DObject* owner);
	virtual ~nrmMaterial();

protected:
	void add_listening_descr(mlMedia* map, unsigned int id, const wchar_t* index);
	void nrmMaterial::update_composition_texture(mlMedia* map, unsigned int id);
	void update_texture(SMapDescr& map_descr);

public:	// Interface members
	void OnMapChanged(mlIMaterial* pmlMaterial, unsigned auID, const wchar_t* apwcIndex);
	void OnParamChanged(mlIMaterial* pmlMaterial);
	mlColor GetDiffuse();
	mlColor GetAmbient();
	mlColor GetSpecular();
	double GetPower(); // 0..255
	mlColor GetEmissive();
	unsigned char GetOpacity(); // 0 - прозрачный, 255 - непрозрачный
	mlMedia* GetMap(const wchar_t* apwcIndex);
// реализация moMedia
	virtual void FrozenChanged();

// реализация mlIVisibleNotifyListener
	virtual void VisualRepresentationChanged(mlMedia* map);
	virtual void VisibleDestroyed(mlMedia* map);


	int m_submeshID;

protected:
	void			CloneMaterialIfNeeded();

	nrm3DObject*	m_owner;
	bool			m_isMaterialCloned;
	ListeningMaps* m_listeningMaps;
};

#endif