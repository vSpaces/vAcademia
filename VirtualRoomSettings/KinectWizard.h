// KinectWizard.h : Declaration of the CKinectWizard

#pragma once

#include "resource.h"       // main symbols
#include <atlhost.h>
#include <atlimage.h>

// CKinectWizard

class CKinectWizard : 
	public CAxDialogImpl<CKinectWizard>
{
public:
	CKinectWizard()
		:m_isRepeatRecognition(false)
		,m_isShowingRecognizeStatus(false)
		,m_isShowTimeProgress(false)
	{
	}

	~CKinectWizard();

	enum { IDD = IDD_KINECTWIZARD };

	BEGIN_MSG_MAP(CKinectWizard)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
		COMMAND_HANDLER(IDCLOSE, BN_CLICKED, OnClickedCancel)
		COMMAND_HANDLER(IDOK, BN_CLICKED, OnClickedOK)
		COMMAND_HANDLER(IDCANCEL, BN_CLICKED, OnClickedCancel)
		COMMAND_HANDLER(IDC_VIDEO, STN_CLICKED, OnStnClickedVideo)
		COMMAND_HANDLER(IDC_WIZARD_TEXT, STN_CLICKED, OnStnClickedWizardText)
		COMMAND_HANDLER(IDC_KINECT_HINT, STN_CLICKED, OnStnClickedKinectHint)
		MESSAGE_HANDLER(WM_TIMER, OnTimer)
		CHAIN_MSG_MAP(CAxDialogImpl<CKinectWizard>)
	END_MSG_MAP()

	// Handler prototypes:
	//  LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	//  LRESULT CommandHandler(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	//  LRESULT NotifyHandler(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);

	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnTimer(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnClickedOK(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnClickedCancel(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnStnClickedVideo(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

private:	
	void SetWizardStep(int step);
	void OnKinectNotFound();
	void OnKinectDLLNotFound();
	void ShowTimeProgressOnDesktop(int timeStep);
	void ShowRecognizedStatusOnDesktop(bool isRecognized);
	void SaveSettings();
	void RedrawWindow();

	HBITMAP m_hBitmap;
	BITMAPINFO m_info;
	HDC m_hdcMem;
	HDC m_hdcMem2;

	CImage m_img;

	unsigned char* m_videoData;
	unsigned int m_videoDataSize;

	unsigned short* m_depthData;
	unsigned int m_depthDataSize;

	int m_timeForHands;

	bool m_isShowingRecognizeStatus;
	bool m_isShowTimeProgress;
	bool m_isRepeatRecognition;

	int m_step;
	int m_time;
	int m_lastPaintTime;

	HMODULE m_hDll;

	float m_leftCornerX, m_leftCornerY, m_leftCornerZ;
	float m_rightCornerX, m_rightCornerY, m_rightCornerZ;

public:
	LRESULT OnStnClickedWizardText(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnStnClickedKinectHint(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
};


