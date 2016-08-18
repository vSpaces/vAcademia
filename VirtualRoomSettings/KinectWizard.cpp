// KinectWizard.cpp : Implementation of CKinectWizard

#include "stdafx.h"
#include "IKinect.h"
#include "KinectWizard.h"
#include ".\kinectwizard.h"
#include "MonitorDrawer.h"
#include "showImageCodes.h"
#include "SaveSettings.h"
#include <sstream>
#include "Time.h"
#define _USE_MATH_DEFINES
#include <math.h>

#include <gdiplus.h>
using namespace Gdiplus;


//#define KINECT_DISABLED		1

template <typename T> std::string ToStr(const T& t) { std::ostringstream os; os<<t; return os.str(); } 

/* перенес в Time.h
#define MIN_REDRAW_TIME		100
#define TIME_FOR_HANDS		30
#define END_RECOGNIZE_TIME	-1
*/ 

IKinect* g_kinect = NULL;


typedef enum _SKELETON_POSITION_INDEX
{
	SKELETON_POSITION_HIP_CENTER = 0,
	SKELETON_POSITION_SPINE,
	SKELETON_POSITION_SHOULDER_CENTER,
	SKELETON_POSITION_HEAD,
	SKELETON_POSITION_SHOULDER_LEFT,
	SKELETON_POSITION_ELBOW_LEFT,
	SKELETON_POSITION_WRIST_LEFT,
	SKELETON_POSITION_HAND_LEFT,
	SKELETON_POSITION_SHOULDER_RIGHT,
	SKELETON_POSITION_ELBOW_RIGHT,
	SKELETON_POSITION_WRIST_RIGHT,
	SKELETON_POSITION_HAND_RIGHT,
	SKELETON_POSITION_HIP_LEFT,
	SKELETON_POSITION_KNEE_LEFT,
	SKELETON_POSITION_ANKLE_LEFT,
	SKELETON_POSITION_FOOT_LEFT,
	SKELETON_POSITION_HIP_RIGHT,
	SKELETON_POSITION_KNEE_RIGHT,
	SKELETON_POSITION_ANKLE_RIGHT,
	SKELETON_POSITION_FOOT_RIGHT,
	SKELETON_POSITION_COUNT
} SKELETON_POSITION_INDEX;

void CKinectWizard::OnKinectDLLNotFound()
{
	MessageBox("kinect_plugin.dll not found", "Error");
	EndDialog(0);
}

void CKinectWizard::OnKinectNotFound()
{
	MessageBox("Kinect device not found", "Error");
	EndDialog(0);
}

LRESULT CKinectWizard::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	CAxDialogImpl<CKinectWizard>::OnInitDialog(uMsg, wParam, lParam, bHandled);

	this->SetTimer(2, (UINT)(MIN_REDRAW_TIME), NULL);

#ifndef KINECT_DISABLED
	if (!g_kinect)
	{
		m_hDll = ::LoadLibrary("kinect_plugin.dll");

		if (m_hDll == NULL)
		{
			OnKinectDLLNotFound();
			return 1;
		}

		IKinect* (*CreateKinectObject)();
		(FARPROC&)CreateKinectObject = GetProcAddress(m_hDll, "CreateKinectObject");
		if( CreateKinectObject == NULL)
		{
			OnKinectDLLNotFound();			
		}
		else
		{
			g_kinect = CreateKinectObject();
			if (!g_kinect->Init())
			{
				OnKinectNotFound();				
			}
			else
			{
				g_kinect->EnableVideo(true);
			}
		}

		SetWizardStep(1);
	}
#else
	g_kinect = NULL;
	SetWizardStep(1);
#endif

	m_videoData = NULL;
	m_depthData = NULL;

	CWindow wnd = GetDlgItem(IDC_VIDEO);

	m_lastPaintTime = GetTickCount() - MIN_REDRAW_TIME - 1;

	void* pBits = 0;
	ZeroMemory(&m_info, sizeof(BITMAPINFO));
	m_info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	m_info.bmiHeader.biWidth = 640;
	m_info.bmiHeader.biHeight = -480;
	m_info.bmiHeader.biPlanes = 1;
	m_info.bmiHeader.biBitCount = 32;
	m_info.bmiHeader.biCompression = BI_RGB;

	m_hBitmap = ::CreateDIBSection(GetDC(), &m_info, DIB_RGB_COLORS,(void **)&pBits, NULL, 0);

	m_hdcMem = CreateCompatibleDC(wnd.GetDC());
	SelectObject(m_hdcMem, m_hBitmap);

	CWindow wnd2 = GetDlgItem(IDC_KINECT_HINT);
	m_hdcMem2 = CreateCompatibleDC(wnd2.GetDC());   

	Invalidate();

	return 1;  // Let the system set the focus
}

