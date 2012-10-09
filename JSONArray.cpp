#include "internal.h"

JSONArray::JSONArray() : JSONAtom(JSON_TYPE_ARRAY)
{
}

JSONArray::~JSONArray()
{
	for (size_t i = 0; i < children.size(); i++)
		delete children[i];
	children.clear();
}

void JSONArray::add(JSONAtom *value)
{
	children.push_back(value);
}

JSONAtom *JSONArray::operator [](size_t key)
{
	if (key >= children.size())
		throw JSONArrayError(JSON_ARRAY_OOB);
	return children[key];
}

size_t JSONArray::size()
{
	return children.size();
}
