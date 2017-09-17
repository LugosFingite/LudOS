// -*- C++ -*-
//===----------------------------------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is dual licensed under the MIT and the University of Illinois Open
// Source Licenses. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef _LIBCPP___BIT_REFERENCE
#define _LIBCPP___BIT_REFERENCE

#include "__config.hpp"
#include "algorithm.hpp"

#if !defined(_LIBCPP_HAS_NO_PRAGMA_SYSTEM_HEADER)
#pragma GCC system_header
#endif

_LIBCPP_PUSH_MACROS
#include "__undef_macros.hpp"


_LIBCPP_BEGIN_NAMESPACE_STD

template <class _Cp, bool _IsConst, typename _Cp::__storage_type = 0> class __bit_iterator;
template <class _Cp> class __bit_const_reference;

template <class _Tp>
struct __has_storage_type
{
    static const bool value = false;
};

template <class _Cp, bool = __has_storage_type<_Cp>::value>
class __bit_reference
{
    typedef typename _Cp::__storage_type    __storage_type;
    typedef typename _Cp::__storage_pointer __storage_pointer;

    __storage_pointer __seg_;
    __storage_type    __mask_;

    friend typename _Cp::__self;

    friend class __bit_const_reference<_Cp>;
    friend class __bit_iterator<_Cp, false>;
public:
    _LIBCPP_INLINE_VISIBILITY operator bool() const _NOEXCEPT
        {return static_cast<bool>(*__seg_ & __mask_);}
    _LIBCPP_INLINE_VISIBILITY bool operator ~() const _NOEXCEPT
        {return !static_cast<bool>(*this);}

    _LIBCPP_INLINE_VISIBILITY
    __bit_reference& operator=(bool __x) _NOEXCEPT
    {
        if (__x)
            *__seg_ |= __mask_;
        else
            *__seg_ &= ~__mask_;
        return *this;
    }

    _LIBCPP_INLINE_VISIBILITY
    __bit_reference& operator=(const __bit_reference& __x) _NOEXCEPT
        {return operator=(static_cast<bool>(__x));}

    _LIBCPP_INLINE_VISIBILITY void flip() _NOEXCEPT {*__seg_ ^= __mask_;}
    _LIBCPP_INLINE_VISIBILITY __bit_iterator<_Cp, false> operator&() const _NOEXCEPT
        {return __bit_iterator<_Cp, false>(__seg_, static_cast<unsigned>(__ctz(__mask_)));}
private:
    _LIBCPP_INLINE_VISIBILITY
    __bit_reference(__storage_pointer __s, __storage_type __m) _NOEXCEPT
        : __seg_(__s), __mask_(__m) {}
};

template <class _Cp>
class __bit_reference<_Cp, false>
{
};

template <class _Cp>
inline _LIBCPP_INLINE_VISIBILITY
void
swap(__bit_reference<_Cp> __x, __bit_reference<_Cp> __y) _NOEXCEPT
{
    bool __t = __x;
    __x = __y;
    __y = __t;
}

template <class _Cp, class _Dp>
inline _LIBCPP_INLINE_VISIBILITY
void
swap(__bit_reference<_Cp> __x, __bit_reference<_Dp> __y) _NOEXCEPT
{
    bool __t = __x;
    __x = __y;
    __y = __t;
}

template <class _Cp>
inline _LIBCPP_INLINE_VISIBILITY
void
swap(__bit_reference<_Cp> __x, bool& __y) _NOEXCEPT
{
    bool __t = __x;
    __x = __y;
    __y = __t;
}

template <class _Cp>
inline _LIBCPP_INLINE_VISIBILITY
void
swap(bool& __x, __bit_reference<_Cp> __y) _NOEXCEPT
{
    bool __t = __x;
    __x = __y;
    __y = __t;
}

template <class _Cp>
class __bit_const_reference
{
    typedef typename _Cp::__storage_type          __storage_type;
    typedef typename _Cp::__const_storage_pointer __storage_pointer;

    __storage_pointer        __seg_;
    __storage_type __mask_;

    friend typename _Cp::__self;
    friend class __bit_iterator<_Cp, true>;
public:
    _LIBCPP_INLINE_VISIBILITY
    __bit_const_reference(const __bit_reference<_Cp>& __x) _NOEXCEPT
        : __seg_(__x.__seg_), __mask_(__x.__mask_) {}

    _LIBCPP_INLINE_VISIBILITY _LIBCPP_CONSTEXPR operator bool() const _NOEXCEPT
        {return static_cast<bool>(*__seg_ & __mask_);}

    _LIBCPP_INLINE_VISIBILITY __bit_iterator<_Cp, true> operator&() const _NOEXCEPT
        {return __bit_iterator<_Cp, true>(__seg_, static_cast<unsigned>(__ctz(__mask_)));}
private:
    _LIBCPP_INLINE_VISIBILITY
    _LIBCPP_CONSTEXPR
    __bit_const_reference(__storage_pointer __s, __storage_type __m) _NOEXCEPT
        : __seg_(__s), __mask_(__m) {}

    __bit_const_reference& operator=(const __bit_const_reference& __x);
};

// find

template <class _Cp, bool _IsConst>
__bit_iterator<_Cp, _IsConst>
__find_bool_true(__bit_iterator<_Cp, _IsConst> __first, typename _Cp::size_type __n)
{
    typedef __bit_iterator<_Cp, _IsConst> _It;
    typedef typename _It::__storage_type __storage_type;
    static const int __bits_per_word = _It::__bits_per_word;
    // do first partial word
    if (__first.__ctz_ != 0)
    {
        __storage_type __clz_f = static_cast<__storage_type>(__bits_per_word - __first.__ctz_);
        __storage_type __dn = _VSTD::min(__clz_f, __n);
        __storage_type __m = (~__storage_type(0) << __first.__ctz_) & (~__storage_type(0) >> (__clz_f - __dn));
        __storage_type __b = *__first.__seg_ & __m;
        if (__b)
            return _It(__first.__seg_, static_cast<unsigned>(_VSTD::__ctz(__b)));
        if (__n == __dn)
            return __first + __n;
        __n -= __dn;
        ++__first.__seg_;
    }
    // do middle whole words
    for (; __n >= __bits_per_word; ++__first.__seg_, __n -= __bits_per_word)
        if (*__first.__seg_)
            return _It(__first.__seg_, static_cast<unsigned>(_VSTD::__ctz(*__first.__seg_)));
    // do last partial word
    if (__n > 0)
    {
        __storage_type __m = ~__storage_type(0) >> (__bits_per_word - __n);
        __storage_type __b = *__first.__seg_ & __m;
        if (__b)
            return _It(__first.__seg_, static_cast<unsigned>(_VSTD::__ctz(__b)));
    }
    return _It(__first.__seg_, static_cast<unsigned>(__n));
}