void CKinectWizard::SetWizardStep(int step)
{
	HWND hWnd = WindowFromDC(GetDC());
	std::string title = "Мастер настройки Kinect. Шаг 1";
	title[title.size() - 1] = title[title.size() - 1] + (step - 1);
	SetWindowText(title.c_str());

	m_step = step;
	if (1 == step)
	{		
		m_timeForHands = TIME_FOR_HANDS;
		m_isRepeatRecognition = false;

		SetDlgItemText(IDOK, "Далее");
		SetDlgItemText(IDCANCEL, "Отмена");
		SetDlgItemText(IDC_WIZARD_TEXT, "Расположите устройство Kinect так, чтобы в его обзор полностью попадала интерактивная доска и 1-2 метра пространства перед ней (как показано на схематичном рисунке-примере). Устройство Kinect должно стоять параллельно плоскости доски.");		

		if (!m_img.IsNull())
		{
			m_img.Destroy();
		}

		m_img.Load("Images/kinect_hint1.jpg");
	}
	else if (2 == step)
	{
		m_time = GetTickCount();
		if (m_isRepeatRecognition)
			m_timeForHands = TIME_FOR_HANDS / 3;

		unsigned int timerID = 1;				
		SetDlgItemText(IDOK, "Назад");
		SetDlgItemText(IDCANCEL, "Отмена");
		SetDlgItemText(IDC_WIZARD_TEXT, "Расположите левую руку, сжатую в кулак, у левого нижнего края изображения на виртуальной доске (как показано на схематичном рисунке-примере)");

		m_img.Destroy();
		m_img.Load("Images/kinect_hint2.jpg");
	}
	else if (3 == step)
	{
		m_time = GetTickCount();
		if (m_isRepeatRecognition)
			m_timeForHands = TIME_FOR_HANDS / 3;


		SetDlgItemText(IDC_WIZARD_TEXT, "Расположите правую руку, сжатую в кулак, у правого нижнего края изображения на виртуальной доске (как показано на схематичном рисунке-примере)");
		SetDlgItemText(IDOK, "Назад");		
		SetDlgItemText(IDCANCEL, "Отмена");

		m_img.Destroy();
		m_img.Load("Images/kinect_hint3.jpg");				
	}
}

void CKinectWizard::ShowRecognizedStatusOnDesktop(bool isRecognized)
{
	CMonitorDrawer* drawer = CMonitorDrawer::Instance();
	if (isRecognized)
	{
		drawer->ShowMonitorImageByCode(SHOW_SUCCESS_RECOGNIZE, 0 );
	}
	else
	{
		drawer->ShowMonitorImageByCode(SHOW_FAIL_RECOGNIZE, 0 );
	}
	drawer->FreeInst();

	m_isShowingRecognizeStatus = true;
}

void CKinectWizard::ShowTimeProgressOnDesktop(int timeStep)
{
	CMonitorDrawer* drawer = CMonitorDrawer::Instance();

	if (timeStep == 0)
	{
		drawer->StopShow();
	}
	else if (timeStep == TIME_FOR_HANDS)
	{
		if (m_isRepeatRecognition)
			drawer->ShowMonitorImageByCode(SHOW_COUNT_BEFORE, timeStep / 3);
		else
			drawer->ShowMonitorImageByCode(SHOW_COUNT_BEFORE, timeStep);
	}

	drawer->FreeInst();

	m_isShowingRecognizeStatus = false;
}

