/*
 * This file is part of rSON
 * Copyright © 2017 Rachel Mant (dx-mon@users.sourceforge.net)
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

#include "test.h"

void tryParserErrorOk(const JSONParserErrorType error)
{
	try
	{
		const auto err = JSONParserError(error).error();
		assertNotNull(err);
	}
	catch (std::exception &)
		{ fail("Caught exception which should not happen"); }
}

void testParserError()
{
	assertEqual(JSONParserError(JSON_PARSER_EOF).errorType(), JSON_PARSER_EOF);
	tryParserErrorOk(JSON_PARSER_EOF);
	tryParserErrorOk(JSON_PARSER_BAD_JSON);
	tryParserErrorOk(JSON_PARSER_BAD_FILE);

	try
	{
		const auto err = JSONParserError(reinterpret_cast<JSONParserErrorType>(-1)).error();
		fail("The error handling failed to throw an exception on an invalid initialisation");
	}
	catch (std::exception &) { }
}

#ifdef __cplusplus
extern "C"
{
#endif

BEGIN_REGISTER_TESTS()
	TEST(testParserError)
END_REGISTER_TESTS()

#ifdef __cplusplus
}
#endif
