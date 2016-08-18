// nrmLine.cpp: implementation of the nrmLine class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "nrmLine.h"

nrmLine::nrmLine(mlMedia* apMLMedia): 
	nrmObject(apMLMedia),
	m_line(NULL)
{
	m_line = MP_NEW(CLine);	
	gRM->scene2d->AddLine(m_line, 0, 0);

	AbsVisibleChanged();
	AbsOpacityChanged();
	AbsXYChanged();
	AbsDepthChanged();
	XY2Changed();
	ColorChanged();
	ThicknessChanged();
}

// изменилось абсолютное положение 
void nrmLine::AbsXYChanged()
{
	if ((m_line) && (GetMLMedia()))
	{
		mlPoint pnt = mpMLMedia->GetIVisible()->GetAbsXY();
		m_line->SetCoords(pnt.x, pnt.y);
		mlPoint xy2 = mpMLMedia->GetILine()->GetAbsXY2();
		m_line->SetSecondaryCoords(xy2.x, xy2.y);	
	}
} 

void nrmLine::SizeChanged()
{
	XY2Changed();
}

// изменилась абсолютная глубина
void nrmLine::AbsDepthChanged()
{
	if ((m_line) && (GetMLMedia()))
	{
		m_line->SetDepth(GetMLMedia()->GetIVisible()->GetAbsDepth());
	}
}

// изменилась абсолютная видимость
void nrmLine::AbsVisibleChanged()
{
	if ((m_line) && (GetMLMedia()))
	{
		bool isVisible = GetMLMedia()->GetIVisible()->GetAbsVisible();
		m_line->SetVisible(isVisible);
	}
}

void nrmLine::AbsOpacityChanged()
{
	if ((m_line) && (mpMLMedia))
	{
		float op = mpMLMedia->GetIVisible()->GetAbsOpacity();
		m_line->SetAlpha((unsigned char)(op * 255.0f));
	}
}

// out: -1 - not handled, 0 - point is out, 1 - point is in
int nrmLine::IsPointIn(const mlPoint aPnt)
{	
	return 0;
}

void nrmLine::XY2Changed()
{
	if ((m_line) && (mpMLMedia))
	{
		mlPoint xy2 = mpMLMedia->GetILine()->GetAbsXY2();
		m_line->SetSecondaryCoords(xy2.x, xy2.y);	
	}
}

void nrmLine::ColorChanged()
{
	if ((m_line) && (mpMLMedia))
	{
		mlColor color = mpMLMedia->GetILine()->GetColor();
		m_line->SetColor(color.r, color.g, color.b);
	}
}

void nrmLine::ThicknessChanged()
{
	if ((m_line) && (mpMLMedia))
	{
		int thickness = mpMLMedia->GetILine()->GetThickness();
		m_line->SetLineWidth(thickness);
	}
}

nrmLine::~nrmLine()
{
	gRM->scene2d->DeleteLine(m_line);
}
