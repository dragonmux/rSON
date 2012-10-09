#ifndef __rSON_H__
#define __rSON_H__

#include <inttypes.h>
#include <map>
#include <vector>
#include <string.h>

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
		JSON_PARSER_BAD_JSON
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

	class JSONParserError
	{
	private:
		JSONParserErrorType parserError;

	public:
		JSONParserError(JSONParserErrorType errorType);
		~JSONParserError();
		JSONParserErrorType errorType() const;
		const char *error() const;
	};

	class JSONTypeError
	{
	private:
		char *errorStr;
		const char *typeToString(JSONAtomType type) const;

	public:
		JSONTypeError(JSONAtomType actual, JSONAtomType expected);
		~JSONTypeError();
		const char *error() const;
	};

	class JSONObjectError
	{
	private:
		JSONObjectErrorType objectError;

	public:
		JSONObjectError(JSONObjectErrorType errorType);
		~JSONObjectError();
		const char *error() const;
	};

	class JSONArrayError
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

	class JSONObject;
	class JSONArray;

	class JSONAtom
	{
	private:
		JSONAtomType type;

	protected:
		JSONAtom();
		JSONAtom(JSONAtomType type);

	public:
		virtual ~JSONAtom();
		JSONAtomType getType();

		void *asNull() const;
		bool asBool() const;
		int asInt() const;
		double asFloat() const;
		const char *asString() const;
		JSONObject *asObject() const;
		JSONArray *asArray() const;
	};

	class JSONNull : public JSONAtom
	{
	public:
		JSONNull();
		~JSONNull();
	};

	class JSONFloat : public JSONAtom
	{
		double value;

	public:
		JSONFloat(double floatValue);
		~JSONFloat();
		operator double() const;
	};

	class JSONInt : public JSONAtom
	{
	private:
		int value;

	public:
		JSONInt(int intValue);
		~JSONInt();
		operator int() const;
	};

	class JSONString : public JSONAtom
	{
	private:
		char *value;

	public:
		JSONString(char *strValue);
		~JSONString();
		operator const char *() const;
	};

	class JSONBool : public JSONAtom
	{
	private:
		bool value;

	public:
		JSONBool(bool boolValue);
		~JSONBool();
		operator bool() const;
	};

	class JSONObject : public JSONAtom
	{
	private:
		typedef std::map<char *, JSONAtom *, StringLess> atomMap;
		typedef atomMap::iterator atomMapIter;
		atomMap children;

	public:
		JSONObject();
		~JSONObject();
		void add(char *key, JSONAtom *value);
		JSONAtom *operator [](const char *key);
	};

	class JSONArray : public JSONAtom
	{
	private:
		std::vector<JSONAtom *> children;

	public:
		JSONArray();
		~JSONArray();
		void add(JSONAtom *value);
		JSONAtom *operator [](size_t key);
	};

	JSONAtom *parseJSON(const char *json);
}

#endif /*__rSON_H__*/
