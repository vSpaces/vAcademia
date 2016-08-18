#include "../StdAfx.h"
#include <cppunit/config/SourcePrefix.h>
#include "../../VASEBitmap.h"

#include "../../VASEFramesDifPack.h"
#include "./test_vaseencoder.h"
#include "../BitmapFixture.h"

#define SAMPLE_RABBIT24_WIDTH	150
#define SAMPLE_RABBIT24_HEIGHT	94

#define SAMPLE_RABBIT32_WIDTH	150
#define SAMPLE_RABBIT32_HEIGHT	94

CPPUNIT_TEST_SUITE_REGISTRATION( VASEEncoderTest );

//??
char	sampleBmpBuffer24[60] = {66,77,60,0,0,0,0,0,0,0,54,0,0,0,40,0,0,0,1,0,0,0,1,0,0,0,1,0,24,0,0,0,0,0,6,0,0,0,18,11,0,0,18,11,0,0,0,0,0,0,0,0,0,0,255,255,255,0,0,0};
char	sampleBmpBuffer32[60] = {66,77,60,0,0,0,0,0,0,0,54,0,0,0,40,0,0,0,1,0,0,0,1,0,0,0,1,0,32,0,0,0,0,0,6,0,0,0,18,11,0,0,18,11,0,0,0,0,0,0,0,0,0,0,255,255,255,0,0,0};
char	sampleIncorrectBuffer[] = {'I','n','c','o','r','r','e','c','t','B','u','f','f','e','r'};

VASEEncoderTest::VASEEncoderTest(void)
{
	fixture = NULL;
	bitmap32 = NULL;
	bitmap24 = NULL;
	bitmap32_changed = NULL;
	bitmap_large = NULL;
	m_memory = malloc(320 * 240 / 2);
}

VASEEncoderTest::~VASEEncoderTest(void)
{
	free(m_memory);
}


void 
VASEEncoderTest::setUp()
{
	fixture = new VASEEncoder();
	bitmap32 = new CBitmapFixture(L"Z:\\User\\Alex\\Programming\\VU\\Release_I_To_Head\\VASEncoder\\test\\\\Resources\\rabbit32.bmp");
	bitmap24 = new CBitmapFixture(L"Z:\\User\\Alex\\Programming\\VU\\Release_I_To_Head\\VASEncoder\\test\\\\Resources\\rabbit24.bmp");
	bitmap_large = new CBitmapFixture(L"Z:\\User\\Alex\\Programming\\VU\\Release_I_To_Head\\VASEncoder\\test\\\\Resources\\rabbit_large.bmp");
	bitmap32_changed = new CBitmapFixture(L"Z:\\User\\Alex\\Programming\\VU\\Release_I_To_Head\\VASEncoder\\test\\\\Resources\\rabbit32_changed.bmp");
}


void 
VASEEncoderTest::tearDown()
{
	delete fixture;
	delete bitmap24;
	delete bitmap32;
	delete bitmap_large;
	delete bitmap32_changed;
}

void VASEEncoderTest::testEncodeEmpty()
{
	// проверка на нулевые данные
	VASEBitmap* bitmap = new VASEBitmap();
	CPPUNIT_ASSERT(	fixture->EncodeDIBBitmap( NULL, 320, 240, 32, bitmap, m_memory) == false);
	delete bitmap;
}

void VASEEncoderTest::testEncodeEmptySize()
{
	// проверка на нулевой размер
	VASEBitmap* bitmap = new VASEBitmap();
	CPPUNIT_ASSERT(	fixture->EncodeDIBBitmap( (void*)&sampleBmpBuffer32, 0, 240, 32, bitmap, m_memory) == false);
	CPPUNIT_ASSERT(	fixture->EncodeDIBBitmap( (void*)&sampleBmpBuffer32, 320, 0, 32, bitmap, m_memory) == false);
	delete bitmap;
}

void VASEEncoderTest::testEncodeTooLargeSize()
{
	// проверка на слишком большой размер
	VASEBitmap* bitmap = new VASEBitmap();
	CPPUNIT_ASSERT(	fixture->EncodeDIBBitmap( (void*)bitmap_large->lpBits, bitmap_large->dimensions.cx
												, bitmap_large->dimensions.cy, 32, bitmap, m_memory) == false);
	delete bitmap;
}

void VASEEncoderTest::testEncodeCorrectBPP()
{
	// проверка на корректный BPP
	VASEBitmap* bitmap = new VASEBitmap();
	CPPUNIT_ASSERT(	fixture->EncodeDIBBitmap( (void*)bitmap32->lpBits, bitmap32->dimensions.cx, bitmap32->dimensions.cy, 32, bitmap, m_memory) == true);
	CPPUNIT_ASSERT(	fixture->EncodeDIBBitmap( (void*)bitmap24->lpBits, bitmap24->dimensions.cx, bitmap24->dimensions.cy, 24, bitmap, m_memory) == false);
	
	// 32 картинку при указании 24 битов загрузит
	CPPUNIT_ASSERT(	fixture->EncodeDIBBitmap( (void*)bitmap32->lpBits, bitmap32->dimensions.cx, bitmap32->dimensions.cy, 24, bitmap, m_memory) == false);
	delete bitmap;
}

