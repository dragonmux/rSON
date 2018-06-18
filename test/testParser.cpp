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
#include "../Parser.h"

void testParserViability()
{
	const char *const json = "[]";
	memoryStream_t stream(const_cast<char *const>(json), length(json));
	JSONParser parser(stream);
	assertTrue(parser.currentChar() == '[');
	assertFalse(stream.atEOF());
}

void tryViabilityFail(JSONParser &parser, void test(JSONParser &))
{
	try
	{
		test(parser);
		fail("Parser failed to throw EOF when it should");
	}
	catch (const JSONParserError &err) { }
}

void testStreamViability()
{
	const char *const json = "[]";
	memoryStream_t stream(const_cast<char *const>(json), length(json) - 1);
	JSONParser parser(stream);
	assertTrue(parser.currentChar() == '[');
	parser.nextChar();
	tryViabilityFail(parser, [](JSONParser &parser) { parser.currentChar(); });
	tryViabilityFail(parser, [](JSONParser &parser) { parser.nextChar(); });
}

void testPower10()
{
	assertIntEqual(power10(0), 1);
	assertIntEqual(power10(2), 100);
	assertIntEqual(power10(4), 10000);
}

void tryLiteralOk(const char *const json, void tests(const JSONAtom &))
{
	try
	{
		memoryStream_t stream(const_cast<char *const>(json), length(json));
		JSONParser parser(stream);
		std::unique_ptr<JSONAtom> atom(literal(parser));
		assertNotNull(atom.get());
		tests(*atom);
	}
	catch (const JSONParserError &err)
		{ fail(err.error()); }
	catch (const JSONTypeError &err)
		{ fail(err.error()); }
	catch (const std::bad_alloc &err)
		{ fail(err.what()); }
}

void tryLiteralFail(const char *const json)
{
	memoryStream_t stream(const_cast<char *const>(json), length(json));
	JSONParser parser(stream);
	try
	{
		std::unique_ptr<JSONAtom> atom(literal(parser));
		fail("The parser failed to throw an exception on invalid literal");
	}
	catch (const JSONParserError &err) { }
}

void testLiteral()
{
	tryLiteralOk("true ", [](const JSONAtom &atom) { assertTrue(atom.asBool()); });
	tryLiteralOk("false ", [](const JSONAtom &atom) { assertFalse(atom.asBool()); });
	tryLiteralOk("null ", [](const JSONAtom &atom) { assertNull(atom.asNull()); });

	tryLiteralFail("invalid ");
	tryLiteralFail("a ");
	tryLiteralFail("0 ");
}

void tryNumberOk(const char *const json, void tests(const JSONAtom &))
{
	try
	{
		memoryStream_t stream(const_cast<char *const>(json), length(json));
		JSONParser parser(stream);
		std::unique_ptr<JSONAtom> atom(number(parser));
		assertNotNull(atom.get());
		tests(*atom);
	}
	catch (const JSONParserError &err)
		{ fail(err.error()); }
	catch (const JSONTypeError &err)
		{ fail(err.error()); }
	catch (const std::bad_alloc &err)
		{ fail(err.what()); }
}

void tryNumberFail(const char *const json)
{
	try
	{
		memoryStream_t stream(const_cast<char *const>(json), length(json));
		JSONParser parser(stream);
		std::unique_ptr<JSONAtom> atom(number(parser));
		fail("The parser failed to throw an exception on invalid number");
	}
	catch (const JSONParserError &err) { }
	catch (const std::bad_alloc &err)
		{ fail(err.what()); }
}

void testIntNumber()
{
	tryNumberOk("0 ", [](const JSONAtom &atom) { assertIntEqual(atom.asInt(), 0); });
	tryNumberOk("190 ", [](const JSONAtom &atom) { assertIntEqual(atom.asInt(), 190); });
	tryNumberOk("-190 ", [](const JSONAtom &atom) { assertIntEqual(atom.asInt(), -190); });
	tryNumberOk("-0 ", [](const JSONAtom &atom) { assertIntEqual(atom.asInt(), -0); });
	tryNumberOk("19e1 ", [](const JSONAtom &atom) { assertIntEqual(atom.asInt(), 190); });
	tryNumberOk("190e-1 ", [](const JSONAtom &atom) { assertIntEqual(atom.asInt(), 19); });

	tryNumberFail("");
	tryNumberFail("-");
	tryNumberFail("0");
	tryNumberFail("00");
	tryNumberFail("12");
	tryNumberFail("00 ");
	tryNumberFail("0.0");
	tryNumberFail("0e00 ");
	tryNumberFail("0.0e");
	tryNumberFail("0e-");
}

