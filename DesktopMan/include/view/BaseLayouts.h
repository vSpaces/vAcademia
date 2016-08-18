#pragma once
#include "DialogLayout.h"

#define IMPLEMENT_LAYOUTS()	SetupVirtualLayout( containers, pCurrentContainer);
#define BEGIN_VIRTUAL_LAYOUT_MAP() \
	virtual void SetupVirtualLayout(CAtlList<CLayoutContainer*>& containers, CLayoutContainer* pCurrentContainer) \
	{ 
#define END_VIRTUAL_LAYOUT_MAP() \
	}

#define DECLARE_VIRTUAL_LAYOUT_MAP() \
	virtual void SetupVirtualLayout(CAtlList<CLayoutContainer*>& containers, CLayoutContainer* pCurrentContainer);

#define BEGIN_VIRTUAL_LAYOUT_MAP_CPP(x) \
void x::SetupVirtualLayout(CAtlList<CLayoutContainer*>& containers, CLayoutContainer* pCurrentContainer) {


class CBaseLayoutT 
{
	public:
		CBaseLayoutT (){}
		~CBaseLayoutT (){}

		virtual void Apply( ){};
		virtual void Bind( HWND a_hWnd){};
		virtual void UnBind( ){}
		virtual void UpdateLayout( ){}
};

template <class T>
class CBaseLayout : 
		public CWindowImpl<CBaseLayoutT, ATL::CWindow>,
		public CDialogLayout<T>,
		public CBaseLayoutT
{
public:
	CBaseLayout(void)
	{
		m_bGripper = FALSE;
	}
	~CBaseLayout(void){}

	BEGIN_MSG_MAP(CBaseLayout)
		CHAIN_MSG_MAP(CDialogLayout<T>)
	END_MSG_MAP()

	virtual void Apply()
	{
		if( m_pRootContainer)
			MP_DELETE( m_pRootContainer);
		MP_NEW_V( m_pRootContainer, CLayoutContainer, m_Tmpl);
		SetupLayout();
	}

	virtual void Bind( HWND a_hWnd)
	{
		SubclassWindow( a_hWnd);
	}

	virtual void UnBind( )
	{
		UnsubclassWindow( );
	}

	virtual void UpdateLayout( )
	{
		RECT rcBounds;
		GetWindowRect(&rcBounds);
		//SetWindowPos( 0, 0,0,rcBounds.right-rcBounds.left,rcBounds.bottom-rcBounds.top,SWP_NOMOVE);
		SendMessage( WM_SIZE, 0, MAKELONG(rcBounds.right-rcBounds.left,rcBounds.bottom-rcBounds.top));
		::InvalidateRect(m_hWnd, NULL, TRUE);
		UpdateWindow();
	}

	virtual void OnFinalMessage(HWND /*hWnd*/) = 0;

public:

	BEGIN_LAYOUT_MAP()
		IMPLEMENT_LAYOUTS()
	END_LAYOUT_MAP()

	BEGIN_VIRTUAL_LAYOUT_MAP()
	END_VIRTUAL_LAYOUT_MAP()
};


