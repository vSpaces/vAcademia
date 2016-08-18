#include "../StdAfx.h"
#include <cppunit/config/SourcePrefix.h>
#include "../../fln/FLNFrame.h"
#include "../BitmapFixture.h"
#include "./FLNFrameTest.h"

#define SAMPLE_RABBIT32_WIDTH	150
#define SAMPLE_RABBIT32_HEIGHT	94

#define SAMPLE_RABBIT32_WIDTH	150
#define SAMPLE_RABBIT32_HEIGHT	94

CPPUNIT_TEST_SUITE_REGISTRATION( FLNFrameTest );

FLNFrameTest::FLNFrameTest(void)
{
	fixture = NULL;
	bitmap32 = NULL;
	bitmap32_changed = NULL;
}

FLNFrameTest::~FLNFrameTest(void)
{
}


void 
FLNFrameTest::setUp()
{
	fixture = new CFLNFrame();
	bitmap32 = new CBitmapFixture(L"Z:\\User\\Alex\\Programming\\VU\\Release_I_To_Head\\VASEncoder\\test\\Resources\\rabbit32.bmp");
	bitmap32_changed = new CBitmapFixture(L"Z:\\User\\Alex\\Programming\\VU\\Release_I_To_Head\\VASEncoder\\test\\Resources\\rabbit32_changed.bmp");
}


void 
FLNFrameTest::tearDown()
{
	delete fixture;
	delete bitmap32;
	delete bitmap32_changed;
}

void FLNFrameTest::testLoadFromDIB()
{
	CPPUNIT_ASSERT(	fixture->LoadFromDIB( bitmap32->dimensions.cx, bitmap32->dimensions.cy, bitmap32->lpBits, bitmap32->bpp));
}

void FLNFrameTest::testDefineChangeBlocks()
{
	fixture->LoadFromDIB( bitmap32->dimensions.cx, bitmap32->dimensions.cy, bitmap32->lpBits, bitmap32->bpp);

	CVASEBmp	bmp;
	CFLNFrame::FillWinBMPFromDIB(&bmp, bitmap32_changed->dimensions.cx, bitmap32_changed->dimensions.cy, bitmap32_changed->lpBits, bitmap32_changed->bpp);

	void * lpMaskBits = malloc(bitmap32_changed->dimensions.cx * bitmap32_changed->dimensions.cy / 2);
	unsigned int blocksCount = fixture->DefineChangeBlocks( false, bmp, lpMaskBits);
	free(lpMaskBits);
	CPPUNIT_ASSERT(	blocksCount == 4);
	
	for( unsigned int i=0; i<blocksCount; i++)
	{
		void* mem = NULL;
		unsigned int memSize = 0;

		if( fixture->GetChangedBlock( i, NULL, &memSize, NULL))
		{
			CPPUNIT_ASSERT(	4624 == memSize || 4488 == memSize || 4356 == memSize);
			mem = malloc( memSize);
			CPPUNIT_ASSERT( fixture->GetChangedBlock( i, (byte*)mem, &memSize, NULL));
			free( mem);
		}
		else
		{
			CPPUNIT_ASSERT_MESSAGE("Failed to calculate needed memory size in GetChangedBlock", FALSE);
		}
	}
}