#include "test.h"

JSONBool *testBool = NULL;

void testConstructTrue()
{
	try
	{
		testBool = new JSONBool(true);
	}
	catch (std::bad_alloc badAlloc)
	{
		fail(badAlloc.what());
	}
	assertNotNull(testBool);
}

void testOperatorBoolTrue()
{
	assertNotNull(testBool);
	assertTrue(*testBool);
}

void testConstructFalse()
{
	try
	{
		testBool = new JSONBool(false);
	}
	catch (std::bad_alloc badAlloc)
	{
		fail(badAlloc.what());
	}
	assertNotNull(testBool);
}

void testOperatorBoolFalse()
{
	assertNotNull(testBool);
	assertFalse(*testBool);
}

void testDistruct()
{
	delete testBool;
	testBool = NULL;
}

void testConversions()
{
	UNWANTED_TYPE(testBool, Null)
	WANTED_TYPE(assertTrue(testBool->asBool()))
	UNWANTED_TYPE(testBool, Int)
	UNWANTED_TYPE(testBool, Float)
	UNWANTED_TYPE(testBool, String)
	UNWANTED_TYPE(testBool, Object)
	UNWANTED_TYPE(testBool, Array)
}

#ifdef __cplusplus
extern "C"
{
#endif

BEGIN_REGISTER_TESTS()
	TEST(testConstructTrue)
	TEST(testOperatorBoolTrue)
	TEST(testConversions)
	TEST(testDistruct)
	TEST(testConstructFalse)
	TEST(testOperatorBoolFalse)
	TEST(testDistruct)
END_REGISTER_TESTS()

#ifdef __cplusplus
}
#endif