void testFloatNumber()
{
	tryNumberOk("0.0 ", [](const JSONAtom &atom) { assertDoubleEqual(atom.asFloat(), 0.0); });
	tryNumberOk("190.0 ", [](const JSONAtom &atom) { assertDoubleEqual(atom.asFloat(), 190.0); });
	tryNumberOk("-0.0 ", [](const JSONAtom &atom) { assertDoubleEqual(atom.asFloat(), -0.0); });
	tryNumberOk("-190.0 ", [](const JSONAtom &atom) { assertDoubleEqual(atom.asFloat(), -190.0); });
	tryNumberOk("19.0e1 ", [](const JSONAtom &atom) { assertDoubleEqual(atom.asFloat(), 190.0); });
	tryNumberOk("19.0e-1 ", [](const JSONAtom &atom) { assertDoubleEqual(atom.asFloat(), 1.9); });
	tryNumberOk("19.0e+0 ", [](const JSONAtom &atom) { assertDoubleEqual(atom.asFloat(), 19.0); });
	tryNumberOk("0.00 ", [](const JSONAtom &atom) { assertDoubleEqual(atom.asFloat(), 0.0); });
	tryNumberOk("0.0015 ", [](const JSONAtom &atom) { assertDoubleEqual(atom.asFloat(), 0.0015); });
	tryNumberOk("0.00150 ", [](const JSONAtom &atom) { assertDoubleEqual(atom.asFloat(), 0.0015); });

	tryNumberFail("00.0 ");
	tryNumberFail("0.0e00 ");
	tryNumberFail("a ");
}

void tryStringOk(const char *const json, void tests(const char *const))
{
	try
	{
		memoryStream_t stream(const_cast<char *const>(json), length(json));
		JSONParser parser(stream);
		std::unique_ptr<char []> atom(parser.string());
		assertNotNull(atom.get());
		tests(atom.get());
	}
	catch (const JSONParserError &err)
		{ fail(err.error()); }
	catch (const JSONTypeError &err)
		{ fail(err.error()); }
	catch (const std::bad_alloc &err)
		{ fail(err.what()); }
}

void tryStringFail(const char *const json)
{
	try
	{
		memoryStream_t stream(const_cast<char *const>(json), length(json));
		JSONParser parser(stream);
		std::unique_ptr<char []> atom(parser.string());
		fail("The parser failed to throw an exception on invalid string");
	}
	catch (const JSONParserError &err) { }
	catch (const std::bad_alloc &err)
		{ fail(err.what()); }
}

void testString()
{
	JSONParser *parser;

	tryStringOk("\"test\" ", [](const char *const atom) { assertStringEqual(atom, "test"); });
	tryStringOk("\"\\\\\" ", [](const char *const atom) { assertStringEqual(atom, "\\\\"); });
	tryStringOk("\" \" ", [](const char *const atom) { assertStringEqual(atom, " "); });
	tryStringOk("\"\\\"\" ", [](const char *const atom) { assertStringEqual(atom, "\\\""); });
	tryStringOk("\"te\\nst\" ", [](const char *const atom) { assertStringEqual(atom, "te\\nst"); });
	tryStringOk("\"\\u2200\" ", [](const char *const atom) { assertStringEqual(atom, "\\u2200"); });
	tryStringOk("\"\\u222B\" ", [](const char *const atom) { assertStringEqual(atom, "\\u222B"); });

	tryStringFail("\" ");
	tryStringFail("\"\\ \" ");
	tryStringFail("\"\n\" ");
	tryStringFail("\"\\u\" ");
	tryStringFail("\"\\u22g0\" ");
}

void tryObjectOk(const char *const json, void tests(const JSONObject &))
{
	try
	{
		memoryStream_t stream(const_cast<char *const>(json), length(json));
		JSONParser parser(stream);
		std::unique_ptr<JSONAtom> atom(object(parser));
		assertNotNull(atom.get());
		tests(atom->asObjectRef());
	}
	catch (const JSONParserError &err)
		{ fail(err.error()); }
	catch (const JSONTypeError &err)
		{ fail(err.error()); }
	catch (const std::bad_alloc &err)
		{ fail(err.what()); }
}

