// nrm3DObject.cpp: implementation of the nrm3DObject class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "IniFile.h"
#include "nrmMaterial.h"
#include "nrmImage.h"
#include "nrmVideo.h"
#include "nrmAnim.h"
#include "nrmActiveX.h"
#include "nrmBrowser.h"
#include "nrm3DObject.h"
#include "LODGroup.h"
#include "UserData.h"
#include "ToolPalette.h"
#include "WhiteBoardManager.h"
#include "PlatformDependent.h"
#include "SimpleDrawingTarget.h"
#include "sharingboardmanager.h"

#include "GlobalSingletonStorage.h"
#include "RMContext.h"

#define DWORD2mlColor(m, c)	m.r = (byte)(((DWORD)c&0x00FF0000) >> 16);	\
							m.g = (byte)(((DWORD)c&0x0000FF00) >> 8);		\
							m.b = (byte)(((DWORD)c&0x000000FF));

#define mlColor2DWORD(m)	(0xFF000000 || (DWORD)(m.r<<16) || (DWORD)(m.g<<8) || m.b);

#define mlColor2D3DCOLORVALUE(m, c)	c.r = m.r/255.0f;	c.g = m.g/255.0f;	c.b = m.b/255.0f;

_SMapDescr::_SMapDescr(mlMedia* map, unsigned int id, const wchar_t* index):
	MP_WSTRING_INIT(m_index)
{
	m_map = map;
	m_materialID = id;
	m_index = index;
}

nrmMaterial::nrmMaterial(nrm3DObject* owner)
{
	m_submeshID = 0;
	m_owner = owner;
	m_isMaterialCloned = false;
	m_listeningMaps = NULL;
}

mlColor nrmMaterial::GetDiffuse()
{
	mlColor	mlcolor;
	ML_INIT_COLOR(mlcolor);
	return mlcolor;
}

mlColor nrmMaterial::GetAmbient()
{
	mlColor	mlcolor;
	ML_INIT_COLOR(mlcolor);
	return mlcolor;
}

mlColor nrmMaterial::GetSpecular()
{
	mlColor	mlcolor;
	ML_INIT_COLOR(mlcolor);
	return mlcolor;
}

double nrmMaterial::GetPower()
{
	return 0;
}

mlColor nrmMaterial::GetEmissive()
{
	mlColor	mlcolor;
	ML_INIT_COLOR(mlcolor);
	return mlcolor;
}

unsigned char nrmMaterial::GetOpacity()
{
	DWORD	color = 0;
	return (unsigned char)((color&0xFF000000) >> 24);
}

mlMedia* nrmMaterial::GetMap(const wchar_t* /*apwcIndex*/)
{
	return NULL;
}

void nrmMaterial::OnParamChanged(mlIMaterial* pmlMaterial)
{
	if (!pmlMaterial)	
	{
		return;
	}

	if ((!m_owner) || (!m_owner->m_obj3d))	
	{
		return;
	}
}

