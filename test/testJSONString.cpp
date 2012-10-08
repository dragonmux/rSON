#include "test.h"
#include <string.h>

JSONString *testString = NULL;

void testConstruct()
{
	try
	{
		testString = new JSONString(strdup("testValue"));
	}
	catch (std::bad_alloc badAlloc)
	{
		fail(badAlloc.what());
	}
	assertNotNull(testString);
}

void testOperatorString()
{
	assertNotNull(testString);
	// TODO: assertConstNotNull()
	assertNotNull((char *)testString->operator const char *());
//	assertStringEqual(*testString, "testValue");
}

void testConversions()
{
	UNWANTED_TYPE(testString, Null)
	UNWANTED_TYPE(testString, Bool)
	UNWANTED_TYPE(testString, Int)
	UNWANTED_TYPE(testString, Float)
	// TODO: assertConstNotNull()
	WANTED_TYPE(assertNotNull((char *)testString->asString()))
	// WANTED_TYPE(assertStringEqual
	UNWANTED_TYPE(testString, Object)
	UNWANTED_TYPE(testString, Array)
}

void testDistruct()
{
	delete testString;
	testString = NULL;
}

#ifdef __cplusplus
extern "C"
{
#endif

BEGIN_REGISTER_TESTS()
	TEST(testConstruct)
	TEST(testOperatorString)
	TEST(testConversions)
	TEST(testDistruct)
END_REGISTER_TESTS()

#ifdef __cplusplus
}
#endif
