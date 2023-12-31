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

#ifndef rSON_HXX
#define rSON_HXX

// This ensures that this header is only ever used in C++11 or newer mode
#if __cplusplus < 201103L
#error "This library must be used in C++11 or newer mode, it cannot be used in C++98 mode."
#endif

#include <cstdint>
#include <cstring>
#include <memory>
#include <vector>
#include <exception>
#include <stdexcept>
#include <string>
#if __cplusplus >= 201703L
#include <filesystem>
#include <string_view>
#endif
#include <type_traits>
#include <utility>
#include <fcntl.h>

#ifdef _WIN32
#	ifdef rSON_EXPORT_API
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
		virtual void readSync() noexcept { }
		virtual void writeSync() noexcept { }
	};

	struct rSON_CLS_API fileStream_t final : public stream_t
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
		~fileStream_t() noexcept final;
		fileStream_t &operator =(const fileStream_t &) = delete;
		fileStream_t &operator =(fileStream_t &&) = default;

		bool read(void *const value, const size_t valueLen, size_t &actualLen) final;
		bool write(const void *const value, const size_t valueLen) final;
		bool atEOF() const noexcept final { return eof; }
		off_t seek(const off_t offset, const int whence) noexcept;
		size_t size() const noexcept { return length; }
		bool valid() const noexcept { return fd != -1; }
	};

	struct rSON_DEFAULT_VISIBILITY memoryStream_t final : public stream_t
	{
	private:
		char *const memory;
		const size_t length;
		size_t pos;

	public:
		memoryStream_t(void *const stream, const size_t streamLength) noexcept;

		bool read(void *const value, const size_t valueLen, size_t &actualLen) noexcept final;
		bool write(const void *const value, const size_t valueLen) noexcept final;
		bool atEOF() const noexcept final { return pos == length; }
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
	class rSON_DEFAULT_VISIBILITY JSONParserError final : public std::exception
	{
	private:
		JSONParserErrorType parserError;

	public:
		JSONParserError(JSONParserErrorType errorType) : parserError(errorType) { }
		JSONParserErrorType errorType() const noexcept { return parserError; }
		const char *error() const noexcept;
		const char *what() const noexcept final { return error(); }
	};

	class rSON_DEFAULT_VISIBILITY JSONTypeError final : public std::exception
	{
	private:
		std::unique_ptr<const char []> errorStr{nullptr};
		const char *typeToString(JSONAtomType type) const noexcept;

	public:
		JSONTypeError(JSONAtomType actual, JSONAtomType expected);
#if __cplusplus >= 201103L
		JSONTypeError(JSONTypeError &&error) noexcept : errorStr(std::move(error.errorStr)) { }
#endif
		const char *error() const noexcept { return errorStr.get(); }
		const char *what() const noexcept final { return error(); }
	};

	class rSON_DEFAULT_VISIBILITY JSONObjectError final : public std::exception
	{
	private:
		JSONObjectErrorType objectError;

	public:
		JSONObjectError(JSONObjectErrorType errorType) : objectError(errorType) { }
		const char *error() const noexcept;
		const char *what() const noexcept final { return error(); }
	};

	class rSON_DEFAULT_VISIBILITY JSONArrayError final : public std::exception
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

		template<typename> struct isBoolean_ : std::false_type { };
		template<> struct isBoolean_<bool> : std::true_type { };

		template<typename T> struct isBoolean : isBoolean_<typename std::remove_cv<T>::type> { };
	}

	template<typename T> struct OpaquePtr final
	{
	private:
		T *ptr;
		internal::delete_t deleteFunc;

	public:
		constexpr OpaquePtr() noexcept : ptr{nullptr}, deleteFunc{nullptr} { }
		OpaquePtr(T *p, internal::delete_t &&del) noexcept : ptr{p}, deleteFunc{del} { }
		OpaquePtr(OpaquePtr &&p) noexcept : OpaquePtr{} { swap(p); }
		~OpaquePtr() noexcept { if (deleteFunc) deleteFunc(ptr); }
		OpaquePtr &operator =(OpaquePtr &&p) noexcept { swap(p); return *this; }

		operator T &() const noexcept { return *ptr; }
		explicit operator T &&() const = delete;
		T &operator *() noexcept { return *ptr; }
		const T &operator *() const noexcept { return *ptr; }
		T *operator ->() noexcept { return ptr; }
		const T *operator ->() const noexcept { return ptr; }
		explicit operator bool() const noexcept { return ptr; }

		void swap(OpaquePtr &p) noexcept
		{
			std::swap(ptr, p.ptr);
			std::swap(deleteFunc, p.deleteFunc);
		}

		OpaquePtr(const OpaquePtr &) = delete;
		OpaquePtr &operator =(const OpaquePtr &) = delete;
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
		// We already have a virtual distructor so can safely delete.. perhaps virtual assignment operators
		// will work.. have to figure out how to make move construction work with this (or copy construction).
		virtual ~JSONAtom() { }
		JSONAtomType getType() const noexcept { return type; }
		virtual void store(stream_t &stream) const = 0;
		virtual size_t length() const = 0;

		bool isNull() const noexcept { return typeIs(JSON_TYPE_NULL); }
		void *asNull() const;
		bool asBool() const;
		int64_t asInt() const { return *this; }
		double asFloat() const { return *this; }
		const std::string &asString() const { return *this; }
		JSONString &asStringRef() const;
		JSONObject *asObject() const;
		JSONObject &asObjectRef() const { return *this; }
		JSONArray *asArray() const;
		JSONArray &asArrayRef() const { return *this; }

		explicit operator bool() const { return asBool(); }
		operator int8_t() const { return int8_t(asInt()); }
		operator int16_t() const { return int16_t(asInt()); }
		operator int32_t() const { return int32_t(asInt()); }
		operator int64_t() const;
		operator double() const;
		operator float() const { return float(asFloat()); }
		operator const char *() const;
		operator const std::string &() const;
		operator JSONString &() const;
		operator JSONObject &() const;
		operator JSONArray &() const;

		// Turns out that it is useful to allow this type to be indexed.
		// These two operator index's automatically convert the JSONAtom as we can know if the index is
		// for an array or object based on the index type.
		JSONAtom &operator [](const char *const key) const;
		JSONAtom &operator [](const std::string &key) const;
#if __cplusplus >= 201703L
		JSONAtom &operator [](std::string_view key) const;
#endif
		JSONAtom &operator [](const size_t key) const;

		// Similarly, it's helpful to be able to add items to it if it's an array or object without having
		// to always do the type conversion explicitly.
		// JSONObject helpers
		bool add(std::string &&key, std::nullptr_t);
		bool add(std::string &&key, bool value);
		bool add(std::string &&key, int64_t value);
		bool add(std::string &&key, double value);
		bool add(std::string &&key, const std::string &value);
		bool add(std::string &&key, std::string &&value);
#if __cplusplus >= 201703L
		bool add(std::string &&key, const std::string_view &value);
#endif
		JSONArray *addArray(std::string &&key);
		JSONObject *addObject(std::string &&key);

		// Convert arbitrary integers to int64_t's to invoke the JSONInt handler properly
		template<typename T> typename std::enable_if<std::is_integral<T>::value && !internal::isBoolean<T>::value &&
			!std::is_enum<T>::value && !std::is_same<T, int64_t>::value>::type
				add(std::string &&key, const T value)
			{ add(std::move(key), static_cast<int64_t>(value)); }

		// Convert enums to their underlying integer type to feed into the JSONInt handling
		template<typename T> typename std::enable_if<std::is_enum<T>::value>::type
			add(std::string &&key, const T value)
				{ add(std::move(key), static_cast<typename std::underlying_type<T>::type>(value)); }

#if __cplusplus >= 201703L
		bool add(const std::string_view &key, std::nullptr_t);
		bool add(const std::string_view &key, bool value);
		bool add(const std::string_view &key, int64_t value);
		bool add(const std::string_view &key, double value);
		bool add(const std::string_view &key, const std::string &value);
		bool add(const std::string_view &key, std::string &&value);
		bool add(const std::string_view &key, const std::string_view &value);
		JSONArray *addArray(const std::string_view &key);
		JSONObject *addObject(const std::string_view &key);

		// Convert arbitrary integers to int64_t's to invoke the JSONInt handler properly
		template<typename T> typename std::enable_if<std::is_integral<T>::value && !internal::isBoolean<T>::value &&
			!std::is_enum<T>::value && !std::is_same<T, int64_t>::value>::type
				add(const std::string_view &key, const T value)
			{ add(key, static_cast<int64_t>(value)); }

		// Convert enums to their underlying integer type to feed into the JSONInt handling
		template<typename T> typename std::enable_if<std::is_enum<T>::value>::type
			add(const std::string_view &key, const T value)
				{ add(key, static_cast<typename std::underlying_type<T>::type>(value)); }
#endif

		// JSONArray helpers
		void add(std::nullptr_t);
		void add(bool value);
		void add(int64_t value);
		void add(double value);
		void add(const std::string &value);
		void add(std::string &&value);
#if __cplusplus >= 201703L
		void add(const std::string_view &value);
#endif
		JSONArray &addArray();
		JSONObject &addObject();

		// Convert arbitrary integers to int64_t's to invoke the JSONInt handler properly
		template<typename T> typename std::enable_if<std::is_integral<T>::value && !internal::isBoolean<T>::value &&
			!std::is_enum<T>::value && !std::is_same<T, int64_t>::value>::type add(const T value)
				{ add(static_cast<int64_t>(value)); }

		// Convert enums to their underlying integer type to feed into the JSONInt handling
		template<typename T> typename std::enable_if<std::is_enum<T>::value>::type add(const T value)
			{ add(static_cast<typename std::underlying_type<T>::type>(value)); }

		// Utility functions to help with type checking (validation)
		bool typeIs(const JSONAtomType atomType) const noexcept { return type == atomType; }
		bool typeIsOrNull(const JSONAtomType atomType) const noexcept
			{ return type == atomType || type == JSON_TYPE_NULL; }
	};

	// A copyable, moveable, container that behaves like a `std::optional<JSONAtom &>` logically
	// would if allowed, making correct usage of the library easier
	class rSON_DEFAULT_VISIBILITY JSONAtomContainer final
	{
	private:
		JSONAtom *atom{nullptr};

	public:
		constexpr JSONAtomContainer() noexcept = default;
		JSONAtomContainer(const std::unique_ptr<JSONAtom> &ptr) noexcept : atom{ptr.get()} { }
		constexpr JSONAtomContainer(JSONAtom *ptr) noexcept : atom{ptr} { }
		constexpr JSONAtomContainer(const JSONAtomContainer &) noexcept = default;
		constexpr JSONAtomContainer(JSONAtomContainer &&) noexcept = default;
		~JSONAtomContainer() noexcept = default;
		constexpr JSONAtomContainer &operator =(const JSONAtomContainer &) noexcept = default;
		constexpr JSONAtomContainer &operator =(JSONAtomContainer &&) noexcept = default;

		// Value validity checking helpers
		constexpr explicit operator bool() const noexcept { return hasValue(); }
		constexpr bool hasValue() const noexcept { return atom; }

		// Value retrival helpers (these throw std::logic_error if you try to ask for an invalid reference)
		constexpr JSONAtom &value() &
		{
			if (!hasValue())
				throw std::logic_error{"JSONAtomContainer does not contain a valid atom"};
			return *atom;
		}

		constexpr JSONAtom &value() const &
		{
			if (!hasValue())
				throw std::logic_error{"JSONAtomContainer does not contain a valid atom"};
			return *atom;
		}

		constexpr const JSONAtom *operator ->() const noexcept
			{ return atom; }
		constexpr JSONAtom *operator ->() noexcept
			{ return atom; }

		constexpr const JSONAtom &operator *() const noexcept
			{ return *atom; }
		constexpr JSONAtom &operator *() noexcept
			{ return *atom; }

		// Held value modifiers
		void swap(JSONAtomContainer &other) noexcept
			{ std::swap(atom, other.atom); }
		constexpr void reset() noexcept
			{ atom = nullptr; }

		// Comparison interface
		constexpr bool operator ==(const JSONAtomContainer &other) noexcept
			{ return atom == other.atom; }
		constexpr bool operator !=(const JSONAtomContainer &other) noexcept
			{ return atom != other.atom; }
	};

	class rSON_CLS_API JSONNull final : public JSONAtom
	{
	public:
		JSONNull();
		~JSONNull();
		size_t length() const final;
		void store(stream_t &stream) const final;
	};

	class rSON_CLS_API JSONFloat : public JSONAtom
	{
		double value;

	public:
		JSONFloat(double floatValue);
		~JSONFloat();
		operator double() const;
		size_t length() const final;
		void store(stream_t &stream) const final;
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
		size_t length() const final;
		void store(stream_t &stream) const final;
	};

	class rSON_DEFAULT_VISIBILITY JSONString : public JSONAtom
	{
	private:
		OpaquePtr<internal::string_t> str;

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
		const std::string &get() const noexcept { return *this; }
		size_t len() const noexcept;
		size_t size() const noexcept { return len(); }
		size_t length() const final;
		void store(stream_t &stream) const final;

		bool isIn(const char *const _value) const noexcept;
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
		size_t length() const final;
		void store(stream_t &stream) const final;
	};

	class rSON_DEFAULT_VISIBILITY JSONObject : public JSONAtom
	{
	private:
		OpaquePtr<internal::object_t> obj;

	public:
		JSONObject();
		JSONObject(JSONObject &object);
		~JSONObject() override = default;

		bool add(const char *const key, std::unique_ptr<JSONAtom> &&value);
		bool add(const char *const key, JSONAtom *value);
		bool add(const char *const key, std::nullptr_t);
		bool add(const char *const key, bool value);
		bool add(const char *const key, int64_t value);
		bool add(const char *const key, double value);
		bool add(const char *const key, const std::string &value);
		bool add(const char *const key, std::string &&value);
#if __cplusplus >= 201703L
		bool add(const char *const key, const std::string_view &value);
#endif
		JSONArray *addArray(const char *const key);
		JSONObject *addObject(const char *const key);

		bool add(std::string &&key, std::unique_ptr<JSONAtom> &&value);
		bool add(std::string &&key, JSONAtom *value);
		bool add(std::string &&key, std::nullptr_t);
		bool add(std::string &&key, bool value);
		bool add(std::string &&key, int64_t value);
		bool add(std::string &&key, double value);
		bool add(std::string &&key, const std::string &value);
		bool add(std::string &&key, std::string &&value);
#if __cplusplus >= 201703L
		bool add(std::string &&key, const std::string_view &value);
#endif
		JSONArray *addArray(std::string &&key);
		JSONObject *addObject(std::string &&key);

		// Convert arbitrary integers to int64_t's to invoke the JSONInt handler properly
		template<typename T> typename std::enable_if<std::is_integral<T>::value && !internal::isBoolean<T>::value &&
			!std::is_enum<T>::value && !std::is_same<T, int64_t>::value>::type
				add(std::string &&key, const T value)
			{ add(std::move(key), static_cast<int64_t>(value)); }

		// Convert enums to their underlying integer type to feed into the JSONInt handling
		template<typename T> typename std::enable_if<std::is_enum<T>::value>::type
			add(std::string &&key, const T value)
				{ add(std::move(key), static_cast<typename std::underlying_type<T>::type>(value)); }

#if __cplusplus >= 201703L
		bool add(const std::string_view &key, std::unique_ptr<JSONAtom> &&value);
		bool add(const std::string_view &key, JSONAtom *value);
		bool add(const std::string_view &key, std::nullptr_t);
		bool add(const std::string_view &key, bool value);
		bool add(const std::string_view &key, int64_t value);
		bool add(const std::string_view &key, double value);
		bool add(const std::string_view &key, const std::string &value);
		bool add(const std::string_view &key, std::string &&value);
		bool add(const std::string_view &key, const std::string_view &value);
		JSONArray *addArray(const std::string_view &key);
		JSONObject *addObject(const std::string_view &key);

		// Convert arbitrary integers to int64_t's to invoke the JSONInt handler properly
		template<typename T> typename std::enable_if<std::is_integral<T>::value && !internal::isBoolean<T>::value &&
			!std::is_enum<T>::value && !std::is_same<T, int64_t>::value>::type
				add(const std::string_view &key, const T value)
			{ add(key, static_cast<int64_t>(value)); }

		// Convert enums to their underlying integer type to feed into the JSONInt handling
		template<typename T> typename std::enable_if<std::is_enum<T>::value>::type
			add(const std::string_view &key, const T value)
				{ add(key, static_cast<typename std::underlying_type<T>::type>(value)); }
#endif

		void del(const char *const key);
		void del(const std::string &key);
#if __cplusplus >= 201703L
		void del(std::string_view key);
#endif
		JSONAtom &operator [](const char *const key) const;
		JSONAtom &operator [](const std::string &key) const;
#if __cplusplus >= 201703L
		JSONAtom &operator [](std::string_view key) const;
#endif
		const std::vector<const char *> &keys() const;
		bool exists(const char *const key) const;
		bool exists(const std::string &key) const;
#if __cplusplus >= 201703L
		bool exists(std::string_view key) const;
#endif
		size_t size() const;
		size_t count() const { return size(); }
		size_t length() const final;
		void store(stream_t &stream) const final;
	};

	class rSON_DEFAULT_VISIBILITY JSONArray : public JSONAtom
	{
	private:
		OpaquePtr<internal::array_t> arr;

	public:
		using iterator = const std::unique_ptr<JSONAtom> *;

		JSONArray();
		JSONArray(JSONArray &array);
		~JSONArray() override = default;

		void add(std::unique_ptr<JSONAtom> &&value);
		void add(JSONAtom *value);
		void add(std::nullptr_t);
		void add(bool value);
		void add(int64_t value);
		void add(double value);
		void add(const std::string &value);
		void add(std::string &&value);
#if __cplusplus >= 201703L
		void add(const std::string_view &value);
#endif
		JSONArray &addArray();
		JSONObject &addObject();

		// Convert arbitrary integers to int64_t's to invoke the JSONInt handler properly
		template<typename T> typename std::enable_if<std::is_integral<T>::value && !internal::isBoolean<T>::value &&
			!std::is_enum<T>::value && !std::is_same<T, int64_t>::value>::type add(const T value)
				{ add(static_cast<int64_t>(value)); }

		// Convert enums to their underlying integer type to feed into the JSONInt handling
		template<typename T> typename std::enable_if<std::is_enum<T>::value>::type add(const T value)
			{ add(static_cast<typename std::underlying_type<T>::type>(value)); }

		void del(const size_t key);
		void del(const JSONAtom *value);
		void del(const JSONAtom &value);
		JSONAtom &operator [](const size_t key) const;
		size_t size() const;
		size_t count() const { return size(); }
		iterator begin() noexcept;
		iterator begin() const noexcept;
		iterator end() noexcept;
		iterator end() const noexcept;
		size_t length() const final;
		void store(stream_t &stream) const final;
	};

	rSON_API std::unique_ptr<JSONAtom> parseJSON(stream_t &json);
	rSON_API std::unique_ptr<JSONAtom> parseJSON(const char *json);
	rSON_API std::unique_ptr<JSONAtom> parseJSON(const std::string &json);
#if __cplusplus >= 201703L
	rSON_API std::unique_ptr<JSONAtom> parseJSON(std::string_view json);
#endif

	rSON_API bool writeJSON(JSONAtomContainer atom, stream_t &stream);

	// Utility templates to help with type checking (validation)
	template<JSONAtomType type> bool typeIs(const JSONAtom &atom) noexcept { return atom.typeIs(type); }
	template<JSONAtomType type> bool typeIsOrNull(const JSONAtom &atom) noexcept { return atom.typeIsOrNull(type); }

#if __cplusplus >= 201703L
#	if _WINDOWS
	constexpr int32_t normalMode{O_RDONLY | O_BINARY};
#	else
	constexpr int32_t normalMode{O_RDONLY | O_NOCTTY};
#	endif

	[[nodiscard]] rSON_API inline std::unique_ptr<JSONAtom> parseJSON(const std::filesystem::path &fileName)
	{
		fileStream_t stream{fileName.c_str(), normalMode};
		return rSON::parseJSON(stream);
	}
#endif
}

#endif /*rSON_HXX*/
