/*
 * This file is part of rSON
 * Copyright © 2012-2013 Rachel Mant (dx-mon@users.sourceforge.net)
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

#ifdef __cplusplus
extern "C"
{
#endif

BEGIN_REGISTER_TESTS()
	TEST(testConstructTrue)
	TEST(testOperatorBoolTrue)
	TEST(testConversions)
	TEST(testDistruct)
	TEST(testConstructFalse)
	TEST(testOperatorBoolFalse)
	TEST(testDistruct)
END_REGISTER_TESTS()

#ifdef __cplusplus
}
#endif
