//----------------------------------------------------------------------------//
// Exporter.cpp                                                               //
// Copyright (C) 2001, 2002 Bruno 'Beosil' Heidelberger                       //
//----------------------------------------------------------------------------//
// This program is free software; you can redistribute it and/or modify it    //
// under the terms of the GNU General Public License as published by the Free //
// Software Foundation; either version 2 of the License, or (at your option)  //
// any later version.                                                         //
//----------------------------------------------------------------------------//

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
#ifdef MAX3DS_BUILD
	#include "MaxMaterial.h"
	#include "MaxMesh.h"
	#include "MaxLight.h"
	#include "MaxFlare.h"
	#include "scenelightsdlg.h"
	#include "flaresdlg.h"
#endif

#define NATURA3D_API
#include "3DobjectConfig.h"
#include "FileSys.h"
#include "coreanimationflags.h"
#include <algorithm>

//----------------------------------------------------------------------------//
// Debug                                                                      //
//----------------------------------------------------------------------------//

// Export options
BOOL	CExporter::calexpoptGenerateGlobalCoords = FALSE;
BOOL	CExporter::calexpoptEXPORTSHADOWDATA = FALSE;
BOOL	CExporter::calexpoptUSELOCALCOORDS = FALSE;
BOOL	CExporter::calexpoptSTAYATLAST;
BOOL	CExporter::calexpoptKEEPFRAME;
BOOL	CExporter::calexpoptKEEPROTATIONSINANIMATION;
BOOL	CExporter::calexpoptKeepConfig;
BOOL	CExporter::calexpoptNORMALIZEVERTS;
float	CExporter::calexportFLOORZPOS;
int		CExporter::maxMaterialIndex;
//BOOL	CExporter::calexpoptExportMaterials = FALSE;	// экспортировать материалы
BOOL	CExporter::calexpoptExportMaterials = TRUE;

//BOOL	CExporter::calexpoptUseTexturesPath = FALSE;	// использовать путь для записи файлов с текстурой
BOOL	CExporter::calexpoptUseTexturesPath = TRUE;

//BOOL	CExporter::calexpoptUseMaterialsPath = FALSE;	// использовать путь для записи файлов с материалом
BOOL	CExporter::calexpoptUseMaterialsPath = TRUE;

//BOOL	CExporter::calexpoptGetnMtrlsNames = FALSE;	// генерировать названия материалов
BOOL	CExporter::calexpoptGetnMtrlsNames = TRUE;

int		CExporter::calexpoptAutoReplaceSameMaterials = 0;	// 0 - спрашивать, 1 - переписывать, 2 - не переписывать
int		CExporter::calexpoptAutoReplaceSameTextures = 0;
CString	CExporter::calexportTexturesPath = "..\\textures\\";
CString	CExporter::calexportMaterialsPath = "..\\materials\\";
CString	CExporter::calexportExportDir = "";
BOOL	CExporter::calexpoptUseBakedIfExists = TRUE;	// использовать baked материал, если он есть
BOOL	CExporter::calexpoptOptimizeAnimations = FALSE;	// оптимизировать анимацию (выкидывает "неинформативные" кадры)

//BOOL	CExporter::calexportGenerateSQL = FALSE;	// создать SQL разбиение на зоны
BOOL	CExporter::calexportGenerateSQL = TRUE;

BOOL	CExporter::calexpoptAutoexportToServer = FALSE;	// скопировать на сервер материалы и модели

BOOL	CExporter::calexprotUseLeftAssigment2Right = FALSE;	// использовать левую привязку на правую
BOOL	CExporter::calexprotUseRigthAssigment2Left = FALSE;	// использовать правую привязку на левую
float	CExporter::calexprotPosCompareValueExact = 0.0001f;	// точность сравнения позиции вершин с разных сторон
BOOL	CExporter::calexprotIsUseSide2SideEffect = FALSE;	// установка весов точек с одной стороны проходила для точек другой стороны

/************************************************************************/
/* Для города
/************************************************************************/
BOOL	CExporter::calexportUsePrefixAsDirName = TRUE;		// использовать префикс имени объекта как имя для директории
BOOL	CExporter::calexportSeparateCommonObjects = TRUE;	// сохранять "общие объекты" в отдельную папку
BOOL	CExporter::calexportSeparateCommonTextures = TRUE;	// сохранять "общие текстуры" в отдельную папку
BOOL	CExporter::calexportFindLODs = TRUE;				// искать и сохранять лоды
BOOL	CExporter::calexportGenerateDBID = FALSE;			// назначать объектам DBID
BOOL	CExporter::calexprotAutomatedExport = FALSE;		// автоматический экспорт без всяких подтверждений
BOOL	CExporter::calexportAbsoluteTexturesPaths = FALSE;		// при установке в true - все текстуры копируются в папку calexportTexturesPath относительно папки экспорта

CString	CExporter::MaxInstallPath = "C:\\Program Files\\Autodesk\\3ds Max 2009\\"; // путь по-умолчанию
CString CExporter::materialExportFormat = ""; // формат экспорта текстур (не используется)
int		CExporter::texturesExportQuality = 50;

BOOL	CExporter::useFloatVertexCoords = FALSE;	// отключение по умолчанию флоат вершин
BOOL	CExporter::exportGeometryOnly = FALSE;	// по умолчанию экспортируем геометрию и пишем в БД
BOOL	CExporter::exportWithoutTextures = FALSE;	// по умолчанию экспортируем текстуры
BOOL	CExporter::libExport = FALSE; // отключить экспорт в библиотеку


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define	VB_WALKABLE		1
#define	VB_INTANGIBLE	2
#define	VB_UNPICKABLE	4
#define STENSIL_NAME_SIGN	_T("[ss]")

//----------------------------------------------------------------------------//
// The one and only exporter instance                                         //
//----------------------------------------------------------------------------//
//! return A value from ARGB format
#define A_ARGB(x)			((x&0xFF000000)>>24)
//! return R value from ARGB format
#define B_ARGB(x)			((x&0x00FF0000)>>16)
//! return G value from ARGB format
#define G_ARGB(x)			((x&0x0000FF00)>>8)
//! return B value from ARGB format
#define R_ARGB(x)			(x&0x000000FF)


SObjectCfg objectCfg;
CExporter theExporter;

//----------------------------------------------------------------------------//
// Constructors                                                               //
//----------------------------------------------------------------------------//
BOOL CreatePath( CString filePath)
{
	SECFileSystem	fd;
	if( fd.DirectoryExists( fd.AppendPaths(fd.GetCurrentDirectory(), filePath)))	return TRUE;
	return fd.MakePath( filePath);
}

bool CheckDirectories()
{
	if( CExporter::calexpoptUseMaterialsPath &&
		!CExporter::calexportMaterialsPath.IsEmpty() &&
		!CreatePath( CExporter::calexportMaterialsPath))
	{
		MessageBox(0, "Неправильное имя папки для материалов", "Ошибка!", MB_OK);
		return false;
	}
	if( CExporter::calexpoptUseTexturesPath &&
		!CExporter::calexportTexturesPath.IsEmpty() &&
		!CreatePath( CExporter::calexportTexturesPath))
	{
		MessageBox(0, "Неправильное имя папки для текстур", "Ошибка!", MB_OK);
		return false;
	}
	return true;
}

CExporter::CExporter()
{
	m_pInterface = 0;
	overAmbient = overDiffuse = FALSE;
	dwAmbient = dwDiffuse = 0xFFFFFFFF;
}

//----------------------------------------------------------------------------//
// Destructor                                                                 //
//----------------------------------------------------------------------------//

CExporter::~CExporter()
{
}

//----------------------------------------------------------------------------//
// Create an exporter instance for a given interface                          //
//----------------------------------------------------------------------------//

bool CExporter::Create(CBaseInterface *pInterface)
{
	// check if a valid interface is set
	if(pInterface == 0)
	{
		SetLastError("Invalid handle.", __FILE__, __LINE__);
		return false;
	}

	m_pInterface = pInterface;

	return true;
}

//----------------------------------------------------------------------------//
// Export the animation to a given file                                       //
//----------------------------------------------------------------------------//

