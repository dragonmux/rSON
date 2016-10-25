/*
 * This file is part of rSON
 * Copyright © 2012-2013 Rachel Mant (dx-mon@users.sourceforge.net)
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

#ifndef __rSON_H__
#define __rSON_H__

#include <stdint.h>
#include <map>
#include <vector>
#include <string.h>

#ifdef _WINDOWS
	#ifdef __rSON__
		#define rSON_API __declspec(dllexport)
	#else
		#define rSON_API __declspec(dllimport)
	#endif
	#define rSON_CLS_API	rSON_API
#else
	#if __GNUC__ >= 4
		#define DEFAULT_VISIBILITY __attribute__ ((visibility("default")))
	#else
		#define DEFAULT_VISIBILITY
	#endif
	#define rSON_CLS_API DEFAULT_VISIBILITY
	#define rSON_API extern rSON_CLS_API
#endif

namespace rSON
{
	// Enumerations

	typedef enum JSONAtomType
	{
		JSON_TYPE_NULL,
		JSON_TYPE_BOOL,
		JSON_TYPE_INT,
		JSON_TYPE_FLOAT,
		JSON_TYPE_STRING,
		JSON_TYPE_OBJECT,
		JSON_TYPE_ARRAY
	} JSONAtomType;

	typedef enum JSONParserErrorType
	{
		JSON_PARSER_EOF,
		JSON_PARSER_BAD_JSON,
		JSON_PARSER_BAD_FILE
	} JSONParserErrorType;

	typedef enum JSONObjectErrorType
	{
		JSON_OBJECT_BAD_KEY
	} JSONObjectErrorType;

	typedef enum JSONArrayErrorType
	{
		JSON_ARRAY_OOB
	} JSONArrayErrorType;

	// Exception classes

	class rSON_CLS_API JSONParserError
	{
	private:
		JSONParserErrorType parserError;

	public:
		JSONParserError(JSONParserErrorType errorType);
		~JSONParserError();
		JSONParserErrorType errorType() const;
		const char *error() const;
	};

	class rSON_CLS_API JSONTypeError
	{
	private:
		char *errorStr;
		const char *typeToString(JSONAtomType type) const;

	public:
		JSONTypeError(JSONAtomType actual, JSONAtomType expected);
		~JSONTypeError();
		const char *error() const;
	};

	class rSON_CLS_API JSONObjectError
	{
	private:
		JSONObjectErrorType objectError;

	public:
		JSONObjectError(JSONObjectErrorType errorType);
		~JSONObjectError();
		const char *error() const;
	};

	class rSON_CLS_API JSONArrayError
	{
	private:
		JSONArrayErrorType arrayError;

	public:
		JSONArrayError(JSONArrayErrorType errorType);
		~JSONArrayError();
		const char *error() const;
	};

	// Support types
	struct StringLess
	{
		inline bool operator()(char *x, char *y) const
		{
			return strcmp(x, y) < 0;
		}
	};

	// Hierachy types

	class JSONString;
	class JSONObject;
	class JSONArray;

	class rSON_CLS_API JSONAtom
	{
	private:
		const JSONAtomType type;

	protected:
		constexpr JSONAtom() noexcept : type(JSON_TYPE_NULL) { }
		constexpr JSONAtom(const JSONAtomType atomType) noexcept : type(atomType) { }

	public:
		virtual ~JSONAtom() { }
		JSONAtomType getType() const noexcept { return type; }
		virtual void store(char *str) = 0;
		virtual size_t length() = 0;

		void *asNull() const;
		bool asBool() const { return *this; }
		int asInt() const { return *this; }
		double asFloat() const { return *this; }
		const char *asString() const { return *this; }
		JSONString &asStringRef() const;
		JSONObject *asObject() const;
		JSONObject &asObjectRef() const { return *this; }
		JSONArray *asArray() const;
		JSONArray &asArrayRef() const { return *this; }

		operator JSONAtom *() const;
		operator bool() const;
		operator int() const;
		operator double() const;
		operator const char *() const;
		operator JSONString &() const;
		operator JSONObject &() const;
		operator JSONArray &() const;

		// Turns out that it is useful to allow this type to be indexed.
		// These two operator index's automatically convert the JSONAtom as we can know if the index is for an array or object based on the index type.
		JSONAtom &operator [](const char *const key) const;
		JSONAtom &operator [](const size_t key) const;

		// Utility functions to help with type checking (validation)
		bool typeIs(const JSONAtomType atomType) const noexcept { return type == atomType; }
		bool typeIsOrNull(const JSONAtomType atomType) const noexcept { return type == atomType || type == JSON_TYPE_NULL; }
	};

	class rSON_CLS_API JSONNull : public JSONAtom
	{
	public:
		JSONNull();
		~JSONNull();
		size_t length();
		void store(char *str);
	};

	class rSON_CLS_API JSONFloat : public JSONAtom
	{
		double value;

	public:
		JSONFloat(double floatValue);
		~JSONFloat();
		operator double() const;
		size_t length();
		void store(char *str);
	};

	class rSON_CLS_API JSONInt : public JSONAtom
	{
	private:
		int value;

	public:
		JSONInt(int intValue);
		~JSONInt();
		operator int() const;
		void set(int intValue);
		size_t length();
		void store(char *str);
	};

	class rSON_CLS_API JSONString : public JSONAtom
	{
	private:
		char *value;

	public:
		JSONString(char *strValue);
		~JSONString();
		operator const char *() const;
		void set(char *strValue);
		size_t length();
		void store(char *str);
	};

	class rSON_CLS_API JSONBool : public JSONAtom
	{
	private:
		bool value;

	public:
		JSONBool(bool boolValue);
		~JSONBool();
		operator bool() const;
		void set(bool boolValue);
		size_t length();
		void store(char *str);
	};

	class rSON_CLS_API JSONObject : public JSONAtom
	{
	private:
		typedef std::map<char *, JSONAtom *, StringLess> atomMap;
		typedef atomMap::iterator atomMapIter;
		typedef atomMap::const_iterator atomMapConstIter;
		typedef std::vector<const char *> keyType;
		typedef keyType::iterator keyTypeIter;
		atomMap children;
		keyType mapKeys;

	public:
		JSONObject();
		JSONObject(JSONObject &object);
		~JSONObject();
		void add(char *key, JSONAtom *value);
		void del(const char *key);
		JSONAtom &operator [](const char *const key) const;
		const std::vector<const char *> &keys() const;
		bool exists(const char *key) const;
		size_t size() const;
		size_t length();
		void store(char *str);
	};

	class rSON_CLS_API JSONArray : public JSONAtom
	{
	private:
		typedef std::vector<JSONAtom *> childType;
		typedef childType::iterator childTypeIter;
		childType children;

	public:
		typedef childType::const_iterator iterator;

		JSONArray();
		JSONArray(JSONArray &array);
		~JSONArray();
		void add(JSONAtom *value);
		void del(size_t key);
		void del(JSONAtom *value);
		JSONAtom &operator [](const size_t key) const;
		size_t size() const;
		iterator begin() const;
		iterator end() const;
		size_t length();
		void store(char *str);
	};

	rSON_API JSONAtom *parseJSON(const char *json);
	rSON_API JSONAtom *parseJSONFile(const char *file);

	rSON_API char *writeJSON(JSONAtom *atom);
	rSON_API void freeString(char **str);

	// Utility templates to help with type checking (validation)
	template<JSONAtomType type> bool typeIs(const JSONAtom &atom) noexcept { return atom.typeIs(type); }
	template<JSONAtomType type> bool typeIsOrNull(const JSONAtom &atom) noexcept { return atom.typeIsOrNull(type); }
}

#endif /*__rSON_H__*/
