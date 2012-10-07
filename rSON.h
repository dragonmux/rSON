#ifndef __rSON_H__
#define __rSON_H__

#include <inttypes.h>

class JSONAtom
{
private:
	JSONAtom();

public:
	virtual ~JSONAtom();
};

class JSONFloat : JSONAtom
{
};

class JSONInt : JSONAtom
{
};

class JSONString : JSONAtom
{
};

class JSONBool : JSONAtom
{
};

class JSONNode : public JSONAtom
{
public:
	~JSONNode();
};

class JSONObject : public JSONNode
{
public:
	~JSONObject();
	JSONAtom *operator [](const char *key);
};

class JSONArray : public JSONNode
{
public:
	~JSONArray();
	JSONAtom *operator [](uint32_t key);
};

#endif /*__rSON_H__*/