bool CExporter::ExportAnimation(const std::string& strFilename)
{
	CString csFullPath = strFilename.c_str();

	// check if a valid interface is set
	if(m_pInterface == 0)
	{
		SetLastError("Invalid handle.", __FILE__, __LINE__);
		return false;
	}

	// build a skeleton candidate
	CSkeletonCandidate skeletonCandidate;
	calexpoptSTAYATLAST = FALSE;
	calexpoptKEEPFRAME = FALSE;
	calexpoptKEEPROTATIONSINANIMATION = FALSE;
	calexportFLOORZPOS = 0;

	bool bRelativeAnimation;
	bool bSaveFirst;
	bool bSaveLast;
	bool bCommon;
	// show export wizard sheet
	CAnimationExportSheet sheet("Cal3D Animation Export", m_pInterface->GetMainWnd());
	CString sCAFName(strFilename.c_str());
	sheet.SetCAFName(sCAFName);
	sheet.SetSkeletonCandidate(&skeletonCandidate);
	sheet.SetAnimationTime(m_pInterface->GetStartFrame(), m_pInterface->GetEndFrame(), m_pInterface->GetCurrentFrame(), m_pInterface->GetFps());
	sheet.SetWizardMode();
	if(sheet.DoModal() != ID_WIZFINISH) return true;

	POSITION pos;
	pos=sheet.m_exportConfig.mslSections.GetHeadPosition();
	if (pos==NULL)
	{
		// if only single export
		// do one cycle step
		pos=(POSITION)1;
	}
	// start export animations
	while (pos)
	{
		if (sheet.m_skeletonFilePage.bExportAllMode)
		{
			// export all sections
			CString sSectionName=sheet.m_exportConfig.mslSections.GetNext(pos);
			if (!sheet.SetSection(sSectionName))
			{
				// errors ??
				return false;
			}

			// create the skeleton candidate from the skeleton file
			if(!sheet.m_skeletonFilePage.m_pSkeletonCandidate->CreateFromSkeletonFile((LPCTSTR)sheet.m_exportConfig.msCSF))
			{
				AfxMessageBox(theExporter.GetELastError().c_str(), MB_OK | MB_ICONEXCLAMATION);
				return -1;
			}

			csFullPath = sheet.m_exportConfig.msCAFName;
		}
		else
		{
			// only first config section in ECF
			/*pos=sheet.m_exportConfig.mslSections.GetHeadPosition();
			sheet.m_exportConfig.SetSectionActive(sheet.m_exportConfig.mslSections.GetNext(pos));
			sheet.SetAnimationTime(sheet.m_exportConfig.miStartFrame, sheet.m_exportConfig.miEndFrame, 
				sheet.m_exportConfig.miDispl, sheet.m_exportConfig.miFrameRate);*/
			sheet.SaveECF();
			// end loop
			pos=NULL;
		}

		bRelativeAnimation = sheet.GetRelState();
		bSaveFirst = sheet.GetFState();
		bSaveLast = sheet.GetLState();
		bCommon = sheet.GetCommonState();

		// get the number of selected bone candidates
		int selectedCount;
		selectedCount = skeletonCandidate.GetSelectedCount();
		if(selectedCount == 0)
		{
			SetLastError("No bones selected to export.", __FILE__, __LINE__);
			return false;
		}

		// create the core animation instance
		CalCoreAnimation coreAnimation;
		if(!coreAnimation.create())
		{
			SetLastError("Creation of core animation instance failed.", __FILE__, __LINE__);
			return false;
		}

		if( calexpoptSTAYATLAST)
			coreAnimation.setFlag(CAL3D_COREANIM_STAYATEND);

		// set the duration of the animation
		float duration;
		duration = (float)(sheet.GetEndFrame() - sheet.GetStartFrame()) / (float)sheet.GetFps();
		coreAnimation.setDuration(fabs(duration));
		coreAnimation.setRelMode( bRelativeAnimation);
		coreAnimation.saveFState( bSaveFirst);
		coreAnimation.saveLState( bSaveLast);

		// get bone candidate vector
		std::vector<CBoneCandidate *>& vectorBoneCandidate = skeletonCandidate.GetVectorBoneCandidate();

		int boneCandidateId;
		for(boneCandidateId = 0; boneCandidateId < vectorBoneCandidate.size(); boneCandidateId++)
		{
			// get the bone candidate
			CBoneCandidate *pBoneCandidate;
			pBoneCandidate = vectorBoneCandidate[boneCandidateId];

			// only create tracks for the selected bone candidates
			if(pBoneCandidate->IsSelected())
			{
				// allocate new core track instance
				
				if(!CreateCoreTrack(pBoneCandidate, boneCandidateId, bCommon, &coreAnimation))
				{
					SetLastError("Memory allocation failed.", __FILE__, __LINE__);
					coreAnimation.destroy();
					m_pInterface->StopProgressInfo();
					return false;
				}
				if( bCommon)
				{
					CalCoreTrack *pCoreTrack;
					pCoreTrack = coreAnimation.getCoreTrack(boneCandidateId);
					if( pCoreTrack && boneCandidateId>Cal::PROJECTION_BONE_ID)
					{
						CalVector		translation;
						CalQuaternion	rotation;
						skeletonCandidate.GetTranslationAndRotation(boneCandidateId, 0.0, translation, rotation);
						pCoreTrack->m_coreCommonBoneLength = translation.length();
					}
				}
			}
		}

		keepFrameRotationSet = false;

		// start the progress info
		m_pInterface->StartProgressInfo("Exporting to animation file...");

		// calculate the end frame and the displacement
		int endFrame;
		endFrame = fabs(duration * (float)sheet.GetFps());
		endFrame++;

		int frame;
		for(frame = 0; frame < endFrame; frame++)
		{
			// update the progress info
			m_pInterface->SetProgressInfo(100.0f * (float)frame / (float)endFrame);

			// calculate the displaced frame
			int displacedFrame;
			displacedFrame = (frame + sheet.GetDisplacement()) % endFrame;		//0 .. endFrame

			// calculate time in seconds
			float time;
			if (duration > 0)
			{
				time = ((float)sheet.GetStartFrame() + (float)displacedFrame) / (float)m_pInterface->GetFps();
			}
			else
			{
				time = ((float)sheet.GetStartFrame() - (float)displacedFrame) / (float)m_pInterface->GetFps();
			}
			
			//time = (float)sheet.GetStartFrame() / (float)sheet.GetFps() + (float)displacedFrame / (float)sheet.GetFps();

			for(boneCandidateId = 0; boneCandidateId < vectorBoneCandidate.size(); boneCandidateId++)
			{
				// get the bone candidate
				CBoneCandidate *pBoneCandidate;
				pBoneCandidate = vectorBoneCandidate[boneCandidateId];

				// only export keyframes for the selected bone candidates
				if(pBoneCandidate->IsSelected())
				{
					// allocate new core keyframe instance
					float settime=(float)frame / (float)sheet.GetFps();
					if(!AddCoreKeyframe(boneCandidateId, settime, time, &skeletonCandidate, &coreAnimation))
					{
						SetLastError("AddCoreKeyframe failed.", __FILE__, __LINE__);
						coreAnimation.destroy();
						m_pInterface->StopProgressInfo();
						return false;
					}
				}
			}
		}

		// Убираем ненужные кадры, которые не несут информационной нагрузки
		if( calexpoptOptimizeAnimations)
			RemoveFakeFrames( &coreAnimation);

		// stop the progress info
		m_pInterface->StopProgressInfo();

		if( calexpoptKEEPFRAME)
		{
			//
			coreAnimation.KeepFrame();
		}

		// save core animation to the file
		CalSaver saver;
		const std::string stdstrFileName(csFullPath.GetBuffer(csFullPath.GetLength()));
		if(!saver.saveCoreAnimation(stdstrFileName, &coreAnimation))
		{
			SetLastError(CalError::getLastErrorText(), __FILE__, __LINE__);
			coreAnimation.destroy();
			return false;
		}

		// destroy the core animation
		coreAnimation.destroy();
	}

	return true;
}

//----------------------------------------------------------------------------//
// Export the material to a given file                                        //
//----------------------------------------------------------------------------//

