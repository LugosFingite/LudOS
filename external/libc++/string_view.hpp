// -*- C++ -*-
//===------------------------ string_view ---------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef _LIBCPP_STRING_VIEW
#define _LIBCPP_STRING_VIEW

#include <stdlib.h>

/*
string_view synopsis

namespace std {

    // 7.2, Class template basic_string_view
    template<class charT, class traits = char_traits<charT>>
        class basic_string_view;

    // 7.9, basic_string_view non-member comparison functions
    template<class charT, class traits>
    constexpr bool operator==(basic_string_view<charT, traits> x,
                              basic_string_view<charT, traits> y) noexcept;
    template<class charT, class traits>
    constexpr bool operator!=(basic_string_view<charT, traits> x,
                              basic_string_view<charT, traits> y) noexcept;
    template<class charT, class traits>
    constexpr bool operator< (basic_string_view<charT, traits> x,
                                 basic_string_view<charT, traits> y) noexcept;
    template<class charT, class traits>
    constexpr bool operator> (basic_string_view<charT, traits> x,
                              basic_string_view<charT, traits> y) noexcept;
    template<class charT, class traits>
    constexpr bool operator<=(basic_string_view<charT, traits> x,
                                 basic_string_view<charT, traits> y) noexcept;
    template<class charT, class traits>
    constexpr bool operator>=(basic_string_view<charT, traits> x,
                              basic_string_view<charT, traits> y) noexcept;
    // see below, sufficient additional overloads of comparison functions

    // 7.10, Inserters and extractors
    template<class charT, class traits>
      basic_ostream<charT, traits>&
        operator<<(basic_ostream<charT, traits>& os,
                   basic_string_view<charT, traits> str);

    // basic_string_view typedef names
    typedef basic_string_view<char> string_view;
    typedef basic_string_view<char16_t> u16string_view;
    typedef basic_string_view<char32_t> u32string_view;
    typedef basic_string_view<wchar_t> wstring_view;

    template<class charT, class traits = char_traits<charT>>
    class basic_string_view {
      public:
      // types
      typedef traits traits_type;
      typedef charT value_type;
      typedef charT* pointer;
      typedef const charT* const_pointer;
      typedef charT& reference;
      typedef const charT& const_reference;
      typedef implementation-defined const_iterator;
      typedef const_iterator iterator;
      typedef reverse_iterator<const_iterator> const_reverse_iterator;
      typedef const_reverse_iterator reverse_iterator;
      typedef size_t size_type;
      typedef ptrdiff_t difference_type;
      static constexpr size_type npos = size_type(-1);

      // 7.3, basic_string_view constructors and assignment operators
      constexpr basic_string_view() noexcept;
      constexpr basic_string_view(const basic_string_view&) noexcept = default;
      basic_string_view& operator=(const basic_string_view&) noexcept = default;
      template<class Allocator>
      constexpr basic_string_view(const charT* str);
      constexpr basic_string_view(const charT* str, size_type len);

      // 7.4, basic_string_view iterator support
      constexpr const_iterator begin() const noexcept;
      constexpr const_iterator end() const noexcept;
      constexpr const_iterator cbegin() const noexcept;
      constexpr const_iterator cend() const noexcept;
      const_reverse_iterator rbegin() const noexcept;
      const_reverse_iterator rend() const noexcept;
      const_reverse_iterator crbegin() const noexcept;
      const_reverse_iterator crend() const noexcept;

      // 7.5, basic_string_view capacity
      constexpr size_type size() const noexcept;
      constexpr size_type length() const noexcept;
      constexpr size_type max_size() const noexcept;
      constexpr bool empty() const noexcept;

      // 7.6, basic_string_view element access
      constexpr const_reference operator[](size_type pos) const;
      constexpr const_reference at(size_type pos) const;
      constexpr const_reference front() const;
      constexpr const_reference back() const;
      constexpr const_pointer data() const noexcept;

      // 7.7, basic_string_view modifiers
      constexpr void remove_prefix(size_type n);
      constexpr void remove_suffix(size_type n);
      constexpr void swap(basic_string_view& s) noexcept;

      size_type copy(charT* s, size_type n, size_type pos = 0) const;

      constexpr basic_string_view substr(size_type pos = 0, size_type n = npos) const;
      constexpr int compare(basic_string_view s) const noexcept;
      constexpr int compare(size_type pos1, size_type n1, basic_string_view s) const;
      constexpr int compare(size_type pos1, size_type n1,
                            basic_string_view s, size_type pos2, size_type n2) const;
      constexpr int compare(const charT* s) const;
      constexpr int compare(size_type pos1, size_type n1, const charT* s) const;
      constexpr int compare(size_type pos1, size_type n1,
                            const charT* s, size_type n2) const;
      constexpr size_type find(basic_string_view s, size_type pos = 0) const noexcept;
      constexpr size_type find(charT c, size_type pos = 0) const noexcept;
      constexpr size_type find(const charT* s, size_type pos, size_type n) const;
      constexpr size_type find(const charT* s, size_type pos = 0) const;
      constexpr size_type rfind(basic_string_view s, size_type pos = npos) const noexcept;
      constexpr size_type rfind(charT c, size_type pos = npos) const noexcept;
      constexpr size_type rfind(const charT* s, size_type pos, size_type n) const;
      constexpr size_type rfind(const charT* s, size_type pos = npos) const;
      constexpr size_type find_first_of(basic_string_view s, size_type pos = 0) const noexcept;
      constexpr size_type find_first_of(charT c, size_type pos = 0) const noexcept;
      constexpr size_type find_first_of(const charT* s, size_type pos, size_type n) const;
      constexpr size_type find_first_of(const charT* s, size_type pos = 0) const;
      constexpr size_type find_last_of(basic_string_view s, size_type pos = npos) const noexcept;
      constexpr size_type find_last_of(charT c, size_type pos = npos) const noexcept;
      constexpr size_type find_last_of(const charT* s, size_type pos, size_type n) const;
      constexpr size_type find_last_of(const charT* s, size_type pos = npos) const;
      constexpr size_type find_first_not_of(basic_string_view s, size_type pos = 0) const noexcept;
      constexpr size_type find_first_not_of(charT c, size_type pos = 0) const noexcept;
      constexpr size_type find_first_not_of(const charT* s, size_type pos, size_type n) const;
      constexpr size_type find_first_not_of(const charT* s, size_type pos = 0) const;
      constexpr size_type find_last_not_of(basic_string_view s, size_type pos = npos) const noexcept;
      constexpr size_type find_last_not_of(charT c, size_type pos = npos) const noexcept;
      constexpr size_type find_last_not_of(const charT* s, size_type pos, size_type n) const;
      constexpr size_type find_last_not_of(const charT* s, size_type pos = npos) const;

     private:
      const_pointer data_;  // exposition only
      size_type     size_;  // exposition only
    };

  // 7.11, Hash support
  template <class T> struct hash;
  template <> struct hash<string_view>;
  template <> struct hash<u16string_view>;
  template <> struct hash<u32string_view>;
  template <> struct hash<wstring_view>;

  constexpr basic_string<char>     operator "" s( const char *str,     size_t len ); // C++17
  constexpr basic_string<wchar_t>  operator "" s( const wchar_t *str,  size_t len ); // C++17
  constexpr basic_string<char16_t> operator "" s( const char16_t *str, size_t len ); // C++17
  constexpr basic_string<char32_t> operator "" s( const char32_t *str, size_t len ); // C++17

}  // namespace std


*/

