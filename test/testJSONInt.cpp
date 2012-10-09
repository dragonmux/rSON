#include "test.h"

JSONInt *testInt = NULL;

void testConstruct()
{
	try
	{
		testInt = new JSONInt(5);
	}
	catch (std::bad_alloc &badAlloc)
	{
		fail(badAlloc.what());
	}
	assertNotNull(testInt);
}

void testOperatorInt()
{
	assertNotNull(testInt);
	assertIntEqual(*testInt, 5);
	assertIntNotEqual(*testInt, 0);
}

void testConversions()
{
	UNWANTED_TYPE(testInt, Null)
	UNWANTED_TYPE(testInt, Bool)
	WANTED_TYPE(assertIntEqual(testInt->asInt(), 5))
	UNWANTED_TYPE(testInt, Float)
	UNWANTED_TYPE(testInt, String)
	UNWANTED_TYPE(testInt, Object)
	UNWANTED_TYPE(testInt, Array)
}

void testDistruct()
{
	delete testInt;
	testInt = NULL;
}

#ifdef __cplusplus
extern "C"
{
#endif

BEGIN_REGISTER_TESTS()
	TEST(testConstruct)
	TEST(testOperatorInt)
	TEST(testConversions)
	TEST(testDistruct)
END_REGISTER_TESTS()

#ifdef __cplusplus
}
#endif
