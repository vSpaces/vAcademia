#pragma once

#include <cppunit/extensions/HelperMacros.h>

class CFLNFrame;
class CBitmapFixture;

// Unit tests for StringToolsTest
class FLNFrameTest : public CPPUNIT_NS::TestCase
{
	CPPUNIT_TEST_SUITE( FLNFrameTest );
	CPPUNIT_TEST( testLoadFromDIB );
	CPPUNIT_TEST( testDefineChangeBlocks );
	CPPUNIT_TEST_SUITE_END();

public:
	/*! Constructs a FLNFrameTest object.
	*/
	FLNFrameTest();

	/// Destructor.
	virtual ~FLNFrameTest();

	void setUp();
	void tearDown();

	void testLoadFromDIB();
	void testDefineChangeBlocks();

private:
	/// Prevents the use of the copy constructor.
	FLNFrameTest( const FLNFrameTest &other );

	/// Prevents the use of the copy operator.
	void operator =( const FLNFrameTest &other );

private:
	CFLNFrame*	fixture;
	CBitmapFixture*	bitmap32;
	CBitmapFixture*	bitmap32_changed;
};
