// nrmEdit.cpp: implementation of the nrmEdit class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "nrmEdit.h"
//#include "n3dpluginsupport.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

nrmEdit::nrmEdit(mlMedia* apMLMedia) : nrmObject(apMLMedia)
{
}

void	nrmEdit::OnSetRenderManager()
{
	_in_ValueChanged = false;

	assert( mpMLMedia->GetIVisible());

	AbsDepthChanged();
	AbsXYChanged();
	AbsVisibleChanged();
	FrozenChanged();
	TabGroupChanged();
	TabIndexChanged();
	MultilineChanged();
	PasswordChanged();
}

nrmEdit::~nrmEdit()
{
}


bool	nrmEdit::SrcChanged()
{
	return true;
}

// реализация  moIVisible
void	nrmEdit::AbsXYChanged()
{
}

void	nrmEdit::SizeChanged()
{
		}

void	nrmEdit::AbsDepthChanged()
{
}

void	nrmEdit::AbsVisibleChanged()
{
}

void	nrmEdit::AbsOpacityChanged()
{
}

mlSize	nrmEdit::GetMediaSize()
{
	mlSize size;
	ML_INIT_SIZE(size);
	return size;
}

mlRect	nrmEdit::GetCropRect()
{
	assert( FALSE);
	mlRect rect;
	ML_INIT_RECT(rect);
	return rect;
}

// -1 - not handled, 0 - point is out, 1 - point is in
int	nrmEdit::IsPointIn(const mlPoint /*aPnt*/)
{
	//assert( FALSE);
	// ??
	return 1; 
}

// реализация  moIButton
void	nrmEdit::AbsEnabledChanged()
{
}

void	nrmEdit::onKeyDown()
{
}

void	nrmEdit::onKeyUp()
{
}

void	nrmEdit::onSetFocus()
{
}

void	nrmEdit::onKillFocus()
{
}

// реализация  moIInput
void	nrmEdit::ValueChanged()
{
	if (_in_ValueChanged)
	  return;
}

void	nrmEdit::StylesChanged()
{
	}

void nrmEdit::PasswordChanged()
	{
	}

void nrmEdit::MultilineChanged()
	{
	}

void nrmEdit::TabGroupChanged()
{
		}
 
bool nrmEdit::InsertText(const wchar_t* /*apwcText*/, int /*aiPos*/)
{
	return true;
}

void nrmEdit::TabIndexChanged()
{
		}

const wchar_t*	nrmEdit::GetValue() const
{
	return L"";
}

void nrmEdit::FrozenChanged()
{
}