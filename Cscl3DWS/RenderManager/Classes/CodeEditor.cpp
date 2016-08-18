#include "StdAfx.h"
#include "CodeEditor.h"
#include "RMContext.h"
#include "PlatformDependent.h"
#include "StringFunctions.h"
#include "IniFile.h"

#include "Shellapi.h"

const WCHAR* SYN_FILES_PATH = L"\\Vacademia\\Editor\\";

struct EnumData {
	DWORD dwProcessId;
	HWND hWnd;
};

BOOL CALLBACK EnumProc( HWND hWnd, LPARAM lParam ) {
    // Retrieve storage location for communication data
    EnumData& ed = *(EnumData*)lParam;
    DWORD dwProcessId = 0x0;
    // Query process ID for hWnd
    GetWindowThreadProcessId( hWnd, &dwProcessId );
    // Apply filter - if you want to implement additional restrictions,
    // this is the place to do so.
    if ( ed.dwProcessId == dwProcessId ) {
        // Found a window matching the process ID
        ed.hWnd = hWnd;
        // Report success
        SetLastError( ERROR_SUCCESS );
        return FALSE;
    }
    // Continue enumeration
    return TRUE;
}

HWND FindWindowFromProcessId( DWORD dwProcessId ) {
	
    EnumData ed = { dwProcessId };
    if ( !EnumWindows( EnumProc, (LPARAM)&ed ) &&
         ( GetLastError() == ERROR_SUCCESS ) ) {
        return ed.hWnd;
    }
    return NULL;
}

HWND FindWindowFromProcess( HANDLE hProcess ) {
    return FindWindowFromProcessId( GetProcessId( hProcess ) );
}

DWORD WINAPI StartEditorProc(LPVOID lpParam)
{
	CCrashHandlerInThread ch;
	ch.SetThreadExceptionHandlers();

	CCodeEditor* editor = (CCodeEditor*)lpParam;

	if (editor)
	{
		if (!editor->CreateSynFile())
		{
			editor->OnClosed();
			return 0;
		}		
	} else {
		return 0;
	}

	std::wstring path = GetApplicationRootDirectory();
	 path += L"editor\\notepad++.exe ";

	std::wstring params = editor->GetSynFileName();

	SHELLEXECUTEINFOW sei;
	ZeroMemory(&sei, sizeof(SHELLEXECUTEINFO));
	sei.cbSize = sizeof(SHELLEXECUTEINFO);
	sei.fMask = SEE_MASK_NOCLOSEPROCESS;
	sei.lpVerb = L"open";
	sei.lpFile = path.c_str();
	sei.lpParameters = params.c_str();
	sei.nShow = SW_SHOWNORMAL;
		
	if (ShellExecuteExW(&sei))
	{
		if (editor)
		{
			editor->SetProcessHandle(sei.hProcess);
		}
		HWND hWnd = NULL;
		while (hWnd == NULL)
		{
			hWnd = FindWindowFromProcess(sei.hProcess);
			Sleep(25);
		}
		if (editor)
		{
			editor->SetWindow(hWnd);
		}		
		g->lw.WriteLn("vJS code editor was started");
		WaitForSingleObject(sei.hProcess, INFINITE);
		g->lw.WriteLn("vJS code editor was finished");
		if (editor)
		{
			editor->OnClosed();
		}
	}
	else
	{
		g->lw.WriteLn("[ERROR] Failed to run vJS code editor. GetLastError = ", GetLastError());
		if (editor) 
		{
			editor->OnClosed();
		}
	}
	
	return 0;
}

CCodeEditor::CCodeEditor():
	MP_WSTRING_INIT(m_code),
	m_isRunning(false),
	m_editorWasRunning(false),
	m_firstTimeAfterClose(false),
	m_process(NULL),
	m_window(NULL),
	m_readError(false),
	m_writeError(false)
{
	 
}

void CCodeEditor::SetWindow(HWND hWnd)
{
	m_window = hWnd;
}

