// -*- C++ -*-
//===---------------------------- stack -----------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is dual licensed under the MIT and the University of Illinois Open
// Source Licenses. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef _LIBCPP_STACK
#define _LIBCPP_STACK

/*
    stack synopsis

namespace std
{

template <class T, class Container = deque<T>>
class stack
{
public:
    typedef Container                                container_type;
    typedef typename container_type::value_type      value_type;
    typedef typename container_type::reference       reference;
    typedef typename container_type::const_reference const_reference;
    typedef typename container_type::size_type       size_type;

protected:
    container_type c;

public:
    stack() = default;
    ~stack() = default;

    stack(const stack& q) = default;
    stack(stack&& q) = default;

    stack& operator=(const stack& q) = default;
    stack& operator=(stack&& q) = default;

    explicit stack(const container_type& c);
    explicit stack(container_type&& c);
    template <class Alloc> explicit stack(const Alloc& a);
    template <class Alloc> stack(const container_type& c, const Alloc& a);
    template <class Alloc> stack(container_type&& c, const Alloc& a);
    template <class Alloc> stack(const stack& c, const Alloc& a);
    template <class Alloc> stack(stack&& c, const Alloc& a);

    bool empty() const;
    size_type size() const;
    reference top();
    const_reference top() const;

    void push(const value_type& x);
    void push(value_type&& x);
    template <class... Args> reference emplace(Args&&... args); // reference in C++17
    void pop();

    void swap(stack& c) noexcept(is_nothrow_swappable_v<Container>)
};

template <class T, class Container>
  bool operator==(const stack<T, Container>& x, const stack<T, Container>& y);
template <class T, class Container>
  bool operator< (const stack<T, Container>& x, const stack<T, Container>& y);
template <class T, class Container>
  bool operator!=(const stack<T, Container>& x, const stack<T, Container>& y);
template <class T, class Container>
  bool operator> (const stack<T, Container>& x, const stack<T, Container>& y);
template <class T, class Container>
  bool operator>=(const stack<T, Container>& x, const stack<T, Container>& y);
template <class T, class Container>
  bool operator<=(const stack<T, Container>& x, const stack<T, Container>& y);

template <class T, class Container>
  void swap(stack<T, Container>& x, stack<T, Container>& y)
  noexcept(noexcept(x.swap(y)));

}  // std

*/

#include "__config.hpp"
#include "deque.hpp"

#if !defined(_LIBCPP_HAS_NO_PRAGMA_SYSTEM_HEADER)
#pragma GCC system_header
#endif

_LIBCPP_BEGIN_NAMESPACE_STD

template <class _Tp, class _Container = deque<_Tp> > class _LIBCPP_TEMPLATE_VIS stack;

template <class _Tp, class _Container>
_LIBCPP_INLINE_VISIBILITY
bool
operator==(const stack<_Tp, _Container>& __x, const stack<_Tp, _Container>& __y);

template <class _Tp, class _Container>
_LIBCPP_INLINE_VISIBILITY
bool
operator< (const stack<_Tp, _Container>& __x, const stack<_Tp, _Container>& __y);

template <class _Tp, class _Container /*= deque<_Tp>*/>
class _LIBCPP_TEMPLATE_VIS stack
{
public:
    typedef _Container                               container_type;
    typedef typename container_type::value_type      value_type;
    typedef typename container_type::reference       reference;
    typedef typename container_type::const_reference const_reference;
    typedef typename container_type::size_type       size_type;
    static_assert((is_same<_Tp, value_type>::value), "" );

protected:
    container_type c;

public:
    _LIBCPP_INLINE_VISIBILITY
    stack()
        _NOEXCEPT_(is_nothrow_default_constructible<container_type>::value)
        : c() {}

    _LIBCPP_INLINE_VISIBILITY
    stack(const stack& __q) : c(__q.c) {}

    _LIBCPP_INLINE_VISIBILITY
    stack& operator=(const stack& __q) {c = __q.c; return *this;}


#ifndef _LIBCPP_CXX03_LANG
    _LIBCPP_INLINE_VISIBILITY
    stack(stack&& __q)
        _NOEXCEPT_(is_nothrow_move_constructible<container_type>::value)
        : c(_VSTD::move(__q.c)) {}

    _LIBCPP_INLINE_VISIBILITY
    stack& operator=(stack&& __q)
        _NOEXCEPT_(is_nothrow_move_assignable<container_type>::value)
        {c = _VSTD::move(__q.c); return *this;}

    _LIBCPP_INLINE_VISIBILITY
    explicit stack(container_type&& __c) : c(_VSTD::move(__c)) {}
#endif  // _LIBCPP_CXX03_LANG

    _LIBCPP_INLINE_VISIBILITY
    explicit stack(const container_type& __c) : c(__c) {}