void nrmMaterial::OnMapChanged(mlIMaterial* pmlMaterial, unsigned int auID, const wchar_t *apwcIndex)
{
	if (!m_owner)	
	{
		return;
	}

	C3DObject* obj = m_owner->m_obj3d;
	if (!obj)
	{
		return;
	}

	mlMedia* map = pmlMaterial->GetMap(apwcIndex);
	if (!map)	
	{
		return;
	}

	CloneMaterialIfNeeded();
	DWORD	mapType = MLMT_UNDEFINED;
	if (map->GetType() == MLMT_VIDEO)
	{
		nrmVideo* pnrmVideo = (nrmVideo*)map->GetSafeMO();
		if (!pnrmVideo)	
		{
			return;
		}

		if (!pnrmVideo->m_video)
		{
			return;
		}

		if (!m_owner->m_obj3d)
		{
			return;
		}

		m_owner->m_obj3d->GetLODGroup()->SetDynamicTexture(auID, pnrmVideo->m_video->GetDynamicTexture());

		mapType = MLMT_VIDEO;
	}
	else	if( map->GetType() == MLMT_IMAGE)
	{		
		nrmImage* img = (nrmImage*)map->GetSafeMO();
        if (ML_MO_IS_NULL(img))	
		{
			return;
		}

		if (!img->GetMLMedia())
		{
			return;
		}

		bool isEditable = false;
		bool isRemoteControl = false;
		mlIImage* pmoImage = img->GetMLMedia()->GetIImage();
		if (pmoImage)
		{
			if (pmoImage->GetEditable())
			{
				isEditable = true;
			}
			if (pmoImage->GetRemoteControlNow())
			{
				isRemoteControl = true;
			}
		}

		if ((!img->m_sprite) && (!isEditable))
		{
			return;
		}

		int level = -1;
		if (img->GetMLMedia()->GetStringProp("level"))
		{
			USES_CONVERSION;
			level = rtl_atoi(W2A(img->GetMLMedia()->GetStringProp("level")));
		}
		
		int textureID = -1;
		if (img->m_sprite)
		{
			textureID = img->m_sprite->GetTextureID(0);
			CTexture* texture = g->tm.GetObjectPointer(textureID);
			if (texture->GetMinFilter() == GL_NEAREST)
			{
				texture->SetCurrentFiltration(GL_LINEAR, GL_LINEAR, 1, 0);
			}
			if (texture->IsDeleted())
			{
				textureID = -1;
			}
		}

		float aspectY = (float)img->GetMLMedia()->GetDoubleProp("aspect");

		if (aspectY == 0.0f)
		{
			aspectY = 1.0f;
		}

		float aspectX = (float)img->GetMLMedia()->GetDoubleProp("aspectX");

		if (aspectX == 0.0f)
		{
			aspectX = 1.0f;
		}		
        
		if (-1 != textureID)
		{
			CTexture* texture = g->tm.GetObjectPointer(textureID);
			m_owner->m_obj3d->GetLODGroup()->SetTexture(auID, textureID, isEditable, level, CVector2D(aspectX, aspectY));
		}

		mapType = MLMT_IMAGE;	

		if (isEditable)
		{
			gRM->wbMan->CreateWhiteBoard(m_owner->m_obj3d, img, textureID, auID);
		}
		else if (isRemoteControl)
		{
			gRM->shbMan->CreateSharingBoard(m_owner->m_obj3d, img, textureID, auID, aspectX, aspectY);
		}
		else
		{
			if (level != -1)
			{
				m_owner->m_obj3d->SetUserData(USER_DATA_RENDER_TARGET, NULL);
			}
			m_owner->m_obj3d->SetUserData(USER_DATA_SHARING_TARGET, NULL);
		}

		img->OnChanged(0);
	}
	/*else	if( map->GetType() == MLMT_ANIMATION)
	{
		nrmAnim*	pnrmAnim = (nrmAnim*)map->GetSafeMO();
		if(!pnrmAnim)	return;
		if(!pnrmAnim->pisprite)	return;
		int txID=pnrmAnim->pisprite->get_textureID();
		object->set_part_subpart_diffuse_map_id(0, auID, txID);
		mapType = MLMT_ANIMATION;
	}
	else	if( map->GetType() == MLMT_ACTIVEX)
	{
		nrmActiveX*	pnrmActiveX = (nrmActiveX*)map->GetSafeMO();
		if(!pnrmActiveX)	return;
		if(!pnrmActiveX->piactivex)	return;
		int txID=pnrmActiveX->piactivex->get_textureID();
		object->set_part_subpart_diffuse_map_id(0, auID, txID);
		int shaderID = object->get_part_subpart_shader_id(0, auID);
		ishader* pShader = m_owner->get_ishaderlib()->get_ishader(shaderID);
		if( pShader)
			pShader->set_predefined_type(materials::SOLID_TEXTURE);
		pnrmActiveX->piwindow->set_texture_clone_mode(TCM_ALWAYS);
		pnrmActiveX->piwindow->enable_external_control( true);
		mapType = MLMT_ACTIVEX;
	}
*/
	else if( map->GetType() == MLMT_BROWSER)
	{
		nrmBrowser* pnrmBrowser = (nrmBrowser*)map->GetSafeMO();
		if (!pnrmBrowser)	
		{
			return;
		}

		IDynamicTexture* pDT = pnrmBrowser->GetDynamicTexture();
		if (!pDT)
		{
			return;
		}

		m_owner->m_obj3d->GetLODGroup()->SetDynamicTexture(auID, pDT);

		mapType = MLMT_BROWSER;
/*
		nrmBrowser*	pnrmBrowser = (nrmBrowser*)map->GetSafeMO();
		if(!pnrmBrowser)	return;
		if(!pnrmBrowser->piactivex)	return;
		int txID=pnrmBrowser->piactivex->get_textureID();
		object->set_part_subpart_diffuse_map_id(0, auID, txID);
		int shaderID = object->get_part_subpart_shader_id(0, auID);
		ishader* pShader = m_owner->get_ishaderlib()->get_ishader(shaderID);
		if( pShader)
			pShader->set_predefined_type(materials::SOLID_TEXTURE);
		pnrmBrowser->piwindow->set_texture_clone_mode(TCM_ALWAYS);
		pnrmBrowser->piwindow->enable_external_control( true);
		mapType = MLMT_ACTIVEX;
*/
	}
	else if( map->GetType() == MLMT_COMPOSITION)
	{
		update_composition_texture(map, auID);
		mapType = MLMT_COMPOSITION;
		// (и будем делать так c каждым изменением композиции)
		// надо запомнить, для какого ID какой индекс слушаем, 
		// чтобы потом можно было найти
		add_listening_descr(map, auID, apwcIndex);
	}

	if (mapType != MLMT_UNDEFINED)
	{	
		m_owner->SetMaterialMapMedia(auID, map);
	}
}

