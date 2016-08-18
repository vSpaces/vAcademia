#pragma once

#define APP_UNKNOWN				_T("unknown type of error")
#define APP_ERROR				_T("application error")
#define ASSERT_ERROR			_T("assert error")
#define APP_HANG				_T("application is hanged")
#define APP_LAUNCHER_ERROR		_T("launcher error")
#define APP_PREV_CRASH			_T("application prev crash")
#define ASSERT_MAKE_USER		_T("user make crash")

#define APP_ERROR_ID			0x01
#define ASSERT_ERROR_ID			0x02	
#define APP_HANG_ID				0x04
#define APP_LAUNCHER_ERROR_ID	0x08
#define APP_PREV_CRASH_ID		0x10
#define ASSERT_MAKE_USER_ID		0x20	

#define RUS 1
#define ENG 2
#define KZH 3

#define XCRASHREPORT_ERROR_LOG_FILE				L"INFO.TXT"
#define XCRASHREPORT_ERROR_HANG_LOG_FILE		L"INFO_HANG.TXT"
#define XCRASHREPORT_ERROR_ASSERT_LOG_FILE		L"INFO_ASSERT.TXT"
#define XCRASHREPORT_LAUCNHER_ERROR_LOG_FILE	L"INFO_LAUCNHER.TXT"

#define CONFIG_FILE		L"player.ini"
#define BUFFER_SIZE		65536
#define SERVER_NAME		"95.169.190.11"

#define MAX_PATH_4		4 * MAX_PATH

#define XCRASHREPORT_CRASH_REPORT_APP			L"VacademiaReport.exe"