template <class _Cp, bool _IsConst>
__bit_iterator<_Cp, _IsConst>
__find_bool_false(__bit_iterator<_Cp, _IsConst> __first, typename _Cp::size_type __n)
{
    typedef __bit_iterator<_Cp, _IsConst> _It;
    typedef typename _It::__storage_type __storage_type;
    const int __bits_per_word = _It::__bits_per_word;
    // do first partial word
    if (__first.__ctz_ != 0)
    {
        __storage_type __clz_f = static_cast<__storage_type>(__bits_per_word - __first.__ctz_);
        __storage_type __dn = _VSTD::min(__clz_f, __n);
        __storage_type __m = (~__storage_type(0) << __first.__ctz_) & (~__storage_type(0) >> (__clz_f - __dn));
        __storage_type __b = ~*__first.__seg_ & __m;
        if (__b)
            return _It(__first.__seg_, static_cast<unsigned>(_VSTD::__ctz(__b)));
        if (__n == __dn)
            return __first + __n;
        __n -= __dn;
        ++__first.__seg_;
    }
    // do middle whole words
    for (; __n >= __bits_per_word; ++__first.__seg_, __n -= __bits_per_word)
    {
        __storage_type __b = ~*__first.__seg_;
        if (__b)
            return _It(__first.__seg_, static_cast<unsigned>(_VSTD::__ctz(__b)));
    }
    // do last partial word
    if (__n > 0)
    {
        __storage_type __m = ~__storage_type(0) >> (__bits_per_word - __n);
        __storage_type __b = ~*__first.__seg_ & __m;
        if (__b)
            return _It(__first.__seg_, static_cast<unsigned>(_VSTD::__ctz(__b)));
    }
    return _It(__first.__seg_, static_cast<unsigned>(__n));
}

template <class _Cp, bool _IsConst, class _Tp>
inline _LIBCPP_INLINE_VISIBILITY
__bit_iterator<_Cp, _IsConst>
find(__bit_iterator<_Cp, _IsConst> __first, __bit_iterator<_Cp, _IsConst> __last, const _Tp& __value_)
{
    if (static_cast<bool>(__value_))
        return __find_bool_true(__first, static_cast<typename _Cp::size_type>(__last - __first));
    return __find_bool_false(__first, static_cast<typename _Cp::size_type>(__last - __first));
}

// count

template <class _Cp, bool _IsConst>
typename __bit_iterator<_Cp, _IsConst>::difference_type
__count_bool_true(__bit_iterator<_Cp, _IsConst> __first, typename _Cp::size_type __n)
{
    typedef __bit_iterator<_Cp, _IsConst> _It;
    typedef typename _It::__storage_type __storage_type;
    typedef typename _It::difference_type difference_type;
    const int __bits_per_word = _It::__bits_per_word;
    difference_type __r = 0;
    // do first partial word
    if (__first.__ctz_ != 0)
    {
        __storage_type __clz_f = static_cast<__storage_type>(__bits_per_word - __first.__ctz_);
        __storage_type __dn = _VSTD::min(__clz_f, __n);
        __storage_type __m = (~__storage_type(0) << __first.__ctz_) & (~__storage_type(0) >> (__clz_f - __dn));
        __r = _VSTD::__pop_count(*__first.__seg_ & __m);
        __n -= __dn;
        ++__first.__seg_;
    }
    // do middle whole words
    for (; __n >= __bits_per_word; ++__first.__seg_, __n -= __bits_per_word)
        __r += _VSTD::__pop_count(*__first.__seg_);
    // do last partial word
    if (__n > 0)
    {
        __storage_type __m = ~__storage_type(0) >> (__bits_per_word - __n);
        __r += _VSTD::__pop_count(*__first.__seg_ & __m);
    }
    return __r;
}

template <class _Cp, bool _IsConst>
typename __bit_iterator<_Cp, _IsConst>::difference_type
__count_bool_false(__bit_iterator<_Cp, _IsConst> __first, typename _Cp::size_type __n)
{
    typedef __bit_iterator<_Cp, _IsConst> _It;
    typedef typename _It::__storage_type __storage_type;
    typedef typename _It::difference_type difference_type;
    const int __bits_per_word = _It::__bits_per_word;
    difference_type __r = 0;
    // do first partial word
    if (__first.__ctz_ != 0)
    {
        __storage_type __clz_f = static_cast<__storage_type>(__bits_per_word - __first.__ctz_);
        __storage_type __dn = _VSTD::min(__clz_f, __n);
        __storage_type __m = (~__storage_type(0) << __first.__ctz_) & (~__storage_type(0) >> (__clz_f - __dn));
        __r = _VSTD::__pop_count(~*__first.__seg_ & __m);
        __n -= __dn;
        ++__first.__seg_;
    }
    // do middle whole words
    for (; __n >= __bits_per_word; ++__first.__seg_, __n -= __bits_per_word)
        __r += _VSTD::__pop_count(~*__first.__seg_);
    // do last partial word
    if (__n > 0)
    {
        __storage_type __m = ~__storage_type(0) >> (__bits_per_word - __n);
        __r += _VSTD::__pop_count(~*__first.__seg_ & __m);
    }
    return __r;
}

template <class _Cp, bool _IsConst, class _Tp>
inline _LIBCPP_INLINE_VISIBILITY
typename __bit_iterator<_Cp, _IsConst>::difference_type
count(__bit_iterator<_Cp, _IsConst> __first, __bit_iterator<_Cp, _IsConst> __last, const _Tp& __value_)
{
    if (static_cast<bool>(__value_))
        return __count_bool_true(__first, static_cast<typename _Cp::size_type>(__last - __first));
    return __count_bool_false(__first, static_cast<typename _Cp::size_type>(__last - __first));
}

// fill_n

template <class _Cp>
void
__fill_n_false(__bit_iterator<_Cp, false> __first, typename _Cp::size_type __n)
{
    typedef __bit_iterator<_Cp, false> _It;
    typedef typename _It::__storage_type __storage_type;
    const int __bits_per_word = _It::__bits_per_word;
    // do first partial word
    if (__first.__ctz_ != 0)
    {
        __storage_type __clz_f = static_cast<__storage_type>(__bits_per_word - __first.__ctz_);
        __storage_type __dn = _VSTD::min(__clz_f, __n);
        __storage_type __m = (~__storage_type(0) << __first.__ctz_) & (~__storage_type(0) >> (__clz_f - __dn));
        *__first.__seg_ &= ~__m;
        __n -= __dn;
        ++__first.__seg_;
    }
    // do middle whole words
    __storage_type __nw = __n / __bits_per_word;
    memset(_VSTD::__to_raw_pointer(__first.__seg_), 0, __nw * sizeof(__storage_type));
    __n -= __nw * __bits_per_word;
    // do last partial word
    if (__n > 0)
    {
        __first.__seg_ += __nw;
        __storage_type __m = ~__storage_type(0) >> (__bits_per_word - __n);
        *__first.__seg_ &= ~__m;
    }
}