// добавляет запись о том, что слушаем уведомления от элемента
void nrmMaterial::add_listening_descr(mlMedia* map, unsigned int id, const wchar_t* index)
{
	if(	m_listeningMaps == NULL)
	{
		m_listeningMaps = MP_NEW(ListeningMaps);
		MP_STRING_DC_INIT((*m_listeningMaps));
	}

	map->GetIVisible()->SetVListener(this);

	// если мапку заменили на другую, то просто меняем значение свойства
	ListeningMaps::iterator vi = m_listeningMaps->begin();
	for ( ; vi != m_listeningMaps->end(); vi++)
	{
		if(vi->m_materialID == id && vi->m_index == index)
		{
			vi->m_map->GetIVisible()->RemoveVListener(this);
			vi->m_map = map;
			return;
		}
	}

	SMapDescr tmp(map, id, index);
	m_listeningMaps->push_back(tmp);
}

void nrmMaterial::update_composition_texture(mlMedia* /*map*/, unsigned int /*id*/)
{
	/*if( !m_owner)	return;
	iobject3d* object = m_owner->piobject3d;
	if( !object)	return;
	if( id >= object->get_subparts_count(0))	return;

	// создаем nrmImage, 
	nrmImage*	pnrmImage = new nrmImage(NULL);
	pnrmImage->SetRenderManager(m_owner->GetRenderManager());
	mlSize size = map->GetIVisible()->GetSize();
	if(!pnrmImage)	return;
	// вызываем у него MakeSnapshot с композицией
	mlMedia** pVisibleChildren = map->GetAllVisibleChildren();
	if(pVisibleChildren == NULL){
		delete pnrmImage;
		return;
	}
	mlRect rcCrop; ML_INIT_RECT(rcCrop);
	map->GetIVisible()->Show();
	pnrmImage->MakeSnapshot(pVisibleChildren, rcCrop);
	map->GetIVisible()->Hide();
	map->FreeChildrenArray(pVisibleChildren);
	// берем у него textureID
	if(!pnrmImage->pisprite)	return;
	int txID=pnrmImage->getTextureID();
	object->set_part_subpart_diffuse_map_id(0, id, txID);
	delete pnrmImage;*/
	// ??!! надо сделать, чтобы это работало через механизм, который сделал Слава для рисования, чтобы при изменениях в композиции изображение снапшотилось сразу на текстуру 
}

void nrmMaterial::update_texture(SMapDescr& map_descr)
{
	DWORD mapType = map_descr.m_map->GetType();

	if (mapType == MLMT_COMPOSITION)
	{
		update_composition_texture(map_descr.m_map, map_descr.m_materialID);
	}
}

void nrmMaterial::FrozenChanged()
{

}

void nrmMaterial::VisualRepresentationChanged(mlMedia* map)
{
	assert(map != NULL);
	if(map == NULL)
	{
		return;
	}

	ListeningMaps::iterator vi = m_listeningMaps->begin();
	for(; vi != m_listeningMaps->end(); vi++)
	{
		if(vi->m_map == map)
		{
			// обновить текстуру
			update_texture(*vi);
		}
	}
}

void nrmMaterial::VisibleDestroyed(mlMedia* map)
{
	// удалить все записи с map из m_listeningMaps
	if(m_listeningMaps == NULL)
	{
		return;
	}

	ListeningMaps::iterator vi = m_listeningMaps->begin();
	for(; vi != m_listeningMaps->end(); vi++)
	{
		if(vi->m_map == map)
		{
			vi = m_listeningMaps->erase(vi);
			vi--;
		}
	}
}

void	nrmMaterial::CloneMaterialIfNeeded()
{
	if( m_isMaterialCloned)	return;
	m_isMaterialCloned = true;

	if( !m_owner)	return;
/*	iobject3d* object = m_owner->piobject3d;
	if( !object)	return;
	iframework*	m_piframework = NULL;
	ibase*	pibase = NULL;
	GET_INTERFACE( ibase, object,&pibase);
	assert( pibase);
	m_piframework = pibase->get_iframework();
	assert( m_piframework);
	if( !m_piframework)	return;
	ishader* pShader = m_piframework->get_ishaderlib()->get_ishader( object->get_part_subpart_shader_id(0, m_submeshID));
	int newMaterialID = m_piframework->get_itextlib()->clone_shader_material(pShader->get_material_id());
	pShader->set_material_id(newMaterialID);*/
}

nrmMaterial::~nrmMaterial()
{
	// если слушали какие-нибудь mlMedia на изменения, 
	if(m_listeningMaps != NULL)
	{
		// то разрегистриться у них
		ListeningMaps::const_iterator cvi = m_listeningMaps->begin();
		for(; cvi != m_listeningMaps->end(); cvi++){
			mlIVisible* vis = cvi->m_map->GetIVisible();
			if(vis != NULL)
				vis->RemoveVListener(this);
		}
	}

	if (m_listeningMaps != NULL)
		MP_DELETE(m_listeningMaps); 
	m_listeningMaps = NULL;
}