#ifdef MAX3DS_BUILD
bool CExporter::ExportMaterial(const std::string& strFilename, CalCoreMaterial* coreMaterial, StdMat* pMaxMtrl, BOOL abSaveMaps)
#else
bool CExporter::ExportMaterial(const std::string& strFilename, BOOL abSaveMaps)
#endif
{
	// check if a valid interface is set
	if(m_pInterface == 0)
	{
		SetLastError("Invalid handle.", __FILE__, __LINE__);
		return false;
	}

	// check if a valid interface is set
	if(m_pInterface == 0)
	{
		SetLastError("Invalid handle.", __FILE__, __LINE__);
		return false;
	}

	CMaterialCandidate	*pMaterialCandidate = NULL;

	// build a material library candidate
	CMaterialLibraryCandidate materialLibraryCandidate;

#ifdef MAX3DS_BUILD
	CMaxMaterial		*pMaxMaterial = NULL;
	if( pMaxMtrl == NULL)
	{
#endif
		if(!materialLibraryCandidate.CreateFromInterface()) return false;

		// show export wizard sheet
		CMaterialExportSheet sheet("Cal3D Material Export", m_pInterface->GetMainWnd());
		sheet.SetMaterialLibraryCandidate(&materialLibraryCandidate);
		sheet.SetWizardMode();
		if(sheet.DoModal() != ID_WIZFINISH) return true;

		// get selected material candidate
		pMaterialCandidate = materialLibraryCandidate.GetSelectedMaterialCandidate();
		if(pMaterialCandidate == 0)
		{
			SetLastError("No material selected.", __FILE__, __LINE__);
			return false;
		}
#ifdef MAX3DS_BUILD
	}else
	{
		pMaxMaterial = new CMaxMaterial();
		// create the max material
		if(pMaxMaterial->Create(pMaxMtrl))
		{
			pMaterialCandidate = new CMaterialCandidate();
			pMaterialCandidate->Create( pMaxMaterial);
		}
	}
#endif
	// create the core material instance
	bool insideInitCoreMaterial = false;
	if (coreMaterial == NULL)
	{
		coreMaterial = new CalCoreMaterial();
		insideInitCoreMaterial = true;
	}
	//CalCoreMaterial coreMaterial;

	if(!coreMaterial->create())
	{
		SetLastError("Creation of core material instance failed.", __FILE__, __LINE__);
		return false;
	}

	// set the ambient color
	CalCoreMaterial::Color coreColor;
	float color[4];


	bool	diffuseTextureExists = false;
	// get the map vector of the material candidate
	std::vector<CMaterialCandidate::Map>& vectorMap = pMaterialCandidate->GetVectorMap();
	if( vectorMap.size() != 0)
	{
		for( int itx=0; itx!=vectorMap.size(); itx++)
		{
			CMaterialCandidate::Map& map = vectorMap[itx];
			if( map.type == Cal::DIFFUSE_TEXTURE)
				diffuseTextureExists = true;
		}
	}
	
	pMaterialCandidate->GetAmbientColor(&color[0]);
	if( overAmbient && diffuseTextureExists)
	{
		coreColor.red = R_ARGB(dwAmbient);
		coreColor.green = G_ARGB(dwAmbient);
		coreColor.blue = B_ARGB(dwAmbient);
		coreColor.alpha = (unsigned char)(255.0f * color[3]);
	}
	else
	{
		coreColor.red = (unsigned char)(255.0f * color[0]);
		coreColor.green = (unsigned char)(255.0f * color[1]);
		coreColor.blue = (unsigned char)(255.0f * color[2]);
		coreColor.alpha = (unsigned char)(255.0f * color[3]);
	}
	coreMaterial->setAmbientColor(coreColor);

	// set the diffuse color
	pMaterialCandidate->GetDiffuseColor(&color[0]);
	if( overDiffuse && diffuseTextureExists)
	{
		coreColor.red = R_ARGB(dwDiffuse);
		coreColor.green = G_ARGB(dwDiffuse);
		coreColor.blue = B_ARGB(dwDiffuse);
		coreColor.alpha = (unsigned char)(255.0f * color[3]);
	}
	else
	{
		coreColor.red = (unsigned char)(255.0f * color[0]);
		coreColor.green = (unsigned char)(255.0f * color[1]);
		coreColor.blue = (unsigned char)(255.0f * color[2]);
		coreColor.alpha = (unsigned char)(255.0f * color[3]);
	}
	coreMaterial->setDiffuseColor(coreColor);

	// set the specular color
	pMaterialCandidate->GetSpecularColor(&color[0]);
	coreColor.red = (unsigned char)(255.0f * color[0]);
	coreColor.green = (unsigned char)(255.0f * color[1]);
	coreColor.blue = (unsigned char)(255.0f * color[2]);
	coreColor.alpha = (unsigned char)(255.0f * color[3]);
	coreMaterial->setSpecularColor(coreColor);

	// set the specular color
#ifdef MAX3DS_BUILD
	pMaterialCandidate->GetEmissiveColor(&color[0]);
	coreColor.red = (unsigned char)(255.0f * color[0]);
	coreColor.green = (unsigned char)(255.0f * color[1]);
	coreColor.blue = (unsigned char)(255.0f * color[2]);
	coreColor.alpha = (unsigned char)(255.0f * color[3]);
	coreMaterial->setEmissiveColor(coreColor);
#endif

	// set the shininess factor
	coreMaterial->setShininess(pMaterialCandidate->GetShininess());

	// reserve memory for all the material data
	if(!coreMaterial->reserve(vectorMap.size()))
	{
		SetLastError("Memory reservation for maps failed.", __FILE__, __LINE__);
		return false;
	}

	// load all maps
	std::sort(vectorMap.begin(), vectorMap.end());
	int mapId;
	for(mapId = 0; mapId < vectorMap.size(); mapId++)
	{
		CalCoreMaterial::Map map;
		// set map data
		Cal3DComString strTexFilename = Cal3DComString(vectorMap[mapId].strFilename.c_str());
		// Меняем формат, если был задан в параметрах экспорта
		//if (materialExportFormat != "")
		//{
			//strTexFilename = strTexFilename.Left(strTexFilename.ReverseFind('.')) + materialExportFormat.GetBuffer(0); 			
		//}
		map.strFilename= strTexFilename;
		map.type = vectorMap[mapId].type;

		// set map in the core material instance
		coreMaterial->setMap(mapId, map);
	}

	// save core mesh to the file
	// after optimizerer
	if (insideInitCoreMaterial)
	{
		CalSaver saver;
		if(!saver.saveCoreMaterialOld(strFilename, coreMaterial))
		{
			SetLastError(CalError::getLastErrorText(), __FILE__, __LINE__);
			coreMaterial->destroy();
			return false;
		}

		// destroy the core mesh
		coreMaterial->destroy();
		delete coreMaterial;
	}
	
	/*
	Cal3DComString str(strFilename.c_str());
	if( str.Find("79")!=-1)
	{
		int iii=0;
	}
	*/

#ifdef  MAX3DS_BUILD
	if( abSaveMaps && pMaxMtrl != NULL)	// save
	{
		CMaxMaterial	mm;
		mm.Create(pMaxMtrl);
		mm.SaveMaps();
	}
#endif

	return true;
}

//----------------------------------------------------------------------------//
// Export the mesh to a given file                                            //
//----------------------------------------------------------------------------//
void write_float(std::ofstream& file, LPSTR abuf, float val)
{
	char	buf[200];	ZeroMemory(&buf, 200);
	sprintf((LPSTR)&buf, "%s=%.2f\n", abuf, val);
	file.write((LPSTR)&buf, strlen((LPSTR)&buf));
}

