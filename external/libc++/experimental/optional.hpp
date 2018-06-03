// -*- C++ -*-
//===-------------------------- optional ----------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is dual licensed under the MIT and the University of Illinois Open
// Source Licenses. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef _LIBCPP_EXPERIMENTAL_OPTIONAL
#define _LIBCPP_EXPERIMENTAL_OPTIONAL

#include <experimental/__config.hpp>
#include <functional.hpp>
#if _LIBCPP_STD_VER > 11
#include <initializer_list.hpp>
#include <type_traits.hpp>
#include <new.hpp>
#include <__functional_base.hpp>
#include <__debug.hpp>
#endif

#if !defined(_LIBCPP_HAS_NO_PRAGMA_SYSTEM_HEADER)
#pragma GCC system_header
#endif

_LIBCPP_PUSH_MACROS
#include <__undef_macros.hpp>

#if _LIBCPP_STD_VER > 11

_LIBCPP_BEGIN_NAMESPACE_LFTS

struct in_place_t {};
constexpr in_place_t in_place{};

struct nullopt_t
{
    explicit constexpr nullopt_t(int) noexcept {}
};

constexpr nullopt_t nullopt{0};

template <class _Tp, bool = is_trivially_destructible<_Tp>::value>
class __optional_storage
{
protected:
    typedef _Tp value_type;
    union
    {
        char __null_state_;
        value_type __val_;
    };
    bool __engaged_ = false;

    _LIBCPP_INLINE_VISIBILITY
    ~__optional_storage()
    {
        if (__engaged_)
            __val_.~value_type();
    }

    _LIBCPP_INLINE_VISIBILITY
    constexpr __optional_storage() noexcept
        :  __null_state_('\0') {}

    _LIBCPP_INLINE_VISIBILITY
    __optional_storage(const __optional_storage& __x)
        :  __engaged_(__x.__engaged_)
        {
            if (__engaged_)
                ::new((void*)_VSTD::addressof(__val_)) value_type(__x.__val_);
        }

    _LIBCPP_INLINE_VISIBILITY
    __optional_storage(__optional_storage&& __x)
                      noexcept(is_nothrow_move_constructible<value_type>::value)
        :  __engaged_(__x.__engaged_)
        {
            if (__engaged_)
                ::new((void*)_VSTD::addressof(__val_)) value_type(_VSTD::move(__x.__val_));
        }

    _LIBCPP_INLINE_VISIBILITY
    constexpr __optional_storage(const value_type& __v)
        :  __val_(__v),
           __engaged_(true) {}

    _LIBCPP_INLINE_VISIBILITY
    constexpr __optional_storage(value_type&& __v)
        :  __val_(_VSTD::move(__v)),
           __engaged_(true) {}

    template <class... _Args>
    _LIBCPP_INLINE_VISIBILITY
    constexpr
    explicit __optional_storage(in_place_t, _Args&&... __args)
       :  __val_(_VSTD::forward<_Args>(__args)...),
           __engaged_(true) {}
};

template <class _Tp>
class __optional_storage<_Tp, true>
{
protected:
    typedef _Tp value_type;
    union
    {
        char __null_state_;
        value_type __val_;
    };
    bool __engaged_ = false;

    _LIBCPP_INLINE_VISIBILITY
    constexpr __optional_storage() noexcept
        :  __null_state_('\0') {}

    _LIBCPP_INLINE_VISIBILITY
    __optional_storage(const __optional_storage& __x)
        :  __engaged_(__x.__engaged_)
        {
            if (__engaged_)
                ::new((void*)_VSTD::addressof(__val_)) value_type(__x.__val_);
        }

    _LIBCPP_INLINE_VISIBILITY
    __optional_storage(__optional_storage&& __x)
                      noexcept(is_nothrow_move_constructible<value_type>::value)
        :  __engaged_(__x.__engaged_)
        {
            if (__engaged_)
                ::new((void*)_VSTD::addressof(__val_)) value_type(_VSTD::move(__x.__val_));
        }

    _LIBCPP_INLINE_VISIBILITY
    constexpr __optional_storage(const value_type& __v)
        :  __val_(__v),
           __engaged_(true) {}

    _LIBCPP_INLINE_VISIBILITY
    constexpr __optional_storage(value_type&& __v)
        :  __val_(_VSTD::move(__v)),
           __engaged_(true) {}

    template <class... _Args>
    _LIBCPP_INLINE_VISIBILITY
    constexpr
    explicit __optional_storage(in_place_t, _Args&&... __args)
       :  __val_(_VSTD::forward<_Args>(__args)...),
           __engaged_(true) {}
};

