// SPDX-License-Identifier: LGPL-3.0-or-later
// SPDX-FileCopyrightText: 2012-2013,2017,2023 Rachel Mant <git@dragonmux.network>
// SPDX-FileContributor: Written by Rachel Mant <git@dragonmux.network>

#include "test.h"

JSONInt *testInt = NULL;

void testConstruct()
{
	try
	{
		testInt = new JSONInt(5);
	}
	catch (std::bad_alloc &badAlloc)
	{
		fail(badAlloc.what());
	}
	assertNotNull(testInt);
}

void testOperatorInt()
{
	assertNotNull(testInt);
	assertInt64Equal(*testInt, 5);
	assertInt64NotEqual(*testInt, 0);
}

void testConversions()
{
	UNWANTED_TYPE(testInt, Null)
	UNWANTED_TYPE(testInt, Bool)
	WANTED_TYPE(assertInt64Equal(testInt->asInt(), 5))
	UNWANTED_TYPE(testInt, Float)
	UNWANTED_TYPE(testInt, String)
	UNWANTED_TYPE(testInt, Object)
	UNWANTED_TYPE(testInt, Array)
}

void testSet()
{
	assertInt64Equal(*testInt, 5);
	testInt->set(16384);
	assertInt64Equal(*testInt, 16384);
}

void testDistruct()
{
	delete testInt;
	testInt = NULL;
}

extern "C"
{
BEGIN_REGISTER_TESTS()
	TEST(testConstruct)
	TEST(testOperatorInt)
	TEST(testConversions)
	TEST(testSet)
	TEST(testDistruct)
END_REGISTER_TESTS()
}
