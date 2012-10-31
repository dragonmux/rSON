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

#ifdef __cplusplus
extern "C"
{
#endif

BEGIN_REGISTER_TESTS()
	TEST(testConstruct)
	TEST(testConversions)
	TEST(testDistruct)
END_REGISTER_TESTS()

#ifdef __cplusplus
}
#endif
