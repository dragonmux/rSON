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

JSONObject *testObject = NULL;
#define KEY(name) \
static const char *name = #name
KEY(testKey1);
KEY(testKey2);

void insert(const char *const keyValue, JSONAtom *value)
{
	char *key = new char[strlen(keyValue) + 1];
	strcpy(key, keyValue);
	testObject->add(key, value);
}

void testConstruct()
{
	try
	{
		testObject = new JSONObject();
	}
	catch (std::bad_alloc &badAlloc)
	{
		fail(badAlloc.what());
	}
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
	JSONAtom *child;

	assertNotNull(testObject);
	assertIntEqual(testObject->size(), 0);

	insert(testKey1, new JSONInt(1));
	assertIntEqual(testObject->size(), 1);

	insert(testKey2, new JSONInt(2));
	assertIntEqual(testObject->size(), 2);

	child = new JSONInt(3);
	testObject->add((char *)testKey2, child);
	delete child;

	assertIntEqual(testObject->size(), 2);
}

void testKeys()
{
	std::vector<const char *> keys = testObject->keys();

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
		{ child = (*testObject)[testKey1]; }
	catch (JSONTypeError &err)
		{ fail(err.error()); }
	assertIntEqual(child->asInt(), 1);

	JSONAtom &atom = *testObject;
	JSONObject &obj = *testObject;
	try
		{ assertPtrEqual(atom[testKey1], (*testObject)[testKey1]); }
	catch (JSONTypeError &err)
		{ fail(err.error()); }
	catch (JSONObjectError &err)
		{ fail(err.error()); }
	try
	{
		child = obj["nonExistant"];
		fail("Object failed to throw exception for bad key");
	}
	catch (JSONObjectError &) { }
}

void testDuplicate()
{
	KEY(a); KEY(b); KEY(c); KEY(d); KEY(e);
	assertNotNull(testObject);
	insert(a, new JSONNull());
	insert(b, new JSONBool(true));
	insert(c, new JSONFloat(1.5));
	insert(d, new JSONArray());
	insert(e, new JSONObject());
	assertIntEqual(testObject->size(), 7);

	JSONObject dupObject(*testObject);
	assertIntNotEqual(dupObject.size(), 0);
	assertIntEqual(dupObject.size(), testObject->size());
}

void testDel()
{
	assertNotNull(testObject);
	assertIntEqual(testObject->size(), 7);

	testObject->del("a");
	assertIntEqual(testObject->size(), 6);

	testObject->del("notPresent");
	assertIntEqual(testObject->size(), 6);

	testObject->del(nullptr);
	assertIntEqual(testObject->size(), 6);
}

void testDistruct()
{
	delete testObject;
	testObject = NULL;
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
	TEST(testKeys)
	TEST(testLookup)
	TEST(testDuplicate)
	TEST(testDel)
	TEST(testDistruct)
END_REGISTER_TESTS()

#ifdef __cplusplus
}
#endif