template <class _Cp>
void
__fill_n_true(__bit_iterator<_Cp, false> __first, typename _Cp::size_type __n)
{
    typedef __bit_iterator<_Cp, false> _It;
    typedef typename _It::__storage_type __storage_type;
    const int __bits_per_word = _It::__bits_per_word;
    // do first partial word
    if (__first.__ctz_ != 0)
    {
        __storage_type __clz_f = static_cast<__storage_type>(__bits_per_word - __first.__ctz_);
        __storage_type __dn = _VSTD::min(__clz_f, __n);
        __storage_type __m = (~__storage_type(0) << __first.__ctz_) & (~__storage_type(0) >> (__clz_f - __dn));
        *__first.__seg_ |= __m;
        __n -= __dn;
        ++__first.__seg_;
    }
    // do middle whole words
    __storage_type __nw = __n / __bits_per_word;
    memset(_VSTD::__to_raw_pointer(__first.__seg_), -1, __nw * sizeof(__storage_type));
    __n -= __nw * __bits_per_word;
    // do last partial word
    if (__n > 0)
    {
        __first.__seg_ += __nw;
        __storage_type __m = ~__storage_type(0) >> (__bits_per_word - __n);
        *__first.__seg_ |= __m;
    }
}

template <class _Cp>
inline _LIBCPP_INLINE_VISIBILITY
void
fill_n(__bit_iterator<_Cp, false> __first, typename _Cp::size_type __n, bool __value_)
{
    if (__n > 0)
    {
        if (__value_)
            __fill_n_true(__first, __n);
        else
            __fill_n_false(__first, __n);
    }
}

// fill

template <class _Cp>
inline _LIBCPP_INLINE_VISIBILITY
void
fill(__bit_iterator<_Cp, false> __first, __bit_iterator<_Cp, false> __last, bool __value_)
{
    _VSTD::fill_n(__first, static_cast<typename _Cp::size_type>(__last - __first), __value_);
}

// copy

template <class _Cp, bool _IsConst>
__bit_iterator<_Cp, false>
__copy_aligned(__bit_iterator<_Cp, _IsConst> __first, __bit_iterator<_Cp, _IsConst> __last,
                                                     __bit_iterator<_Cp, false> __result)
{
    typedef __bit_iterator<_Cp, _IsConst> _In;
    typedef  typename _In::difference_type difference_type;
    typedef typename _In::__storage_type __storage_type;
    const int __bits_per_word = _In::__bits_per_word;
    difference_type __n = __last - __first;
    if (__n > 0)
    {
        // do first word
        if (__first.__ctz_ != 0)
        {
            unsigned __clz = __bits_per_word - __first.__ctz_;
            difference_type __dn = _VSTD::min(static_cast<difference_type>(__clz), __n);
            __n -= __dn;
            __storage_type __m = (~__storage_type(0) << __first.__ctz_) & (~__storage_type(0) >> (__clz - __dn));
            __storage_type __b = *__first.__seg_ & __m;
            *__result.__seg_ &= ~__m;
            *__result.__seg_ |= __b;
            __result.__seg_ += (__dn + __result.__ctz_) / __bits_per_word;
            __result.__ctz_ = static_cast<unsigned>((__dn + __result.__ctz_)  % __bits_per_word);
            ++__first.__seg_;
            // __first.__ctz_ = 0;
        }
        // __first.__ctz_ == 0;
        // do middle words
        __storage_type __nw = __n / __bits_per_word;
        memmove(_VSTD::__to_raw_pointer(__result.__seg_),
                       _VSTD::__to_raw_pointer(__first.__seg_),
                       __nw * sizeof(__storage_type));
        __n -= __nw * __bits_per_word;
        __result.__seg_ += __nw;
        // do last word
        if (__n > 0)
        {
            __first.__seg_ += __nw;
            __storage_type __m = ~__storage_type(0) >> (__bits_per_word - __n);
            __storage_type __b = *__first.__seg_ & __m;
            *__result.__seg_ &= ~__m;
            *__result.__seg_ |= __b;
            __result.__ctz_ = static_cast<unsigned>(__n);
        }
    }
    return __result;
}

template <class _Cp, bool _IsConst>
__bit_iterator<_Cp, false>
__copy_unaligned(__bit_iterator<_Cp, _IsConst> __first, __bit_iterator<_Cp, _IsConst> __last,
                                                       __bit_iterator<_Cp, false> __result)
{
    typedef __bit_iterator<_Cp, _IsConst> _In;
    typedef  typename _In::difference_type difference_type;
    typedef typename _In::__storage_type __storage_type;
    static const int __bits_per_word = _In::__bits_per_word;
    difference_type __n = __last - __first;
    if (__n > 0)
    {
        // do first word
        if (__first.__ctz_ != 0)
        {
            unsigned __clz_f = __bits_per_word - __first.__ctz_;
            difference_type __dn = _VSTD::min(static_cast<difference_type>(__clz_f), __n);
            __n -= __dn;
            __storage_type __m = (~__storage_type(0) << __first.__ctz_) & (~__storage_type(0) >> (__clz_f - __dn));
            __storage_type __b = *__first.__seg_ & __m;
            unsigned __clz_r = __bits_per_word - __result.__ctz_;
            __storage_type __ddn = _VSTD::min<__storage_type>(__dn, __clz_r);
            __m = (~__storage_type(0) << __result.__ctz_) & (~__storage_type(0) >> (__clz_r - __ddn));
            *__result.__seg_ &= ~__m;
            if (__result.__ctz_ > __first.__ctz_)
                *__result.__seg_ |= __b << (__result.__ctz_ - __first.__ctz_);
            else
                *__result.__seg_ |= __b >> (__first.__ctz_ - __result.__ctz_);
            __result.__seg_ += (__ddn + __result.__ctz_) / __bits_per_word;
            __result.__ctz_ = static_cast<unsigned>((__ddn + __result.__ctz_)  % __bits_per_word);
            __dn -= __ddn;
            if (__dn > 0)
            {
                __m = ~__storage_type(0) >> (__bits_per_word - __dn);
                *__result.__seg_ &= ~__m;
                *__result.__seg_ |= __b >> (__first.__ctz_ + __ddn);
                __result.__ctz_ = static_cast<unsigned>(__dn);
            }
            ++__first.__seg_;
            // __first.__ctz_ = 0;
        }
        // __first.__ctz_ == 0;
        // do middle words
        unsigned __clz_r = __bits_per_word - __result.__ctz_;
        __storage_type __m = ~__storage_type(0) << __result.__ctz_;
        for (; __n >= __bits_per_word; __n -= __bits_per_word, ++__first.__seg_)
        {
            __storage_type __b = *__first.__seg_;
            *__result.__seg_ &= ~__m;
            *__result.__seg_ |= __b << __result.__ctz_;
            ++__result.__seg_;
            *__result.__seg_ &= __m;
            *__result.__seg_ |= __b >> __clz_r;
        }
        // do last word
        if (__n > 0)
        {
            __m = ~__storage_type(0) >> (__bits_per_word - __n);
            __storage_type __b = *__first.__seg_ & __m;
            __storage_type __dn = _VSTD::min(__n, static_cast<difference_type>(__clz_r));
            __m = (~__storage_type(0) << __result.__ctz_) & (~__storage_type(0) >> (__clz_r - __dn));
            *__result.__seg_ &= ~__m;
            *__result.__seg_ |= __b << __result.__ctz_;
            __result.__seg_ += (__dn + __result.__ctz_) / __bits_per_word;
            __result.__ctz_ = static_cast<unsigned>((__dn + __result.__ctz_)  % __bits_per_word);
            __n -= __dn;
            if (__n > 0)
            {
                __m = ~__storage_type(0) >> (__bits_per_word - __n);
                *__result.__seg_ &= ~__m;
                *__result.__seg_ |= __b >> __dn;
                __result.__ctz_ = static_cast<unsigned>(__n);
            }
        }
    }
    return __result;
}

