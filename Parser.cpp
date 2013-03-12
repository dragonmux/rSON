/*
 * This file is part of rSON
 * Copyright © 2012 Richard/Rachel Mant (dx-mon@users.sourceforge.net)
 *
 * rSON is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * rSON is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

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
	bool lastWasComma;

	void validateUnicodeSequence();

public:
	JSONParser(const char *toParse);
	void nextChar();
	void skipWhite();
	void match(char x, bool skip);
	bool lastTokenComma();
	void lastNoComma();
	char currentChar();
	char *literal();
	char *string();
	size_t number();
} JSONParser;

JSONAtom *expression(JSONParser *parser, bool matchComma = true);

inline bool isLowerAlpha(char x)
{
	return x >= 'a' && x <= 'z';
}

inline bool isControl(char x)
{
	return (x >= 0 && x <= 0x1F) || x == 0x7F;
}

inline bool isAllowedAlpha(char x)
{
	return x != '"' && x != '\\' && isControl(x) == false;
}

inline bool isNumber(char x)
{
	return x >= '0' && x <= '9';
}

inline bool isObjectBegin(char x)
{
	return x == '{';
}

inline bool isObjectEnd(char x)
{
	return x == '}';
}

inline bool isArrayBegin(char x)
{
	return x == '[';
}

inline bool isArrayEnd(char x)
{
	return x == ']';
}

inline bool isSlash(char x)
{
	return x == '\\';
}

inline bool isQuote(char x)
{
	return x == '"';
}

inline bool isExponent(char x)
{
	return x == 'e' || x == 'E';
}

inline bool isNewLine(char x)
{
	return x == '\n' || x == '\r';
}

inline bool isWhiteSpace(char x)
{
	return x == ' ' || x == '\t' || isNewLine(x);
}

inline bool isHex(char x)
{
	return (x >= '0' && x <= '9') ||
		(x >= 'A' && x <= 'F') ||
		(x >= 'a' && x <= 'f');
}

JSONParser::JSONParser(const char *toParse)
{
	json = toParse;
	next = json;
	jsonEnd = json + strlen(json);
	lastWasComma = false;
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
		while (isWhiteSpace(currentChar()))
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
		if (x == ',')
			lastWasComma = true;
		else
			lastWasComma = false;
		nextChar();
		if (skip)
			skipWhite();
	}
	else
		throw JSONParserError(JSON_PARSER_BAD_JSON);
}

bool JSONParser::lastTokenComma()
{
	return lastWasComma;
}

void JSONParser::lastNoComma()
{
	lastWasComma = false;
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

	if (isLowerAlpha(currentChar()) == false)
		throw JSONParserError(JSON_PARSER_BAD_JSON);
	while (isLowerAlpha(currentChar()))
		nextChar();

	len = (size_t)(next - start) + 1;
	ret = new char[len]();
	memcpy(ret, start, len - 1);
	ret[len - 1] = 0;
	skipWhite();

	return ret;
}

void JSONParser::validateUnicodeSequence()
{
	char len = 0;
	for (len = 0; len < 4 && isHex(next[1]); len++)
		nextChar();
	if (len != 4)
		throw JSONParserError(JSON_PARSER_BAD_JSON);
}

char *JSONParser::string()
{
	size_t len;
	char *str;
	const char *start, *end;
	bool slash = false;

	match('"', false);
	start = next;

	while (isQuote(currentChar()) == false || slash == true)
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
					validateUnicodeSequence();
					// Make sure there are 4 hex characters here
					break;
				default:
					throw JSONParserError(JSON_PARSER_BAD_JSON);
			}
			slash = false;
		}
		else
		{
			slash = isSlash(currentChar());
			if (slash == false && isAllowedAlpha(currentChar()) == false)
				throw JSONParserError(JSON_PARSER_BAD_JSON);
		}
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

	if (isNumber(currentChar()) == false)
		throw JSONParserError(JSON_PARSER_BAD_JSON);

	if (currentChar() == '0')
	{
		nextChar();
		if (isNumber(currentChar()))
			throw JSONParserError(JSON_PARSER_BAD_JSON);
		return 0;
	}

	while (isNumber(currentChar()))
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
		while (isObjectEnd(parser->currentChar()) == false)
		{
			char *key = parser->string();
			parser->match(':', true);
			object->add(key, expression(parser));
		}
		if (parser->lastTokenComma())
			throw JSONParserError(JSON_PARSER_BAD_JSON);
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
		while (isArrayEnd(parser->currentChar()) == false)
			array->add(expression(parser));
		if (parser->lastTokenComma())
			throw JSONParserError(JSON_PARSER_BAD_JSON);
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
	if (isExponent(parser->currentChar()))
	{
		parser->nextChar();
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
		if (isNumber(parser->currentChar()))
			atom = number(parser);
		else
			atom = literal(parser);
	}

	if (matchComma && isObjectEnd(parser->currentChar()) == false && isArrayEnd(parser->currentChar()) == false)
		parser->match(',', true);
	else
		parser->lastNoComma();
	return atom;
}

JSONAtom *rSON::parseJSON(const char *json)
{
	JSONAtom *ret = NULL;
	JSONParser *parser;
	if (json == NULL)
		return NULL;
	parser = new JSONParser(json);

	if (isObjectBegin(parser->currentChar()) || isArrayBegin(parser->currentChar()))
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