#include "__config.hpp"
#include "__string.hpp"
#include "algorithm.hpp"
#include "iterator.hpp"
#include "limits.hpp"
#include "__debug.hpp"

#if !defined(_LIBCPP_HAS_NO_PRAGMA_SYSTEM_HEADER)
#pragma GCC system_header
#endif

_LIBCPP_PUSH_MACROS
#include "__undef_macros.hpp"


_LIBCPP_BEGIN_NAMESPACE_STD

template<class _CharT, class _Traits = char_traits<_CharT> >
        class _LIBCPP_TEMPLATE_VIS basic_string_view {
public:
    // types
    typedef _Traits                                    traits_type;
    typedef _CharT                                     value_type;
    typedef const _CharT*                              pointer;
    typedef const _CharT*                              const_pointer;
    typedef const _CharT&                              reference;
    typedef const _CharT&                              const_reference;
    typedef const_pointer                              const_iterator; // See [string.view.iterators]
    typedef const_iterator                             iterator;
    typedef _VSTD::reverse_iterator<const_iterator>    const_reverse_iterator;
    typedef const_reverse_iterator                     reverse_iterator;
    typedef size_t                                     size_type;
    typedef ptrdiff_t                                  difference_type;
    static _LIBCPP_CONSTEXPR const size_type npos = -1; // size_type(-1);

    static_assert(is_pod<value_type>::value, "Character type of basic_string_view must be a POD");
    static_assert((is_same<_CharT, typename traits_type::char_type>::value),
                  "traits_type::char_type must be the same type as CharT");

    // [string.view.cons], construct/copy
    _LIBCPP_CONSTEXPR _LIBCPP_INLINE_VISIBILITY
    basic_string_view() _NOEXCEPT : __data (nullptr), __size(0) {}

    _LIBCPP_CONSTEXPR _LIBCPP_INLINE_VISIBILITY
    basic_string_view(const basic_string_view&) _NOEXCEPT = default;

    _LIBCPP_CONSTEXPR_AFTER_CXX11 _LIBCPP_INLINE_VISIBILITY
    basic_string_view& operator=(const basic_string_view&) _NOEXCEPT = default;

    _LIBCPP_CONSTEXPR _LIBCPP_INLINE_VISIBILITY
    basic_string_view(const _CharT* __s, size_type __len)
        : __data(__s), __size(__len)
    {
        // #if _LIBCPP_STD_VER > 11
        //         _LIBCPP_ASSERT(__len == 0 || __s != nullptr, "string_view::string_view(_CharT *, size_t): received nullptr");
        // #endif
    }

    _LIBCPP_CONSTEXPR _LIBCPP_INLINE_VISIBILITY
    basic_string_view(const _CharT* __s)
        : __data(__s), __size(_Traits::length(__s)) {}

    // [string.view.iterators], iterators
    _LIBCPP_CONSTEXPR _LIBCPP_INLINE_VISIBILITY
    const_iterator begin()  const _NOEXCEPT { return cbegin(); }

    _LIBCPP_CONSTEXPR _LIBCPP_INLINE_VISIBILITY
    const_iterator end()    const _NOEXCEPT { return cend(); }

    _LIBCPP_CONSTEXPR _LIBCPP_INLINE_VISIBILITY
    const_iterator cbegin() const _NOEXCEPT { return __data; }

    _LIBCPP_CONSTEXPR _LIBCPP_INLINE_VISIBILITY
    const_iterator cend()   const _NOEXCEPT { return __data + __size; }

    _LIBCPP_CONSTEXPR_AFTER_CXX14 _LIBCPP_INLINE_VISIBILITY
    const_reverse_iterator rbegin()   const _NOEXCEPT { return const_reverse_iterator(cend()); }

    _LIBCPP_CONSTEXPR_AFTER_CXX14 _LIBCPP_INLINE_VISIBILITY
    const_reverse_iterator rend()     const _NOEXCEPT { return const_reverse_iterator(cbegin()); }

    _LIBCPP_CONSTEXPR_AFTER_CXX14 _LIBCPP_INLINE_VISIBILITY
    const_reverse_iterator crbegin()  const _NOEXCEPT { return const_reverse_iterator(cend()); }

    _LIBCPP_CONSTEXPR_AFTER_CXX14 _LIBCPP_INLINE_VISIBILITY
    const_reverse_iterator crend()    const _NOEXCEPT { return const_reverse_iterator(cbegin()); }

    // [string.view.capacity], capacity
    _LIBCPP_CONSTEXPR _LIBCPP_INLINE_VISIBILITY
    size_type size()     const _NOEXCEPT { return __size; }

    _LIBCPP_CONSTEXPR _LIBCPP_INLINE_VISIBILITY
    size_type length()   const _NOEXCEPT { return __size; }

    _LIBCPP_CONSTEXPR _LIBCPP_INLINE_VISIBILITY
    size_type max_size() const _NOEXCEPT { return numeric_limits<size_type>::max(); }

    _LIBCPP_CONSTEXPR bool _LIBCPP_INLINE_VISIBILITY
    empty()         const _NOEXCEPT { return __size == 0; }

    // [string.view.access], element access
    _LIBCPP_CONSTEXPR _LIBCPP_INLINE_VISIBILITY
    const_reference operator[](size_type __pos) const _NOEXCEPT { return __data[__pos]; }

    _LIBCPP_CONSTEXPR _LIBCPP_INLINE_VISIBILITY
    const_reference at(size_type __pos) const
    {
        assert(__pos < size());
        return __data[__pos];
    }

    _LIBCPP_CONSTEXPR _LIBCPP_INLINE_VISIBILITY
    const_reference front() const
    {
        return _LIBCPP_ASSERT(!empty(), "string_view::front(): string is empty"), __data[0];
    }

    _LIBCPP_CONSTEXPR _LIBCPP_INLINE_VISIBILITY
    const_reference back() const
    {
        return _LIBCPP_ASSERT(!empty(), "string_view::back(): string is empty"), __data[__size-1];
    }

    _LIBCPP_CONSTEXPR _LIBCPP_INLINE_VISIBILITY
    const_pointer data() const _NOEXCEPT { return __data; }

    // [string.view.modifiers], modifiers:
    _LIBCPP_CONSTEXPR_AFTER_CXX11 _LIBCPP_INLINE_VISIBILITY
    void remove_prefix(size_type __n) _NOEXCEPT
    {
        _LIBCPP_ASSERT(__n <= size(), "remove_prefix() can't remove more than size()");
        __data += __n;
        __size -= __n;
    }

    _LIBCPP_CONSTEXPR_AFTER_CXX11 _LIBCPP_INLINE_VISIBILITY
    void remove_suffix(size_type __n) _NOEXCEPT
    {
        _LIBCPP_ASSERT(__n <= size(), "remove_suffix() can't remove more than size()");
        __size -= __n;
    }

    _LIBCPP_CONSTEXPR_AFTER_CXX11 _LIBCPP_INLINE_VISIBILITY
    void swap(basic_string_view& __other) _NOEXCEPT
    {
        const value_type *__p = __data;
        __data = __other.__data;
        __other.__data = __p;

        size_type __sz = __size;
        __size = __other.__size;
        __other.__size = __sz;
    }

    _LIBCPP_INLINE_VISIBILITY
    size_type copy(_CharT* __s, size_type __n, size_type __pos = 0) const
    {
        if (__pos > size())
            assert(false && "string_view::copy");
        size_type __rlen = _VSTD::min(__n, size() - __pos);
        _Traits::copy(__s, data() + __pos, __rlen);
        return __rlen;
    }

    _LIBCPP_CONSTEXPR _LIBCPP_INLINE_VISIBILITY
    basic_string_view substr(size_type __pos = 0, size_type __n = npos) const
    {
        return __pos > size()
                ? (assert(false && "string_view::substr"), basic_string_view())
                : basic_string_view(data() + __pos, _VSTD::min(__n, size() - __pos));
    }

    _LIBCPP_CONSTEXPR_AFTER_CXX11 int compare(basic_string_view __sv) const _NOEXCEPT
    {
        size_type __rlen = _VSTD::min( size(), __sv.size());
        int __retval = _Traits::compare(data(), __sv.data(), __rlen);
        if ( __retval == 0 ) // first __rlen chars matched
            __retval = size() == __sv.size() ? 0 : ( size() < __sv.size() ? -1 : 1 );
        return __retval;
    }

    _LIBCPP_CONSTEXPR_AFTER_CXX11 _LIBCPP_INLINE_VISIBILITY
    int compare(size_type __pos1, size_type __n1, basic_string_view __sv) const
    {
        return substr(__pos1, __n1).compare(__sv);
    }

    _LIBCPP_CONSTEXPR_AFTER_CXX11 _LIBCPP_INLINE_VISIBILITY
    int compare(                       size_type __pos1, size_type __n1,
                                       basic_string_view __sv, size_type __pos2, size_type __n2) const
    {
        return substr(__pos1, __n1).compare(__sv.substr(__pos2, __n2));
    }

    _LIBCPP_CONSTEXPR_AFTER_CXX11 _LIBCPP_INLINE_VISIBILITY
    int compare(const _CharT* __s) const _NOEXCEPT
    {
        return compare(basic_string_view(__s));
    }

    _LIBCPP_CONSTEXPR_AFTER_CXX11 _LIBCPP_INLINE_VISIBILITY
    int compare(size_type __pos1, size_type __n1, const _CharT* __s) const
    {
        return substr(__pos1, __n1).compare(basic_string_view(__s));
    }

    _LIBCPP_CONSTEXPR_AFTER_CXX11 _LIBCPP_INLINE_VISIBILITY
    int compare(size_type __pos1, size_type __n1, const _CharT* __s, size_type __n2) const
    {
        return substr(__pos1, __n1).compare(basic_string_view(__s, __n2));
    }

    // find
    _LIBCPP_CONSTEXPR_AFTER_CXX11 _LIBCPP_INLINE_VISIBILITY
    size_type find(basic_string_view __s, size_type __pos = 0) const _NOEXCEPT
    {
        _LIBCPP_ASSERT(__s.size() == 0 || __s.data() != nullptr, "string_view::find(): received nullptr");
        return __str_find<value_type, size_type, traits_type, npos>
                (data(), size(), __s.data(), __pos, __s.size());
    }

    _LIBCPP_CONSTEXPR_AFTER_CXX11 _LIBCPP_INLINE_VISIBILITY
    size_type find(_CharT __c, size_type __pos = 0) const _NOEXCEPT
    {
        return __str_find<value_type, size_type, traits_type, npos>
                (data(), size(), __c, __pos);
    }

    _LIBCPP_CONSTEXPR_AFTER_CXX11 _LIBCPP_INLINE_VISIBILITY
    size_type find(const _CharT* __s, size_type __pos, size_type __n) const
    {
        _LIBCPP_ASSERT(__n == 0 || __s != nullptr, "string_view::find(): received nullptr");
        return __str_find<value_type, size_type, traits_type, npos>
                (data(), size(), __s, __pos, __n);
    }

    _LIBCPP_CONSTEXPR_AFTER_CXX11 _LIBCPP_INLINE_VISIBILITY
    size_type find(const _CharT* __s, size_type __pos = 0) const
    {
        _LIBCPP_ASSERT(__s != nullptr, "string_view::find(): received nullptr");
        return __str_find<value_type, size_type, traits_type, npos>
                (data(), size(), __s, __pos, traits_type::length(__s));
    }

    // rfind
    _LIBCPP_CONSTEXPR_AFTER_CXX11 _LIBCPP_INLINE_VISIBILITY
    size_type rfind(basic_string_view __s, size_type __pos = npos) const _NOEXCEPT
    {
        _LIBCPP_ASSERT(__s.size() == 0 || __s.data() != nullptr, "string_view::find(): received nullptr");
        return __str_rfind<value_type, size_type, traits_type, npos>
                (data(), size(), __s.data(), __pos, __s.size());
    }

    _LIBCPP_CONSTEXPR_AFTER_CXX11 _LIBCPP_INLINE_VISIBILITY
    size_type rfind(_CharT __c, size_type __pos = npos) const _NOEXCEPT
    {
        return __str_rfind<value_type, size_type, traits_type, npos>
                (data(), size(), __c, __pos);
    }

    _LIBCPP_CONSTEXPR_AFTER_CXX11 _LIBCPP_INLINE_VISIBILITY
    size_type rfind(const _CharT* __s, size_type __pos, size_type __n) const
    {
        _LIBCPP_ASSERT(__n == 0 || __s != nullptr, "string_view::rfind(): received nullptr");
        return __str_rfind<value_type, size_type, traits_type, npos>
                (data(), size(), __s, __pos, __n);
    }

    _LIBCPP_CONSTEXPR_AFTER_CXX11 _LIBCPP_INLINE_VISIBILITY
    size_type rfind(const _CharT* __s, size_type __pos=npos) const
    {
        _LIBCPP_ASSERT(__s != nullptr, "string_view::rfind(): received nullptr");
        return __str_rfind<value_type, size_type, traits_type, npos>
                (data(), size(), __s, __pos, traits_type::length(__s));
    }

    // find_first_of
    _LIBCPP_CONSTEXPR_AFTER_CXX11 _LIBCPP_INLINE_VISIBILITY
    size_type find_first_of(basic_string_view __s, size_type __pos = 0) const _NOEXCEPT
    {
        _LIBCPP_ASSERT(__s.size() == 0 || __s.data() != nullptr, "string_view::find_first_of(): received nullptr");
        return __str_find_first_of<value_type, size_type, traits_type, npos>
                (data(), size(), __s.data(), __pos, __s.size());
    }

    _LIBCPP_CONSTEXPR_AFTER_CXX11 _LIBCPP_INLINE_VISIBILITY
    size_type find_first_of(_CharT __c, size_type __pos = 0) const _NOEXCEPT
    { return find(__c, __pos); }

    _LIBCPP_CONSTEXPR_AFTER_CXX11 _LIBCPP_INLINE_VISIBILITY
    size_type find_first_of(const _CharT* __s, size_type __pos, size_type __n) const
    {
        _LIBCPP_ASSERT(__n == 0 || __s != nullptr, "string_view::find_first_of(): received nullptr");
        return __str_find_first_of<value_type, size_type, traits_type, npos>
                (data(), size(), __s, __pos, __n);
    }

    _LIBCPP_CONSTEXPR_AFTER_CXX11 _LIBCPP_INLINE_VISIBILITY
    size_type find_first_of(const _CharT* __s, size_type __pos=0) const
    {
        _LIBCPP_ASSERT(__s != nullptr, "string_view::find_first_of(): received nullptr");
        return __str_find_first_of<value_type, size_type, traits_type, npos>
                (data(), size(), __s, __pos, traits_type::length(__s));
    }

    // find_last_of
    _LIBCPP_CONSTEXPR_AFTER_CXX11 _LIBCPP_INLINE_VISIBILITY
    size_type find_last_of(basic_string_view __s, size_type __pos=npos) const _NOEXCEPT
    {
        _LIBCPP_ASSERT(__s.size() == 0 || __s.data() != nullptr, "string_view::find_last_of(): received nullptr");
        return __str_find_last_of<value_type, size_type, traits_type, npos>
                (data(), size(), __s.data(), __pos, __s.size());
    }

    _LIBCPP_CONSTEXPR_AFTER_CXX11 _LIBCPP_INLINE_VISIBILITY
    size_type find_last_of(_CharT __c, size_type __pos = npos) const _NOEXCEPT
    { return rfind(__c, __pos); }

    _LIBCPP_CONSTEXPR_AFTER_CXX11 _LIBCPP_INLINE_VISIBILITY
    size_type find_last_of(const _CharT* __s, size_type __pos, size_type __n) const
    {
        _LIBCPP_ASSERT(__n == 0 || __s != nullptr, "string_view::find_last_of(): received nullptr");
        return __str_find_last_of<value_type, size_type, traits_type, npos>
                (data(), size(), __s, __pos, __n);
    }

    _LIBCPP_CONSTEXPR_AFTER_CXX11 _LIBCPP_INLINE_VISIBILITY
    size_type find_last_of(const _CharT* __s, size_type __pos=npos) const
    {
        _LIBCPP_ASSERT(__s != nullptr, "string_view::find_last_of(): received nullptr");
        return __str_find_last_of<value_type, size_type, traits_type, npos>
                (data(), size(), __s, __pos, traits_type::length(__s));
    }

    // find_first_not_of
    _LIBCPP_CONSTEXPR_AFTER_CXX11 _LIBCPP_INLINE_VISIBILITY
    size_type find_first_not_of(basic_string_view __s, size_type __pos=0) const _NOEXCEPT
    {
        _LIBCPP_ASSERT(__s.size() == 0 || __s.data() != nullptr, "string_view::find_first_not_of(): received nullptr");
        return __str_find_first_not_of<value_type, size_type, traits_type, npos>
                (data(), size(), __s.data(), __pos, __s.size());
    }

    _LIBCPP_CONSTEXPR_AFTER_CXX11 _LIBCPP_INLINE_VISIBILITY
    size_type find_first_not_of(_CharT __c, size_type __pos=0) const _NOEXCEPT
    {
        return __str_find_first_not_of<value_type, size_type, traits_type, npos>
                (data(), size(), __c, __pos);
    }

    _LIBCPP_CONSTEXPR_AFTER_CXX11 _LIBCPP_INLINE_VISIBILITY
    size_type find_first_not_of(const _CharT* __s, size_type __pos, size_type __n) const
    {
        _LIBCPP_ASSERT(__n == 0 || __s != nullptr, "string_view::find_first_not_of(): received nullptr");
        return __str_find_first_not_of<value_type, size_type, traits_type, npos>
                (data(), size(), __s, __pos, __n);
    }

    _LIBCPP_CONSTEXPR_AFTER_CXX11 _LIBCPP_INLINE_VISIBILITY
    size_type find_first_not_of(const _CharT* __s, size_type __pos=0) const
    {
        _LIBCPP_ASSERT(__s != nullptr, "string_view::find_first_not_of(): received nullptr");
        return __str_find_first_not_of<value_type, size_type, traits_type, npos>
                (data(), size(), __s, __pos, traits_type::length(__s));
    }

    // find_last_not_of
    _LIBCPP_CONSTEXPR_AFTER_CXX11 _LIBCPP_INLINE_VISIBILITY
    size_type find_last_not_of(basic_string_view __s, size_type __pos=npos) const _NOEXCEPT
    {
        _LIBCPP_ASSERT(__s.size() == 0 || __s.data() != nullptr, "string_view::find_last_not_of(): received nullptr");
        return __str_find_last_not_of<value_type, size_type, traits_type, npos>
                (data(), size(), __s.data(), __pos, __s.size());
    }

    _LIBCPP_CONSTEXPR_AFTER_CXX11 _LIBCPP_INLINE_VISIBILITY
    size_type find_last_not_of(_CharT __c, size_type __pos=npos) const _NOEXCEPT
    {
        return __str_find_last_not_of<value_type, size_type, traits_type, npos>
                (data(), size(), __c, __pos);
    }

    _LIBCPP_CONSTEXPR_AFTER_CXX11 _LIBCPP_INLINE_VISIBILITY
    size_type find_last_not_of(const _CharT* __s, size_type __pos, size_type __n) const
    {
        _LIBCPP_ASSERT(__n == 0 || __s != nullptr, "string_view::find_last_not_of(): received nullptr");
        return __str_find_last_not_of<value_type, size_type, traits_type, npos>
                (data(), size(), __s, __pos, __n);
    }

    _LIBCPP_CONSTEXPR_AFTER_CXX11 _LIBCPP_INLINE_VISIBILITY
    size_type find_last_not_of(const _CharT* __s, size_type __pos=npos) const
    {
        _LIBCPP_ASSERT(__s != nullptr, "string_view::find_last_not_of(): received nullptr");
        return __str_find_last_not_of<value_type, size_type, traits_type, npos>
                (data(), size(), __s, __pos, traits_type::length(__s));
    }

private:
    const   value_type* __data;
    size_type           __size;
};