template <class _Cp, bool _IsConst>
inline _LIBCPP_INLINE_VISIBILITY
__bit_iterator<_Cp, false>
copy(__bit_iterator<_Cp, _IsConst> __first, __bit_iterator<_Cp, _IsConst> __last, __bit_iterator<_Cp, false> __result)
{
    if (__first.__ctz_ == __result.__ctz_)
        return __copy_aligned(__first, __last, __result);
    return __copy_unaligned(__first, __last, __result);
}

// copy_backward

template <class _Cp, bool _IsConst>
__bit_iterator<_Cp, false>
__copy_backward_aligned(__bit_iterator<_Cp, _IsConst> __first, __bit_iterator<_Cp, _IsConst> __last,
                                                     __bit_iterator<_Cp, false> __result)
{
    typedef __bit_iterator<_Cp, _IsConst> _In;
    typedef  typename _In::difference_type difference_type;
    typedef typename _In::__storage_type __storage_type;
    const int __bits_per_word = _In::__bits_per_word;
    difference_type __n = __last - __first;
    if (__n > 0)
    {
        // do first word
        if (__last.__ctz_ != 0)
        {
            difference_type __dn = _VSTD::min(static_cast<difference_type>(__last.__ctz_), __n);
            __n -= __dn;
            unsigned __clz = __bits_per_word - __last.__ctz_;
            __storage_type __m = (~__storage_type(0) << (__last.__ctz_ - __dn)) & (~__storage_type(0) >> __clz);
            __storage_type __b = *__last.__seg_ & __m;
            *__result.__seg_ &= ~__m;
            *__result.__seg_ |= __b;
            __result.__ctz_ = static_cast<unsigned>(((-__dn & (__bits_per_word - 1)) +
                                                       __result.__ctz_)  % __bits_per_word);
            // __last.__ctz_ = 0
         }
        // __last.__ctz_ == 0 || __n == 0
        // __result.__ctz_ == 0 || __n == 0
        // do middle words
        __storage_type __nw = __n / __bits_per_word;
        __result.__seg_ -= __nw;
        __last.__seg_ -= __nw;
        memmove(_VSTD::__to_raw_pointer(__result.__seg_),
                       _VSTD::__to_raw_pointer(__last.__seg_),
                       __nw * sizeof(__storage_type));
        __n -= __nw * __bits_per_word;
        // do last word
        if (__n > 0)
        {
            __storage_type __m = ~__storage_type(0) << (__bits_per_word - __n);
            __storage_type __b = *--__last.__seg_ & __m;
            *--__result.__seg_ &= ~__m;
            *__result.__seg_ |= __b;
            __result.__ctz_ = static_cast<unsigned>(-__n & (__bits_per_word - 1));
        }
    }
    return __result;
}

template <class _Cp, bool _IsConst>
__bit_iterator<_Cp, false>
__copy_backward_unaligned(__bit_iterator<_Cp, _IsConst> __first, __bit_iterator<_Cp, _IsConst> __last,
                                                       __bit_iterator<_Cp, false> __result)
{
    typedef __bit_iterator<_Cp, _IsConst> _In;
    typedef  typename _In::difference_type difference_type;
    typedef typename _In::__storage_type __storage_type;
    const int __bits_per_word = _In::__bits_per_word;
    difference_type __n = __last - __first;
    if (__n > 0)
    {
        // do first word
        if (__last.__ctz_ != 0)
        {
            difference_type __dn = _VSTD::min(static_cast<difference_type>(__last.__ctz_), __n);
            __n -= __dn;
            unsigned __clz_l = __bits_per_word - __last.__ctz_;
            __storage_type __m = (~__storage_type(0) << (__last.__ctz_ - __dn)) & (~__storage_type(0) >> __clz_l);
            __storage_type __b = *__last.__seg_ & __m;
            unsigned __clz_r = __bits_per_word - __result.__ctz_;
            __storage_type __ddn = _VSTD::min(__dn, static_cast<difference_type>(__result.__ctz_));
            if (__ddn > 0)
            {
                __m = (~__storage_type(0) << (__result.__ctz_ - __ddn)) & (~__storage_type(0) >> __clz_r);
                *__result.__seg_ &= ~__m;
                if (__result.__ctz_ > __last.__ctz_)
                    *__result.__seg_ |= __b << (__result.__ctz_ - __last.__ctz_);
                else
                    *__result.__seg_ |= __b >> (__last.__ctz_ - __result.__ctz_);
                __result.__ctz_ = static_cast<unsigned>(((-__ddn & (__bits_per_word - 1)) +
                                                         __result.__ctz_)  % __bits_per_word);
                __dn -= __ddn;
            }
            if (__dn > 0)
            {
                // __result.__ctz_ == 0
                --__result.__seg_;
                __result.__ctz_ = static_cast<unsigned>(-__dn & (__bits_per_word - 1));
                __m = ~__storage_type(0) << __result.__ctz_;
                *__result.__seg_ &= ~__m;
                __last.__ctz_ -= __dn + __ddn;
                *__result.__seg_ |= __b << (__result.__ctz_ - __last.__ctz_);
            }
            // __last.__ctz_ = 0
         }
        // __last.__ctz_ == 0 || __n == 0
        // __result.__ctz_ != 0 || __n == 0
        // do middle words
        unsigned __clz_r = __bits_per_word - __result.__ctz_;
        __storage_type __m = ~__storage_type(0) >> __clz_r;
        for (; __n >= __bits_per_word; __n -= __bits_per_word)
        {
            __storage_type __b = *--__last.__seg_;
            *__result.__seg_ &= ~__m;
            *__result.__seg_ |= __b >> __clz_r;
            *--__result.__seg_ &= __m;
            *__result.__seg_ |= __b << __result.__ctz_;
        }
        // do last word
        if (__n > 0)
        {
            __m = ~__storage_type(0) << (__bits_per_word - __n);
            __storage_type __b = *--__last.__seg_ & __m;
            __clz_r = __bits_per_word - __result.__ctz_;
            __storage_type __dn = _VSTD::min(__n, static_cast<difference_type>(__result.__ctz_));
            __m = (~__storage_type(0) << (__result.__ctz_ - __dn)) & (~__storage_type(0) >> __clz_r);
            *__result.__seg_ &= ~__m;
            *__result.__seg_ |= __b >> (__bits_per_word - __result.__ctz_);
            __result.__ctz_ = static_cast<unsigned>(((-__dn & (__bits_per_word - 1)) +
                                                     __result.__ctz_)  % __bits_per_word);
            __n -= __dn;
            if (__n > 0)
            {
                // __result.__ctz_ == 0
                --__result.__seg_;
                __result.__ctz_ = static_cast<unsigned>(-__n & (__bits_per_word - 1));
                __m = ~__storage_type(0) << __result.__ctz_;
                *__result.__seg_ &= ~__m;
                *__result.__seg_ |= __b << (__result.__ctz_ - (__bits_per_word - __n - __dn));
            }
        }
    }
    return __result;
}

