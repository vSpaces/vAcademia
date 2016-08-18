#pragma once

#include "./BaseLayouts.h"

class CConfirmWebCamLayout : 
		public CBaseLayout< CConfirmWebCamLayout>
{
public:
	CConfirmWebCamLayout(void){}
	~CConfirmWebCamLayout(void){}
	virtual void OnFinalMessage(HWND /*hWnd*/)
	{
		MP_DELETE_THIS;
	}

public:
	enum { IDD = IDD_CONFIRMWEBCAMDIALOG };
	//SIDE BY SIDE layout map
	DECLARE_VIRTUAL_LAYOUT_MAP()
};

class CConfirmLayout : 
	public CBaseLayout< CConfirmLayout>
{
public:
	CConfirmLayout(void){}
	~CConfirmLayout(void){}
	virtual void OnFinalMessage(HWND /*hWnd*/)
	{
		MP_DELETE_THIS;
	}

public:
	enum { IDD = IDD_CONFIRM_DIALOG };
	//ONE AT A TIME layout map
	DECLARE_VIRTUAL_LAYOUT_MAP()
};

class CConfirmTextureBoardLayout : 
	public CBaseLayout< CConfirmTextureBoardLayout>
{
public:
	CConfirmTextureBoardLayout(void){}
	~CConfirmTextureBoardLayout(void){}
	virtual void OnFinalMessage(HWND /*hWnd*/)
	{
		MP_DELETE_THIS;
	}

public:
	enum { IDD = IDD_CONFIRM_TEXTURE_DIALOG };
	//SIDE BY SIDE layout map
	DECLARE_VIRTUAL_LAYOUT_MAP()
};

