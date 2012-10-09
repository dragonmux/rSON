#include "test.h"

JSONNull *testNull = NULL;

void testConstruct()
{
	try
	{
		testNull = new JSONNull();
	}
	catch (std::bad_alloc &badAlloc)
	{
		fail(badAlloc.what());
	}
	assertNotNull(testNull);
}

void testConversions()
{
	WANTED_TYPE(assertNull(testNull->asNull()))
	UNWANTED_TYPE(testNull, Bool)
	UNWANTED_TYPE(testNull, Int)
	UNWANTED_TYPE(testNull, Float)
	UNWANTED_TYPE(testNull, String)
	UNWANTED_TYPE(testNull, Object)
	UNWANTED_TYPE(testNull, Array)
}

void testDistruct()
{
	delete testNull;
	testNull = NULL;
}

#ifdef __cplusplus
extern "C"
{
#endif

BEGIN_REGISTER_TESTS()
	TEST(testConstruct)
	TEST(testConversions)
	TEST(testDistruct)
END_REGISTER_TESTS()

#ifdef __cplusplus
}
#endif
