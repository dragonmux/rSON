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

#include <string>
#include <string_view>
#include "test.h"
#include "internal/string.hxx"
#include <string.h>

using namespace std::literals::string_literals;
using namespace std::literals::string_view_literals;

JSONString *testString = NULL;
static const auto testValue{"testValue"sv};

void testConstruct()
{
	try
		{ testString = new JSONString(testValue); }
	catch (std::bad_alloc &badAlloc)
		{ fail(badAlloc.what()); }
	assertNotNull(testString);
	assertStringEqual(testString->get().data(), testValue.data());
	assertIntEqual(testString->len(), testValue.length());
}

#define TRY(seq, tests) \
try \
{ \
	string = new JSONString(seq); \
	assertNotNull(string); \
	const char *str = *string; \
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
	string = new JSONString(seq); \
	delete string; \
	fail("JSONString failed to throw an exception on invalid string"); \
} \
catch (JSONParserError &err) \
{ \
}

void testEscapes()
{
	JSONString *string;

	TRY("\\\""s, assertStringEqual(str, "\""));
	TRY("\\t\\r\\n"s, assertStringEqual(str, "\t\r\n"));
	TRY("\\b"s, assertStringEqual(str, "\x08"));
	TRY("\\f"s, assertStringEqual(str, "\x0C"));
	TRY("\\/"s, assertStringEqual(str, "/"));
	TRY("\\\\"s, assertStringEqual(str, "\\"));
	TRY("\\u0050"s, assertStringEqual(str, "\x50"));
	TRY("\\u0000"s, assertStringEqual(str, "\xC0\x80"));
	TRY("\\u0155"s, assertStringEqual(str, "\xC5\x95"));
	TRY("\\u5555"s, assertStringEqual(str, "\xE5\x95\x95"));
	TRY("\\u5A5A"s, assertStringEqual(str, "\xE5\xA9\x9A"));
	TRY_SHOULD_FAIL("\\u5A5Q"s);
	TRY_SHOULD_FAIL("\\u%A5A"s);
	TRY_SHOULD_FAIL("\\u5=5A"s);
	TRY_SHOULD_FAIL("\\u5A^A"s);
}

#undef TRY_SHOULD_FAIL
#undef TRY

void testOperatorString()
{
	assertNotNull(testString);
	assertNotNull(static_cast<const char *>(*testString));
	assertStringEqual(*testString, "testValue");
}

void testConversions()
{
	assertNotNull(testString);
	UNWANTED_TYPE(testString, Null)
	UNWANTED_TYPE(testString, Bool)
	UNWANTED_TYPE(testString, Int)
	UNWANTED_TYPE(testString, Float)
	WANTED_TYPE( \
		assertNotNull(testString->asString()); \
		assertStringEqual(testString->asString(), "testValue");
		JSONAtom &atom = *testString;
		JSONString &str = atom;
		assertPtrEqual(&str, testString)
	)
	UNWANTED_TYPE(testString, Object)
	UNWANTED_TYPE(testString, Array)
}

void testSet()
{
	const char *const newStr = "This is only a test";
	assertNotNull(testString);
	testString->set(strNewDup(newStr));
	assertStringEqual(*testString, newStr);
	assertIntEqual(testString->len(), strlen(newStr));
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
	TEST(testSet)
	TEST(testDistruct)
END_REGISTER_TESTS()

#ifdef __cplusplus
}
#endif