bool CExporter::ExportMesh(std::string& strFilename)
{
	// check if a valid interface is set
	if(m_pInterface == 0)
	{
		SetLastError("Invalid handle.", __FILE__, __LINE__);
		return false;
	}

	// Divide lights and other object into different arrays
	std::vector<CBaseNode*>	sel_objects;
	std::vector<int>	sel_objects_ids;
	std::vector<CBaseNode*>	lights_objects;
	std::vector<CBaseNode*>	flares_objects;

	calexpoptUSELOCALCOORDS = FALSE;
	calexpoptEXPORTSHADOWDATA = FALSE;
	calexpoptNORMALIZEVERTS = FALSE;
	calexprotUseLeftAssigment2Right = FALSE;
	calexprotUseRigthAssigment2Left = FALSE;
	maxMaterialIndex = 0;
	calexpoptAutoReplaceSameMaterials = 0;
	calexpoptAutoReplaceSameTextures = 0;

	//////////////////////////////////////////////////////////////////////////

	// Divide objects into categories
	int i;
	for(i=0; i<theExporter.GetInterface()->GetSelectedNodeCount(); i++)
	{
		CBaseNode* m_pNode = theExporter.GetInterface()->GetSelectedNode(i);
		#ifdef MAX3DS_BUILD		
			if (m_pNode->GetINode()->IsGroupHead()) continue;
			Object *obj = m_pNode->GetINode()->EvalWorldState(0.0).obj;
			if( !obj)	continue;
			Class_ID	c1 = obj->ClassID();
			Class_ID	c2(OMNI_LIGHT_CLASS_ID, 0);
			if( c1 == c2)
				lights_objects.push_back(m_pNode);
			else
			{
				CString nodeName = CString(m_pNode->GetName().c_str());	
				if( nodeName.Find("#") > -1)
				{
					if(nodeName == "#flare")	flares_objects.push_back(m_pNode);
					// ?? other plugs objects
				}else	
				{
					sel_objects.push_back(m_pNode);
					sel_objects_ids.push_back(i);
				}
			}
		#else
			sel_objects.push_back(m_pNode);
			sel_objects_ids.push_back(i);
		#endif
	}
	if( sel_objects.size() == 1 && sel_objects[0]->GetName().find(STENSIL_NAME_SIGN) != std::string::npos)
		calexpoptEXPORTSHADOWDATA = TRUE;

	BOOL	multSel = sel_objects.size()<2?false:true;
	BOOL	m_bExportMaterials = TRUE;
	BOOL	m_bKeepConfig = FALSE;
	CString	txPath;
	CString	materialsPath;
	overAmbient = overDiffuse = FALSE;
	dwAmbient = dwDiffuse = 0x00FFFFFF;

	// Link objects
	BOOL	linkObject = FALSE;


	MapReferencesNames	mapReferencesNames;
	//CalVector		trans;
	//CalQuaternion	rtn;
	//D3DXMATRIX		rmd3d;
	//CString	linkObjName;

	// build a mesh candidate
	CMeshCandidate meshCandidate;
	// build a skeleton candidate
	CSkeletonCandidate skeletonCandidate;

	//FACEDESCARRAY	faceDesc;
	//CMpeg4FDP*		mpeg4Face = NULL;
	SECFileSystem	fs;
	calexportExportDir = fs.GetPath(strFilename.c_str());
	CString			fileName = fs.GetFileName(strFilename.c_str());
	std::vector<CString>	sceneConfigLines;
	int mtrlCount;

	// Materials map
	typedef	std::map<CString/*real material name*/, CString/*generated material name*/>	MaterialsMap;
	MaterialsMap	mapMaterialsNames;

	// show export wizard sheet
	CMeshExportSheet sheet("Cal3D Mesh Export", m_pInterface->GetMainWnd(), multSel);
	sheet.SetMultSelectMode(multSel);
	sheet.SetColorOverrides(&overAmbient, &overDiffuse, &dwAmbient, &dwDiffuse);
	//sheet.SetTxPath(&txPath);
	//sheet.SetMaterialExport(&m_bExportMaterials);
	sheet.SetCFGKeep(&m_bKeepConfig);
	sheet.SetSkeletonCandidate(&skeletonCandidate);
	sheet.SetMeshCandidate(&meshCandidate);
	//sheet.SetFaceDesc(&faceDesc);
	//sheet.SetMpeg4Face( &mpeg4Face);
	//meshCandidate.SetFaceDesc(&faceDesc);
	//meshCandidate.SetFDP( &mpeg4Face);
	sheet.SetWizardMode();
	if(sheet.DoModal() != ID_WIZFINISH) return true;

	m_bKeepConfig = calexpoptKeepConfig;
	m_bExportMaterials = calexpoptExportMaterials;

	if( !calexpoptUseTexturesPath)	
		txPath = "";
	else
		txPath = calexportTexturesPath;
	if( !calexpoptUseMaterialsPath)
		materialsPath = "";
	else
		materialsPath = calexportMaterialsPath;

	if( multSel)
	{
		StartLog( fileName+".log");
		calexpoptUSELOCALCOORDS = TRUE;
	}

	if( !multSel)
	{
		if( !CheckDirectories())	return false;
	}

	if( multSel)
		calexpoptUSELOCALCOORDS = true;

	for(i=0; i<sel_objects.size(); i++)
	{
		fs.ChangeDirectory(calexportExportDir);
		CBaseNode* m_pNode = sel_objects[i];
		CString nodeName = CString(m_pNode->GetName().c_str());
		// Определим свойства объекта, заданные в [...]
		CString nodeNameProperties = nodeName;
		int findOpenBracket = nodeName.Find("[");
		if (findOpenBracket != -1)
		{
			nodeName = nodeName.Left(findOpenBracket);
		}

		if( multSel)
		{
			WriteLog( "Exporting: "+nodeName+"...");
		}

		if(nodeName.GetLength()>0 && nodeName[0] == '&')
		{
			linkObject = TRUE;
		}
		else
		{
			linkObject = FALSE;
		}


#ifdef  MAX3DS_BUILD
		INode*	m_pMyNode = m_pNode->GetINode();
		INode*	m_pRefNode2 = m_pNode->GetINode()->GetActualINode();
		Object*	m_pRef = m_pNode->GetINode()->GetObjectRef();
#endif

		int materialThreadId = -1;
		std::vector<int> mtrlsId;
		CalCoreMesh coreMesh;

		CalVector		m_worldTranslation(0,0,0);
		CalQuaternion	m_worldRotation(0,0,0,1);
		if( calexpoptUSELOCALCOORDS)
		{
			m_pInterface->GetTranslationAndRotation(sel_objects[i], NULL, 0.0, m_worldTranslation, m_worldRotation);
		}

		if( !linkObject)
		{

			objectCfg.is_reference = false;
			if( multSel)
			{
				meshCandidate.Create(&skeletonCandidate, 999, 0.01f, sel_objects_ids[i]);
				meshCandidate.DisableLOD();
			}

			coreMesh.set_worldRotation(m_worldRotation);
			coreMesh.set_worldTranslation(m_worldTranslation);
			m_worldTranslation.invert();
			m_worldRotation.conjugate();

			// create the core mesh instance
			if(!coreMesh.create())
			{
				SetLastError("Creation of core mesh instance failed.", __FILE__, __LINE__);
				return false;
			}

			if( multSel || skeletonCandidate.IsNull())
				coreMesh.NoSkel( true);

			// get the submesh candidate vector
			std::vector<CSubmeshCandidate *>& vectorSubmeshCandidate = meshCandidate.GetVectorSubmeshCandidate();

			// start the progress info
			m_pInterface->StartProgressInfo("Exporting to mesh file...");

			int submeshCandidateId;
			for(submeshCandidateId = 0; submeshCandidateId < vectorSubmeshCandidate.size(); submeshCandidateId++)
			{
				// update the progress info
				m_pInterface->SetProgressInfo(100.0f * (float)submeshCandidateId / (float)vectorSubmeshCandidate.size());

				// get the submesh candidate
				CSubmeshCandidate *pSubmeshCandidate;
				pSubmeshCandidate = vectorSubmeshCandidate[submeshCandidateId];

				// get the face vector
				std::vector<CSubmeshCandidate::Face>& vectorFace = pSubmeshCandidate->GetVectorFace();

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

					// set the core material id
					//pCoreSubmesh->setCoreMaterialThreadId(pSubmeshCandidate->GetMaterialThreadId());
					pCoreSubmesh->setCoreMaterialThreadId(materialThreadId);
//#ifdef MAX7
//					mtrlsId.push_back(pSubmeshCandidate->GetMaterialThreadId()+1);
//#else
					mtrlsId.push_back(pSubmeshCandidate->GetMaterialThreadId());
//#endif

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

						// set the vertex normal
						CalVector normal;
						pVertexCandidate->GetNormal(vertex.normal);
						vertex.normal *= m_worldRotation;

						// set the collapse id
						vertex.collapseId = pVertexCandidate->GetCollapseId();

						// set the face collapse count
						vertex.faceCollapseCount = pVertexCandidate->GetFaceCollapseCount();

						// get the texture coordinate vector
						std::vector<CVertexCandidate::TextureCoordinate>& vectorTextureCoordinate = pVertexCandidate->GetVectorTextureCoordinate();

						// set all texture coordinates
						int textureCoordinateId;
						for(textureCoordinateId = 0; textureCoordinateId < vectorTextureCoordinate.size(); textureCoordinateId++)
						{
							CalCoreSubmesh::TxCoor textureCoordinate;
							textureCoordinate.u = vectorTextureCoordinate[textureCoordinateId].u;
							textureCoordinate.v = vectorTextureCoordinate[textureCoordinateId].v;

							// set texture coordinate
							pCoreSubmesh->setTextureCoordinate(pVertexCandidate->GetLodId(), textureCoordinateId, textureCoordinate);
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
					int vc = pCoreSubmesh->getVectorVertex().size();
				}
			}

			// stop the progress info
			m_pInterface->StopProgressInfo();
		}
		else
		{

			objectCfg.is_reference = true;
			Cal3DComString	referenceObjectName = nodeName.Right(nodeName.GetLength()-1);
			if (referenceObjectName.Find("[")>0)
				referenceObjectName = referenceObjectName.Left(referenceObjectName.Find("["));

#ifdef MAX3DS_BUILD
			CBaseNode* m_pLinkNode = GetInterface()->GetNode(referenceObjectName);
#else
			CBaseNode* m_pLinkNode = GetInterface()->GetNode(std::string(referenceObjectName));
#endif
			if( !m_pLinkNode)
			{
				Cal3DComString	error;
				error.Format("Reference node named \"%s\" not found.", referenceObjectName.GetBuffer());
				MessageBox( 0, error, "Error!", MB_OK);
				continue;
			}

			MapReferencesNames::iterator	it = mapReferencesNames.find(CString(referenceObjectName));
			if( it == mapReferencesNames.end())
				mapReferencesNames.insert( MapReferencesNames::value_type(CString(referenceObjectName), 1));
			else
				it->second++;

			objectCfg.ref_translation = m_worldTranslation;
			objectCfg.ref_rotation = m_worldRotation;
			objectCfg.ref_scale.set( 1, 1, 1);
			objectCfg.ref_name = referenceObjectName;

		}

		// Create needed directories
		if( multSel)	// Format path
		{
			CString sDirName = nodeName;

			switch(nodeName[0])
			{

#ifdef  MAX3DS_BUILD
				case '&':
				{
					sDirName = sDirName.Right(nodeName.GetLength() - 1);
#ifdef DEBUG
for( MapReferencesNames::iterator it1=mapReferencesNames.begin();
it1!=mapReferencesNames.end();it1++)
	ATLTRACE("map name: %s", (LPCTSTR)it1->first);
#endif
					//Cal3DComString	sNameToFind(sDirName.GetBuffer(0));
					MapReferencesNames::iterator	it = mapReferencesNames.find(sDirName);
					ATLASSERT( it != mapReferencesNames.end());
					if( it == mapReferencesNames.end())
					{
						WriteLog( "Error: no instance object found "+sDirName);
						continue;
					}
					CString	sDirNameCopy = sDirName;
					sDirName.Format("%s%i", sDirNameCopy.GetBuffer(0), it->second);
				}
				break;
#endif
				case '+':
				case '%':
					sceneConfigLines.push_back(nodeName);
					sDirName = sDirName.Right(nodeName.GetLength() - 1);
					break;
				default:
					sceneConfigLines.push_back(nodeName);
					break;
			}
			fs.MakeDirectory(calexportExportDir + "\\" + sDirName);
			fs.ChangeDirectory(calexportExportDir + "\\" + sDirName);
			if( !CheckDirectories())	return false;
		}

		CalSaver saver;
		if( !linkObject)
		{
			if( (!multSel && calexpoptEXPORTSHADOWDATA) || (multSel && nodeName.Find(STENSIL_NAME_SIGN) > -1))
				coreMesh.createShadowData();
			mtrlCount = coreMesh.getVectorCoreSubmesh().size();
			// save core mesh to the file

			std::string	meshFileName(fileName.GetBuffer(0));
			if(!saver.saveCoreMesh(meshFileName, &coreMesh))
			{
				SetLastError(CalError::getLastErrorText(), __FILE__, __LINE__);
				coreMesh.destroy();
				return false;
			}
		}


		std::string::size_type pos;
		std::string config;

		//get xml config name
		pos = fileName.ReverseFind('.');
		config = fileName.Mid(0, ++pos);
		config.append("xml");
		if( multSel || sheet.SaveConfig())		//saver.saveConfig(strFilename, &coreMesh);
		{

			CExportXMLFile configFile;
			// clear objectCfg structure

			CExporter::clearObjectCfg();
			objectCfg.collision_type = "AABB";
			objectCfg.scale=1.0f;

			if (m_bKeepConfig)
			try
			{
				if (configFile.Open(config.c_str(), CFile::modeRead, NULL))
				{
					//load config
					CExporter::parseXML(configFile, Cal3DComString(strFilename.c_str()));
					configFile.Close();
				}
			} catch(CFileExException e){}

			//get mesh_src
			Cal3DComString meshFileName = Cal3DComString(strFilename.c_str());
			if (meshFileName.ReverseFind('\\') != -1)
			{
				meshFileName = meshFileName.Right(meshFileName.GetLength()-meshFileName.ReverseFind('\\')-1);
			}
			meshFileName.MakeLower();

			objectCfg.mesh_srcs.push_back( meshFileName);

			//get skeleton_src
			Cal3DComString skelFileName = Cal3DComString(skeletonCandidate.m_strFilename.c_str());
			if (skelFileName.ReverseFind('\\') != -1)
			{
				skelFileName = skelFileName.Right(skelFileName.GetLength()-skelFileName.ReverseFind('\\')-1);
			}
			skelFileName.MakeLower();
			objectCfg.skeleton_src = skelFileName;

			// find first brackets
			int findFrom = 0;
			int findOpenBracket = nodeNameProperties.Find("[", findFrom);

			// get help property
			Cal3DComString helpProperty;
			if (findOpenBracket != -1)
			{
				helpProperty = nodeNameProperties.Left(findOpenBracket);
			}
			else
			{
				helpProperty = nodeNameProperties;
			}

			// if found
			while (findOpenBracket != -1)
			{
				// start parsing nodeName
				int findCloseBracket = nodeNameProperties.Find("]",findFrom);
				if (findCloseBracket == -1)
					break;					// error in nodeNameProperties
				Cal3DComString param = nodeNameProperties.Mid(findOpenBracket+1, findCloseBracket - findOpenBracket - 1);
				param.MakeLower();
				if (param.CompareNoCase("oob") == 0 || param.CompareNoCase("aabb") == 0)
				{
					param.MakeUpper();
					objectCfg.collision_type = param;
				}
				else if (param.Compare("int") == 0 || param.Compare("flip") == 0)
				{
					Cal3DComString propName;
					bool propFound = false;
					if (param.Compare("int") == 0)
						propName = "intangible";
					else if (param.Compare("flip") == 0)
						propName = "flip_faces";
					
					if (m_bKeepConfig)
					{
						// find property in objectCfg
						std::vector<_property>::iterator vci_p = objectCfg.properties.begin();
						for (; vci_p != objectCfg.properties.end(); vci_p++)
						{
							// if found
							propName.MakeLower();
							if (propName.Compare(vci_p->name) == 0)
							{
								vci_p->value = "true";
								propFound = true;
								break;
							}
						}
					}
					// if not found
					if (!propFound)
					{
						// create new property and push it into objectCfg
						_property prop;
						prop.name = propName;
						prop.value = "true";
						objectCfg.properties.push_back(prop);
					}
				}
				// find next brackets
				findFrom = findCloseBracket+1;
				findOpenBracket = nodeNameProperties.Find("[", findFrom);
			}
			// find other properties and fill them or make new
			bool helpFound = false;
			bool texturesFound = false;
			bool materialsFound = false;
			if (m_bKeepConfig)
			{
				std::vector<_property>::iterator vci_p = objectCfg.properties.begin();
				for (; vci_p != objectCfg.properties.end(); vci_p++)
				{
					// if found	help property
					if (vci_p->name.Compare("help") == 0)
					{
						vci_p->value = helpProperty;
						helpFound = true;
					}
					// if found textures property
					else if (vci_p->name.Compare("textures") == 0)
					{
						if (txPath.Compare("")!=0)
							vci_p->value = txPath;
						texturesFound = true;
					}
					else if (vci_p->name.Compare("materials") == 0)
					{
						if (txPath.Compare("")!=0)
							vci_p->value = txPath;
						materialsFound = true;
					}
				}
			}
			// if help property not found create new
			if (!helpFound)
			{
				_property prop;
				prop.name = "help";
				prop.value = helpProperty;
				objectCfg.properties.push_back(prop);
			}
			// if textures property not found create new
			if (!texturesFound && txPath.Compare("")!=0)
			{
				_property prop;
				prop.name = "textures";
				prop.value = txPath;
				objectCfg.properties.push_back(prop);
			}
			// if textures property not found create new
			if (!materialsFound && materialsPath.Compare("")!=0)
			{
				_property prop;
				prop.name = "materials";
				prop.value = materialsPath;
				objectCfg.properties.push_back(prop);
			}
			//save xml config
			//CExporter::saveXMLConfig(Cal3DComString(config.c_str()));
		}

		// destroy the core mesh
		if( !linkObject)
		{
			coreMesh.destroy();

			// Save all materials
			objectCfg.materials.clear();
#ifdef MAX3DS_BUILD
			for( int mids=0; m_bExportMaterials && mids<mtrlsId.size(); mids++)
			{
				CString	materialFileName;
				CString	materialFilePath;
				materialFileName.Format( "mtrl%d.crf", mids+1);
				StdMat*	material = meshCandidate.GetMtrl(mtrlsId[mids]);
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
					if( !linkObject)
					{
						_material	mtrl;
						mtrl.src = Cal3DComString(materialFileName.GetBuffer(0));
						objectCfg.materials.push_back(mtrl);
					}
					if( calexpoptUseMaterialsPath)
						materialFileName = calexportMaterialsPath + materialFileName;
					materialFilePath = fs.AppendPaths( fs.GetCurrentDirectory(), materialFileName);

					CalCoreMaterial* coreMaterial = new CalCoreMaterial();
					ExportMaterial( materialFilePath.GetBuffer(0), coreMaterial, material);

					if (coreMaterial != NULL)
					{
						CalSaver saver;
						if(!saver.saveCoreMaterialOld(materialFilePath.GetBuffer(0), coreMaterial))
						{
							SetLastError(CalError::getLastErrorText(), __FILE__, __LINE__);
							coreMaterial->destroy();							
						}
						coreMaterial->destroy();
						delete coreMaterial;
					}
				}
			}
#endif
		}
		if( multSel || sheet.SaveConfig())
		{
			if( objectCfg.is_reference)
				CExporter::saveRefXMLConfig(Cal3DComString(config.c_str()));
			else
				CExporter::saveXMLConfig(Cal3DComString(config.c_str()));
		}
	}