// [string.view.comparison]
// operator ==
template<class _CharT, class _Traits>
_LIBCPP_CONSTEXPR_AFTER_CXX11 _LIBCPP_INLINE_VISIBILITY
bool operator==(basic_string_view<_CharT, _Traits> __lhs,
                basic_string_view<_CharT, _Traits> __rhs) _NOEXCEPT
{
    if ( __lhs.size() != __rhs.size()) return false;
    return __lhs.compare(__rhs) == 0;
}

template<class _CharT, class _Traits>
_LIBCPP_CONSTEXPR_AFTER_CXX11 _LIBCPP_INLINE_VISIBILITY
bool operator==(basic_string_view<_CharT, _Traits> __lhs,
                typename common_type<basic_string_view<_CharT, _Traits> >::type __rhs) _NOEXCEPT
{
    if ( __lhs.size() != __rhs.size()) return false;
    return __lhs.compare(__rhs) == 0;
}

template<class _CharT, class _Traits>
_LIBCPP_CONSTEXPR_AFTER_CXX11 _LIBCPP_INLINE_VISIBILITY
bool operator==(typename common_type<basic_string_view<_CharT, _Traits> >::type __lhs,
                basic_string_view<_CharT, _Traits> __rhs) _NOEXCEPT
{
    if ( __lhs.size() != __rhs.size()) return false;
    return __lhs.compare(__rhs) == 0;
}


