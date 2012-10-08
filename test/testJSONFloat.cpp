#include "test.h"

JSONFloat *testFloat = NULL;

void testConstruct()
{
	try
	{
		testFloat = new JSONFloat(5.0);
	}
	catch (std::bad_alloc badAlloc)
	{
		fail(badAlloc.what());
	}
	assertNotNull(testFloat);
}

/*void testOperatorDouble()
{
	assertNotNull(testFloat);
	assertDoubleEqual(*testFloat, 5.0);
	assertDoubleNotEqual(*testFloat, 0.0);
}*/

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
//	TEST(testOperatorDouble)
	TEST(testDistruct)
END_REGISTER_TESTS()

#ifdef __cplusplus
}
#endif
