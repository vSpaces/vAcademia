// base.h: interface for the CRenderer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IBASE_H__D9939FA9_60F6_411C_8565_6CF8904939FD__INCLUDED_)
#define AFX_IBASE_H__D9939FA9_60F6_411C_8565_6CF8904939FD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// drawing primitives from VB
#define DRAW_FROM_VB		256

#define	RS_DEFAULT			0xFFFFFFFF

// color and alpha texture operators and stages
#define	CS_DIFFUSE_ST1		1
#define	CS_DIFFUSE_ST2		2
#define	CS_TEXTURE_ST1		4
#define	CS_TEXTURE_ST2		8
#define	BS_MODULATE			16
#define	BS_DISABLE			32
#define	BS_ARG1				64
#define	BS_ARG2				128
#define	CS_CURRENT_ST2		256
#define	CS_SPECULAR_ST1		512
#define	CS_SPECULAR_ST2		1024
#define	BS_ADD				2048
#define	CS_CURRENT_ST1		4096
#define	BS_TEXTUREALPHAPM	8192
#define	CS_TFACTOR2			16384
#define	BS_DOTPRODUCT3		32768

// lighting states
#define	LS_ON				1
#define	LS_OFF				2

// Z-buffer states
#define	ZBENABLE_ON			1
#define	ZBENABLE_OFF		2


// culling states
#define	CS_NONE				1
#define	CS_CW				2
#define	CS_CCW				3

// Material flags
#define	MF_SPECULARS		1
#define	MF_LIGHTING			2
#define	MF_ZENABLE			4

#define		TRANSFORM_WORLD        D3DTS_WORLD
#define	    TRANSFORM_VIEW         D3DTS_VIEW
#define	    TRANSFORM_PROJECTION   D3DTS_PROJECTION

#define		DEFAULT_CULLMODE		D3DCULL_CCW
#define		OPPOSITE_CULLMODE		D3DCULL_CW

#define		MULTIPASS_SHADER		true
#define		SINGLEPASS_SHADER		false

// alpha function
enum	ALPHAFUNC	{	AF_NEVER=D3DCMP_NEVER,AF_LESS=D3DCMP_LESS,AF_EQUAL=D3DCMP_EQUAL,AF_LESSEQUAL=D3DCMP_LESSEQUAL,
						AF_GREATER=D3DCMP_GREATER,AF_NOTEQUAL=D3DCMP_NOTEQUAL,AF_GREATEREQUAL=D3DCMP_GREATEREQUAL,
						AF_ALWAYS=D3DCMP_ALWAYS};

//////////////////////////////////////////////////////////////////////////
// Тип альфа-фильтрации
//////////////////////////////////////////////////////////////////////////
enum	ALPHABLEND
{
	// Неопределенное значение
	AB_UNDEFINED,
	// Выключено
	AB_DISABLED,
	// source*D3DBLEND_SRCCOLOR + dest*D3DBLEND_INVSRCCOLOR
	AB_SOURCEALPHA
};

//////////////////////////////////////////////////////////////////////////
// Direct3D интерфейсы
//////////////////////////////////////////////////////////////////////////
namespace	natura3d
{
	typedef	LPDIRECT3DDEVICE8			lprenderdevice;
	typedef	IDirect3DDevice8			renderdevice;
	typedef	D3DFORMAT					pxformat;
	typedef	D3DRENDERSTATETYPE			renderstatetype;
	typedef	D3DSTATEBLOCKTYPE			stateblocktype;
	typedef	ALPHAFUNC					alphafunc;
	typedef	LPDIRECT3DDEVICE8			lprenderdevice;
	typedef	IDirect3DVertexBuffer8		vertexbuffer;
	typedef	LPDIRECT3DVERTEXBUFFER8		lpvertexbuffer;
	typedef	IDirect3DIndexBuffer8		indexbuffer;
	typedef	LPDIRECT3DINDEXBUFFER8		lpindexbuffer;
	typedef	LPDIRECT3DTEXTURE8			lptexture;
	typedef	IDirect3DBaseTexture8		basetexture;
	typedef	LPDIRECT3DBASETEXTURE8		lpbasetexture;
	typedef	IDirect3DSurface8			imagesurface;
	typedef	LPDIRECT3DSURFACE8			lpimagesurface;
};

//////////////////////////////////////////////////////////////////////////
// Обработка ошибок
//////////////////////////////////////////////////////////////////////////
typedef	DWORD	n3dresult;

//////////////////////////////////////////////////////////////////////////
// Базовый интерфейс
//////////////////////////////////////////////////////////////////////////
#include "n3dguids.h"

namespace natura3d
{
	struct	isprite;
	struct	icamera;
	struct	iobject3d;
	struct	iloadable;
	struct	ivisibleobject;
	struct	icollisionobject;
	struct	igroup3d;
	struct	iplayable;
	struct	ibutton;
	struct	ilight;
	struct	ilensflare;
	struct	ishadow;
	struct	icontrolledobject;
	struct	icontroller;
	struct	ipluginsupport;
	struct	idatacontainer;
	struct	imotion;
	struct	ixml;

	// {AE1A84E3-A688-4635-BCF5-F270527F292D}
	N3DHDEFINE_GUID(iid_ibase, 
	0xae1a84e3, 0xa688, 0x4635, 0xbc, 0xf5, 0xf2, 0x70, 0x52, 0x7f, 0x29, 0x2d);

	struct	iframework;
	struct	ibase// : public ibaseinterface
	{
		//////////////////////////////////////////////////////////////////////////
		virtual	void	free_object() = 0;
		virtual	bool	query_interface(const N3DGUID& name, void** ppinterface) = 0;
		//////////////////////////////////////////////////////////////////////////
		virtual	DWORD		get_internal_type() = 0;
		virtual	iframework*	get_iframework() = 0;
	};
};

#endif // !defined(AFX_RENDERER_H__D9939FA9_60F6_411C_8565_6CF8904939FD__INCLUDED_)
