//----------------------------------------------------------------------------//
// ExporterTown.cpp                                                               //

//----------------------------------------------------------------------------//
// Includes                                                                   //
//----------------------------------------------------------------------------//
#include "StdAfx.h"
#include "Exporter.h"
#include "BaseInterface.h"
#include "SkeletonCandidate.h"
#include "BoneCandidate.h"
#include "BaseNode.h"
#include "SkeletonExportSheet.h"
#include "AnimationExportSheet.h"
#include "MeshExportSheet.h"
#include "MaterialExportSheet.h"
#include "MeshCandidate.h"
#include "SubmeshCandidate.h"
#include "VertexCandidate.h"
#include "MaterialLibraryCandidate.h"
#include "MaterialCandidate.h"
//#include "FaceDesc.h"
//#include "Mpeg4FDP.h"
#include "MaxMaterial.h"
#include "MaxMesh.h"
#include "MaxLight.h"
#include "MaxFlare.h"
#include "FileSys.h"
#include "scenelightsdlg.h"
#include "coreanimationflags.h"
#include "flaresdlg.h"
#include <algorithm>

#define NATURA3D_API
#include "3DobjectConfig.h"
#include "autoexport/AutoExportSettings.h"

// arc libs
#include "zlibengn.h"

// ATI optimize
#include "tootlelib.h"
// NVidia optimize
#include "NvTriStrip.h"

// for notepad
#include <process.h>

// for http
#include <winhttp.h>

#include "FreeImagePlus.h"

// for cams
#include "gencam.h"

// Export options


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


#ifdef MAX3DS_BUILD
/*
#define VIEW_DISTANCE_MAX		"20000"
#define VIEW_DISTANCE_LOD1		"20000"
#define VIEW_DISTANCE_LOD2		"10000"
#define VIEW_DISTANCE_LOD3		"5000"
#define VIEW_SHADOW_MAX			"1"
#define VIEW_SHADOW_LOD1		"0"
#define VIEW_SHADOW_LOD2		"1"
#define VIEW_SHADOW_LOD3		"1"
*/

#define VIEW_DISTANCE_MAX		25000 
#define VIEW_DISTANCE_LOD2		20000
#define VIEW_DISTANCE_LOD1		10000
#define VIEW_DISTANCE_LOD0		5000
#define VIEW_SHADOW_MAX			1
#define VIEW_SHADOW_LOD2		0
#define VIEW_SHADOW_LOD1		1
#define VIEW_SHADOW_LOD0		1
// если лод > 2 то дистанция lod*5000+10000

#define GROUP_CONFIG_FILE_NAME		"group.scxml"
#define ZONE_SIZE					20000
#define BINARY_MESH_VERSION			5

#if BINARY_MESH_VERSION < 4
	#define OBJECT_CONFIG_FILE_NAME		"mesh.bmf"
#else
	#define OBJECT_CONFIG_FILE_NAME		"mesh.bmf.zip"
#endif

#define CAL_CONFIG_FILE_NAME	"cal3d.cfg"
#define CAL_CONFIG_PARAM_SERVER_PATH	"ServerPath"

#define MATERIAL_TEXTURES_FILE_NAME		"materials.xml" // список диффузных текстур у данной модели

// Версия 1 - Базовая
// Версия 2
//	- для всех вершин scale, offY, offY, offZ - пишем double
// Версия 3
//	- адресация текстурных координат float
// Версия 4
//  - добавлен в конец блок описания физических примитивов
//  - ..
//////////////////////////////////////////////////////////////////////////
extern	SObjectCfg objectCfg;

//////////////////////////////////////////////////////////////////////////
char	specCharacters[] = {'+',	// объект 
							'&',	// объект - ссылка
							'%', 
							'@', 
							'~',	// дерево
							'$',	// skybox
							'#',	// примитив коллизии
							'^'		// место сиденья
};

// & - объект ссылка
// @ - объект для коллизий
// ~ - объект - дерево

// # - примитивы коллизии #objectName[type={box, sphere}], где objectName имя объекта, для кторого создан примитв
// ^ - место для сиденья ^objectName, где objectName имя объекта, привязанного к месту сиденья.
// Pivot осью Х должен указывать направление, в котором сидит аватар, ось Z указывает на ось позвоночника

//////////////////////////////////////////////////////////////////////////
bool TootleIsInited = false;
std::string MaxInstallPath = "C:\\Program Files\\Autodesk\\3ds Max 2009\\"; // default path
std::string ServerPath = "";
//////////////////////////////////////////////////////////////////////////
// генерация случайного имени для объекта (таких как примитивы и место посадки)
unsigned int tempObjectNumber = 0;

CString GetTempName()
{
	CString tempObjectName;

	tempObjectName.Format("tempName_%i", tempObjectNumber);

	tempObjectNumber ++;
	
	return tempObjectName;
}

//////////////////////////////////////////////////////////////////////////
// Получение уровня блокировки объекта
// 0 -Нет, 1 - Свойств, 2 - Геометрии, 3- Полная
int GetModifyLevel(CString objectName)
{
	int modifyLevel = 0;

	HINTERNET hParserSession = WinHttpOpen(L"VU exporter/1.0",
		WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
		WINHTTP_NO_PROXY_NAME,
		WINHTTP_NO_PROXY_BYPASS,
		0);

	if(hParserSession)
	{
		std::string url = CAutoExportSettings::GetParserUrl().GetBuffer(0);
		std::string host, uri;

		if (url.find("://") != std::string::npos)
		{
			url = url.substr(url.find("://") + 3);
		}

		if (url.find("/") == -1)
		{
			return 0;
		}

		host = url.substr(0, url.find("/"));
		uri = url.substr(url.find("/"));

		wchar_t	*parserHost = new wchar_t[host.length()+1];
		wchar_t	*parserUri = new wchar_t[uri.length()+1];
		memset(parserHost, 0, url.length()+1);
		memset(parserUri, 0, url.length()+1);

		MultiByteToWideChar(CP_ACP, NULL, host.c_str(), -1, parserHost, host.length()+1);
		MultiByteToWideChar(CP_ACP, NULL, uri.c_str(), -1, parserUri, uri.length()+1);

		HINTERNET hParserConnection = WinHttpConnect(hParserSession,
			parserHost,
			INTERNET_DEFAULT_HTTP_PORT,											
			0);

		if(hParserConnection)
		{
			HINTERNET hParserRequest = WinHttpOpenRequest(hParserConnection,
				L"POST",
				parserUri,
				NULL,
				WINHTTP_NO_REFERER,
				WINHTTP_DEFAULT_ACCEPT_TYPES,
				0);

			if(hParserRequest)
			{
				WinHttpSetTimeouts(hParserRequest, 0, 6000, 6000, 6000);

				long dataSize = objectName.GetLength() + 5;

				char* dataBuffer = new char[dataSize];
				dataBuffer[0] = 'l';
				dataBuffer[1] = 'o';
				dataBuffer[2] = 'c';
				dataBuffer[3] = 'k';
				dataBuffer[4] = '=';

				memcpy(&dataBuffer[5], objectName.GetBuffer(), objectName.GetLength());

				bool httpResult = false;

				httpResult = WinHttpAddRequestHeaders(hParserRequest, 
					L"Content-Type: application/x-www-form-urlencoded",
					-1L,
					WINHTTP_ADDREQ_FLAG_ADD | WINHTTP_ADDREQ_FLAG_REPLACE );

				if(httpResult)
				{
					httpResult = WinHttpSendRequest(hParserRequest,
						WINHTTP_NO_ADDITIONAL_HEADERS,
						0,
						&dataBuffer[0],
						dataSize,
						dataSize,
						0);

				}

				if (httpResult)
				{
					httpResult = WinHttpReceiveResponse(hParserRequest, NULL);
				}

				// WinHttpReadData 
				if(httpResult)
				{
					DWORD dwDownloaded = 0;
					LPSTR pszOutBuffer;
					DWORD dwSize = 0;

					//do 
					//{
						// Check for available data.
						dwSize = 0;
						if (!WinHttpQueryDataAvailable( hParserRequest, &dwSize))
							printf( "Error %u in WinHttpQueryDataAvailable.\n",
							GetLastError());

						// Allocate space for the buffer.
						pszOutBuffer = new char[dwSize+1];
						if (!pszOutBuffer)
						{
							printf("Out of memory\n");
							dwSize=0;
						}
						else
						{
							// Read the Data.
							ZeroMemory(pszOutBuffer, dwSize+1);

							if (!WinHttpReadData( hParserRequest, (LPVOID)pszOutBuffer, 
								dwSize, &dwDownloaded))
								printf( "Error %u in WinHttpReadData.\n", 
								GetLastError());
							else
								printf("%s", pszOutBuffer);

							modifyLevel = pszOutBuffer[0]- '0';
							// Free the memory allocated to the buffer.
							delete [] pszOutBuffer;
						}

					//} while (dwSize>0);

				}

				// Если произошла ошибка
				if (!httpResult)
				{
					int errCode = GetLastError();
					AfxMessageBox("Ошибка во время передачи данных на сервер", MB_OK | MB_ICONEXCLAMATION);
				}

				delete[] dataBuffer;

				WinHttpCloseHandle(hParserRequest);
			}

			WinHttpCloseHandle(hParserConnection);
		}

		WinHttpCloseHandle(hParserSession);
	}



	return modifyLevel;
}

//////////////////////////////////////////////////////////////////////////
// сортировка лодов
std::vector<LOD_PROPS> SortLodsByName(std::vector<LOD_PROPS> lodList)
{
	for(int i = 0; i < lodList.size(); i++)
	{
		for (int j = i+1; j < lodList.size(); j++)
		{
			if (lodList[i].lod > lodList[j].lod)
			{
				std::swap(lodList[i], lodList[j]);
			}
		}
	}

	return lodList;
}

std::vector<LOD_PROPS> SortLodsByDistance(std::vector<LOD_PROPS> lodList)
{
	for(int i = 0; i < lodList.size(); i++)
	{
		for (int j = i+1; j < lodList.size(); j++)
		{
			if (lodList[i].distance > lodList[j].distance)
			{
				std::swap(lodList[i], lodList[j]);
			}
		}
	}

	return lodList;
}

//////////////////////////////////////////////////////////////////////////

BOOL operator<(const Cal3DComString& str1, const Cal3DComString& str2)
{
	return str1.Compare(str2);
}

void	AlignFloatToWorld(float& f)
{
	f = floorf(f) + 0.5f;
}

//////////////////////////////////////////////////////////////////////////
CString	GetLodDirName( CString	asBaseName, int aiLod)
{
	if( aiLod <= 0)	return asBaseName;

	CString	ret;
	ret.Format("%slod%i", asBaseName.GetBuffer(0), aiLod);
	return	ret;
}

