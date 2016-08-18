
#include "stdafx.h"
#include <string>
#include "glApp.h"
#include "GlobalSingletonStorage.h"
#include "SimpleDrawingTarget.h"
#include "FilterPalette.h"
#include "PlatformDependent.h"
#include "SoundTrack.h"
#include "TimeProcessor.h"

#include "PBO.h"

int g_sourceTextureID;
int g_yuvTextureID;
int g_resultTextureID;
int g_waveletTextureID;
int g_waveletDepackTextureID;
CColor3 g_rainColor(255, 255, 255);

CGLApp glApp("test", 1280, 1024);

CSimpleDrawingTarget* g_renderTarget;

CFilterPalette g_filterPalette;
CFilter* g_rgbToYUVFilter = NULL;
CFilter* g_yuvToRGBFilter = NULL;
CFilter* g_waveletCompositionFilter = NULL;
CFilter* g_waveletDecompositionFilter = NULL;

CSoundTrack g_soundTrack;

#define TEXTURE_SIZE	1024

int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) 
{
	glApp.WinMain(hInstance, hPrevInstance, lpCmdLine, nCmdShow);
}

CGLApp::CGLApp(std::string name, unsigned int width, unsigned int height):
	CGLApplication(name, width, height, true)
{
	
}

void DrawPoints()
{
	/*g->stp.PushMatrix();
	g->cm.SetCamera2d();
	glBegin(GL_POINTS);
	g->stp.SetColor(200, 200, 200);
	g->stp.PrepareForRender();
	for (int x = 0; x < 3; x++)
	{
		float xx = rand() / 32768.0f * 2.0f - 1.0f;
		float yy = rand() / 32768.0f * 2.0f - 1.0f;
		for (int y = 0; y < 10; y++)
		{
			glVertex2f(xx + rand() / 32768.0f * 0.1f - 0.05f, yy + rand() / 32768.0f * 0.1f - 0.05f);
		}
	}
	glEnd();
	static int cnt = 0;
	cnt++;
	if (cnt%2048 == 256)
	{
		g_font->OutTextXY(320, 256, L"Ñ Ä Ð Í", FONT_ALIGN_CENTER, FONT_VALIGN_MIDDLE);
	}
	if (cnt%2048 == 512)
	{
		g_font->OutTextXY(320, 256, L"2 0 1 0", FONT_ALIGN_CENTER, FONT_VALIGN_MIDDLE);
	}
	if (cnt%2048 == 768)
	{
		g_font->OutTextXY(320, 256, L"Ì È Ð", FONT_ALIGN_CENTER, FONT_VALIGN_MIDDLE);
	}
	if (cnt%2048 == 1024)
	{
		g_font->OutTextXY(320, 256, L"Ì À Ñ È", FONT_ALIGN_CENTER, FONT_VALIGN_MIDDLE);
	}
	if (cnt%2048 == 1280)
	{
		g_smallFont->OutTextXY(320, 256, L"Æ Â À × Ê À", FONT_ALIGN_CENTER, FONT_VALIGN_MIDDLE);
	}
	if (cnt%2048 == 1536)
	{
		g_smallFont->OutTextXY(320, 128, L"Ð Î Ê", FONT_ALIGN_CENTER, FONT_VALIGN_MIDDLE);
		g_smallFont->OutTextXY(320, 256, L"- í -", FONT_ALIGN_CENTER, FONT_VALIGN_MIDDLE);
		g_smallFont->OutTextXY(320, 384, L"Ð Î Ë Ë", FONT_ALIGN_CENTER, FONT_VALIGN_MIDDLE);
	}
	if (cnt%2048 == 1792)
	{
		g_font->OutTextXY(320, 256, L"Õ Ð Þ !", FONT_ALIGN_CENTER, FONT_VALIGN_MIDDLE);
	}
	g->stp.PopMatrix();*/
}

CPBO* pbo = NULL;

