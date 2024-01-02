// SPDX-License-Identifier: LGPL-3.0-or-later
// SPDX-FileCopyrightText: 2012-2013,2016-2017,2023 Rachel Mant <git@dragonmux.network>
// SPDX-FileContributor: Written by Rachel Mant <git@dragonmux.network>

#include "test.h"

JSONBool *testBool = NULL;

void testConstructTrue()
{
	try
	{
		testBool = new JSONBool(true);
	}
	catch (std::bad_alloc &badAlloc)
	{
		fail(badAlloc.what());
	}
	assertNotNull(testBool);
}

void testOperatorBoolTrue()
{
	assertNotNull(testBool);
	assertTrue(bool(*testBool));
	assertTrue(testBool->asBool());
}

void testConstructFalse()
{
	try
	{
		testBool = new JSONBool(false);
	}
	catch (std::bad_alloc &badAlloc)
	{
		fail(badAlloc.what());
	}
	assertNotNull(testBool);
}

void testOperatorBoolFalse()
{
	assertNotNull(testBool);
	assertFalse(bool(*testBool));
	assertFalse(testBool->asBool());
}

void testDistruct()
{
	delete testBool;
	testBool = NULL;
}

void testConversions()
{
	UNWANTED_TYPE(testBool, Null)
	WANTED_TYPE(assertTrue(testBool->asBool()))
	UNWANTED_TYPE(testBool, Int)
	UNWANTED_TYPE(testBool, Float)
	UNWANTED_TYPE(testBool, String)
	UNWANTED_TYPE(testBool, Object)
	UNWANTED_TYPE(testBool, Array)
}

void testSet()
{
	assertNotNull(testBool);
	testBool->set(true);
	assertTrue(bool(*testBool));
	testBool->set(false);
	assertFalse(bool(*testBool));
}

extern "C"
{
BEGIN_REGISTER_TESTS()
	TEST(testConstructTrue)
	TEST(testOperatorBoolTrue)
	TEST(testConversions)
	TEST(testDistruct)
	TEST(testConstructFalse)
	TEST(testOperatorBoolFalse)
	TEST(testSet)
	TEST(testDistruct)
END_REGISTER_TESTS()
}