//////////////////////////////////////////////////////////////////////////
BOOL	IsNumber( TCHAR aChar)
{
	if( aChar < '0' || aChar > '9')	return FALSE;
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
BOOL	IsPrefix(CString	asPrefixCandidate)
{
	// Prefix is sq[number][_]
	CString	sPrefixBase = "sq";
	if( asPrefixCandidate.GetLength() < sPrefixBase.GetLength() + 1)	return FALSE;
	asPrefixCandidate.MakeLower();

	//if( asPrefixCandidate[asPrefixCandidate.GetLength()-1] != '_')	return FALSE;
	if( asPrefixCandidate[asPrefixCandidate.GetLength()-1] == '_')
	{
		asPrefixCandidate = asPrefixCandidate.Left(asPrefixCandidate.GetLength()-1);
	}

	if( asPrefixCandidate.Find( sPrefixBase.GetBuffer(0)) != 0)	return FALSE;

	/*int	index = sPrefixBase.GetLength();
	while (index < asPrefixCandidate.GetLength()-1)
	{
		if( !IsNumber( asPrefixCandidate[index]))	return FALSE;
		index++;
	}*/

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
void	ParseName( CString	sMaxNodeName, CString& sPrefix, CString& sName, CString& sPostfixes, CString& sReferenceName, vespace::VESPACE_OBJECTS_TYPES& eType, BOOL& bZone)
{
	ATLASSERT( !sMaxNodeName.IsEmpty());

	eType = vespace::VE_UNDEFINED;
	// если это ссылочный объект, то уберем его ссылку
	if( sMaxNodeName[0] == '&')
	{
		sReferenceName = sMaxNodeName.Right( sMaxNodeName.GetLength() - 1);
		sMaxNodeName = sReferenceName;
		if( sReferenceName.Find("[")!=-1)
		{
			//sMaxNodeName = sReferenceName.Right(sReferenceName.GetLength() - sReferenceName.Find("["));
			sReferenceName = sReferenceName.Left(sReferenceName.Find("["));
		}
		eType = vespace::VE_REFERENCE;
	}
	else	if( sMaxNodeName[0] == '~')
	{
		sMaxNodeName = sMaxNodeName.Right( sMaxNodeName.GetLength() - 1);
		eType = vespace::VE_TREE;
	}
	else	if( sMaxNodeName[0] == '$')
	{
		sMaxNodeName = sMaxNodeName.Right( sMaxNodeName.GetLength() - 1);
		eType = vespace::VE_SKYBOX;
	}

	else	if( sMaxNodeName[0] == '#')
	{
		sMaxNodeName = sMaxNodeName.Right( sMaxNodeName.GetLength() - 1);

		eType = vespace::VE_COLLIDER_OBJECT;
	}
	else	if (sMaxNodeName[0] == '^')
	{		
		sMaxNodeName = sMaxNodeName.Right( sMaxNodeName.GetLength() - 1);

		eType = vespace::VE_SITTING_PLACE;
	}


	bZone = FALSE;

	if (sMaxNodeName.Find("[zone]") != -1)
	{
		bZone = TRUE;
	}

	/*if (sMaxNodeName.GetLength() >= 4)
	{
		bZone = (('z' == sMaxNodeName[0]) && ('o' == sMaxNodeName[1]) &&
				 ('n' == sMaxNodeName[2]) && ('e' == sMaxNodeName[3]));
	}*/

	// уберем спец символы
	int	specCount = 0;
	int specCharsCount = sizeof(specCharacters)/sizeof(specCharacters[0]);
	while (specCount < sMaxNodeName.GetLength())
	{
		int i=0;
		for (i=0; i<specCharsCount; i++)
		{
			if( sMaxNodeName[specCount] == specCharacters[i])	break;
		}
		if( i == specCharsCount)	break;
		specCount++;
	}
	sMaxNodeName = sMaxNodeName.Right( sMaxNodeName.GetLength() - specCount);
	
	// ищем префикс
	int	subPrefix = sMaxNodeName.Find("_");
	if( subPrefix != -1)
	{
		if( IsPrefix( sMaxNodeName.Left( subPrefix+1)))	sPrefix = sMaxNodeName.Left( subPrefix+1);
	}
	/*if( sPrefix.IsEmpty())
	{
		bool	prefixFound = false;
		for (int i=1; i<sMaxNodeName.GetLength() - 1; i++)
		{
			if( IsPrefix( sMaxNodeName.Left( i)))
			{
				if( !prefixFound)
					prefixFound = true;
			}
			else
			{
				if( prefixFound)
				{
					sPrefix = sMaxNodeName.Left( i - 1);
					
					break;
				}
			}
		}
	}*/
	if( sPrefix.GetLength()>0 && sPrefix[sPrefix.GetLength()-1] == '_')
		sPrefix = sPrefix.Left( sPrefix.GetLength()-1);

	//sMaxNodeName = sMaxNodeName.Right( sMaxNodeName.GetLength() - sPrefix.GetLength());

	/*
	// убираем переднее _ (которое могло остаться от префикса)
	while (sMaxNodeName.GetLength()>0 && sMaxNodeName[0] == '_')
		sMaxNodeName = sMaxNodeName.Right(sMaxNodeName.GetLength()-1);
	*/

	// ищем постфиксы
	int	subPostfix = sMaxNodeName.Find("[");
	if( subPostfix != -1)
	{
		sPostfixes = sMaxNodeName.Right(sMaxNodeName.GetLength() - subPostfix);
		sMaxNodeName = sMaxNodeName.Left(subPostfix);
	}

	sName = sMaxNodeName;
}

MAP_NODE_DESCRIPTION*	GetNodeDesriptionByName(CString name, std::vector<MAP_NODE_DESCRIPTION>*	aObjects, CString location = "")
{
	ATLASSERT( aObjects);
	if( !aObjects)	return NULL;
	std::vector<MAP_NODE_DESCRIPTION>::iterator	it2 = aObjects->begin(),
												end2 = aObjects->end();
	name.MakeLower();
	for ( ; it2!=end2; ++it2)
	{
		if( it2->m_IsReference)	continue;
		CString test_name = it2->m_Name;
		test_name.MakeLower();
		if (name == test_name)
		{
			if (location != "")
			{
				std::vector<TWO_STRING> params = it2->m_Properties;

				for (int i = 0; i < params.size(); i++)
				{
					if (params[i].key == "location" && params[i].value == location)
					{
						return &(*it2);
					}
				}
			}
			else
			{
				return &(*it2);
			}		
		}
	}
	ATLASSERT( NULL);
	return NULL;
}

MAP_NODE_DESCRIPTION*	GetNodeDesriptionByINode(INode* name, std::vector<MAP_NODE_DESCRIPTION>*	aObjects)
{
	ATLASSERT( aObjects);
	if( !aObjects)	return NULL;
	std::vector<MAP_NODE_DESCRIPTION>::iterator	it2 = aObjects->begin(),
												end2 = aObjects->end();
	for ( ; it2!=end2; ++it2)
	{
		//if( it2->m_IsReference)	continue;
		if (it2->m_pBaseNode->GetINode()  == name)
		{
			return &(*it2);
		}
	}
	return NULL;
}

MAP_NODE_DESCRIPTION* GetReferenceChainHead(MAP_NODE_DESCRIPTION* node)
{
	if( node->m_pParentGroupNodeDescription == NULL)
		return node->m_IsReference ? node : NULL;
	return GetReferenceChainHead(node->m_pParentGroupNodeDescription);
}

bool IsNodeInReferenceChain(MAP_NODE_DESCRIPTION* node)
{
	MAP_NODE_DESCRIPTION* chainHead = GetReferenceChainHead(node);
	return (chainHead==NULL||chainHead==node) ? false:true;
}

//////////////////////////////////////////////////////////////////////////
void	CExporter::GetSelectedObjectsDesc(std::vector<MAP_NODE_DESCRIPTION>*	aSelectedObjects, MapReferencesNames& mapReferencesNames)
{
	ATLASSERT( aSelectedObjects);
	if( !aSelectedObjects)	return;

	std::vector<CBaseNode*>	selectedNodes;

	int i=0;
	for(; i<theExporter.GetInterface()->GetSelectedNodeCount(); i++)
	{
		CBaseNode* m_pNode = theExporter.GetInterface()->GetSelectedNode(i);
		ATLASSERT( m_pNode);
		selectedNodes.push_back(m_pNode);
	}
	
	// Sort objects.. Move groups to tail.
	for(i=0; i<selectedNodes.size(); i++)
	{
		if( !selectedNodes[i]->GetINode())	continue;

		for(int j=i+1; j<selectedNodes.size(); j++)
		{
			if( !selectedNodes[j]->GetINode())	continue;

			if( 
				// groups to tail
				(selectedNodes[i]->GetINode()->IsGroupHead() &&
				!selectedNodes[j]->GetINode()->IsGroupHead())  ||

				// references to tail
				(selectedNodes[i]->GetName().length()>0 && selectedNodes[i]->GetName()[0]=='&' &&
				 (selectedNodes[j]->GetName().length()==0 || selectedNodes[j]->GetName()[0]!='&'))
			  )
			{
				CBaseNode*	temp = selectedNodes[i];
				selectedNodes[i] = selectedNodes[j];
				selectedNodes[j] = temp;
				continue;
			}
		}
	}

	for(i=0; i<selectedNodes.size(); i++)
	{
		CBaseNode* m_pNode = selectedNodes[i];
		ATLASSERT( m_pNode);
		if( !m_pNode)	
			continue;

		Object *obj = m_pNode->GetINode()->EvalWorldState(0.0).obj;
		if( !obj)	continue;

		CString	sMaxNodeName = m_pNode->GetName().c_str();

		Class_ID	objectClassID = obj->ClassID();

		
		/*if( objectClassID == Class_ID(DUMMY_CLASS_ID, 0))
		{
			// save error...
			CString sType = "DUMMY";
			WriteLog( "Object: "+sMaxNodeName+" is of unsupported type:" + sType);
			continue;
		}*/
		
		MAP_NODE_DESCRIPTION	desc;
		CString	sPrefix;
		CString	sPostfixes;
		CString	sName;
		CString	sReferenceName;
		vespace::VESPACE_OBJECTS_TYPES	eType;
		BOOL bZone;
		ParseName( sMaxNodeName, sPrefix, sName, sPostfixes, sReferenceName, eType, bZone);

		desc.m_Prefix = sPrefix;
		desc.m_Name = sName;
		desc.m_ReferenceName = sReferenceName;
		desc.m_IsReference = !desc.m_ReferenceName.IsEmpty();
		desc.m_pBaseNode = m_pNode;
		desc.m_Type = eType;
		desc.m_IsZone = bZone;
		TSTR	buffer;
		m_pNode->GetINode()->GetUserPropBuffer( buffer);
		desc.m_UserDefinedProperties = buffer;

		if(objectClassID == Class_ID(LOOKAT_CAM_CLASS_ID, 0))
		{
			desc.m_Type = vespace::VE_CAMERA;
		}
		else if(objectClassID == Class_ID(TARGET_CLASS_ID, 0))
		{
			desc.m_Type = vespace::VE_CAMERA_TARGET;
			desc.m_Name += ".Target";
		}
		
		//////////////////////////////////////////////////////////////////////////
		if( !sReferenceName.IsEmpty() )
		{
			MapReferencesNames::iterator	it = mapReferencesNames.find( sReferenceName);
			if( it == mapReferencesNames.end())
			{
				mapReferencesNames.insert( MapReferencesNames::value_type(CString(sReferenceName), 1));
				it = mapReferencesNames.find( sReferenceName);
			}
			else
			{
				it->second++;
			}
			desc.m_Name.Format("%s%u", sReferenceName, it->second);
		}

		if (eType == vespace::VE_COLLIDER_OBJECT || eType == vespace::VE_SITTING_PLACE)
		{
			desc.m_ReferenceName = desc.m_Name;
			desc.m_Name = GetTempName();
		}
		
		// парсим постфиксы
		while (!sPostfixes.IsEmpty())
		{
			int	openIndex = sPostfixes.Find('[');
			int	closeIndex = sPostfixes.Find(']');
			if( openIndex==-1 || closeIndex==-1)
				break;

			CString	sNextPostfix = sPostfixes.Mid(openIndex+1, closeIndex-openIndex-1);

			CString	sNextPostfixName;
			CString	sNextPostfixValue;
			if( sNextPostfix.Find("=") != -1)
			{
				sNextPostfixName = sNextPostfix.Left(sNextPostfix.Find("="));
				sNextPostfixValue = sNextPostfix.Right( sNextPostfix.GetLength() - sNextPostfixName.GetLength()-1);
			}
			else
			{
				int	lastValueIndex = sNextPostfix.GetLength();
				while ( lastValueIndex>0 && IsNumber( sNextPostfix[lastValueIndex-1]))
				{
					lastValueIndex--;
				}
				sNextPostfixValue = sNextPostfix.Right( sNextPostfix.GetLength() - lastValueIndex);
				sNextPostfixName = sNextPostfix.Left( sNextPostfix.GetLength() - sNextPostfixValue.GetLength());
			}
			sPostfixes = sPostfixes.Right( sPostfixes.GetLength() - closeIndex - 1);

			if (sNextPostfixName == "name")
				desc.m_Name = sNextPostfixValue;
			else if (sNextPostfixName == "logicobj")
				desc.m_logicObjName = sNextPostfixValue;
			else	if (sNextPostfixName == "zone")
				desc.m_zoneName = sNextPostfixValue;
			else	if( sNextPostfixName=="l" || sNextPostfixName=="level" || sNextPostfixName=="id")
				desc.m_Level = atof( sNextPostfixValue.GetBuffer(0));
			else	if( sNextPostfixName=="b" || sNextPostfixName=="bias")
				desc.m_Bias = atof( sNextPostfixValue.GetBuffer(0));
			else	if( sNextPostfixName=="lod")
			{
				LOD_PROPS newLod;
				newLod.lod = atof(sNextPostfixValue.GetBuffer(0));
				desc.m_LodProps.push_back(newLod);
			}
			else	if(sNextPostfixName=="distance")
			{
				if(desc.m_LodProps.empty())
				{
					LOD_PROPS defaultLod;
					desc.m_LodProps.push_back(defaultLod);
				}
				desc.m_LodProps[desc.m_LodProps.size()-1].distance = atof( sNextPostfixValue.GetBuffer(0));
			}
			else	if(sNextPostfixName=="shadow")
			{
				if(desc.m_LodProps.empty())
				{
					LOD_PROPS defaultLod;
					desc.m_LodProps.push_back(defaultLod);
				}
				desc.m_LodProps[desc.m_LodProps.size()-1].shadow = atof( sNextPostfixValue.GetBuffer(0));
			}
			else	if( sNextPostfixName=="cmn" || sNextPostfixName=="common")
				desc.m_Common = TRUE;
			else	if( sNextPostfixName=="class")
				desc.m_Class = sNextPostfixValue;
			else	if( sNextPostfixName=="closed")
				desc.m_ExplodeGroup = FALSE;
			else	if( sNextPostfixName=="nogeometry" ||
						sNextPostfixName=="nogeo" ||
						sNextPostfixName=="nogeom" ||
						sNextPostfixName=="no_geometry")
			{
				desc.m_NoGeometry = TRUE;
			}
			else	if( sNextPostfixName=="hidden")
			{
				desc.m_Properties.push_back(TWO_STRING( "hidden", "true"));
			}
			else	if( sNextPostfixName=="grass")
			{
				desc.m_Grass = TRUE;
			}
			else	if( sNextPostfixName=="res")
			{
				desc.m_Res = sNextPostfixValue;
			}
			else	if(sNextPostfixName=="target")
			{
				desc.m_Res = sNextPostfixValue;
			}
			else	if( sNextPostfixName=="stencil")
			{
				desc.m_GenerateStencilData = true;
			}
			else	if( sNextPostfixName=="__DBID")
			{
				desc.m_bDatabaseSet = TRUE;
				desc.m_DatabaseID = atoi( sNextPostfixValue);
			}
			else	if( sNextPostfixName=="type")
			{
				if( sNextPostfixValue == "tree")
				{
					desc.m_Type = vespace::VE_TREE;
				}
				else	if( sNextPostfixValue == "group")
				{
					desc.m_Type = vespace::VE_GROUP3D;
				}
				else	if( sNextPostfixValue == "location" || sNextPostfixValue == "location_bounds")
				{
					desc.m_Type = vespace::VE_LOCATIONBOUNDS;
				}
				else	if( sNextPostfixValue == "object")
				{
					desc.m_Type = vespace::VE_OBJECT3D;
				}
				else	if( sNextPostfixValue == "sunlensflare" || sNextPostfixValue == "sunlensflares")
				{
					desc.m_Type = vespace::VE_SUNLENSFLARE;
					desc.m_NoGeometry = TRUE;
				}
				else	if( sNextPostfixValue == "collider_aabb")
				{
					desc.m_Type = vespace::VE_COLLIDER_AABB;
				}
				else	if( sNextPostfixValue == "collider_oobb")
				{
					desc.m_Type = vespace::VE_COLLIDER_OOBB;
				}
				else	if( sNextPostfixValue == "collider_mesh")
				{
					desc.m_Type = vespace::VE_COLLIDER_MESH;
				}

				else	if( sNextPostfixValue == "box")
				{
					desc.m_Properties.push_back(TWO_STRING( "collider_type", "box"));
				}
				else	if( sNextPostfixValue == "sphere")
				{
					desc.m_Properties.push_back(TWO_STRING( "collider_type", "sphere"));
				}


				else
				{
					//ATLASSERT(FALSE);
					WriteLog( "Incorrect type["+sNextPostfixValue+"] for object: "+desc.m_Name);
				}
			}
			else
			{
				desc.m_Properties.push_back(TWO_STRING( sNextPostfixName, sNextPostfixValue));
			}
		}
		if (m_pNode->GetINode()->IsGroupHead()) 
		{
			desc.m_IsGroupHead = TRUE;
		}
		else
		{
			desc.m_IsGroupHead = FALSE;
		}
		if( m_pNode->GetINode()->GetParentNode()->IsGroupHead())
		{
			desc.m_pParentGroupNode = m_pNode->GetINode()->GetParentNode();
		}
		if( desc.m_Type == vespace::VE_UNDEFINED)
		{
			desc.m_Type = vespace::VE_OBJECT3D;
			if( desc.m_IsGroupHead)
				desc.m_Type = vespace::VE_GROUP3D;
		}
		
		if (desc.m_LodProps.empty() && desc.m_IsGroupHead != TRUE)
		{
			LOD_PROPS defaultLod;
			desc.m_LodProps.push_back(defaultLod);
		}
		else
		{
			desc.m_LodProps = SortLodsByName(desc.m_LodProps);
		}

		if(!desc.m_LodProps.empty())
		{
			desc.m_Lod = desc.m_LodProps[0].lod;
		}

		aSelectedObjects->push_back(desc);
	}

	// Sort objects.. Move lods to tail
	// 1. Geometry objects // + примитивы коллизий
	// 2. Lods objects
	// 3. Groups
	// 4. Reference objects
	for(i=0; i<aSelectedObjects->size(); i++)
	{
		for(int j=i+1; j<aSelectedObjects->size(); j++)
		{
			std::vector<MAP_NODE_DESCRIPTION>::iterator it_i = aSelectedObjects->begin() + i;
			std::vector<MAP_NODE_DESCRIPTION>::iterator it_j = aSelectedObjects->begin() + j;

			// 4
			if( (it_i->m_IsReference && !it_j->m_IsReference))
			{
				std::swap( *it_i, *it_j);
				continue;
			}
			else	if( (it_j->m_IsReference && !it_i->m_IsReference))
			{
				continue;
			}

			// 3
			if( (it_i->m_IsGroupHead && !it_j->m_IsGroupHead))
			{
				std::swap( *it_i, *it_j);
				continue;
			}
			else	if( (it_j->m_IsGroupHead && !it_i->m_IsGroupHead))
			{
				continue;
			}

			// 2
			if( it_i->m_Lod > it_j->m_Lod)
			{

				std::swap( *it_i, *it_j);
				continue;
			}
			else	if( it_i->m_Lod < it_j->m_Lod)
			{
				continue;
			}
		}
	}

	
	std::vector<MAP_NODE_DESCRIPTION>::iterator	it = aSelectedObjects->begin(),
												end = aSelectedObjects->end();

	// установим связи для group объектов
	for ( ; it!=aSelectedObjects->end(); ++it)
	{
		if( it->m_pParentGroupNode)
		{
			MAP_NODE_DESCRIPTION*	referenceNode = GetNodeDesriptionByINode( it->m_pParentGroupNode, aSelectedObjects);
			ATLASSERT( referenceNode != &(*it));
			if( referenceNode == &(*it))	continue;
			if( !referenceNode)
			{
				WriteLog( "Grouped object removed because Group is NULL or REFERENCE:"+it->m_Name);
				it->m_Type = vespace::VE_UNDEFINED;
				//it = aSelectedObjects->erase( it);
				//it--;
				continue;
			}
			it->m_pParentGroupNodeDescription = referenceNode; // ?? проверка в условии неинициализированного объекта
			if( !referenceNode->m_ExplodeGroup || it->m_pParentGroupNodeDescription->m_IsReference)
			{
				//it->m_pParentGroupNodeDescription = referenceNode;
				if( it->m_pParentGroupNodeDescription->m_IsReference)
				{
					WriteLog( "Grouped object removed because Group is NULL or REFERENCE:"+it->m_Name);
					it->m_Type = vespace::VE_UNDEFINED;
					//it = aSelectedObjects->erase( it);
					//it--;
				}
			}
			else
			{
				it->m_pParentGroupNodeDescription = NULL;
				it->m_pParentGroupNode = NULL;
			}
		}
	}

	// установим связи для reference объектов
	//it = aSelectedObjects->begin();
	//end = aSelectedObjects->end();
	//for ( ; it!=end; ++it)
	for(int objIndex = 0; objIndex < aSelectedObjects->size(); objIndex++)
	{
		MAP_NODE_DESCRIPTION* node = &(aSelectedObjects->at(objIndex));//&(*it);
		if( node->m_Type == vespace::VE_UNDEFINED)
			continue;
		if( node->m_IsReference && !IsNodeInReferenceChain(node))
		{
			MAP_NODE_DESCRIPTION*	referenceNode = GetNodeDesriptionByName( node->m_ReferenceName, aSelectedObjects);
			
			if (referenceNode == NULL)
			{
				CString notFoundMessage = "Не найден родительский объект '"+node->m_ReferenceName+"' для объекта-ссылки '" + node->m_Name + "'.\nУбедитесь что он выделен.";
				AfxMessageBox(notFoundMessage.GetBuffer(0), MB_OK | MB_ICONEXCLAMATION);

				aSelectedObjects->erase(aSelectedObjects->begin() + objIndex);
				objIndex--;

				continue;
			}

			//ATLASSERT( referenceNode != &(*it));
			ATLASSERT( referenceNode != node);
			if( !referenceNode)	continue;
			//if( referenceNode == &(*it))	continue;
			if( referenceNode == node)	continue;
			node->m_pReferenceNodeDescription = referenceNode;
		}
	}
}

//////////////////////////////////////////////////////////////////////////
CString	CExporter::GetObjectDirName( MAP_NODE_DESCRIPTION* objectDesc, MapReferencesNames& mapReferencesNames)
{
	/*if( objectDesc->m_pReferenceNodeDescription)
	{
		if( objectDesc->m_pReferenceNodeDescription->m_Name.Find("wins")!=-1)
			int kjhwcfw=0;
	}
	else
	{
		if( objectDesc->m_Name.Find("wins")!=-1)
			int kjhwcfw=0;
	}*/

	CString pathObjectDirectory = "";

	if(CExporter::libExport)
	{
		//pathObjectDirectory += "library\\";
	}

	if( CExporter::calexportSeparateCommonObjects &&
			objectDesc->m_Common)
	{
		// common object
		ATLASSERT( !objectDesc->m_IsReference);
		ATLASSERT( !objectDesc->m_Name.IsEmpty());
		pathObjectDirectory += "common\\";
	}
	
	if( objectDesc->m_pParentGroupNodeDescription != NULL &&
		!objectDesc->m_pParentGroupNodeDescription->m_ExplodeGroup)
	{
		pathObjectDirectory += (GetObjectDirName(objectDesc->m_pParentGroupNodeDescription, mapReferencesNames)) + "\\";
	}
	
	CString	pathPrefix = objectDesc->m_Prefix;
	if( objectDesc->m_IsReference && objectDesc->m_pReferenceNodeDescription)
	{
		pathPrefix = objectDesc->m_pReferenceNodeDescription->m_Prefix;
	}

	Cal3DComString	objectDirectoryName = objectDesc->m_Name;
	// create lod directory name
	if( objectDesc->m_Lod > -1)
		objectDirectoryName = GetLodDirName( objectDesc->m_Name, objectDesc->m_Lod);

	if( CExporter::calexportUsePrefixAsDirName &&
		!pathPrefix.IsEmpty())
	{
		pathObjectDirectory += pathPrefix + "\\";
	}
	
	if( objectDesc->m_IsReference)
	{
		// reference object
		if( objectDesc->m_pReferenceNodeDescription)
		{
			if( objectDesc->m_Name.IsEmpty())
			{
				MapReferencesNames::iterator	it = mapReferencesNames.find( objectDesc->m_ReferenceName);
				if( it == mapReferencesNames.end())
				{
					mapReferencesNames.insert( MapReferencesNames::value_type(CString(objectDesc->m_ReferenceName), 1));
					it = mapReferencesNames.find( objectDesc->m_ReferenceName);
				}
				else
				{
					it->second++;
				}
				CString	pathReference;	pathReference.Format("%s%u", 
							//objectDesc->m_pReferenceNodeDescription->m_Prefix.GetBuffer(0),
							objectDesc->m_pReferenceNodeDescription->m_Name.GetBuffer(0),
							it->second);
				pathObjectDirectory += pathReference;
			}
			else
			{
				pathObjectDirectory += objectDirectoryName;
			}
		}
		else
		{
			//ATLASSERT( FALSE);
			WriteLog( "Reference object: "+objectDesc->m_ReferenceName+" for object " + objectDesc->m_Name + " not found");
		}
	}
	else
	{
		// normal object
		pathObjectDirectory += objectDirectoryName;
	}
	//if( objectDesc->m_Lod > -1)
	//	pathObjectDirectory += "\\" + GetLodDirName( objectDesc->m_Name, objectDesc->m_Lod);
	pathObjectDirectory.Replace("\\\\","\\");
	return pathObjectDirectory;
}

//////////////////////////////////////////////////////////////////////////
void	CExporter::GetNodeTransform( MAP_NODE_DESCRIPTION* objectDesc, CalVector& translation, CalQuaternion& rotation, CalVector& scale, bool localToParent)
{
	translation.set(0,0,0);
	rotation = CalQuaternion(0,0,0,1);
	scale.set(1, 1, 1);
	m_pInterface->GetTranslationAndRotation(objectDesc->m_pBaseNode, NULL, 0.0, translation, rotation);

//CalQuaternion rtn1(0,0,1,0);	CalVector v1(1,0,0); v1 *= rtn1;
//CalQuaternion rtn2(0,0,0,1);	CalVector v2(1,0,0); v2 *= rtn2;

	if(objectDesc->m_IsReference
		&& objectDesc->m_pParentGroupNode == NULL)
	{
		m_pInterface->GetScale(objectDesc->m_pBaseNode, 0.0, scale);
		if( objectDesc->m_pReferenceNodeDescription)
		{
			//////////////////////////////////////////////////////////////////////////
			// поворот объекта-ссылки
			CalVector parentTranslation;
			CalQuaternion parentRotation;
			m_pInterface->GetTranslationAndRotation(objectDesc->m_pReferenceNodeDescription->m_pBaseNode, NULL, 0.0, parentTranslation, parentRotation);

			//rotation.conjugate();
			//rotation *= parentRotation; //ломает все. Не учитывает поворот самого объекта

			//////////////////////////////////////////////////////////////////////////
			CalVector vParent(1, 1, 1);
			m_pInterface->GetScale(objectDesc->m_pReferenceNodeDescription->m_pBaseNode, 0.0, vParent);
			for(int i=0;i<3;i++)
				if( vParent[i] != 0)
					scale[i] /= vParent[i];
		}
	}
	//
	
	if( localToParent &&
		objectDesc->m_pParentGroupNodeDescription != NULL)
	{
		CalVector		parentTranslation, parentScale;
		CalQuaternion	parentRotation;
		GetNodeTransform(objectDesc->m_pParentGroupNodeDescription,
							parentTranslation, parentRotation, parentScale);

		parentRotation.conjugate();
		rotation *= parentRotation;
		
		translation -= parentTranslation;
		translation *= parentRotation;
	}
	if(CExporter::useFloatVertexCoords == FALSE)
	{
		AlignFloatToWorld( translation.x);
		AlignFloatToWorld( translation.y);
		AlignFloatToWorld( translation.z);
	}
}

//////////////////////////////////////////////////////////////////////////
bool	CExporter::CreateCal3DObjects( MAP_NODE_DESCRIPTION* objectDesc, CMeshCandidate& meshCandidate, CSkeletonCandidate& skeletonCandidate, CalCoreMesh& coreMesh)
{
	ATLASSERT( objectDesc);
	if( IsNodeInReferenceChain(objectDesc))	return false;
	if( objectDesc->m_IsReference)	return false;
	if( objectDesc->m_IsGroupHead)	return false;

	ATLASSERT( objectDesc->m_pBaseNode);

	//////////////////////////////////////////////////////////////////////////
	bool	boundsCalculated = false;
	objectDesc->xmin = FLT_MAX;	objectDesc->ymin = FLT_MAX;	objectDesc->zmin = FLT_MAX;
	objectDesc->xmax = -FLT_MAX;	objectDesc->ymax = -FLT_MAX;	objectDesc->zmax = -FLT_MAX;
	
	m_faces.clear();
	m_vertex.clear();
	m_texCoords.clear();
	m_facesInMaterial.clear();
	m_vertexInMaterial.clear();
	m_texLevelsForVertex.clear();

	// получим трансформацию объекта
	CalVector		m_worldTranslation(0,0,0);
	CalQuaternion	m_worldRotation(0,0,0,1);
	CalVector		m_worldScale(0,0,0);
	GetNodeTransform(objectDesc, m_worldTranslation, m_worldRotation, m_worldScale);

	meshCandidate.Create(&skeletonCandidate, 999, 0.01f, objectDesc->m_pBaseNode);
	meshCandidate.DisableLOD();

	coreMesh.set_worldRotation(m_worldRotation);
	coreMesh.set_worldTranslation(m_worldTranslation);
	m_worldTranslation.invert();
	m_worldRotation.conjugate();

	int	materialThreadId = -1;
	int vertexCount = 0;
	// create the core mesh instance
	if(!coreMesh.create())
	{
		SetLastError("Creation of core mesh instance failed.", __FILE__, __LINE__);
		return false;
	}

	coreMesh.NoSkel( true);

	// get the submesh candidate vector
	std::vector<CSubmeshCandidate *>& vectorSubmeshCandidate = meshCandidate.GetVectorSubmeshCandidate();

	// start the progress info
	m_pInterface->StartProgressInfo("Exporting object ["+Cal3DComString( objectDesc->m_Name)+"] to mesh file...");

	std::vector<CalVector> vertexBuf;
	std::vector<unsigned int> facesBuf;
	float* vx = NULL;
	unsigned int* ofx = NULL;

	int submeshCandidateId;	
	for(submeshCandidateId = 0; submeshCandidateId < vectorSubmeshCandidate.size(); submeshCandidateId++)
	{
		vertexBuf.clear();
		facesBuf.clear();

		// update the progress info
		m_pInterface->SetProgressInfo(100.0f * (float)submeshCandidateId / (float)vectorSubmeshCandidate.size());

		// get the submesh candidate
		CSubmeshCandidate *pSubmeshCandidate;
		pSubmeshCandidate = vectorSubmeshCandidate[submeshCandidateId];

		// get the face vector
		std::vector<CSubmeshCandidate::Face>& vectorFace = pSubmeshCandidate->GetVectorFace();

		m_facesInMaterial.push_back(vectorFace.size());

		if (vectorFace.size() == 0)
		{
			m_vertexInMaterial.push_back(0);
		}

		// check if the submesh actually contains faces
		if(vectorFace.size() > 0)
		{
			materialThreadId++;
			// allocate new core submesh instance
			CalCoreSubmesh *pCoreSubmesh;
			pCoreSubmesh = new CalCoreSubmesh();
			if(pCoreSubmesh == 0)
			{
				SetLastError("Memory allocation failed.", __FILE__, __LINE__);
				coreMesh.destroy();
				m_pInterface->StopProgressInfo();
				return false;
			}

			// create the core submesh instance
			if(!pCoreSubmesh->create())
			{
				SetLastError(CalError::getLastErrorText(), __FILE__, __LINE__);
				delete pCoreSubmesh;
				coreMesh.destroy();
				m_pInterface->StopProgressInfo();
				return false;
			}

			pCoreSubmesh->setCoreMaterialThreadId(materialThreadId);

			// get the vertex candidate vector
			std::vector<CVertexCandidate *>& vectorVertexCandidate = pSubmeshCandidate->GetVectorVertexCandidate();

			// get the spring vector
			std::vector<CSubmeshCandidate::Spring>& vectorSpring = pSubmeshCandidate->GetVectorSpring();

			// reserve memory for all the submesh data
			if(!pCoreSubmesh->reserve(vectorVertexCandidate.size(), pSubmeshCandidate->GetMapCount(), vectorFace.size(), vectorSpring.size()))
			{
				SetLastError(CalError::getLastErrorText(), __FILE__, __LINE__);
				delete pCoreSubmesh;
				coreMesh.destroy();
				m_pInterface->StopProgressInfo();
				return false;
			}

			int vertexCandidateId;
			m_vertexInMaterial.push_back(vectorVertexCandidate.size());
			if( vectorVertexCandidate.size()>0)
				boundsCalculated = true;
			for(vertexCandidateId = 0; vertexCandidateId < vectorVertexCandidate.size(); vertexCandidateId++)
			{
				// get the vertex candidate
				CVertexCandidate *pVertexCandidate;
				pVertexCandidate = vectorVertexCandidate[vertexCandidateId];

				CalCoreSubmesh::Vertex vertex;

				// set the vertex position
				CalVector position;
				pVertexCandidate->GetPosition(vertex.position);
				vertex.position += m_worldTranslation;
				vertex.position *= m_worldRotation;

				objectDesc->xmin = min( objectDesc->xmin, vertex.position.x);
				objectDesc->ymin = min( objectDesc->ymin, vertex.position.y);
				objectDesc->zmin = min( objectDesc->zmin, vertex.position.z);

				objectDesc->xmax = max( objectDesc->xmax, vertex.position.x);
				objectDesc->ymax = max( objectDesc->ymax, vertex.position.y);
				objectDesc->zmax = max( objectDesc->zmax, vertex.position.z);

				// set the vertex normal
				CalVector normal;
				pVertexCandidate->GetNormal(vertex.normal);
				vertex.normal *= m_worldRotation;

				//m_vertex.push_back(vertex.position);
				vertexBuf.push_back(vertex.position);
				
				// set the collapse id
				vertex.collapseId = pVertexCandidate->GetCollapseId();

				// set the face collapse count
				vertex.faceCollapseCount = pVertexCandidate->GetFaceCollapseCount();

				// get the texture coordinate vector
				std::vector<CVertexCandidate::TextureCoordinate>& vectorTextureCoordinate = pVertexCandidate->GetVectorTextureCoordinate();

				m_texLevelsForVertex.push_back(vectorTextureCoordinate.size());
				
				// set all texture coordinates
				int textureCoordinateId;
				for(textureCoordinateId = 0; textureCoordinateId < vectorTextureCoordinate.size(); textureCoordinateId++)
				{
					CalCoreSubmesh::TxCoor textureCoordinate;
					textureCoordinate.u = vectorTextureCoordinate[textureCoordinateId].u;
					textureCoordinate.v = vectorTextureCoordinate[textureCoordinateId].v;

					// set texture coordinate
					pCoreSubmesh->setTextureCoordinate(pVertexCandidate->GetLodId(), textureCoordinateId, textureCoordinate);

					m_texCoords.push_back(CalVector(textureCoordinate.u, textureCoordinate.v, 1));
				}

				if( !skeletonCandidate.IsNull()){
					// get the influence vector
					std::vector<CVertexCandidate::Influence>& vectorInfluence = pVertexCandidate->GetVectorInfluence();

					// reserve memory for the influences in the vertex
					vertex.vectorInfluence.reserve(vectorInfluence.size());
					vertex.vectorInfluence.resize(vectorInfluence.size());

					// set all influences
					int influenceId;
					for(influenceId = 0; influenceId < vectorInfluence.size(); influenceId++)
					{
						vertex.vectorInfluence[influenceId].boneId = vectorInfluence[influenceId].boneId;
						vertex.vectorInfluence[influenceId].weight = vectorInfluence[influenceId].weight;
					}
				}

				// set vertex in the core submesh instance
				pCoreSubmesh->setVertex(pVertexCandidate->GetLodId(), vertex);

				// set the physical property if there is a spring system
				if(vectorSpring.size() > 0)
				{
					// get the physical property vector
					CVertexCandidate::PhysicalProperty physicalPropertyCandidate;
					pVertexCandidate->GetPhysicalProperty(physicalPropertyCandidate);

					CalCoreSubmesh::PhysicalProperty physicalProperty;
					physicalProperty.weight = physicalPropertyCandidate.weight;

					// set the physical property in the core submesh instance
					pCoreSubmesh->setPhysicalProperty(vertexCandidateId, physicalProperty);

				}
			}

			int faceId;
			for(faceId = 0; faceId < vectorFace.size(); faceId++)
			{
				CalCoreSubmesh::Face face;

				// set the vertex ids
				face.vertexId[0] = vectorFace[faceId].vertexLodId[0];
				face.vertexId[1] = vectorFace[faceId].vertexLodId[1];
				face.vertexId[2] = vectorFace[faceId].vertexLodId[2];

				//m_faces.push_back(face.vertexId[0] + vertexCount);
				//m_faces.push_back(face.vertexId[1] + vertexCount);
				//m_faces.push_back(face.vertexId[2] + vertexCount);
				facesBuf.push_back(face.vertexId[0]);
				facesBuf.push_back(face.vertexId[1]);
				facesBuf.push_back(face.vertexId[2]);

				// set face in the core submesh instance
				pCoreSubmesh->setFace(vectorFace[faceId].lodId, face);
			}

			int springId;
			for(springId = 0; springId < vectorSpring.size(); springId++)
			{
				CalCoreSubmesh::Spring spring;

				// set the vertex ids
				spring.vertexId[0] = vectorSpring[springId].vertexId[0];
				spring.vertexId[1] = vectorSpring[springId].vertexId[1];
				spring.springCoefficient = vectorSpring[springId].springCoefficient;
				spring.idleLength = vectorSpring[springId].idleLength;

				// set face in the core submesh instance
				pCoreSubmesh->setSpring(springId, spring);
			}

			// set the LOD step count
			pCoreSubmesh->setLodCount(pSubmeshCandidate->GetLodCount());

			// add the core submesh to the core mesh instance
			coreMesh.addCoreSubmesh(pCoreSubmesh);

//////////////////////////////////////////////////////////////////////////
// ATI Tootle & NVidia Face Optimization
			int optimizeResult = 0;
			
			vx = new float[3*vertexBuf.size()];
			for (int v = 0; v < vertexBuf.size(); v ++)
			{
				vx[3*v] = vertexBuf[v].x;
				vx[3*v+1] = vertexBuf[v].y;
				vx[3*v+2] = vertexBuf[v].z;
			}
		
			if(TootleIsInited)
			{
				unsigned int* fx = new unsigned int[facesBuf.size()];
				for (int f = 0; f < facesBuf.size(); f++)
				{
					fx[f] = facesBuf[f];
				}						

				ofx = new unsigned int[facesBuf.size()];

				int tempVc = vertexBuf.size();
				int tempFc = facesBuf.size();


				optimizeResult = TootleOptimize(
					vx,
					fx,
					vertexBuf.size(),
					facesBuf.size()/3,					
					3*sizeof(float),
					TOOTLE_DEFAULT_VCACHE_SIZE,
					NULL,
					0,
					TOOTLE_CW,
					ofx,
					NULL);

				delete[] fx;
			}
			else
			{
				unsigned short* fx = new unsigned short[facesBuf.size()];
				for (int f = 0; f < facesBuf.size(); f++)
				{
					fx[f] = facesBuf[f];
					unsigned short tmp = fx[f];
				}

				int tempVc = vertexBuf.size();
				int tempFc = facesBuf.size();

				// use NVidia optimization
				SetCacheSize(CACHESIZE_GEFORCE3);
				SetStitchStrips(true);
				SetMinStripSize(0);
				SetListsOnly(true);

				PrimitiveGroup* primitiveGroupsList = NULL;
				unsigned short numSections = NULL;
				optimizeResult = GenerateStrips(fx, facesBuf.size(), &primitiveGroupsList, &numSections);

				int ml = primitiveGroupsList[0].numIndices; // 630

				ofx = new unsigned int[primitiveGroupsList[0].numIndices];

				for (int i = 0; i < primitiveGroupsList[0].numIndices; i++)
				{
					ofx[i] = primitiveGroupsList[0].indices[i];
				}
				
				delete[] primitiveGroupsList;
				delete[] fx;
			}
					
			for (int i = 0; i < vertexBuf.size(); i++)
			{
				m_vertex.push_back(vertexBuf[i]);
			}

			if (optimizeResult == TRUE)
			{
				for (int i = 0; i < facesBuf.size(); i++)
				{
					m_faces.push_back(ofx[i]+vertexCount);
				}				
			}
			else
			{
				for (int i = 0; i < facesBuf.size(); i++)
				{
					m_faces.push_back(facesBuf[i]+vertexCount);
				}
			}
			
			delete[] vx;

			if (ofx != NULL)
			{
				delete[] ofx;
			}

			vertexCount += pCoreSubmesh->getVectorVertex().size();
		}
	}

	if( !boundsCalculated)
	{
		objectDesc->xmin = m_worldTranslation.x;
		objectDesc->ymin = m_worldTranslation.y;
		objectDesc->zmin = m_worldTranslation.z;

		objectDesc->xmax = m_worldTranslation.x;
		objectDesc->ymax = m_worldTranslation.y;
		objectDesc->zmax = m_worldTranslation.z;

		objectDesc->xmin = 0;
		objectDesc->ymin = 0;
		objectDesc->zmin = 0;

		objectDesc->xmax = 0;
		objectDesc->ymax = 0;
		objectDesc->zmax = 0;
	}

	// stop the progress info
	m_pInterface->StopProgressInfo();
	return true;
}
//////////////////////////////////////////////////////////////////////////
PRIMITIVE_DESCRIPTION	CExporter::GetColliderObject(MAP_NODE_DESCRIPTION* objectDesc)
{
	PRIMITIVE_DESCRIPTION colliderObj;

	// установим тип примитива
	for (int i = 0; i < objectDesc->m_Properties.size(); i++)
	{
		if (objectDesc->m_Properties[i].key == "collider_type")
		{
			if (objectDesc->m_Properties[i].value == "box")
			{
				colliderObj.type = 1;
			}
			else if (objectDesc->m_Properties[i].value == "sphere")
			{
				colliderObj.type = 2;
			}
		}
	}

	// установим размеры примитива
	CalCoreMesh			coreMesh;
	CMeshCandidate		meshCandidate;
	CSkeletonCandidate	skeletonCandidate;

	CreateCal3DObjects(objectDesc, meshCandidate, skeletonCandidate, coreMesh);

    colliderObj.xmin = objectDesc->xmin;
	colliderObj.ymin = objectDesc->ymin;
	colliderObj.zmin = objectDesc->zmin;
	colliderObj.xmax = objectDesc->xmax;
	colliderObj.ymax = objectDesc->ymax;
	colliderObj.zmax = objectDesc->zmax;

	// установим трансформацию примитива
	GetNodeTransform(objectDesc, colliderObj.translation, colliderObj.rotation, colliderObj.scale);

	//colliderObj.translation.invert();
	//colliderObj.rotation.conjugate();	

	coreMesh.destroy();

	return colliderObj;
}
//////////////////////////////////////////////////////////////////////////
SITPLACE_DESCRIPTION	CExporter::GetSitplaceObject(MAP_NODE_DESCRIPTION* objectDesc)
{
	SITPLACE_DESCRIPTION sitplaceObj;

	// установим трансформацию сиденья
	CalVector scaleBuf;
	GetNodeTransform(objectDesc, sitplaceObj.translation, sitplaceObj.rotation, scaleBuf);

	return sitplaceObj;
}
//////////////////////////////////////////////////////////////////////////
void	CExporter::FillObjectConfig( MAP_NODE_DESCRIPTION* objectDesc, CString meshFileName, CString skelFileName, SObjectCfg* objectCfg)
{
	ATLASSERT( objectCfg);

	objectCfg->is_reference = FALSE;
	objectCfg->boxes.clear();
	objectCfg->collision_src = "";
	objectCfg->collision_type = "";
	objectCfg->materials.clear();
	//objectCfg->mesh_src = "";
	objectCfg->mesh_srcs.clear();
	objectCfg->properties.clear();
	objectCfg->scale = 0.0f;
	objectCfg->shader = "";
	objectCfg->skeleton_src = "";
	//////////////////////////////////////////////////////////////////////////
	//CExporter::clearObjectCfg();
	objectCfg->collision_type = "AABB";
	objectCfg->scale=1.0f;

	if( objectDesc->m_IsReference)
	{
		ATLASSERT( objectDesc->m_pReferenceNodeDescription);
		if( objectDesc->m_pReferenceNodeDescription)
		{
			CalVector	translation, scale;
			CalQuaternion	rotation;
			GetNodeTransform( objectDesc, translation, rotation, scale);
			objectCfg->is_reference = TRUE;
			objectCfg->ref_name = objectDesc->m_ReferenceName;
			//objectCfg->ref_name = objectDesc->m_pReferenceNodeDescription->m_Prefix+"\\"+objectDesc->m_pReferenceNodeDescription->m_Name;
			objectCfg->ref_name = objectDesc->m_pReferenceNodeDescription->m_Name;
			objectCfg->ref_translation = translation;
			objectCfg->ref_rotation = rotation;
			objectCfg->ref_scale = scale;
		}
	}
	else
	{
		//objectCfg->mesh_src = meshFileName;
		objectCfg->mesh_srcs.push_back( Cal3DComString( meshFileName));
		objectCfg->skeleton_src = skelFileName;

		for (int iprops=0; iprops < objectDesc->m_Properties.size(); iprops++)
		{
			CString	propName;
			CString	propValue = "true";
			TWO_STRING	property = objectDesc->m_Properties[iprops];
			property.key.MakeLower();
			property.value.MakeLower();
			if (property.key=="oob" || property.key=="aabb")
			{
				objectCfg->collision_type = property.key;
			}
			else if (property.key=="int")
			{
				propName = "intangible";
			}
			else if (property.key=="flip")
			{
				propName = "flip_faces";
			}
			else if (property.key=="shader")
			{
				objectCfg->shader = property.value;
			}
			else
			{
				propName = property.key;
				propValue = property.value;
			}

			// create new property and push it into objectCfg
			if( !propName.IsEmpty())
			{
				_property prop;
				prop.name = propName;
				prop.value = propValue;
				objectCfg->properties.push_back(prop);
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////
void	GetMaterialsIds( CMeshCandidate& meshCandidate, std::vector<int>*	materialsIds)
{
	ATLASSERT(materialsIds);

	int	materialThreadId = 0;
	// get the submesh candidate vector
	std::vector<CSubmeshCandidate *>& vectorSubmeshCandidate = meshCandidate.GetVectorSubmeshCandidate();
	
	int submeshCandidateId;
	for(submeshCandidateId = 0; submeshCandidateId < vectorSubmeshCandidate.size(); submeshCandidateId++)
	{
		// get the submesh candidate
		CSubmeshCandidate *pSubmeshCandidate;
		pSubmeshCandidate = vectorSubmeshCandidate[submeshCandidateId];

		// get the face vector
		std::vector<CSubmeshCandidate::Face>& vectorFace = pSubmeshCandidate->GetVectorFace();

		// check if the submesh actually contains faces
		if(vectorFace.size() > 0)
		{
			// allocate new core submesh instance
			materialsIds->push_back(materialThreadId);
		}
		materialThreadId++;
	}
}

//////////////////////////////////////////////////////////////////////////
BOOL CreateAbsolutePath( CString filePath)
{
	SECFileSystem	fd;
	if( fd.DirectoryExists( filePath))	return TRUE;
	return fd.MakePath( filePath);
}

//////////////////////////////////////////////////////////////////////////
void	SaveSceneConfig( CString	pathSceneFile, std::vector<TWO_STRING>& vecObjects)
{
	CFile	file;

	try
	{
		file.Open(pathSceneFile, CFile::modeCreate | CFile::modeWrite);
	}
	catch(...)
	{
	}

	for (int i=0; i < vecObjects.size(); i++)
	{
		CString	fline = vecObjects[ i].key;
		if( !vecObjects[ i].value.IsEmpty())
			fline += "=" + vecObjects[ i].value + "\r\n";
		else
			fline += "\r\n";

		//vecObjects[i].value.Replace("xml", "bmf");
		//CString	fline = vecObjects[i].key + "=" + vecObjects[i].value + "\n";

		file.Write( fline.GetBuffer(0), fline.GetLength());
	}
	file.Close();
}

CString CExporter::GetMaterialName(int mId, CMeshCandidate& meshCandidate, std::vector<int>& materialsIds,
								MaterialsMap& mapMaterialsNames, MAP_NODE_DESCRIPTION*	objectDesc,
								SECFileSystem* fs, CString& pathAbsoluteObjectDirectory,
								CString& pathMaterials)
{
	StdMat*	material = meshCandidate.GetMtrl(materialsIds[mId]);
	CString materialFileName;
	ASSERT(material);
	if( material)
	{
		materialFileName.Format("%s.crf", material->GetName());
		if( calexpoptGetnMtrlsNames)
		{
			MaterialsMap::iterator	it = mapMaterialsNames.find(materialFileName);
			if( it == mapMaterialsNames.end())
			{
				CString	genName;	genName.Format( "mtrl%d.crf", mapMaterialsNames.size()+1);
				mapMaterialsNames.insert( MaterialsMap::value_type( materialFileName, genName));
				materialFileName = genName;
			}
			else
			{
				materialFileName = it->second;
			}
		}
		if( !objectDesc->m_IsReference)
		{
			_material	mtrl;
			mtrl.src = Cal3DComString(materialFileName.GetBuffer(0));
			objectCfg.materials.push_back(mtrl);
		}
		CString	absPathMaterial = fs->AppendPaths(pathAbsoluteObjectDirectory,  pathMaterials);
		CreateAbsolutePath( absPathMaterial);
		materialFileName = absPathMaterial + materialFileName;
	}

	return materialFileName;
}

//////////////////////////////////////////////////////////////////////////
void	SaveSceneVESpaceConfig( CString	pathSceneFile, std::vector<TWO_STRING>& vecObjects)
{
	SaveSceneConfig( pathSceneFile+".scn", vecObjects);
}

//////////////////////////////////////////////////////////////////////////
void	SaveSceneVESpaceSQL( CString	pathSceneFile, std::vector<TWO_STRING>& vecObjects)
{
	SaveSceneConfig( pathSceneFile+".mysql", vecObjects);
}

void	SaveSceneVESpaceXML( CString	pathSceneFile, std::vector<TWO_STRING>& vecObjects)
{
	vecObjects.insert(vecObjects.begin(), TWO_STRING("<xml encoding=\"CP1251\">", ""));
	vecObjects.push_back(TWO_STRING("</xml>", ""));
	SaveSceneConfig( pathSceneFile+".xml", vecObjects);
}

//////////////////////////////////////////////////////////////////////////
//	INSERT INTO `objects_map` ( `id` , `res` , `version` , `px` , `py` , `pz` , `rx` , `ry` , `rz` , `rw` , `bounds` , `sqId` ) 
//	VALUES (
//	'1', 'resources/houdse01/mesh.xml', '1', '100', '150', '12.3', '0.23', '0.21', '0.2', '0.24', '-10;-10;-10;10;10;10', '1'
//	);

long GetZoneCenterCoordinateByValue( float val)
{
	int zoneID = ((int)(val / ZONE_SIZE));
	if( val < 0)	zoneID -= 1;
	return zoneID * ZONE_SIZE + ZONE_SIZE/2;
}

DWORD GetZoneIDByCoordinates(float ax, float ay)
{
	int	ny = (int)(ay/20000.0f) + 16000;
	int	nx = (int)(ax/20000.0f) + 16000;
	if( ay < 0)	ny -= 1;
	if( ax < 0)	nx -= 1;
	DWORD sqID = (ny<<15) + nx;
	return sqID;
}

CString	GetSQLObjectInsertionString(DWORD objectID, 
										vespace::VESPACE_OBJECTS_TYPES	eType,
										Cal3DComString name, 
										Cal3DComString reference, 
										Cal3DComString res,
										unsigned int level, unsigned int lod,
										Cal3DComString aclass, DWORD version, 
										CalVector translation, CalVector scale, CalQuaternion rotation,
										CalVector boundsVector,
										DWORD sqID,
										CString* aparams, std::vector<SITPLACE_DESCRIPTION> sitplaces,
										BOOL grass,
										Cal3DComString zoneName,
										std::map<Cal3DComString, CalVector>& internalZonePivots,
										std::vector<TWO_STRING> props)
{
	if( eType == vespace::VE_UNDEFINED)
	{
		if( res.Find(".bmf")!=-1)	eType = vespace::VE_OBJECT3D;
		else	if( res.Find(".scn")!=-1 || res.Find(".scml")!=-1)	eType = vespace::VE_GROUP3D;
	}

	CString sitplaceDescription = "{\"action\":\"pose\",";
	if (!sitplaces.empty())
	{
		for (int sitIndex = 0; sitIndex < sitplaces.size(); sitIndex++)
		{
			SITPLACE_DESCRIPTION sitplace = sitplaces[sitIndex];
			CString sitBuf = "";

			sitplace.translation = sitplace.translation - translation;
			CalQuaternion cpyRotation = rotation;
			cpyRotation.w = -cpyRotation.w; 
			sitplace.rotation = cpyRotation*sitplace.rotation;
			sitplace.rotation.Normalize();

			float angle = 0;
			CalVector axes;

			sitplace.rotation.extract(axes, angle);

			angle = (angle*180.0f)/PI;

			float r = (float)sqrtf(sitplace.rotation.x * sitplace.rotation.x + 
				sitplace.rotation.y * sitplace.rotation.y + sitplace.rotation.z * sitplace.rotation.z);

			if (r < 0.000001)
			{
				axes.set(0, 0, 1);
				angle = 0;
			}

			CString sitPose = "_sitting.caf";
			for (int i = 0; i < props.size(); i++)
			{
				if (props[i].key == "pose")
				{
					sitPose = props[i].value;
					break;
				}
			}

			sitBuf.Format("\"sitting%d\":{\"tx\":%.8f,\"ty\":%.8f,\"tz\":%.8f,\"rx\":%.8f,\"ry\":%.8f,\"rz\":%.8f,\"rw\":%.8f,\"motion\":\"%s\"}",
				(sitIndex + 1),
				sitplace.translation.x,
				sitplace.translation.y,
				sitplace.translation.z,
				axes.x,
				axes.y,
				axes.z,
				angle,
				sitPose.GetBuffer(0));

			if (sitIndex != 0)
			{
				sitplaceDescription += ",";
			}

			sitplaceDescription += sitBuf;
		}

		sitplaceDescription += "},{\"action\":\"stand\"}";
		aparams->Format("%s", sitplaceDescription.GetBuffer(0));	
//		sitplaceDescription.Format("{\"action\":\"pose\",\"tx\":%.8f,\"ty\":%.8f,\"tz\":%.8f,\"rx\":%.8f,\"ry\":%.8f,\"rz\":%.8f,\"rw\":%.8f,\"motion\":\"_sitting.bvh\"}{\"action\":\"stand\"}",
	}
	else
	{
		sitplaceDescription = aparams->GetBuffer(0);
	}

	if (zoneName == "")
	{
		if (!CExporter::calexpoptGenerateGlobalCoords)
		{
			float zoneCenterX = GetZoneCenterCoordinateByValue(translation.x);
			translation.x -= zoneCenterX;
			float zoneCenterY = GetZoneCenterCoordinateByValue(translation.y);
			translation.y -= zoneCenterY;
		}
	}
	else
	{
		std::map<Cal3DComString, CalVector>::iterator iter = internalZonePivots.find(zoneName);

		if (iter == internalZonePivots.end())
		{
			ATLASSERT(FALSE);
		}
		else
		{
			if (!CExporter::calexpoptGenerateGlobalCoords)
			{
				translation.x -= (*iter).second.x;
				translation.y -= (*iter).second.y;
				translation.x += -ZONE_SIZE/2.0f;
				translation.y += -ZONE_SIZE/2.0f;
			}			
		}
	}

	CString ret;
	ret.Format("INSERT INTO `objects_map` ( `id` , `type`, `res` , `name`, `reference`, `level`, `lod`, `class`, `version` , `px` , `py` , `pz` , `rx` , `ry` , `rz` , `rw`, `bx` , `by` , `bz` , `sqId` , `params`, `grass`, `scalex`, `scaley`, `scalez`) VALUES('%u', '%u', '%s', '%s', '%s', '%u', '%u', '%s', '%u', '%.8f','%.8f','%.8f', '%.8f','%.8f','%.8f','%.8f', '%.8f','%.8f','%.8f', '%u', '%s', '%u', '%.8f','%.8f','%.8f');",
		objectID, 
		(unsigned int) eType,
		res.GetBuffer(0), name.GetBuffer(0), reference.GetBuffer(0), level, lod, aclass.GetBuffer(0), version, 
		translation.x, translation.y, translation.z,
		rotation.x, rotation.y, rotation.z, rotation.w, 
		boundsVector.x, boundsVector.y, boundsVector.z, 
		sqID,
		//aparams.GetBuffer(0),
		sitplaceDescription,
		grass,
		scale.x,scale.y,scale.z);
	ret.Replace("\\", "\\\\");
	return ret;
}

CString	GetXMLObjectInsertionString(DWORD objectID, 
										vespace::VESPACE_OBJECTS_TYPES	eType,
										Cal3DComString name, 
										Cal3DComString reference, 
										Cal3DComString res,
										unsigned int level, unsigned int lod,
										Cal3DComString aclass, DWORD version, 
										CalVector translation, CalVector scale, CalQuaternion rotation,
										CalVector boundsVector,
										DWORD sqID,
										CString* aparams, std::vector<SITPLACE_DESCRIPTION> sitplaces,
										BOOL grass,
										Cal3DComString zoneName,
										Cal3DComString logicObjName,
										std::map<Cal3DComString, CalVector>& internalZonePivots,
										std::vector<TWO_STRING> props)
{
	if( eType == vespace::VE_UNDEFINED)
	{
		if (res.Find(".bmf") != -1)	
		{
			eType = vespace::VE_OBJECT3D;
		}
		else if (res.Find(".scn") != -1 || res.Find(".scml") != -1)
		{
			eType = vespace::VE_GROUP3D;
		}
	}

	if(reference.Compare("ref") == 0)
	{
		reference = "";
		/*CalVector rotAxes;
		float rotAngle = 0;

		rotation.extract(rotAxes, rotAngle);

		CalQuaternion newRotation(rotAxes, rotAngle + 1);

		rotation = newRotation;*/
	}

	CString sitplaceDescription = "{\"action\":\"pose\",";
	if (!sitplaces.empty())
	{
		for (int sitIndex = 0; sitIndex < sitplaces.size(); sitIndex++)
		{
			SITPLACE_DESCRIPTION sitplace = sitplaces[sitIndex];
			CString sitBuf = "";

			sitplace.translation = sitplace.translation - translation;
			CalQuaternion cpyRotation = rotation;
			cpyRotation.conjugate();
			sitplace.translation *= cpyRotation;
			
			//cpyRotation.w = -cpyRotation.w; 
			sitplace.rotation = cpyRotation*sitplace.rotation;

			float angle = 0;
			CalVector axes;

			sitplace.rotation.extract(axes, angle);

			angle = (angle*180.0f)/PI;

			if(angle > 180 || angle < -180)
			{
				angle = -360 * (angle/abs(angle)) + angle;
			}		

			float r = (float)sqrtf(sitplace.rotation.x * sitplace.rotation.x + 
				sitplace.rotation.y * sitplace.rotation.y + sitplace.rotation.z * sitplace.rotation.z);

			if (r < 0.000001)
			{
				axes.set(0, 0, 1);
				angle = 0;
			}

			CString sitPose = "_sitting.caf";
			for (int i = 0; i < props.size(); i++)
			{
				if (props[i].key == "pose")
				{
					sitPose = props[i].value;
					break;
				}
			}

			sitBuf.Format("\"sitting%d\":{\"tx\":%.8f,\"ty\":%.8f,\"tz\":%.8f,\"rx\":%.8f,\"ry\":%.8f,\"rz\":%.8f,\"rw\":%.8f,\"motion\":\"%s\"}",
				(sitIndex + 1),
				sitplace.translation.x,
				sitplace.translation.y,
				sitplace.translation.z,
				axes.x,
				axes.y,
				axes.z,
				angle,
				sitPose.GetBuffer(0));

			if (sitIndex != 0)
			{
				sitplaceDescription += ",";
			}

			sitplaceDescription += sitBuf;
		}

		sitplaceDescription += "},{\"action\":\"stand\"}";
		aparams->Format("%s", sitplaceDescription.GetBuffer(0));	
		//		sitplaceDescription.Format("{\"action\":\"pose\",\"tx\":%.8f,\"ty\":%.8f,\"tz\":%.8f,\"rx\":%.8f,\"ry\":%.8f,\"rz\":%.8f,\"rw\":%.8f,\"motion\":\"_sitting.bvh\"}{\"action\":\"stand\"}",
	}
	else
	{
		sitplaceDescription = aparams->GetBuffer(0);
	}

	for(int propIndex = 0; propIndex < props.size(); propIndex++)
	{
		if (props[propIndex].key == "cam_name")
		{
			if (!sitplaceDescription.IsEmpty())
			{
				sitplaceDescription += ",";
			}
			sitplaceDescription += "{\"cam_name\":\"" + props[propIndex].value + "\"}";
		}
	}

	if (zoneName == "")
	{
		if (!CExporter::calexpoptGenerateGlobalCoords)
		{
			float zoneCenterX = GetZoneCenterCoordinateByValue(translation.x);
			translation.x -= zoneCenterX;
			float zoneCenterY = GetZoneCenterCoordinateByValue(translation.y);
			translation.y -= zoneCenterY;
		}
	}
	else
	{
		std::map<Cal3DComString, CalVector>::iterator iter = internalZonePivots.find(zoneName);

		if (iter == internalZonePivots.end())
		{
			ATLASSERT(FALSE);
		}
		else
		{
			if (!CExporter::calexpoptGenerateGlobalCoords)
			{
				translation.x -= (*iter).second.x;
				translation.y -= (*iter).second.y;
				translation.x += -ZONE_SIZE/2.0f;
				translation.y += -ZONE_SIZE/2.0f;
			}			
		}
	}

	Cal3DComString libString = " ";
	if(CExporter::libExport)
	{
		for(int propIndex = 0; propIndex < props.size(); propIndex++)
		{
			if (props[propIndex].key == "cat")
			{
				libString += "category='" + props[propIndex].value + "' ";
			}			
			if (props[propIndex].key == "subcat")
			{
				libString += "subcategory='" + props[propIndex].value + "' ";
			}			
			if (props[propIndex].key == "name2")
			{
				libString += "name_display='" + props[propIndex].value + "' ";
			}			

		}		
	}

	CString ret;
	ret.Format("<set id='%u' type='%u' res='%s' name='%s' reference='%s' level='%u' lod='%u' class='%s' version='%u' px='%.8f' py='%.8f' pz='%.8f' scalex='%.8f' scaley='%.8f' scalez='%.8f' rx='%.8f' ry='%.8f' rz='%.8f' rw='%.8f' bx='%.8f' by='%.8f' bz='%.8f' sqId='%u' params='%s' grass='%u' zonename='%s' logicobj='%s'%s/>",
		objectID, 
		(unsigned int) eType,
		res.GetBuffer(0), name.GetBuffer(0), reference.GetBuffer(0), level, lod, aclass.GetBuffer(0), version, 
		//fmodf(translation.x, 200.0f) - 100.0f, fmodf(translation.y, 200.0f) - 100.0f, /*fmodf(translation.z, 200.0f) - 100.0f*/translation.z,
		translation.x, translation.y, translation.z,
		scale.x, scale.y, scale.z,
		rotation.x, rotation.y, rotation.z, rotation.w, 
		boundsVector.x, boundsVector.y, boundsVector.z, 
		sqID,
		//aparams.GetBuffer(0),
		sitplaceDescription,
		grass,
		zoneName.GetBuffer(0),
		logicObjName.GetBuffer(0),
		libString.GetBuffer(0));
	ret.Replace("\\", "\\\\");
	return ret;
}

void CExporter::GetNodeParentBounds(MAP_NODE_DESCRIPTION* objectDesc, std::vector<MAP_NODE_DESCRIPTION>* vecSelectedObjects, CalVector& amin, CalVector& amax)
{
	CalVector	vmin( objectDesc->xmin, objectDesc->ymin, objectDesc->zmin);
	CalVector	vmax( objectDesc->xmax, objectDesc->ymax, objectDesc->zmax);
	if( objectDesc->m_IsGroupHead &&
		!objectDesc->m_ExplodeGroup)
	{
		GetGroupBounds(objectDesc, vecSelectedObjects, vmin, vmax);
	}
	if( objectDesc->m_pReferenceNodeDescription)
	{
		vmin.set( objectDesc->m_pReferenceNodeDescription->xmin, objectDesc->m_pReferenceNodeDescription->ymin, objectDesc->m_pReferenceNodeDescription->zmin);
		vmax.set( objectDesc->m_pReferenceNodeDescription->xmax, objectDesc->m_pReferenceNodeDescription->ymax, objectDesc->m_pReferenceNodeDescription->zmax);
	}
	amin.set(FLT_MAX,FLT_MAX,FLT_MAX);
	amax.set(-FLT_MAX,-FLT_MAX,-FLT_MAX);
	CalVector		translation, scale;
	CalQuaternion	rotation;
	GetNodeTransform( objectDesc, translation, rotation, scale, true);
	CalVector	verts[8] = {CalVector( vmin.x, vmin.y, vmin.z),
							CalVector( vmin.x, vmax.y, vmin.z),
							CalVector( vmax.x, vmax.y, vmin.z),
							CalVector( vmax.x, vmin.y, vmin.z),
							CalVector( vmin.x, vmin.y, vmax.z),
							CalVector( vmin.x, vmax.y, vmax.z),
							CalVector( vmax.x, vmax.y, vmax.z),
							CalVector( vmax.x, vmin.y, vmax.z)};
	for (int i=0; i<8; i++)
	{
		verts[i] *= rotation;
		verts[i].x *= scale.x;
		verts[i].y *= scale.y;
		verts[i].z *= scale.z;
		verts[i] += translation;
		amin.x = min(amin.x, verts[i].x);	amin.y = min(amin.y, verts[i].y);	amin.z = min(amin.z, verts[i].z);
		amax.x = max(amax.x, verts[i].x);	amax.y = max(amax.y, verts[i].y);	amax.z = max(amax.z, verts[i].z);
	}
}

//////////////////////////////////////////////////////////////////////////
void CExporter::GetGroupBounds(MAP_NODE_DESCRIPTION* objectDesc, std::vector<MAP_NODE_DESCRIPTION>* vecSelectedObjects, CalVector& amin, CalVector& amax)
{
	ATLASSERT( objectDesc);
	amin.set( FLT_MAX, FLT_MAX, FLT_MAX);
	amax.set( -FLT_MAX, -FLT_MAX, -FLT_MAX);
	std::vector<MAP_NODE_DESCRIPTION>::iterator	it = vecSelectedObjects->begin(),
												end = vecSelectedObjects->end();
	for ( ;it!=end;it++)
	{
		if( it->m_pParentGroupNodeDescription == objectDesc)
		{
			CalVector	vmin, vmax;
			GetNodeParentBounds( &(*it), vecSelectedObjects, vmin, vmax);

			amin.x = min( amin.x, vmin.x);
			amin.y = min( amin.y, vmin.y);
			amin.z = min( amin.z, vmin.z);

			amax.x = max( amax.x, vmax.x);
			amax.y = max( amax.y, vmax.y);
			amax.z = max( amax.z, vmax.z);
		}
	}
}

void CExporter::OpenZoneDescFile(CString strFilename)
{
	ZoneDescFile = fopen(strFilename.GetBuffer(0), "w");
	fprintf(ZoneDescFile, "<zones>\n");
}

void CExporter::WriteZoneDesc(std::vector<MAP_NODE_DESCRIPTION>& descs)
{
	assert(ZoneDescFile);
	if( !ZoneDescFile)	return;
	
	if (descs.size() == 0)
	{
		return;
	}

	assert(ZoneDescFile);

	CString strZone, strName, strPortal, strGlobalZoneVisible;

	strGlobalZoneVisible = "false";

	for (unsigned int k = 0; k < descs.size(); k++)
	{
		MAP_NODE_DESCRIPTION desc = descs[k];

		if (desc.m_UserDefinedProperties.Find("[globalzonevisible]") != -1)
		{
			strGlobalZoneVisible = "true";
		}
	}
	
	MAP_NODE_DESCRIPTION desc = descs[0];

	fprintf(ZoneDescFile, "\t<zone name=\"%s\" globalzonevisible=\"%s\">\n", desc.m_Name, strGlobalZoneVisible);
	Cal3DComString zoneName = desc.m_Name;
	
	int k;
	for (k = 0; k < descs.size(); k++)
	{
		desc = descs[k];

		CalCoreMesh			coreMesh;
		CMeshCandidate		meshCandidate;
		CSkeletonCandidate	skeletonCandidate;
		CreateCal3DObjects(&desc, meshCandidate, skeletonCandidate, coreMesh);

		CalVector		worldTranslation(0, 0, 0);
		CalQuaternion	worldRotation(0, 0, 0, 1);
		CalVector		worldScale(0, 0, 0);
		GetNodeTransform(&desc, worldTranslation, worldRotation, worldScale);

		//int	ny = worldTranslation.x / 20000.0f + 16000;
		//int	nx = worldTranslation.y / 20000.0f + 16000;

		//DWORD sqID = (ny << 15) + nx;
		DWORD sqID = GetZoneIDByCoordinates( worldTranslation.x, worldTranslation.y);
		long nx = GetZoneCenterCoordinateByValue( worldTranslation.x);
		long ny = GetZoneCenterCoordinateByValue( worldTranslation.y);

		float zoneCenterX = GetZoneCenterCoordinateByValue(worldTranslation.x);
		worldTranslation.x -= zoneCenterX;
		float zoneCenterY = GetZoneCenterCoordinateByValue(worldTranslation.y);
		worldTranslation.y -= zoneCenterY;

		std::map<Cal3DComString, CalVector>::iterator iter = m_internalZonePivots.find(zoneName);
		if (iter == m_internalZonePivots.end())
		{
			CalVector pivot((nx - 16000.0f) * 20000.0f, (ny - 16000.0f) * 20000.0f, 0);
			m_internalZonePivots.insert(std::map<Cal3DComString, CalVector>::value_type(zoneName, pivot));
		}

		CalVector minC(desc.xmin, desc.ymin, desc.zmin), maxC(desc.xmax, desc.ymax, desc.zmax);
		CalVector size = (maxC - minC) / 2.0f;
		fprintf(ZoneDescFile, "\t\t<boundingbox size=\"%f;%f;%f\" rotation=\"%f;%f;%f;%f\" scale=\"%f;%f;%f\" translation=\"%f;%f;%f\" sqID=\"%u\"/>\n", size.x, size.y, size.z, worldRotation.x, worldRotation.y, worldRotation.z, worldRotation.w, worldScale.x, worldScale.y, worldScale.z, worldTranslation.x, worldTranslation.y, worldTranslation.z, sqID);
		
		strZone = desc.m_UserDefinedProperties;

		while (strZone.GetLength() > 0)
		{
			strName = "";
			strPortal = "";

			int i = 0;
			while ((strZone[i] != ']') && (i < strZone.GetLength()))
			{
				strName += strZone[i];
				i++;
			}

			if ((strZone[0] != '[') || (i == strZone.GetLength()))
			{
				return;
			}

			strZone.Delete(0, i + 1);
			strName.Delete(0);

			if (strName == "globalzonevisible")
			{
				continue;
			}

			if (strName.Find(",") != -1)
			{
				strPortal = strName;
				strPortal.Delete(0, strName.Find(",") + 1);
				strName.Delete(strName.Find(","), strName.GetLength() - strName.Find(","));
			}

			fprintf(ZoneDescFile, "\t\t<visiblezone name=\"%s\" portal=\"%s\" />\n", strName, strPortal);
		}
	}

	fprintf(ZoneDescFile, "\t</zone>\n");
}

void CExporter::CloseZoneDescFile()
{
	assert(ZoneDescFile);
	if( !ZoneDescFile)	return;
	fprintf(ZoneDescFile, "</zones>\n");
	fclose(ZoneDescFile);
}

void CExporter::OpenLocationsDescFile(CString strFilename)
{
	LocationsDescFile = fopen(strFilename.GetBuffer(0), "w");
	fprintf(LocationsDescFile, "<locations>\n");
}

void CExporter::WriteLocationDesc(MAP_NODE_DESCRIPTION& location)
{
	assert( location.m_Type == vespace::VE_LOCATIONBOUNDS);
	assert(LocationsDescFile);
	if( !LocationsDescFile)	return;

	CString zoneName;
	CString zoneScriptID;

	for( int i=0; i<location.m_Properties.size();i++)
	{
		TWO_STRING prop = location.m_Properties[i];
		if( prop.key == "name")	zoneName = prop.value;
		else	if( prop.key == "script" || prop.key == "script_id")	zoneScriptID = prop.value;
	}

	CMeshCandidate meshCandidate;
	CSkeletonCandidate skeletonCandidate;
	CalCoreMesh coreMesh;
	CreateCal3DObjects( &location, meshCandidate, skeletonCandidate, coreMesh);


	CalVector objectTranslation, objectScale;
	CalQuaternion objectRotation;
	GetNodeTransform( &location, objectTranslation, objectRotation, objectScale);

	float ax = location.xmin + objectTranslation.x;
	float axmax = location.xmax + objectTranslation.x;
	while( ax < axmax)
	{
		float ay = location.ymin + objectTranslation.y;
		float aymax = location.ymax + objectTranslation.y;
		while( ay < aymax)
		{
			DWORD sqID = GetZoneIDByCoordinates(ax, ay);
			ay += ZONE_SIZE;

			CString str;
			str.Format("<location name='%s' script_id='%s' sqID='%u'/>", 
				zoneName.GetBuffer(0), zoneScriptID.GetBuffer(0), sqID);
			fprintf(LocationsDescFile, str);
			fprintf(LocationsDescFile, "\n");
		}
		ax += ZONE_SIZE;
	}
	coreMesh.destroy();
}

CString CExporter::GetLocationXmlDesc(MAP_NODE_DESCRIPTION* locObject, std::vector<CAMERA_DESCRIPTION>* cams)
{
	std::vector<SITPLACE_DESCRIPTION> dummySits;

	CMeshCandidate locMeshCandidate;
	CSkeletonCandidate locSkeletonCandidate;
	CalCoreMesh locCoreMesh;
	CreateCal3DObjects( locObject, locMeshCandidate, locSkeletonCandidate, locCoreMesh);

	CalVector locTranslation, locScale;
	CalQuaternion locRotation;
	GetNodeTransform( locObject, locTranslation, locRotation, locScale);

	CalVector vmin( locObject->xmin, locObject->ymin, locObject->zmin);
	CalVector vmax( locObject->xmax, locObject->ymax, locObject->zmax);
	CalVector locRad = (vmax - vmin) / 2.0f;

	DWORD locSqID = GetZoneIDByCoordinates(locTranslation.x, locTranslation.y);
	
	// формирование параметра из полученного массива камер
	CString locationName = "Локация";
	// поиск параметра name_ru
	std::vector<TWO_STRING> locProp = locObject->m_Properties;
	for (int propIndex = 0; propIndex < locProp.size(); propIndex++)
	{
		if(locProp[propIndex].key == "name_ru")
		{
			locationName = locProp[propIndex].value;
			break;
		}
	}

	CString paramsStr = "{name:\"" + locationName + "\",cameras:[";
	CString camsStr = "";

	std::vector<CAMERA_DESCRIPTION>::iterator	it = cams->begin(),
												end = cams->end();
	for ( ;it!=end;it++)
	{
		std::vector<TWO_STRING> camParams = it->params;
		bool inLoc = false;

		for (int i = 0; i < camParams.size(); i++)
		{
			if (camParams[i].key == "location" && camParams[i].value == locObject->m_Name)
			{
				inLoc = true;
				break;
			}
		}

		if (inLoc)
		{
			if (camsStr != "") // Если не 1я камера
			{
				camsStr += ",";
			}

			CString camCustomParams = ""; // Строка кустомных параметров
			for (int i = 0; i < camParams.size(); i++) 
			{
				if (camParams[i].key != "location") // Если параметр не имя локации записываем
				{
					camCustomParams += ",\"" + camParams[i].key + "\":\"" + camParams[i].value + "\"";
				}
			}

			// преобразуем координаты камеры из глобальных в локальные относительно локации
			CalVector cameraPos, targetPos;
			cameraPos = it->cameraPos;
			targetPos = it->targetPos;

			// отложено
			//cameraPos = cameraPos - locTranslation;
			//targetPos = targetPos - locTranslation;

			CString curCamStr = "";
			CString camName = it->name;
			curCamStr.Format("{\"name\":\"%s\",\"eye_x\":%.2f,\"eye_y\":%.2f,\"eye_z\":%.2f,\"at_x\":%.2f,\"at_y\":%.2f,\"at_z\":%.2f,\"fov\":%.2f%s}",
								camName.GetBuffer(0),
								cameraPos.x,
								cameraPos.y,
								cameraPos.z,
								targetPos.x,
								targetPos.y,
								targetPos.z,
								it->fov,
								camCustomParams.GetBuffer(0));

			camsStr += curCamStr;
		}
	}

	paramsStr += camsStr + "]}";

	if (locObject->m_UserDefinedProperties != "")
	{
		//camsStr += "," + locObject->m_UserDefinedProperties;
		paramsStr += "," + locObject->m_UserDefinedProperties;
	}

	CString	locXmlString = GetXMLObjectInsertionString( 
		0, 
		locObject->m_Type,
		Cal3DComString( locObject->m_Name),
		"",
		"",
		0, 0,
		"",
		1,
		locTranslation,
		locScale,
		locRotation, locRad, locSqID,
		&paramsStr, dummySits, 
		0,
		"",
		"",
		m_internalZonePivots,
		locObject->m_Properties);

	locCoreMesh.destroy();

	return locXmlString;
}


void CExporter::CloseLocationsDescFile()
{
	assert(LocationsDescFile);
	if( !LocationsDescFile)	return;
	fprintf(LocationsDescFile, "</locations>\n");
	fclose(LocationsDescFile);
}

//////////////////////////////////////////////////////////////////////////
void	CExporter::WeldVertices( Mesh* mesh1, Matrix3 m1, 
								Mesh* mesh2, Matrix3 m2, 
								float dist)
{
	int vc1 = mesh1->getNumVerts();
	int vc2 = mesh2->getNumVerts();

	float distSq = dist*dist;


	BitArray	array;
	array.SetSize(vc2);	array.ClearAll();

	for( int i1=0; i1<vc1; i1++)
	{
		for( int i2=0; i2<vc2; i2++)
		{
			if( array[i2])	continue;

			Point3	p1 = mesh1->getVert( i1) * m1;
			Point3	p2 = mesh2->getVert( i2) * m2;

			float dx = p2.x - p1.x;
			float dy = p2.y - p1.y;
			float dz = p2.z - p1.z;

			if( dx*dx+dy*dy+dz*dz <= distSq)
			{
				p2 = p1;

				Matrix3 m1_i = m1;	m1_i.Invert();
				Matrix3 m2_i = m2;	m2_i.Invert();

				p1 = p1 * m1_i;
				p2 = p2 * m2_i;

				mesh1->setVert( i1, p1);
				mesh2->setVert( i2, p2);

				array.Set( i2);
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////
void	CExporter::WeldSelected( float dist)
{
	// check if a valid interface is set
	if(m_pInterface == 0)
	{
		SetLastError("Invalid handle.", __FILE__, __LINE__);
		return;
	}

	std::vector<MAP_NODE_DESCRIPTION>	vecSelectedObjects;
	typedef	std::map<CString/*base object name*/, int/*ref count*/>	MapReferencesNames;
	MapReferencesNames	mapReferencesNames;

	GetSelectedObjectsDesc( &vecSelectedObjects, mapReferencesNames);

	int i=0;
	for (i = 0; i < vecSelectedObjects.size(); i++)
	{
		MAP_NODE_DESCRIPTION*	objectDesc = &vecSelectedObjects[i];
		if( objectDesc->m_Type != vespace::VE_OBJECT3D &&
			objectDesc->m_Type != vespace::VE_REFERENCE)
		{
			vecSelectedObjects.erase( vecSelectedObjects.begin() + i);
			i--;
		}
	}

	// Сливаем вершины
	for (i = 0; i < vecSelectedObjects.size(); i++)
	{
		CMaxMesh*	maxMesh = (CMaxMesh*)m_pInterface->GetMesh(vecSelectedObjects[i].m_pBaseNode);
		if( maxMesh)
		{
			Mesh*		mesh = maxMesh->GetIMesh();
			if( mesh)
			{
				Matrix3 m_tm1 = maxMesh->GetNode()->GetObjectTM(SecToTicks(m_pInterface->GetCurrentTime()));
				for (int j = i+1; j < vecSelectedObjects.size(); j++)
				{
					CMaxMesh*	maxMesh2 = (CMaxMesh*)m_pInterface->GetMesh(vecSelectedObjects[j].m_pBaseNode);
					if( maxMesh2)
					{
						Mesh*		mesh2 = maxMesh2->GetIMesh();
						if( mesh2)
						{

							Matrix3 m_tm2 = maxMesh2->GetNode()->GetObjectTM(SecToTicks(m_pInterface->GetCurrentTime()));
							WeldVertices(mesh, 
										m_tm1,
										mesh2, 
										m_tm2,
										dist);
							//delete mesh2;
						}
						delete maxMesh2;
					}

				}
				//delete mesh;
			}
			delete maxMesh;
		}
	}
	m_pInterface->RedrawViews();
}


void CorrectPathSlashes(CString& objectPath)
{
	objectPath.Replace("/", "\\");
	while ( objectPath.Find("\\\\")!=-1)
		objectPath.Replace("\\\\", "\\");
}


/*
	Все текстуры при экспорте складываются в одну папку.
	 1. Эта папка лежит на том же уровне что и каталоги объектов, если вложенность экспорта каталога = 1
	 Например:	[папка экспорта]\SampleObject\mesh.bmf
				[папка экспорта]\CExporter::calexportTexturesPath

	 2. Эта папка лежит на N уровней ниже папки экспорта, если вложенность экспорта каталога = N
	 Например:	[папка экспорта]\sq4\SampleGroup\SampleObject\mesh.bmf
				[папка экспорта]\sq4\CExporter::calexportTexturesPath
	*/
CString CExporter::GetTexturesPath(const CString& objectPath, const CString& texturesPath)
{
	if( !CExporter::calexportAbsoluteTexturesPaths)
		return texturesPath;
	
	CString temp = objectPath;
	CorrectPathSlashes(temp);
	if( temp.GetLength() > 0
		&&  temp[0] == '\\')
			temp.Delete(0);
	if( temp.GetLength() > 0 
		&& temp[temp.GetLength()-1] == '\\')
			temp.Delete(temp.GetLength()-1);

	int slashesCount = 0;
	int lastIndex = -1;
	while( (lastIndex = temp.Find('\\', lastIndex+1))!=-1)
		slashesCount++;

	// 1. Эта папка лежит на том же уровне что и каталоги объектов, если вложенность экспорта каталога = 1
	if( slashesCount == 0)
		slashesCount = 1;

	CString ret;
	// 2. Эта папка лежит на N уровней ниже папки экспорта, если вложенность экспорта каталога = N
	while( slashesCount>0)
	{
		ret += "..\\";
		slashesCount--;
	}
	ret += texturesPath;
	CorrectPathSlashes(ret);
	return ret;
}

//////////////////////////////////////////////////////////////////////////
bool CExporter::ExportTownMesh(std::string& strFilename)
{
	//инициализация Tootle
	if (TootleInit() == TOOTLE_OK)
	{
		TootleIsInited = true;
	}

	// поиск в реестре пути к 3ds max 2009
	HKEY hKey = NULL;
	TCHAR regMaxPath[MAX_PATH] = "SOFTWARE\\Autodesk\\3dsmax\\11.0\\MAX-1:409";

	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, regMaxPath, 0, KEY_READ, &hKey) == ERROR_SUCCESS)
	{
		TCHAR maxPathBuf[MAX_PATH];
		DWORD maxPathBufLen = MAX_PATH;

		if (RegQueryValueEx(hKey, "Installdir", 0, NULL, (LPBYTE)maxPathBuf, &maxPathBufLen) == ERROR_SUCCESS)
		{
			MaxInstallPath = maxPathBuf;
			CExporter::MaxInstallPath = MaxInstallPath;
		}
	}

	if (hKey != NULL)
	{
		RegCloseKey(hKey);
	}

	// чтение конфига cal3d.cfg
	std::string calConfigPath = MaxInstallPath + CAL_CONFIG_FILE_NAME;
	FILE* calConfigFile = fopen(calConfigPath.c_str(), "rb");

	if (calConfigFile != NULL)
	{
		const int calParamBufLen = 300;		
		char calParamBuf[calParamBufLen];

		while (fgets(calParamBuf, calParamBufLen, calConfigFile) != NULL)
		{
			std::string calParam = calParamBuf;

			if (calParam.find(CAL_CONFIG_PARAM_SERVER_PATH) != -1)
			{
				ServerPath = calParam.substr(calParam.find("=") + 1);
			}
		}

		fclose(calConfigFile);
	}

	// Берем путь из настройки, а не из конфига
	if(CExporter::calexprotAutomatedExport || ServerPath == "")
	{
		ServerPath = CAutoExportSettings::GetGeometryPath();
		if(CExporter::libExport)
		{
			//ServerPath = ServerPath + "\\" + "library";
		}
	}

	ZoneDescFile = NULL;
	LocationsDescFile = NULL;
	// check if a valid interface is set
	if(m_pInterface == 0)
	{
		SetLastError("Invalid handle.", __FILE__, __LINE__);
		return false;
	}

	TCHAR	szCurrentDir[MAX_PATH*2+1];	ZeroMemory( szCurrentDir, sizeof(szCurrentDir)/sizeof(szCurrentDir[0]));
	GetCurrentDirectory( MAX_PATH*2, szCurrentDir);
	
	// build a mesh candidate
	CMeshCandidate meshCandidate;
	// build a skeleton candidate
	CSkeletonCandidate skeletonCandidate;
	// Flag of multiselection
	BOOL	bMultiSelection = theExporter.GetInterface()->GetSelectedNodeCount() > 1?TRUE:FALSE;

	if( !calexprotAutomatedExport)
	{
		CExporter::calexpoptUSELOCALCOORDS = FALSE;
		CExporter::calexpoptEXPORTSHADOWDATA = FALSE;
		CExporter::calexpoptNORMALIZEVERTS = FALSE;
		CExporter::calexpoptAutoReplaceSameMaterials = 0;
		CExporter::calexpoptAutoReplaceSameTextures = 0;

		// show export wizard sheet
		CMeshExportSheet sheet("Cal3D Mesh Export", m_pInterface->GetMainWnd(), bMultiSelection?1:0);
		sheet.SetMultSelectMode(bMultiSelection);
		sheet.SetColorOverrides(&overAmbient, &overDiffuse, &dwAmbient, &dwDiffuse);
		sheet.SetSkeletonCandidate(&skeletonCandidate);
		sheet.SetMeshCandidate(&meshCandidate);
		sheet.SetWizardMode();
		if(sheet.DoModal() != ID_WIZFINISH) return true;
	}
	else
	{
		overAmbient = TRUE;
		overDiffuse = TRUE;
		dwAmbient = 0xFFFFFFFF;
		dwDiffuse = 0xFFFFFFFF;
	}

	// Setup	directories
	SECFileSystem	fs;
	// Имя файла
	CString	pathUserSelectedFileName = fs.GetFileName(strFilename.c_str());
	// Путь файла, выбранный пользователем
	CString	pathUserSelectedDirectory = fs.GetPath(strFilename.c_str());
	CreateAbsolutePath( pathUserSelectedDirectory);
	// Пути сохранения текстур относительно геометрии
	CString	pathTextures = "";
	// Пути сохранения материалов относительно геометрии
	CString	pathMaterials = "";
	// Список имен объектов
	std::vector<TWO_STRING>	sceneConfigLines;
	// Список имен объектов
	std::vector<TWO_STRING>	sceneSQLConfigLines;
	std::vector<TWO_STRING>	sceneXMLConfigLines;
	// Границы локальных зон
	typedef std::vector<MAP_NODE_DESCRIPTION> VecDescs;
	std::map<CString, VecDescs> sceneZones;
	// Границы локаций
	VecDescs					sceneLocations;

	if( CExporter::calexpoptUseTexturesPath)	pathTextures = CExporter::calexportTexturesPath;
	if( CExporter::calexpoptUseMaterialsPath)	pathMaterials = CExporter::calexportMaterialsPath;
	

	// Materials map
	MaterialsMap	mapMaterialsNames;

	// Selection parameters
	std::vector<MAP_NODE_DESCRIPTION>	vecSelectedObjects;
	std::vector<CString>				vecSceneConfigLines;
	typedef	std::map<CString/*base object name*/, int/*ref count*/>	MapReferencesNames;
	MapReferencesNames	mapReferencesNames;

	if( bMultiSelection)
	{
		StartLog( pathUserSelectedDirectory + "/" + pathUserSelectedFileName+".log");
		CExporter::calexpoptUSELOCALCOORDS = TRUE;
	}

	GetSelectedObjectsDesc( &vecSelectedObjects, mapReferencesNames);

	int __dbCurrentMaxDB = 0;
	int i = 0;
	for (i = 0; i < vecSelectedObjects.size(); i++)
	{
		MAP_NODE_DESCRIPTION*	objectDesc = &vecSelectedObjects[i];
		__dbCurrentMaxDB = max( __dbCurrentMaxDB, objectDesc->m_DatabaseID);
	}

	// Выдираем зоны в отдельный массив
	for (i = 0; i < vecSelectedObjects.size(); i++)
	{
		if (vecSelectedObjects[i].m_IsZone)
		{
			std::map<CString, VecDescs>::iterator iter = sceneZones.find(vecSelectedObjects[i].m_Name);
			if (iter == sceneZones.end())
			{
				VecDescs vecDescs;
				vecDescs.push_back(vecSelectedObjects[i]);
				sceneZones.insert(std::map<CString, VecDescs>::value_type(vecSelectedObjects[i].m_Name, vecDescs));
			}
			else
			{
				(*iter).second.push_back(vecSelectedObjects[i]);
			}
			vecSelectedObjects.erase(vecSelectedObjects.begin() + i);
			i--;
		}
	}
	// Пишем описание зон
	if( sceneZones.size() > 0)
	{
		OpenZoneDescFile(pathUserSelectedDirectory + "\\zones.xml");
		std::map<CString, VecDescs>::iterator iter = sceneZones.begin();
		std::map<CString, VecDescs>::iterator iterEnd = sceneZones.end();
		for ( ; iter != iterEnd; iter++)
		{
			WriteZoneDesc((*iter).second);
		}
		CloseZoneDescFile();
	}

	// Выдираем локации в отдельный массив.
	for (i = 0; i < vecSelectedObjects.size(); i++)
	{
		if (vecSelectedObjects[i].m_Type == vespace::VE_LOCATIONBOUNDS)
		{
			//??
			sceneLocations.push_back( vecSelectedObjects[i]);
			vecSelectedObjects.erase(vecSelectedObjects.begin() + i);
			i--;
		}
	}

	// Пишем описание локаций
/*	if( sceneLocations.size() > 0)
	{
		OpenLocationsDescFile(pathUserSelectedDirectory + "\\locations.xml");
		for( i=0; i<sceneLocations.size(); i++)
			WriteLocationDesc( sceneLocations[i]);
		CloseLocationsDescFile();
	}
*/

	std::vector<CAMERA_DESCRIPTION> cams;
	// заносим камеры в отдельный массив структур
	for (i = 0; i < vecSelectedObjects.size(); i++)
	{
		if (vecSelectedObjects[i].m_Type == vespace::VE_CAMERA)
		{
			CAMERA_DESCRIPTION camera;
			MAP_NODE_DESCRIPTION* camDesc = &vecSelectedObjects[i];

			CalVector		camTranslation, camScale;
			CalQuaternion	camRotation;
			GetNodeTransform(camDesc, camTranslation, camRotation, camScale);

			camera.cameraPos = camTranslation;

			// получим фов
			GenCamera* maxCam = (GenCamera*)(camDesc->m_pBaseNode->GetINode()->GetObjectRef());
			camera.fov = RadToDeg(maxCam->GetFOV(0));

			camera.params = camDesc->m_Properties; // копируем свойства
			camera.id = camDesc->m_Level; // порядковый номер камеры
			camera.name = camDesc->m_Name; // имя камеры
			if (camDesc->m_Res != "")
			{
				camera.params.push_back(TWO_STRING(CString("target"), camDesc->m_Res)); // если установлен постфикс target, он хранится в m_res
			}	

			// ищем таргет камеры. Если вдруг не выделили, то возьмем координаты объекта указанного в параметре target
			// поправка: target используется еще и как параметр
			CString targetName = camDesc->m_Name + ".Target";
			
			// имя локации к которой принадлежит камера
			CString locName = "";
			for (int paramIndex = 0; paramIndex < camera.params.size(); paramIndex++)
			{
				if (camera.params[paramIndex].key == "location")
				{
					locName = camera.params[paramIndex].value;
					break;
				}
			}

			MAP_NODE_DESCRIPTION* camTargetDesc = GetNodeDesriptionByName(targetName, &vecSelectedObjects, locName);
			if (camTargetDesc == NULL)
			{
				targetName = camDesc->m_Res;
				camTargetDesc = GetNodeDesriptionByName(targetName, &vecSelectedObjects);
			}

			if (camTargetDesc != NULL)
			{
				GetNodeTransform(camTargetDesc, camTranslation, camRotation, camScale);
				camera.targetPos = camTranslation;
			}
			else
			{
				AfxMessageBox("Не найден объект target для камеры.\nУбедитесь что он выделен.", MB_OK | MB_ICONEXCLAMATION);
				return false;
			}

			cams.push_back(camera);
		}
	}
	// сортировка камер
	if(cams.size() > 0)
	{
		for (int camIndex1 = 0; camIndex1 < cams.size()-1; camIndex1 ++)
		{
			for (int camIndex2 = camIndex1+1; camIndex2 < cams.size(); camIndex2 ++)
			{
				CAMERA_DESCRIPTION cam1 = cams[camIndex1];
				CAMERA_DESCRIPTION cam2 = cams[camIndex2];
				CString cam1Location = "";
				CString cam2Location = "";

				for (int paramIndex = 0; paramIndex < cam1.params.size(); paramIndex++)
				{
					if (cam1.params[paramIndex].key == "location")
					{
						cam1Location = cam1.params[paramIndex].value;
						break;
					}
				}

				for (int paramIndex = 0; paramIndex < cam2.params.size(); paramIndex++)
				{
					if (cam2.params[paramIndex].key == "location")
					{
						cam2Location = cam2.params[paramIndex].value;
						break;
					}
				}

				if (cam1Location == cam2Location)
				{
					if (cam1.id > cam2.id)
					{
						cams[camIndex1] = cam2;
						cams[camIndex2] = cam1;
					}
				}
			}
		}
	}

	// добавляем описание локации
	if(sceneLocations.size() > 0)
	{
		for( i=0; i<sceneLocations.size(); i++)
		{
			CString locXmlString = GetLocationXmlDesc(&sceneLocations[i], &cams);

			sceneXMLConfigLines.push_back( TWO_STRING(locXmlString, ""));
			sceneConfigLines.push_back( TWO_STRING("location", sceneLocations[i].m_Name));
		}
	}

	//////////////////////////////////////////////////////////////////////////
	for (i = 0; i < vecSelectedObjects.size(); i++)
	{
		if (vecSelectedObjects[i].m_IsZone)
		{
			ATLASSERT( FALSE);
			continue;
		}
		if( vecSelectedObjects[i].m_Type == vespace::VE_UNDEFINED)
			continue;

		// пропустим примитивы коллизий и места для посадки
		// и камеры
		if( vecSelectedObjects[i].m_Type == vespace::VE_COLLIDER_OBJECT ||
			vecSelectedObjects[i].m_Type == vespace::VE_SITTING_PLACE ||
			vecSelectedObjects[i].m_Type == vespace::VE_CAMERA ||
			vecSelectedObjects[i].m_Type == vespace::VE_CAMERA_TARGET)
			continue;
	
		// 1. Определяем директорию для экспорта объекта
		// 2. Создаем геометрию и скелет для объекта
		// 3. Определяем конфиг для этого объекта
		// 4. Экспортим текстуры
		// 5. Записываем конфиг и файлы
		// 6. Пишем скриптовый файл

		/************************************************************************/
		/* 
		/************************************************************************/
		// 1. Определяем директорию для экспорта объекта
		MAP_NODE_DESCRIPTION*	objectDesc = &vecSelectedObjects[i];
		WriteLog( "Exporting object: "+objectDesc->m_Name);
		CString		pathObjectDirectory = GetObjectDirName( objectDesc, mapReferencesNames);
		CString		pathAbsoluteObjectDirectory = "";
		objectDesc->m_PathObjectDirectory = pathObjectDirectory;
		pathAbsoluteObjectDirectory = pathUserSelectedDirectory + "\\" + pathObjectDirectory;
		CorrectPathSlashes(pathAbsoluteObjectDirectory);

		// 2.0 Создаем геометрию и скелет для объекта
		WriteLog( "Exporting object 2: "+objectDesc->m_Name);
		CalCoreMesh			coreMesh;
		CMeshCandidate		meshCandidate;
		CSkeletonCandidate	skeletonCandidate;

		//SITPLACE_DESCRIPTION sitplaceObject;
		std::vector<SITPLACE_DESCRIPTION> sitplaceObjects;

		if( IsNodeInReferenceChain(objectDesc))
			continue;
		CString	configFileName = OBJECT_CONFIG_FILE_NAME;
		if( objectDesc->m_IsGroupHead && objectDesc->m_Type!=vespace::VE_REFERENCE)
		{
			if( objectDesc->m_ExplodeGroup)
				continue;
			configFileName = GROUP_CONFIG_FILE_NAME;
		}
		else	if( objectDesc->m_Type == vespace::VE_OBJECT3D ||
					objectDesc->m_Type == vespace::VE_COLLIDER_OOBB ||
					objectDesc->m_Type == vespace::VE_COLLIDER_MESH ||
					objectDesc->m_Type == vespace::VE_COLLIDER_AABB ||
					objectDesc->m_Type == vespace::VE_REFERENCE ||
					objectDesc->m_Type == vespace::VE_SKYBOX)
		{
			WriteLog( "Exporting object 2.1: "+objectDesc->m_Name);
			// 2.1. Поиск примитивов коллизий для данного объекта и их обработка
			std::vector<PRIMITIVE_DESCRIPTION> colliderObjects;
			int p;
			for (p = 0; p < vecSelectedObjects.size(); p++)
			{
				if ((vecSelectedObjects[p].m_Type == vespace::VE_COLLIDER_OBJECT) && (vecSelectedObjects[p].m_ReferenceName == objectDesc->m_Name))
				{
					colliderObjects.push_back(GetColliderObject(&vecSelectedObjects[p]));
				}
			}
			WriteLog( "Exporting object 2.2.1: "+objectDesc->m_Name);
			// 2.2.1 Поиск места сиденья у объекта (может быть несколько)
			if (objectDesc->m_Type == vespace::VE_REFERENCE)
			{
				objectDesc->m_UserDefinedProperties = objectDesc->m_pReferenceNodeDescription->m_UserDefinedProperties;				
				// 2.2.2установка родительского класса референсу
				objectDesc->m_Class = objectDesc->m_pReferenceNodeDescription->m_Class;
				// 2.2.3 установка level
				objectDesc->m_Level = objectDesc->m_pReferenceNodeDescription->m_Level;
			}
			else
			{
				for (p = 0; p < vecSelectedObjects.size(); p++)
				{
					if ((vecSelectedObjects[p].m_Type == vespace::VE_SITTING_PLACE) && (vecSelectedObjects[p].m_ReferenceName == objectDesc->m_Name))
					{
						sitplaceObjects.push_back(GetSitplaceObject(&vecSelectedObjects[p]));
						//sitplaceObject = GetSitplaceObject(&vecSelectedObjects[p]);
					}
				}
			}

			WriteLog( "Exporting object CreateCal3DObjects: "+objectDesc->m_Name);

			//////////////////////////////////////////////////////////////////////////
			CreateCal3DObjects( objectDesc, meshCandidate, skeletonCandidate, coreMesh);

			WriteLog( "Exporting object 3: "+objectDesc->m_Name);

			// 3. Определяем конфиг для этого объекта
			configFileName = OBJECT_CONFIG_FILE_NAME;
			CString	meshFileName = OBJECT_CONFIG_FILE_NAME;
			CString	skelFileName = fs.GetFileName( CString(skeletonCandidate.m_strFilename.c_str()));

			CString objectTexturesPath = GetTexturesPath(pathObjectDirectory, pathTextures);
			CString objectMaterialsPath = GetTexturesPath(pathObjectDirectory, pathMaterials);

			if( !pathTextures.IsEmpty())
			{
				objectDesc->m_Properties.push_back( TWO_STRING("textures", objectTexturesPath));
			}
			if( !pathMaterials.IsEmpty())
			{
				objectDesc->m_Properties.push_back( TWO_STRING("materials", objectMaterialsPath));
			}
			if( CExporter::calexportSeparateCommonObjects)
			{
				objectDesc->m_Properties.push_back( TWO_STRING("common_objects", "true"));
			}
			if( CExporter::calexportSeparateCommonTextures)
			{
				objectDesc->m_Properties.push_back( TWO_STRING("common_textures", "true"));
			}

			FillObjectConfig(	objectDesc, 
								meshFileName,
								skelFileName,
								&objectCfg);

			std::vector<int> materialsIds;

			WriteLog( "Exporting object 4: "+objectDesc->m_Name);

			// 4. Экспортим текстуры
			//FreeImage_Initialise();
			if( objectDesc->m_Type != vespace::VE_REFERENCE)
			{
				GetMaterialsIds( meshCandidate, &materialsIds);
				// создаем файл списка текстур объекта
				CreateAbsolutePath( pathAbsoluteObjectDirectory);
				FILE* materialFile = fopen(pathAbsoluteObjectDirectory + "\\" + MATERIAL_TEXTURES_FILE_NAME, "wb");
				std::string head = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\r\n<materials>\r\n";
				fwrite(head.c_str(), 1, head.length(), materialFile);

 				for( int mids=0; mids < materialsIds.size(); mids++)
				{
					CString	materialFileName;
					CString	materialFilePath;
					StdMat*	material = meshCandidate.GetMtrl(materialsIds[mids]);
					ASSERT(material);
					if( material)
					{
						materialFileName.Format("%s.crf", material->GetName());
						if( calexpoptGetnMtrlsNames)
						{
							MaterialsMap::iterator	it = mapMaterialsNames.find(materialFileName);
							if( it == mapMaterialsNames.end())
							{
								CString	genName;	genName.Format( "mtrl%d.crf", mapMaterialsNames.size()+1);
								mapMaterialsNames.insert( MaterialsMap::value_type( materialFileName, genName));
								materialFileName = genName;
							}
							else
							{
								materialFileName = it->second;
							}
						}
						if( !objectDesc->m_IsReference)
						{
							_material	mtrl;
							mtrl.src = Cal3DComString(materialFileName.GetBuffer(0));
							objectCfg.materials.push_back(mtrl);
						}
						CString	pathMaterial = fs.AppendPaths(pathAbsoluteObjectDirectory,  objectMaterialsPath);
						CreateAbsolutePath( pathMaterial);
						materialFileName = pathMaterial + materialFileName;
						//////////////////////////////////////////////////////////////////////////
						// save materials
						CalCoreMaterial* coreMaterial = new CalCoreMaterial(); // нужен для сохранения инфы в файле модели
						ExportMaterial( materialFileName.GetBuffer(0), coreMaterial, material, FALSE/*dont save maps*/);
						//////////////////////////////////////////////////////////////////////////
						
						CMaxMaterial	maxMaterial;
						maxMaterial.Create(material, coreMaterial);
						CString	pathTexturesLocal = fs.AppendPaths( pathUserSelectedDirectory+"\\"+pathObjectDirectory, objectTexturesPath);
						CString	pathTexturesCommon = fs.AppendPaths( pathUserSelectedDirectory+"\\common\\textures\\", objectTexturesPath);

						CreateAbsolutePath( pathTexturesLocal);
						
						if( CExporter::calexportSeparateCommonTextures)
						{
							CreateAbsolutePath( pathTexturesCommon);							
						}

						if (ServerPath != "" && calexpoptAutoexportToServer)
						{
							CString pathTexturesLocalServer = fs.AppendPaths(CString(ServerPath.c_str()) + "\\" + pathObjectDirectory, objectTexturesPath);
							CorrectPathSlashes(pathTexturesLocalServer);

							CString pathTexturesCommonServer = fs.AppendPaths(CString(ServerPath.c_str()) + "\\common\\textures\\", objectTexturesPath);
							CorrectPathSlashes(pathTexturesCommonServer);

							CreateAbsolutePath(pathTexturesLocalServer);
							
							if (CExporter::calexportSeparateCommonTextures)
							{
								CreateAbsolutePath( pathTexturesCommonServer);
							}
							
							maxMaterial.SaveMaps( pathTexturesLocal, pathTexturesCommon, pathTexturesLocalServer, pathTexturesCommonServer);							
						}
						else
						{
							maxMaterial.SaveMaps( pathTexturesLocal, pathTexturesCommon);							
						}

						if (coreMaterial != NULL)
						{
							// запишем названия диффузных текстур в отдельный xml файл
							for (int materialIndex = 0; materialIndex < coreMaterial->getVectorMap().size(); materialIndex++)
							{
								if (coreMaterial->getVectorMap()[materialIndex].type == 1 || coreMaterial->getVectorMap()[materialIndex].type == 0)
								{
									std::string textureName = "<texture>" + coreMaterial->getVectorMap()[materialIndex].strFilename + "</texture>\r\n";

									fwrite(textureName.c_str(), 1, textureName.length(), materialFile);
									coreMaterial->getVectorMap()[materialIndex].strFilename;
								}
							}

							CalSaver saver;
							if(!saver.saveCoreMaterial(materialFileName.GetBuffer(0), coreMaterial))
							{
								SetLastError(CalError::getLastErrorText(), __FILE__, __LINE__);								
							}
							
							coreMaterial->destroy();
							delete coreMaterial;
						}
					}
				}
				std::string foot = "</materials>\r\n";
				fwrite(foot.c_str(), 1, foot.length(), materialFile);

				fclose(materialFile);
			}

			WriteLog( "Exporting object 5: "+objectDesc->m_Name);

			//FreeImage_DeInitialise();
			// 5. Записываем конфиг и файлы
			if( objectDesc->m_Type != vespace::VE_REFERENCE ||
				objectDesc->m_pParentGroupNodeDescription != NULL)
			{
				CreateAbsolutePath( pathAbsoluteObjectDirectory);
				SetCurrentDirectory( pathAbsoluteObjectDirectory);

				//////////////////////////////////////////////////////////////////////////
				if( !objectDesc->m_NoGeometry)
				{
					int modLevel = 0;
					if(calexprotAutomatedExport)
					{
						modLevel = GetModifyLevel(objectDesc->m_Name);
					}
					if(modLevel != 2 && modLevel != 3)
					{
						// пишем только для объектов
						CreateBMFFile(meshFileName, objectDesc,
							meshCandidate,
							mapMaterialsNames,
							materialsIds,
							pathAbsoluteObjectDirectory,
							GetTexturesPath(pathObjectDirectory, pathMaterials),
							pathObjectDirectory,
							colliderObjects, MaxInstallPath);
					}
					else
					{
						CString mess = "Объект '"+objectDesc->m_Name+"' защищен он изменения геометрии.\nОтключите ее перед экспортом, если хотите заменить.";
						AfxMessageBox(mess.GetBuffer(0), MB_OK | MB_ICONSTOP);
					}
				}
			}
			coreMesh.destroy();
		}

		WriteLog( "Exporting object 6: "+objectDesc->m_Name);

		//////////////////////////////////////////////////////////////////////////
		// параметры экрана
		if(objectDesc->m_Class == "panel_object")
		{
			CString panelParams = "";
			// пропорции экрана
			CMaxMesh*	maxMesh = (CMaxMesh*)m_pInterface->GetMesh(objectDesc->m_pBaseNode);
			maxMesh->GetIMesh()->buildBoundingBox();
			Box3 bound = maxMesh->GetIMesh()->getBoundingBox();
			int dimensionX = bound.pmax.x - bound.pmin.x;
			int dimensionY = bound.pmax.y - bound.pmin.y;

			// если пропорции явно заданы
			CString userDefinedProps = objectDesc->m_UserDefinedProperties;

			CString propTemplate = "\"proportions\":\"";
			int proportionsPos = userDefinedProps.Find(propTemplate);
			if(proportionsPos != -1)
			{
				CString dimensions = userDefinedProps.Mid(proportionsPos + propTemplate.GetLength(),
					userDefinedProps.Right(userDefinedProps.GetLength() - (proportionsPos + propTemplate.GetLength())).FindOneOf("\"") + proportionsPos - 1);

				CString dimX = dimensions.Left(dimensions.Find(';'));
				CString dimY = dimensions.Right(dimensions.GetLength() - dimensions.Find(';') - 1);

				dimensionX = atoi(dimX.GetBuffer());
				dimensionY = atoi(dimY.GetBuffer());
			}

			// поиск камеры для экрана
			CAMERA_DESCRIPTION screenCam;
			for (int camIndex = 0; camIndex < cams.size(); camIndex++)
			{
				CAMERA_DESCRIPTION cam = cams[camIndex];
				CString camViewObject = "";
				for (int paramIndex = 0; paramIndex < cam.params.size(); paramIndex++)
				{
					if(cam.params[paramIndex].key == "view_object")
					{
						camViewObject = cam.params[paramIndex].value;
						break;
					}
				}

				if (camViewObject == objectDesc->m_Name)
				{
					screenCam = cam;
					break;
				}
			}		

			panelParams.Format("{\"proportions\":\"%d;%d\",\"cam_coords\":{\"dx\":%.8f,\"dy\":%.8f,\"dz\":%.8f,\"ex\":%.8f,\"ey\":%.8f,\"ez\":%.8f}}",
				dimensionX,
				dimensionY,
				screenCam.cameraPos.x,
				screenCam.cameraPos.y,
				screenCam.cameraPos.z,
				screenCam.targetPos.x,
				screenCam.targetPos.y,
				screenCam.targetPos.z);
			objectDesc->m_UserDefinedProperties = panelParams;
		}
		//////////////////////////////////////////////////////////////////////////

		// не пишем конфигурацию для низких лодов
		if (objectDesc->m_Lod != 0)
		{
			continue;
		}

		std::vector<LOD_PROPS> objectLodsProps;
		// ищем лоды у объекта
		std::vector<MAP_NODE_DESCRIPTION> objectLods;
		int p;
		//if()
		for (p = 0; p < vecSelectedObjects.size(); p++)
		{
			if ((objectDesc->m_Name == vecSelectedObjects[p].m_Name) && (objectDesc->m_Lod != vecSelectedObjects[p].m_Lod))
			{
				objectLods.push_back(vecSelectedObjects[p]);
			}
		}
		
		/*
		if((objectDesc->m_pParentGroupNodeDescription!=NULL && 
				!objectDesc->m_pParentGroupNodeDescription->m_ExplodeGroup))
		{
			
			continue;
		}
		*/

		CalVector objectTranslation, objectScale;
		CalQuaternion objectRotation;
		GetNodeTransform( objectDesc, objectTranslation, objectRotation, objectScale);

		CalVector vmin( objectDesc->xmin, objectDesc->ymin, objectDesc->zmin);
		CalVector vmax( objectDesc->xmax, objectDesc->ymax, objectDesc->zmax);

		if( objectDesc->m_IsGroupHead &&
			!objectDesc->m_ExplodeGroup)
		{
			GetGroupBounds(objectDesc, &vecSelectedObjects, vmin, vmax);
		}
		if( objectDesc->m_Type == vespace::VE_REFERENCE &&
			objectDesc->m_pReferenceNodeDescription)
		{
			vmin.set( objectDesc->m_pReferenceNodeDescription->xmin, 
						objectDesc->m_pReferenceNodeDescription->ymin, 
						objectDesc->m_pReferenceNodeDescription->zmin);
			vmax.set( objectDesc->m_pReferenceNodeDescription->xmax, 
						objectDesc->m_pReferenceNodeDescription->ymax, 
						objectDesc->m_pReferenceNodeDescription->zmax);
		}

		CalVector avg = (vmin + vmax) / 2.0f;
		CalVector rad = (vmax - vmin) / 2.0f;
		//objectTranslation += avg;
		/*int	ny = objectTranslation.y/20000.0f + 16000;
		int	nx = objectTranslation.x/20000.0f + 16000;
		DWORD sqID = (ny<<15) + nx;*/

		//	DWORD sqID = GetZoneIDByCoordinates(objectTranslation.x, objectTranslation.y);
		DWORD sqID = GetZoneIDByCoordinates(objectTranslation.x /*+ CAutoExportSettings::GetWorldX()*/
											, objectTranslation.y /*+ CAutoExportSettings::GetWorldY()*/);

		if( sceneSQLConfigLines.size() == 0)
		{
			sceneSQLConfigLines.push_back(TWO_STRING("TRUNCATE TABLE `objects_map`;", ""));
		}

		// формируем строку пути ресурсов для объекта и лодов + дистанция + наличие тени
		// дистанция и тень описываются в максе как [distance=] и [shadow=] в имени объекта
		// вместо пропущенных значений записываются значения по-умолчанию
		CString lodsResPath = "";

		CString resPath = pathObjectDirectory + "\\" + configFileName;
		if( objectDesc->m_Type != vespace::VE_GROUP3D &&
			objectDesc->m_Type != vespace::VE_OBJECT3D &&
			objectDesc->m_Type != vespace::VE_COLLIDER_OOBB &&
			objectDesc->m_Type != vespace::VE_COLLIDER_MESH &&
			objectDesc->m_Type != vespace::VE_COLLIDER_AABB &&
			objectDesc->m_Type != vespace::VE_SKYBOX &&
			objectDesc->m_Type != vespace::VE_REFERENCE)
		{
			resPath.Empty();
		}
		
		if( objectDesc->m_NoGeometry)
		{
			resPath.Empty();
		}
		if( !objectDesc->m_Res.IsEmpty())
			resPath = objectDesc->m_Res;

		for (int lodPropIndex = 0; lodPropIndex < objectDesc->m_LodProps.size(); lodPropIndex++)
		{			
			LOD_PROPS lodProps = objectDesc->m_LodProps[lodPropIndex];

			lodProps.resource = resPath;

			if (lodProps.distance == -1)
			{
				if (objectLods.size() == 0 && objectDesc->m_LodProps.size() == 1)
				{
					lodProps.distance = VIEW_DISTANCE_MAX;				
				}
				else if (objectLods.size() == 0 && lodPropIndex == objectDesc->m_LodProps.size()-1)
				{
					lodProps.distance = VIEW_DISTANCE_MAX;	
				}
				else
				{
					if (lodProps.lod == 0)
					{
						lodProps.distance = VIEW_DISTANCE_LOD0;
					}
					else if (lodProps.lod == 1)
					{
						lodProps.distance = VIEW_DISTANCE_LOD1;
					}
					else if (lodProps.lod == 2)
					{
						lodProps.distance = VIEW_DISTANCE_LOD2;
					}
					else
					{
						lodProps.distance = lodProps.lod*10000;
					}
				}				
			}

			if (lodProps.shadow == -1)
			{
				if (objectLods.size() == 0 && objectDesc->m_LodProps.size() == 1)
				{
					lodProps.shadow = VIEW_SHADOW_MAX;				
				}
				else
				{
					if (lodProps.lod == 0)
					{
						lodProps.shadow = VIEW_SHADOW_LOD0;
					}
					else if (lodProps.lod == 1)
					{
						lodProps.shadow = VIEW_SHADOW_LOD1;
					}
					else if (lodProps.lod == 2)
					{
						lodProps.shadow = VIEW_SHADOW_LOD2;
					}
					else
					{
						lodProps.shadow = 0;
					}
				}	
			}

			objectLodsProps.push_back(lodProps);
		}

		for (p = 0; p < objectLods.size(); p++)
		{
			MAP_NODE_DESCRIPTION* objectLod = &objectLods[p];

			CString pathObjectLodDirectory = GetObjectDirName( objectLod, mapReferencesNames);
			CString lodResPath = pathObjectLodDirectory + "\\" + OBJECT_CONFIG_FILE_NAME;

			if( objectLod->m_Type != vespace::VE_GROUP3D &&
				objectLod->m_Type != vespace::VE_OBJECT3D &&
				objectLod->m_Type != vespace::VE_COLLIDER_OOBB &&
				objectLod->m_Type != vespace::VE_COLLIDER_MESH &&
				objectLod->m_Type != vespace::VE_COLLIDER_AABB &&
				objectLod->m_Type != vespace::VE_SKYBOX &&
				objectLod->m_Type != vespace::VE_REFERENCE)
			{
				lodResPath.Empty();
			}
			
			if( objectLod->m_NoGeometry)
			{
				lodResPath.Empty();
			}

			if( !objectLod->m_Res.IsEmpty())
				lodResPath = objectLod->m_Res;

			for (int lodPropIndex = 0; lodPropIndex < objectLod->m_LodProps.size(); lodPropIndex++)
			{			
				LOD_PROPS lodProps = objectLod->m_LodProps[lodPropIndex];

				lodProps.resource = lodResPath;

				if (lodProps.distance == -1)
				{
					if (objectLod->m_LodProps.size() == 1 && objectLods.size() == 1)
					{
						lodProps.distance = VIEW_DISTANCE_MAX;
					}
					else if (p == objectLods.size()-1 && lodPropIndex == objectLod->m_LodProps.size()-1 && lodProps.lod*10000 <= VIEW_DISTANCE_MAX)
					{
						lodProps.distance = VIEW_DISTANCE_MAX;	
					}
					else
					{
						if (lodProps.lod == 1)
						{
							lodProps.distance = VIEW_DISTANCE_LOD1;
						}
						else if (lodProps.lod == 2)
						{
							lodProps.distance = VIEW_DISTANCE_LOD2;
						}
						else
						{
							lodProps.distance = lodProps.lod*10000;
						}
					}				
				}

				if (lodProps.shadow == -1)
				{
					if (lodProps.lod == 1)
					{
						lodProps.shadow = VIEW_SHADOW_LOD1;
					}
					else if (lodProps.lod == 2)
					{
						lodProps.shadow = VIEW_SHADOW_LOD2;
					}
					else
					{
						lodProps.shadow = 0;
					}
				}

				objectLodsProps.push_back(lodProps);
			}
		}

		objectLodsProps = SortLodsByDistance(objectLodsProps);
		lodsResPath = "";

		for (int lodIndex = 0; lodIndex < objectLodsProps.size(); lodIndex++)
		{
			LOD_PROPS lod = objectLodsProps[lodIndex];
			lodsResPath += lod.resource + ";";
			CString distanceStr = "";
			distanceStr.Format("%d", lod.distance);
			lodsResPath += distanceStr + ";";
			CString shadowStr = "";
			shadowStr.Format("%d", lod.shadow);
			lodsResPath += shadowStr + ";";
		}

		if(lodsResPath.IsEmpty())
		{
			lodsResPath = resPath;
		}

		DWORD	type = objectDesc->m_Type;
		if( type == vespace::VE_REFERENCE)
		{
			if( objectDesc->m_IsGroupHead)
				type |= vespace::VE_GROUP3D;
			else
				type |= vespace::VE_OBJECT3D;
		}

		DWORD dbID = objectDesc->m_DatabaseID;
		if( !objectDesc->m_bDatabaseSet)
		{
			if( CExporter::calexportGenerateDBID)
			{
				Cal3DComString sDBID;
				dbID = __dbCurrentMaxDB++;
				sDBID.Format("[__DBID=%u]", dbID);
				objectDesc->m_pBaseNode->SetName( objectDesc->m_pBaseNode->GetName() + sDBID.GetBuffer());
			}
		}

		// сохраняем тип
		vespace::VESPACE_OBJECTS_TYPES objectType = objectDesc->m_Type;

		// правка объекта-ссылки
		if (objectType == vespace::VE_REFERENCE)
		{
			objectType = vespace::VE_OBJECT3D;
			for (int p=0; p < vecSelectedObjects.size(); p++)
			{
				if (objectDesc->m_ReferenceName == vecSelectedObjects[p].m_Name)
				{
					lodsResPath = vecSelectedObjects[p].m_Res;
					objectDesc->m_ReferenceName = "ref";
					break;
				}
			}
		}
		else
		{
			// сохраняем для объектов ссылок путь к ресурсу
			objectDesc->m_Res = lodsResPath;		
		}

		if((objectDesc->m_pParentGroupNodeDescription!=NULL && 
			!objectDesc->m_pParentGroupNodeDescription->m_ExplodeGroup))
		{

			continue;
		}

		CString	sqlString = GetSQLObjectInsertionString( 
			dbID, 
			//objectDesc->m_Type,
			objectType,
			Cal3DComString( objectDesc->m_Name),
			Cal3DComString( objectDesc->m_ReferenceName),
			Cal3DComString( lodsResPath),			//Cal3DComString( lodsResPath + resPath),
			objectDesc->m_Level, objectDesc->m_Lod,
			Cal3DComString( objectDesc->m_Class),
			1,	/*version*/
			objectTranslation,
			objectScale,
			objectRotation, rad, sqID,
			&objectDesc->m_UserDefinedProperties, sitplaceObjects,
			objectDesc->m_Grass,
			Cal3DComString( objectDesc->m_zoneName),
			m_internalZonePivots,
			objectDesc->m_Properties);
		sceneSQLConfigLines.push_back( TWO_STRING(sqlString, ""));

		CString	sXmlString = GetXMLObjectInsertionString( 
			dbID, 
			//objectDesc->m_Type,
			objectType,
			Cal3DComString( objectDesc->m_Name),
			Cal3DComString( objectDesc->m_ReferenceName),
			Cal3DComString( lodsResPath),			//Cal3DComString( lodsResPath + resPath),
			objectDesc->m_Level, objectDesc->m_Lod,
			Cal3DComString( objectDesc->m_Class),
			1,	/*version*/
			objectTranslation,
			objectScale,
			objectRotation, rad, sqID,
			&objectDesc->m_UserDefinedProperties, sitplaceObjects, 
			objectDesc->m_Grass,
			Cal3DComString( objectDesc->m_zoneName),
			Cal3DComString( objectDesc->m_logicObjName),
			m_internalZonePivots,
			objectDesc->m_Properties);
		sceneXMLConfigLines.push_back( TWO_STRING(sXmlString, ""));

		if( objectDesc->m_IsGroupHead)
			sceneConfigLines.push_back( TWO_STRING("group", pathObjectDirectory + "\\" + configFileName));
		else	if( objectDesc->m_Type == vespace::VE_SKYBOX)
			sceneConfigLines.push_back( TWO_STRING("skybox", pathObjectDirectory + "\\" + configFileName));
		else
			sceneConfigLines.push_back( TWO_STRING("object3d", pathObjectDirectory + "\\" + configFileName));
	}

	// Save groups config
	for (i = 0; i < vecSelectedObjects.size(); i++)
	{
		MAP_NODE_DESCRIPTION* objectDesc = &vecSelectedObjects[i];
		if( objectDesc->m_IsGroupHead && 
			!objectDesc->m_ExplodeGroup)
		{
			// запишем конфиг для группы
			CString configFileName = GROUP_CONFIG_FILE_NAME;
			CString configPath = pathUserSelectedDirectory + "\\" + objectDesc->m_PathObjectDirectory + "\\" + configFileName;
			SaveGroupConfig( configPath, objectDesc, &vecSelectedObjects);
			// копируем конфиг на сервер
			if (ServerPath != "" && calexpoptAutoexportToServer)
			{
				CString copyPath = "";
				copyPath = copyPath + ServerPath.c_str() + "\\" + objectDesc->m_PathObjectDirectory.GetBuffer(0);
				CorrectPathSlashes(copyPath);
				fs.MakePath(copyPath);
				copyPath = copyPath + "\\" + configFileName;
				fs.DeleteFile(copyPath);
				fs.CopyFile(configPath, copyPath);				
			}
		}
	}

	// 6. Пишем скриптовый файл
	if( !sceneConfigLines.empty())
	{
		CString	pathSceneFile = fs.GetBaseFileName(pathUserSelectedFileName);
		fs.ChangeDirectory( pathUserSelectedDirectory);
		SaveSceneVESpaceConfig( pathSceneFile, sceneConfigLines);
		SaveSceneVESpaceSQL( pathSceneFile, sceneSQLConfigLines);
		SaveSceneVESpaceXML( pathSceneFile, sceneXMLConfigLines);
		
		if (ServerPath != "" && calexpoptAutoexportToServer)
		{
			if(!calexprotAutomatedExport)
			{
				//Открываем блокнот :)
				CString notepadParam = "";
				notepadParam.Format("notepad %s.xml", pathSceneFile.GetBuffer(0));
				_spawnlp(P_NOWAIT, "notepad", notepadParam.GetBuffer(0), NULL);
			}
			else 
			{
				//Используем более продвинутую систему
				HINTERNET hParserSession = 0;

				if (CExporter::exportGeometryOnly == true)
				{
					// не делаем ничего, т.к. в базу ничего в этом режиме не пишем
				}
				else
				{
					hParserSession = WinHttpOpen(L"VU exporter/1.0",
											WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
											WINHTTP_NO_PROXY_NAME,
											WINHTTP_NO_PROXY_BYPASS,
											0);
				}

				if(hParserSession)
				{
					std::string url = CAutoExportSettings::GetParserUrl().GetBuffer(0);
					std::string host, uri;

					if (url.find("://") != std::string::npos)
					{
						url = url.substr(url.find("://") + 3);
					}

					host = url.substr(0, url.find("/"));
					uri = url.substr(url.find("/"));

					wchar_t	*parserHost = new wchar_t[host.length()+1];
					wchar_t	*parserUri = new wchar_t[uri.length()+1];
					memset(parserHost, 0, url.length()+1);
					memset(parserUri, 0, url.length()+1);

					MultiByteToWideChar(CP_ACP, NULL, host.c_str(), -1, parserHost, host.length()+1);
					MultiByteToWideChar(CP_ACP, NULL, uri.c_str(), -1, parserUri, uri.length()+1);


					HINTERNET hParserConnection = WinHttpConnect(hParserSession,
												parserHost,
												INTERNET_DEFAULT_HTTP_PORT,											
												0);

					if(hParserConnection)
					{
						HINTERNET hParserRequest = WinHttpOpenRequest(hParserConnection,
												L"POST",
												parserUri,
												NULL,
												WINHTTP_NO_REFERER,
												WINHTTP_DEFAULT_ACCEPT_TYPES,
												0);

						if(hParserRequest)
						{
							WinHttpSetTimeouts(hParserRequest, 0, 20000, 20000, 20000);

							//AfxMessageBox("Request", MB_OK | MB_ICONEXCLAMATION);

							CString sceneFilePath = "";
							sceneFilePath.Format("%s.xml", pathSceneFile.GetBuffer(0));


							FILE* parserData = fopen(sceneFilePath.GetBuffer(0), "rb");

							fseek(parserData, 0, SEEK_END);
							long dataSize = ftell(parserData);
							fseek(parserData, 0, SEEK_SET);

							// get system user name
							char userNameBuf[255];
							DWORD bufSize = sizeof(userNameBuf);
							GetUserName(userNameBuf, &bufSize);

							std::string systemUserName(userNameBuf);
							int nameSize = systemUserName.length();
							
							char* dataBuffer = new char[dataSize + 11 + nameSize + 5];
							dataBuffer[0] = 's';
							dataBuffer[1] = 'y';
							dataBuffer[2] = 's';
							dataBuffer[3] = '=';
							memcpy(&dataBuffer[4], &userNameBuf[0], nameSize);
							//char* dataBuffer = new char[dataSize+5];
							//char* dataBuffer = new char[dataSize + 11];
							dataBuffer[nameSize + 4] = '&';
							dataBuffer[nameSize + 5] = 'l';
							dataBuffer[nameSize + 6] = 'i';
							dataBuffer[nameSize + 7] = 'b';
							dataBuffer[nameSize + 8] = '=';
							dataBuffer[nameSize + 9] = '0';
							if(libExport)
							{
								dataBuffer[nameSize + 9] = '1';
							}
							dataBuffer[nameSize + 10] = '&';

							dataBuffer[nameSize + 11] = 't';
							dataBuffer[nameSize + 12] = 'e';
							dataBuffer[nameSize + 13] = 'x';
							dataBuffer[nameSize + 14] = 't';
							dataBuffer[nameSize + 15] = '=';

							fread(&dataBuffer[nameSize + 16], 1, dataSize, parserData);
							fclose(parserData);

							dataSize += 11 + nameSize + 5;

							bool httpResult = false;

							httpResult = WinHttpAddRequestHeaders(hParserRequest, 
										L"Content-Type: application/x-www-form-urlencoded",
										-1L,
										WINHTTP_ADDREQ_FLAG_ADD | WINHTTP_ADDREQ_FLAG_REPLACE );

							if(httpResult)
							{
								httpResult = WinHttpSendRequest(hParserRequest,
												WINHTTP_NO_ADDITIONAL_HEADERS,
												0,
												&dataBuffer[0],
												dataSize,
												dataSize,
												0);

							}
							
							if (httpResult)
							{
								httpResult = WinHttpReceiveResponse(hParserRequest, NULL);
							}

							// WinHttpReadData 

							// Если произошла ошибка
							if (!httpResult)
							{
								int errCode = GetLastError();
								AfxMessageBox("Ошибка во время передачи данных на сервер", MB_OK | MB_ICONEXCLAMATION);
							}

							delete[] dataBuffer;

							WinHttpCloseHandle(hParserRequest);
						}

						WinHttpCloseHandle(hParserConnection);
					}

					if (hParserSession)
					{
						WinHttpCloseHandle(hParserSession);
					}					
				}

				SetCurrentDirectory(MaxInstallPath);

				CString reloadPath = MaxInstallPath;
				reloadPath += "77.bat";
				WinExec(reloadPath, SW_SHOW);

				reloadPath = MaxInstallPath;
				reloadPath += "78.bat";
				WinExec(reloadPath, SW_SHOW);
			}
		}
	}

	SetCurrentDirectory( szCurrentDir);

	if (TootleIsInited)
	{
		TootleCleanup();
	}

	return true;
}


void	CExporter::CreateBMFFile(const CString& meshFileName, MAP_NODE_DESCRIPTION* objectDesc,
									CMeshCandidate& meshCandidate,
									MaterialsMap& mapMaterialsNames,
									std::vector<int>& materialsIds,
									CString&	pathAbsoluteObjectDirectory,
									CString&	pathMaterials,
									CString&	pathObjectDirectory,
									std::vector<PRIMITIVE_DESCRIPTION>& colliderObjects,
									CString& maxPath)
{
	WriteLog("Start export " + meshFileName);

	BYTE tmpByte;
	WORD tmpWord;
	short tmpShort;

	// используем для поиска крашей
	// 
	FILE* fl2 = fopen("c:\\log.txt", "wb");
	if( fl2)
	{
		fprintf(fl2, meshFileName);
		fclose(fl2);
	}

	FILE* fl = fopen(meshFileName, "wb");
	if( !fl)
	{
		MessageBox(0, "Can`t open file: " + meshFileName, "Error!", MB_OK);
		return;
	}

	CString	shader_path="";
	for (int i=0; i<objectDesc->m_Properties.size(); i++)
	{
		if( objectDesc->m_Properties[i].key == "shader")
		{
			shader_path = objectDesc->m_Properties[i].value;
			break;
		}
	}

	// записываем сигнатуру
	char sign[4] = "BMF";
	fwrite(&sign[0], 1, 3, fl);

	CalVector tmpTranslation;
	CalQuaternion tmpRotation;
	CalVector tmpScale;
	if( objectDesc->m_pParentGroupNodeDescription &&
		!objectDesc->m_pParentGroupNodeDescription->m_ExplodeGroup)
	{
		GetNodeTransform(objectDesc, tmpTranslation, tmpRotation, tmpScale, true);
	}
	else
	{
		GetNodeTransform(objectDesc, tmpTranslation, tmpRotation, tmpScale);
	}

	// записываем версию
	tmpByte = BINARY_MESH_VERSION;
	fwrite(&tmpByte, 1, 1, fl);

	fwrite(&tmpTranslation, 1, sizeof(tmpTranslation), fl);
	fwrite(&tmpRotation, 1, sizeof(tmpRotation), fl);
	fwrite(&tmpScale, 1, sizeof(tmpScale), fl);

	// ModelDescription
	if (BINARY_MESH_VERSION < 4)
	{
		fwrite(&objectCfg.is_reference, 1, 1, fl);
		if (objectCfg.is_reference)
		{
			fwrite(objectCfg.ref_name.GetBuffer(0), 1, objectCfg.ref_name.GetLength() + 1, fl);
			fwrite(&tmpRotation, 1, 16, fl);
			fwrite(&tmpTranslation, 1, 12, fl);
			fwrite(&tmpScale, 1, 12, fl);
		}
		else
		{
			fwrite(&objectCfg.scale, 1, 4, fl);
			fwrite(objectCfg.collision_type.GetBuffer(0), 1, objectCfg.collision_type.GetLength() + 1, fl);
			fwrite(objectCfg.collision_src.GetBuffer(0), 1, objectCfg.collision_src.GetLength() + 1, fl);
			fwrite(objectCfg.shader.GetBuffer(0), 1, objectCfg.shader.GetLength() + 1, fl);
		}

		tmpByte = objectCfg.properties.size();
		fwrite(&tmpByte, 1, 1, fl);

		int j;
		for (j = 0; j < tmpByte; j++)
		{
			fwrite(objectCfg.properties[j].name.GetBuffer(0), 1, objectCfg.properties[j].name.GetLength() + 1, fl);
			fwrite(objectCfg.properties[j].value.GetBuffer(0), 1, objectCfg.properties[j].value.GetLength() + 1, fl);
		}

		tmpByte = objectCfg.boxes.size();
		fwrite(&tmpByte, 1, 1, fl);
		for (j = 0; j < tmpByte; j++)
		{
			fwrite(&objectCfg.boxes[j], 1, sizeof(box3d), fl);
		}

		if( objectCfg.is_reference)
		{
			fclose(fl);
			return;
		}
	}

	WriteLog("Start export materials " + meshFileName);

	// кол-во материалов
	tmpWord = materialsIds.size();
	fwrite(&tmpWord, 1, 2, fl);

	WORD fsOffset = 0;
	WORD vxOffset = 0;

	SECFileSystem fs;

	// MaterialDescription
	int _vxOffset[1000];
	int j;
	for (j = 0; j < tmpWord; j++)
	{
		CString materialFileName = GetMaterialName(j, meshCandidate, materialsIds,
			mapMaterialsNames, objectDesc, &fs, pathAbsoluteObjectDirectory, pathMaterials);

		FILE* fl2 = fopen(materialFileName.GetBuffer(0), "rb");
		fseek(fl2, 0, 2);
		BYTE fsize  = ftell(fl2);
		fclose(fl2);

		fwrite(&fsize, 1, 1, fl);
		fwrite(&fsOffset, 1, 2, fl);
		fwrite(&vxOffset, 1, 2, fl);
		fwrite(&m_facesInMaterial[materialsIds[j]], 1, 2, fl);


		// Get Real File Name
		CString sRealMaterialFileName;
		//if( calexpoptGetnMtrlsNames)
		{
			StdMat*	material = meshCandidate.GetMtrl(materialsIds[j]);
			ASSERT(material);
			if( material)
			{
				sRealMaterialFileName = material->GetName();
			}
		}

		int startIndex = sRealMaterialFileName.Find("[");
		int endIndex = sRealMaterialFileName.Find("]");
		char tmp[200];
		int cnt = 0;
		
		if ((startIndex > 0) && (endIndex > 0))
		{
			startIndex++;

			while (startIndex != endIndex)
			{
				tmp[cnt++] = sRealMaterialFileName[startIndex++];
			}
		}

		tmp[cnt] = 0;

		if (BINARY_MESH_VERSION < 4)
		{
			fwrite(&tmp[0], 1, strlen(tmp) + 1, fl);
		}			

		_vxOffset[j] = vxOffset;

		fsOffset += m_facesInMaterial[materialsIds[j]];
		vxOffset += m_vertexInMaterial[materialsIds[j]];		
	}

	BYTE* buf = new BYTE[10000];
	for (j = 0; j < tmpWord; j++)
	{
		CString materialFileName = GetMaterialName(j, meshCandidate, materialsIds,
			mapMaterialsNames, objectDesc, &fs, pathAbsoluteObjectDirectory, pathMaterials);

		FILE* fl2 = fopen(materialFileName, "rb");
		if(!fl2)
		{
			MessageBox(0, materialFileName.GetBuffer(0), "Can`t open material file! ", MB_OK);
			continue;
		}
		fseek(fl2, 0, 2);
		int fsize  = ftell(fl2);
		fseek(fl2, 0, 0);
		fread(&buf[0], 1, fsize, fl2);
		fclose(fl2);

		unsigned char* _pMatCount = &buf[8];
		unsigned short* pMatCount = (unsigned short*)_pMatCount;
		pMatCount++;
		*pMatCount = m_texLevelsForVertex[_vxOffset[j]];

		fwrite(&buf[0], 1, fsize, fl);
	}

	delete[] buf;


	if (BINARY_MESH_VERSION < 4)
	{
		unsigned skeletonSize;

		// а вот не сохраняем пока скелет и все!
		skeletonSize = 0;

		fwrite(&skeletonSize, 1, 2, fl);

		if (skeletonSize > 0)
		{
			// write skeleton file
		}
	}

	WriteLog("Start export faces and vertices " + meshFileName);

	unsigned vertexCount, faceCount;

	// кол-во фейсов и вершин
	faceCount = (int)(m_faces.size() / 3);
	vertexCount = m_vertex.size();

	fwrite(&vertexCount, 1, 2, fl);
	fwrite(&faceCount, 1, 2, fl);

//////////////////////////////////////////////////////////////////////////
// Для высокополигональных объектов
	if(BINARY_MESH_VERSION > 4)
	{
		int useFloat = CExporter::useFloatVertexCoords;
		fwrite(&useFloat, 1, 1, fl);
	}
//////////////////////////////////////////////////////////////////////////

	//int i = 0;

	// VertexDescription
	float minX, minY, minZ, maxX, maxY, maxZ;
	minX = FLT_MAX;
	minY = FLT_MAX;
	minZ = FLT_MAX;
	maxX = -FLT_MAX;
	maxY = -FLT_MAX;
	maxZ = -FLT_MAX;

	for (j = 0; j < vertexCount; j++)
	{
		if (m_vertex[j].x > maxX)
		{
			maxX = m_vertex[j].x;
		}

		if (m_vertex[j].y > maxY)
		{
			maxY = m_vertex[j].y;
		}

		if (m_vertex[j].z > maxZ)
		{
			maxZ = m_vertex[j].z;
		}

		if (m_vertex[j].x < minX)
		{
			minX = m_vertex[j].x;
		}

		if (m_vertex[j].y < minY)
		{
			minY = m_vertex[j].y;
		}

		if (m_vertex[j].z < minZ)
		{
			minZ = m_vertex[j].z;
		}
	}

	if( BINARY_MESH_VERSION == 1)
	{
		float offX = (minX + maxX) / 2.0f;
		float offY = (minY + maxY) / 2.0f;
		float offZ = (minZ + maxZ) / 2.0f;

		float absMaxX = (maxX - minX) / 2.0f;
		float absMaxY = (maxY - minY) / 2.0f;
		float absMaxZ = (maxZ - minZ) / 2.0f;

		float scaleX = 32767.0f / absMaxX;
		float scaleY = 32767.0f / absMaxY;
		float scaleZ = 32767.0f / absMaxZ;

		float scaleXY = (scaleX < scaleY)? scaleX : scaleY;
		float scale = (scaleXY < scaleZ)? scaleXY : scaleZ;

		fwrite(&scale, 1, 4, fl);
		fwrite(&offX, 1, 4, fl);
		fwrite(&offY, 1, 4, fl);
		fwrite(&offZ, 1, 4, fl);

		int texI = 0;

		for (j = 0; j < vertexCount; j++)
		{
			float tmp = m_vertex[j].x;
			tmpShort = (short)((tmp - offX) * scale);
			fwrite(&tmpShort, 1, 2, fl);
			tmpShort = (short)((m_vertex[j].y - offY) * scale);
			fwrite(&tmpShort, 1, 2, fl);
			tmpShort = (short)((m_vertex[j].z - offZ) * scale);
			fwrite(&tmpShort, 1, 2, fl);

			for (int k = 0; k < m_texLevelsForVertex[j]; k++)
			{
				tmpWord = (WORD)((m_texCoords[texI].x + 128.0f) * 255.0f);
				fwrite(&tmpWord, 1, 2, fl);
				tmpWord = (WORD)((m_texCoords[texI].y + 128.0f) * 255.0f);
				fwrite(&tmpWord, 1, 2, fl);
				texI++;
			}
		}
	}
	else
	{
		double centerX = minX;
		double centerY = minY;
		double centerZ = minZ;

		double absSizeX = (maxX - minX)/* / 2.0f*/;
		double absSizeY = (maxY - minY)/* / 2.0f*/;
		double absSizeZ = (maxZ - minZ)/* / 2.0f*/;

		double scaleX = (int)(absSizeX / 65535.0) + 1;
		double scaleY = (int)(absSizeY / 65535.0) + 1;
		double scaleZ = (int)(absSizeZ / 65535.0) + 1;

		double scaleXY = (scaleX > scaleY)? scaleX : scaleY;
		int scale = (scaleXY > scaleZ)? scaleXY : scaleZ;

		int iCenterX = (int)(floorf(centerX / scale));
		int iCenterY = (int)(floorf(centerY / scale));
		int iCenterZ = (int)(floorf(centerZ / scale));
		
		fwrite(&scale, 1, sizeof(scale), fl);
		fwrite(&iCenterX, 1, sizeof(iCenterX), fl);
		fwrite(&iCenterY, 1, sizeof(iCenterY), fl);
		fwrite(&iCenterZ, 1, sizeof(iCenterZ), fl);

		int texI = 0;

		for (j = 0; j < vertexCount; j++)
		{
#ifdef DEBUG
#define NEED_X	 20648.0
#define NEED_Y	 3740.0
#define NEED_Z	 5.3
CalVector needPosition( NEED_X, NEED_Y, NEED_Z);
if( (CalVector(m_vertex[j].x+tmpTranslation.x,m_vertex[j].y+tmpTranslation.y,m_vertex[j].z+tmpTranslation.z)-needPosition).length() < 3.0f)
int jshf=0;
#endif
			float tmp;
			if (BINARY_MESH_VERSION > 4 && CExporter::useFloatVertexCoords)
			{
				tmp = m_vertex[j].x;
				fwrite(&tmp, 1, sizeof(tmp), fl);

				tmp = m_vertex[j].y;
				fwrite(&tmp, 1, sizeof(tmp), fl);

				tmp = m_vertex[j].z;
				fwrite(&tmp, 1, sizeof(tmp), fl);				
			}
			else
			{
				tmp = m_vertex[j].x;
				tmpShort = (short)(floorf(tmp - iCenterX) / scale);
				fwrite(&tmpShort, 1, sizeof(tmpShort), fl);

				tmp = m_vertex[j].y;
				tmpShort = (short)(floorf(tmp - iCenterY) / scale);
				fwrite(&tmpShort, 1, sizeof(tmpShort), fl);

				tmp = m_vertex[j].z;
				tmpShort = (short)(floorf(tmp - iCenterZ) / scale);
				fwrite(&tmpShort, 1, sizeof(tmpShort), fl);
			}

			for (int k = 0; k < m_texLevelsForVertex[j]; k++)
			{
				if( BINARY_MESH_VERSION <= 2)
				{
					tmpWord = (WORD)((m_texCoords[texI].x + 128.0f) * 255.0f);
					fwrite(&tmpWord, 1, 2, fl);
					tmpWord = (WORD)((m_texCoords[texI].y + 128.0f) * 255.0f);
					fwrite(&tmpWord, 1, 2, fl);
				}
				else
				{
					fwrite(&m_texCoords[texI].x, 1, sizeof(m_texCoords[texI].x), fl);
					fwrite(&m_texCoords[texI].y, 1, sizeof(m_texCoords[texI].y), fl);
				}
				texI++;
			}
		}
	}

	// FaceDescription
	for (j = 0; j < faceCount * 3; j++)
	{
		//WORD indices = m_faces[j];

		if (m_faces[j] >= vertexCount)
		{
			m_faces[j] = 0;
		}

		fwrite(&m_faces[j], 1, 2, fl);
	}

	WriteLog("Start export collision boxes " + meshFileName);

	if (BINARY_MESH_VERSION > 3)
	{
		// Number of primitives
		tmpWord = colliderObjects.size();
		fwrite(&tmpWord, 1, 2, fl);

		// CollisionPrimitive
		PRIMITIVE_DESCRIPTION colliderObj;

		CalVector colliderTranslation;
		CalQuaternion colliderRotation;
		CalVector colliderScale;

	    for (j = 0; j < colliderObjects.size(); j++)
		{
			colliderObj = colliderObjects[j];

			tmpByte = colliderObj.type;
			fwrite(&tmpByte, 1, 1, fl);

			colliderTranslation = colliderObj.translation;
			colliderRotation = colliderObj.rotation;
			colliderScale = colliderObj.scale;

			colliderTranslation = colliderTranslation - tmpTranslation;

			CalQuaternion cpyRotation = tmpRotation;
			cpyRotation.conjugate();
			colliderTranslation = colliderTranslation*cpyRotation;

			colliderRotation = colliderRotation*cpyRotation;
		
			//colliderScale.set(colliderScale.x/tmpScale.x, colliderScale.y/tmpScale.y, colliderScale.z/tmpScale.z);

			if(colliderObj.type == 1) // box
			{
				fwrite(&colliderTranslation, 1, 12, fl);
				fwrite(&colliderRotation, 1, 16, fl);
				fwrite(&colliderScale, 1, 12, fl);

				float x_length = abs(colliderObj.xmax - colliderObj.xmin);
				float y_length = abs(colliderObj.ymax - colliderObj.ymin);
				float z_length = abs(colliderObj.zmax - colliderObj.zmin);

				fwrite(&x_length, 1, 4, fl);
				fwrite(&y_length, 1, 4, fl);
				fwrite(&z_length, 1, 4, fl);
			}
			else if(colliderObj.type == 2) // sphere
			{
				fwrite(&colliderTranslation, 1, 12, fl);			
				fwrite(&colliderScale, 1, 12, fl);

				float r = abs(colliderObj.xmax - colliderObj.xmin)/2;
				fwrite(&r, 1, 4, fl);
			}
		}
	}

	fclose(fl);


	WriteLog("Start zlib packing " + meshFileName);

	// Упаковка файла
	if (BINARY_MESH_VERSION > 3)
	{
		// пакуем zip

		ZlibEngine zip;	

		//CString zipFileName = OBJECT_CONFIG_FILE_NAME;
		//zipFileName += ".zip";
		//zip.compress(meshFileName, zipFileName.GetBuffer(0));

		FILE* fin = fopen(meshFileName, "rb");
		fseek(fin, 0, 2);
		unsigned long finSize = ftell(fin);
		unsigned int uncompressedSize = finSize;
		char* finBuf = new char[finSize];
		fseek(fin, 0, 0);
		fread(&finBuf[0], 1, finSize, fin);
		fclose(fin);

		std::string fnBMF = meshFileName;
		fnBMF = fnBMF.substr(0, fnBMF.size() - 5);
		fin = fopen(fnBMF.c_str(), "wb");
		fwrite(&finBuf[0], 1, finSize, fin);
		fclose(fin);

		_unlink(meshFileName);

		unsigned long foutSize = finSize + finSize*0.02 + 12;
		char* foutBuf = new char[foutSize];	
		zip.compmem(finBuf, &finSize, foutBuf, &foutSize);
		

		FILE* fout = fopen(OBJECT_CONFIG_FILE_NAME, "wb");
		// записываем размер в первые 4 байта
		fwrite(&uncompressedSize, sizeof(int), 1, fout);

		fwrite(&foutBuf[0], 1, foutSize, fout);
		fclose(fout);
		
		delete[] finBuf;
		delete[] foutBuf;
	}

	WriteLog("Start copying to server " + meshFileName);

	// копируем в папку сервера
	if (ServerPath != "" && calexpoptAutoexportToServer)
	{
		CString copyPath = "";
		copyPath = copyPath + ServerPath.c_str() + "\\" + pathObjectDirectory.GetBuffer(0);

		CorrectPathSlashes(copyPath);
		fs.MakePath(copyPath);

		CString copyMaterialsPath = copyPath + "\\" + MATERIAL_TEXTURES_FILE_NAME;

		CString dirPath = copyPath + "\\";
		copyPath = dirPath + OBJECT_CONFIG_FILE_NAME;
		fs.DeleteFile(copyPath);
		fs.CopyFile(OBJECT_CONFIG_FILE_NAME, copyPath);

		// копируем список текстур materials.xml
		fs.DeleteFile(copyMaterialsPath);
		fs.CopyFile(MATERIAL_TEXTURES_FILE_NAME, copyMaterialsPath);
		
		CString textureViewerPath = maxPath;
		textureViewerPath += "textures_viewer.exe ";

		CString textureViewerParams = dirPath + " diff";

		WinExec(textureViewerPath + textureViewerParams, SW_SHOW);
	}
	
	WriteLog("End export " + meshFileName);
}

void	CExporter::SaveGroupConfig(const CString& meshFileName, MAP_NODE_DESCRIPTION* objectDesc, std::vector<MAP_NODE_DESCRIPTION>*	aSelectedObjects)
{
	ATLASSERT( objectDesc);
	CFile	file;

	try
	{
		if( !(file.Open(meshFileName, CFile::modeCreate | CFile::modeWrite)))
		{
			return;
		}
	}
	catch(...)
	{
	}

	CString fline;
	fline = "<?xml version=\"1.0\" encoding=\"ASCII\"?>\n<group>\n";	
	file.Write( fline.GetBuffer(0), fline.GetLength());

	std::vector<MAP_NODE_DESCRIPTION>::iterator	it = aSelectedObjects->begin(),
												end = aSelectedObjects->end();
	for ( ; it!=end; ++it)
	{
		if( (it->m_pParentGroupNodeDescription == objectDesc) && (it->m_Lod == 0))
		{
			//CString	objectFileName = it->m_PathObjectDirectory + "\\" + OBJECT_CONFIG_FILE_NAME;
			CString	objectFileName = it->m_Res;
			CString	groupFileName = objectDesc->m_PathObjectDirectory;
			if( objectFileName.Find(groupFileName) == 0)
			{
				//objectFileName = objectFileName.Right( objectFileName.GetLength() - groupFileName.GetLength());
				objectFileName.Replace(groupFileName, "");
			}
			if( it->m_IsGroupHead)
			{
				ATLASSERT( FALSE);// группа в группе
				fline = "<item type=\"group\" ";
			}
			else
			{
				fline = "<item type=\"object3d\" ";
			}
			if( !it->m_Name.IsEmpty())
				fline += "name=\"" + it->m_Name + "\" ";
			fline += " src=\"" + objectFileName + "\"/>\n";

			file.Write( fline.GetBuffer(0), fline.GetLength());
		}
	}
	fline = "<\\group>\n";	
	file.Write( fline.GetBuffer(0), fline.GetLength());
	file.Close();
}

#endif
