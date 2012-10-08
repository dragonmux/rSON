#include "test.h"

JSONArray *testArray = NULL;

void testConstruct()
{
	try
	{
		testArray = new JSONArray();
	}
	catch (std::bad_alloc badAlloc)
	{
		fail(badAlloc.what());
	}
}

void testConversions()
{
	UNWANTED_TYPE(testArray, Null)
	UNWANTED_TYPE(testArray, Bool)
	UNWANTED_TYPE(testArray, Int)
	UNWANTED_TYPE(testArray, Float)
	UNWANTED_TYPE(testArray, String)
	UNWANTED_TYPE(testArray, Object)
	WANTED_TYPE( \
		assertNotNull(testArray->asArray()); \
		assertPtrEqual(testArray->asArray(), testArray))
}

void testDistruct()
{
	delete testArray;
	testArray = NULL;
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
