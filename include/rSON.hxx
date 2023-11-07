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

#ifndef rSON__H
#define rSON__H

#include <stdint.h>
#include <string.h>
#include <memory>
#include <vector>
#include <exception>
#include <string>
#if __cplusplus >= 201703L
#include <filesystem>
#include <string_view>
#endif
#include <fcntl.h>

#ifdef _WIN32
#	ifdef __rSON__
#		define rSON_CLS_API __declspec(dllexport)
#	else
#		define rSON_CLS_API __declspec(dllimport)
#	endif
#	define rSON_DEFAULT_VISIBILITY
#	define rSON_API	extern rSON_CLS_API
#else
#	if __GNUC__ >= 4
#		define rSON_DEFAULT_VISIBILITY __attribute__ ((visibility("default")))
#	else
#		error "This library cannot be compiled or used correctly with a GCC less than 4.x series"
#	endif
#	define rSON_CLS_API rSON_DEFAULT_VISIBILITY
#	define rSON_API extern rSON_CLS_API
#	include <sys/types.h>
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
#define rSON_NOEXCEPT noexcept
#else
#define rSON_FINAL
#define rSON_VFINAL
#define rSON_NOEXCEPT
#endif

namespace rSON
{
	struct notImplemented_t : public std::exception { };

	// Stream types for JSON IO
	struct rSON_CLS_API stream_t
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
		virtual void readSync() rSON_NOEXCEPT { }
		virtual void writeSync() rSON_NOEXCEPT { }
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
		~fileStream_t() rSON_NOEXCEPT rSON_VFINAL;
		fileStream_t &operator =(const fileStream_t &) = delete;
		fileStream_t &operator =(fileStream_t &&) = default;

