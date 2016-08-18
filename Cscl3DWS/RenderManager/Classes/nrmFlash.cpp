
#include "stdafx.h"
#include <locale>
#include <stdlib.h>
#include "nrmFlash.h"
#include "FlashPlugin.h"
#include "IDynamicTexture.h"
#include "HelperFunctions.h"

#include "GlobalSingletonStorage.h"
#include "RMContext.h"

nrmFlash::nrmFlash(mlMedia* apMLMedia) : 
	nrmObject(apMLMedia), 
	m_flash(NULL),
	m_sprite(NULL)
{
}

nrmFlash::~nrmFlash()
{
	if (m_sprite){
		m_sprite->RemoveDynamicTextures();
	}

	recreate_sprite(false);

}

// изменилось абсолютное положение 
void nrmFlash::AbsXYChanged()
{
	if (!m_sprite)
	{
		return;
	}

	mlPoint pnt = mpMLMedia->GetIVisible()->GetAbsXY();
	m_sprite->SetCoords(pnt.x, pnt.y);
} 

void nrmFlash::SizeChanged()
{
	if(!m_flash)
		return;
	mlSize sz=mpMLMedia->GetIVisible()->GetSize();
	m_flash->SetSize(sz.width, sz.height);
	if(sz.width <= 0)
		sz.width = 1;
	if(sz.height <= 0)
		sz.height = 1;
	
	//recreate_sprite();
	if(m_sprite){
		m_sprite->SetSize(sz.width, sz.height);
		m_sprite->SetWorkRect(0, 0, sz.width, sz.height);
	}

	AbsDepthChanged();
	AbsXYChanged();
	AbsVisibleChanged();
}

// изменилась абсолютная глубина
void nrmFlash::AbsDepthChanged()
{
	if(m_sprite)
		m_sprite->SetDepth(GetMLMedia()->GetIVisible()->GetAbsDepth());
}

mlSize nrmFlash::GetMediaSize(){
	mlSize size; ML_INIT_SIZE(size);
	return size;
}

bool nrmFlash::recreate_sprite(bool create){
if (m_sprite)
return true;
	if (m_sprite)
	{
		m_sprite->DeleteChangesListener(this);
		gRM->scene2d->DeleteSprite(m_sprite->GetName());
		g->sprm.DeleteObject(m_sprite->GetID());
		m_sprite = NULL;
	}
	if(!create)
		return true;

	CComString name;
	name = "flash_";
	name += IntToStr(g->sprm.GetCount()).c_str();

	int spriteID = g->sprm.AddObject(name.GetBuffer(0));
	m_sprite = g->sprm.GetObjectPointer(spriteID);
	gRM->scene2d->AddSprite(spriteID, 0, 0);
//	m_sprite->SetVisibilityProvider(this);

	// IDynamicTexture* dynamicTexture = NULL;
	m_sprite->LoadFromDynamicTexture(m_flash->GetDynamicTexture());

	return true;
}

// загрузка нового ресурса
bool nrmFlash::SrcChanged()
{
	/*const wchar_t* pwcSrc = mpMLMedia->GetILoadable()->GetSrc();
	USES_CONVERSION;

	if(m_flash == NULL)
		m_flash = CreateFlashObject(this, m_pRenderManager->GetMainWindow(), gRM->resLib);

	if(m_flash == NULL)
		return false;

//	m_flash->SetParentWindow(m_pRenderManager->GetMainWindow());

	mlSize sz = mpMLMedia->GetIVisible()->GetSize();
	//m_sprite->SetSize(sz.width, sz.height);
	m_flash->SetSize(sz.width, sz.height);

	recreate_sprite();

	AbsDepthChanged();
	AbsXYChanged();
	AbsVisibleChanged();

	if (pwcSrc != NULL && *pwcSrc != L'\0'){
		m_flash->LoadMovie(pwcSrc);
	}else{
//		ValueChanged();
	}*/

	return true;
}

__forceinline omsresult HResult2OMSResult(HRESULT hr){
	if(FAILED(hr)){
		switch(hr){
		case E_NOINTERFACE: return OMS_ERROR_NOT_INITIALIZED;
		case E_INVALIDARG: return OMS_ERROR_INVALID_ARG;
		}
		return OMS_ERROR_FAILURE;
	}
	return OMS_OK;
}

void nrmFlash::onSetFocus(){
	if(!m_flash) return;
	m_flash->SetFocus();
}

void nrmFlash::onKillFocus(){
	if(!m_flash) return;
	m_flash->KillFocus();
}

void nrmFlash::onKeyDown(){
	if(!m_flash) return;
}

void nrmFlash::onKeyUp(){
	if(!m_flash) return;

}

void nrmFlash::AbsEnabledChanged(){
	/*BOOL	bEnabled = */GetMLMedia()->GetIButton()->GetAbsEnabled();
	// ??
}

void nrmFlash::AbsVisibleChanged()
{
	bool isVisible = GetMLMedia()->GetIVisible()->GetAbsVisible();
	if (m_sprite)
	{
#ifndef USE_VISIBLE_DESTINATION
		m_sprite->SetVisible(isVisible);
#else
		m_sprite->SetVisible(isVisible, 4);
#endif
	}
}

IDynamicTexture* nrmFlash::GetDynamicTexture(){
	if(!m_flash)
		return NULL;
	return m_flash->GetDynamicTexture();
}

void nrmFlash::setVariable(const wchar_t *var, const wchar_t *value){
	/*bool bRet = */m_flash->SetVariable(var, value);
	// ??
}

int nrmFlash::getScaleMode(){
	// ??
	return -1;
}

void nrmFlash::setScaleMode(int){
	// ??
}

int nrmFlash::getWMode(){
	// ??
	return -1;
}

void nrmFlash::setWMode(int){
	// ??
}

// возвращает длину данных
long nrmFlash::getVariable(const wchar_t *var, mlOutString &sRetVal){
	const wchar_t* pwcRetVal = m_flash->GetVariable(var);
	if(pwcRetVal != NULL)
		sRetVal = pwcRetVal;
	return 0;
}

long nrmFlash::callFunction(const wchar_t *request, mlOutString &sRetVal){
	const wchar_t* pwcRetVal = m_flash->CallFunction(request);
	if(pwcRetVal != NULL)
		sRetVal = pwcRetVal;
	return 0;
}

void nrmFlash::OnAdjustSize(int& aiWidth, int& aiHeight){
	// получены родные размеры флэшки, уточнить размеры текстуры, на которую она будет отрисовываться
	mlSize sz = mpMLMedia->GetIVisible()->GetSize();
	if(sz.width != 0 && sz.height != 0) // если заданы в скрипте, 
		return; // то не меняем
	sz.width = aiWidth;
	sz.height = aiHeight;
	mpMLMedia->GetIVisible()->SetSize(sz);
	SizeChanged(); // ?
}

void nrmFlash::onFSCommand(const wchar_t *command, const wchar_t *args){
	mpMLMedia->GetIFlash()->onFSCommand(command, args);
}