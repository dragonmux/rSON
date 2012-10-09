#include "test.h"

JSONFloat *testFloat = NULL;

void testConstruct()
{
	try
	{
		testFloat = new JSONFloat(5.0);
	}
	catch (std::bad_alloc &badAlloc)
	{
		fail(badAlloc.what());
	}
	assertNotNull(testFloat);
}

void testOperatorDouble()
{
	assertNotNull(testFloat);
	assertDoubleEqual(*testFloat, 5.0);
	assertDoubleNotEqual(*testFloat, 0.0);
}

void testConversions()
{
	UNWANTED_TYPE(testFloat, Null)
	UNWANTED_TYPE(testFloat, Bool)
	UNWANTED_TYPE(testFloat, Int)
	WANTED_TYPE(testFloat->asFloat())
	WANTED_TYPE(assertDoubleEqual(testFloat->asFloat(), 5.0))
	UNWANTED_TYPE(testFloat, String)
	UNWANTED_TYPE(testFloat, Object)
	UNWANTED_TYPE(testFloat, Array)
}

void testDistruct()
{
	delete testFloat;
	testFloat = NULL;
}

#ifdef __cplusplus
extern "C"
{
#endif

BEGIN_REGISTER_TESTS()
	TEST(testConstruct)
	TEST(testOperatorDouble)
	TEST(testConversions)
	TEST(testDistruct)
END_REGISTER_TESTS()

#ifdef __cplusplus
}
#endif