// operator !=
template<class _CharT, class _Traits>
_LIBCPP_CONSTEXPR_AFTER_CXX11 _LIBCPP_INLINE_VISIBILITY
bool operator!=(basic_string_view<_CharT, _Traits> __lhs, basic_string_view<_CharT, _Traits> __rhs) _NOEXCEPT
{
    if ( __lhs.size() != __rhs.size())
        return true;
    return __lhs.compare(__rhs) != 0;
}

template<class _CharT, class _Traits>
_LIBCPP_CONSTEXPR_AFTER_CXX11 _LIBCPP_INLINE_VISIBILITY
bool operator!=(basic_string_view<_CharT, _Traits> __lhs,
                typename common_type<basic_string_view<_CharT, _Traits> >::type __rhs) _NOEXCEPT
{
    if ( __lhs.size() != __rhs.size())
        return true;
    return __lhs.compare(__rhs) != 0;
}

template<class _CharT, class _Traits>
_LIBCPP_CONSTEXPR_AFTER_CXX11 _LIBCPP_INLINE_VISIBILITY
bool operator!=(typename common_type<basic_string_view<_CharT, _Traits> >::type __lhs,
                basic_string_view<_CharT, _Traits> __rhs) _NOEXCEPT
{
    if ( __lhs.size() != __rhs.size())
        return true;
    return __lhs.compare(__rhs) != 0;
}


