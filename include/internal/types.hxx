// SPDX-License-Identifier: LGPL-3.0-or-later
// SPDX-FileCopyrightText: 2012-2013,2018,2023 Rachel Mant <git@dragonmux.network>
// SPDX-FileCopyrightText: 2021 Amyspark <amy@amyspark.me>
// SPDX-FileContributor: Written by Rachel Mant <git@dragonmux.network>
// SPDX-FileContributor: Modified by Amyspark <amy@amyspark.me>

#ifndef INTERNAL_TYPES_HXX
#define INTERNAL_TYPES_HXX

#include <string>
#include <string_view>
#include <functional>
#include <map>
#include "rSON.hxx"

namespace rSON
{
	namespace internal
	{
		struct string_t final
		{
		private:
			std::string string{};

		public:
			string_t(std::string &&str);
			string_t(const std::string_view &str);
			string_t &operator =(string_t &&str) noexcept;
			const std::string &value() const noexcept { return string; }
			const char *data() const noexcept { return string.data(); }
			size_t size() const noexcept { return string.size(); }
			size_t length() const noexcept { return string.length(); }
			bool operator ==(const string_t &str) const noexcept { return str.value() == string; }
			bool operator <(const string_t &str) const noexcept { return str.value() < string; }
		};

		inline bool operator ==(const string_t &a, const std::string_view &b) noexcept
			{ return a.value() == b; }
		inline bool operator ==(const std::string_view &a, const string_t &b) noexcept
			{ return a == b.value(); }

		inline bool operator <(const string_t &a, const std::string_view &b) noexcept
			{ return a.value() < b; }
		inline bool operator <(const std::string_view &a, const string_t &b) noexcept
			{ return a < b.value(); }

		inline bool operator ==(const string_t &a, const char *const b) noexcept
			{ return a == std::string_view{b, strlen(b)}; }
		inline bool operator ==(const char *const a, const string_t &b) noexcept
			{ return std::string_view{a, strlen(a)} == b; }

		inline bool operator <(const string_t &a, const char *const b) noexcept
			{ return a < std::string_view{b, strlen(b)}; }
		inline bool operator <(const char *const a, const string_t &b) noexcept
			{ return std::string_view{a, strlen(a)} < b; }

		struct object_t final
		{
		private:
			using holder_t = std::map<std::string, std::unique_ptr<JSONAtom>, std::less<>>;
			using list_t = std::vector<const char *>;
			holder_t children{};
			list_t mapKeys{};

		public:
			using iter_t = holder_t::iterator;
			using constIter_t = holder_t::const_iterator;

			object_t() = default;
			void clone(const object_t &object);
			JSONAtom *add(std::string &&key, std::unique_ptr<JSONAtom> &&value);
			void del(const std::string_view &key);
			JSONAtom &operator [](const std::string_view &key) const;
			const list_t &keys() const noexcept { return mapKeys; }
			bool exists(const std::string_view &key) const noexcept;
			size_t size() const noexcept { return children.size(); }
			size_t count() const noexcept { return children.size(); }

			iter_t begin() noexcept { return children.begin(); }
			constIter_t begin() const noexcept { return children.begin(); }
			iter_t end() noexcept { return children.end(); }
			constIter_t end() const noexcept { return children.end(); }
		};

		struct array_t final
		{
		private:
			using holder_t = std::vector<std::unique_ptr<JSONAtom>>;
			holder_t children{};

		public:
			using iter_t = holder_t::iterator;
			using constIter_t = holder_t::const_iterator;

			array_t() = default;
			void clone(const array_t &array);
			JSONAtom &add(std::unique_ptr<JSONAtom> &&value);
			void del(const size_t key);
			void del(const JSONAtom &value);
			JSONAtom &operator [](const size_t key) const;
			size_t size() const noexcept { return children.size(); }
			size_t count() const noexcept { return children.size(); }
			const JSONAtom *last() const noexcept;

			iter_t begin() noexcept { return children.begin(); }
			constIter_t begin() const noexcept { return children.begin(); }
			iter_t end() noexcept { return children.end(); }
			constIter_t end() const noexcept { return children.end(); }
		};

		template<typename T> inline static void del(void *const object)
		{
			if (object)
				static_cast<T *>(object)->~T();
			operator delete(object);
		}

		template<typename T> struct makeOpaque_ { using uniqueType = OpaquePtr<T>; };
		template<typename T> struct makeOpaque_<T []> { using arrayType = OpaquePtr<T []>; };
		template<typename T, size_t N> struct makeOpaque_<T [N]> { struct invalidType { }; };

#if !defined(_MSC_VER) || _MSC_VER >= 1928
		template<typename T, typename... Args> inline typename makeOpaque_<T>::uniqueType makeOpaque(Args &&...args)
		{
			using consT = typename std::remove_const<T>::type;
			return OpaquePtr<T>(new consT(std::forward<Args>(args)...), del<T>);
		}
#endif

		template<typename T> inline typename makeOpaque_<T>::arrayType makeOpaque(const size_t num)
		{
			using consT = typename std::remove_const<typename std::remove_extent<T>::type>::type;
			return OpaquePtr<T>(new consT[num](), del<T []>);
		}

		template<typename T, typename... Args> inline typename makeOpaque_<T>::invalidType makeOpaque(Args &&...) noexcept = delete;
	}
}

using namespace rSON;
using namespace rSON::internal;

inline size_t strlen(const std::unique_ptr<char []> &str) noexcept { return strlen(str.get()); }

#endif /*INTERNAL_TYPES_HXX*/
