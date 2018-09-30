/*
 * This file is part of rSON
 * Copyright © 2012-2018 Rachel Mant (dx-mon@users.sourceforge.net)
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

#ifndef rSON__H
#define rSON__H

#include <stdint.h>
#include <string.h>
#include <memory>
#include <vector>

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
#if __cplusplus >= 201103L && __cplusplus < 201402L
#define rSON_DEPRECATE(reason, type) [[gnu::deprecated(reason)]] rSON_API type
#elif __cplusplus >= 201402L
#define rSON_DEPRECATE(reason, type) [[deprecated(reason)]] rSON_API type
#else
#define rSON_DEPRECATE(reason, type) rSON_API type
#endif

#if __cplusplus >= 201103L
#define rSON_FINAL final
#define rSON_VFINAL final override
#else
#define rSON_FINAL
#define rSON_VFINAL
#endif

namespace rSON
{
	struct rSON_CLS_API notImplemented_t : public std::exception { };

	// Stream types for JSON IO
	struct stream_t
	{
	public:
		stream_t() = default;
		stream_t(const stream_t &) = delete;
		stream_t(stream_t &&) = default;
		virtual ~stream_t() = default;
		stream_t &operator =(const stream_t &) = delete;
		stream_t &operator =(stream_t &&) = default;

		template<typename T> bool read(T &value)
			{ return read(&value, sizeof(T)); }
		template<typename T, size_t N> bool read(std::array<T, N> &value)
			{ return read(value.data(), N); }

		template<typename T> bool write(const T &value)
			{ return write(&value, sizeof(T)); }
		template<typename T, size_t N> bool write(const std::array<T, N> &value)
			{ return write(value.data(), N); }

		bool read(void *const value, const size_t valueLen)
		{
			size_t actualLen = 0;
			if (!read(value, valueLen, actualLen))
				return false;
			return valueLen == actualLen;
		}

		virtual bool read(void *const, const size_t, size_t &) { throw notImplemented_t(); }
		virtual bool write(const void *const, const size_t) { throw notImplemented_t(); }
		virtual bool atEOF() const { throw notImplemented_t(); }
		virtual void readSync() noexcept { }
		virtual void writeSync() noexcept { }
	};

	struct rSON_CLS_API fileStream_t rSON_FINAL : public stream_t
	{
	private:
		int fd;
		size_t length;
		bool eof;
		int32_t mode;

	public:
		fileStream_t(const char *const fileName, const int32_t mode, const int32_t perms = 0);
		fileStream_t(const fileStream_t &) = delete;
		fileStream_t(fileStream_t &&) = default;
		~fileStream_t() noexcept final override;
		fileStream_t &operator =(const fileStream_t &) = delete;
		fileStream_t &operator =(fileStream_t &&) = default;

		bool read(void *const value, const size_t valueLen, size_t &actualLen) final override;
		bool write(const void *const value, const size_t valueLen) final override;
		bool atEOF() const noexcept final override { return eof; }
		off_t seek(const off_t offset, const int whence) noexcept;
		size_t size() const noexcept { return length; }
		bool valid() const noexcept { return fd != -1; }
	};

	struct rSON_CLS_API memoryStream_t rSON_FINAL : public stream_t
	{
	private:
		char *const memory;
		const size_t length;
		size_t pos;

	public:
		memoryStream_t(void *const stream, const size_t streamLength) noexcept;

		bool read(void *const value, const size_t valueLen, size_t &actualLen) noexcept final override;
		bool write(const void *const value, const size_t valueLen) noexcept final override;
		bool atEOF() const noexcept final override { return pos == length; }
	};

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
		JSON_ARRAY_OOB,
		JSON_ARRAY_BAD_ATOM
	} JSONArrayErrorType;

	// Exception classes
	class rSON_CLS_API JSONParserError rSON_FINAL
	{
	private:
		JSONParserErrorType parserError;

	public:
		JSONParserError(JSONParserErrorType errorType);
		JSONParserErrorType errorType() const;
		const char *error() const;
	};

	class rSON_CLS_API JSONTypeError rSON_FINAL
	{
	private:
		std::unique_ptr<const char []> errorStr;
		const char *typeToString(JSONAtomType type) const;

	public:
		JSONTypeError(JSONAtomType actual, JSONAtomType expected);
		JSONTypeError(JSONTypeError &&error) noexcept : errorStr(std::move(error.errorStr)) { }
		const char *error() const;
	};

	class rSON_CLS_API JSONObjectError rSON_FINAL
	{
	private:
		JSONObjectErrorType objectError;

	public:
		JSONObjectError(JSONObjectErrorType errorType);
		const char *error() const;
	};

	class rSON_CLS_API JSONArrayError rSON_FINAL
	{
	private:
		JSONArrayErrorType arrayError;

	public:
		JSONArrayError(JSONArrayErrorType errorType);
		const char *error() const;
	};

	// Impl types
	namespace internal
	{
		struct object_t;
		struct array_t;
	}

	template<typename T> struct managedPtr_t final
	{
	private:
		T *ptr;
		template<typename U> friend struct managedPtr_t;

	public:
		using pointer = T *;
		using reference = T &;

		constexpr managedPtr_t() noexcept : ptr(nullptr) { }
		managedPtr_t(T *p) noexcept : ptr(p) { }
		managedPtr_t(managedPtr_t &&p) noexcept : managedPtr_t() { swap(p); }
		template<typename U, typename = typename std::enable_if<!std::is_same<T, U>::value &&
			std::is_base_of<T, U>::value>::type> managedPtr_t(managedPtr_t<U> &&p) noexcept :
			managedPtr_t() { swap(p); }
		~managedPtr_t() noexcept { delete ptr; }
		managedPtr_t &operator =(managedPtr_t &&p) noexcept { swap(p); return *this; }

		operator T &() const noexcept { return *ptr; }
		explicit operator T &&() const = delete;
		T &operator *() const noexcept { return *ptr; }
		T *operator ->() const noexcept { return ptr; }
		T *get() noexcept { return ptr; }
		T *get() const noexcept { return ptr; }
		bool valid() const noexcept { return ptr; }
		explicit operator bool() const noexcept { return ptr; }
		void swap(managedPtr_t &p) noexcept { std::swap(ptr, p.ptr); }

		template<typename U, typename = typename std::enable_if<!std::is_same<T, U>::value &&
			std::is_base_of<T, U>::value>::type> void swap(managedPtr_t<U> &p) noexcept
		{
			delete ptr;
			ptr = p.ptr;
			p.ptr = nullptr;
		}

		managedPtr_t(const managedPtr_t &) = delete;
		managedPtr_t &operator =(const managedPtr_t &) = delete;
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
		// TODO: Implement full move semantics, which solves the issue of having pointers not references for all JSONAtom's.. well, maybe.
		// Move semantics would be good anyway as it makes handling a JSONAtom tree cheaper and easier.
		// We already have a virtual distructor so can safely delete.. perhaps virtual assignment operators will work.. have to figure out how to make move construction work with this (or copy construction).
		virtual ~JSONAtom() { }
		JSONAtomType getType() const noexcept { return type; }
		virtual void store(stream_t &stream) const = 0;
		virtual size_t length() const = 0;

		bool isNull() const noexcept { return typeIs(JSON_TYPE_NULL); }
		void *asNull() const;
		bool asBool() const { return *this; }
		int64_t asInt() const { return *this; }
		double asFloat() const { return *this; }
		const char *asString() const { return *this; }
		JSONString &asStringRef() const;
		JSONObject *asObject() const;
		JSONObject &asObjectRef() const { return *this; }
		JSONArray *asArray() const;
		JSONArray &asArrayRef() const { return *this; }

		operator JSONAtom *() const;
		operator bool() const;
		operator int8_t() const { return asInt(); }
		operator int16_t() const { return asInt(); }
		operator int32_t() const { return asInt(); }
		operator int64_t() const;
		operator double() const;
		operator float() const { return double(*this); }
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
	using jsonAtom_t = JSONAtom;
	using jsonAtomPtr_t = std::unique_ptr<JSONAtom>;

	class rSON_CLS_API JSONNull rSON_FINAL : public JSONAtom
	{
	public:
		JSONNull();
		~JSONNull();
		size_t length() const rSON_VFINAL;
		void store(stream_t &stream) const rSON_VFINAL;
	};

	class rSON_CLS_API JSONFloat : public JSONAtom
	{
		double value;

	public:
		JSONFloat(double floatValue);
		~JSONFloat();
		operator double() const;
		size_t length() const rSON_VFINAL;
		void store(stream_t &stream) const rSON_VFINAL;
	};

	class rSON_CLS_API JSONInt : public JSONAtom
	{
	private:
		int64_t value;

	public:
		JSONInt(int64_t intValue);
		~JSONInt();
		operator int64_t() const;
		void set(int64_t intValue);
		size_t length() const rSON_VFINAL;
		void store(stream_t &stream) const rSON_VFINAL;
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
		const char *get() const noexcept { return value; }
		size_t len() const;
		size_t length() const rSON_VFINAL;
		void store(stream_t &stream) const rSON_VFINAL;

		bool isIn(const char *const _value) const noexcept { return strcmp(value, _value) == 0; }
		template<typename... Values> bool isIn(const char *const _value, Values ...values) const noexcept
		{
			if (isIn(_value))
				return true;
			return isIn(values...);
		}
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
		size_t length() const rSON_VFINAL;
		void store(stream_t &stream) const rSON_VFINAL;
	};

	class JSONObject : public JSONAtom
	{
	private:
		managedPtr_t<internal::object_t> obj;

	public:
		rSON_CLS_API JSONObject();
		rSON_CLS_API JSONObject(JSONObject &object);
		rSON_CLS_API void add(const char *const key, jsonAtomPtr_t &&value);
		rSON_CLS_API void add(const char *const key, JSONAtom *value);
		rSON_CLS_API void del(const char *const key);
		rSON_CLS_API JSONAtom &operator [](const char *const key) const;
		rSON_CLS_API const std::vector<const char *> &keys() const;
		rSON_CLS_API bool exists(const char *const key) const;
		rSON_CLS_API size_t size() const;
		rSON_CLS_API size_t count() const { return size(); }
		rSON_CLS_API size_t length() const rSON_VFINAL;
		rSON_CLS_API void store(stream_t &stream) const rSON_VFINAL;
	};
	using jsonObject_t = JSONObject;

	class JSONArray : public JSONAtom
	{
	private:
		managedPtr_t<internal::array_t> arr;

	public:
		using iterator = const jsonAtomPtr_t *;

		rSON_CLS_API JSONArray();
		rSON_CLS_API JSONArray(JSONArray &array);
		rSON_CLS_API void add(jsonAtomPtr_t &&value);
		rSON_CLS_API void add(JSONAtom *value);
		rSON_CLS_API void del(const size_t key);
		rSON_CLS_API void del(const JSONAtom *value);
		rSON_CLS_API void del(const JSONAtom &value);
		rSON_CLS_API JSONAtom &operator [](const size_t key) const;
		rSON_CLS_API size_t size() const;
		rSON_CLS_API size_t count() const { return size(); }
		rSON_CLS_API iterator begin() const;
		rSON_CLS_API iterator end() const;
		rSON_CLS_API size_t length() const rSON_VFINAL;
		rSON_CLS_API void store(stream_t &stream) const rSON_VFINAL;
	};
	using jsonArray_t = JSONArray;

	rSON_API JSONAtom *parseJSON(stream_t &json);
	rSON_API JSONAtom *parseJSON(const char *json);
	rSON_DEPRECATE("parseJSON(stream_t &) fully replaces this call", JSONAtom *) parseJSONFile(const char *file);

	rSON_API void writeJSON(const JSONAtom *const atom, stream_t &stream);
	rSON_DEPRECATE("writeJSON(const JSONAtom *const, stream_t &) fully replaces this call", char *) writeJSON(JSONAtom *atom);
	rSON_DEPRECATE("If not using deprecated form of writeJSON(), this must not be called", void) freeString(char **str);

	// Utility templates to help with type checking (validation)
	template<JSONAtomType type> bool typeIs(const JSONAtom &atom) noexcept { return atom.typeIs(type); }
	template<JSONAtomType type> bool typeIsOrNull(const JSONAtom &atom) noexcept { return atom.typeIsOrNull(type); }
}

#endif /*rSON__H*/