void VASEEncoderTest::testEncodeIncorrectBPP()
{
	// проверка на не корректный BPP
	VASEBitmap* bitmap = new VASEBitmap();
	CPPUNIT_ASSERT(	fixture->EncodeDIBBitmap( (void*)&sampleBmpBuffer32, 1, 1, 16, bitmap, m_memory) == NULL);
	CPPUNIT_ASSERT(	fixture->EncodeDIBBitmap( (void*)&sampleBmpBuffer24, 1, 1, 0, bitmap, m_memory) == NULL);
	delete bitmap;
}

void VASEEncoderTest::testEncode32()
{
	// проверка на не корректный BPP
	VASEBitmap*	encodedBitmap = new VASEBitmap();
	bool res = fixture->EncodeDIBBitmap( (void*)bitmap32->lpBits, SAMPLE_RABBIT32_WIDTH, SAMPLE_RABBIT32_HEIGHT, 32, encodedBitmap, m_memory);
	CPPUNIT_ASSERT(	res);
	CPPUNIT_ASSERT(	encodedBitmap->GetSize().cx == SAMPLE_RABBIT32_WIDTH);
	CPPUNIT_ASSERT(	encodedBitmap->GetSize().cy == SAMPLE_RABBIT32_HEIGHT);
	const void*		lpData;
	unsigned int dataSize;
	unsigned int returnedCount = encodedBitmap->GetBuffer( &lpData, &dataSize);
	CPPUNIT_ASSERT(	lpData != NULL);
	CPPUNIT_ASSERT(	dataSize != 0);
	CPPUNIT_ASSERT(	returnedCount == dataSize);
	delete encodedBitmap;
}

void VASEEncoderTest::testEncode24()
{
	// проверка на не корректный BPP
	VASEBitmap*	bitmap = new VASEBitmap();
	CPPUNIT_ASSERT(	fixture->EncodeDIBBitmap( (void*)bitmap24->lpBits, SAMPLE_RABBIT24_WIDTH, SAMPLE_RABBIT24_HEIGHT, 24, bitmap, m_memory) == false);
	delete bitmap;
}

void VASEEncoderTest::testDefineChangeBlocks()
{
	VASEFramesDifPack*	diffPack = new VASEFramesDifPack();

	void * lpMaskBits = malloc(bitmap32->dimensions.cx * bitmap32->dimensions.cy / 2);
	unsigned long time = timeGetTime();
	for (int i = 0; i < 100; ++i)
	{
		fixture->EncodeDifference( bitmap32->lpBits, bitmap32->dimensions.cx, bitmap32->dimensions.cy, bitmap32->bpp
			, bitmap32_changed->lpBits, bitmap32_changed->dimensions.cx, bitmap32_changed->dimensions.cy, bitmap32_changed->bpp
			, diffPack, lpMaskBits);
	}
	unsigned long delta = timeGetTime() - time;
	std::cout << "time = " << delta;
	free(lpMaskBits);
	return;
	
	CPPUNIT_ASSERT(	diffPack != NULL);
	CPPUNIT_ASSERT(	diffPack->GetBitmapsCount() == 4);
	for( unsigned int i=0; i<diffPack->GetBitmapsCount(); i++)
	{
		VASEBitmap*		bitmap = diffPack->GetBitmap( i);
		CPPUNIT_ASSERT(	bitmap != NULL);
	}
	delete diffPack;
}

void VASEEncoderTest::testDefineChangeBlocksSize()
{
	CSize	sizes[] = { CSize(34,34), CSize(33,34), CSize(34,33), CSize(33,33)};
	VASEFramesDifPack*	diffPack = new VASEFramesDifPack();
	void * lpMaskBits = malloc(bitmap32->dimensions.cx * bitmap32->dimensions.cy / 2);
	fixture->EncodeDifference( bitmap32->lpBits, bitmap32->dimensions.cx, bitmap32->dimensions.cy, bitmap32->bpp
					, bitmap32_changed->lpBits, bitmap32_changed->dimensions.cx, bitmap32_changed->dimensions.cy, bitmap32_changed->bpp
					, diffPack, lpMaskBits);
	free(lpMaskBits);

	CPPUNIT_ASSERT(	diffPack != NULL);
	CPPUNIT_ASSERT(	diffPack->GetBitmapsCount() == 4);
	for( unsigned int i=0; i<diffPack->GetBitmapsCount(); i++)
	{
		VASEBitmap*		bitmap = diffPack->GetBitmap( i);
		CPPUNIT_ASSERT(	bitmap != NULL);
		CPPUNIT_ASSERT(	bitmap->GetSize() == sizes[i]);
	}
	delete diffPack;
}