// operator <
template<class _CharT, class _Traits>
_LIBCPP_CONSTEXPR_AFTER_CXX11 _LIBCPP_INLINE_VISIBILITY
bool operator<(basic_string_view<_CharT, _Traits> __lhs, basic_string_view<_CharT, _Traits> __rhs) _NOEXCEPT
{
    return __lhs.compare(__rhs) < 0;
}

template<class _CharT, class _Traits>
_LIBCPP_CONSTEXPR_AFTER_CXX11 _LIBCPP_INLINE_VISIBILITY
bool operator<(basic_string_view<_CharT, _Traits> __lhs,
               typename common_type<basic_string_view<_CharT, _Traits> >::type __rhs) _NOEXCEPT
{
    return __lhs.compare(__rhs) < 0;
}

template<class _CharT, class _Traits>
_LIBCPP_CONSTEXPR_AFTER_CXX11 _LIBCPP_INLINE_VISIBILITY
bool operator<(typename common_type<basic_string_view<_CharT, _Traits> >::type __lhs,
               basic_string_view<_CharT, _Traits> __rhs) _NOEXCEPT
{
    return __lhs.compare(__rhs) < 0;
}


// operator >
template<class _CharT, class _Traits>
_LIBCPP_CONSTEXPR_AFTER_CXX11 _LIBCPP_INLINE_VISIBILITY
bool operator> (basic_string_view<_CharT, _Traits> __lhs, basic_string_view<_CharT, _Traits> __rhs) _NOEXCEPT
{
    return __lhs.compare(__rhs) > 0;
}