template <class _Tp>
class optional
    : private __optional_storage<_Tp>
{
    typedef __optional_storage<_Tp> __base;
public:
    typedef _Tp value_type;

    static_assert(!is_reference<value_type>::value,
              "Instantiation of optional with a reference type is ill-formed.");
    static_assert(!is_same<typename remove_cv<value_type>::type, in_place_t>::value,
              "Instantiation of optional with a in_place_t type is ill-formed.");
    static_assert(!is_same<typename remove_cv<value_type>::type, nullopt_t>::value,
              "Instantiation of optional with a nullopt_t type is ill-formed.");
    static_assert(is_object<value_type>::value,
        "Instantiation of optional with a non-object type is undefined behavior.");
    static_assert(is_nothrow_destructible<value_type>::value,
        "Instantiation of optional with an object type that is not noexcept destructible is undefined behavior.");

    _LIBCPP_INLINE_VISIBILITY constexpr optional() noexcept {}
    _LIBCPP_INLINE_VISIBILITY optional(const optional&) = default;
    _LIBCPP_INLINE_VISIBILITY optional(optional&&) = default;
    _LIBCPP_INLINE_VISIBILITY ~optional() = default;
    _LIBCPP_INLINE_VISIBILITY constexpr optional(nullopt_t) noexcept {}
    _LIBCPP_INLINE_VISIBILITY constexpr optional(const value_type& __v)
        : __base(__v) {}
    _LIBCPP_INLINE_VISIBILITY constexpr optional(value_type&& __v)
        : __base(_VSTD::move(__v)) {}

    template <class... _Args,
              class = typename enable_if
                      <
                           is_constructible<value_type, _Args...>::value
                      >::type
             >
    _LIBCPP_INLINE_VISIBILITY
    constexpr
    explicit optional(in_place_t, _Args&&... __args)
        : __base(in_place, _VSTD::forward<_Args>(__args)...) {}

    template <class _Up, class... _Args,
              class = typename enable_if
                      <
                           is_constructible<value_type, initializer_list<_Up>&, _Args...>::value
                      >::type
             >
    _LIBCPP_INLINE_VISIBILITY
    constexpr
    explicit optional(in_place_t, initializer_list<_Up> __il, _Args&&... __args)
        : __base(in_place, __il, _VSTD::forward<_Args>(__args)...) {}

    _LIBCPP_INLINE_VISIBILITY
    optional& operator=(nullopt_t) noexcept
    {
        if (this->__engaged_)
        {
            this->__val_.~value_type();
            this->__engaged_ = false;
        }
        return *this;
    }

    _LIBCPP_INLINE_VISIBILITY
    optional&
    operator=(const optional& __opt)
    {
        if (this->__engaged_ == __opt.__engaged_)
        {
            if (this->__engaged_)
                this->__val_ = __opt.__val_;
        }
        else
        {
            if (this->__engaged_)
                this->__val_.~value_type();
            else
                ::new((void*)_VSTD::addressof(this->__val_)) value_type(__opt.__val_);
            this->__engaged_ = __opt.__engaged_;
        }
        return *this;
    }

    _LIBCPP_INLINE_VISIBILITY
    optional&
    operator=(optional&& __opt)
        noexcept(is_nothrow_move_assignable<value_type>::value &&
                 is_nothrow_move_constructible<value_type>::value)
    {
        if (this->__engaged_ == __opt.__engaged_)
        {
            if (this->__engaged_)
                this->__val_ = _VSTD::move(__opt.__val_);
        }
        else
        {
            if (this->__engaged_)
                this->__val_.~value_type();
            else
                ::new((void*)_VSTD::addressof(this->__val_))
                    value_type(_VSTD::move(__opt.__val_));
            this->__engaged_ = __opt.__engaged_;
        }
        return *this;
    }

    template <class _Up,
              class = typename enable_if
                      <
                          is_same<typename remove_reference<_Up>::type, value_type>::value &&
                          is_constructible<value_type, _Up>::value &&
                          is_assignable<value_type&, _Up>::value
                      >::type
             >
    _LIBCPP_INLINE_VISIBILITY
    optional&
    operator=(_Up&& __v)
    {
        if (this->__engaged_)
            this->__val_ = _VSTD::forward<_Up>(__v);
        else
        {
            ::new((void*)_VSTD::addressof(this->__val_)) value_type(_VSTD::forward<_Up>(__v));
            this->__engaged_ = true;
        }
        return *this;
    }

    template <class... _Args,
              class = typename enable_if
                      <
                          is_constructible<value_type, _Args...>::value
                      >::type
             >
    _LIBCPP_INLINE_VISIBILITY
    void
    emplace(_Args&&... __args)
    {
        *this = nullopt;
        ::new((void*)_VSTD::addressof(this->__val_))
            value_type(_VSTD::forward<_Args>(__args)...);
        this->__engaged_ = true;
    }

    template <class _Up, class... _Args,
              class = typename enable_if
                      <
                          is_constructible<value_type, initializer_list<_Up>&, _Args...>::value
                      >::type
             >
    _LIBCPP_INLINE_VISIBILITY
    void
    emplace(initializer_list<_Up> __il, _Args&&... __args)
    {
        *this = nullopt;
        ::new((void*)_VSTD::addressof(this->__val_))
            value_type(__il, _VSTD::forward<_Args>(__args)...);
        this->__engaged_ = true;
    }

    _LIBCPP_INLINE_VISIBILITY
    void
    swap(optional& __opt)
        noexcept(is_nothrow_move_constructible<value_type>::value &&
                 __is_nothrow_swappable<value_type>::value)
    {
        using _VSTD::swap;
        if (this->__engaged_ == __opt.__engaged_)
        {
            if (this->__engaged_)
                swap(this->__val_, __opt.__val_);
        }
        else
        {
            if (this->__engaged_)
            {
                ::new((void*)_VSTD::addressof(__opt.__val_))
                    value_type(_VSTD::move(this->__val_));
                this->__val_.~value_type();
            }
            else
            {
                ::new((void*)_VSTD::addressof(this->__val_))
                    value_type(_VSTD::move(__opt.__val_));
                __opt.__val_.~value_type();
            }
            swap(this->__engaged_, __opt.__engaged_);
        }
    }

    _LIBCPP_INLINE_VISIBILITY
    constexpr
    value_type const*
    operator->() const
    {
        _LIBCPP_ASSERT(this->__engaged_, "optional operator-> called for disengaged value");
#ifndef _LIBCPP_HAS_NO_BUILTIN_ADDRESSOF
        return __builtin_addressof(this->__val_);
#else
        return __operator_arrow(__has_operator_addressof<value_type>{});
#endif
    }

    _LIBCPP_INLINE_VISIBILITY
    value_type*
    operator->()
    {
        _LIBCPP_ASSERT(this->__engaged_, "optional operator-> called for disengaged value");
        return _VSTD::addressof(this->__val_);
    }

    _LIBCPP_INLINE_VISIBILITY
    constexpr
    const value_type&
    operator*() const
    {
        _LIBCPP_ASSERT(this->__engaged_, "optional operator* called for disengaged value");
        return this->__val_;
    }

    _LIBCPP_INLINE_VISIBILITY
    value_type&
    operator*()
    {
        _LIBCPP_ASSERT(this->__engaged_, "optional operator* called for disengaged value");
        return this->__val_;
    }

    _LIBCPP_INLINE_VISIBILITY
    constexpr explicit operator bool() const noexcept {return this->__engaged_;}

    _LIBCPP_NORETURN _LIBCPP_INLINE_VISIBILITY
#ifndef _LIBCPP_NO_EXCEPTIONS
_LIBCPP_AVAILABILITY_BAD_OPTIONAL_ACCESS
#endif
    constexpr void __throw_bad_optional_access() const
    {
#ifndef _LIBCPP_NO_EXCEPTIONS
        throw bad_optional_access();
#else
        abort();
#endif
    }

    _LIBCPP_INLINE_VISIBILITY _LIBCPP_AVAILABILITY_BAD_OPTIONAL_ACCESS
    constexpr value_type const& value() const
    {
        if (!this->__engaged_)
            __throw_bad_optional_access();
        return this->__val_;
    }

    _LIBCPP_INLINE_VISIBILITY _LIBCPP_AVAILABILITY_BAD_OPTIONAL_ACCESS
    value_type& value()
    {
        if (!this->__engaged_)
            __throw_bad_optional_access();
        return this->__val_;
    }

    template <class _Up>
    _LIBCPP_INLINE_VISIBILITY
    constexpr value_type value_or(_Up&& __v) const&
    {
        static_assert(is_copy_constructible<value_type>::value,
                      "optional<T>::value_or: T must be copy constructible");
        static_assert(is_convertible<_Up, value_type>::value,
                      "optional<T>::value_or: U must be convertible to T");
        return this->__engaged_ ? this->__val_ :
                                  static_cast<value_type>(_VSTD::forward<_Up>(__v));
    }

    template <class _Up>
    _LIBCPP_INLINE_VISIBILITY
    value_type value_or(_Up&& __v) &&
    {
        static_assert(is_move_constructible<value_type>::value,
                      "optional<T>::value_or: T must be move constructible");
        static_assert(is_convertible<_Up, value_type>::value,
                      "optional<T>::value_or: U must be convertible to T");
        return this->__engaged_ ? _VSTD::move(this->__val_) :
                                  static_cast<value_type>(_VSTD::forward<_Up>(__v));
    }

private:
    _LIBCPP_INLINE_VISIBILITY
    value_type const*
    __operator_arrow(true_type) const
    {
        return _VSTD::addressof(this->__val_);
    }

    _LIBCPP_INLINE_VISIBILITY
    constexpr
    value_type const*
    __operator_arrow(false_type) const
    {
        return &this->__val_;
    }
};