void tryObjectFail(const char *const json)
{
	try
	{
		memoryStream_t stream(const_cast<char *const>(json), length(json));
		JSONParser parser(stream);
		std::unique_ptr<JSONAtom> atom(object(parser));
		fail("The parser failed to throw an exception on invalid object");
	}
	catch (const JSONParserError &err) { }
	catch (const std::bad_alloc &err)
		{ fail(err.what()); }
}

void testObject()
{
	JSONParser *parser;
	JSONAtom *atom;
	JSONObject *objectAtom;

	tryObjectOk("{}", [](const JSONObject &objectAtom) { assertIntEqual(objectAtom.size(), 0); });

	tryObjectOk("{\"testKey\": 0}", [](const JSONObject &objectAtom)
	{
		assertIntEqual(objectAtom.size(), 1);
		assertTrue(objectAtom.exists("testKey"));
		assertIntEqual(objectAtom["testKey"].asInt(), 0);
	});

	tryObjectOk("{\"testInt\": 0, \"testBool\": true, \"testFloat\": -0.4}", [](const JSONObject &objectAtom)
	{
		assertIntEqual(objectAtom.size(), 3);
		assertTrue(objectAtom.exists("testInt"));
		assertIntEqual(objectAtom["testInt"].asInt(), 0);
		assertTrue(objectAtom.exists("testBool"));
		assertTrue(objectAtom["testBool"].asBool());
		assertTrue(objectAtom.exists("testFloat"));
		assertDoubleEqual(objectAtom["testFloat"].asFloat(), -0.4);
	});

	tryObjectFail("{true}");
	tryObjectFail("{true: 0}");
	tryObjectFail("{");
	tryObjectFail("{\"key\"}");
	tryObjectFail("{\"key\": }");
	tryObjectFail("{\"key\": ,}");
	tryObjectFail("{\"key\": junk}");
	tryObjectFail("{\"key\": null, }");
}

void tryArrayOk(const char *const json, void tests(const JSONArray &))
{
	try
	{
		memoryStream_t stream(const_cast<char *const>(json), length(json));
		JSONParser parser(stream);
		std::unique_ptr<JSONAtom> atom(array(parser));
		assertNotNull(atom.get());
		tests(atom->asArrayRef());
	}
	catch (const JSONParserError &err)
		{ fail(err.error()); }
	catch (const JSONTypeError &err)
		{ fail(err.error()); }
	catch (const std::bad_alloc &err)
		{ fail(err.what()); }
}

void tryArrayFail(const char *const json)
{
	try
	{
		memoryStream_t stream(const_cast<char *const>(json), length(json));
		JSONParser parser(stream);
		std::unique_ptr<JSONAtom> atom(array(parser));
		fail("The parser failed to throw an exception on invalid array");
	}
	catch (const JSONParserError &err) { }
	catch (const std::bad_alloc &err)
		{ fail(err.what()); }
}

void testArray()
{
	JSONParser *parser;
	JSONAtom *atom;
	JSONArray *arrayAtom;

	tryArrayOk("[]", [](const JSONArray &arrayAtom) { assertIntEqual(arrayAtom.size(), 0); });

	tryArrayOk("[0]", [](const JSONArray &arrayAtom)
	{
		assertIntEqual(arrayAtom.size(), 1);
		assertNotNull(arrayAtom[0]);
		assertIntEqual(arrayAtom[0].asInt(), 0);
	});

	tryArrayOk("[0, true]", [](const JSONArray &arrayAtom)
	{
		assertIntEqual(arrayAtom.size(), 2);
		assertNotNull(arrayAtom[0]);
		assertIntEqual(arrayAtom[0].asInt(), 0);
		assertNotNull(arrayAtom[1]);
		assertTrue(arrayAtom[1].asBool());
	});

	tryArrayFail("[");
	tryArrayFail("[,]");
	tryArrayFail("[, true]");
	tryArrayFail("[null, ]");
	tryArrayFail("[null, ");
}

