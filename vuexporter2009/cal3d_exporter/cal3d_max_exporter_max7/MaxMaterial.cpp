//----------------------------------------------------------------------------//
// MaxMaterial.cpp                                                            //
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
#include "MaxMaterial.h"
#include "BaseInterface.h"
#include "FileSys.h"
#include "resexistsdlg.h"

#include <process.h>
#include "FreeImagePlus.h"
//----------------------------------------------------------------------------//
// Debug                                                                      //
//----------------------------------------------------------------------------//

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//----------------------------------------------------------------------------//
// Constructors                                                               //
//----------------------------------------------------------------------------//

CMaxMaterial::CMaxMaterial()
{
	m_pIStdMat = 0;
}

//----------------------------------------------------------------------------//
// Destructor                                                                 //
//----------------------------------------------------------------------------//

CMaxMaterial::~CMaxMaterial()
{
}

//----------------------------------------------------------------------------//
// Create a max material instance                                             //
//----------------------------------------------------------------------------//

bool CMaxMaterial::Create(StdMat *pIStdMat, CalCoreMaterial* coreMaterial)
{
	// check if the internal node is valid
	if(pIStdMat == 0)
	{
		theExporter.SetLastError("Invalid handle.", __FILE__, __LINE__);
		return false;
	}

	/*CString sName = pIStdMat->GetName();
	if( calexpoptUseBakedIfExists)
	{
		Mtl*	mtlBaked = NULL;
		if( pIStdMat->ClassID() == Class_ID(BAKE_SHELL_CLASS_ID,0))
		{
			mtlBaked = pIStdMat;
		}
		else
		{
			//pIStdMat->
		}
		if( mtlBaked)
		{
			ATLASSERT( mtlBaked->NumSubMtls() == 2);
			if( mtlBaked->NumSubMtls() == 2)
			{
				Mtl* bakedMaterial = mtlBaked->GetSubMtl(1);	// Get baked material
				ATLASSERT( bakedMaterial->ClassID() == Class_ID(DMTL_CLASS_ID, 0));
				if( bakedMaterial->ClassID() == Class_ID(DMTL_CLASS_ID, 0))
				{
					StdMat*	stdBakedMaterial = (StdMat*)bakedMaterial->GetSubMtl(1);	// Get baked material
					ATLASSERT( stdBakedMaterial);
					if( stdBakedMaterial)
					{
						pIStdMat = stdBakedMaterial;
					}
				}
			}
		}
	}*/

	m_pIStdMat = pIStdMat;

	m_coreMaterial = coreMaterial;

	return true;
}

//----------------------------------------------------------------------------//
// Get the ambient color of the material                                      //
//----------------------------------------------------------------------------//

void CMaxMaterial::GetAmbientColor(float *pColor)
{
	// get color from the internal standard material
	Color color;
	color = m_pIStdMat->GetAmbient(0);

	pColor[0] = color.r;
	pColor[1] = color.g;
	pColor[2] = color.b;
	pColor[3] = 0.0f;
}

//----------------------------------------------------------------------------//
// Get the diffuse color of the material                                      //
//----------------------------------------------------------------------------//

void CMaxMaterial::GetDiffuseColor(float *pColor)
{
	// get color from the internal standard material
	Color color;
	color = m_pIStdMat->GetDiffuse(0);

	pColor[0] = color.r;
	pColor[1] = color.g;
	pColor[2] = color.b;
	pColor[3] = 1.0 - m_pIStdMat->GetOpacity(0);//0.0f;
}

//----------------------------------------------------------------------------//
// Get the self illumination color of the material                                      //
//----------------------------------------------------------------------------//

void CMaxMaterial::GetEmissiveColor(float *pColor)
{
	// get color from the internal standard material
	Color color;
	color = m_pIStdMat->GetSelfIllumColor(0);

	pColor[0] = color.r;
	pColor[1] = color.g;
	pColor[2] = color.b;
	pColor[3] = 1.0 - m_pIStdMat->GetOpacity(0);//0.0f;
}

//----------------------------------------------------------------------------//
// Get the number of maps of the material                                     //
//----------------------------------------------------------------------------//

