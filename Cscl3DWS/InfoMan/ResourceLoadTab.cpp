#include "StdAfx.h"
#include "ResourceLoadTab.h"
#include "LogTab.h"
#include "IDesktopMan.h"
#include "..\oms\oms_context.h"
#include "rm.h"
#include ".\resourceloadtab.h"

CResourceLoadTab::CResourceLoadTab()
:MP_VECTOR_INIT(mResPoint)
{
}


void CResourceLoadTab::SetContext( oms::omsContext *apContext)
{
	mpOmsContext= apContext;
	currentSession = NULL;
	m_building = false;
}

LRESULT CResourceLoadTab::OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	SetDownloadResourceText();
	RedrawWindow( NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
	return 0;
}

LRESULT CResourceLoadTab::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	ModifyStyleEx(0, WS_EX_CLIENTEDGE);
	SetWindowPos( 0, &rcDefault, SWP_NOSIZE | SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_HIDEWINDOW);

	CButton checkFiltr( GetDlgItem( IDC_RESOURCE_CHECK));
	checkFiltr.SetCheck(false);
	mWorkFlag = false;
	mEditResourcesLoadingLog.Attach( GetDlgItem( IDC_EDIT_OUTPUT));

	mBitmap.LoadBitmap(IDB_IMAP);

	mMapWidth = 1850;
	mMapHeight = 1704;

	rd_x = -118.984 - 180;
	rd_y = 55987.703 + 150;
	lu_x = -102598.852 - 180;
	lu_y = -38448.883 + 150;

/*SM: locallu.x = -102778.852
SM: locallu.u = -38298.883
SM: koefXToMap = -0.01805232614077918
SM: koefYToMap = -0.018043854317224044
SM: koefXToMap = 1850
SM: koefYToMap = 1704*/

	koefXToMap = (float) mMapWidth / (lu_x - rd_x);
	koefYToMap = (float) mMapHeight / (lu_y - rd_y);

	m_building = false;

	bHandled = FALSE;
	return 0;
}

LRESULT CResourceLoadTab::OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	CPaintDC	paintDC( m_hWnd);

	CSize bmSize;
	mBitmap.GetBitmapDimension( &bmSize);
	bmSize.cx = 400;
	bmSize.cy = 400;

	CDC pDCMemory;
	pDCMemory.CreateCompatibleDC(paintDC);
	CBitmap bmp;
	bmp.CreateCompatibleBitmap(paintDC, bmSize.cx, bmSize.cy);
	pDCMemory.SelectBitmap((HBITMAP)bmp);

	CPoint origin(0,0);
	pDCMemory.DrawState( origin, bmSize, (HBITMAP)mBitmap, 0);
	
	if (mResPoint.size() > 0)
	{
		for (int i = 0; i < mResPoint.size(); i++)
		{
			float ax = 16 + (mResPoint[i].sPoint.x - (-86713))/((-29036) - (-86713)) * (379.0f - 16.0f);
			float ay = 91 + (mResPoint[i].sPoint.y - (32659))/((-12286) - (32659)) * (373.0f - 91.0f);
			CPoint resPoint((int)ax,(int)ay);

			DWORD color = RGB(255,0,0);
			switch( mResPoint[i].type)
			{
			case 1:	//bmf
				color = RGB(255,255,0);
				break;
			case 2:	//text
				color = RGB(0,255,0);
				break;
			case 3:	//sound
				color = RGB(0,0,255);
				break;
			}

			pDCMemory.SetPixel((int)resPoint.x, (int)resPoint.y, color);
			pDCMemory.SetPixel((int)resPoint.x, (int)resPoint.y-1, color);
			pDCMemory.SetPixel((int)resPoint.x+1, (int)resPoint.y, color);
			pDCMemory.SetPixel((int)resPoint.x+1, (int)resPoint.y-1, color);

			if( mResPoint[i].type != 4)
			{
				CString tmp;
				tmp.Format("%i", mResPoint[i].id);
				pDCMemory.TextOut((int)resPoint.x, (int)resPoint.y - 20, tmp);
			}
		}
	}


	paintDC.BitBlt(0, 0, bmSize.cx, bmSize.cy, pDCMemory, 0, 0, SRCCOPY);
	return 0;
}

LRESULT CResourceLoadTab::OnBnClickedCheckResourceLoad(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	if (IsDlgButtonChecked(IDC_RESOURCE_CHECK) == BST_CHECKED)
	{
		SetTimer( 0, 100, NULL);
	}
	else
	{
		KillTimer(0);
	}
	return 0;
}

void CResourceLoadTab::SetDownloadResourceText()
{
	CString str = "";
	CString tmp;
	mResPoint.clear();

	int downloadThreadCount = mpOmsContext->mpResLoadingStat->GetResourcesLoadInfo(&mpResLoadStruct);

	float x, y, z;
	mpOmsContext->mpRM->GetLoadingResourcesCoords( mpResLoadStruct, downloadThreadCount, &x, &y, &z);

	long totalToLoad = 0;
	long totalLoaded = 0;
	for (int i = 0; i < downloadThreadCount; i++)
	{
		tmp.Format("%i", i+1);
		str += tmp + ".";
		tmp.Format("[%2d", mpResLoadStruct[i].loadedPercent);
		str += tmp + "%]\t";
		str += (CString) mpResLoadStruct[i].filename + "\t";
		tmp.Format("%2d", mpResLoadStruct[i].fileSize);
		str += tmp + "\r\n";

		totalToLoad += mpResLoadStruct[i].fileSize;
		totalLoaded += (mpResLoadStruct[i].fileSize * mpResLoadStruct[i].loadedPercent / 100);

		for( int j=0; j<mpResLoadStruct[i].coordsSize; j++)
		{
			viewResOnBitmap myStruct;
			myStruct.type = 0;
			if( mpResLoadStruct[i].pIAsynchResource)
			{
				switch (mpResLoadStruct[i].pIAsynchResource->GetMainPriority())
				{
				case RES_TYPE_BMF: // он же RES_TYPE_MESH, он же RES_TYPE_SKELETON
					myStruct.type = 1;
					break;

				case RES_TYPE_TEXTURE:
					myStruct.type = 2;
					break;

				case RES_TYPE_SOUND:
					myStruct.type = 4;
					break;
				}
			}
			myStruct.id = i+1;
			myStruct.sPoint.x = mpResLoadStruct[i].x[j];
			myStruct.sPoint.y = mpResLoadStruct[i].y[j];
			mResPoint.push_back(myStruct);
		}
	}
	str += tmp + "\r\n";
	tmp.Format("Loaded %2d of %2d", totalLoaded, totalToLoad);
	str += tmp;

	viewResOnBitmap myStruct;
	myStruct.type = 4;
	myStruct.sPoint.x = x;
	myStruct.sPoint.y = y;
	mResPoint.push_back(myStruct);

	mEditResourcesLoadingLog.SetWindowText(str);
}

__declspec(dllexport) void ShowDC()
{
	CResourceLoadTab mCResourceLoadTab;
	mCResourceLoadTab.DoModal();

}