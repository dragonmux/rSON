/*
 * This file is part of rSON
 * Copyright © 2012-2020 Rachel Mant (dx-mon@users.sourceforge.net)
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

#include <algorithm>
#include "internal/types.hxx"
#include "internal/string.hxx"

#if !defined(_MSC_VER) || _MSC_VER >= 1928
JSONObject::JSONObject() : JSONAtom{JSON_TYPE_OBJECT}, obj{makeOpaque<object_t>()} { }
#else
JSONObject::JSONObject() : JSONAtom{JSON_TYPE_OBJECT}, obj{} { }
#endif

JSONObject::JSONObject(JSONObject &object) : JSONObject{}
	{ obj->clone(*object.obj); }

void object_t::clone(const object_t &object)
{
	for (const auto &atom : object)
	{
		add(std::string{atom.first}, [](const JSONAtom &value) -> std::unique_ptr<JSONAtom>
		{
			switch (value.getType())
			{
				case JSON_TYPE_NULL:
					return std::make_unique<JSONNull>();
				case JSON_TYPE_BOOL:
					return std::make_unique<JSONBool>(value.asBool());
				case JSON_TYPE_INT:
					return std::make_unique<JSONInt>(value);
				case JSON_TYPE_FLOAT:
					return std::make_unique<JSONFloat>(value);
				case JSON_TYPE_STRING:
					return std::make_unique<JSONString>(value.asString());
				case JSON_TYPE_OBJECT:
					return std::make_unique<JSONObject>(value);
				case JSON_TYPE_ARRAY:
					return std::make_unique<JSONArray>(value);
				default:
					throw JSONObjectError(JSON_OBJECT_BAD_KEY);
			}
		}(*atom.second));
	}
}

JSONAtom *object_t::add(std::string &&key, std::unique_ptr<JSONAtom> &&value)
{
	if (children.find(key) != children.end())
		return nullptr;
	const auto result{children.emplace(std::move(key), std::move(value))};
	if (result.second)
	{
		mapKeys.push_back(result.first->first.c_str());
		return result.first->second.get();
	}
	return nullptr;
}

void object_t::del(const std::string_view &key)
{
	if (key.empty())
		return;
	const auto &atom = children.find(key);
	if (atom != children.end())
	{
		const auto &atomKey = std::find_if(mapKeys.begin(), mapKeys.end(),
			[&](const std::string_view atom) -> bool { return key == atom; });
		mapKeys.erase(atomKey);
		children.erase(atom);
	}
}

JSONAtom &object_t::operator [](const std::string_view &key) const
{
	const auto &node = children.find(key);
	if (node == children.end())
		throw JSONObjectError(JSON_OBJECT_BAD_KEY);
	return *node->second;
}

bool object_t::exists(const std::string_view &key) const noexcept
	{ return children.find(key) != children.end(); }

bool JSONObject::add(const char *const key, std::unique_ptr<JSONAtom> &&value)
	{ return obj->add(key, std::move(value)); }
bool JSONObject::add(const char *const key, JSONAtom *value)
	{ return obj->add(key, std::unique_ptr<JSONAtom>{value}); }

void JSONObject::del(const char *const key)
{
	/* Make sure the key is not nullptr before delegating into the normal delete logic */
	if (key)
		obj->del(key);
}

void JSONObject::del(const std::string &key)
	{ obj->del(key); }
void JSONObject::del(const std::string_view key)
	{ obj->del(key); }

JSONAtom &JSONObject::operator [](const char *const key) const
{
	/* Make sure the key is not nullptr before delegating into the normal lookup logic */
	if (key)
		return (*obj)[key];
	throw JSONObjectError(JSON_OBJECT_BAD_KEY);
}

JSONAtom &JSONObject::operator [](const std::string &key) const
	{ return (*obj)[key]; }
JSONAtom &JSONObject::operator [](const std::string_view key) const
	{ return (*obj)[key]; }
const std::vector<const char *> &JSONObject::keys() const { return obj->keys(); }

bool JSONObject::exists(const char *const key) const
{
	/* Make sure the key is not nullptr before delegating into the normal existance logic */
	if (key)
		return obj->exists(key);
	return false;
}

bool JSONObject::exists(const std::string &key) const
	{ return obj->exists(key); }
bool JSONObject::exists(const std::string_view key) const
	{ return obj->exists(key); }
size_t JSONObject::size() const { return obj->size(); }

bool JSONObject::add(const char *const key, std::nullptr_t)
	{ return obj->add(key, std::make_unique<JSONNull>()); }
bool JSONObject::add(const char *const key, const bool value)
	{ return obj->add(key, std::make_unique<JSONBool>(value)); }