template <class _Cp, bool _IsConst>
inline _LIBCPP_INLINE_VISIBILITY
__bit_iterator<_Cp, false>
copy_backward(__bit_iterator<_Cp, _IsConst> __first, __bit_iterator<_Cp, _IsConst> __last, __bit_iterator<_Cp, false> __result)
{
    if (__last.__ctz_ == __result.__ctz_)
        return __copy_backward_aligned(__first, __last, __result);
    return __copy_backward_unaligned(__first, __last, __result);
}

// move

template <class _Cp, bool _IsConst>
inline _LIBCPP_INLINE_VISIBILITY
__bit_iterator<_Cp, false>
move(__bit_iterator<_Cp, _IsConst> __first, __bit_iterator<_Cp, _IsConst> __last, __bit_iterator<_Cp, false> __result)
{
    return _VSTD::copy(__first, __last, __result);
}

// move_backward

template <class _Cp, bool _IsConst>
inline _LIBCPP_INLINE_VISIBILITY
__bit_iterator<_Cp, false>
move_backward(__bit_iterator<_Cp, _IsConst> __first, __bit_iterator<_Cp, _IsConst> __last, __bit_iterator<_Cp, false> __result)
{
    return _VSTD::copy_backward(__first, __last, __result);
}

// swap_ranges

template <class __C1, class __C2>
__bit_iterator<__C2, false>
__swap_ranges_aligned(__bit_iterator<__C1, false> __first, __bit_iterator<__C1, false> __last,
                      __bit_iterator<__C2, false> __result)
{
    typedef __bit_iterator<__C1, false> _I1;
    typedef  typename _I1::difference_type difference_type;
    typedef typename _I1::__storage_type __storage_type;
    const int __bits_per_word = _I1::__bits_per_word;
    difference_type __n = __last - __first;
    if (__n > 0)
    {
        // do first word
        if (__first.__ctz_ != 0)
        {
            unsigned __clz = __bits_per_word - __first.__ctz_;
            difference_type __dn = _VSTD::min(static_cast<difference_type>(__clz), __n);
            __n -= __dn;
            __storage_type __m = (~__storage_type(0) << __first.__ctz_) & (~__storage_type(0) >> (__clz - __dn));
            __storage_type __b1 = *__first.__seg_ & __m;
            *__first.__seg_ &= ~__m;
            __storage_type __b2 = *__result.__seg_ & __m;
            *__result.__seg_ &= ~__m;
            *__result.__seg_ |= __b1;
            *__first.__seg_  |= __b2;
            __result.__seg_ += (__dn + __result.__ctz_) / __bits_per_word;
            __result.__ctz_ = static_cast<unsigned>((__dn + __result.__ctz_)  % __bits_per_word);
            ++__first.__seg_;
            // __first.__ctz_ = 0;
        }
        // __first.__ctz_ == 0;
        // do middle words
        for (; __n >= __bits_per_word; __n -= __bits_per_word, ++__first.__seg_, ++__result.__seg_)
            swap(*__first.__seg_, *__result.__seg_);
        // do last word
        if (__n > 0)
        {
            __storage_type __m = ~__storage_type(0) >> (__bits_per_word - __n);
            __storage_type __b1 = *__first.__seg_ & __m;
            *__first.__seg_ &= ~__m;
            __storage_type __b2 = *__result.__seg_ & __m;
            *__result.__seg_ &= ~__m;
            *__result.__seg_ |= __b1;
            *__first.__seg_  |= __b2;
            __result.__ctz_ = static_cast<unsigned>(__n);
        }
    }
    return __result;
}

template <class __C1, class __C2>
__bit_iterator<__C2, false>
__swap_ranges_unaligned(__bit_iterator<__C1, false> __first, __bit_iterator<__C1, false> __last,
                        __bit_iterator<__C2, false> __result)
{
    typedef __bit_iterator<__C1, false> _I1;
    typedef  typename _I1::difference_type difference_type;
    typedef typename _I1::__storage_type __storage_type;
    const int __bits_per_word = _I1::__bits_per_word;
    difference_type __n = __last - __first;
    if (__n > 0)
    {
        // do first word
        if (__first.__ctz_ != 0)
        {
            unsigned __clz_f = __bits_per_word - __first.__ctz_;
            difference_type __dn = _VSTD::min(static_cast<difference_type>(__clz_f), __n);
            __n -= __dn;
            __storage_type __m = (~__storage_type(0) << __first.__ctz_) & (~__storage_type(0) >> (__clz_f - __dn));
            __storage_type __b1 = *__first.__seg_ & __m;
            *__first.__seg_ &= ~__m;
            unsigned __clz_r = __bits_per_word - __result.__ctz_;
            __storage_type __ddn = _VSTD::min<__storage_type>(__dn, __clz_r);
            __m = (~__storage_type(0) << __result.__ctz_) & (~__storage_type(0) >> (__clz_r - __ddn));
            __storage_type __b2 = *__result.__seg_ & __m;
            *__result.__seg_ &= ~__m;
            if (__result.__ctz_ > __first.__ctz_)
            {
                unsigned __s = __result.__ctz_ - __first.__ctz_;
                *__result.__seg_ |= __b1 << __s;
                *__first.__seg_  |= __b2 >> __s;
            }
            else
            {
                unsigned __s = __first.__ctz_ - __result.__ctz_;
                *__result.__seg_ |= __b1 >> __s;
                *__first.__seg_  |= __b2 << __s;
            }
            __result.__seg_ += (__ddn + __result.__ctz_) / __bits_per_word;
            __result.__ctz_ = static_cast<unsigned>((__ddn + __result.__ctz_)  % __bits_per_word);
            __dn -= __ddn;
            if (__dn > 0)
            {
                __m = ~__storage_type(0) >> (__bits_per_word - __dn);
                __b2 = *__result.__seg_ & __m;
                *__result.__seg_ &= ~__m;
                unsigned __s = __first.__ctz_ + __ddn;
                *__result.__seg_ |= __b1 >> __s;
                *__first.__seg_  |= __b2 << __s;
                __result.__ctz_ = static_cast<unsigned>(__dn);
            }
            ++__first.__seg_;
            // __first.__ctz_ = 0;
        }
        // __first.__ctz_ == 0;
        // do middle words
        __storage_type __m = ~__storage_type(0) << __result.__ctz_;
        unsigned __clz_r = __bits_per_word - __result.__ctz_;
        for (; __n >= __bits_per_word; __n -= __bits_per_word, ++__first.__seg_)
        {
            __storage_type __b1 = *__first.__seg_;
            __storage_type __b2 = *__result.__seg_ & __m;
            *__result.__seg_ &= ~__m;
            *__result.__seg_ |= __b1 << __result.__ctz_;
            *__first.__seg_  = __b2 >> __result.__ctz_;
            ++__result.__seg_;
            __b2 = *__result.__seg_ & ~__m;
            *__result.__seg_ &= __m;
            *__result.__seg_ |= __b1 >> __clz_r;
            *__first.__seg_  |= __b2 << __clz_r;
        }
        // do last word
        if (__n > 0)
        {
            __m = ~__storage_type(0) >> (__bits_per_word - __n);
            __storage_type __b1 = *__first.__seg_ & __m;
            *__first.__seg_ &= ~__m;
            __storage_type __dn = _VSTD::min<__storage_type>(__n, __clz_r);
            __m = (~__storage_type(0) << __result.__ctz_) & (~__storage_type(0) >> (__clz_r - __dn));
            __storage_type __b2 = *__result.__seg_ & __m;
            *__result.__seg_ &= ~__m;
            *__result.__seg_ |= __b1 << __result.__ctz_;
            *__first.__seg_  |= __b2 >> __result.__ctz_;
            __result.__seg_ += (__dn + __result.__ctz_) / __bits_per_word;
            __result.__ctz_ = static_cast<unsigned>((__dn + __result.__ctz_)  % __bits_per_word);
            __n -= __dn;
            if (__n > 0)
            {
                __m = ~__storage_type(0) >> (__bits_per_word - __n);
                __b2 = *__result.__seg_ & __m;
                *__result.__seg_ &= ~__m;
                *__result.__seg_ |= __b1 >> __dn;
                *__first.__seg_  |= __b2 << __dn;
                __result.__ctz_ = static_cast<unsigned>(__n);
            }
        }
    }
    return __result;
}

