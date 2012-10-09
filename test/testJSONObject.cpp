#include "test.h"

JSONObject *testObject = NULL;
#define KEY(name) \
static const char *name = #name
KEY(testKey1);
KEY(testKey2);

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
	char *key;

	assertNotNull(testObject);
	assertIntEqual(testObject->size(), 0);

	key = new char[strlen(testKey1) + 1];
	strcpy(key, testKey1);
	testObject->add(key, new JSONInt(1));
	assertIntEqual(testObject->size(), 1);

	key = new char[strlen(testKey2) + 1];
	strcpy(key, testKey2);
	testObject->add(key, new JSONInt(2));
	assertIntEqual(testObject->size(), 2);

	child = new JSONInt(3);
	testObject->add((char *)testKey2, child);
	delete child;

	assertIntEqual(testObject->size(), 2);
}

void testLookup()
{
	JSONAtom *child;
	assertNotNull(testObject);
	assertIntEqual(testObject->size(), 2);
	try
	{
		child = (*testObject)[testKey1];
	}
	catch (JSONTypeError &err)
	{
		fail(err.error());
	}
	assertIntEqual(child->asInt(), 1);
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
	TEST(testLookup)
	TEST(testDistruct)
END_REGISTER_TESTS()

#ifdef __cplusplus
}
#endif
