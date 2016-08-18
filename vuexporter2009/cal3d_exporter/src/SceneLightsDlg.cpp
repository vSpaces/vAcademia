// SceneLightsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "..\cal3d_max_exporter\cal3d_max_exporter.h"
#include "scenelightsdlg.h"
#include "maxlight.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSceneLightsDlg dialog
#define Point32CREF(a)	(0x00000000 + ((int)(a.x * 255)<<16) + ((int)(a.y * 255)<<8) + (a.z * 255))
#define Point32CREFINT(a,b)	(0x00000000 + ((int)(a.x * 255 * b)<<16) + ((int)(a.y * 255 * b)<<8) + (a.z * b * 255))

CSceneLightsDlg::CSceneLightsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSceneLightsDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSceneLightsDlg)
	m_Size = 0.0;
	m_Distance = 0.0;
	//}}AFX_DATA_INIT
}


void CSceneLightsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSceneLightsDlg)
	DDX_Control(pDX, IDC_LIST1, m_LightsList);
	DDX_Text(pDX, IDC_EDIT2, m_Size);
	DDV_MinMaxDouble(pDX, m_Size, 0.1, 1000000.);
	DDX_Text(pDX, IDC_EDIT3, m_Distance);
	DDV_MinMaxDouble(pDX, m_Distance, 0., 10000000.);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSceneLightsDlg, CDialog)
	//{{AFX_MSG_MAP(CSceneLightsDlg)
	ON_BN_CLICKED(IDOK, OnSet)
	ON_WM_DRAWITEM()
	ON_LBN_SELCHANGE(IDC_LIST1, OnSelchangeList1)
	ON_BN_CLICKED(IDC_BUTTON1, OnButton1)
	ON_BN_CLICKED(IDC_BUTTON4, OnButton4)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSceneLightsDlg message handlers

BOOL CSceneLightsDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	for (int i=0;i<lights->size();i++)
	{
		m_LightsList.AddString((*lights)[i]->GetName().c_str());
	}
	((CEdit*)GetDlgItem(IDC_EDIT4))->SetWindowText("Omni01.lgx");
	((CEdit*)GetDlgItem(IDC_EDIT2))->SetWindowText("100");
	((CEdit*)GetDlgItem(IDC_EDIT3))->SetWindowText("1000");
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CSceneLightsDlg::OnSet() 
{
	// TODO: Add your control notification handler code here
	UpdateData();
	int* indices;
	int sc = get_sel_indices(&indices);

	Cal3DComString flare_name;
	((CEdit*)GetDlgItem(IDC_EDIT1))->GetLine(0, flare_name.GetBufferSetLength(255));

	bool on = !flare_name.IsEmpty();
	for (int i=0; i<sc; i++)
	{
		int index = indices[i];
		CMaxLight* light = (*lights)[index];
		CalExporter_Flare*	fl = &light->flare;
		if (on)
		{
			fl->enabled = true;
			fl->fname = flare_name;
			fl->size = m_Size;
			fl->dist = m_Distance;
		}else
		{
			fl->enabled = false;
		}
	}
	flare_name.ReleaseBuffer();

	free( indices);
}

int CSceneLightsDlg::get_sel_indices(int **pi, bool show_ex)
{
	CListBox*	lb = (CListBox*)GetDlgItem(IDC_LIST1);
	int sc = lb->GetSelCount();

	if(sc <= 0)
	{
		if( show_ex)	MessageBox("Select at least one light");
		return 0;
	}
	int* indices = (int*)malloc(sc*sizeof(int));
	lb->GetSelItems(sc, indices);

	*pi = indices;
}

void CSceneLightsDlg::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	// TODO: Add your message handler code here and/or call default
	if( nIDCtl==IDC_BUTTON2 || nIDCtl==IDC_BUTTON3)
	{
		LOGBRUSH	lb;
		lb.lbStyle = BS_SOLID;

		int* inds;
		if ( get_sel_indices(&inds) == 0)	lb.lbColor = 0xFFC0C0C0;
		else
		{	CMaxLight* light = (*lights)[inds[0]];
			Point3 p = light->GetDiffuse();
			if( nIDCtl==IDC_BUTTON2)	lb.lbColor = Point32CREF(p);
			else if( nIDCtl==IDC_BUTTON3)	lb.lbColor = Point32CREFINT(p, light->GetIntensity());
			free( inds);
		}
		HBRUSH	newb;
		newb = CreateBrushIndirect( &lb);
		FillRect( lpDrawItemStruct->hDC, (CONST RECT *) &lpDrawItemStruct->rcItem, newb);
		DeleteObject(newb);
		return;
	}	
	CDialog::OnDrawItem(nIDCtl, lpDrawItemStruct);
}

