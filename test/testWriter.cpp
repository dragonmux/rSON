/*
 * This file is part of rSON
 * Copyright © 2012-2017 Rachel Mant (dx-mon@users.sourceforge.net)
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

#include <fcntl.h>
#include "test.h"

#ifdef _MSVC
#define O_NOCTTY O_BINARY
#endif

char *strnew(const char *str)
{
	char *ret = new char[strlen(str) + 1];
	strcpy(ret, str);
	return ret;
}

void doTest(JSONAtom *atom, const char *result)
{
	char *json = writeJSON(atom);
	assertNotNull(json);
	assertStringEqual(json, result);
	freeString(&json);
	assertNull(json);
}

void testNull()
{
	JSONNull *null = new JSONNull();
	doTest(null, "null");
	delete null;
}

void testBool()
{
	JSONBool *bTrue = new JSONBool(true);
	JSONBool *bFalse = new JSONBool(false);
	doTest(bTrue, "true");
	doTest(bFalse, "false");
	delete bTrue;
	delete bFalse;
}

void testInt()
{
	JSONInt *num = new JSONInt(8192);
	doTest(num, "8192");
	delete num;
}

void testFloat()
{
	JSONFloat *num = new JSONFloat(8192.016384);
	doTest(num, "8192.016384");
	delete num;

	try
	{
		stream_t stream;
		JSONFloat(0).store(stream);
		fail("Store did not encounter an exception as it is supposed to");
	}
	catch (notImplemented_t &) { }
}

void testObject()
{
	char *key;
	JSONObject *obj;

	obj = new JSONObject();
	doTest(obj, "{}");

	key = strnew("test");
	obj->add(key, new JSONNull());
	doTest(obj, "{\"test\": null}");

	key = strnew("array");
	obj->add(key, new JSONArray());
	doTest(obj, "{\"array\": [], \"test\": null}");

	key = strnew("a");
	obj->add(key, new JSONInt(55));
	doTest(obj, "{\"a\": 55, \"array\": [], \"test\": null}");

	key = strnew("b");
	obj->add(key, new JSONString(strnew("This is only a test")));
	doTest(obj, "{\"a\": 55, \"array\": [], \"b\": \"This is only a test\", \"test\": null}");

	delete obj;
}

void testArray()
{
	JSONArray *arr, *outerArr;
	arr = new JSONArray();
	doTest(arr, "[]");
	arr->add(new JSONNull());
	doTest(arr, "[null]");
	arr->add(new JSONBool(true));
	doTest(arr, "[null, true]");
	outerArr = new JSONArray();
	outerArr->add(arr);
	outerArr->add(new JSONNull());
	doTest(outerArr, "[[null, true], null]");
	outerArr->add(new JSONInt(-15));
	outerArr->add(new JSONFloat(0.75));
	outerArr->add(new JSONString(strnew("This is only a test")));
	doTest(outerArr, "[[null, true], null, -15, 0.750000, \"This is only a test\"]");
	delete outerArr;
}

void testBadWrite()
{
	assertNull(writeJSON(nullptr));
}

void testFileWrite()
{
	//TODO: Figure out what the hell to use as test reference data.
	const char *const refData = "{\"widget\": {\"debug\": \"on\", "
		"\"window\": {\"title\": \"Sample Konfabulator Widget\", "
		"\"name\": \"main_window\", \"width\": 500, \"height\": 500}, "
		"\"image\": {\"src\": \"Images/Sun.png\", \"name\": \"sun1\", "
		"\"hOffset\": 250, \"vOffset\": 250, \"alignment\": \"center\"}, "
		"\"text\": {\"data\": \"Click Here\", \"size\": 36, \"style\": \"bold\", "
		"\"name\": \"text1\", \"hOffset\": 250, \"vOffset\": 100, "
		"\"alignment\": \"center\", \"onMouseUp\": \"sun1.opacity = "
		"(sun1.opacity / 100) * 90;\"}}}";
	// Create the tree to write from the above blob
	memoryStream_t sourceStream(const_cast<char *const>(refData), strlen(refData) + 1);
	JSONAtom *json = parseJSON(sourceStream);
	assertNotNull(json);

	// Write it using the stream writer engine, and using writeJSON for code coverage
	fileStream_t destStream("test.json", O_RDWR | O_CREAT | O_TRUNC | O_NOCTTY, S_IRUSR | S_IWUSR);
	stream_t &readStream = destStream;
	writeJSON(json, destStream);

	// Go back to the start of our test file and allocate memory to read it
	destStream.seek(0, SEEK_SET);
	char *const resultData = new (std::nothrow) char[destStream.size() + 1];
	assertNotNull(resultData);
	// Read it
	const bool result = readStream.read(resultData, destStream.size());
	// Cleanup
	unlink("test.json");
	// Continue testing (verify results)
	assertTrue(result);
	resultData[destStream.size()] = 0;
	assertStringEqual(resultData, refData);
}

#ifdef __cplusplus
extern "C"
{
#endif

BEGIN_REGISTER_TESTS()
	TEST(testNull)
	TEST(testBool)
	TEST(testInt)
	TEST(testFloat)
	TEST(testObject)
	TEST(testArray)
	TEST(testBadWrite)
	TEST(testFileWrite)
END_REGISTER_TESTS()

#ifdef __cplusplus
}
#endif
