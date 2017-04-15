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
	assertIntEqual(*testInt, 5);
	assertIntNotEqual(*testInt, 0);
}

void testConversions()
{
	UNWANTED_TYPE(testInt, Null)
	UNWANTED_TYPE(testInt, Bool)
	WANTED_TYPE(assertIntEqual(testInt->asInt(), 5))
	UNWANTED_TYPE(testInt, Float)
	UNWANTED_TYPE(testInt, String)
	UNWANTED_TYPE(testInt, Object)
	UNWANTED_TYPE(testInt, Array)
}

void testSet()
{
	assertIntEqual(*testInt, 5);
	testInt->set(16384);
	assertIntEqual(*testInt, 16384);
}

void testDistruct()
{
	delete testInt;
	testInt = NULL;
}

#ifdef __cplusplus
extern "C"
{
#endif

BEGIN_REGISTER_TESTS()
	TEST(testConstruct)
	TEST(testOperatorInt)
	TEST(testConversions)
	TEST(testSet)
	TEST(testDistruct)
END_REGISTER_TESTS()

#ifdef __cplusplus
}
#endif