int CMaxMaterial::GetMapCount()
{
	int mapCount;
	mapCount = 0;

	// loop through all maps of the material
	int mapId;
	for(mapId = 0; mapId < m_pIStdMat->NumSubTexmaps(); mapId++)
	{
		// get the texture map
		Texmap *pTexMap;
		pTexMap = m_pIStdMat->GetSubTexmap(mapId);

		// check if the map is valid
		if((pTexMap != 0) && (m_pIStdMat->MapEnabled(mapId)))
		{
			mapCount++;
		}
	}

	return mapCount;
}

//----------------------------------------------------------------------------//
// Get the filename of the bitmap of the given map                            //
//----------------------------------------------------------------------------//
std::string CMaxMaterial::GetMapFilename(Texmap *pTexMap)
{
	CString	strFilename;
	if(pTexMap->ClassID() == Class_ID(BMTEX_CLASS_ID, 0))
	{
		// get the full filepath
		SECFileSystem	fs;
		strFilename = fs.GetFileName(((BitmapTex *)pTexMap)->GetMapName());
	}
	else
	{
		// Определить тип мапы
		// Добавить название материала
		strFilename = CString(GetName().c_str()) + "_" + CString(pTexMap->GetName()) + ".png";
	}
	strFilename.Replace("#", "_");
	return std::string( strFilename.GetBuffer(0));
}

std::string CMaxMaterial::GetMapFilename(int mapId)
{
	int mapCount;
	mapCount = 0;

	// loop through all maps of the material
	int materialMapId = m_pIStdMat->NumSubTexmaps();
	for(materialMapId = 0; materialMapId < m_pIStdMat->NumSubTexmaps(); materialMapId++)
	{
		// get the texture map
		Texmap *pTexMap;
		pTexMap = m_pIStdMat->GetSubTexmap(materialMapId);

		// check if the map is valid
		if((pTexMap != 0) && (m_pIStdMat->MapEnabled(materialMapId)))
		{
			// check if we reached the wanted map
			if(mapId == mapCount)
			{
				return GetMapFilename( pTexMap);
			}

			mapCount++;
		}
	}

	return "";
}

//----------------------------------------------------------------------------//
// Get the name of the material                                               //
//----------------------------------------------------------------------------//

std::string CMaxMaterial::GetName()
{
	// check if the internal material is valid
	if(m_pIStdMat == 0) return "<void>";

	return m_pIStdMat->GetName();
}

//----------------------------------------------------------------------------//
// Get the shininess factor of the material                                   //
//----------------------------------------------------------------------------//

float CMaxMaterial::GetShininess()
{
	//	return m_pIStdMat->GetShinStr(0);	// 19.01.2006
	return m_pIStdMat->GetShininess(0);
}

//----------------------------------------------------------------------------//
// Get the specular color of the material                                     //
//----------------------------------------------------------------------------//

void CMaxMaterial::GetSpecularColor(float *pColor)
{
	// get color from the internal standard material
	Color color;
	color = m_pIStdMat->GetSpecular(0);

	float specLevel = m_pIStdMat->GetShinStr(0);

	pColor[0] = color.r*specLevel;
	pColor[1] = color.g*specLevel;
	pColor[2] = color.b*specLevel;
	pColor[3] = 0.0f;
}

//----------------------------------------------------------------------------//
DWORD get_2power(DWORD val, BOOL bUpper)
{
	if( val == 0)	
		return 0;
	DWORD	stVal = 0x80000000;
	while( !(val&stVal))
		stVal >>= 1;

	if( val&(~stVal))
		return bUpper?(stVal<<1):stVal;
	else
		return bUpper?stVal:(stVal>>1);
}

int is_2_power(DWORD num)
{
	if( num == 0)	
		return 0;
	int pow=0;
	DWORD t=1;
	while((t&num) == 0)
	{
		t <<= 1;
		pow++;
		if( t == 0x00008000)	return -1;
	}
	if(t == num)	return pow;
	return -1;

}

