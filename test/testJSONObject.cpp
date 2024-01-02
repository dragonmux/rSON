// SPDX-License-Identifier: LGPL-3.0-or-later
// SPDX-FileCopyrightText: 2012-2013,2017-2020,2023 Rachel Mant <git@dragonmux.network>
// SPDX-FileContributor: Written by Rachel Mant <git@dragonmux.network>

#include <string_view>
#include <substrate/utility>
#include "test.h"
#include "internal/string.hxx"

using namespace std::literals::string_view_literals;

JSONObject *testObject = NULL;
#define KEY(name) \
static const char *name = #name
KEY(testKey1);
KEY(testKey2);

class JSONBad final : public JSONAtom
{
public:
	JSONBad() noexcept : JSONAtom(JSONAtomType(-1)) { }
	void store(stream_t &) const final override { }
	size_t length() const final override { return 0; }
};

void testConstruct()
{
	try
		{ testObject = new JSONObject(); }
	catch (std::bad_alloc &badAlloc)
		{ fail(badAlloc.what()); }
	assertNotNull(testObject);
}

void testConversions()
{
	assertNotNull(testObject);
	UNWANTED_TYPE(testObject, Null)
	UNWANTED_TYPE(testObject, Bool)
	UNWANTED_TYPE(testObject, Int)
	UNWANTED_TYPE(testObject, Float)
	UNWANTED_TYPE(testObject, String)
	WANTED_TYPE( \
		assertNotNull(testObject->asObject()); \
		assertPtrEqual(testObject->asObject(), testObject))
	UNWANTED_TYPE(testObject, Array)
}

void testSize()
{
	assertNotNull(testObject);
	assertIntEqual(testObject->size(), 0);
}

void testAdd()
{
	assertNotNull(testObject);
	assertIntEqual(testObject->size(), 0);

	testObject->add(testKey1, new JSONInt(1));
	assertIntEqual(testObject->size(), 1);

	testObject->add(testKey2, new JSONInt(2));
	assertIntEqual(testObject->size(), 2);

	auto child = substrate::make_unique<JSONInt>(3);
	testObject->add(testKey2, std::move(child));

	assertIntEqual(testObject->size(), 2);
}

void testKeys()
{
	const auto &keys = testObject->keys();

	assertIntEqual(keys.size(), 2);
	assertIntEqual(strcmp(keys[0], testKey1), 0);
	assertIntEqual(strcmp(keys[1], testKey2), 0);
}

void testLookup()
{
	JSONAtom *child;
	assertNotNull(testObject);
	assertIntEqual(testObject->size(), 2);
	try
		{ child = &(*testObject)[testKey1]; }
	catch (JSONTypeError &err)
		{ fail(err.error()); }
	assertIntEqual(child->asInt(), 1);

	JSONAtom &atom = *testObject;
	JSONObject &obj = *testObject;
	try
		{ assertPtrEqual(&atom[testKey1], &(*testObject)[testKey1]); }
	catch (JSONTypeError &err)
		{ fail(err.error()); }
	catch (JSONObjectError &err)
		{ fail(err.error()); }
	try
	{
		child = &obj["nonExistant"];
		fail("Object failed to throw exception for bad key");
	}
	catch (JSONObjectError &) { }
	try
	{
		child = &obj[nullptr];
		fail("Object failed to throw exception for bad key");
	}
	catch (JSONObjectError &) { }
}

void testDuplicate()
{
	KEY(a); KEY(b); KEY(c); KEY(d); KEY(e); KEY(f); KEY(g);
	assertNotNull(testObject);
	testObject->add(a, new JSONNull());
	testObject->add(b, new JSONBool(true));
	testObject->add(c, new JSONFloat(1.5));
	testObject->add(d, new JSONString("This is only a test"sv));
	testObject->add(e, new JSONArray());
	testObject->add(f, new JSONObject());
	assertIntEqual(testObject->size(), 8);

	JSONObject dupObject(*testObject);
	assertIntNotEqual(dupObject.size(), 0);
	assertIntEqual(dupObject.size(), testObject->size());

	testObject->add(g, new JSONBad());
	assertIntEqual(testObject->size(), 9);
	try
	{
		JSONObject redup(*testObject);
		fail("JSONObject constructor failed to throw JSONObjectError when it should have!");
	}
	catch (JSONObjectError &) { }
	testObject->del("g");
	assertIntEqual(testObject->size(), 8);
}

void testDel()
{
	assertNotNull(testObject);
	assertIntEqual(testObject->size(), 8);

	testObject->del("a");
	assertIntEqual(testObject->size(), 7);

	testObject->del("notPresent");
	assertIntEqual(testObject->size(), 7);

	testObject->del(nullptr);
	assertIntEqual(testObject->size(), 7);
}

void testDistruct()
{
	delete testObject;
	testObject = NULL;
}

extern "C"
{
BEGIN_REGISTER_TESTS()
	TEST(testConstruct)
	TEST(testConversions)
	TEST(testSize)
	TEST(testAdd)
	TEST(testKeys)
	TEST(testLookup)
	TEST(testDuplicate)
	TEST(testDel)
	TEST(testDistruct)
END_REGISTER_TESTS()
}
