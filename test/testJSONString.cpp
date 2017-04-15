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

#define TRY(seq, tests) \
try \
{ \
	str = new char[sizeof(seq)]; \
	strcpy(str, seq); \
	string = new JSONString(str); \
	assertNotNull(string); \
	str = (char *)string->operator const char *(); \
	tests; \
} \
catch (JSONParserError &err) \
{ \
	delete string; \
	fail(err.error()); \
} \
delete string

#define TRY_SHOULD_FAIL(seq) \
try \
{ \
	str = new char[sizeof(seq)]; \
	strcpy(str, seq); \
	string = new JSONString(str); \
	delete string; \
	fail("JSONString failed to throw an exception on invalid string"); \
} \
catch (JSONParserError &err) \
{ \
} \
delete [] str;

void testEscapes()
{
	JSONString *string;
	char *str;

	TRY("\\\"", assertStringEqual(str, "\""));
	TRY("\\t\\r\\n", assertStringEqual(str, "\t\r\n"));
	TRY("\\b", assertStringEqual(str, "\x08"));
	TRY("\\f", assertStringEqual(str, "\x0C"));
	TRY("\\/", assertStringEqual(str, "/"));
	TRY("\\\\", assertStringEqual(str, "\\"));
	TRY("\\u0050", assertStringEqual(str, "\x50"));
	TRY("\\u0000", assertStringEqual(str, "\xC0\x80"));
	TRY("\\u0155", assertStringEqual(str, "\xC5\x95"));
	TRY("\\u5555", assertStringEqual(str, "\xE5\x95\x95"));
	TRY("\\u5A5A", assertStringEqual(str, "\xE5\xA9\x9A"));
	TRY_SHOULD_FAIL("\\u5A5Q");
}

#undef TRY_SHOULD_FAIL
#undef TRY

void testOperatorString()
{
	assertNotNull(testString);
	assertConstNotNull(static_cast<const char *>(*testString));
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
	TEST(testEscapes)
	TEST(testOperatorString)
	TEST(testConversions)
	TEST(testDistruct)
END_REGISTER_TESTS()

#ifdef __cplusplus
}
#endif