// Comparisons between optionals
template <class _Tp>
inline _LIBCPP_INLINE_VISIBILITY
constexpr
bool
operator==(const optional<_Tp>& __x, const optional<_Tp>& __y)
{
    if (static_cast<bool>(__x) != static_cast<bool>(__y))
        return false;
    if (!static_cast<bool>(__x))
        return true;
    return *__x == *__y;
}

template <class _Tp>
inline _LIBCPP_INLINE_VISIBILITY
constexpr
bool
operator!=(const optional<_Tp>& __x, const optional<_Tp>& __y)
{
    return !(__x == __y);
}

template <class _Tp>
inline _LIBCPP_INLINE_VISIBILITY
constexpr
bool
operator<(const optional<_Tp>& __x, const optional<_Tp>& __y)
{
    if (!static_cast<bool>(__y))
        return false;
    if (!static_cast<bool>(__x))
        return true;
    return *__x < *__y;
}

template <class _Tp>
inline _LIBCPP_INLINE_VISIBILITY
constexpr
bool
operator>(const optional<_Tp>& __x, const optional<_Tp>& __y)
{
    return __y < __x;
}

template <class _Tp>
inline _LIBCPP_INLINE_VISIBILITY
constexpr
bool
operator<=(const optional<_Tp>& __x, const optional<_Tp>& __y)
{
    return !(__y < __x);
}

