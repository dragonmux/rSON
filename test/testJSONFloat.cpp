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

JSONFloat *testFloat = NULL;

void testConstruct()
{
	try
	{
		testFloat = new JSONFloat(5.0);
	}
	catch (std::bad_alloc &badAlloc)
	{
		fail(badAlloc.what());
	}
	assertNotNull(testFloat);
}

void testOperatorDouble()
{
	assertNotNull(testFloat);
	assertDoubleEqual(*testFloat, 5.0);
	assertDoubleNotEqual(*testFloat, 0.0);
}

void testConversions()
{
	UNWANTED_TYPE(testFloat, Null)
	UNWANTED_TYPE(testFloat, Bool)
	UNWANTED_TYPE(testFloat, Int)
	WANTED_TYPE(testFloat->asFloat())
	WANTED_TYPE(assertDoubleEqual(testFloat->asFloat(), 5.0))
	UNWANTED_TYPE(testFloat, String)
	UNWANTED_TYPE(testFloat, Object)
	UNWANTED_TYPE(testFloat, Array)
}

void testDistruct()
{
	delete testFloat;
	testFloat = NULL;
}

#ifdef __cplusplus
extern "C"
{
#endif

BEGIN_REGISTER_TESTS()
	TEST(testConstruct)
	TEST(testOperatorDouble)
	TEST(testConversions)
	TEST(testDistruct)
END_REGISTER_TESTS()

#ifdef __cplusplus
}
#endif
