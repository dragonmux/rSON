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

typedef struct JSONParser
{
private:
	const char *json;
	const char *next;
	const char *jsonEnd;
	bool lastWasComma;

public:
	JSONParser(const char *toParse);
	void nextChar();
	void skipWhite();
	void match(char x, bool skip);
	bool lastTokenComma();
	void lastNoComma();
	char currentChar();
	char *literal();
	char *string();
	size_t number();
} JSONParser;

extern JSONAtom *object(JSONParser *parser);
extern JSONAtom *array(JSONParser *parser);
extern size_t power10(size_t power);
extern JSONAtom *literal(JSONParser *parser);
extern JSONAtom *number(JSONParser *parser);
extern JSONAtom *expression(JSONParser *parser, bool matchComma);

void testParserViability()
{
	JSONParser *parser = new JSONParser("");
	delete parser;
}

void testPower10()
{
	assertIntEqual(power10(0), 1);
	assertIntEqual(power10(2), 100);
	assertIntEqual(power10(4), 10000);
}

#define TRY(tests) \
try \
{ \
	atom = literal(parser); \
	assertNotNull(atom); \
	tests; \
	delete atom; \
} \
catch (JSONParserError &err) \
{ \
	delete parser; \
	fail(err.error()); \
} \
catch (JSONTypeError &err) \
{ \
	delete atom; \
	delete parser; \
	fail(err.error()); \
}

void testLiteral()
{
	JSONParser *parser;
	JSONAtom *atom;

	parser = new JSONParser("true ");
	TRY(assertTrue(atom->asBool()));
	delete parser;

	parser = new JSONParser("false ");
	TRY(assertFalse(atom->asBool()));
	delete parser;

	parser = new JSONParser("null ");
	TRY(assertNull(atom->asNull()));
	delete parser;

	parser = new JSONParser("invalid ");
	try
	{
		atom = literal(parser);
		delete atom;
		delete parser;
		fail("The parser failed to throw an exception on invalid literal");
	}
	catch (JSONParserError &err)
	{
	}
	delete parser;
}

#undef TRY
#define TRY(tests) \
try \
{ \
	atom = number(parser); \
	assertNotNull(atom); \
	tests; \
	delete atom; \
} \
catch (JSONParserError &err) \
{ \
	delete parser; \
	fail(err.error()); \
} \
catch (JSONTypeError &err) \
{ \
	delete atom; \
	delete parser; \
	fail(err.error()); \
}

#define TRY_SHOULD_FAIL() \
try \
{ \
	atom = number(parser); \
	delete atom; \
	delete parser; \
	fail("The parser failed to throw an exception on invalid number"); \
} \
catch (JSONParserError &err) \
{ \
}

void testIntNumber()
{
	JSONParser *parser;
	JSONAtom *atom;

	parser = new JSONParser("0 ");
	TRY(assertIntEqual(atom->asInt(), 0));
	delete parser;

	parser = new JSONParser("190 ");
	TRY(assertIntEqual(atom->asInt(), 190));
	delete parser;

	parser = new JSONParser("-190 ");
	TRY(assertIntEqual(atom->asInt(), -190));
	delete parser;

	parser = new JSONParser("-0 ");
	TRY(assertIntEqual(atom->asInt(), -0));
	delete parser;

	parser = new JSONParser("19e1 ");
	TRY(assertIntEqual(atom->asInt(), 190));
	delete parser;

	parser = new JSONParser("190e-1 ");
	TRY(assertIntEqual(atom->asInt(), 19));
	delete parser;

	parser = new JSONParser("00 ");
	TRY_SHOULD_FAIL();
	delete parser;

	parser = new JSONParser("0e00 ");
	TRY_SHOULD_FAIL();
	delete parser;
}

