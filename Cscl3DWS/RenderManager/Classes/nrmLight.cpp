// nrmLight.cpp: implementation of the nrmLight class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "nrmLight.h"
//#include "3dloader.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
//#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

nrmLight::nrmLight(mlMedia* apMLMedia) : nrmObject(apMLMedia)
{
	//lights.reserve(50);
}

nrmLight::~nrmLight()
{
	/*if( get_ilightslib())
	{
		for(int i=0; i<lights.size(); i++)
		{
			get_ilightslib()->remove_light(lights[i]);
		}
	}
	lights.clear();*/
}

// загрузка нового ресурса
bool nrmLight::SrcChanged()
{
	/*USES_CONVERSION;
	// 1. проверить был ли до этого создан буфер вершин
	// 2. проверить, добавлен ли этот объект в сцену
	const wchar_t* pwcSrc=mpMLMedia->GetILoadable()->GetSrc();
	if(!pwcSrc) return false;
	mlString sSRC=pwcSrc; 
	if(sSRC.empty()) return false;
	//
	mlILoadable* pMLILdbl=mpMLMedia->GetILoadable();
	mlI3DObject* pMLI3DO=mpMLMedia->GetI3DObject();
	mlMedia* pMLMScene3D=pMLI3DO->GetScene3D();
	if( !pMLMScene3D)
	{
		rm_trace("Error: Lights is outside Scene3D: %s\n", CComString((LPWSTR)sSRC.c_str()).GetBuffer());
		return false;
	}
	//
	iscene*	pScene = get_iscenelib()->get_ptr_scene(pMLMScene3D);
	//
	std::vector<int>	alights;
	alights.reserve(256);
	if( !get_ilightslib()->load_lights((LPCTSTR)W2A(pwcSrc), alights))
	{
		//get_ilightslib()->free_lights(alights);
		rm_trace("Error: Can`t load Lights : %s\n", CComString((LPWSTR)sSRC.c_str()).GetBuffer());
		return false;
	}

	if( alights.size() == 0)
	{
		rm_trace("Error: No lights in : %s\n", CComString((LPWSTR)sSRC.c_str()).GetBuffer());
		return false;
	}

	std::vector<int>::iterator	alit = alights.begin();
	std::vector<int>::iterator	aend = alights.end();
	for(;alit!=aend;++alit)
	{
		lights.push_back(get_ilightslib()->get_light(*alit));
	}

	//lights.assign(alights.begin(), alights.end());
	//get_ilightslib()->free_lights(alights);

	if( pMLMScene3D)
	{
		iscene*	pScene = get_iscenelib()->get_ptr_scene(pMLMScene3D);
		if( pScene)
		{
			std::vector<ilight*>::iterator	lit = lights.begin();
			std::vector<ilight*>::iterator	end = lights.end();
			for( ; lit!=end; ++lit)
			{
				pScene->add_light(*lit);
			}
			//pScene->add_light_from_vector(get_lights());
		}
		else
		{
			rm_trace("Error: Scene3D is undefined for light: %s\n", CComString((LPWSTR)sSRC.c_str()).GetBuffer());
			return false;
		}
	}*/

	return true;
}

// реализация moI3DObject
ml3DPosition nrmLight::GetPos(){
	ml3DPosition pos;
	ML_INIT_3DPOSITION(pos);
	// ??
	return pos;
}

void nrmLight::PosChanged(ml3DPosition &/*pos*/){
	// ??
}

void nrmLight::RotationChanged(ml3DRotation &/*rot*/){
	// ??
}

ml3DScale nrmLight::GetScale(){
	ml3DScale scl;
	ML_INIT_3DSCALE(scl);
	// ??
	return scl;
}

void nrmLight::ScaleChanged(ml3DScale &/*scl*/){
}

ml3DRotation nrmLight::GetRotation(){
	ml3DRotation rot;
	ML_INIT_3DROTATION(rot);
	// ??
	return rot;
}

void nrmLight::VisibleChanged(){}; // изменилась видимость

// изменился флаг проверки на пересечения
void nrmLight::CheckCollisionsChanged(){
	GetMLMedia()->GetI3DObject()->GetCheckCollisions();
} 

int nrmLight::moveTo(ml3DPosition /*aPos3D*/, int /*aiDuaration*/, bool /*abCheckCollisions*/){
	// ??
	return -1;
}

ml3DPosition nrmLight::getAbsolutePosition()
{
	ml3DPosition pos; ML_INIT_3DPOSITION(pos);
	ATLASSERT( FALSE);
	return pos;
}

ml3DRotation nrmLight::getAbsoluteRotation()
{
	ml3DRotation rot; ML_INIT_3DROTATION(rot);
	ATLASSERT( FALSE);
	return rot;
}