void CGLApp::Pack()
{
	g_renderTarget->AttachTexture(g->tm.GetObjectPointer(g_yuvTextureID));
	g_rgbToYUVFilter->SetSourceImage(0, g_sourceTextureID);
	g_renderTarget->ApplyFilter(g_rgbToYUVFilter);

	g->tm.BindTexture(g_sourceTextureID);

	for (int i = 0; i < 100; i++)
	{
	glBegin(GL_QUADS);
	glVertex2f(-1,1);
	glVertex2f(-1,-1);
	glVertex2f(1,-1);
	glVertex2f(1,1);
	glEnd();
	}

	glFlush();
	glFinish();

	g->tp.Init();
	g->tp.Start();

	/*char* a = new char[TEXTURE_SIZE * TEXTURE_SIZE * 4];
	int tt1 = g->tp.GetTickCount();
	glGetTexImage(GL_TEXTURE_2D, 0, GL_BGRA_EXT, GL_UNSIGNED_BYTE, a);
	int tt2 = g->tp.GetTickCount();
	delete[] a;*/

	/*int t1 = g->tp.GetTickCount();
	if (!pbo)
		pbo = new CPBO(TEXTURE_SIZE, TEXTURE_SIZE, 4, false, PBO_USAGE_READ_TEXTURE);
	int t2 = g->tp.GetTickCount();
	pbo->Capture();
	int t3 = g->tp.GetTickCount();
	glFlush();
	glFinish();
	Sleep(600);
	int t4 = g->tp.GetTickCount();
	void* data = pbo->GetPointer();
	/*FILE* fl = fopen("d:\\2.raw", "wb");
	fwrite(data, 1, TEXTURE_SIZE * TEXTURE_SIZE * 4, fl);
	fclose(fl);*/
	/*int t45 = g->tp.GetTickCount();
	pbo->ReleasePointer();
	int t46 = g->tp.GetTickCount();
	void* data3 = pbo->GetPointer();
	pbo->ReleasePointer();
	int t5 = g->tp.GetTickCount();
	int ii = 0;*/

	/*char* a = new char[TEXTURE_SIZE * TEXTURE_SIZE * 3];
	memset(a, 0xFFFFFFFF, TEXTURE_SIZE * TEXTURE_SIZE * 3);

	int tt1 = g->tp.GetTickCount();
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, TEXTURE_SIZE, TEXTURE_SIZE, 0, GL_RGB, GL_UNSIGNED_BYTE, a);
	int tt2 = g->tp.GetTickCount();*/

	/*int t1 = g->tp.GetTickCount();
	if (!pbo)
		pbo = new CPBO(TEXTURE_SIZE, TEXTURE_SIZE, 3, true, PBO_USAGE_WRITE_TEXTURE);
	int t2 = g->tp.GetTickCount();
	void* data = pbo->GetPointer();
	int t3 = g->tp.GetTickCount();
	memset(data, 0xFFFFFFFF, TEXTURE_SIZE * TEXTURE_SIZE * 4);
	int t4 = g->tp.GetTickCount();
	pbo->ReleasePointer();
	Sleep(500);
	glFlush();
	glFinish();
	int t5 = g->tp.GetTickCount();
	pbo->Capture();
	int t6 = g->tp.GetTickCount();
	glTexImage2D(GL_TEXTURE_2D, 0, GL_BGRA_EXT, TEXTURE_SIZE, TEXTURE_SIZE, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	int t7 = g->tp.GetTickCount();
	pbo->Uncapture();
	int t8 = g->tp.GetTickCount();*/

	g_renderTarget->AttachTexture(g->tm.GetObjectPointer(g_waveletTextureID));
	g_waveletCompositionFilter->SetSourceImage(0, g_yuvTextureID);
	g_renderTarget->ApplyFilter(g_waveletCompositionFilter);
}

void CGLApp::Unpack()
{
	//g->tm.SaveTexture(g_waveletTextureID, "d:\\step1.png");

	g_renderTarget->AttachTexture(g->tm.GetObjectPointer(g_waveletDepackTextureID));
	g_waveletDecompositionFilter->SetSourceImage(0, g_waveletTextureID);
	g_waveletDecompositionFilter->SetFloatArgument("modResult", 8);
	g_renderTarget->ApplyFilter(g_waveletDecompositionFilter);

	//g->tm.SaveTexture(g_waveletDepackTextureID, "d:\\step2.png");

	g_renderTarget->AttachTexture(g->tm.GetObjectPointer(g_waveletTextureID));
	g_waveletDecompositionFilter->SetSourceImage(0, g_waveletDepackTextureID);
	g_waveletDecompositionFilter->SetFloatArgument("modResult", 4);
	g_renderTarget->ApplyFilter(g_waveletDecompositionFilter);

	//g->tm.SaveTexture(g_waveletTextureID, "d:\\step3.png");

	g_renderTarget->AttachTexture(g->tm.GetObjectPointer(g_waveletDepackTextureID));
	g_waveletDecompositionFilter->SetSourceImage(0, g_waveletTextureID);
	g_waveletDecompositionFilter->SetFloatArgument("modResult", 2);
	g_renderTarget->ApplyFilter(g_waveletDecompositionFilter);

	//g->tm.SaveTexture(g_waveletDepackTextureID, "d:\\step4.png");

	g_renderTarget->AttachTexture(g->tm.GetObjectPointer(g_waveletTextureID));
	g_waveletDecompositionFilter->SetSourceImage(0, g_waveletDepackTextureID);
	g_waveletDecompositionFilter->SetFloatArgument("modResult", 1);
	g_renderTarget->ApplyFilter(g_waveletDecompositionFilter);

	//g->tm.SaveTexture(g_waveletTextureID, "d:\\step5.png");

	g_renderTarget->AttachTexture(g->tm.GetObjectPointer(g_resultTextureID));
	g_yuvToRGBFilter->SetSourceImage(0, g_waveletTextureID);
	g_renderTarget->ApplyFilter(g_yuvToRGBFilter);

	/*g->tm.SaveTexture(g_yuvTextureID, "d:\\yuv.png");
	g->tm.SaveTexture(g_resultTextureID, "d:\\rgb.png");*/
}