void testFloatNumber()
{
	JSONParser *parser;
	JSONAtom *atom;

	parser = new JSONParser("0.0 ");
	TRY(assertDoubleEqual(atom->asFloat(), 0.0));
	delete parser;

	parser = new JSONParser("190.0 ");
	TRY(assertDoubleEqual(atom->asFloat(), 190.0));
	delete parser;

	parser = new JSONParser("-0.0 ");
	TRY(assertDoubleEqual(atom->asFloat(), -0.0));
	delete parser;

	parser = new JSONParser("-190.0 ");
	TRY(assertDoubleEqual(atom->asFloat(), -190.0));
	delete parser;

	parser = new JSONParser("19.0e1 ");
	TRY(assertDoubleEqual(atom->asFloat(), 190.0));
	delete parser;

	parser = new JSONParser("19.0e-1 ");
	TRY(assertDoubleEqual(atom->asFloat(), 1.9));
	delete parser;

	parser = new JSONParser("19.0e+0 ");
	TRY(assertDoubleEqual(atom->asFloat(), 19.0));
	delete parser;

	parser = new JSONParser("0.00 ");
	TRY(assertDoubleEqual(atom->asFloat(), 0.0));
	delete parser;

	parser = new JSONParser("0.0015 ");
	TRY(assertDoubleEqual(atom->asFloat(), 0.0015));
	delete parser;

	parser = new JSONParser("0.00150 ");
	TRY(assertDoubleEqual(atom->asFloat(), 0.0015));
	delete parser;

	parser = new JSONParser("00.0 ");
	TRY_SHOULD_FAIL();
	delete parser;

	parser = new JSONParser("0.0e00 ");
	TRY_SHOULD_FAIL();
	delete parser;
}

#undef TRY_SHOULD_FAIL
#undef TRY
#define TRY(tests) \
try \
{ \
	atom = parser->string(); \
	assertNotNull(atom); \
	tests; \
	delete [] atom; \
} \
catch (JSONParserError &err) \
{ \
	delete parser; \
	fail(err.error()); \
} \
catch (JSONTypeError &err) \
{ \
	delete [] atom; \
	delete parser; \
	fail(err.error()); \
}

#define TRY_SHOULD_FAIL() \
try \
{ \
	atom = parser->string(); \
	delete [] atom; \
	delete parser; \
	fail("The parser failed to throw an exception on invalid string"); \
} \
catch (JSONParserError &err) \
{ \
}

void testString()
{
	JSONParser *parser;
	char *atom;

	parser = new JSONParser("\"test\" ");
	TRY(assertStringEqual(atom, "test"));
	delete parser;

	parser = new JSONParser("\"\\\\\" ");
	TRY(assertStringEqual(atom, "\\\\"));
	delete parser;

	parser = new JSONParser("\" \" ");
	TRY(assertStringEqual(atom, " "));
	delete parser;

	parser = new JSONParser("\"\\\"\" ");
	TRY(assertStringEqual(atom, "\\\""));
	delete parser;

	parser = new JSONParser("\"te\\nst\" ");
	TRY(assertStringEqual(atom, "te\\nst"));
	delete parser;

	parser = new JSONParser("\" ");
	TRY_SHOULD_FAIL();
	delete parser;

	parser = new JSONParser("\"\\ \" ");
	TRY_SHOULD_FAIL();
	delete parser;

	parser = new JSONParser("\"\n\" ");
	TRY_SHOULD_FAIL();
	delete parser;
}

#undef TRY_SHOULD_FAIL
#undef TRY
#define TRY(tests) \
try \
{ \
	atom = object(parser); \
	assertNotNull(atom); \
	objectAtom = atom->asObject(); \
	tests; \
	delete atom; \
} \
catch (JSONParserError &err) \
{ \
	delete parser; \
	fail(err.error()); \
} \
catch (JSONTypeError &err) \
{ \
	delete atom; \
	delete parser; \
	fail(err.error()); \
}

#define TRY_SHOULD_FAIL() \
try \
{ \
	atom = object(parser); \
	delete atom; \
	delete parser; \
	fail("The parser failed to throw an exception on invalid object"); \
} \
catch (JSONParserError &err) \
{ \
}

void testObject()
{
	JSONParser *parser;
	JSONAtom *atom;
	JSONObject *objectAtom;

	parser = new JSONParser("{}");
	TRY(assertIntEqual(objectAtom->size(), 0));
	delete parser;

	parser = new JSONParser("{\"testKey\": 0}");
	TRY(
		assertIntEqual(objectAtom->size(), 1);
		assertTrue(objectAtom->exists("testKey"));
		assertIntEqual((*objectAtom)["testKey"]->asInt(), 0)
	);
	delete parser;

	parser = new JSONParser("{\"testInt\": 0, \"testBool\": true}");
	TRY(
		assertIntEqual(objectAtom->size(), 2);
		assertTrue(objectAtom->exists("testInt"));
		assertIntEqual((*objectAtom)["testInt"]->asInt(), 0);
		assertTrue(objectAtom->exists("testBool"));
		assertTrue((*objectAtom)["testBool"]->asBool())
	);
	delete parser;

	parser = new JSONParser("{true}");
	TRY_SHOULD_FAIL();
	delete parser;

	parser = new JSONParser("{true: 0}");
	TRY_SHOULD_FAIL();
	delete parser;

	parser = new JSONParser("{");
	TRY_SHOULD_FAIL();
	delete parser;

	parser = new JSONParser("{\"key\"}");
	TRY_SHOULD_FAIL();
	delete parser;

	parser = new JSONParser("{\"key\": }");
	TRY_SHOULD_FAIL();
	delete parser;

	parser = new JSONParser("{\"key\": ,}");
	TRY_SHOULD_FAIL();
	delete parser;

	parser = new JSONParser("{\"key\": junk}");
	TRY_SHOULD_FAIL();
	delete parser;

	parser = new JSONParser("{\"key\": null, }");
	TRY_SHOULD_FAIL();
	delete parser;
}