void CSceneLightsDlg::OnSelchangeList1() 
{
	// TODO: Add your control notification handler code here
	UpdateLightProperties();
}

void CSceneLightsDlg::UpdateLightProperties()
{
	CEdit* flare_name = (CEdit*)GetDlgItem(IDC_EDIT1);

	int* inds;
	int count;
	if ( (count=get_sel_indices(&inds)) != 0)
	{
		CMaxLight*	light = (*lights)[inds[0]];
		flare_name->SetWindowText(light->flare.fname.GetBuffer());
		free(inds);
	}

	Invalidate();
}

void CSceneLightsDlg::OnButton1() 
{
	// TODO: Add your control notification handler code here
	CString filter = "PNG (*.png)|*.png|Bitmaps (*.bmp)|*.bmp||";
	CFileDialog	fd(TRUE,NULL,NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,filter);
	if( fd.DoModal())
	{
	//	house->load(fd.GetPathName());		
		((CEdit*)GetDlgItem(IDC_EDIT1))->SetWindowText(fd.GetPathName());
	}
}

void CSceneLightsDlg::OnButton4() 
{
	// TODO: Add your control notification handler code here
	CString filter = "Lights (*.lgx)|*.lgx||";
	CFileDialog	fd(FALSE,NULL,NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,filter);
	if( fd.DoModal())
	{
		((CEdit*)GetDlgItem(IDC_EDIT4))->SetWindowText(fd.GetPathName());
	}	
}

#define WRITE_LIGHT_TYPE(a,b)	{ a.write("type=",5);	\
	int lt = b->GetLightType();	\
	if( lt == 1)	a.write("point\n", 6);	\
	}

#define WRITE_STR(a,b){ a.write(b,strlen(b));}

#define WRITE_BR(a){ a.write("\n",1);}

#define WRITE_FL(a,b){	\
	char ch[25]; ZeroMemory(&ch, 25);	\
	sprintf((char*)&ch, "%.4f", b);		\
	WRITE_STR(a, (char*)&ch)			\
}

#define WRITE_DIFFUSE(a,b)	{	\
	WRITE_STR(a,"difr=")	WRITE_FL(a,b.x)	WRITE_BR(a)\
	WRITE_STR(a,"difg=")	WRITE_FL(a,b.y)	WRITE_BR(a)\
	WRITE_STR(a,"difb=")	WRITE_FL(a,b.z)	WRITE_BR(a)\
}

#define WRITE_DIFFUSE_COLOR(a,b)	{	\
	Point3 color = b->GetDiffuse();	\
	WRITE_DIFFUSE(a,color)	\
}

#define WRITE_SPEC_COLOR(a,b)	{	\
	Point3 color = b->GetDiffuse();	\
	WRITE_STR(a,"specr=")	WRITE_FL(a,color.x)	WRITE_BR(a)\
	WRITE_STR(a,"specg=")	WRITE_FL(a,color.y)	WRITE_BR(a)\
	WRITE_STR(a,"specb=")	WRITE_FL(a,color.z)	WRITE_BR(a)\
}

bool CSceneLightsDlg::SaveLights()
{
	Cal3DComString filename;
	((CEdit*)GetDlgItem(IDC_EDIT4))->GetLine(0, filename.GetBufferSetLength(255));
	// open the file
	std::ofstream file;
	file.open(filename.GetString(), std::ios::out | std::ios::binary);
	if(!file)	return false;

	WRITE_STR(file, "type=ambient")	WRITE_BR(file)
	WRITE_DIFFUSE(file, ambient)

	for (int i=0; i<lights->size(); i++) {
		CMaxLight*	light = (*lights)[i];
		
		WRITE_LIGHT_TYPE(file, light)
		WRITE_DIFFUSE_COLOR(file, light)
		WRITE_SPEC_COLOR(file, light)
		if( light->GetUseAtten())
		{
			WRITE_STR(file, "radiusStart=")	WRITE_FL(file, light->GetStartAtten())	WRITE_BR(file)
			WRITE_STR(file, "radiusEnd=")	WRITE_FL(file, light->GetStartAtten())	WRITE_BR(file)
		}
		if (!light->flare.fname.IsEmpty())
		{
			WRITE_STR(file, "flare=single")	WRITE_BR(file)
			WRITE_STR(file, "flare_t=")	WRITE_STR(file, light->flare.fname.GetBuffer())	WRITE_BR(file)
			WRITE_STR(file, "flare_max_dist=")	WRITE_FL(file, light->flare.dist)	WRITE_BR(file)
			WRITE_STR(file, "flare_max_size=")	WRITE_FL(file, light->flare.size)	WRITE_BR(file)
		}
	}
	file.close();
	return true;
}

void CSceneLightsDlg::OnCancel() 
{
	// TODO: Add extra cleanup here
	if( SaveLights())	CDialog::OnOK();
}
