#include "stdafx.h"
#include "URLCheckerVersion.h"
#include "..\cURL\include\curl\curl.h""

struct MemoryStruct {
	char *memory;
	size_t size;
};

size_t WriteMemoryCallback(void *ptr, size_t size, size_t nmemb, void *data)
{
	size_t realsize = size * nmemb;
	struct MemoryStruct *mem = (struct MemoryStruct *)data;

	mem->memory = (char *)realloc(mem->memory, mem->size + realsize + 1);
	if (mem->memory) {
		memcpy(&(mem->memory[mem->size]), ptr, realsize);
		mem->size += realsize;
		mem->memory[mem->size] = 0;
	}
	return realsize;
}

string GetIP(const string &url)
{
	CURL *curl_handle;
	CURLcode res;

	struct MemoryStruct chunk;

	chunk.memory = NULL;
	chunk.size = 0;

	curl_handle = curl_easy_init(); //инициализация сессии
	if(curl_handle)
	{
		string sUrl = (url);
		curl_easy_setopt(curl_handle, CURLOPT_URL, sUrl.c_str());
		curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
		curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)&chunk);
		if((res = curl_easy_perform(curl_handle)) != CURLE_OK)
		{
			return "";
		}
		curl_easy_cleanup(curl_handle); //завершение сессии
	}
	string buf;
	if(chunk.size != 0)
	{
		buf = (const char*) chunk.memory;
	}
	else
	{
		buf = "";
	}
	if(chunk.memory)
		free(chunk.memory);
	curl_global_cleanup();
	
	if (buf.size() >= 7 && buf.size() <= 15)
	{
		bool isCorrect = true;
		for (int i = 0; i < buf.size(); i++)
		if ((buf[i] != '.') && ((buf[i] < '0') || (buf[i] > '9')))
		{
			isCorrect = false;
		}

		if (isCorrect)
		{
			return buf;
		}
	}

	return "";
}


CURLCheckerVersion::CURLCheckerVersion():
	m_update(1),
	m_isNotAvailable(1)
{
}

bool CURLCheckerVersion::Check(const string &url, const string &ini, const string &buildType)
{
	CURL *curl_handle;
	CURLcode res;

	struct MemoryStruct chunk;

	chunk.memory = NULL;
	chunk.size = 0;

	curl_handle = curl_easy_init(); //инициализация сессии
	if(curl_handle)
	{
		string sUrl = (url + "?version=" + ini + "&buildtype=" + buildType);
		curl_easy_setopt(curl_handle, CURLOPT_URL, sUrl.c_str());
		curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
		curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)&chunk);
		if((res = curl_easy_perform(curl_handle)) != CURLE_OK)
		{
			return false;
		}
		curl_easy_cleanup(curl_handle); //завершение сессии
	}
	string buf;
	if(chunk.size != 0)
	{
		buf = (const char*) chunk.memory;
	}else
	{
		buf = "";
	}
	if(chunk.memory)
		free(chunk.memory);
	curl_global_cleanup();
	ParseData(buf);
	return true;
}

CURLCheckerVersion::~CURLCheckerVersion(){};

string CURLCheckerVersion:: GetVersion()const
{
	return m_version;
}

unsigned int CURLCheckerVersion:: GetUpdateType()const
{
	return m_update;
}

bool CURLCheckerVersion:: IsServerNotAvailable()const
{
	return(m_isNotAvailable == 1);
}

void CURLCheckerVersion:: ParseData(const string &data)
{
	char tmp[80] = {0};
	sscanf_s(data.c_str(), "{version:'v_%[^']',update:%u,isTO:%u", &tmp, _countof(tmp),&m_update, &m_isNotAvailable);
	m_version = string(tmp);
}