    template <class _Alloc>
        _LIBCPP_INLINE_VISIBILITY
        explicit stack(const _Alloc& __a,
                       typename enable_if<uses_allocator<container_type,
                                                         _Alloc>::value>::type* = 0)
            : c(__a) {}
    template <class _Alloc>
        _LIBCPP_INLINE_VISIBILITY
        stack(const container_type& __c, const _Alloc& __a,
              typename enable_if<uses_allocator<container_type,
                                                _Alloc>::value>::type* = 0)
            : c(__c, __a) {}
    template <class _Alloc>
        _LIBCPP_INLINE_VISIBILITY
        stack(const stack& __s, const _Alloc& __a,
              typename enable_if<uses_allocator<container_type,
                                                _Alloc>::value>::type* = 0)
            : c(__s.c, __a) {}
#ifndef _LIBCPP_CXX03_LANG
    template <class _Alloc>
        _LIBCPP_INLINE_VISIBILITY
        stack(container_type&& __c, const _Alloc& __a,
              typename enable_if<uses_allocator<container_type,
                                                _Alloc>::value>::type* = 0)
            : c(_VSTD::move(__c), __a) {}
    template <class _Alloc>
        _LIBCPP_INLINE_VISIBILITY
        stack(stack&& __s, const _Alloc& __a,
              typename enable_if<uses_allocator<container_type,
                                                _Alloc>::value>::type* = 0)
            : c(_VSTD::move(__s.c), __a) {}
#endif  // _LIBCPP_CXX03_LANG

    _LIBCPP_INLINE_VISIBILITY
    bool empty()     const      {return c.empty();}
    _LIBCPP_INLINE_VISIBILITY
    size_type size() const      {return c.size();}
    _LIBCPP_INLINE_VISIBILITY
    reference top()             {return c.back();}
    _LIBCPP_INLINE_VISIBILITY
    const_reference top() const {return c.back();}

    _LIBCPP_INLINE_VISIBILITY
    void push(const value_type& __v) {c.push_back(__v);}
#ifndef _LIBCPP_CXX03_LANG
    _LIBCPP_INLINE_VISIBILITY
    void push(value_type&& __v) {c.push_back(_VSTD::move(__v));}

    template <class... _Args>
        _LIBCPP_INLINE_VISIBILITY
#if _LIBCPP_STD_VER > 14
        reference emplace(_Args&&... __args)
        { return c.emplace_back(_VSTD::forward<_Args>(__args)...);}
#else
        void      emplace(_Args&&... __args)
        {        c.emplace_back(_VSTD::forward<_Args>(__args)...);}
#endif
#endif  // _LIBCPP_CXX03_LANG

    _LIBCPP_INLINE_VISIBILITY
    void pop() {c.pop_back();}

    _LIBCPP_INLINE_VISIBILITY
    void swap(stack& __s)
        _NOEXCEPT_(__is_nothrow_swappable<container_type>::value)
    {
        using _VSTD::swap;
        swap(c, __s.c);
    }

    template <class T1, class _C1>
    friend
    bool
    operator==(const stack<T1, _C1>& __x, const stack<T1, _C1>& __y);

    template <class T1, class _C1>
    friend
    bool
    operator< (const stack<T1, _C1>& __x, const stack<T1, _C1>& __y);
};

template <class _Tp, class _Container>
inline _LIBCPP_INLINE_VISIBILITY
bool
operator==(const stack<_Tp, _Container>& __x, const stack<_Tp, _Container>& __y)
{
    return __x.c == __y.c;
}

template <class _Tp, class _Container>
inline _LIBCPP_INLINE_VISIBILITY
bool
operator< (const stack<_Tp, _Container>& __x, const stack<_Tp, _Container>& __y)
{
    return __x.c < __y.c;
}

template <class _Tp, class _Container>
inline _LIBCPP_INLINE_VISIBILITY
bool
operator!=(const stack<_Tp, _Container>& __x, const stack<_Tp, _Container>& __y)
{
    return !(__x == __y);
}

template <class _Tp, class _Container>
inline _LIBCPP_INLINE_VISIBILITY
bool
operator> (const stack<_Tp, _Container>& __x, const stack<_Tp, _Container>& __y)
{
    return __y < __x;
}

template <class _Tp, class _Container>
inline _LIBCPP_INLINE_VISIBILITY
bool
operator>=(const stack<_Tp, _Container>& __x, const stack<_Tp, _Container>& __y)
{
    return !(__x < __y);
}

template <class _Tp, class _Container>
inline _LIBCPP_INLINE_VISIBILITY
bool
operator<=(const stack<_Tp, _Container>& __x, const stack<_Tp, _Container>& __y)
{
    return !(__y < __x);
}

template <class _Tp, class _Container>
inline _LIBCPP_INLINE_VISIBILITY
typename enable_if<
    __is_swappable<_Container>::value,
    void
>::type
swap(stack<_Tp, _Container>& __x, stack<_Tp, _Container>& __y)
    _NOEXCEPT_(_NOEXCEPT_(__x.swap(__y)))
{
    __x.swap(__y);
}

template <class _Tp, class _Container, class _Alloc>
struct _LIBCPP_TEMPLATE_VIS uses_allocator<stack<_Tp, _Container>, _Alloc>
    : public uses_allocator<_Container, _Alloc>
{
};

_LIBCPP_END_NAMESPACE_STD

#endif  // _LIBCPP_STACK