template<class _CharT, class _Traits>
_LIBCPP_CONSTEXPR_AFTER_CXX11 _LIBCPP_INLINE_VISIBILITY
bool operator>(basic_string_view<_CharT, _Traits> __lhs,
               typename common_type<basic_string_view<_CharT, _Traits> >::type __rhs) _NOEXCEPT
{
    return __lhs.compare(__rhs) > 0;
}

template<class _CharT, class _Traits>
_LIBCPP_CONSTEXPR_AFTER_CXX11 _LIBCPP_INLINE_VISIBILITY
bool operator>(typename common_type<basic_string_view<_CharT, _Traits> >::type __lhs,
               basic_string_view<_CharT, _Traits> __rhs) _NOEXCEPT
{
    return __lhs.compare(__rhs) > 0;
}


// operator <=
template<class _CharT, class _Traits>
_LIBCPP_CONSTEXPR_AFTER_CXX11 _LIBCPP_INLINE_VISIBILITY
bool operator<=(basic_string_view<_CharT, _Traits> __lhs, basic_string_view<_CharT, _Traits> __rhs) _NOEXCEPT
{
    return __lhs.compare(__rhs) <= 0;
}

template<class _CharT, class _Traits>
_LIBCPP_CONSTEXPR_AFTER_CXX11 _LIBCPP_INLINE_VISIBILITY
bool operator<=(basic_string_view<_CharT, _Traits> __lhs,
                typename common_type<basic_string_view<_CharT, _Traits> >::type __rhs) _NOEXCEPT
{
    return __lhs.compare(__rhs) <= 0;
}