#define TRY(testString, tests) \
try \
{ \
	atom = parseJSON(testString); \
	assertNotNull(atom); \
	tests; \
	delete atom; \
} \
catch (JSONParserError &err) \
{ \
	fail(err.error()); \
} \
catch (JSONTypeError &err) \
{ \
	delete atom; \
	fail(err.error()); \
}

#define TRY_SHOULD_FAIL(testString) \
try \
{ \
	atom = parseJSON(testString); \
	delete atom; \
	fail("The parser failed to throw an exception on invalid JSON"); \
} \
catch (JSONParserError &err) \
{ \
}

void testParseJSON()
{
	JSONAtom *atom;
	JSONObject *object;
	JSONArray *array, *innerArray;

	TRY("{\n\t\"testInt\": 0,\n\t\"testArray\": [\n\t\tnull,\n\t\ttrue,\n\t\tfalse\n\t]\n}",
		assertNotNull(atom);
		object = atom->asObject();
		assertIntEqual(object->size(), 2);
		assertTrue(object->exists("testInt"));
		assertIntEqual((*object)["testInt"].asInt(), 0);
		assertTrue(object->exists("testArray"));
		array = (*object)["testArray"].asArray();
		assertIntEqual(array->size(), 3);
		assertNotNull((*array)[0]);
		assertNotNull((*array)[1]);
		assertNotNull((*array)[2]);
		assertNull((*array)[0].asNull());
		assertTrue((*array)[1].asBool());
		assertFalse((*array)[2].asBool())
	);

	TRY("[\n\t0,\n\t\[\n\t\tnull,\n\t\ttrue,\n\t\tfalse\n\t]\n]",
		assertNotNull(atom);
		array = atom->asArray();
		assertIntEqual(array->size(), 2);
		assertNotNull((*array)[0]);
		assertIntEqual((*array)[0].asInt(), 0);
		assertNotNull((*array)[1]);
		innerArray = (*array)[1].asArray();
		assertIntEqual(innerArray->size(), 3);
		assertNotNull((*innerArray)[0]);
		assertNotNull((*innerArray)[1]);
		assertNotNull((*innerArray)[2]);
		assertNull((*innerArray)[0].asNull());
		assertTrue((*innerArray)[1].asBool());
		assertFalse((*innerArray)[2].asBool())
	);

	TRY("[\n\t0,\n\t1\n]",
		assertNotNull(atom);
		array = atom->asArray();
		assertIntEqual(array->size(), 2);
		assertIntEqual((*array)[0].asInt(), 0);
		assertIntEqual((*array)[1].asInt(), 1)
	);

	TRY_SHOULD_FAIL("true");
	TRY_SHOULD_FAIL("false");
	TRY_SHOULD_FAIL("null");
	TRY_SHOULD_FAIL("invalid");
	TRY_SHOULD_FAIL("0");
	TRY_SHOULD_FAIL("\"true\"");
}

#undef TRY
#undef TRY_SHOULD_FAIL
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#define TRY(testFile, tests) \
try \
{ \
	atom = parseJSONFile(testFile); \
	assertNotNull(atom); \
	tests; \
	delete atom; \
} \
catch (JSONParserError &err) \
{ \
	fail(err.error()); \
} \
catch (JSONTypeError &err) \
{ \
	delete atom; \
	fail(err.error()); \
}

#define TRY_SHOULD_FAIL(testFile) \
try \
{ \
	atom = parseJSONFile(testFile); \
	delete atom; \
	fail("The parser failed to throw an exception on invalid JSON"); \
} \
catch (JSONParserError &err) \
{ \
}

void testParseJSONFile()
{
	JSONAtom *atom;

	TRY_SHOULD_FAIL("nonExistant.json");
	fclose(fopen("test.json", "wb"));
	TRY_SHOULD_FAIL("test.json");
	unlink("test.json");
}

#pragma GCC diagnostic pop
#undef TRY_SHOULD_FAIL
#undef TRY

#ifdef __cplusplus
extern "C"
{
#endif

BEGIN_REGISTER_TESTS()
	TEST(testParserViability)
	TEST(testStreamViability)
	TEST(testPower10)
	TEST(testLiteral)
	TEST(testIntNumber)
	TEST(testFloatNumber)
	TEST(testString)
	TEST(testObject)
	TEST(testArray)
	TEST(testParseJSON)
	TEST(testParseJSONFile)
END_REGISTER_TESTS()

#ifdef __cplusplus
}
#endif