		bool read(void *const value, const size_t valueLen, size_t &actualLen) rSON_VFINAL;
		bool write(const void *const value, const size_t valueLen) rSON_VFINAL;
		bool atEOF() const rSON_NOEXCEPT rSON_VFINAL { return eof; }
		off_t seek(const off_t offset, const int whence) rSON_NOEXCEPT;
		size_t size() const rSON_NOEXCEPT { return length; }
		bool valid() const rSON_NOEXCEPT { return fd != -1; }
	};

	struct rSON_DEFAULT_VISIBILITY memoryStream_t rSON_FINAL : public stream_t
	{
	private:
		char *const memory;
		const size_t length;
		size_t pos;

	public:
		memoryStream_t(void *const stream, const size_t streamLength) rSON_NOEXCEPT;

		bool read(void *const value, const size_t valueLen, size_t &actualLen) rSON_NOEXCEPT rSON_VFINAL;
		bool write(const void *const value, const size_t valueLen) rSON_NOEXCEPT rSON_VFINAL;
		bool atEOF() const rSON_NOEXCEPT rSON_VFINAL { return pos == length; }
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
	class rSON_DEFAULT_VISIBILITY JSONParserError rSON_FINAL : public std::exception
	{
	private:
		JSONParserErrorType parserError;

	public:
		JSONParserError(JSONParserErrorType errorType) : parserError(errorType) { }
		JSONParserErrorType errorType() const noexcept { return parserError; }
		const char *error() const noexcept;
		const char *what() const noexcept final { return error(); }
	};

	class rSON_DEFAULT_VISIBILITY JSONTypeError rSON_FINAL : public std::exception
	{
	private:
		std::unique_ptr<const char []> errorStr{nullptr};
		const char *typeToString(JSONAtomType type) const noexcept;

	public:
		JSONTypeError(JSONAtomType actual, JSONAtomType expected);
#if __cplusplus >= 201103L
		JSONTypeError(JSONTypeError &&error) rSON_NOEXCEPT : errorStr(std::move(error.errorStr)) { }
#endif
		const char *error() const noexcept { return errorStr.get(); }
		const char *what() const noexcept final { return error(); }
	};

	class rSON_DEFAULT_VISIBILITY JSONObjectError rSON_FINAL : public std::exception
	{
	private:
		JSONObjectErrorType objectError;

	public:
		JSONObjectError(JSONObjectErrorType errorType) : objectError(errorType) { }
		const char *error() const noexcept;
		const char *what() const noexcept final { return error(); }
	};

	class rSON_DEFAULT_VISIBILITY JSONArrayError rSON_FINAL : public std::exception
	{
	private:
		JSONArrayErrorType arrayError;

	public:
		JSONArrayError(JSONArrayErrorType errorType) : arrayError(errorType) { }
		const char *error() const noexcept;
		const char *what() const noexcept final { return error(); }
	};

	// Impl types
	namespace internal
	{
		using delete_t = void (*)(void *const);

		struct string_t;
		struct object_t;
		struct array_t;
	}

	template<typename T> struct opaquePtr_t final
	{
	private:
		T *ptr;
		internal::delete_t deleteFunc;

	public:
		constexpr opaquePtr_t() rSON_NOEXCEPT : ptr{nullptr}, deleteFunc{nullptr} { }
		opaquePtr_t(T *p, internal::delete_t &&del) rSON_NOEXCEPT : ptr{p}, deleteFunc{del} { }
		opaquePtr_t(opaquePtr_t &&p) rSON_NOEXCEPT : opaquePtr_t{} { swap(p); }
		~opaquePtr_t() rSON_NOEXCEPT { if (deleteFunc) deleteFunc(ptr); }
		opaquePtr_t &operator =(opaquePtr_t &&p) rSON_NOEXCEPT { swap(p); return *this; }

		operator T &() const rSON_NOEXCEPT { return *ptr; }
		explicit operator T &&() const = delete;
		T &operator *() rSON_NOEXCEPT { return *ptr; }
		const T &operator *() const rSON_NOEXCEPT { return *ptr; }
		T *operator ->() rSON_NOEXCEPT { return ptr; }
		const T *operator ->() const rSON_NOEXCEPT { return ptr; }
		explicit operator bool() const rSON_NOEXCEPT { return ptr; }

		void swap(opaquePtr_t &p) rSON_NOEXCEPT
		{
			std::swap(ptr, p.ptr);
			std::swap(deleteFunc, p.deleteFunc);
		}

		opaquePtr_t(const opaquePtr_t &) = delete;
		opaquePtr_t &operator =(const opaquePtr_t &) = delete;
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
		constexpr JSONAtom() rSON_NOEXCEPT : type(JSON_TYPE_NULL) { }
		constexpr JSONAtom(const JSONAtomType atomType) rSON_NOEXCEPT : type(atomType) { }

	public:
		// TODO: Implement full move semantics, which solves the issue of having pointers not references for all JSONAtom's.. well, maybe.
		// Move semantics would be good anyway as it makes handling a JSONAtom tree cheaper and easier.
		// We already have a virtual distructor so can safely delete.. perhaps virtual assignment operators will work.. have to figure out how to make move construction work with this (or copy construction).
		virtual ~JSONAtom() { }
		JSONAtomType getType() const rSON_NOEXCEPT { return type; }
		virtual void store(stream_t &stream) const = 0;
		virtual size_t length() const = 0;

		bool isNull() const rSON_NOEXCEPT { return typeIs(JSON_TYPE_NULL); }
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

		operator bool() const;
		operator int8_t() const { return int8_t(asInt()); }
		operator int16_t() const { return int16_t(asInt()); }
		operator int32_t() const { return int32_t(asInt()); }
		operator int64_t() const;
		operator double() const;
		operator float() const { return float(asFloat()); }
		operator const char *() const;
		operator JSONString &() const;
		operator JSONObject &() const;
		operator JSONArray &() const;

		// Turns out that it is useful to allow this type to be indexed.
		// These two operator index's automatically convert the JSONAtom as we can know if the index is for an array or object based on the index type.
		JSONAtom &operator [](const char *const key) const;
		JSONAtom &operator [](const size_t key) const;

		// Utility functions to help with type checking (validation)
		bool typeIs(const JSONAtomType atomType) const rSON_NOEXCEPT { return type == atomType; }
		bool typeIsOrNull(const JSONAtomType atomType) const rSON_NOEXCEPT { return type == atomType || type == JSON_TYPE_NULL; }
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

	class rSON_DEFAULT_VISIBILITY JSONString : public JSONAtom
	{
	private:
		opaquePtr_t<internal::string_t> str;

	public:
		JSONString(char *value, size_t length);
		JSONString(const char *value, size_t length);
		JSONString(const std::string &value);
		JSONString(std::string &&value);
#if __cplusplus >= 201703L
		JSONString(const std::string_view &value);
#endif
		JSONString(const JSONString &value) : JSONString{value.get()} { }
		~JSONString() override = default;
		operator const char *() const;
		operator const std::string &() const;
		void set(char *value);
		void set(const char *value);
		void set(const std::string &value);
		void set(std::string &&value);
#if __cplusplus >= 201703L
		void set(const std::string_view &value);
#endif
		const std::string &get() const rSON_NOEXCEPT { return *this; }
		size_t len() const noexcept;
		size_t size() const noexcept { return len(); }
		size_t length() const rSON_VFINAL;
		void store(stream_t &stream) const rSON_VFINAL;

		bool isIn(const char *const _value) const rSON_NOEXCEPT;
		template<typename... Values> bool isIn(const char *const _value, Values ...values) const rSON_NOEXCEPT
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

	class rSON_DEFAULT_VISIBILITY JSONObject : public JSONAtom
	{
	private:
		opaquePtr_t<internal::object_t> obj;

	public:
		JSONObject();
		JSONObject(JSONObject &object);
		~JSONObject() override = default;
		void add(const char *const key, jsonAtomPtr_t &&value);
		void add(const char *const key, JSONAtom *value);
		void del(const char *const key);
		JSONAtom &operator [](const char *const key) const;
		const std::vector<const char *> &keys() const;
		bool exists(const char *const key) const;
		size_t size() const;
		size_t count() const { return size(); }
		size_t length() const rSON_VFINAL;
		void store(stream_t &stream) const rSON_VFINAL;
	};
	using jsonObject_t = JSONObject;

	class rSON_DEFAULT_VISIBILITY JSONArray : public JSONAtom
	{
	private:
		opaquePtr_t<internal::array_t> arr;

	public:
		using iterator = const jsonAtomPtr_t *;

		JSONArray();
		JSONArray(JSONArray &array);
		~JSONArray() override = default;
		void add(jsonAtomPtr_t &&value);
		void add(JSONAtom *value);
		void del(const size_t key);
		void del(const JSONAtom *value);
		void del(const JSONAtom &value);
		JSONAtom &operator [](const size_t key) const;
		size_t size() const;
		size_t count() const { return size(); }
		iterator begin() rSON_NOEXCEPT;
		iterator begin() const rSON_NOEXCEPT;
		iterator end() rSON_NOEXCEPT;
		iterator end() const rSON_NOEXCEPT;
		size_t length() const rSON_VFINAL;
		void store(stream_t &stream) const rSON_VFINAL;
	};
	using jsonArray_t = JSONArray;

	rSON_API JSONAtom *parseJSON(stream_t &json);
	rSON_API JSONAtom *parseJSON(const char *json);

	rSON_API bool writeJSON(const JSONAtom *const atom, stream_t &stream);

	// Utility templates to help with type checking (validation)
	template<JSONAtomType type> bool typeIs(const JSONAtom &atom) rSON_NOEXCEPT { return atom.typeIs(type); }
	template<JSONAtomType type> bool typeIsOrNull(const JSONAtom &atom) rSON_NOEXCEPT { return atom.typeIsOrNull(type); }

#if __cplusplus >= 201703L
#	if _WINDOWS
	constexpr int32_t normalMode{O_RDONLY | O_BINARY};
#	else
	constexpr int32_t normalMode{O_RDONLY | O_NOCTTY};
#	endif

	[[nodiscard]] rSON_API inline JSONAtom *parseJSON(const std::filesystem::path &fileName)
	{
		fileStream_t stream{fileName.c_str(), normalMode};
		return rSON::parseJSON(stream);
	}
#endif
}

#endif /*rSON__H*/