DWORD get_mip_count(DWORD dwWidth, DWORD dwHeight)
{
	if( dwWidth == dwHeight)	return -1;	// square texture
	if( is_2_power(dwHeight) == -1)	return -1;	// height is not power of 2

	int step = 0;
	int mipcount = 0;
	for( int i=0; i<32; i++)
	{
		if( (dwWidth & 0x1) == 0x1)
			mipcount++;
		else
		{
			if( mipcount>0 && dwWidth != 0)
				return -1;	// существует 1 после последовательности единиц и нулей
		}
		dwWidth >>= 1;
	}
	return mipcount<=1 ? -1 : mipcount;
}

bool CMaxMaterial::SaveMap2(Texmap *pTexMap, CString& strFilename)
{
	WORD outputWidth = 256;
	WORD outputHeight = 256;
	TSTR className = "";
	BOOL isBitmapTexture = FALSE;

	BitmapTex* tex = NULL;
	Bitmap* bmp = NULL;

	strFilename.MakeLower();
	pTexMap->GetClassName(className);

	// get the full filepath
	SECFileSystem	fs;

	bool isNeedToResize = false;
	if (className == TSTR("Bitmap") && pTexMap->SuperClassID() == TEXMAP_CLASS_ID)
	{
		isBitmapTexture = TRUE;
		tex = (BitmapTex*) pTexMap;
		ATLASSERT(tex);
		if (tex)
		{
			bmp = tex->GetBitmap(0);
			
			if( bmp)
			{
				outputWidth = get_2power(bmp->Width(), FALSE);
				outputHeight = get_2power(bmp->Height(), FALSE);

				if (outputWidth * 2 == bmp->Width())
				{
					outputWidth = bmp->Width();
			}

				if (outputHeight * 2 == bmp->Height())
				{
					outputHeight = bmp->Height();
				}

				if ((bmp->Width() != outputWidth) ||
					(bmp->Height() != outputHeight))
				{
					isNeedToResize = true;
					MessageBox(NULL, "Текстура \n" + fs.GetFileName(strFilename) +
					  "\n имеет ширину или высоту не являющуюся степенью 2-ки и будет уменьшена экспортером!",
					  "Экспортер", MB_OK);
				}

				if ((outputWidth > 1024) || (outputHeight > 1024))
				{
					isNeedToResize = true;
					MessageBox(NULL, "Текстура \n" + fs.GetFileName(strFilename) +
					  "\n имеет ширину или высоту больше 1024 пикселей и будет уменьшена экспортером!",
					  "Экспортер", MB_OK);
				}				
			}
			else
			{
				return false;
			}
		}
		else
		{
			return false;
		}
	}
	
	bool	createTextureFile = true;

/*	if(fs.FileExists( strFilename))
	{
		if( CExporter::calexpoptAutoReplaceSameTextures==0)
		{
			CResExistsDlg	dlg( 0, GetMapFilename(pTexMap).c_str());
			if( dlg.DoModal() == IDCANCEL)
				createTextureFile = false;
			if( dlg.m_bHideWindow)
			{
				CExporter::calexpoptAutoReplaceSameTextures = createTextureFile?1:2;
			}
		}
		else	if( CExporter::calexpoptAutoReplaceSameTextures == 2)
		{
			createTextureFile = false;
		}
	}*/

	// записать файл, если его не было или перезаписать, если так указано
	if (createTextureFile && isBitmapTexture)
	{
		TCHAR*		lpFileNameAndPath = tex->GetMapName();
		ATLASSERT( lpFileNameAndPath);
		if( !lpFileNameAndPath)
		{
			return false;
		}

		CString fileNameAndPath(lpFileNameAndPath);
		CString fileName = fs.GetFileName(fileNameAndPath);
		CString filePath = fs.GetPath(fileNameAndPath);
		CString mipFilePathAndName = filePath + "\\mipmap\\" + fileName;
		if(fs.FileExists(mipFilePathAndName))
		{
			fileNameAndPath = mipFilePathAndName;
		}
		fileNameAndPath.Replace("\\\\", "\\");
		fileNameAndPath.Replace("//", "/");

		bmp = tex->GetBitmap(0);
		
		// проверяем используется ли альфа канал текстуры
		//
		bool useAlpha = false;
		bool hasAlpha = false;

		if (bmp->HasAlpha())
		{
			hasAlpha = true;
		}
		if (tex->GetAlphaSource() == 0) // 0 - image alpha, 3 - none
		{
			useAlpha = true;
		}

		// проверим psd файл или нет
		bool isPSD = false;
		FREE_IMAGE_FORMAT imgFormat = FreeImage_GetFileType(fileNameAndPath.GetBuffer(0));

		if (imgFormat == FIF_PSD)
		{
			isPSD = true; // означает, что необходимо сохранить в png. И если не используется альфа то потом в jpg

			MessageBox(NULL, "Текстура " + fs.GetFileName(fileNameAndPath) + "\n" +
						fileNameAndPath +
					  "\n имеет формат PSD и будет пересохранена экспортером в другой формат!",
					  "Экспортер", MB_OK);
		}

		if (!fs.FileExists(fileNameAndPath.GetBuffer(0)))
		{
			MessageBox(NULL, "Текстура " + fs.GetFileName(fileNameAndPath) + "\n" + 
					fileNameAndPath +
				  "\n не найдена и не может быть скопирована на сервер!",
				  "Экспортер", MB_OK);
			return false;
		}

		// загружаем файл в фрее
		fipImage freeImage;
		if(!freeImage.load(fileNameAndPath.GetBuffer(0)))
		{
			MessageBox(NULL, "Текстура " + fs.GetFileName(fileNameAndPath) + "\n" + 
					fileNameAndPath +
				  "\n сохранена в неподдерживаемом формате и не может быть скопирована на сервер!",
				  "Экспортер", MB_OK);
			return false;
		}

		std::string fileFormat = "";
		int savingFlags = 0;

		CString strNewFilename = strFilename;
		CString fileNameSearch = fs.GetFileName(strFilename);
		
		if (isPSD)
		{
			strNewFilename = strNewFilename.Left(strNewFilename.ReverseFind('.')) + ".png";
			freeImage.save(strNewFilename.GetBuffer(0), PNG_Z_NO_COMPRESSION);
			freeImage.load(strNewFilename.GetBuffer(0));
			fs.DeleteFile(strNewFilename);
			fileNameSearch = fs.GetFileName(strNewFilename);
		}

			// конвертация в 32bit image
			int bpp = freeImage.getBitsPerPixel();
			if (bpp > 32)
			{
			isNeedToResize = true;
			MessageBox(NULL, "Текстура " + fs.GetFileName(fileNameAndPath) + "\n" +
				fileNameAndPath +
				  "\n имеет 16 бит на канал цвета и будет пересохранена экспортером в другой формат!",
				  "Экспортер", MB_OK);				
				freeImage.convertTo32Bits();
			}

		bool isPngSaved = false;
		if (hasAlpha && useAlpha)
		{
			// файл с альфой
				

			// проверим количество значений альфы. Если они всего 0 и 255 то сохраним в gif
			bool oneBitAlpha = false; // больше не используем GIF
			RGBQUAD pixel;
			/*for (int y = 0; y < freeImage.getHeight(); y++)
			{
				for (int x = 0; x < freeImage.getWidth(); x++)
				{
					freeImage.getPixelColor(x, y, &pixel);
					if ((pixel.rgbReserved != 0) && (pixel.rgbReserved != 255))
					{
						oneBitAlpha = false;
						break;
					}
				}

				if (oneBitAlpha == false)
				{
					break;
				}
			}*/
			//CString temp;
			//temp.Format("%s ,%d", fileNameAndPath.GetBuffer(0), oneBitAlpha);
			//MessageBox(NULL, temp.GetBuffer(0), "test", MB_ICONERROR);
			//oneBitAlpha = false; // не сохранять gif
			if (oneBitAlpha == true)
			{
				// ресайзинг файла для gif (необходимо делать перед конвертацией)
				if( outputWidth > 1024)
				{
					outputWidth = 1024;
				}
				if( outputHeight > 1024)
				{
					outputHeight = 1024;
				}

				if ((outputHeight != freeImage.getHeight()) || (outputWidth != freeImage.getWidth()))
				{
					freeImage.rescale(outputWidth, outputHeight, FILTER_BILINEAR);
				}

				fileFormat = ".gif";
				freeImage.convertTo24Bits();
				freeImage.colorQuantize(FIQ_WUQUANT);

				int paletteSize = freeImage.getColorsUsed();
				BYTE *transparency = new BYTE[paletteSize];
				RGBQUAD *palette =  freeImage.getPalette();

				for (int i = 0; i < paletteSize; i++)
				{
					if ((palette[i].rgbBlue == 255) && (palette[i].rgbRed == 255) && (palette[i].rgbGreen == 255))
					{
						transparency[i] = 0;	
					}
					else
					{
						transparency[i] = 255;
					}
				}

				freeImage.setTransparencyTable(transparency, paletteSize);
				delete[] transparency;
			}
			else
			{
				fileFormat = ".png";
				isPngSaved = true;
				savingFlags = PNG_Z_BEST_COMPRESSION;
			}
		}
		else 
		{
			// файл без альфы или альфа не используется
			freeImage.convertTo24Bits();
			fileFormat = ".jpg";
			savingFlags = CExporter::texturesExportQuality;
		}

		// ресайзинг файла
		if( outputWidth>1024)	{outputWidth = 1024;}
		if( outputHeight>1024)	{outputHeight = 1024;}
		if (outputHeight != freeImage.getHeight() || outputWidth != freeImage.getWidth())
		{
			freeImage.rescale(outputWidth, outputHeight, FILTER_BILINEAR);
		}
	
//		bool wasExisted = false;

		if ((isPSD) || (isNeedToResize))
		{
		strNewFilename = strNewFilename.Left(strNewFilename.ReverseFind('.')) + fileFormat.c_str();
		if(!fs.FileExists(strNewFilename))
		{	
			freeImage.save(strNewFilename.GetBuffer(0), savingFlags);
		}
		}
		else
		{
			if (fs.FileExists(fileNameAndPath))
			{
				if (fs.FileExists(strNewFilename))
				{
					fs.DeleteFile(strNewFilename);
		}
				fs.CopyFile(fileNameAndPath, strNewFilename);
			}
		}

//		else
//		{
//			wasExisted = true;
//		}

		strFilename = strNewFilename;
		strNewFilename = fs.GetFileName(strNewFilename);

		if (m_coreMaterial != NULL)
		{			
			// замена имен файлов
 			for (int i = 0; i < m_coreMaterial->getVectorMap().size(); i++)
			{
				if (fileNameSearch.CompareNoCase(m_coreMaterial->getVectorMap()[i].strFilename) == 0)     
				{
					m_coreMaterial->getVectorMap()[i].strFilename = strNewFilename.GetBuffer(0);
					break;
				}				
			}
		}

		/* раскомментировать если понадобится оптимизация png
		if (isPngSaved && !wasExisted)
		{
			// use optiPNG			
			CString optiPngParam = "";
			CorrectPathSlashes(strFilename);			
			optiPngParam.Format("%soptipng -force \"%s\"", CExporter::MaxInstallPath, strFilename.GetBuffer(0));
			CString optiPngPath = "";
			optiPngPath.Format("%soptipng", CExporter::MaxInstallPath);
			_spawnlp(P_WAIT, optiPngPath.GetBuffer(0), optiPngParam.GetBuffer(0), NULL);
		}*/		
	}

	return true;
}

