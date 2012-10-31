/*
 * This file is part of rSON
 * Copyright © 2012 Richard/Rachel Mant (dx-mon@users.sourceforge.net)
 *
 * rSON is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * rSON is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "test.h"
#include <string.h>

JSONString *testString = NULL;
static const char *constTestValue = "testValue";

void testConstruct()
{
	char *testValue = new char[strlen(constTestValue) + 1]();
	strcpy(testValue, constTestValue);
	try
	{
		testString = new JSONString(testValue);
	}
	catch (std::bad_alloc &badAlloc)
	{
		delete [] testValue;
		fail(badAlloc.what());
	}
	assertNotNull(testString);
}

void testOperatorString()
{
	assertNotNull(testString);
	assertConstNotNull(*testString);
	assertStringEqual(*testString, "testValue");
}

void testConversions()
{
	UNWANTED_TYPE(testString, Null)
	UNWANTED_TYPE(testString, Bool)
	UNWANTED_TYPE(testString, Int)
	UNWANTED_TYPE(testString, Float)
	WANTED_TYPE( \
		assertConstNotNull(testString->asString()); \
		assertStringEqual(testString->asString(), "testValue")
	)
	UNWANTED_TYPE(testString, Object)
	UNWANTED_TYPE(testString, Array)
}

void testDistruct()
{
	delete testString;
	testString = NULL;
}

#ifdef __cplusplus
extern "C"
{
#endif

BEGIN_REGISTER_TESTS()
	TEST(testConstruct)
	TEST(testOperatorString)
	TEST(testConversions)
	TEST(testDistruct)
END_REGISTER_TESTS()

#ifdef __cplusplus
}
#endif