#ifdef MAX3DS_BUILD
	if( multSel)
		SaveXMLScene("scene.xml");

	if (lights_objects.size() > 0) 
	{

		// create lights array
		std::vector<CMaxLight*>	n3d_lights;
		for (int il=0; il<lights_objects.size(); il++)
		{
			CMaxLight*	light = new CMaxLight();
			light->Create(lights_objects[il]->GetINode());
			n3d_lights.push_back(light);
		}

		// show dialog
		CSceneLightsDlg	dlg;
		dlg.lights = &n3d_lights;
		dlg.ambient = m_pInterface->GetAmbient();
		dlg.DoModal();

		// save lights

		// drop lights
		for(int il=0; il<n3d_lights.size(); il++)
			delete n3d_lights[il];
		n3d_lights.clear();
	}

	if (flares_objects.size() > 0) 
	{

		// create lights array
		std::vector<CMaxFlare*>	n3d_flares;
		int il;
		for (il=0; il<flares_objects.size(); il++)
		{
			CMaxFlare*	flare = new CMaxFlare();
			flare->Create(flares_objects[il]->GetINode());
			n3d_flares.push_back(flare);
		}

		// show dialog
		CSceneFlaresDlg	dlg;
		dlg.flares = &n3d_flares;
		dlg.DoModal();

		// save flares
		fs.ChangeDirectory(calexportExportDir);
		fs.MakeDirectory("plugs");
		fs.ChangeDirectory(calexportExportDir + "\\plugs\\");

		sceneConfigLines.push_back(CString("flares=plugs\\flares.cfg"));

		std::ofstream file;
		file.open("flares.cfg", std::ios::out | std::ios::binary);
		if( file)
		{
			for(il=0; il<n3d_flares.size(); il++)
			{
				CString	path(n3d_flares[il]->flare.fname.GetBuffer());
				path = fs.GetRelPath(calexportExportDir+"plugs\\", path);
				// texture name
				file.write("flare=single\n", strlen("flare=single\n"));
				file.write("flare_t=", strlen("flare_t="));
				file.write(path.GetBuffer(0), path.GetLength());	file.write("\n", 1);
				// distance
				write_float(file, "flare_max_dist", n3d_flares[il]->flare.dist);
				// size
				write_float(file, "flare_max_size", n3d_flares[il]->flare.size);
				// position
				CalVector pos=n3d_flares[il]->GetPosition();
				write_float(file, "posx", pos.x);
				write_float(file, "posy", pos.y);
				write_float(file, "posz", pos.z);
				
			}
			file.close();
		}


		// drop flares
		for(il=0; il<n3d_flares.size(); il++)
			delete n3d_flares[il];
		n3d_flares.clear();
	}
