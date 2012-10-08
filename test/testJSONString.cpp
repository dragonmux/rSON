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
	TEST(testDistruct)
END_REGISTER_TESTS()

#ifdef __cplusplus
}
#endif