LRESULT CKinectWizard::OnTimer(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	if (!g_kinect)
	{
		return 0;
	}

	if (m_step > 1)
	{
		int timeStep = (GetTickCount() - m_time) / 1000 - STATUS_TIME;

		if (m_isShowingRecognizeStatus)
			m_timeForHands = 0;
		else
			if (m_isRepeatRecognition)
				m_timeForHands = TIME_FOR_HANDS / 3;
			else
				m_timeForHands = TIME_FOR_HANDS;

		if (timeStep >= m_timeForHands)
		{
			float* coords = g_kinect->GetSkeletonXYZ();

			if ( m_step == 2 )
			{
				m_leftCornerX = coords[SKELETON_POSITION_HAND_LEFT * 3 + 0];
				m_leftCornerY = coords[SKELETON_POSITION_HAND_LEFT * 3 + 1];
				m_leftCornerZ = coords[SKELETON_POSITION_HAND_LEFT * 3 + 2];

				if ((0 == m_leftCornerX) && (0 == m_leftCornerY) && (0 == m_leftCornerZ))
				{	//MessageBox("Kinect не распознал левую руку! Пройдите этот шаг еще раз!", "Ошибка!");
					m_isRepeatRecognition = true;
					SetWizardStep(2);

					if (!m_isShowingRecognizeStatus)
						ShowRecognizedStatusOnDesktop(false);
					else
						ShowTimeProgressOnDesktop(TIME_FOR_HANDS);
				}
				else
				{	// левая рука распознана
					if (g_kinect->IsFoundSkeleton() )
					{
						m_isRepeatRecognition = false;
						m_time = GetTickCount(); // для показа статуса

						if (!m_isShowingRecognizeStatus)
							ShowRecognizedStatusOnDesktop(true);
						else
						{
							SetWizardStep(3);	
							ShowTimeProgressOnDesktop(TIME_FOR_HANDS);
						}
					}
					else
					{
						m_isRepeatRecognition = true;
						m_time = GetTickCount(); // для показа статуса

						if (!m_isShowingRecognizeStatus)
							ShowRecognizedStatusOnDesktop(false);
						else
						{
							SetWizardStep(2);
							ShowTimeProgressOnDesktop(TIME_FOR_HANDS);
						}
					}
				}
			}
			else if ( m_step == 3 )
			{
				m_rightCornerX = coords[SKELETON_POSITION_HAND_RIGHT * 3 + 0];
				m_rightCornerY = coords[SKELETON_POSITION_HAND_RIGHT * 3 + 1];
				m_rightCornerZ = coords[SKELETON_POSITION_HAND_RIGHT * 3 + 2];

				if ((0 == m_rightCornerX) && (0 == m_rightCornerY) && (0 == m_rightCornerZ))
				{	//MessageBox("Kinect не распознал правую руку! Пройдите этот шаг еще раз!", "Ошибка!");
					m_isRepeatRecognition = true;
					SetWizardStep(3);

					if (!m_isShowingRecognizeStatus)
						ShowRecognizedStatusOnDesktop(false);
					else
						ShowTimeProgressOnDesktop(TIME_FOR_HANDS);
				}
				else
				{   // правая рука распознана
					if (g_kinect->IsFoundSkeleton() )
					{
						m_isRepeatRecognition = false;
						m_time = GetTickCount(); // для показа статуса

						if (!m_isShowingRecognizeStatus)
							ShowRecognizedStatusOnDesktop(true);
						else
						{
							EndDialog(0);
							KillTimer(2);

							MessageBox("Устройство Kinect настроено!", "Успешно!");
							SaveSettings();
						}
					}
					else
					{
						m_isRepeatRecognition = true;
						m_time = GetTickCount(); // для показа статуса

						if (!m_isShowingRecognizeStatus)
							ShowRecognizedStatusOnDesktop(false);
						else
						{
							SetWizardStep(3);
							ShowTimeProgressOnDesktop(TIME_FOR_HANDS);
						}
					}
				}
			}
		} // endif (timeStep >= m_timeForHands)
	} // endif (m_step > 1)

	RedrawWindow();

	return 0;
}

void CKinectWizard::SaveSettings()
{
	CSaveSettings* sharedSaveSettings =  CSaveSettings::Instance();
	CUserSettings* settings = sharedSaveSettings->GetUserSettings();

	settings->SaveSetting(USER_SETTING_KINECT_LEFT_CORNER_X, ToStr(m_leftCornerX));
	settings->SaveSetting(USER_SETTING_KINECT_LEFT_CORNER_Y, ToStr(m_leftCornerY));
	settings->SaveSetting(USER_SETTING_KINECT_LEFT_CORNER_Z, ToStr(m_leftCornerZ));
	settings->SaveSetting(USER_SETTING_KINECT_RIGHT_CORNER_X, ToStr(m_rightCornerX));
	settings->SaveSetting(USER_SETTING_KINECT_RIGHT_CORNER_Y, ToStr(m_rightCornerY));
	settings->SaveSetting(USER_SETTING_KINECT_RIGHT_CORNER_Z, ToStr(m_rightCornerZ));

	sharedSaveSettings->NotifyAboutChangeSettings();
	sharedSaveSettings->FreeInst();
}