#endif

#ifdef MAX3DS_BUILD
	// add references to scenes config
	MapReferencesNames::iterator	refit = mapReferencesNames.begin(),
									refend = mapReferencesNames.end();
	for (; refit!=refend; ++refit)
	{
		int count = refit->second;
		for (int i=0; i<count; i++)
		{
			CString	sRefName;
			sRefName.Format("+%s%i", refit->first, (i+1));
			sceneConfigLines.push_back(sRefName);
		}
	}

	if( !sceneConfigLines.empty())
	{
		CString fn(strFilename.c_str());
		fn = fs.GetBaseFileName(fn);
		fs.ChangeDirectory(calexportExportDir);
		SaveSceneConfig( fn, sceneConfigLines);
	}
#endif

	if ( CExporter::calexprotIsUseSide2SideEffect)
	{
		int d = 0;		
		AfxMessageBox("Веса точек изменились и для подтверждения данных изменений необходимо сохранить сцену!", MB_OK | MB_ICONEXCLAMATION);
		CExporter::calexprotIsUseSide2SideEffect = FALSE;
	}

	return true;
}

//----------------------------------------------------------------------------//
// Export the skeleton to a given file                                        //
//----------------------------------------------------------------------------//

bool CExporter::ExportSkeleton(const std::string& strFilename)
{
	// check if a valid interface is set
	if(m_pInterface == 0)
	{
		SetLastError("Invalid handle.", __FILE__, __LINE__);
		return false;
	}

	// build a skeleton candidate
	CSkeletonCandidate skeletonCandidate;
	if(!skeletonCandidate.CreateFromInterface()) return false;

	// show export wizard sheet
	CSkeletonExportSheet sheet("Cal3D Skeleton Export", m_pInterface->GetMainWnd());
	sheet.SetSkeletonCandidate(&skeletonCandidate);
	sheet.SetWizardMode();
	if(sheet.DoModal() != ID_WIZFINISH) return true;

	// build the selected ids of the bone candidates
	int selectedCount;
	selectedCount = skeletonCandidate.BuildSelectedId();
	if(selectedCount == 0)
	{
		SetLastError("No bones selected to export.", __FILE__, __LINE__);
		return false;
	}

	// create the core skeleton instance
	CalCoreSkeleton coreSkeleton;
	if(!coreSkeleton.create())
	{
		SetLastError("Creation of core skeleton instance failed.", __FILE__, __LINE__);
		return false;
	}

	// get bone candidate vector
	std::vector<CBoneCandidate *>& vectorBoneCandidate = skeletonCandidate.GetVectorBoneCandidate();

	// start the progress info
	m_pInterface->StartProgressInfo("Exporting to skeleton file...");

	int boneCandidateId;
	int selectedId;
	for(boneCandidateId = 0, selectedId = 0; boneCandidateId < vectorBoneCandidate.size(); boneCandidateId++)
	{
		// get the bone candidate
		CBoneCandidate *pBoneCandidate;
		pBoneCandidate = vectorBoneCandidate[boneCandidateId];

		// only export selected bone candidates
		if(pBoneCandidate->IsSelected())
		{
			// update the progress info
			m_pInterface->SetProgressInfo(100.0f * (selectedId + 1) / selectedCount);
			selectedId++;

			// allocate new core bone instance
			CalCoreBone *pCoreBone;
			pCoreBone = new CalCoreBone();
			if(pCoreBone == 0)
			{
				SetLastError("Memory allocation failed.", __FILE__, __LINE__);
				coreSkeleton.destroy();
				m_pInterface->StopProgressInfo();
				return false;
			}

			// create the core bone instance
			if(!pCoreBone->create(Cal3DComString(pBoneCandidate->GetNode()->GetName().c_str())))
			{
				SetLastError(CalError::getLastErrorText(), __FILE__, __LINE__);
				delete pCoreBone;
				coreSkeleton.destroy();
				m_pInterface->StopProgressInfo();
				return false;
			}

			// get the parent id of the bone candidate
			int parentId;
			parentId = skeletonCandidate.GetParentSelectedId(boneCandidateId);

			// set the parentId
			pCoreBone->setParentId(parentId);

			// get the translation and the rotation of the bone candidate
			CalVector translation;
			CalQuaternion rotation;
			skeletonCandidate.GetTranslationAndRotation(boneCandidateId, 0.0f, translation, rotation);

			// set the translation and rotation
			pCoreBone->setTranslation(translation);
			pCoreBone->setRotation(rotation);

			// get the bone space translation and the rotation of the bone candidate
			CalVector translationBoneSpace;
			CalQuaternion rotationBoneSpace;
			skeletonCandidate.GetTranslationAndRotationBoneSpace(boneCandidateId, 0.0f, translationBoneSpace, rotationBoneSpace);

			// set the bone space translation and rotation
			pCoreBone->setTranslationBoneSpace(translationBoneSpace);
			pCoreBone->setRotationBoneSpace(rotationBoneSpace);

			// set the core skeleton of the core bone instance
			pCoreBone->setCoreSkeleton(&coreSkeleton);

			// add the core bone to the core skeleton instance
			int boneId;
			boneId = coreSkeleton.addCoreBone(pCoreBone);

			// adjust child list of parent bone
			if(parentId != -1)
			{
				// get parent core bone
				CalCoreBone *pParentCoreBone;
				pParentCoreBone = coreSkeleton.getCoreBone(parentId);
				if(pParentCoreBone == 0)
				{
					SetLastError(CalError::getLastErrorText(), __FILE__, __LINE__);
					delete pCoreBone;
					coreSkeleton.destroy();
					m_pInterface->StopProgressInfo();
					return false;
				}

				// add this core bone to the child list of the parent bone
				pParentCoreBone->addChildId(boneId);
			}
		}
	}

	// stop the progress info
	m_pInterface->StopProgressInfo();

	// save core skeleton to the file
	CalSaver saver;
	if(!saver.saveCoreSkeleton(strFilename, &coreSkeleton))
	{
		SetLastError(CalError::getLastErrorText(), __FILE__, __LINE__);
		coreSkeleton.destroy();
		return false;
	}

	// destroy core skeleton instance
	coreSkeleton.destroy();

	return true;
}

//----------------------------------------------------------------------------//
// Get the last error message                                                 //
//----------------------------------------------------------------------------//

CBaseInterface *CExporter::GetInterface()
{
	return m_pInterface;
}

//----------------------------------------------------------------------------//
// Get the last error message                                                 //
//----------------------------------------------------------------------------//

const std::string& CExporter::GetELastError()
{
	return m_strLastError;
}

//----------------------------------------------------------------------------//
// Set the last error message                                                 //
//----------------------------------------------------------------------------//

void CExporter::SetLastError(const std::string& strText, const std::string& strFilename, int line)
{
	std::stringstream strstrError;
	strstrError << strText << "\n(" << strFilename << " " << line << ")" << std::ends;

	m_strLastError = strstrError.str();
}

//----------------------------------------------------------------------------//
// Set the last error message from the cal3d library                          //
//----------------------------------------------------------------------------//

void CExporter::SetLastErrorFromCal(const std::string& strFilename, int line)
{
	std::stringstream strstrError;
	strstrError << "cal3d : " << CalError::getLastErrorDescription();

	if(!CalError::getLastErrorText().empty())
	{
		strstrError << " '" << CalError::getLastErrorText() << "'";
	}

	strstrError << " in " << CalError::getLastErrorFile()
				<< "(" << CalError::getLastErrorLine() << ")";

	strstrError << "\n(" << strFilename << " " << line << ")" << std::ends;

	m_strLastError = strstrError.str();
}

//----------------------------------------------------------------------------//

void CExporter::SaveSceneConfig(CString fname, std::vector<CString>& names)
{
	// 0 - override
	// 1 - append missing

	int mode=1;

	std::vector<Cal3DComString>	types;
	std::vector<Cal3DComString>	paths;

	CFile	file;
	try
	{
		if( file.Open(fname+".scn", CFile::modeRead))
		{
			file.Close();
			CFileEx	fe;
			try{
				if (fe.Open(Cal3DComString(fname.GetBuffer(0))+Cal3DComString(".scn"))) {
					load_config(fe, types, paths);
					fe.Close();
				}
			}catch(CFileExException e){}
			if( MessageBox(NULL, "Override scene file?", "Question!", MB_ICONWARNING | MB_YESNO) == IDYES)
				mode = 0;
		}else
			mode = 0;

	}catch(CFileException fex)
	{
	}

	file.Open(fname+".scn", CFile::modeCreate | CFile::modeWrite);
	if( mode == 1)
	{
		for( int i=0; i<types.size(); i++)
		{
			file.Write( types[i].GetBuffer(0), types[i].GetLength());
			file.Write( "=", 1);
			file.Write( paths[i].GetBuffer(0), paths[i].GetLength());
			file.Write( "\n", 1);
		}
	}

	for( int i=0; i<names.size(); i++)
	{
		CString oname = names[i];
		CString fline;
		switch(oname[0])
		{
			case '+':
			case '&':
				fline = oname.Right(oname.GetLength() - 1);
				fline = "object3d=" + fline + "\\" + fname + ".xml";
				break;
			case '%':
				fline = oname.Right(oname.GetLength() - 1);
				fline = "BSP=" + fline + "\\" + fname + ".xml";
				break;
			default:
				fline = "object3d=" + oname + "\\" + fname + ".xml";
				break;
		}
		fline += '\n';

		bool	writeit=true;
		if( mode == 1)
		{
			for( int i=0; i<types.size(); i++)
			{
				CString temp="";
				temp += CString(types[i].GetBuffer());
				temp += "=";
				temp += CString(paths[i].GetBuffer());
				temp += "\n";
				temp.MakeLower();
				fline.MakeLower();
				if( temp == fline)
				{
					//names.erase(names.begin()+i);
					//types.erase(types.begin()+i);
					//i--;
					writeit = false;
					break;
				}
			}

		}

		if( writeit)
			file.Write( fline.GetBuffer(0), fline.GetLength());
	}
	file.Close();
}