template <class __C1, class __C2>
inline _LIBCPP_INLINE_VISIBILITY
__bit_iterator<__C2, false>
swap_ranges(__bit_iterator<__C1, false> __first1, __bit_iterator<__C1, false> __last1,
            __bit_iterator<__C2, false> __first2)
{
    if (__first1.__ctz_ == __first2.__ctz_)
        return __swap_ranges_aligned(__first1, __last1, __first2);
    return __swap_ranges_unaligned(__first1, __last1, __first2);
}

// rotate

template <class _Cp>
struct __bit_array
{
    typedef typename _Cp::difference_type difference_type;
    typedef typename _Cp::__storage_type  __storage_type;
    typedef typename _Cp::__storage_pointer __storage_pointer;
    typedef typename _Cp::iterator        iterator;
    static const unsigned __bits_per_word = _Cp::__bits_per_word;
    static const unsigned _Np = 4;

    difference_type __size_;
    __storage_type __word_[_Np];

    _LIBCPP_INLINE_VISIBILITY static difference_type capacity()
        {return static_cast<difference_type>(_Np * __bits_per_word);}
    _LIBCPP_INLINE_VISIBILITY explicit __bit_array(difference_type __s) : __size_(__s) {}
    _LIBCPP_INLINE_VISIBILITY iterator begin()
    {
        return iterator(pointer_traits<__storage_pointer>::pointer_to(__word_[0]), 0);
    }
    _LIBCPP_INLINE_VISIBILITY iterator end()
    {
        return iterator(pointer_traits<__storage_pointer>::pointer_to(__word_[0]) + __size_ / __bits_per_word,
                                                  static_cast<unsigned>(__size_ % __bits_per_word));
    }
};

template <class _Cp>
__bit_iterator<_Cp, false>
rotate(__bit_iterator<_Cp, false> __first, __bit_iterator<_Cp, false> __middle, __bit_iterator<_Cp, false> __last)
{
    typedef __bit_iterator<_Cp, false> _I1;
    typedef  typename _I1::difference_type difference_type;
    difference_type __d1 = __middle - __first;
    difference_type __d2 = __last - __middle;
    _I1 __r = __first + __d2;
    while (__d1 != 0 && __d2 != 0)
    {
        if (__d1 <= __d2)
        {
            if (__d1 <= __bit_array<_Cp>::capacity())
            {
                __bit_array<_Cp> __b(__d1);
                _VSTD::copy(__first, __middle, __b.begin());
                _VSTD::copy(__b.begin(), __b.end(), _VSTD::copy(__middle, __last, __first));
                break;
            }
            else
            {
                __bit_iterator<_Cp, false> __mp = _VSTD::swap_ranges(__first, __middle, __middle);
                __first = __middle;
                __middle = __mp;
                __d2 -= __d1;
            }
        }
        else
        {
            if (__d2 <= __bit_array<_Cp>::capacity())
            {
                __bit_array<_Cp> __b(__d2);
                _VSTD::copy(__middle, __last, __b.begin());
                _VSTD::copy_backward(__b.begin(), __b.end(), _VSTD::copy_backward(__first, __middle, __last));
                break;
            }
            else
            {
                __bit_iterator<_Cp, false> __mp = __first + __d2;
                _VSTD::swap_ranges(__first, __mp, __middle);
                __first = __mp;
                __d1 -= __d2;
            }
        }
    }
    return __r;
}

// equal

template <class _Cp, bool _IC1, bool _IC2>
bool
__equal_unaligned(__bit_iterator<_Cp, _IC1> __first1, __bit_iterator<_Cp, _IC1> __last1,
                  __bit_iterator<_Cp, _IC2> __first2)
{
    typedef __bit_iterator<_Cp, _IC1> _It;
    typedef  typename _It::difference_type difference_type;
    typedef typename _It::__storage_type __storage_type;
    static const int __bits_per_word = _It::__bits_per_word;
    difference_type __n = __last1 - __first1;
    if (__n > 0)
    {
        // do first word
        if (__first1.__ctz_ != 0)
        {
            unsigned __clz_f = __bits_per_word - __first1.__ctz_;
            difference_type __dn = _VSTD::min(static_cast<difference_type>(__clz_f), __n);
            __n -= __dn;
            __storage_type __m = (~__storage_type(0) << __first1.__ctz_) & (~__storage_type(0) >> (__clz_f - __dn));
            __storage_type __b = *__first1.__seg_ & __m;
            unsigned __clz_r = __bits_per_word - __first2.__ctz_;
            __storage_type __ddn = _VSTD::min<__storage_type>(__dn, __clz_r);
            __m = (~__storage_type(0) << __first2.__ctz_) & (~__storage_type(0) >> (__clz_r - __ddn));
            if (__first2.__ctz_ > __first1.__ctz_)
            {
                if ((*__first2.__seg_ & __m) != (__b << (__first2.__ctz_ - __first1.__ctz_)))
                    return false;
            }
            else
            {
                if ((*__first2.__seg_ & __m) != (__b >> (__first1.__ctz_ - __first2.__ctz_)))
                    return false;
            }
            __first2.__seg_ += (__ddn + __first2.__ctz_) / __bits_per_word;
            __first2.__ctz_ = static_cast<unsigned>((__ddn + __first2.__ctz_)  % __bits_per_word);
            __dn -= __ddn;
            if (__dn > 0)
            {
                __m = ~__storage_type(0) >> (__bits_per_word - __dn);
                if ((*__first2.__seg_ & __m) != (__b >> (__first1.__ctz_ + __ddn)))
                    return false;
                __first2.__ctz_ = static_cast<unsigned>(__dn);
            }
            ++__first1.__seg_;
            // __first1.__ctz_ = 0;
        }
        // __first1.__ctz_ == 0;
        // do middle words
        unsigned __clz_r = __bits_per_word - __first2.__ctz_;
        __storage_type __m = ~__storage_type(0) << __first2.__ctz_;
        for (; __n >= __bits_per_word; __n -= __bits_per_word, ++__first1.__seg_)
        {
            __storage_type __b = *__first1.__seg_;
            if ((*__first2.__seg_ & __m) != (__b << __first2.__ctz_))
                return false;
            ++__first2.__seg_;
            if ((*__first2.__seg_ & ~__m) != (__b >> __clz_r))
                return false;
        }
        // do last word
        if (__n > 0)
        {
            __m = ~__storage_type(0) >> (__bits_per_word - __n);
            __storage_type __b = *__first1.__seg_ & __m;
            __storage_type __dn = _VSTD::min(__n, static_cast<difference_type>(__clz_r));
            __m = (~__storage_type(0) << __first2.__ctz_) & (~__storage_type(0) >> (__clz_r - __dn));
            if ((*__first2.__seg_ & __m) != (__b << __first2.__ctz_))
                return false;
            __first2.__seg_ += (__dn + __first2.__ctz_) / __bits_per_word;
            __first2.__ctz_ = static_cast<unsigned>((__dn + __first2.__ctz_)  % __bits_per_word);
            __n -= __dn;
            if (__n > 0)
            {
                __m = ~__storage_type(0) >> (__bits_per_word - __n);
                if ((*__first2.__seg_ & __m) != (__b >> __dn))
                    return false;
            }
        }
    }
    return true;
}

