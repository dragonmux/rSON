// SPDX-License-Identifier: LGPL-3.0-or-later
// SPDX-FileCopyrightText: 2012-2013,2017-2020,2023 Rachel Mant <git@dragonmux.network>
// SPDX-FileContributor: Written by Rachel Mant <git@dragonmux.network>

#include "test.h"

JSONNull *testNull = NULL;

void testConstruct()
{
	try
	{
		testNull = new JSONNull();
	}
	catch (std::bad_alloc &badAlloc)
	{
		fail(badAlloc.what());
	}
	assertNotNull(testNull);
}

void testConversions()
{
	WANTED_TYPE(assertNull(testNull->asNull()))
	UNWANTED_TYPE(testNull, Bool)
	UNWANTED_TYPE(testNull, Int)
	UNWANTED_TYPE(testNull, Float)
	UNWANTED_TYPE(testNull, String)
	UNWANTED_TYPE(testNull, Object)
	UNWANTED_TYPE(testNull, Array)
}

void testDistruct()
{
	delete testNull;
	testNull = NULL;
}

extern "C"
{
BEGIN_REGISTER_TESTS()
	TEST(testConstruct)
	TEST(testConversions)
	TEST(testDistruct)
END_REGISTER_TESTS()
}
