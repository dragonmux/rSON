#include "internal.h"
//#include "String.h"
#include "Memory.h"
#include <string.h>
#include <math.h>

typedef struct JSONParser
{
private:
	const char *json;
	const char *next;
	const char *jsonEnd;

public:
	JSONParser(const char *toParse);
	void nextChar();
	void skipWhite();
	void match(char x, bool skip);
	char currentChar();
	char *literal();
	char *string();
	size_t number();
} JSONParser;

JSONAtom *expression(JSONParser *parser, bool matchComma = true);

bool IsLowerAlpha(char x)
{
	return x >= 'a' && x <= 'z';
}

bool IsControl(char x)
{
	return (x >= 0 && x <= 0x1F) || x == 0x7F;
}

bool IsAllowedAlpha(char x)
{
	return x != '"' && x != '\\' && IsControl(x) == false;
}

bool IsNumber(char x)
{
	return x >= '0' && x <= '9';
}

bool IsObjectBegin(char x)
{
	return x == '{';
}

bool IsObjectEnd(char x)
{
	return x == '}';
}

bool IsArrayBegin(char x)
{
	return x == '[';
}

bool IsArrayEnd(char x)
{
	return x == ']';
}

bool IsSlash(char x)
{
	return x == '\\';
}

bool IsQuote(char x)
{
	return x == '"';
}

bool IsExponent(char x)
{
	return x == 'e' || x == 'E';
}

bool IsNewLine(char x)
{
	return x == '\n' || x == '\r';
}

bool IsWhiteSpace(char x)
{
	return x == ' ' || x == '\t' || IsNewLine(x);
}

JSONParser::JSONParser(const char *toParse)
{
	json = toParse;
	next = json;
	jsonEnd = json + strlen(json);
}

void JSONParser::nextChar()
{
	if (next == jsonEnd)
		throw JSONParserError(JSON_PARSER_EOF);
	next++;
}

// This function intentionally ignores EOF to prevent the
// parser from exiting via exception when it sees the final } or ]
void JSONParser::skipWhite()
{
	try
	{
		while (IsWhiteSpace(currentChar()))
			nextChar();
	}
	catch (JSONParserError &err)
	{
	}
}

void JSONParser::match(char x, bool skip)
{
	if (currentChar() == x)
	{
		nextChar();
		if (skip)
			skipWhite();
	}
	else
		throw JSONParserError(JSON_PARSER_BAD_JSON);
}

char JSONParser::currentChar()
{
	char ret = *next;
	if (next == jsonEnd)
		throw JSONParserError(JSON_PARSER_EOF);
	return ret;
}

char *JSONParser::literal()
{
	size_t len;
	char *ret;
	const char *start = next;

	if (IsLowerAlpha(currentChar()) == false)
		throw JSONParserError(JSON_PARSER_BAD_JSON);
	while (IsLowerAlpha(currentChar()))
		nextChar();

	len = (size_t)(next - start) + 1;
	ret = new char[len]();
	memcpy(ret, start, len - 1);
	ret[len - 1] = 0;
	skipWhite();

	return ret;
}

char *JSONParser::string()
{
	size_t len;
	char *str;
	const char *end, *start = next;
	bool slash = false;

	match('"', false);

	while (IsQuote(currentChar()) == false && slash == false)
	{
		if (slash)
		{
			switch (currentChar())
			{
				case '"':
				case '\\':
				case '/':
				case 'b':
				case 'f':
				case 'n':
				case 'r':
				case 't':
					break;
				case 'u':
					// Make sure there are 4 hex characters here
					break;
				default:
					throw JSONParserError(JSON_PARSER_BAD_JSON);
			}
		}
		slash = IsSlash(currentChar());
		if (slash == false && IsAllowedAlpha(currentChar()) == false)
			throw JSONParserError(JSON_PARSER_BAD_JSON);
		nextChar();
	}

	end = next;
	match('"', true);

	len = (size_t)(end - start) + 1;
	str = new char[len]();
	memcpy(str, start, len - 1);
	str[len - 1] = 0;
	return str;
}