template <class _Tp>
inline _LIBCPP_INLINE_VISIBILITY
constexpr
bool
operator>=(const optional<_Tp>& __x, const optional<_Tp>& __y)
{
    return !(__x < __y);
}


// Comparisons with nullopt
template <class _Tp>
inline _LIBCPP_INLINE_VISIBILITY
constexpr
bool
operator==(const optional<_Tp>& __x, nullopt_t) noexcept
{
    return !static_cast<bool>(__x);
}

template <class _Tp>
inline _LIBCPP_INLINE_VISIBILITY
constexpr
bool
operator==(nullopt_t, const optional<_Tp>& __x) noexcept
{
    return !static_cast<bool>(__x);
}

template <class _Tp>
inline _LIBCPP_INLINE_VISIBILITY
constexpr
bool
operator!=(const optional<_Tp>& __x, nullopt_t) noexcept
{
    return static_cast<bool>(__x);
}

template <class _Tp>
inline _LIBCPP_INLINE_VISIBILITY
constexpr
bool
operator!=(nullopt_t, const optional<_Tp>& __x) noexcept
{
    return static_cast<bool>(__x);
}

template <class _Tp>
inline _LIBCPP_INLINE_VISIBILITY
constexpr
bool
operator<(const optional<_Tp>&, nullopt_t) noexcept
{
    return false;
}

template <class _Tp>
inline _LIBCPP_INLINE_VISIBILITY
constexpr
bool
operator<(nullopt_t, const optional<_Tp>& __x) noexcept
{
    return static_cast<bool>(__x);
}

template <class _Tp>
inline _LIBCPP_INLINE_VISIBILITY
constexpr
bool
operator<=(const optional<_Tp>& __x, nullopt_t) noexcept
{
    return !static_cast<bool>(__x);
}

template <class _Tp>
inline _LIBCPP_INLINE_VISIBILITY
constexpr
bool
operator<=(nullopt_t, const optional<_Tp>&) noexcept
{
    return true;
}

template <class _Tp>
inline _LIBCPP_INLINE_VISIBILITY
constexpr
bool
operator>(const optional<_Tp>& __x, nullopt_t) noexcept
{
    return static_cast<bool>(__x);
}

template <class _Tp>
inline _LIBCPP_INLINE_VISIBILITY
constexpr
bool
operator>(nullopt_t, const optional<_Tp>&) noexcept
{
    return false;
}