void CExporter::SaveMeshConfig(const std::string& strFilename, int smc, bool skel, CString txPath, CString txParent, CalVector* tr, CalQuaternion* rtn, BOOL m_bKeepConfig)
{
//  DWORD	type = 0;
//  bool cooling=false;
	SECFileSystem	fs;
	Cal3DComString dir = fs.GetCurrentDirectory();;
	if(dir[dir.GetLength()-1] == '\\')	dir = dir.Left(dir.GetLength()-1);
	dir = dir.Right(dir.GetLength() - dir.ReverseFind('\\') - 1);
	int length = dir.GetLength();
	while(length>0 && !((dir[0]>='A' && dir[0]<='Z') || (dir[0]>='a' && dir[0]<='z')))
	{	dir = dir.Right(dir.GetLength()-1);	length--;}
	while(length>0 && !((dir[length-1]>='A' && dir[length-1]<='Z') || (dir[length-1]>='a' && dir[length-1]<='z')))
	{	dir = dir.Left(dir.GetLength()-1);	length--;}

  std::vector<Cal3DComString>	appendix;

  // получим имя файла конфигурации из имени файла модели
  std::string config;

  std::string::size_type pos;
  pos = strFilename.find_last_of(".");

  config = strFilename.substr(0, ++pos);
  config.append("cfg");

  if (m_bKeepConfig) {
	std::vector<Cal3DComString>	keys;
	std::vector<Cal3DComString>	vals;
	CFileEx	fe;
	try{
		if (fe.Open(Cal3DComString(config.c_str()))) {
			load_config(fe, keys, vals);
			for (int i=0; i<keys.size(); i++) {
				Cal3DComString	strKey = keys[i];
				if( strKey != "mesh" && strKey != "skeleton" && \
					strKey != "material" && strKey != "mpegFAPU" && \
					strKey != "scale" && strKey != "help" && strKey != "textures")
				{
					Cal3DComString	temp = strKey;
					temp += "=";
					temp += vals[i];
					appendix.push_back(temp);
				}
			}
			fe.Close();
		}
	}catch(CFileExException e){}
	
  }

  // open the file
  std::ofstream file;
  file.open(config.c_str(), std::ios::out | std::ios::binary);
  if(!file)
  {
    CalError::setLastError(CalError::FILE_CREATION_FAILED, __FILE__, __LINE__, strFilename);
    return ;
  }

//  file.write("<?xml version=\"1.0\" encoding=\"UTF-8\"?>");
  // Сохраним данные о конфигурации модели //
  file.write( "scale=1.0\n\n", 11);				// масштаб модели

  if( txParent == "")
  {
	  if( skel)
		file.write( "skeleton=skel.csf\n\n", 19);	// если есть скелет, то пропишем его
	  file.write( "mesh=mesh.cmf\n\n", 15);			// сама модель
	  // get the submesh vector
	  for( int i=1; i <= smc; i++){										// материалы модели
		CString mn;
		mn.Format("material=mtrl%d.crf", i);
		file.write( mn.GetBuffer(0), mn.GetLength());
		file.write( "\n\n", 2);
	  }
	  ///////////////////////////////////////////
	  if( txPath != "")
	  {
		txPath = "textures="+txPath;
		file.write( txPath.GetBuffer(0), txPath.GetLength());
		file.write( "\n\n", 2);
	  }
  }else
  {
	  switch(txParent[0])
	  {
		case '+':
		case '%':
			txParent = txParent.Right(txParent.GetLength() - 1);
			break;
	  }
	  txPath = "link="+txParent;
	  file.write( txPath.GetBuffer(0), txPath.GetLength());	file.write( "\n", 1);

	  if( tr && rtn)
	  {
		  //file.write( (char*)tr, sizeof(CalVector));
		  //file.write( (char*)rtn, sizeof(CalQuaternion));
		  file.write( (char*)rtn, sizeof(D3DXMATRIX));
	  }
  }
  if (dir != "") 
  {	file.write( "help=", 5);	file.write( dir.GetBuffer(), dir.GetLength());	file.write( "\n\n", 2);}

  /*if (type & VB_WALKABLE) file.write( "walkable=1\n\n", 12);
  if (type & VB_INTANGIBLE) file.write( "intangible=1\n\n", 14);
  if (type & VB_UNPICKABLE) file.write( "unpickable=1\n\n", 14);
  if (cooling) file.write( "flip_faces=1\n\n", 14);*/
  for (int ia=0; ia<appendix.size(); ia++) {
	  file.write( appendix[ia].GetBuffer(), appendix[ia].GetLength());
	  file.write( "\n\n", 2);
  }

  appendix.clear();
 
  // explicitly close the file
  file.close();
}

void CExporter::StartLog(CString fname)
{
	CFile	file;
	if( file.Open( fname, CFile::modeCreate))
	{
		logFileName = fname;
		file.Close();
	}
	else
		logFileName = "";
}

void CExporter::WriteLog(CString info)
{
	CFile	file;
	if( file.Open( logFileName, CFile::modeWrite))
	{
		file.SeekToEnd();
		file.Write( info.GetBuffer(0), info.GetLength());
		file.Write("\n", 1);
		file.Close();
	}
}


/************************************************************************/
/* XLM Scene saving
/************************************************************************/
typedef struct _SELF_OBJECT
{
	Cal3DComString		name;
	enum so_type {SOT_SIMPLE, SOT_GROUP};
	_SELF_OBJECT*	parent;
} SELF_OBJECT;

#ifdef MAX3DS_BUILD
typedef struct _MaxXMLScene
{
	std::vector<SELF_OBJECT*>	groups;
	std::vector<SELF_OBJECT*>	objects;

	void	add_object(CBaseNode*	node)
	{
		INode*	pn = node->GetINode();

	}
} MaxXMLScene;


bool CExporter::SaveXMLScene(const Cal3DComString &fileName)
{
  // open the file
  std::ofstream file;
  file.open(fileName.GetString(), std::ios::out | std::ios::binary);
  if(!file)
  {
    //CalError::setLastError(CalError::FILE_CREATION_FAILED, __FILE__, __LINE__, );
    return false;
  }

	int counter=0;
	for(int i=0; i<theExporter.GetInterface()->GetSelectedNodeCount(); i++)
	{
		CBaseNode* m_pNode = theExporter.GetInterface()->GetSelectedNode(i);

		if (m_pNode->GetINode()->IsGroupHead() && m_pNode->GetINode()->GetParentNode()->GetParentNode()==NULL) {
			SaveNode(file, m_pNode, 0, "mesh");
		}
	}
	file.close();
	return true;
}

void CExporter::SaveNode(std::ofstream& file, CBaseNode *node, int ilevel, Cal3DComString name)
{
	if (node->GetINode()->IsGroupHead()) {
		Cal3DComString	str;
		str.Format("<group _name=\"%s\">\n", node->GetName().c_str());
		{for(int ibl=0;ibl<ilevel;ibl++) file.write( "\t", 1);}
		file.write( str.GetBuffer(), str.GetLength());
		for (int i=0; i<node->GetChildCount(); i++)
		{
			SaveNode(file, node->GetChild(i), ilevel+1, name);
		}
		str = "</group>\n";
		{for(int ibl=0;ibl<ilevel;ibl++) file.write( "\t", 1);}file.write( str.GetBuffer(), str.GetLength());
		return;
	}
	Cal3DComString	str(node->GetName().c_str());
	Cal3DComString fline;
	switch(str[0])
	{
		case '+':
		case '&':
			fline = str.Right(str.GetLength() - 1);
			fline += "\\" + name + ".xml";
			break;
		case '%':
			fline = str.Right(str.GetLength() - 1);
			fline += "\\" + name + ".xml";
			break;
	}
	{for(int ibl=0;ibl<ilevel;ibl++) file.write( "\t", 1);}
	str.Format("<object src=\"%s\">", fline.GetBuffer(0));	file.write( str.GetBuffer(), str.GetLength());
	str = "</object>\n";	file.write( str.GetBuffer(), str.GetLength());
}
#endif

