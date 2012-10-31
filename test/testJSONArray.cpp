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

JSONArray *testArray = NULL;

void testConstruct()
{
	try
	{
		testArray = new JSONArray();
	}
	catch (std::bad_alloc &badAlloc)
	{
		fail(badAlloc.what());
	}
	assertNotNull(testArray);
}

void testConversions()
{
	assertNotNull(testArray);
	UNWANTED_TYPE(testArray, Null)
	UNWANTED_TYPE(testArray, Bool)
	UNWANTED_TYPE(testArray, Int)
	UNWANTED_TYPE(testArray, Float)
	UNWANTED_TYPE(testArray, String)
	UNWANTED_TYPE(testArray, Object)
	WANTED_TYPE( \
		assertNotNull(testArray->asArray()); \
		assertPtrEqual(testArray->asArray(), testArray))
}

void testSize()
{
	assertNotNull(testArray);
	assertIntEqual(testArray->size(), 0);
}

void testAdd()
{
	assertNotNull(testArray);
	assertIntEqual(testArray->size(), 0);

	testArray->add(new JSONInt(1));
	assertIntEqual(testArray->size(), 1);

	testArray->add(new JSONInt(2));
	assertIntEqual(testArray->size(), 2);
}

#define COMMON_CATCH(tryStuff) \
try \
{ \
	tryStuff; \
} \
catch (JSONTypeError &err) \
{ \
	fail(err.error()); \
}

#define CATCH_FAIL(tryStuff) \
COMMON_CATCH(tryStuff) \
catch (JSONArrayError &err) \
{ \
	fail(err.error()); \
}

void testLookup()
{
	JSONAtom *child;

	assertNotNull(testArray);
	assertIntEqual(testArray->size(), 2);

	CATCH_FAIL( \
		child = (*testArray)[0]; \
		assertNotNull(child); \
		assertIntEqual(child->asInt(), 1)
	)

	CATCH_FAIL( \
		child = (*testArray)[1]; \
		assertNotNull(child); \
		assertIntEqual(child->asInt(), 2)
	)

	COMMON_CATCH( \
		child = (*testArray)[2]; \
		fail("Array index out of bounds exception not thrown when it should have been!")
	)
	catch (JSONArrayError &err)
	{
	}
}

void testDistruct()
{
	delete testArray;
	testArray = NULL;
}

#ifdef __cplusplus
extern "C"
{
#endif

BEGIN_REGISTER_TESTS()
	TEST(testConstruct)
	TEST(testConversions)
	TEST(testSize)
	TEST(testAdd)
	TEST(testLookup)
	TEST(testDistruct)
END_REGISTER_TESTS()

#ifdef __cplusplus
}
#endif