template <class _Tp>
inline _LIBCPP_INLINE_VISIBILITY
constexpr
bool
operator>=(const optional<_Tp>&, nullopt_t) noexcept
{
    return true;
}

template <class _Tp>
inline _LIBCPP_INLINE_VISIBILITY
constexpr
bool
operator>=(nullopt_t, const optional<_Tp>& __x) noexcept
{
    return !static_cast<bool>(__x);
}

// Comparisons with T
template <class _Tp>
inline _LIBCPP_INLINE_VISIBILITY
constexpr
bool
operator==(const optional<_Tp>& __x, const _Tp& __v)
{
    return static_cast<bool>(__x) ? *__x == __v : false;
}

template <class _Tp>
inline _LIBCPP_INLINE_VISIBILITY
constexpr
bool
operator==(const _Tp& __v, const optional<_Tp>& __x)
{
    return static_cast<bool>(__x) ? *__x == __v : false;
}

template <class _Tp>
inline _LIBCPP_INLINE_VISIBILITY
constexpr
bool
operator!=(const optional<_Tp>& __x, const _Tp& __v)
{
    return static_cast<bool>(__x) ? !(*__x == __v) : true;
}

template <class _Tp>
inline _LIBCPP_INLINE_VISIBILITY
constexpr
bool
operator!=(const _Tp& __v, const optional<_Tp>& __x)
{
    return static_cast<bool>(__x) ? !(*__x == __v) : true;
}

template <class _Tp>
inline _LIBCPP_INLINE_VISIBILITY
constexpr
bool
operator<(const optional<_Tp>& __x, const _Tp& __v)
{
    return static_cast<bool>(__x) ? less<_Tp>{}(*__x, __v) : true;
}

template <class _Tp>
inline _LIBCPP_INLINE_VISIBILITY
constexpr
bool
operator<(const _Tp& __v, const optional<_Tp>& __x)
{
    return static_cast<bool>(__x) ? less<_Tp>{}(__v, *__x) : false;
}

template <class _Tp>
inline _LIBCPP_INLINE_VISIBILITY
constexpr
bool
operator<=(const optional<_Tp>& __x, const _Tp& __v)
{
    return !(__x > __v);
}

template <class _Tp>
inline _LIBCPP_INLINE_VISIBILITY
constexpr
bool
operator<=(const _Tp& __v, const optional<_Tp>& __x)
{
    return !(__v > __x);
}

template <class _Tp>
inline _LIBCPP_INLINE_VISIBILITY
constexpr
bool
operator>(const optional<_Tp>& __x, const _Tp& __v)
{
    return static_cast<bool>(__x) ? __v < __x : false;
}

template <class _Tp>
inline _LIBCPP_INLINE_VISIBILITY
constexpr
bool
operator>(const _Tp& __v, const optional<_Tp>& __x)
{
    return static_cast<bool>(__x) ? __x < __v : true;
}

template <class _Tp>
inline _LIBCPP_INLINE_VISIBILITY
constexpr
bool
operator>=(const optional<_Tp>& __x, const _Tp& __v)
{
    return !(__x < __v);
}

template <class _Tp>
inline _LIBCPP_INLINE_VISIBILITY
constexpr
bool
operator>=(const _Tp& __v, const optional<_Tp>& __x)
{
    return !(__v < __x);
}


template <class _Tp>
inline _LIBCPP_INLINE_VISIBILITY
void
swap(optional<_Tp>& __x, optional<_Tp>& __y) noexcept(noexcept(__x.swap(__y)))
{
    __x.swap(__y);
}

template <class _Tp>
inline _LIBCPP_INLINE_VISIBILITY
constexpr
optional<typename decay<_Tp>::type>
make_optional(_Tp&& __v)
{
    return optional<typename decay<_Tp>::type>(_VSTD::forward<_Tp>(__v));
}

_LIBCPP_END_NAMESPACE_LFTS

_LIBCPP_BEGIN_NAMESPACE_STD

template <class _Tp>
struct _LIBCPP_TEMPLATE_VIS hash<std::experimental::optional<_Tp> >
{
    typedef std::experimental::optional<_Tp> argument_type;
    typedef size_t        result_type;

    _LIBCPP_INLINE_VISIBILITY
    result_type operator()(const argument_type& __opt) const _NOEXCEPT
    {
        return static_cast<bool>(__opt) ? hash<_Tp>()(*__opt) : 0;
    }
};

_LIBCPP_END_NAMESPACE_STD

#endif  // _LIBCPP_STD_VER > 11

_LIBCPP_POP_MACROS

#endif  // _LIBCPP_EXPERIMENTAL_OPTIONAL
