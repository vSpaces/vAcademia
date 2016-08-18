//////////////////////////////////////////////////////////////////////////
#pragma once

#include "n3ddefs.h"

struct _lod_config {
	int			level;
	int			distance;
	Cal3DComString	src;

	_lod_config()
	{
		level = -1;
		distance = -1;
	}

	_lod_config(const _lod_config& item)
	{
		*this = item;
	}

	void operator=(const _lod_config& item)
	{
		src = item.src;
		level = item.level;
		distance = item.distance;
	}
};

struct _material
{
	Cal3DComString src;
	Cal3DComString shader;
	_material(){};
	_material( const _material& m) : src(m.src), shader(m.shader){};
	void operator =( const _material& m)
	{
		this->src=m.src;
		this->shader=m.shader;
	};
};

struct _property
{
	Cal3DComString name;
	Cal3DComString value;
	int surfaceID;
	_property()
	{
		surfaceID = -1;
	};
	_property( const _property& m) : name(m.name), value(m.value), surfaceID(m.surfaceID)
	{
		surfaceID = -1;
	};
	void operator =( const _property& m)
	{
		this->name=m.name;
		this->value=m.value;
		this->surfaceID=m.surfaceID;
	};
};

struct SObjectCfg
{
	float scale;
	Cal3DComString skeleton_src;
	std::vector<Cal3DComString>	mesh_srcs;
	//Cal3DComString mesh_src;
	
	Cal3DComString shader;
	std::vector<_material> materials;
	std::vector<_property> properties;
	// Properties list:
	// - help filp_faces intangible, 
	// - pick link textures face animation priority mpegFAPU walkable unpickable touchable
	// - hidden gravity camera_ghost stencil_shadow projective_shadow alpharef
	
	Cal3DComString collision_type;
	Cal3DComString collision_src;
	std::vector<box3d> boxes;

	// Instances
	bool			is_reference;
	Cal3DComString		ref_name;
	CalVector		ref_translation;
	CalQuaternion	ref_rotation;
	CalVector		ref_scale;

	// Lods
	std::vector<_lod_config>	lods;

	SObjectCfg()
	{
		is_reference = false;
		ref_translation.set(0,0,0);
		ref_rotation.set(0,0,1,0);
		ref_scale.set(1,1,1);
	};

	SObjectCfg( const SObjectCfg& m)
	{
		this->scale = m.scale;
		this->skeleton_src = m.skeleton_src;
		//this->mesh_src = m.mesh_src;
		this->mesh_srcs.assign( m.mesh_srcs.begin(), m.mesh_srcs.end());
		this->shader = m.shader;
		this->materials = m.materials;
		this->properties = m.properties;
		this->collision_type = m.collision_type;
		this->collision_src = m.collision_src;
		this->boxes = m.boxes;
		this->is_reference = m.is_reference;
		this->ref_name = m.ref_name;
		this->ref_translation = m.ref_translation;
		this->ref_rotation = m.ref_rotation;
		this->ref_scale = m.ref_scale;
		this->lods.assign( m.lods.begin(), m.lods.end());
	};
};

//////////////////////////////////////////////////////////////////////////