// use for Cal3d models
void CMaxMaterial::SaveMap(Texmap *pTexMap, CString& strFilename)
{
	WORD	outputWidth = 256;
	WORD	outputHeight = 256;
	TSTR	className = "";
	BOOL	isBitmapTexture = FALSE;
	BOOL	isPsdFile = FALSE;

	pTexMap->GetClassName(className);
	strFilename.MakeLower();
	if( strFilename.Find(".psd") != -1)
	{
		isPsdFile = TRUE;
		strFilename.Replace(".psd", ".png");
	}

	if(className==TSTR("Bitmap") && pTexMap->SuperClassID() == TEXMAP_CLASS_ID)
	{
		isBitmapTexture = TRUE;
		BitmapTex * tex = (BitmapTex*) pTexMap;
		
		ATLASSERT( tex);
		if( tex)
		{
			Bitmap*	bmp = tex->GetBitmap( 0);
			//ATLASSERT( bmp);
			if( bmp)
			{
				outputWidth = get_2power( bmp->Width(), TRUE);
				outputHeight = get_2power( bmp->Height(), TRUE);
			}
			else
			{
				return;
			}
		}
	}
	// get the full filepath
	SECFileSystem	fs;
	bool	createTextureFile = true;
	if( fs.FileExists( strFilename))
	{
		if( CExporter::calexpoptAutoReplaceSameTextures==0)
		{
			CResExistsDlg	dlg( 0, GetMapFilename(pTexMap).c_str());
			if( dlg.DoModal() == IDCANCEL)
				createTextureFile = false;
			if( dlg.m_bHideWindow)
			{
				CExporter::calexpoptAutoReplaceSameTextures = createTextureFile?1:2;
			}
		}
		else	if( CExporter::calexpoptAutoReplaceSameTextures == 2)
		{
			createTextureFile = false;
		}
	}

	if( createTextureFile)
	{
		// Проверим, может имеет смысл просто скопировать файл.
		// Условие копирования: файл должен быть миптекстурой или иметь степень двойки
		BOOL bitmapRenderRequired = TRUE;
		if( isBitmapTexture)
		{
			BitmapTex * tex = (BitmapTex*) pTexMap;
			ATLASSERT( tex);
			if( !tex)	return;
			TCHAR*		lpFileNameAndPath = tex->GetMapName();
			ATLASSERT( lpFileNameAndPath);
			if( !lpFileNameAndPath)	return;

			CString fileNameAndPath(lpFileNameAndPath);
			CString fileName = fs.GetFileName(fileNameAndPath);
			CString filePath = fs.GetPath(fileNameAndPath);
			CString mipFilePathAndName = filePath + "\\mipmap\\" + fileName;
			if( fs.FileExists(mipFilePathAndName))
				fileNameAndPath = mipFilePathAndName;
			Bitmap*	bmp = tex->GetBitmap( 0);

			if( isPsdFile)
			{
				fileNameAndPath.Replace(".psd", ".png");
				if(	get_mip_count( bmp->Width(), bmp->Height()) != -1)
				{
					MessageBox( 0, "PSD file should not contains mip-maps\n["+fileNameAndPath+"]", "Warning...", MB_OK);
				}
			}
			else	if( (outputWidth == bmp->Width()
				&& outputHeight == bmp->Height()) ||			// картинка уже степени двойки ИЛИ
				get_mip_count( bmp->Width(), bmp->Height()) != -1)	// это мипмап текстура
			{

				// Просто скопируем файл
				if( fs.FileExists( fileNameAndPath))
				{
					fs.DeleteFile( strFilename);
					if( fs.CopyFile( fileNameAndPath, strFilename))
					{
						bitmapRenderRequired = FALSE;
					}
				}
			}
		}
		
		if( bitmapRenderRequired)
		{
			Bitmap *bmap;
			BitmapInfo bi;

			if( outputWidth>1024)	outputWidth = 1024;
			if( outputHeight>1024)	outputHeight = 1024;
			
			bi.SetType(BMM_TRUE_64);
			bi.SetWidth(outputWidth);
			bi.SetHeight(outputHeight);
			bi.SetFlags(MAP_HAS_ALPHA);
			bi.SetCustomFlag(0);

			bmap = TheManager->Create(&bi);
			pTexMap->RenderBitmap(0.0, bmap);

			bi.SetName(strFilename);
			bmap->OpenOutput(&bi);
			bmap->Write( &bi);
			bmap->Close(&bi);
		}
	}
}