bool CGLApp::DrawScene()
{
	g->tp.Init();
	g->tp.Start();
	int t1 = g->tp.GetTickCount();

	for (int i = 0; i < 3; i++)
	{
		Pack();	
		Unpack();
	}

	glFlush();
	glFinish();

	int t2 = g->tp.GetTickCount();

	for (int i = 0; i < 10; i++)
	{
		Pack();	
		//Unpack();
	}
	glFlush();
	glFinish();

	int t3 = g->tp.GetTickCount();

	t1 = g->tp.GetTickCount();

	for (int i = 0; i < 10; i++)
	{
		Unpack();	
	}

	glFlush();
	glFinish();

	t2 = g->tp.GetTickCount();

	for (int i = 0; i < 10; i++)
	{
		Unpack();	
	}
	glFlush();
	glFinish();

	t3 = g->tp.GetTickCount();

	return false; // execute only once

	// update rain
	/*static int id = 0;
	id++;
	id = id%3;

	g_renderTarget->AttachTexture(g->tm.GetObjectPointer(g_renderTexturesID[(id + 1)%3]));
	g_renderTarget->ApplyGeometry(DrawPoints);

	g_renderTarget->AttachTexture(g->tm.GetObjectPointer(g_renderTexturesID[(id + 2)%3]));
	g_rainFilter->SetSourceImage(0, g_renderTexturesID[(id + 1)%3]);
	g_rainFilter->SetSourceImage(1, g_renderTexturesID[id]);
	g_renderTarget->ApplyFilter(g_rainFilter);

	g_applyRainFilter->SetSourceImage(0, g_rainTextureID);
	g_applyRainFilter->SetSourceImage(1, g_renderTexturesID[(id + 2)%3]);
	
	glClearColor(1.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);
	g->stp.PushMatrix();
	g->cm.SetCamera2d();
	g->stp.SetState(ST_CULL_FACE, false);
	g->stp.SetState(ST_ZTEST, false);
	g_applyRainFilter->Bind(1280, 1024);
	g->sp.PutNormalSprite(0, 0, 0, 0, 1280, 1024, g_rainTextureID, g_rainColor);
	g_applyRainFilter->Unbind();
	g->stp.PopMatrix();	*/
}

void CGLApp::OnLoading()
{
	/*glHint(GL_TEXTURE_COMPRESSION_HINT_ARB, GL_NICEST);
	g->rs.SetBool(NORMAL_TEXTURES_PACKING, true);
	g->rs.SetBool(TRANSPARENT_TEXTURES_PACKING, true);*/
	std::string fileName = "source";
	fileName += IntToStr(TEXTURE_SIZE);
	fileName += ".png";
	g_sourceTextureID = g->tm.LoadTextureFromFile("source", fileName);

	g->tm.BindTexture(g_sourceTextureID);

	/*for (int i = 0; i < 100; i++)
	{
	glBegin(GL_QUADS);
	glVertex2f(-1,1);
	glVertex2f(-1,-1);
	glVertex2f(1,-1);
	glVertex2f(1,1);
	glEnd();
	}*/
	
	g_renderTarget = new CSimpleDrawingTarget();
	g_renderTarget->SetFormat(false, 0);
	g_renderTarget->SetWidth(TEXTURE_SIZE);
	g_renderTarget->SetHeight(TEXTURE_SIZE);
	
	g_yuvTextureID = g_renderTarget->CreateTempTexture();
	g_yuvTextureID = g->tm.ManageGLTexture(g_yuvTextureID, TEXTURE_SIZE, TEXTURE_SIZE);

	g_resultTextureID = g_renderTarget->CreateTempTexture();
	g_resultTextureID = g->tm.ManageGLTexture(g_resultTextureID, TEXTURE_SIZE, TEXTURE_SIZE);

	g_waveletTextureID = g_renderTarget->CreateTempTexture();
	g_waveletTextureID = g->tm.ManageGLTexture(g_waveletTextureID, TEXTURE_SIZE, TEXTURE_SIZE);

	g_waveletDepackTextureID = g_renderTarget->CreateTempTexture();
	g_waveletDepackTextureID = g->tm.ManageGLTexture(g_waveletDepackTextureID, TEXTURE_SIZE, TEXTURE_SIZE);
	
	g_renderTarget->AttachTexture(g->tm.GetObjectPointer(g_yuvTextureID));
	g_renderTarget->Create();

	g_filterPalette.LoadAll();	
	g_rgbToYUVFilter = g_filterPalette.GetObjectPointer(g_filterPalette.GetObjectNumber("wavelet_rgb_to_yuv.xml"));
	g_yuvToRGBFilter = g_filterPalette.GetObjectPointer(g_filterPalette.GetObjectNumber("wavelet_yuv_to_rgb.xml"));
	g_waveletCompositionFilter = g_filterPalette.GetObjectPointer(g_filterPalette.GetObjectNumber("wavelet_composition.xml"));
	g_waveletDecompositionFilter = g_filterPalette.GetObjectPointer(g_filterPalette.GetObjectNumber("wavelet_decomposition.xml"));

	g->stp.SetDefaultStates();
	g->stp.SetState(ST_ZTEST, false);
	g->stp.PrepareForRender();
}