#undef TRY_SHOULD_FAIL
#undef TRY
#define TRY(tests) \
try \
{ \
	atom = array(parser); \
	assertNotNull(atom); \
	arrayAtom = atom->asArray(); \
	tests; \
	delete atom; \
} \
catch (JSONParserError &err) \
{ \
	delete parser; \
	fail(err.error()); \
} \
catch (JSONTypeError &err) \
{ \
	delete atom; \
	delete parser; \
	fail(err.error()); \
}

#define TRY_SHOULD_FAIL() \
try \
{ \
	atom = array(parser); \
	delete atom; \
	delete parser; \
	fail("The parser failed to throw an exception on invalid array"); \
} \
catch (JSONParserError &err) \
{ \
}

void testArray()
{
	JSONParser *parser;
	JSONAtom *atom;
	JSONArray *arrayAtom;

	parser = new JSONParser("[]");
	TRY(assertIntEqual(arrayAtom->size(), 0));
	delete parser;

	parser = new JSONParser("[0]");
	TRY(
		assertIntEqual(arrayAtom->size(), 1);
		assertNotNull((*arrayAtom)[0]);
		assertIntEqual((*arrayAtom)[0]->asInt(), 0)
	);
	delete parser;

	parser = new JSONParser("[0, true]");
	TRY(
		assertIntEqual(arrayAtom->size(), 2);
		assertNotNull((*arrayAtom)[0]);
		assertIntEqual((*arrayAtom)[0]->asInt(), 0);
		assertNotNull((*arrayAtom)[1]);
		assertTrue((*arrayAtom)[1]->asBool())
	);
	delete parser;

	parser = new JSONParser("[");
	TRY_SHOULD_FAIL();
	delete parser;

	parser = new JSONParser("[,]");
	TRY_SHOULD_FAIL();
	delete parser;

	parser = new JSONParser("[, true]");
	TRY_SHOULD_FAIL();
	delete parser;

	parser = new JSONParser("[null, ]");
	TRY_SHOULD_FAIL();
	delete parser;

	parser = new JSONParser("[null, ");
	TRY_SHOULD_FAIL();
	delete parser;
}

#undef TRY_SHOULD_FAIL
#undef TRY
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
		assertIntEqual((*object)["testInt"]->asInt(), 0);
		assertTrue(object->exists("testArray"));
		array = (*object)["testArray"]->asArray();
		assertIntEqual(array->size(), 3);
		assertNotNull((*array)[0]);
		assertNotNull((*array)[1]);
		assertNotNull((*array)[2]);
		assertNull((*array)[0]->asNull());
		assertTrue((*array)[1]->asBool());
		assertFalse((*array)[2]->asBool())
	);

	TRY("[\n\t0,\n\t\[\n\t\tnull,\n\t\ttrue,\n\t\tfalse\n\t]\n]",
		assertNotNull(atom);
		array = atom->asArray();
		assertIntEqual(array->size(), 2);
		assertNotNull((*array)[0]);
		assertIntEqual((*array)[0]->asInt(), 0);
		assertNotNull((*array)[1]);
		innerArray = (*array)[1]->asArray();
		assertIntEqual(innerArray->size(), 3);
		assertNotNull((*innerArray)[0]);
		assertNotNull((*innerArray)[1]);
		assertNotNull((*innerArray)[2]);
		assertNull((*innerArray)[0]->asNull());
		assertTrue((*innerArray)[1]->asBool());
		assertFalse((*innerArray)[2]->asBool())
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

#undef TRY_SHOULD_FAIL
#undef TRY

#ifdef __cplusplus
extern "C"
{
#endif

BEGIN_REGISTER_TESTS()
	TEST(testParserViability)
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