void CMaxMaterial::SaveMaps()
{
	int mapCount;
	mapCount = 0;

	// loop through all maps of the material
	int materialMapId;
	for(materialMapId = 0; materialMapId < m_pIStdMat->NumSubTexmaps(); materialMapId++)
	{
		// get the texture map
		Texmap *pTexMap;
		pTexMap = m_pIStdMat->GetSubTexmap(materialMapId);

		// check if the map is valid
		if((pTexMap != 0) && (m_pIStdMat->MapEnabled(materialMapId)))
		{
			// Вставить нормальную работу с директориями
			// get the full filepath
			SECFileSystem	fs;
			CString strFilename( GetMapFilename(pTexMap).c_str());
			CString	texturesPaths;
			if( CExporter::calexpoptUseTexturesPath)
				texturesPaths = fs.AppendPaths(fs.GetCurrentDirectory(), CExporter::calexportTexturesPath);
			else
				texturesPaths = fs.GetCurrentDirectory();
			strFilename = fs.AppendPaths(texturesPaths, strFilename);
			SaveMap(pTexMap, strFilename);
			mapCount++;
		}
	}
}

bool	TextureNameIsCommon(CString strFilename)
{
	strFilename.MakeLower();
	if( strFilename.Left(3) == "cmn")	return true;
	return false;
}