template <class _Cp, bool _IC1, bool _IC2>
bool
__equal_aligned(__bit_iterator<_Cp, _IC1> __first1, __bit_iterator<_Cp, _IC1> __last1,
                __bit_iterator<_Cp, _IC2> __first2)
{
    typedef __bit_iterator<_Cp, _IC1> _It;
    typedef  typename _It::difference_type difference_type;
    typedef typename _It::__storage_type __storage_type;
    static const int __bits_per_word = _It::__bits_per_word;
    difference_type __n = __last1 - __first1;
    if (__n > 0)
    {
        // do first word
        if (__first1.__ctz_ != 0)
        {
            unsigned __clz = __bits_per_word - __first1.__ctz_;
            difference_type __dn = _VSTD::min(static_cast<difference_type>(__clz), __n);
            __n -= __dn;
            __storage_type __m = (~__storage_type(0) << __first1.__ctz_) & (~__storage_type(0) >> (__clz - __dn));
            if ((*__first2.__seg_ & __m) != (*__first1.__seg_ & __m))
                return false;
            ++__first2.__seg_;
            ++__first1.__seg_;
            // __first1.__ctz_ = 0;
            // __first2.__ctz_ = 0;
        }
        // __first1.__ctz_ == 0;
        // __first2.__ctz_ == 0;
        // do middle words
        for (; __n >= __bits_per_word; __n -= __bits_per_word, ++__first1.__seg_, ++__first2.__seg_)
            if (*__first2.__seg_ != *__first1.__seg_)
                return false;
        // do last word
        if (__n > 0)
        {
            __storage_type __m = ~__storage_type(0) >> (__bits_per_word - __n);
            if ((*__first2.__seg_ & __m) != (*__first1.__seg_ & __m))
                return false;
        }
    }
    return true;
}

template <class _Cp, bool _IC1, bool _IC2>
inline _LIBCPP_INLINE_VISIBILITY
bool
equal(__bit_iterator<_Cp, _IC1> __first1, __bit_iterator<_Cp, _IC1> __last1, __bit_iterator<_Cp, _IC2> __first2)
{
    if (__first1.__ctz_ == __first2.__ctz_)
        return __equal_aligned(__first1, __last1, __first2);
    return __equal_unaligned(__first1, __last1, __first2);
}

template <class _Cp, bool _IsConst,
          typename _Cp::__storage_type>
class __bit_iterator
{
public:
    typedef typename _Cp::difference_type                                                          difference_type;
    typedef bool                                                                                  value_type;
    typedef __bit_iterator                                                                        pointer;
    typedef typename conditional<_IsConst, __bit_const_reference<_Cp>, __bit_reference<_Cp> >::type reference;
    typedef random_access_iterator_tag                                                            iterator_category;

private:
    typedef typename _Cp::__storage_type                                           __storage_type;
    typedef typename conditional<_IsConst, typename _Cp::__const_storage_pointer,
                                           typename _Cp::__storage_pointer>::type  __storage_pointer;
    static const unsigned __bits_per_word = _Cp::__bits_per_word;

    __storage_pointer __seg_;
    unsigned          __ctz_;

public:
    _LIBCPP_INLINE_VISIBILITY __bit_iterator() _NOEXCEPT
#if _LIBCPP_STD_VER > 11
    : __seg_(nullptr), __ctz_(0)
#endif
    {}

    _LIBCPP_INLINE_VISIBILITY
    __bit_iterator(const __bit_iterator<_Cp, false>& __it) _NOEXCEPT
        : __seg_(__it.__seg_), __ctz_(__it.__ctz_) {}

    _LIBCPP_INLINE_VISIBILITY reference operator*() const _NOEXCEPT
        {return reference(__seg_, __storage_type(1) << __ctz_);}

    _LIBCPP_INLINE_VISIBILITY __bit_iterator& operator++()
    {
        if (__ctz_ != __bits_per_word-1)
            ++__ctz_;
        else
        {
            __ctz_ = 0;
            ++__seg_;
        }
        return *this;
    }

    _LIBCPP_INLINE_VISIBILITY __bit_iterator operator++(int)
    {
        __bit_iterator __tmp = *this;
        ++(*this);
        return __tmp;
    }

    _LIBCPP_INLINE_VISIBILITY __bit_iterator& operator--()
    {
        if (__ctz_ != 0)
            --__ctz_;
        else
        {
            __ctz_ = __bits_per_word - 1;
            --__seg_;
        }
        return *this;
    }

    _LIBCPP_INLINE_VISIBILITY __bit_iterator operator--(int)
    {
        __bit_iterator __tmp = *this;
        --(*this);
        return __tmp;
    }

    _LIBCPP_INLINE_VISIBILITY __bit_iterator& operator+=(difference_type __n)
    {
        if (__n >= 0)
            __seg_ += (__n + __ctz_) / __bits_per_word;
        else
            __seg_ += static_cast<difference_type>(__n - __bits_per_word + __ctz_ + 1)
                    / static_cast<difference_type>(__bits_per_word);
        __n &= (__bits_per_word - 1);
        __ctz_ = static_cast<unsigned>((__n + __ctz_)  % __bits_per_word);
        return *this;
    }

    _LIBCPP_INLINE_VISIBILITY __bit_iterator& operator-=(difference_type __n)
    {
        return *this += -__n;
    }

    _LIBCPP_INLINE_VISIBILITY __bit_iterator operator+(difference_type __n) const
    {
        __bit_iterator __t(*this);
        __t += __n;
        return __t;
    }