template<class _CharT, class _Traits>
_LIBCPP_CONSTEXPR_AFTER_CXX11 _LIBCPP_INLINE_VISIBILITY
bool operator<=(typename common_type<basic_string_view<_CharT, _Traits> >::type __lhs,
                basic_string_view<_CharT, _Traits> __rhs) _NOEXCEPT
{
    return __lhs.compare(__rhs) <= 0;
}


// operator >=
template<class _CharT, class _Traits>
_LIBCPP_CONSTEXPR_AFTER_CXX11 _LIBCPP_INLINE_VISIBILITY
bool operator>=(basic_string_view<_CharT, _Traits> __lhs, basic_string_view<_CharT, _Traits> __rhs) _NOEXCEPT
{
    return __lhs.compare(__rhs) >= 0;
}


template<class _CharT, class _Traits>
_LIBCPP_CONSTEXPR_AFTER_CXX11 _LIBCPP_INLINE_VISIBILITY
bool operator>=(basic_string_view<_CharT, _Traits> __lhs,
                typename common_type<basic_string_view<_CharT, _Traits> >::type __rhs) _NOEXCEPT
{
    return __lhs.compare(__rhs) >= 0;
}

template<class _CharT, class _Traits>
_LIBCPP_CONSTEXPR_AFTER_CXX11 _LIBCPP_INLINE_VISIBILITY
bool operator>=(typename common_type<basic_string_view<_CharT, _Traits> >::type __lhs,
                basic_string_view<_CharT, _Traits> __rhs) _NOEXCEPT
{
    return __lhs.compare(__rhs) >= 0;
}

