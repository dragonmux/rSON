#include "String.h"
#include "Memory.h"
#include <string.h>
#include <stdarg.h>
#include <stdio.h>

char *formatString(const char *format, ...)
{
	int len;
	char *ret;
	va_list args;
	va_start(args, format);
	len = vsnprintf(NULL, 0, format, args);
	va_end(args);
	ret = (char *)memMalloc(len + 1);
	va_start(args, format);
	vsprintf(ret, format, args);
	va_end(args);
	return ret;
}

char *strrpbrk(const char *s, const char *accept)
{
	size_t lenS, lenAccept, i;
	const char *rS;

	if (s == NULL || accept == NULL)
		return NULL;

	lenS = strlen(s);
	lenAccept = strlen(accept);

	if (lenS == 0 || lenAccept == 0)
		return NULL;

	for (rS = s + lenS - 1; rS != (s - 1); rS--)
	{
		for (i = 0; i < lenAccept; i++)
		{
			if (*rS == accept[i])
				return (char *)rS;
		}
	}

	return NULL;
}
