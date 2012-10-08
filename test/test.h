#include <libTest.h>

#include "../internal.h"

#define WANTED_TYPE(whatToDo) \
try \
{ \
	whatToDo; \
} \
catch (JSONTypeError e) \
{ \
	fail(e.error()); \
}

#define UNWANTED_TYPE(onWhat, type) \
try \
{ \
	onWhat->as ## type (); \
	fail("Type " #type " converted even though wrong"); \
} \
catch (JSONTypeError e) \
{ \
}