void CKinectWizard::RedrawWindow()
{
	PAINTSTRUCT ps;
	HDC hDC  = BeginPaint(&ps);

	if (g_kinect)
	{		
		if (!m_videoData)
		{
			m_videoDataSize = g_kinect->GetNextVideoFrameSize();

			if (m_videoDataSize > 0)
			{
				m_videoData = new unsigned char[m_videoDataSize];
			}
			else
			{
				EndPaint( &ps);
				return;
			}
		}
		g_kinect->GetNextVideoFrame((void**)&m_videoData);

		//hardcoded locations to Blue, Green, Red (BGR) index positions HWNDddd      
		const int blueIndex = 0;
		const int greenIndex = 1;
		const int redIndex = 2;
		const int alphaIndex = 3;

		int k = 0;

		for (int y = 0; y < KINECT_VIDEO_HEIGHT; y++)
		{
			for (int x = 0; x < KINECT_VIDEO_WIDTH; x++, k++)			
			{
				unsigned int index = y * KINECT_VIDEO_WIDTH * 4 + x * 4;
				if (index < m_videoDataSize)
				{
					if(m_videoData[index + alphaIndex] == 0)
					{
						m_videoData[index + blueIndex] = 0;
						m_videoData[index + greenIndex] = 0;
						m_videoData[index + redIndex] = 255;
					}
				}
			}
		}

		CWindow wnd = GetDlgItem(IDC_VIDEO);

		HDC dc1 = wnd.GetDC();
		::SetDIBits(dc1, m_hBitmap, 0, KINECT_VIDEO_HEIGHT, m_videoData, &m_info, DIB_RGB_COLORS);


		BITMAP bitmap;
		GetObject(m_hBitmap, sizeof(bitmap), &bitmap);
		BitBlt(dc1, 0, 0, bitmap.bmWidth, bitmap.bmHeight, m_hdcMem, 0, 0, SRCCOPY);


		float* skeletonCoords = NULL;
		skeletonCoords = g_kinect->GetSkeletonXY();
		if ( m_step > 1 && g_kinect->IsFoundSkeleton() )
		{
			Graphics * g = Graphics::FromHDC(dc1);
			Pen yellowPen(Color(255, 255, 0), 20);

			if ( m_step == 2 )
			{
				g->DrawEllipse(&yellowPen,
					skeletonCoords[SKELETON_POSITION_HAND_LEFT * 2 + 0],// + offsetX,
					skeletonCoords[SKELETON_POSITION_HAND_LEFT * 2 + 1],// + offsetY,
					20.f, 20.f);
			}
			if ( m_step == 3 )
			{
				g->DrawEllipse(&yellowPen, 
					skeletonCoords[SKELETON_POSITION_HAND_RIGHT * 2 + 0],// + offsetX,
					skeletonCoords[SKELETON_POSITION_HAND_RIGHT * 2 + 1],// + offsetY,
					20.f, 20.f);
			}
			delete g;
		}

		SelectObject(m_hdcMem2, m_img);
		CStatic wnd2 = GetDlgItem(IDC_KINECT_HINT);		
		HDC dc2 = wnd2.GetDC();
		GetObject(m_img, sizeof(bitmap), &bitmap);
		BitBlt(dc2, 0, 0, bitmap.bmWidth, bitmap.bmHeight, m_hdcMem2, 0, 0, SRCCOPY); 

		ReleaseDC(dc1);
		ReleaseDC(dc2);
	}

	EndPaint( &ps);
}

LRESULT CKinectWizard::OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	RedrawWindow();

	return 0;  
}


// CKinectWizard
LRESULT CKinectWizard::OnStnClickedVideo(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	// TODO: Add your control notification handler code here

	return 0;
}

LRESULT CKinectWizard::OnClickedOK(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	if (m_step == 1)
	{
		SetWizardStep(2);
		ShowTimeProgressOnDesktop(TIME_FOR_HANDS);
	}
	else
	{
		SetWizardStep(m_step - 1);
		ShowTimeProgressOnDesktop(0); // timeStep == 0
	}
	return 0;
}

LRESULT CKinectWizard::OnClickedCancel(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	EndDialog(0);
	ShowTimeProgressOnDesktop(0); // timeStep == 0
	return 0;
}

LRESULT CKinectWizard::OnStnClickedWizardText(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	// TODO: Add your control notification handler code here

	return 0;
}

CKinectWizard::~CKinectWizard()
{
	if (g_kinect)
	{
		if (!m_img.IsNull())
		{			
			m_img.Destroy();
		}
		g_kinect->UnInit();
		g_kinect = NULL;
		if (m_hdcMem)
		{
			DeleteDC(m_hdcMem);
			m_hdcMem = NULL;
		}
		if (m_hdcMem2)
		{
			DeleteDC(m_hdcMem2);
			m_hdcMem2 = NULL;
		}
		if (m_hBitmap)
		{
			DeleteObject(m_hBitmap);
			m_hBitmap = NULL;
		}
		if (m_videoData)
		{
			delete[] m_videoData;
			m_videoData = NULL;
		}		
		if (m_depthData)
		{
			delete[] m_depthData;
			m_depthData = NULL;
		}	
	}
}
LRESULT CKinectWizard::OnStnClickedKinectHint(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	// TODO: Add your control notification handler code here

	return 0;
}
