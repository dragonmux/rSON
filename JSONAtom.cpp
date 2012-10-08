#include "internal.h"

JSONAtom::JSONAtom() : type(JSON_TYPE_NULL)
{
}

JSONAtom::JSONAtom(JSONAtomType atomType) : type(atomType)
{
}

JSONAtom::~JSONAtom()
{
}

JSONAtomType JSONAtom::getType()
{
	return type;
}

void *JSONAtom::asNull() const
{
	if (type != JSON_TYPE_NULL)
		throw JSONTypeError(type, JSON_TYPE_NULL);
	return NULL;
}

bool JSONAtom::asBool() const
{
	if (type != JSON_TYPE_BOOL)
		throw JSONTypeError(type, JSON_TYPE_BOOL);
	return *((JSONBool *)this);
}

int JSONAtom::asInt() const
{
	if (type != JSON_TYPE_INT)
		throw JSONTypeError(type, JSON_TYPE_INT);
	return *((JSONInt *)this);
}

double JSONAtom::asFloat() const
{
	if (type != JSON_TYPE_FLOAT)
		throw JSONTypeError(type, JSON_TYPE_FLOAT);
	return *((JSONFloat *)this);
}

const char *JSONAtom::asString() const
{
	if (type != JSON_TYPE_STRING)
		throw JSONTypeError(type, JSON_TYPE_STRING);
	return *((JSONString *)this);
}

JSONObject *JSONAtom::asObject() const
{
	if (type != JSON_TYPE_OBJECT)
		throw JSONTypeError(type, JSON_TYPE_OBJECT);
	return (JSONObject *)this;
}

JSONArray *JSONAtom::asArray() const
{
	if (type != JSON_TYPE_ARRAY)
		throw JSONTypeError(type, JSON_TYPE_ARRAY);
	return (JSONArray *)this;
}
