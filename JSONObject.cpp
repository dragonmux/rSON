#include "internal.h"

JSONObject::JSONObject() : JSONAtom(JSON_TYPE_OBJECT)
{
}

JSONObject::~JSONObject()
{
	for (atomMapIter i = children.begin(); i != children.end(); i++)
	{
		delete i->first;
		delete i->second;
	}
	children.clear();
}

void JSONObject::add(char *key, JSONAtom *value)
{
	atomMapIter node = children.find(key);
	if (node != children.end())
		return;
	children[key] = value;
}

JSONAtom *JSONObject::operator [](const char *key)
{
	atomMapIter node = children.find((char *)key);
	if (node == children.end())
		throw JSONObjectError(JSON_OBJECT_BAD_KEY);
	return node->second;
}

size_t JSONObject::size()
{
	return children.size();
}