typedef basic_string_view<char>     string_view;
typedef basic_string_view<char16_t> u16string_view;
typedef basic_string_view<char32_t> u32string_view;
typedef basic_string_view<wchar_t>  wstring_view;

// [string.view.hash]
template<class _CharT, class _Traits>
struct _LIBCPP_TEMPLATE_VIS hash<basic_string_view<_CharT, _Traits> >
        : public unary_function<basic_string_view<_CharT, _Traits>, size_t>
{
    size_t operator()(const basic_string_view<_CharT, _Traits> __val) const _NOEXCEPT;
};

template<class _CharT, class _Traits>
size_t
hash<basic_string_view<_CharT, _Traits> >::operator()(
        const basic_string_view<_CharT, _Traits> __val) const _NOEXCEPT
{
    return __do_string_hash(__val.data(), __val.data() + __val.size());
}


#if _LIBCPP_STD_VER > 11
inline namespace literals
{
    inline namespace string_view_literals
    {
        inline _LIBCPP_INLINE_VISIBILITY _LIBCPP_CONSTEXPR
                basic_string_view<char> operator "" sv(const char *__str, size_t __len)
        {
            return basic_string_view<char> (__str, __len);
        }

        inline _LIBCPP_INLINE_VISIBILITY _LIBCPP_CONSTEXPR
                basic_string_view<wchar_t> operator "" sv(const wchar_t *__str, size_t __len)
        {
            return basic_string_view<wchar_t> (__str, __len);
        }

        inline _LIBCPP_INLINE_VISIBILITY _LIBCPP_CONSTEXPR
                basic_string_view<char16_t> operator "" sv(const char16_t *__str, size_t __len)
        {
            return basic_string_view<char16_t> (__str, __len);
        }

        inline _LIBCPP_INLINE_VISIBILITY _LIBCPP_CONSTEXPR
                basic_string_view<char32_t> operator "" sv(const char32_t *__str, size_t __len)
        {
            return basic_string_view<char32_t> (__str, __len);
        }
    }
}
#endif
_LIBCPP_END_NAMESPACE_STD

_LIBCPP_POP_MACROS

#endif // _LIBCPP_STRING_VIEW
