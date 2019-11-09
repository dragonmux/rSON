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
#include "../String.hxx"

JSONArray *testArray = NULL;
constexpr std::array<int32_t, 6> testValues
{
	-50, 0, 128, 65536, INT32_MIN, INT32_MAX
};

class JSONBad final : public JSONAtom
{
public:
	JSONBad() noexcept : JSONAtom(JSONAtomType(-1)) { }
	void store(stream_t &stream) const final override { }
	size_t length() const final override { return 0; }
};

void testConstruct()
{
	try
		{ testArray = new JSONArray(); }
	catch (std::bad_alloc &badAlloc)
		{ fail(badAlloc.what()); }
	assertNotNull(testArray);
	assertIntEqual(testArray->count(), 0);
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
		{ fail(err.error()); }

#define CATCH_FAIL(tryStuff) \
	COMMON_CATCH(tryStuff) \
	catch (JSONArrayError &err) \
		{ fail(err.error()); }

void testLookup()
{
	JSONAtom *child;

	assertNotNull(testArray);
	assertIntEqual(testArray->size(), 2);

	CATCH_FAIL( \
		child = &(*testArray)[0]; \
		assertNotNull(child); \
		assertIntEqual(child->asInt(), 1)
	)

	CATCH_FAIL( \
		child = &(*testArray)[1]; \
		assertNotNull(child); \
		assertIntEqual(child->asInt(), 2)
	)

	COMMON_CATCH( \
		child = &(*testArray)[2]; \
		fail("Array index out of bounds exception not thrown when it should have been!")
	)
	catch (JSONArrayError &) { }

	child = testArray;
	CATCH_FAIL(assertPtrEqual(&(*child)[size_t(0)], &(*testArray)[0]))
}

void testDuplicate()
{
	assertNotNull(testArray);
	testArray->add(new JSONNull());
	testArray->add(new JSONBool(true));
	testArray->add(new JSONFloat(1.5));
	testArray->add(new JSONString(strNewDup("This is only a test")));
	testArray->add(new JSONArray());
	testArray->add(new JSONObject());
	assertIntEqual(testArray->size(), 8);

	JSONArray dupArray(*testArray);
	assertIntNotEqual(dupArray.size(), 0);
	assertIntEqual(dupArray.size(), testArray->size());

	testArray->add(new JSONBad());
	assertIntEqual(testArray->size(), 9);
	try
	{
		JSONArray redup(*testArray);
		fail("JSONArray constructor failed to throw JSONArrayError when it should have been!");
	}
	catch (JSONArrayError &) { }
	testArray->del(8);
	assertIntEqual(testArray->size(), 8);
}

void testDel()
{
	assertNotNull(testArray);
	assertIntEqual(testArray->size(), 8);
	testArray->del(size_t(0));
	assertIntEqual(testArray->size(), 7);
	JSONAtom *child = &(*testArray)[6];
	assertNotNull(child);
	testArray->del(child);
	assertIntEqual(testArray->size(), 6);
	testArray->del(4);
	assertIntEqual(testArray->size(), 5);

	CATCH_FAIL(testArray->del(nullptr))
	try
	{
		testArray->del(5);
		fail("Array index out of bounds exception not thrown when it should have been!");
	}
	catch (JSONArrayError &) { }
}

void testDistruct()
{
	delete testArray;
	testArray = NULL;
}

void testIterate()
{
	JSONArray array{};
	for (const auto &value : testValues)
	{
		try
		{
			std::unique_ptr<JSONAtom> jsonAtom = std::make_unique<JSONInt>(value);
			array.add(std::move(jsonAtom));
		}
		catch (std::bad_alloc &)
			{ fail("Out of memory occured during construction phase of test"); }
	}
	assertIntEqual(array.count(), 6);
	size_t i = 0;
	for (auto &valuePtr : array)
	{
		assertNotNull(valuePtr.get());
		auto &valueAtom = *valuePtr;
		assertIntEqual(valueAtom.getType(), JSON_TYPE_INT);
		int32_t value = valueAtom;
		assertIntEqual(value, testValues[i++]);
		assertLessThan(i, 7);
	}
	assertIntEqual(i, 6);
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
	TEST(testDuplicate)
	TEST(testDel)
	TEST(testDistruct)
	TEST(testIterate)
END_REGISTER_TESTS()

#ifdef __cplusplus
}
#endif
