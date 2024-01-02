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

#include <memory>
#include <string_view>
#include <fcntl.h>
#include "test.h"

#ifdef _WIN32
#define O_NOCTTY O_BINARY
#endif

using namespace std::literals::string_view_literals;

char *strnew(const char *str)
{
	char *ret = new char[strlen(str) + 1];
	strcpy(ret, str);
	return ret;
}

void doTest(JSONAtom *atom, const std::string_view result)
{
	const auto json{std::make_unique<char []>(result.length() + 1U)};
	memoryStream_t stream{json.get(), result.length() + 1U};
	assertTrue(writeJSON(atom, stream));
	assertNotNull(json.get());
	assertStringEqual(json.get(), result.data());
}

void testNull()
{
	JSONNull *null = new JSONNull();
	doTest(null, "null"sv);
	delete null;
}

void testBool()
{
	JSONBool *bTrue = new JSONBool(true);
	JSONBool *bFalse = new JSONBool(false);
	doTest(bTrue, "true"sv);
	doTest(bFalse, "false"sv);
	delete bTrue;
	delete bFalse;
}

void testInt()
{
	JSONInt *num = new JSONInt(8192);
	doTest(num, "8192"sv);
	delete num;
}

void testFloat()
{
	JSONFloat *num = new JSONFloat(8192.016384);
	doTest(num, "8192.0163840000004711"sv);
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
	std::unique_ptr<char []> key{};
	JSONObject obj{};
	doTest(&obj, "{}"sv);

	key.reset(strnew("test"));
	obj.add(key.get(), new JSONNull());
	doTest(&obj, "{\"test\": null}"sv);

	key.reset(strnew("array"));
	obj.add(key.get(), new JSONArray());
	doTest(&obj, "{\"array\": [], \"test\": null}"sv);

	key.reset(strnew("a"));
	obj.add(key.get(), new JSONInt(55));
	doTest(&obj, "{\"a\": 55, \"array\": [], \"test\": null}"sv);

	key.reset(strnew("b"));
	obj.add(key.get(), new JSONString("This is only a test"sv));
	doTest(&obj, "{\"a\": 55, \"array\": [], \"b\": \"This is only a test\", \"test\": null}"sv);
}

void testArray()
{
	JSONArray *arr, *outerArr;
	arr = new JSONArray();
	doTest(arr, "[]"sv);
	arr->add(new JSONNull());
	doTest(arr, "[null]"sv);
	arr->add(new JSONBool(true));
	doTest(arr, "[null, true]"sv);
	outerArr = new JSONArray();
	outerArr->add(arr);
	outerArr->add(new JSONNull());
	doTest(outerArr, "[[null, true], null]"sv);
	outerArr->add(new JSONInt(-15));
	outerArr->add(new JSONFloat(0.75));
	outerArr->add(new JSONString("This is only a test"sv));
	doTest(outerArr, "[[null, true], null, -15, 0.7500000000000000, \"This is only a test\"]"sv);
	delete outerArr;
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
void testBadWrite()
{
	memoryStream_t stream{nullptr, 0U};
	assertFalse(writeJSON(nullptr, stream));
}
#pragma GCC diagnostic pop

void testFileWrite()
{
	//TODO: Figure out what the hell to use as test reference data.
	const char *const refData = "{\"widget\": {\"debug\": \"on\", "
		"\"image\": {\"alignment\": \"center\", \"hOffset\": 250, "
		"\"name\": \"sun1\", \"src\": \"Images/Sun.png\", \"vOffset\": 250}, "
		"\"text\": {\"alignment\": \"center\", \"data\": \"Click Here\", "
		"\"hOffset\": 250, \"name\": \"text1\", \"onMouseUp\": \"sun1.opacity = "
		"(sun1.opacity / 100) * 90;\", \"size\": 36, \"style\": \"bold\", "
		"\"vOffset\": 100}, \"window\": {\"height\": 500, \"name\": \"main_window\", "
		"\"title\": \"Sample Konfabulator Widget\", \"width\": 500}}}";
	// Create the tree to write from the above blob
	memoryStream_t sourceStream(const_cast<char *const>(refData), strlen(refData) + 1);
	auto json = parseJSON(sourceStream);
	assertNotNull(json.get());

	// Write it using the stream writer engine, and using writeJSON for code coverage
	fileStream_t destStream("test.json", O_RDWR | O_CREAT | O_TRUNC | O_NOCTTY, normalMode);
	stream_t &readStream = destStream;
	writeJSON(json, destStream); // could just as well be json->store(destStream)..

	// Go back to the start of our test file and allocate memory to read it
	destStream.seek(0, SEEK_SET);
	const std::unique_ptr<char []> resultData{new (std::nothrow) char[destStream.size() + 1]};
	assertNotNull(resultData.get());
	// Read it
	const bool result = readStream.read(resultData.get(), destStream.size());
	// Cleanup
	unlink("test.json");
	// Continue testing (verify results)
	assertTrue(result);
	resultData[destStream.size()] = 0;
	assertStringEqual(resultData.get(), refData);
}

extern "C"
{
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
}