bool JSONObject::add(const char *const key, const int64_t value)
	{ return obj->add(key, std::make_unique<JSONInt>(value)); }
bool JSONObject::add(const char *const key, const double value)
	{ return obj->add(key, std::make_unique<JSONFloat>(value)); }
bool JSONObject::add(const char *const key, const std::string &value)
	{ return obj->add(key, std::make_unique<JSONString>(value)); }
bool JSONObject::add(const char *const key, std::string &&value)
	{ return obj->add(key, std::make_unique<JSONString>(std::move(value))); }
bool JSONObject::add(const char *const key, const std::string_view &value)
	{ return obj->add(key, std::make_unique<JSONString>(value)); }

JSONArray *JSONObject::addArray(const char *const key)
{
	const auto result{obj->add(key, std::make_unique<JSONArray>())};
	return static_cast<JSONArray *>(result);
}

JSONObject *JSONObject::addObject(const char *const key)
{
	const auto result{obj->add(key, std::make_unique<JSONObject>())};
	return static_cast<JSONObject *>(result);
}

bool JSONObject::add(std::string &&key, std::unique_ptr<JSONAtom> &&value)
	{ return obj->add(std::move(key), std::move(value)); }
bool JSONObject::add(std::string &&key, JSONAtom *value)
	{ return obj->add(std::move(key), std::unique_ptr<JSONAtom>{value}); }
bool JSONObject::add(std::string &&key, std::nullptr_t)
	{ return obj->add(std::move(key), std::make_unique<JSONNull>()); }
bool JSONObject::add(std::string &&key, const bool value)
	{ return obj->add(std::move(key), std::make_unique<JSONBool>(value)); }
bool JSONObject::add(std::string &&key, const int64_t value)
	{ return obj->add(std::move(key), std::make_unique<JSONInt>(value)); }
bool JSONObject::add(std::string &&key, const double value)
	{ return obj->add(std::move(key), std::make_unique<JSONFloat>(value)); }
bool JSONObject::add(std::string &&key, const std::string &value)
	{ return obj->add(std::move(key), std::make_unique<JSONString>(value)); }
bool JSONObject::add(std::string &&key, std::string &&value)
	{ return obj->add(std::move(key), std::make_unique<JSONString>(std::move(value))); }
bool JSONObject::add(std::string &&key, const std::string_view &value)
	{ return obj->add(std::move(key), std::make_unique<JSONString>(value)); }

JSONArray *JSONObject::addArray(std::string &&key)
{
	auto *const result{obj->add(std::move(key), std::make_unique<JSONArray>())};
	return static_cast<JSONArray *>(result);
}

JSONObject *JSONObject::addObject(std::string &&key)
{
	auto *const result{obj->add(std::move(key), std::make_unique<JSONObject>())};
	return static_cast<JSONObject *>(result);
}

bool JSONObject::add(const std::string_view &key, std::unique_ptr<JSONAtom> &&value)
	{ return obj->add(std::string{key}, std::move(value)); }
bool JSONObject::add(const std::string_view &key, JSONAtom *value)
	{ return obj->add(std::string{key}, std::unique_ptr<JSONAtom>{value}); }
bool JSONObject::add(const std::string_view &key, std::nullptr_t)
	{ return obj->add(std::string{key}, std::make_unique<JSONNull>()); }
bool JSONObject::add(const std::string_view &key, const bool value)
	{ return obj->add(std::string{key}, std::make_unique<JSONBool>(value)); }
bool JSONObject::add(const std::string_view &key, const int64_t value)
	{ return obj->add(std::string{key}, std::make_unique<JSONInt>(value)); }
bool JSONObject::add(const std::string_view &key, const double value)
	{ return obj->add(std::string{key}, std::make_unique<JSONFloat>(value)); }
bool JSONObject::add(const std::string_view &key, const std::string &value)
	{ return obj->add(std::string{key}, std::make_unique<JSONString>(value)); }
bool JSONObject::add(const std::string_view &key, std::string &&value)
	{ return obj->add(std::string{key}, std::make_unique<JSONString>(std::move(value))); }
bool JSONObject::add(const std::string_view &key, const std::string_view &value)
	{ return obj->add(std::string{key}, std::make_unique<JSONString>(value)); }

JSONArray *JSONObject::addArray(const std::string_view &key)
{
	auto *const result{obj->add(std::string{key}, std::make_unique<JSONArray>())};
	return static_cast<JSONArray *>(result);
}

JSONObject *JSONObject::addObject(const std::string_view &key)
{
	auto *const result{obj->add(std::string{key}, std::make_unique<JSONObject>())};
	return static_cast<JSONObject *>(result);
}
