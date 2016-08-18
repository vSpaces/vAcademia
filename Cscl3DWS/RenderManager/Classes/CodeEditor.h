
#pragma once

#include "Thread.h"
#include "rm.h"
#include "oms_context.h"

#define MAX_CURRENT_LOCATION_SIZE 1024

class CCodeEditor : public rm::ICodeEditorSupport
{
public:
	CCodeEditor();
	~CCodeEditor();

	void Start(std::wstring code);
	void Close();

	void SetWindow(HWND hWnd);
	bool CreateSynFile();
	void SetProcessHandle(HANDLE hndl);
	void SetNewProgramText();
	void SetSynFileChanged();
	
	std::wstring GetCode();
	std::wstring GetSynFileName();
	std::wstring GetSynFilePath();
		
	void OnClosed();
	void OnDestroy();

private:
	void ReadAndExecuteCode();

	MP_WSTRING m_code;
	CThread m_thread;
	bool m_isRunning;
	bool m_editorWasRunning;
	bool m_firstTimeAfterClose;
	HANDLE m_process;
	HWND m_window;
	std::wstring m_synFileName;
	std::wstring m_synFilePath;
	bool m_readError;
	bool m_writeError;

	wchar_t m_currentLocation[MAX_CURRENT_LOCATION_SIZE + 1];
};