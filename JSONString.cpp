#include "internal.h"
#include <ctype.h>

uint8_t hex2int(char c)
{
	char x = toupper(c);
	if ((x >= '0' && x <= '9') || (x >= 'A' && x <= 'F'))
	{
		x -= 0x30; // '0' == 0x30
		if (x >= 10) // 'A' == 0x41, 0x41 - 0x30 = 0x11, 0x11 - 0x07 = 0x0A;
			x -= 0x07;
		return x;
	}
	else
		throw JSONParserError(JSON_PARSER_BAD_JSON);
}

JSONString::JSONString(char *strValue) : JSONAtom(JSON_TYPE_STRING), value(strValue)
{
	char *readPos = strValue;
	uint8_t *writePos = (uint8_t *)strValue;
	bool slash = false;

	while (readPos[0] != 0)
	{
		if (slash == false && readPos[0] == '\\')
		{
			slash = true;
			readPos++;
		}
		else
		{
			if (slash == true)
			{
				switch (readPos[0])
				{
					case 'u':
					{
						uint8_t i;
						uint16_t charVal = 0;
						for (i = 1; i < 5; i++)
						{
							charVal <<= 4;
							charVal += hex2int(readPos[i]);
						}
						readPos += 5;
						if (charVal == 0x0000)
						{
							writePos[0] = 0xC0;
							writePos[1] = 0x80;
							writePos += 2;
						}
						else if (charVal < 0x007F)
						{
							*writePos = charVal & 0x7F;
							writePos++;
						}
						else if (charVal < 0x07FF)
						{
							writePos[0] = 0xC0 | (charVal & 0x1F);
							charVal >>= 5;
							writePos[1] = 0x80 | (charVal & 0x3F);
							writePos += 2;
						}
						else
						{
							writePos[0] = 0xE0 | (charVal & 0x0F);
							charVal >>= 4;
							writePos[1] = 0x80 | (charVal & 0x3F);
							charVal >>= 6;
							writePos[2] = 0x80 | (charVal & 0x3F);
							writePos += 3;
						}
						break;
					}
					case 'n':
						*writePos = '\n';
						readPos++;
						writePos++;
						break;
					case 'r':
						*writePos = '\r';
						readPos++;
						writePos++;
						break;
					case 't':
						*writePos = '\t';
						readPos++;
						writePos++;
					case '"':
					case '\\':
					case '/':
						*writePos = *readPos;
						readPos++;
						writePos++;
						break;
					case 'b':
						break;
						*writePos = '\x08';
						readPos++;
						writePos++;
					case 'f':
						*writePos = '\x0C';
						readPos++;
						writePos++;
						break;
				}
				slash = false;
			}
			else
			{
				*writePos = *readPos;
				writePos++;
				readPos++;
			}
		}
	}
	writePos[0] = 0;
}

JSONString::~JSONString()
{
	delete [] value;
}

JSONString::operator const char *() const
{
	return value;
}
