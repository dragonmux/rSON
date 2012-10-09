#include "test.h"
#include <string.h>

JSONString *testString = NULL;
static const char *constTestValue = "testValue";

void testConstruct()
{
	char *testValue = new char[strlen(constTestValue) + 1]();
	strcpy(testValue, constTestValue);
	try
	{
		testString = new JSONString(testValue);
	}
	catch (std::bad_alloc &badAlloc)
	{
		delete [] testValue;
		fail(badAlloc.what());
	}
	assertNotNull(testString);
}

void testOperatorString()
{
	assertNotNull(testString);
	// TODO: assertConstNotNull()
	assertNotNull((char *)testString->operator const char *());
	assertStringEqual(*testString, "testValue");
}

void testConversions()
{
	UNWANTED_TYPE(testString, Null)
	UNWANTED_TYPE(testString, Bool)
	UNWANTED_TYPE(testString, Int)
	UNWANTED_TYPE(testString, Float)
	// TODO: assertConstNotNull()
	WANTED_TYPE( \
		assertNotNull((char *)testString->asString()); \
		assertStringEqual(testString->asString(), "testValue")
	)
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