void CMaxMaterial::SaveMaps(LPCTSTR lpOthersPath, LPCTSTR lpCommonPath, LPCTSTR lpOthersPathServer, LPCTSTR lpCommonPathServer)
{
	ATLASSERT( lpOthersPath);
	ATLASSERT( lpCommonPath);

	int mapCount;
	mapCount = 0;

	// loop through all maps of the material
	int materialMapId;
	for(materialMapId = 0; materialMapId < m_pIStdMat->NumSubTexmaps(); materialMapId++)
	{
		// get the texture map
		Texmap *pTexMap;
		pTexMap = m_pIStdMat->GetSubTexmap(materialMapId);

		// check if the map is valid
		if((pTexMap != 0) && (m_pIStdMat->MapEnabled(materialMapId)))
		{
			// Вставить нормальную работу с директориями
			// get the full filepath
			SECFileSystem	fs;
			CString strFilename( GetMapFilename(pTexMap).c_str());
			
			CString strFilenameCopy = "";

			if ( CExporter::calexportSeparateCommonTextures &&
					TextureNameIsCommon(strFilename))
			{
				if (CString(lpCommonPathServer) != "")
				{
					fs.MakePath(CString(lpCommonPathServer));
					strFilenameCopy = CString(lpCommonPathServer);
				}

				strFilename = CString( lpCommonPath) + "\\" + strFilename;
			}
			else
			{
				if (CString(lpOthersPathServer) != "")
				{
					fs.MakePath(CString(lpOthersPathServer));
					strFilenameCopy = CString(lpOthersPathServer);
				}

				strFilename = CString( lpOthersPath) + "\\" + strFilename;
			}

			SaveMap2(pTexMap, strFilename);

			if (strFilenameCopy != "")
			{
				strFilenameCopy = strFilenameCopy + "\\" + fs.GetFileName(strFilename);
				CorrectPathSlashes(strFilenameCopy);
				fs.MakePath(fs.GetPath(strFilenameCopy));
				fs.DeleteFile(strFilenameCopy);
				fs.CopyFile(strFilename, strFilenameCopy);
			}

			mapCount++;
		}
	}
}

int CMaxMaterial::GetMaxMapType(int mapId)
{
	int mapCount;
	mapCount = 0;

	// loop through all maps of the material
	int materialMapId = m_pIStdMat->NumSubTexmaps();
	for(materialMapId = 0; materialMapId < m_pIStdMat->NumSubTexmaps(); materialMapId++)
	{
		// get the texture map
		Texmap *pTexMap;
		pTexMap = m_pIStdMat->GetSubTexmap(materialMapId);

		// check if the map is valid
		if((pTexMap != 0) && (m_pIStdMat->MapEnabled(materialMapId)))
		{
			// check if we reached the wanted map
			if(mapId == mapCount)	return materialMapId;

			mapCount++;
		}
	}

	return -1;
}

void CMaxMaterial::CorrectPathSlashes(CString& objectPath)
{
	objectPath.Replace("/", "\\");
	while ( objectPath.Find("\\\\")!=-1)
		objectPath.Replace("\\\\", "\\");
}