    _LIBCPP_INLINE_VISIBILITY __bit_iterator operator-(difference_type __n) const
    {
        __bit_iterator __t(*this);
        __t -= __n;
        return __t;
    }

    _LIBCPP_INLINE_VISIBILITY
    friend __bit_iterator operator+(difference_type __n, const __bit_iterator& __it) {return __it + __n;}

    _LIBCPP_INLINE_VISIBILITY
    friend difference_type operator-(const __bit_iterator& __x, const __bit_iterator& __y)
        {return (__x.__seg_ - __y.__seg_) * __bits_per_word + __x.__ctz_ - __y.__ctz_;}

    _LIBCPP_INLINE_VISIBILITY reference operator[](difference_type __n) const {return *(*this + __n);}

    _LIBCPP_INLINE_VISIBILITY friend bool operator==(const __bit_iterator& __x, const __bit_iterator& __y)
        {return __x.__seg_ == __y.__seg_ && __x.__ctz_ == __y.__ctz_;}

    _LIBCPP_INLINE_VISIBILITY friend bool operator!=(const __bit_iterator& __x, const __bit_iterator& __y)
        {return !(__x == __y);}

    _LIBCPP_INLINE_VISIBILITY friend bool operator<(const __bit_iterator& __x, const __bit_iterator& __y)
        {return __x.__seg_ < __y.__seg_ || (__x.__seg_ == __y.__seg_ && __x.__ctz_ < __y.__ctz_);}

    _LIBCPP_INLINE_VISIBILITY friend bool operator>(const __bit_iterator& __x, const __bit_iterator& __y)
        {return __y < __x;}

    _LIBCPP_INLINE_VISIBILITY friend bool operator<=(const __bit_iterator& __x, const __bit_iterator& __y)
        {return !(__y < __x);}

    _LIBCPP_INLINE_VISIBILITY friend bool operator>=(const __bit_iterator& __x, const __bit_iterator& __y)
        {return !(__x < __y);}

private:
    _LIBCPP_INLINE_VISIBILITY
    __bit_iterator(__storage_pointer __s, unsigned __ctz) _NOEXCEPT
        : __seg_(__s), __ctz_(__ctz) {}

    friend typename _Cp::__self;

    friend class __bit_reference<_Cp>;
    friend class __bit_const_reference<_Cp>;
    friend class __bit_iterator<_Cp, true>;
    template <class _Dp> friend struct __bit_array;
    template <class _Dp> friend void __fill_n_false(__bit_iterator<_Dp, false> __first, typename _Dp::size_type __n);
    template <class _Dp> friend void __fill_n_true(__bit_iterator<_Dp, false> __first, typename _Dp::size_type __n);
    template <class _Dp, bool _IC> friend __bit_iterator<_Dp, false> __copy_aligned(__bit_iterator<_Dp, _IC> __first,
                                                                                  __bit_iterator<_Dp, _IC> __last,
                                                                                  __bit_iterator<_Dp, false> __result);
    template <class _Dp, bool _IC> friend __bit_iterator<_Dp, false> __copy_unaligned(__bit_iterator<_Dp, _IC> __first,
                                                                                    __bit_iterator<_Dp, _IC> __last,
                                                                                    __bit_iterator<_Dp, false> __result);
    template <class _Dp, bool _IC> friend __bit_iterator<_Dp, false> copy(__bit_iterator<_Dp, _IC> __first,
                                                                        __bit_iterator<_Dp, _IC> __last,
                                                                        __bit_iterator<_Dp, false> __result);
    template <class _Dp, bool _IC> friend __bit_iterator<_Dp, false> __copy_backward_aligned(__bit_iterator<_Dp, _IC> __first,
                                                                                           __bit_iterator<_Dp, _IC> __last,
                                                                                           __bit_iterator<_Dp, false> __result);
    template <class _Dp, bool _IC> friend __bit_iterator<_Dp, false> __copy_backward_unaligned(__bit_iterator<_Dp, _IC> __first,
                                                                                             __bit_iterator<_Dp, _IC> __last,
                                                                                             __bit_iterator<_Dp, false> __result);
    template <class _Dp, bool _IC> friend __bit_iterator<_Dp, false> copy_backward(__bit_iterator<_Dp, _IC> __first,
                                                                                 __bit_iterator<_Dp, _IC> __last,
                                                                                 __bit_iterator<_Dp, false> __result);
    template <class __C1, class __C2>friend __bit_iterator<__C2, false> __swap_ranges_aligned(__bit_iterator<__C1, false>,
                                                                                           __bit_iterator<__C1, false>,
                                                                                           __bit_iterator<__C2, false>);
    template <class __C1, class __C2>friend __bit_iterator<__C2, false> __swap_ranges_unaligned(__bit_iterator<__C1, false>,
                                                                                             __bit_iterator<__C1, false>,
                                                                                             __bit_iterator<__C2, false>);
    template <class __C1, class __C2>friend __bit_iterator<__C2, false> swap_ranges(__bit_iterator<__C1, false>,
                                                                                 __bit_iterator<__C1, false>,
                                                                                 __bit_iterator<__C2, false>);
    template <class _Dp> friend __bit_iterator<_Dp, false> rotate(__bit_iterator<_Dp, false>,
                                                                __bit_iterator<_Dp, false>,
                                                                __bit_iterator<_Dp, false>);
    template <class _Dp, bool _IC1, bool _IC2> friend bool __equal_aligned(__bit_iterator<_Dp, _IC1>,
                                                    __bit_iterator<_Dp, _IC1>,
                                                    __bit_iterator<_Dp, _IC2>);
    template <class _Dp, bool _IC1, bool _IC2> friend bool __equal_unaligned(__bit_iterator<_Dp, _IC1>,
                                                      __bit_iterator<_Dp, _IC1>,
                                                      __bit_iterator<_Dp, _IC2>);
    template <class _Dp, bool _IC1, bool _IC2> friend bool equal(__bit_iterator<_Dp, _IC1>,
                                                                __bit_iterator<_Dp, _IC1>,
                                                                __bit_iterator<_Dp, _IC2>);
    template <class _Dp, bool _IC> friend __bit_iterator<_Dp, _IC> __find_bool_true(__bit_iterator<_Dp, _IC>,
                                                                          typename _Dp::size_type);
    template <class _Dp, bool _IC> friend __bit_iterator<_Dp, _IC> __find_bool_false(__bit_iterator<_Dp, _IC>,
                                                                           typename _Dp::size_type);
    template <class _Dp, bool _IC> friend typename __bit_iterator<_Dp, _IC>::difference_type
                   __count_bool_true(__bit_iterator<_Dp, _IC>, typename _Dp::size_type);
    template <class _Dp, bool _IC> friend typename __bit_iterator<_Dp, _IC>::difference_type
                   __count_bool_false(__bit_iterator<_Dp, _IC>, typename _Dp::size_type);
};

_LIBCPP_END_NAMESPACE_STD

_LIBCPP_POP_MACROS

#endif  // _LIBCPP___BIT_REFERENCE