size_t JSONParser::number()
{
	size_t num = 0;

	if (IsNumber(currentChar()) == false)
		throw JSONParserError(JSON_PARSER_BAD_JSON);

	if (currentChar() == '0')
	{
		nextChar();
		return 0;
	}

	while (IsNumber(currentChar()))
	{
		num *= 10;
		num += currentChar() - '0';
		nextChar();
	}
	return num;
}

JSONAtom *object(JSONParser *parser)
{
	JSONObject *object = new JSONObject();
	try
	{
		parser->match('{', true);
		while (IsObjectEnd(parser->currentChar()) == false)
		{
			char *key = parser->string();
			parser->match(':', true);
			object->add(key, expression(parser));
		}
		parser->match('}', true);
	}
	catch (JSONParserError &err)
	{
		delete object;
		throw;
	}
	return object;
}

JSONAtom *array(JSONParser *parser)
{
	JSONArray *array = new JSONArray();
	try
	{
		parser->match('[', true);
		while (IsArrayEnd(parser->currentChar()) == false)
			array->add(expression(parser));
		parser->match(']', true);
	}
	catch (JSONParserError &err)
	{
		delete array;
		throw;
	}
	return array;
}

size_t power10(size_t power)
{
	size_t i, ret = 1;
	for (i = 0; i < power; i++)
		ret *= 10;
	return ret;
}

JSONAtom *number(JSONParser *parser)
{
	bool sign = false, mulSign = false;
	int integer = 0;
	size_t decimal = 0, multiplier = 0;
	bool decimalValid = false;

	if (parser->currentChar() == '-')
	{
		parser->match('-', false);
		sign = true;
	}
	integer = parser->number();
	if (parser->currentChar() == '.')
	{
		parser->match('.', false);
		decimal = parser->number();
		decimalValid = true;
	}
	if (IsExponent(parser->currentChar()))
	{
		if (parser->currentChar() == '-')
		{
			parser->match('-', false);
			mulSign = true;
		}
		else if (parser->currentChar() == '+')
			parser->match('+', false);
		multiplier = parser->number();
	}

	if (decimalValid == false)
	{
		int mul = power10(multiplier);
		if (mulSign)
			integer /= mul;
		else
			integer *= mul;
		if (sign)
			return new JSONInt(-integer);
		return new JSONInt(integer);
	}
	else
	{
		size_t mul = pow10(multiplier);
		double num = decimal;
		while (decimal < 0.0)
			decimal /= 10.0;
		num += integer;
		if (mulSign)
			num /= mul;
		else
			num *= mul;
		if (sign)
			return new JSONFloat(-num);
		return new JSONFloat(num);
	}
}

JSONAtom *literal(JSONParser *parser)
{
	JSONAtom *ret = NULL;
	char *lit = parser->literal();

	if (strcmp(lit, "true") == 0)
		ret = new JSONBool(true);
	else if (strcmp(lit, "false") == 0)
		ret = new JSONBool(false);
	else if (strcmp(lit, "null") == 0)
		ret = new JSONNull();
	delete [] lit;

	if (ret == NULL)
		throw JSONParserError(JSON_PARSER_BAD_JSON);
	return ret;
}

JSONAtom *expression(JSONParser *parser, bool matchComma)
{
	JSONAtom *atom = NULL;
	switch (parser->currentChar())
	{
		case '{':
			atom = object(parser);
			break;
		case '[':
			atom = array(parser);
			break;
		case '"':
			atom = new JSONString(parser->string());
			break;
	}

	if (atom == NULL)
	{
		if (IsNumber(parser->currentChar()))
			atom = number(parser);
		else
			atom = literal(parser);
	}

	if (matchComma)
		parser->match(',', true);
	return atom;
}

JSONAtom *parseJSON(const char *json)
{
	JSONAtom *ret = NULL;
	JSONParser *parser = new JSONParser(json);

	if (IsObjectBegin(parser->currentChar()) || IsArrayBegin(parser->currentChar()))
	{
		try
		{
			ret = expression(parser, false);
		}
		catch (JSONParserError &err)
		{
			delete parser;
			throw;
		}
	}
	else
	{
		delete parser;
		throw JSONParserError(JSON_PARSER_BAD_JSON);
	}

	delete parser;
	return ret;
}