void CCodeEditor::Start(std::wstring code)
{
	if (m_isRunning)
	{
		if (m_thread.IsThreadTerminated())
		if (m_process)
		{
			DWORD exitCode;
			GetExitCodeProcess(m_process, &exitCode);
			if (exitCode != STILL_ACTIVE)
			{
				m_isRunning = false;
			}
		}
		else
		{
			m_isRunning = false;
		}
	}

	if (!m_isRunning)
	{
		gRM->mapMan->GetCurrentLocationName(m_currentLocation, MAX_CURRENT_LOCATION_SIZE);
		m_isRunning = true;
		m_code = code;
		m_thread.SetParameter(this);
		m_thread.SetThreadProcedure(&StartEditorProc);
		m_thread.Start();
		m_editorWasRunning = true;
	}
	else
	{
		SetForegroundWindow(m_window);
		g->lw.WriteLn("Attempt to start vJS editor again. vJS code editor was started yet");
	}
}

bool CCodeEditor::CreateSynFile()
{
	m_synFilePath =  GetApplicationDataDirectory();
	m_synFilePath += SYN_FILES_PATH;
	CreateDirectory(m_synFilePath);
	m_synFilePath += m_currentLocation;
	m_synFilePath += L"\\";
	CreateDirectory(m_synFilePath);

	m_synFileName = m_synFilePath;
	m_synFileName += m_currentLocation;
	m_synFileName += L".js";

	FILE * f = _wfopen(m_synFileName.c_str(), L"wb");
	m_writeError = f == NULL;
	if (!m_writeError) {
		size_t writed = fwrite(m_code.c_str(), sizeof(WCHAR), m_code.length(), f);
		fclose(f);
		m_writeError = writed != m_code.length();
	}

	if (!m_writeError) 
	{
		g->lw.WriteLn("vJS editor syn file created: ", m_synFileName);
	} else 
	{
		g->lw.WriteLn("[ERROR] Failed to create vJS editor syn file '", m_synFileName, "'. ");
	}

	return !m_writeError;
}

std::wstring CCodeEditor::GetSynFileName() {
	return m_synFileName;
}

std::wstring CCodeEditor::GetCode()
{
	return m_code;
}

void CCodeEditor::SetNewProgramText()
{
	if (!m_editorWasRunning)
		return;
	
	if (m_isRunning || m_firstTimeAfterClose)
	{
		ReadAndExecuteCode();
		m_firstTimeAfterClose = false;
	}
}

void CCodeEditor::ReadAndExecuteCode() 
{
	if (m_writeError) {
		return;
	}

	wchar_t currentLocation[MAX_CURRENT_LOCATION_SIZE + 1];
	gRM->mapMan->GetCurrentLocationName(currentLocation, MAX_CURRENT_LOCATION_SIZE);
	if (wcscmp(currentLocation, m_currentLocation) == 0)
	{
		FILE * f = _wfopen(m_synFileName.c_str(), L"rb");
		m_readError = f == NULL;
		if (!m_readError) 
		{
			std::wstring buffer;
			fseek(f, 0, SEEK_END);
			buffer.resize(ftell(f)/sizeof(WCHAR));
			fseek(f, 0, SEEK_SET);
			size_t readed = fread(&buffer[0], sizeof(WCHAR), buffer.size(), f);
			m_readError = readed != buffer.size();
			fclose(f);
			if (!m_readError && m_code != buffer)
			{
				m_code = buffer;
				g->lw.WriteLn("New vJS program text accepted!"/*, buffer.substr(0, min(buffer.size(), 100))*/);
				gRM->nrMan->GetContext()->mpSM->OnCodeEditorClose(m_code);
			}
		} 
		if (m_readError)
		{
			g->lw.WriteLn("[ERROR] Failed to read vJS program from: ", m_synFileName, "'. ");
		}
	}
	else
	{
		g->lw.WriteLn("Attempt to save vJS code from another location");
	}
}

void CCodeEditor::OnClosed()
{
	if (m_isRunning)
	{
		g->lw.WriteLn("vJS code editor onClosed");
	}
	m_firstTimeAfterClose = true;
	m_isRunning = false;
}

void CCodeEditor::SetProcessHandle(HANDLE hndl)
{
	m_process = hndl;
}

void CCodeEditor::Close()
{
	m_isRunning = false;

	if (m_process)
	{
		rtl_TerminateOtherProcess(m_process, 0);
		CloseHandle(m_process);	
	}
}

void CCodeEditor::OnDestroy()
{
	if (m_isRunning)
	if (m_process)
	{
		rtl_TerminateOtherProcess(m_process, 0);
		CloseHandle(m_process);	
	}
}

CCodeEditor::~CCodeEditor()
{
}