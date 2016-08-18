#include "stdafx.h"
#include "LoginQueryOut2.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

using namespace ServiceManProtocol;

// Описание версий протокола логинации
// Версия 2
//   requestID - номер запроса
//   aLogin - логин
//   aPassword - пароль
//   apwcEncodingPassword - пароль созраненный с предыдущей сессии
//   apwcComputerJeyCode - хэш компьютера
//   source - 
//   isAnonymous - анонимный вход
//   apwcLanguage - язык установки
//   IP - айпи адрес
//   apwcAuthKey - ключ авторизации с сайта
//   pComputerInfoSize - параметры компа
//   apwcVersion - версия
// Версия 3
//   apwcProxies - установленные прокси сервера
// Версия 4
//   apwcUpdaterKey - ключ обновления для апдейтера
// Версия 5
//   apwcDownloadURL - ссылка на скачивание клиента в ответ сервера

#define AUTH_PROTOCOL_VERSION 6
CLoginQueryOut2::CLoginQueryOut2(int requestID, const wchar_t *aLogin, const wchar_t *aPassword, const wchar_t *apwcEncodingPassword, const wchar_t *apwcComputerJeyCode, int source, bool isAnonymous, const wchar_t* apwcLanguage, const wchar_t *IP, const wchar_t* apwcAuthKey, unsigned char* pComputerInfo, unsigned int pComputerInfoSize, const wchar_t *apwcVersion, unsigned char auProxiesMask, const wchar_t* apwcUpdaterKey, const wchar_t *apwcBuildType)
{
	data.clear();

	data.add( (unsigned short)AUTH_PROTOCOL_VERSION);
	data.add(requestID);

	unsigned short size = wcslen(aLogin);
	data.addData(sizeof(unsigned short), (BYTE*)&size);
	data.addData(size*2, (BYTE*)aLogin);

	size = wcslen(aPassword);
	data.addData(sizeof(unsigned short), (BYTE*)&size);
	data.addData(size*2, (BYTE*)aPassword);
	
	size = wcslen(apwcEncodingPassword);
	data.addData(sizeof(unsigned short), (BYTE*)&size);
	data.addData(size*2, (BYTE*)apwcEncodingPassword);

	size = wcslen(apwcComputerJeyCode);
	data.addData(sizeof(unsigned short), (BYTE*)&size);
	data.addData(size*2, (BYTE*)apwcComputerJeyCode);

	data.add(source);
	data.add(isAnonymous);

	size = wcslen(apwcLanguage);
	data.addData(sizeof(unsigned short), (BYTE*)&size);
	data.addData(size*2, (BYTE*)apwcLanguage);

	size = wcslen(IP);
	data.addData(sizeof(unsigned short), (BYTE*)&size);
	data.addData(size*2, (BYTE*)IP);

	size = wcslen(apwcAuthKey);
	data.addData(sizeof(unsigned short), (BYTE*)&size);
	data.addData(size*2, (BYTE*)apwcAuthKey);

	data.addData(sizeof(unsigned int), (BYTE*)&pComputerInfoSize);
	data.addData(pComputerInfoSize, (BYTE*)pComputerInfo);

	size = wcslen(apwcVersion);
	data.addData(sizeof(unsigned short), (BYTE*)&size);
	data.addData(size*2, (BYTE*)apwcVersion);	

	data.add(auProxiesMask);

	size = wcslen(apwcUpdaterKey);
	data.addData(sizeof(unsigned short), (BYTE*)&size);
	data.addData(size*2, (BYTE*)apwcUpdaterKey);

	size = wcslen(apwcBuildType);
	data.addData(sizeof(unsigned short), (BYTE*)&size);
	data.addData(size*2, (BYTE*)apwcBuildType);
}


