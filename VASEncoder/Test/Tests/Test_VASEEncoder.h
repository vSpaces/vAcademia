#pragma once

#include <cppunit/extensions/HelperMacros.h>

class VASEEncoder;
class CBitmapFixture;

// Unit tests for StringToolsTest
class VASEEncoderTest : public CPPUNIT_NS::TestCase
{
	CPPUNIT_TEST_SUITE( VASEEncoderTest );
//	CPPUNIT_TEST( testEncodeEmpty );
//	CPPUNIT_TEST( testEncodeEmptySize );
//	CPPUNIT_TEST( testEncodeCorrectBPP );
//	CPPUNIT_TEST( testEncodeIncorrectBPP );
//	CPPUNIT_TEST( testEncodeTooLargeSize );
//	CPPUNIT_TEST( testEncode32 );
//	CPPUNIT_TEST( testEncode24 );
	CPPUNIT_TEST( testDefineChangeBlocks );
//	CPPUNIT_TEST( testDefineChangeBlocksSize );
	CPPUNIT_TEST_SUITE_END();

public:
	/*! Constructs a VASEEncoderTest object.
	*/
	VASEEncoderTest();

	/// Destructor.
	virtual ~VASEEncoderTest();

	void setUp();
	void tearDown();

	void testEncodeEmpty();
	void testEncodeEmptySize();
	void testEncodeTooLargeSize();
	void testEncodeCorrectBPP();
	void testEncodeIncorrectBPP();
	void testEncode32();
	void testEncode24();
	void testDefineChangeBlocks();
	void testDefineChangeBlocksSize();

private:
	/// Prevents the use of the copy constructor.
	VASEEncoderTest( const VASEEncoderTest &other );

	/// Prevents the use of the copy operator.
	void operator =( const VASEEncoderTest &other );

	void * m_memory;

private:
	VASEEncoder*	fixture;
	CBitmapFixture*	bitmap32;
	CBitmapFixture*	bitmap24;
	CBitmapFixture*	bitmap32_changed;
	CBitmapFixture*	bitmap_large;
};