void CExporter::load_config(CFileEx& file, std::vector<Cal3DComString>& types, std::vector<Cal3DComString>& names)
{
	int	count = 0;
	char*	data;
	DWORD	size;

	if( file.get_file_data( (LPVOID*)&data, &size) == NULL)	return;
	//
	DWORD	curPos = 0;
	Cal3DComString curline;

	while( true){
		if( curPos >= size)	break;
		curline = "";
		while( data[ curPos] == 10) curPos++;
		while( curPos < size && data[ curPos] != 10){
			if( data[ curPos] != 13)
				curline += data[ curPos];
			curPos++;
		}
		std::string strBuffer;
		strBuffer.append( curline.GetBuffer(0));

		std::string::size_type pos;
		pos = strBuffer.find_first_not_of(" \t");
		if((pos == std::string::npos) || (strBuffer[pos] == '\n') || (strBuffer[pos] == '\r') || (strBuffer[pos] == 0)) continue;
		if(strBuffer[pos] == '#') continue;
		std::string strKey;
		strKey = strBuffer.substr(pos, strBuffer.find_first_of(" =\t\n\r", pos) - pos);
		pos += strKey.size();
		pos = strBuffer.find_first_not_of(" \t", pos);
		if((pos == std::string::npos) || (strBuffer[pos] != '='))	break;
		pos = strBuffer.find_first_not_of(" \t", pos + 1);
		std::string strData;
		strData = strBuffer.substr(pos, strBuffer.find_first_of("\n\r", pos) - pos);

		Cal3DComString	str(strKey.c_str());	str.MakeLower();
		types.push_back(str);
		str = Cal3DComString(strData.c_str());	str.MakeLower();
		names.push_back(str);
	}
}

bool CExporter::CreateCoreTrack(CBoneCandidate *pBoneCandidate, int boneCandidateId, bool bCommon, CalCoreAnimation* coreAnimation)
{
	if(Cal::CURRENT_FILE_VERSION>=1003 && boneCandidateId == 0)
		if( !CreateCoreTrack(NULL, Cal::PROJECTION_BONE_ID, bCommon, coreAnimation))	return false;

	CalCoreTrack *pCoreTrack;
	pCoreTrack = new CalCoreTrack();
	if(pCoreTrack == 0)
	{
		SetLastError("Memory allocation failed.", __FILE__, __LINE__);
		return NULL;
	}

	// create the core track instance
	if(!pCoreTrack->create())
	{
		SetLastError(CalError::getLastErrorText(), __FILE__, __LINE__);
		delete pCoreTrack;
		return false;
	}

	// set the core bone id
	pCoreTrack->setCoreBoneId(boneCandidateId);

	// set the core bone name
	if( bCommon)
	{
		CString	nm;
		if( pBoneCandidate)
			nm = pBoneCandidate->GetCommonName();
		else
			nm = "frame";
		pCoreTrack->setCoreBoneName(nm.GetBuffer(0));
	}

	// add the core track to the core animation instance
	if(!coreAnimation->addCoreTrack(pCoreTrack))
	{
		SetLastError(CalError::getLastErrorText(), __FILE__, __LINE__);
		delete pCoreTrack;
		return false;
	}

	return true;
}

CalQuaternion get_planeQ(CalQuaternion rotation)
{
	rotation.Normalize();
	CalVector xaxe(1,0,0);
	CalVector xtr(1,0,0);
	xtr *= rotation;
	xtr.z = 0;
	xtr.normalize();
	CalQuaternion	planeQ;
	if( xtr.dot(xaxe) > 0.99)
	{
		planeQ = CalQuaternion(0,0,0,1);
	}
	else	if( xtr.dot(xaxe) < -0.99)
	{
		planeQ = CalQuaternion(CalVector(0,0,1),3.141692f);
	}
	else	
		planeQ = CalQuaternion(xaxe.getNormal(xtr), -xaxe.getAngle(xtr));
	xaxe *= planeQ;
	return planeQ;
}

bool CExporter::AddCoreKeyframe(int boneCandidateId, float settime, float time, CSkeletonCandidate* skeletonCandidate, CalCoreAnimation* coreAnimation)
{
	std::vector<CBoneCandidate *>& vectorBoneCandidate = skeletonCandidate->GetVectorBoneCandidate();
	CBoneCandidate *	pRoot = vectorBoneCandidate[0];
	BOOL	character = FALSE;
	if( pRoot->GetName().find("Bip")!=std::string::npos || pRoot->GetName().find("bip")!=std::string::npos)
		character = TRUE;

	if(Cal::CURRENT_FILE_VERSION>=1003 && boneCandidateId == 0)
		if( !AddCoreKeyframe(Cal::PROJECTION_BONE_ID, settime, time, skeletonCandidate, coreAnimation))	return false;

	// allocate new core keyframe instance
	CalCoreKeyframe *pCoreKeyframe;
	pCoreKeyframe = new CalCoreKeyframe();
	if(pCoreKeyframe == 0)
	{
		SetLastError("Memory allocation failed.", __FILE__, __LINE__);
		return false;
	}

	// create the core keyframe instance
	if(!pCoreKeyframe->create())
	{
		SetLastError(CalError::getLastErrorText(), __FILE__, __LINE__);
		delete pCoreKeyframe;
		return false;
	}

	// set the time
	pCoreKeyframe->setTime(settime);

	// get the translation and the rotation of the bone candidate
	CalVector translation;
	CalQuaternion rotation;
	if(boneCandidateId == Cal::PROJECTION_BONE_ID)
		skeletonCandidate->GetTranslationAndRotation(0, time, translation, rotation);
	else
		skeletonCandidate->GetTranslationAndRotation(boneCandidateId, time, translation, rotation);

	//
	if(Cal::CURRENT_FILE_VERSION>=1003)
	{

		if( boneCandidateId == 0)
		{
			translation.x = 0; translation.y = 0;
			if( character)
			{
				// calculate Bip bone rotation
				CalQuaternion planeQ = get_planeQ(rotation);

				// если не нужно переносить поворот анимации из cal3d в движок
				if( calexpoptKEEPROTATIONSINANIMATION)
				{
					planeQ = keepFrameRotation;
				}
				planeQ.conjugate();
				rotation *= planeQ;

				translation.z -= calexportFLOORZPOS;
			}else
			{
				rotation = CalQuaternion(CalVector(0,0,1),0);
				translation.z = 0;
//				translation.set(0,0,0);
			}

		}
		else	if( boneCandidateId == Cal::PROJECTION_BONE_ID)
		{
			if( character)
			{
				// calculate plane rotation
				translation.z = calexportFLOORZPOS;

				rotation = get_planeQ(rotation);

				// если не нужно переносить поворот анимации из cal3d в движок
				if( calexpoptKEEPROTATIONSINANIMATION)
				{
					if( !keepFrameRotationSet)
					{
						keepFrameRotationSet = true;
						keepFrameRotation = rotation;
			}
					else
					{
						rotation = keepFrameRotation;
		}
	}
			}
		}
	}

	// set the translation and rotation
	pCoreKeyframe->setTranslation(translation);
	pCoreKeyframe->setRotation(rotation);

	// get the core track for this bone candidate
	CalCoreTrack *pCoreTrack;
	/*if(Cal::CURRENT_FILE_VERSION>=1003)	pCoreTrack = coreAnimation->getCoreTrack(pBoneCandidate->GetId());
	else	*/
	pCoreTrack = coreAnimation->getCoreTrack(boneCandidateId);
	if(pCoreTrack == 0)
	{
		SetLastError(CalError::getLastErrorText(), __FILE__, __LINE__);
		delete pCoreKeyframe;
		return false;
	}

	// add this core keyframe to the core track
	pCoreTrack->addCoreKeyframe(pCoreKeyframe);
	return true;
}

void CExporter::RemoveFakeFrames( CalCoreAnimation* apCoreAnimation)
{
	assert( apCoreAnimation);
	if( !apCoreAnimation)	return;
	apCoreAnimation->RemoveFakeFrames();
	/*
	// 1. Убираем одинаковые кадры. Если перемещение и поворот на кадрах n..m постоянны, то
	// можно удалить все кадры в диапазоне n+1..m+1
	std::list<CalCoreTrack *>&	tracks = apCoreAnimation->getListCoreTrack();
	std::list<CalCoreTrack *>::iterator	it = tracks.begin(),
										end = tracks.end();
	for(; it!=end; ++it)
	{
		CalCoreTrack*	track = *it;
		std::map<float, CalCoreKeyframe *>& keyframes = track->getMapCoreKeyframe();
		int keyframesSize = keyframes.size();
		std::map<float, CalCoreKeyframe *>::iterator	kit = keyframes.begin(),
														kend = keyframes.end();//,
														kbend = keyframes.end();
		kbend--;

		CalVector		translation, _translation;
		CalQuaternion	rotation, _rotation;
		std::map<float, CalCoreKeyframe *>::iterator	startConst = keyframes.end();
		for(; kit!=kend; kit++)
		{
			float time = kit->first;
			CalCoreKeyframe*	keyFrame = kit->second;
			int ii=0;
		}
			if( startConst == keyframes.end())
			{
				translation = keyFrame->getTranslation();
				rotation = keyFrame->getRotation();
				startConst = kit;
			}
			else
			{
				_translation = keyFrame->getTranslation();
				_rotation = keyFrame->getRotation();
				if( _translation != translation ||
					rotation.x != rotation.x ||
					rotation.y != rotation.y ||
					rotation.z != rotation.z ||
					rotation.w != rotation.w ||
					kit == kbend)
				{
					startConst++;
					while( startConst != kit)
					{
						startConst = keyframes.erase(startConst);
					}
					startConst = keyframes.end();
				}
			}
		}
	}*/
	
}