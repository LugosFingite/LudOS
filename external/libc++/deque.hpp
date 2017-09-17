// -*- C++ -*-
//===---------------------------- deque -----------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is dual licensed under the MIT and the University of Illinois Open
// Source Licenses. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef _LIBCPP_DEQUE
#define _LIBCPP_DEQUE

/*
    deque synopsis

namespace std
{

template <class T, class Allocator = allocator<T> >
class deque
{
public:
    // types:
    typedef T value_type;
    typedef Allocator allocator_type;

    typedef typename allocator_type::reference       reference;
    typedef typename allocator_type::const_reference const_reference;
    typedef implementation-defined                   iterator;
    typedef implementation-defined                   const_iterator;
    typedef typename allocator_type::size_type       size_type;
    typedef typename allocator_type::difference_type difference_type;

    typedef typename allocator_type::pointer         pointer;
    typedef typename allocator_type::const_pointer   const_pointer;
    typedef std::reverse_iterator<iterator>          reverse_iterator;
    typedef std::reverse_iterator<const_iterator>    const_reverse_iterator;

    // construct/copy/destroy:
    deque() noexcept(is_nothrow_default_constructible<allocator_type>::value);
    explicit deque(const allocator_type& a);
    explicit deque(size_type n);
    explicit deque(size_type n, const allocator_type& a); // C++14
    deque(size_type n, const value_type& v);
    deque(size_type n, const value_type& v, const allocator_type& a);
    template <class InputIterator>
        deque(InputIterator f, InputIterator l);
    template <class InputIterator>
        deque(InputIterator f, InputIterator l, const allocator_type& a);
    deque(const deque& c);
    deque(deque&& c)
        noexcept(is_nothrow_move_constructible<allocator_type>::value);
    deque(initializer_list<value_type> il, const Allocator& a = allocator_type());
    deque(const deque& c, const allocator_type& a);
    deque(deque&& c, const allocator_type& a);
    ~deque();

    deque& operator=(const deque& c);
    deque& operator=(deque&& c)
        noexcept(
             allocator_type::propagate_on_container_move_assignment::value &&
             is_nothrow_move_assignable<allocator_type>::value);
    deque& operator=(initializer_list<value_type> il);

    template <class InputIterator>
        void assign(InputIterator f, InputIterator l);
    void assign(size_type n, const value_type& v);
    void assign(initializer_list<value_type> il);

    allocator_type get_allocator() const noexcept;

    // iterators:

    iterator       begin() noexcept;
    const_iterator begin() const noexcept;
    iterator       end() noexcept;
    const_iterator end() const noexcept;

    reverse_iterator       rbegin() noexcept;
    const_reverse_iterator rbegin() const noexcept;
    reverse_iterator       rend() noexcept;
    const_reverse_iterator rend() const noexcept;

    const_iterator         cbegin() const noexcept;
    const_iterator         cend() const noexcept;
    const_reverse_iterator crbegin() const noexcept;
    const_reverse_iterator crend() const noexcept;

    // capacity:
    size_type size() const noexcept;
    size_type max_size() const noexcept;
    void resize(size_type n);
    void resize(size_type n, const value_type& v);
    void shrink_to_fit();
    bool empty() const noexcept;

    // element access:
    reference operator[](size_type i);
    const_reference operator[](size_type i) const;
    reference at(size_type i);
    const_reference at(size_type i) const;
    reference front();
    const_reference front() const;
    reference back();
    const_reference back() const;

    // modifiers:
    void push_front(const value_type& v);
    void push_front(value_type&& v);
    void push_back(const value_type& v);
    void push_back(value_type&& v);
    template <class... Args> reference emplace_front(Args&&... args);  // reference in C++17
    template <class... Args> reference emplace_back(Args&&... args);   // reference in C++17
    template <class... Args> iterator emplace(const_iterator p, Args&&... args);
    iterator insert(const_iterator p, const value_type& v);
    iterator insert(const_iterator p, value_type&& v);
    iterator insert(const_iterator p, size_type n, const value_type& v);
    template <class InputIterator>
        iterator insert(const_iterator p, InputIterator f, InputIterator l);
    iterator insert(const_iterator p, initializer_list<value_type> il);
    void pop_front();
    void pop_back();
    iterator erase(const_iterator p);
    iterator erase(const_iterator f, const_iterator l);
    void swap(deque& c)
        noexcept(allocator_traits<allocator_type>::is_always_equal::value);  // C++17
    void clear() noexcept;
};

template <class T, class Allocator>
    bool operator==(const deque<T,Allocator>& x, const deque<T,Allocator>& y);
template <class T, class Allocator>
    bool operator< (const deque<T,Allocator>& x, const deque<T,Allocator>& y);
template <class T, class Allocator>
    bool operator!=(const deque<T,Allocator>& x, const deque<T,Allocator>& y);
template <class T, class Allocator>
    bool operator> (const deque<T,Allocator>& x, const deque<T,Allocator>& y);
template <class T, class Allocator>
    bool operator>=(const deque<T,Allocator>& x, const deque<T,Allocator>& y);
template <class T, class Allocator>
    bool operator<=(const deque<T,Allocator>& x, const deque<T,Allocator>& y);

// specialized algorithms:
template <class T, class Allocator>
    void swap(deque<T,Allocator>& x, deque<T,Allocator>& y)
         noexcept(noexcept(x.swap(y)));

}  // std

*/

#include "__config.hpp"
#include "__split_buffer.hpp"
#include "type_traits.hpp"
#include "initializer_list.hpp"
#include "iterator.hpp"
#include "algorithm.hpp"
#include "stdexcept.hpp"

#if !defined(_LIBCPP_HAS_NO_PRAGMA_SYSTEM_HEADER)
#pragma GCC system_header
#endif

_LIBCPP_PUSH_MACROS
#include "__undef_macros.hpp"


_LIBCPP_BEGIN_NAMESPACE_STD

template <class _Tp, class _Allocator> class __deque_base;
template <class _Tp, class _Allocator = allocator<_Tp> > class _LIBCPP_TEMPLATE_VIS deque;

template <class _ValueType, class _Pointer, class _Reference, class _MapPointer,
          class _DiffType, _DiffType _BlockSize>
class _LIBCPP_TEMPLATE_VIS __deque_iterator;

template <class _RAIter,
          class _V2, class _P2, class _R2, class _M2, class _D2, _D2 _B2>
__deque_iterator<_V2, _P2, _R2, _M2, _D2, _B2>
copy(_RAIter __f,
     _RAIter __l,
     __deque_iterator<_V2, _P2, _R2, _M2, _D2, _B2> __r,
     typename enable_if<__is_random_access_iterator<_RAIter>::value>::type* = 0);

template <class _V1, class _P1, class _R1, class _M1, class _D1, _D1 _B1,
          class _OutputIterator>
_OutputIterator
copy(__deque_iterator<_V1, _P1, _R1, _M1, _D1, _B1> __f,
     __deque_iterator<_V1, _P1, _R1, _M1, _D1, _B1> __l,
     _OutputIterator __r);

template <class _V1, class _P1, class _R1, class _M1, class _D1, _D1 _B1,
          class _V2, class _P2, class _R2, class _M2, class _D2, _D2 _B2>
__deque_iterator<_V2, _P2, _R2, _M2, _D2, _B2>
copy(__deque_iterator<_V1, _P1, _R1, _M1, _D1, _B1> __f,
     __deque_iterator<_V1, _P1, _R1, _M1, _D1, _B1> __l,
     __deque_iterator<_V2, _P2, _R2, _M2, _D2, _B2> __r);

template <class _RAIter,
          class _V2, class _P2, class _R2, class _M2, class _D2, _D2 _B2>
__deque_iterator<_V2, _P2, _R2, _M2, _D2, _B2>
copy_backward(_RAIter __f,
              _RAIter __l,
              __deque_iterator<_V2, _P2, _R2, _M2, _D2, _B2> __r,
              typename enable_if<__is_random_access_iterator<_RAIter>::value>::type* = 0);

template <class _V1, class _P1, class _R1, class _M1, class _D1, _D1 _B1,
          class _OutputIterator>
_OutputIterator
copy_backward(__deque_iterator<_V1, _P1, _R1, _M1, _D1, _B1> __f,
              __deque_iterator<_V1, _P1, _R1, _M1, _D1, _B1> __l,
              _OutputIterator __r);

template <class _V1, class _P1, class _R1, class _M1, class _D1, _D1 _B1,
          class _V2, class _P2, class _R2, class _M2, class _D2, _D2 _B2>
__deque_iterator<_V2, _P2, _R2, _M2, _D2, _B2>
copy_backward(__deque_iterator<_V1, _P1, _R1, _M1, _D1, _B1> __f,
              __deque_iterator<_V1, _P1, _R1, _M1, _D1, _B1> __l,
              __deque_iterator<_V2, _P2, _R2, _M2, _D2, _B2> __r);

template <class _RAIter,
          class _V2, class _P2, class _R2, class _M2, class _D2, _D2 _B2>
__deque_iterator<_V2, _P2, _R2, _M2, _D2, _B2>
move(_RAIter __f,
     _RAIter __l,
     __deque_iterator<_V2, _P2, _R2, _M2, _D2, _B2> __r,
     typename enable_if<__is_random_access_iterator<_RAIter>::value>::type* = 0);

template <class _V1, class _P1, class _R1, class _M1, class _D1, _D1 _B1,
          class _OutputIterator>
_OutputIterator
move(__deque_iterator<_V1, _P1, _R1, _M1, _D1, _B1> __f,
     __deque_iterator<_V1, _P1, _R1, _M1, _D1, _B1> __l,
     _OutputIterator __r);

template <class _V1, class _P1, class _R1, class _M1, class _D1, _D1 _B1,
          class _V2, class _P2, class _R2, class _M2, class _D2, _D2 _B2>
__deque_iterator<_V2, _P2, _R2, _M2, _D2, _B2>
move(__deque_iterator<_V1, _P1, _R1, _M1, _D1, _B1> __f,
     __deque_iterator<_V1, _P1, _R1, _M1, _D1, _B1> __l,
     __deque_iterator<_V2, _P2, _R2, _M2, _D2, _B2> __r);

template <class _RAIter,
          class _V2, class _P2, class _R2, class _M2, class _D2, _D2 _B2>
__deque_iterator<_V2, _P2, _R2, _M2, _D2, _B2>
move_backward(_RAIter __f,
              _RAIter __l,
              __deque_iterator<_V2, _P2, _R2, _M2, _D2, _B2> __r,
              typename enable_if<__is_random_access_iterator<_RAIter>::value>::type* = 0);

template <class _V1, class _P1, class _R1, class _M1, class _D1, _D1 _B1,
          class _OutputIterator>
_OutputIterator
move_backward(__deque_iterator<_V1, _P1, _R1, _M1, _D1, _B1> __f,
              __deque_iterator<_V1, _P1, _R1, _M1, _D1, _B1> __l,
              _OutputIterator __r);

template <class _V1, class _P1, class _R1, class _M1, class _D1, _D1 _B1,
          class _V2, class _P2, class _R2, class _M2, class _D2, _D2 _B2>
__deque_iterator<_V2, _P2, _R2, _M2, _D2, _B2>
move_backward(__deque_iterator<_V1, _P1, _R1, _M1, _D1, _B1> __f,
              __deque_iterator<_V1, _P1, _R1, _M1, _D1, _B1> __l,
              __deque_iterator<_V2, _P2, _R2, _M2, _D2, _B2> __r);

template <class _ValueType, class _DiffType>
struct __deque_block_size {
  static const _DiffType value = sizeof(_ValueType) < 256 ? 4096 / sizeof(_ValueType) : 16;
};

template <class _ValueType, class _Pointer, class _Reference, class _MapPointer,
          class _DiffType, _DiffType _BS =
#ifdef _LIBCPP_ABI_INCOMPLETE_TYPES_IN_DEQUE
// Keep template parameter to avoid changing all template declarations thoughout
// this file.
                               0
#else
                               __deque_block_size<_ValueType, _DiffType>::value
#endif
          >
class _LIBCPP_TEMPLATE_VIS __deque_iterator
{
    typedef _MapPointer __map_iterator;
public:
    typedef _Pointer  pointer;
    typedef _DiffType difference_type;
private:
    __map_iterator __m_iter_;
    pointer        __ptr_;

    static const difference_type __block_size;
public:
    typedef _ValueType                  value_type;
    typedef random_access_iterator_tag  iterator_category;
    typedef _Reference                  reference;

    _LIBCPP_INLINE_VISIBILITY __deque_iterator() _NOEXCEPT
#if _LIBCPP_STD_VER > 11
     : __m_iter_(nullptr), __ptr_(nullptr)
#endif
     {}

    template <class _Pp, class _Rp, class _MP>
    _LIBCPP_INLINE_VISIBILITY
    __deque_iterator(const __deque_iterator<value_type, _Pp, _Rp, _MP, difference_type, _BS>& __it,
                typename enable_if<is_convertible<_Pp, pointer>::value>::type* = 0) _NOEXCEPT
        : __m_iter_(__it.__m_iter_), __ptr_(__it.__ptr_) {}

    _LIBCPP_INLINE_VISIBILITY reference operator*() const {return *__ptr_;}
    _LIBCPP_INLINE_VISIBILITY pointer operator->() const {return __ptr_;}

    _LIBCPP_INLINE_VISIBILITY __deque_iterator& operator++()
    {
        if (++__ptr_ - *__m_iter_ == __block_size)
        {
            ++__m_iter_;
            __ptr_ = *__m_iter_;
        }
        return *this;
    }

    _LIBCPP_INLINE_VISIBILITY __deque_iterator operator++(int)
    {
        __deque_iterator __tmp = *this;
        ++(*this);
        return __tmp;
    }

    _LIBCPP_INLINE_VISIBILITY __deque_iterator& operator--()
    {
        if (__ptr_ == *__m_iter_)
        {
            --__m_iter_;
            __ptr_ = *__m_iter_ + __block_size;
        }
        --__ptr_;
        return *this;
    }

    _LIBCPP_INLINE_VISIBILITY __deque_iterator operator--(int)
    {
        __deque_iterator __tmp = *this;
        --(*this);
        return __tmp;
    }

    _LIBCPP_INLINE_VISIBILITY __deque_iterator& operator+=(difference_type __n)
    {
        if (__n != 0)
        {
            __n += __ptr_ - *__m_iter_;
            if (__n > 0)
            {
                __m_iter_ += __n / __block_size;
                __ptr_ = *__m_iter_ + __n % __block_size;
            }
            else // (__n < 0)
            {
                difference_type __z = __block_size - 1 - __n;
                __m_iter_ -= __z / __block_size;
                __ptr_ = *__m_iter_ + (__block_size - 1 - __z % __block_size);
            }
        }
        return *this;
    }

    _LIBCPP_INLINE_VISIBILITY __deque_iterator& operator-=(difference_type __n)
    {
        return *this += -__n;
    }

    _LIBCPP_INLINE_VISIBILITY __deque_iterator operator+(difference_type __n) const
    {
        __deque_iterator __t(*this);
        __t += __n;
        return __t;
    }

    _LIBCPP_INLINE_VISIBILITY __deque_iterator operator-(difference_type __n) const
    {
        __deque_iterator __t(*this);
        __t -= __n;
        return __t;
    }

    _LIBCPP_INLINE_VISIBILITY
    friend __deque_iterator operator+(difference_type __n, const __deque_iterator& __it)
        {return __it + __n;}

    _LIBCPP_INLINE_VISIBILITY
    friend difference_type operator-(const __deque_iterator& __x, const __deque_iterator& __y)
    {
        if (__x != __y)
            return (__x.__m_iter_ - __y.__m_iter_) * __block_size
                 + (__x.__ptr_ - *__x.__m_iter_)
                 - (__y.__ptr_ - *__y.__m_iter_);
        return 0;
    }

    _LIBCPP_INLINE_VISIBILITY reference operator[](difference_type __n) const
        {return *(*this + __n);}

    _LIBCPP_INLINE_VISIBILITY friend
        bool operator==(const __deque_iterator& __x, const __deque_iterator& __y)
        {return __x.__ptr_ == __y.__ptr_;}

    _LIBCPP_INLINE_VISIBILITY friend
        bool operator!=(const __deque_iterator& __x, const __deque_iterator& __y)
        {return !(__x == __y);}

    _LIBCPP_INLINE_VISIBILITY friend
        bool operator<(const __deque_iterator& __x, const __deque_iterator& __y)
        {return __x.__m_iter_ < __y.__m_iter_ ||
               (__x.__m_iter_ == __y.__m_iter_ && __x.__ptr_ < __y.__ptr_);}

    _LIBCPP_INLINE_VISIBILITY friend
        bool operator>(const __deque_iterator& __x, const __deque_iterator& __y)
        {return __y < __x;}

    _LIBCPP_INLINE_VISIBILITY friend
        bool operator<=(const __deque_iterator& __x, const __deque_iterator& __y)
        {return !(__y < __x);}

    _LIBCPP_INLINE_VISIBILITY friend
        bool operator>=(const __deque_iterator& __x, const __deque_iterator& __y)
        {return !(__x < __y);}

private:
    _LIBCPP_INLINE_VISIBILITY __deque_iterator(__map_iterator __m, pointer __p) _NOEXCEPT
        : __m_iter_(__m), __ptr_(__p) {}

    template <class _Tp, class _Ap> friend class __deque_base;
    template <class _Tp, class _Ap> friend class _LIBCPP_TEMPLATE_VIS deque;
    template <class _Vp, class _Pp, class _Rp, class _MP, class _Dp, _Dp>
        friend class _LIBCPP_TEMPLATE_VIS __deque_iterator;

    template <class _RAIter,
              class _V2, class _P2, class _R2, class _M2, class _D2, _D2 _B2>
    friend
    __deque_iterator<_V2, _P2, _R2, _M2, _D2, _B2>
    copy(_RAIter __f,
         _RAIter __l,
         __deque_iterator<_V2, _P2, _R2, _M2, _D2, _B2> __r,
         typename enable_if<__is_random_access_iterator<_RAIter>::value>::type*);

    template <class _V1, class _P1, class _R1, class _M1, class _D1, _D1 _B1,
              class _OutputIterator>
    friend
    _OutputIterator
    copy(__deque_iterator<_V1, _P1, _R1, _M1, _D1, _B1> __f,
         __deque_iterator<_V1, _P1, _R1, _M1, _D1, _B1> __l,
         _OutputIterator __r);

    template <class _V1, class _P1, class _R1, class _M1, class _D1, _D1 _B1,
              class _V2, class _P2, class _R2, class _M2, class _D2, _D2 _B2>
    friend
    __deque_iterator<_V2, _P2, _R2, _M2, _D2, _B2>
    copy(__deque_iterator<_V1, _P1, _R1, _M1, _D1, _B1> __f,
         __deque_iterator<_V1, _P1, _R1, _M1, _D1, _B1> __l,
         __deque_iterator<_V2, _P2, _R2, _M2, _D2, _B2> __r);

    template <class _RAIter,
              class _V2, class _P2, class _R2, class _M2, class _D2, _D2 _B2>
    friend
    __deque_iterator<_V2, _P2, _R2, _M2, _D2, _B2>
    copy_backward(_RAIter __f,
                  _RAIter __l,
                  __deque_iterator<_V2, _P2, _R2, _M2, _D2, _B2> __r,
                  typename enable_if<__is_random_access_iterator<_RAIter>::value>::type*);

    template <class _V1, class _P1, class _R1, class _M1, class _D1, _D1 _B1,
              class _OutputIterator>
    friend
    _OutputIterator
    copy_backward(__deque_iterator<_V1, _P1, _R1, _M1, _D1, _B1> __f,
                  __deque_iterator<_V1, _P1, _R1, _M1, _D1, _B1> __l,
                  _OutputIterator __r);

    template <class _V1, class _P1, class _R1, class _M1, class _D1, _D1 _B1,
              class _V2, class _P2, class _R2, class _M2, class _D2, _D2 _B2>
    friend
    __deque_iterator<_V2, _P2, _R2, _M2, _D2, _B2>
    copy_backward(__deque_iterator<_V1, _P1, _R1, _M1, _D1, _B1> __f,
                  __deque_iterator<_V1, _P1, _R1, _M1, _D1, _B1> __l,
                  __deque_iterator<_V2, _P2, _R2, _M2, _D2, _B2> __r);

    template <class _RAIter,
              class _V2, class _P2, class _R2, class _M2, class _D2, _D2 _B2>
    friend
    __deque_iterator<_V2, _P2, _R2, _M2, _D2, _B2>
    move(_RAIter __f,
         _RAIter __l,
         __deque_iterator<_V2, _P2, _R2, _M2, _D2, _B2> __r,
         typename enable_if<__is_random_access_iterator<_RAIter>::value>::type*);

    template <class _V1, class _P1, class _R1, class _M1, class _D1, _D1 _B1,
              class _OutputIterator>
    friend
    _OutputIterator
    move(__deque_iterator<_V1, _P1, _R1, _M1, _D1, _B1> __f,
         __deque_iterator<_V1, _P1, _R1, _M1, _D1, _B1> __l,
         _OutputIterator __r);

    template <class _V1, class _P1, class _R1, class _M1, class _D1, _D1 _B1,
              class _V2, class _P2, class _R2, class _M2, class _D2, _D2 _B2>
    friend
    __deque_iterator<_V2, _P2, _R2, _M2, _D2, _B2>
    move(__deque_iterator<_V1, _P1, _R1, _M1, _D1, _B1> __f,
         __deque_iterator<_V1, _P1, _R1, _M1, _D1, _B1> __l,
         __deque_iterator<_V2, _P2, _R2, _M2, _D2, _B2> __r);

    template <class _RAIter,
              class _V2, class _P2, class _R2, class _M2, class _D2, _D2 _B2>
    friend
    __deque_iterator<_V2, _P2, _R2, _M2, _D2, _B2>
    move_backward(_RAIter __f,
                  _RAIter __l,
                  __deque_iterator<_V2, _P2, _R2, _M2, _D2, _B2> __r,
                  typename enable_if<__is_random_access_iterator<_RAIter>::value>::type*);

    template <class _V1, class _P1, class _R1, class _M1, class _D1, _D1 _B1,
              class _OutputIterator>
    friend
    _OutputIterator
    move_backward(__deque_iterator<_V1, _P1, _R1, _M1, _D1, _B1> __f,
                  __deque_iterator<_V1, _P1, _R1, _M1, _D1, _B1> __l,
                  _OutputIterator __r);

    template <class _V1, class _P1, class _R1, class _M1, class _D1, _D1 _B1,
              class _V2, class _P2, class _R2, class _M2, class _D2, _D2 _B2>
    friend
    __deque_iterator<_V2, _P2, _R2, _M2, _D2, _B2>
    move_backward(__deque_iterator<_V1, _P1, _R1, _M1, _D1, _B1> __f,
                  __deque_iterator<_V1, _P1, _R1, _M1, _D1, _B1> __l,
                  __deque_iterator<_V2, _P2, _R2, _M2, _D2, _B2> __r);
};

template <class _ValueType, class _Pointer, class _Reference, class _MapPointer,
          class _DiffType, _DiffType _BlockSize>
const _DiffType __deque_iterator<_ValueType, _Pointer, _Reference, _MapPointer,
                                 _DiffType, _BlockSize>::__block_size =
    __deque_block_size<_ValueType, _DiffType>::value;

// copy

template <class _RAIter,
          class _V2, class _P2, class _R2, class _M2, class _D2, _D2 _B2>
__deque_iterator<_V2, _P2, _R2, _M2, _D2, _B2>
copy(_RAIter __f,
     _RAIter __l,
     __deque_iterator<_V2, _P2, _R2, _M2, _D2, _B2> __r,
     typename enable_if<__is_random_access_iterator<_RAIter>::value>::type*)
{
    typedef typename __deque_iterator<_V2, _P2, _R2, _M2, _D2, _B2>::difference_type difference_type;
    typedef typename __deque_iterator<_V2, _P2, _R2, _M2, _D2, _B2>::pointer pointer;
    const difference_type __block_size = __deque_iterator<_V2, _P2, _R2, _M2, _D2, _B2>::__block_size;
    while (__f != __l)
    {
        pointer __rb = __r.__ptr_;
        pointer __re = *__r.__m_iter_ + __block_size;
        difference_type __bs = __re - __rb;
        difference_type __n = __l - __f;
        _RAIter __m = __l;
        if (__n > __bs)
        {
            __n = __bs;
            __m = __f + __n;
        }
        _VSTD::copy(__f, __m, __rb);
        __f = __m;
        __r += __n;
    }
    return __r;
}

template <class _V1, class _P1, class _R1, class _M1, class _D1, _D1 _B1,
          class _OutputIterator>
_OutputIterator
copy(__deque_iterator<_V1, _P1, _R1, _M1, _D1, _B1> __f,
     __deque_iterator<_V1, _P1, _R1, _M1, _D1, _B1> __l,
     _OutputIterator __r)
{
    typedef typename __deque_iterator<_V1, _P1, _R1, _M1, _D1, _B1>::difference_type difference_type;
    typedef typename __deque_iterator<_V1, _P1, _R1, _M1, _D1, _B1>::pointer pointer;
    const difference_type __block_size = __deque_iterator<_V1, _P1, _R1, _M1, _D1, _B1>::__block_size;
    difference_type __n = __l - __f;
    while (__n > 0)
    {
        pointer __fb = __f.__ptr_;
        pointer __fe = *__f.__m_iter_ + __block_size;
        difference_type __bs = __fe - __fb;
        if (__bs > __n)
        {
            __bs = __n;
            __fe = __fb + __bs;
        }
        __r = _VSTD::copy(__fb, __fe, __r);
        __n -= __bs;
        __f += __bs;
    }
    return __r;
}

template <class _V1, class _P1, class _R1, class _M1, class _D1, _D1 _B1,
          class _V2, class _P2, class _R2, class _M2, class _D2, _D2 _B2>
__deque_iterator<_V2, _P2, _R2, _M2, _D2, _B2>
copy(__deque_iterator<_V1, _P1, _R1, _M1, _D1, _B1> __f,
     __deque_iterator<_V1, _P1, _R1, _M1, _D1, _B1> __l,
     __deque_iterator<_V2, _P2, _R2, _M2, _D2, _B2> __r)
{
    typedef typename __deque_iterator<_V1, _P1, _R1, _M1, _D1, _B1>::difference_type difference_type;
    typedef typename __deque_iterator<_V1, _P1, _R1, _M1, _D1, _B1>::pointer pointer;
    const difference_type __block_size = __deque_iterator<_V1, _P1, _R1, _M1, _D1, _B1>::__block_size;
    difference_type __n = __l - __f;
    while (__n > 0)
    {
        pointer __fb = __f.__ptr_;
        pointer __fe = *__f.__m_iter_ + __block_size;
        difference_type __bs = __fe - __fb;
        if (__bs > __n)
        {
            __bs = __n;
            __fe = __fb + __bs;
        }
        __r = _VSTD::copy(__fb, __fe, __r);
        __n -= __bs;
        __f += __bs;
    }
    return __r;
}

// copy_backward

template <class _RAIter,
          class _V2, class _P2, class _R2, class _M2, class _D2, _D2 _B2>
__deque_iterator<_V2, _P2, _R2, _M2, _D2, _B2>
copy_backward(_RAIter __f,
              _RAIter __l,
              __deque_iterator<_V2, _P2, _R2, _M2, _D2, _B2> __r,
              typename enable_if<__is_random_access_iterator<_RAIter>::value>::type*)
{
    typedef typename __deque_iterator<_V2, _P2, _R2, _M2, _D2, _B2>::difference_type difference_type;
    typedef typename __deque_iterator<_V2, _P2, _R2, _M2, _D2, _B2>::pointer pointer;
    while (__f != __l)
    {
        __deque_iterator<_V2, _P2, _R2, _M2, _D2, _B2> __rp = _VSTD::prev(__r);
        pointer __rb = *__rp.__m_iter_;
        pointer __re = __rp.__ptr_ + 1;
        difference_type __bs = __re - __rb;
        difference_type __n = __l - __f;
        _RAIter __m = __f;
        if (__n > __bs)
        {
            __n = __bs;
            __m = __l - __n;
        }
        _VSTD::copy_backward(__m, __l, __re);
        __l = __m;
        __r -= __n;
    }
    return __r;
}

template <class _V1, class _P1, class _R1, class _M1, class _D1, _D1 _B1,
          class _OutputIterator>
_OutputIterator
copy_backward(__deque_iterator<_V1, _P1, _R1, _M1, _D1, _B1> __f,
              __deque_iterator<_V1, _P1, _R1, _M1, _D1, _B1> __l,
              _OutputIterator __r)
{
    typedef typename __deque_iterator<_V1, _P1, _R1, _M1, _D1, _B1>::difference_type difference_type;
    typedef typename __deque_iterator<_V1, _P1, _R1, _M1, _D1, _B1>::pointer pointer;
    difference_type __n = __l - __f;
    while (__n > 0)
    {
        --__l;
        pointer __lb = *__l.__m_iter_;
        pointer __le = __l.__ptr_ + 1;
        difference_type __bs = __le - __lb;
        if (__bs > __n)
        {
            __bs = __n;
            __lb = __le - __bs;
        }
        __r = _VSTD::copy_backward(__lb, __le, __r);
        __n -= __bs;
        __l -= __bs - 1;
    }
    return __r;
}

template <class _V1, class _P1, class _R1, class _M1, class _D1, _D1 _B1,
          class _V2, class _P2, class _R2, class _M2, class _D2, _D2 _B2>
__deque_iterator<_V2, _P2, _R2, _M2, _D2, _B2>
copy_backward(__deque_iterator<_V1, _P1, _R1, _M1, _D1, _B1> __f,
              __deque_iterator<_V1, _P1, _R1, _M1, _D1, _B1> __l,
              __deque_iterator<_V2, _P2, _R2, _M2, _D2, _B2> __r)
{
    typedef typename __deque_iterator<_V1, _P1, _R1, _M1, _D1, _B1>::difference_type difference_type;
    typedef typename __deque_iterator<_V1, _P1, _R1, _M1, _D1, _B1>::pointer pointer;
    difference_type __n = __l - __f;
    while (__n > 0)
    {
        --__l;
        pointer __lb = *__l.__m_iter_;
        pointer __le = __l.__ptr_ + 1;
        difference_type __bs = __le - __lb;
        if (__bs > __n)
        {
            __bs = __n;
            __lb = __le - __bs;
        }
        __r = _VSTD::copy_backward(__lb, __le, __r);
        __n -= __bs;
        __l -= __bs - 1;
    }
    return __r;
}

// move

template <class _RAIter,
          class _V2, class _P2, class _R2, class _M2, class _D2, _D2 _B2>
__deque_iterator<_V2, _P2, _R2, _M2, _D2, _B2>
move(_RAIter __f,
     _RAIter __l,
     __deque_iterator<_V2, _P2, _R2, _M2, _D2, _B2> __r,
     typename enable_if<__is_random_access_iterator<_RAIter>::value>::type*)
{
    typedef typename __deque_iterator<_V2, _P2, _R2, _M2, _D2, _B2>::difference_type difference_type;
    typedef typename __deque_iterator<_V2, _P2, _R2, _M2, _D2, _B2>::pointer pointer;
    const difference_type __block_size = __deque_iterator<_V2, _P2, _R2, _M2, _D2, _B2>::__block_size;
    while (__f != __l)
    {
        pointer __rb = __r.__ptr_;
        pointer __re = *__r.__m_iter_ + __block_size;
        difference_type __bs = __re - __rb;
        difference_type __n = __l - __f;
        _RAIter __m = __l;
        if (__n > __bs)
        {
            __n = __bs;
            __m = __f + __n;
        }
        _VSTD::move(__f, __m, __rb);
        __f = __m;
        __r += __n;
    }
    return __r;
}

template <class _V1, class _P1, class _R1, class _M1, class _D1, _D1 _B1,
          class _OutputIterator>
_OutputIterator
move(__deque_iterator<_V1, _P1, _R1, _M1, _D1, _B1> __f,
     __deque_iterator<_V1, _P1, _R1, _M1, _D1, _B1> __l,
     _OutputIterator __r)
{
    typedef typename __deque_iterator<_V1, _P1, _R1, _M1, _D1, _B1>::difference_type difference_type;
    typedef typename __deque_iterator<_V1, _P1, _R1, _M1, _D1, _B1>::pointer pointer;
    const difference_type __block_size = __deque_iterator<_V1, _P1, _R1, _M1, _D1, _B1>::__block_size;
    difference_type __n = __l - __f;
    while (__n > 0)
    {
        pointer __fb = __f.__ptr_;
        pointer __fe = *__f.__m_iter_ + __block_size;
        difference_type __bs = __fe - __fb;
        if (__bs > __n)
        {
            __bs = __n;
            __fe = __fb + __bs;
        }
        __r = _VSTD::move(__fb, __fe, __r);
        __n -= __bs;
        __f += __bs;
    }
    return __r;
}

template <class _V1, class _P1, class _R1, class _M1, class _D1, _D1 _B1,
          class _V2, class _P2, class _R2, class _M2, class _D2, _D2 _B2>
__deque_iterator<_V2, _P2, _R2, _M2, _D2, _B2>
move(__deque_iterator<_V1, _P1, _R1, _M1, _D1, _B1> __f,
     __deque_iterator<_V1, _P1, _R1, _M1, _D1, _B1> __l,
     __deque_iterator<_V2, _P2, _R2, _M2, _D2, _B2> __r)
{
    typedef typename __deque_iterator<_V1, _P1, _R1, _M1, _D1, _B1>::difference_type difference_type;
    typedef typename __deque_iterator<_V1, _P1, _R1, _M1, _D1, _B1>::pointer pointer;
    const difference_type __block_size = __deque_iterator<_V1, _P1, _R1, _M1, _D1, _B1>::__block_size;
    difference_type __n = __l - __f;
    while (__n > 0)
    {
        pointer __fb = __f.__ptr_;
        pointer __fe = *__f.__m_iter_ + __block_size;
        difference_type __bs = __fe - __fb;
        if (__bs > __n)
        {
            __bs = __n;
            __fe = __fb + __bs;
        }
        __r = _VSTD::move(__fb, __fe, __r);
        __n -= __bs;
        __f += __bs;
    }
    return __r;
}

// move_backward

template <class _RAIter,
          class _V2, class _P2, class _R2, class _M2, class _D2, _D2 _B2>
__deque_iterator<_V2, _P2, _R2, _M2, _D2, _B2>
move_backward(_RAIter __f,
              _RAIter __l,
              __deque_iterator<_V2, _P2, _R2, _M2, _D2, _B2> __r,
              typename enable_if<__is_random_access_iterator<_RAIter>::value>::type*)
{
    typedef typename __deque_iterator<_V2, _P2, _R2, _M2, _D2, _B2>::difference_type difference_type;
    typedef typename __deque_iterator<_V2, _P2, _R2, _M2, _D2, _B2>::pointer pointer;
    while (__f != __l)
    {
        __deque_iterator<_V2, _P2, _R2, _M2, _D2, _B2> __rp = _VSTD::prev(__r);
        pointer __rb = *__rp.__m_iter_;
        pointer __re = __rp.__ptr_ + 1;
        difference_type __bs = __re - __rb;
        difference_type __n = __l - __f;
        _RAIter __m = __f;
        if (__n > __bs)
        {
            __n = __bs;
            __m = __l - __n;
        }
        _VSTD::move_backward(__m, __l, __re);
        __l = __m;
        __r -= __n;
    }
    return __r;
}

template <class _V1, class _P1, class _R1, class _M1, class _D1, _D1 _B1,
          class _OutputIterator>
_OutputIterator
move_backward(__deque_iterator<_V1, _P1, _R1, _M1, _D1, _B1> __f,
              __deque_iterator<_V1, _P1, _R1, _M1, _D1, _B1> __l,
              _OutputIterator __r)
{
    typedef typename __deque_iterator<_V1, _P1, _R1, _M1, _D1, _B1>::difference_type difference_type;
    typedef typename __deque_iterator<_V1, _P1, _R1, _M1, _D1, _B1>::pointer pointer;
    difference_type __n = __l - __f;
    while (__n > 0)
    {
        --__l;
        pointer __lb = *__l.__m_iter_;
        pointer __le = __l.__ptr_ + 1;
        difference_type __bs = __le - __lb;
        if (__bs > __n)
        {
            __bs = __n;
            __lb = __le - __bs;
        }
        __r = _VSTD::move_backward(__lb, __le, __r);
        __n -= __bs;
        __l -= __bs - 1;
    }
    return __r;
}

template <class _V1, class _P1, class _R1, class _M1, class _D1, _D1 _B1,
          class _V2, class _P2, class _R2, class _M2, class _D2, _D2 _B2>
__deque_iterator<_V2, _P2, _R2, _M2, _D2, _B2>
move_backward(__deque_iterator<_V1, _P1, _R1, _M1, _D1, _B1> __f,
              __deque_iterator<_V1, _P1, _R1, _M1, _D1, _B1> __l,
              __deque_iterator<_V2, _P2, _R2, _M2, _D2, _B2> __r)
{
    typedef typename __deque_iterator<_V1, _P1, _R1, _M1, _D1, _B1>::difference_type difference_type;
    typedef typename __deque_iterator<_V1, _P1, _R1, _M1, _D1, _B1>::pointer pointer;
    difference_type __n = __l - __f;
    while (__n > 0)
    {
        --__l;
        pointer __lb = *__l.__m_iter_;
        pointer __le = __l.__ptr_ + 1;
        difference_type __bs = __le - __lb;
        if (__bs > __n)
        {
            __bs = __n;
            __lb = __le - __bs;
        }
        __r = _VSTD::move_backward(__lb, __le, __r);
        __n -= __bs;
        __l -= __bs - 1;
    }
    return __r;
}

template <bool>
class __deque_base_common
{
protected:
    _LIBCPP_NORETURN void __throw_length_error() const;
    _LIBCPP_NORETURN void __throw_out_of_range() const;
};

template <bool __b>
void
__deque_base_common<__b>::__throw_length_error() const
{
    _VSTD::__throw_length_error("deque");
}

template <bool __b>
void
__deque_base_common<__b>::__throw_out_of_range() const
{
    _VSTD::__throw_out_of_range("deque");
}

template <class _Tp, class _Allocator>
class __deque_base
    : protected __deque_base_common<true>
{
    __deque_base(const __deque_base& __c);
    __deque_base& operator=(const __deque_base& __c);
protected:
    typedef _Tp                                      value_type;
    typedef _Allocator                               allocator_type;
    typedef allocator_traits<allocator_type>         __alloc_traits;
    typedef value_type&                              reference;
    typedef const value_type&                        const_reference;
    typedef typename __alloc_traits::size_type       size_type;
    typedef typename __alloc_traits::difference_type difference_type;
    typedef typename __alloc_traits::pointer         pointer;
    typedef typename __alloc_traits::const_pointer   const_pointer;

    static const difference_type __block_size;

    typedef typename __rebind_alloc_helper<__alloc_traits, pointer>::type __pointer_allocator;
    typedef allocator_traits<__pointer_allocator>        __map_traits;
    typedef typename __map_traits::pointer               __map_pointer;
    typedef typename __rebind_alloc_helper<__alloc_traits, const_pointer>::type __const_pointer_allocator;
    typedef typename allocator_traits<__const_pointer_allocator>::const_pointer __map_const_pointer;
    typedef __split_buffer<pointer, __pointer_allocator> __map;

    typedef __deque_iterator<value_type, pointer, reference, __map_pointer,
                             difference_type>    iterator;
    typedef __deque_iterator<value_type, const_pointer, const_reference, __map_const_pointer,
                             difference_type>    const_iterator;

    __map __map_;
    size_type __start_;
    __compressed_pair<size_type, allocator_type> __size_;

    iterator       begin() _NOEXCEPT;
    const_iterator begin() const _NOEXCEPT;
    iterator       end() _NOEXCEPT;
    const_iterator end() const _NOEXCEPT;

    _LIBCPP_INLINE_VISIBILITY size_type&            size()          {return __size_.first();}
    _LIBCPP_INLINE_VISIBILITY
    const size_type& size() const _NOEXCEPT {return __size_.first();}
    _LIBCPP_INLINE_VISIBILITY allocator_type&       __alloc()       {return __size_.second();}
    _LIBCPP_INLINE_VISIBILITY
    const allocator_type& __alloc() const _NOEXCEPT {return __size_.second();}

    _LIBCPP_INLINE_VISIBILITY
    __deque_base()
        _NOEXCEPT_(is_nothrow_default_constructible<allocator_type>::value);
    _LIBCPP_INLINE_VISIBILITY
    explicit __deque_base(const allocator_type& __a);
public:
    ~__deque_base();

#ifndef _LIBCPP_CXX03_LANG
    __deque_base(__deque_base&& __c)
        _NOEXCEPT_(is_nothrow_move_constructible<allocator_type>::value);
    __deque_base(__deque_base&& __c, const allocator_type& __a);
#endif  // _LIBCPP_CXX03_LANG

    void swap(__deque_base& __c)
#if _LIBCPP_STD_VER >= 14
        _NOEXCEPT;
#else
        _NOEXCEPT_(!__alloc_traits::propagate_on_container_swap::value ||
                    __is_nothrow_swappable<allocator_type>::value);
#endif
protected:
    void clear() _NOEXCEPT;

    bool __invariants() const;

    _LIBCPP_INLINE_VISIBILITY
    void __move_assign(__deque_base& __c)
        _NOEXCEPT_(__alloc_traits::propagate_on_container_move_assignment::value &&
                   is_nothrow_move_assignable<allocator_type>::value)
    {
        __map_ = _VSTD::move(__c.__map_);
        __start_ = __c.__start_;
        size() = __c.size();
        __move_assign_alloc(__c);
        __c.__start_ = __c.size() = 0;
    }

    _LIBCPP_INLINE_VISIBILITY
    void __move_assign_alloc(__deque_base& __c)
        _NOEXCEPT_(!__alloc_traits::propagate_on_container_move_assignment::value ||
                   is_nothrow_move_assignable<allocator_type>::value)
        {__move_assign_alloc(__c, integral_constant<bool,
                      __alloc_traits::propagate_on_container_move_assignment::value>());}

private:
    _LIBCPP_INLINE_VISIBILITY
    void __move_assign_alloc(__deque_base& __c, true_type)
        _NOEXCEPT_(is_nothrow_move_assignable<allocator_type>::value)
        {
            __alloc() = _VSTD::move(__c.__alloc());
        }

    _LIBCPP_INLINE_VISIBILITY
    void __move_assign_alloc(__deque_base&, false_type) _NOEXCEPT
        {}
};

template <class _Tp, class _Allocator>
const typename __deque_base<_Tp, _Allocator>::difference_type
    __deque_base<_Tp, _Allocator>::__block_size =
        __deque_block_size<value_type, difference_type>::value;

template <class _Tp, class _Allocator>
bool
__deque_base<_Tp, _Allocator>::__invariants() const
{
    if (!__map_.__invariants())
        return false;
    if (__map_.size() >= size_type(-1) / __block_size)
        return false;
    for (typename __map::const_iterator __i = __map_.begin(), __e = __map_.end();
         __i != __e; ++__i)
        if (*__i == nullptr)
            return false;
    if (__map_.size() != 0)
    {
        if (size() >= __map_.size() * __block_size)
            return false;
        if (__start_ >= __map_.size() * __block_size - size())
            return false;
    }
    else
    {
        if (size() != 0)
            return false;
        if (__start_ != 0)
            return false;
    }
    return true;
}

template <class _Tp, class _Allocator>
typename __deque_base<_Tp, _Allocator>::iterator
__deque_base<_Tp, _Allocator>::begin() _NOEXCEPT
{
    __map_pointer __mp = __map_.begin() + __start_ / __block_size;
    return iterator(__mp, __map_.empty() ? 0 : *__mp + __start_ % __block_size);
}

template <class _Tp, class _Allocator>
typename __deque_base<_Tp, _Allocator>::const_iterator
__deque_base<_Tp, _Allocator>::begin() const _NOEXCEPT
{
    __map_const_pointer __mp = static_cast<__map_const_pointer>(__map_.begin() + __start_ / __block_size);
    return const_iterator(__mp, __map_.empty() ? 0 : *__mp + __start_ % __block_size);
}

template <class _Tp, class _Allocator>
typename __deque_base<_Tp, _Allocator>::iterator
__deque_base<_Tp, _Allocator>::end() _NOEXCEPT
{
    size_type __p = size() + __start_;
    __map_pointer __mp = __map_.begin() + __p / __block_size;
    return iterator(__mp, __map_.empty() ? 0 : *__mp + __p % __block_size);
}

template <class _Tp, class _Allocator>
typename __deque_base<_Tp, _Allocator>::const_iterator
__deque_base<_Tp, _Allocator>::end() const _NOEXCEPT
{
    size_type __p = size() + __start_;
    __map_const_pointer __mp = static_cast<__map_const_pointer>(__map_.begin() + __p / __block_size);
    return const_iterator(__mp, __map_.empty() ? 0 : *__mp + __p % __block_size);
}

template <class _Tp, class _Allocator>
inline
__deque_base<_Tp, _Allocator>::__deque_base()
    _NOEXCEPT_(is_nothrow_default_constructible<allocator_type>::value)
    : __start_(0), __size_(0) {}

template <class _Tp, class _Allocator>
inline
__deque_base<_Tp, _Allocator>::__deque_base(const allocator_type& __a)
    : __map_(__pointer_allocator(__a)), __start_(0), __size_(0, __a) {}

template <class _Tp, class _Allocator>
__deque_base<_Tp, _Allocator>::~__deque_base()
{
    clear();
    typename __map::iterator __i = __map_.begin();
    typename __map::iterator __e = __map_.end();
    for (; __i != __e; ++__i)
        __alloc_traits::deallocate(__alloc(), *__i, __block_size);
}

#ifndef _LIBCPP_CXX03_LANG

template <class _Tp, class _Allocator>
__deque_base<_Tp, _Allocator>::__deque_base(__deque_base&& __c)
    _NOEXCEPT_(is_nothrow_move_constructible<allocator_type>::value)
    : __map_(_VSTD::move(__c.__map_)),
      __start_(_VSTD::move(__c.__start_)),
      __size_(_VSTD::move(__c.__size_))
{
    __c.__start_ = 0;
    __c.size() = 0;
}

template <class _Tp, class _Allocator>
__deque_base<_Tp, _Allocator>::__deque_base(__deque_base&& __c, const allocator_type& __a)
    : __map_(_VSTD::move(__c.__map_), __pointer_allocator(__a)),
      __start_(_VSTD::move(__c.__start_)),
      __size_(_VSTD::move(__c.size()), __a)
{
    if (__a == __c.__alloc())
    {
        __c.__start_ = 0;
        __c.size() = 0;
    }
    else
    {
        __map_.clear();
        __start_ = 0;
        size() = 0;
    }
}

#endif  // _LIBCPP_CXX03_LANG

template <class _Tp, class _Allocator>
void
__deque_base<_Tp, _Allocator>::swap(__deque_base& __c)
#if _LIBCPP_STD_VER >= 14
        _NOEXCEPT
#else
        _NOEXCEPT_(!__alloc_traits::propagate_on_container_swap::value ||
                    __is_nothrow_swappable<allocator_type>::value)
#endif
{
    __map_.swap(__c.__map_);
    _VSTD::swap(__start_, __c.__start_);
    _VSTD::swap(size(), __c.size());
    __swap_allocator(__alloc(), __c.__alloc());
}

template <class _Tp, class _Allocator>
void
__deque_base<_Tp, _Allocator>::clear() _NOEXCEPT
{
    allocator_type& __a = __alloc();
    for (iterator __i = begin(), __e = end(); __i != __e; ++__i)
        __alloc_traits::destroy(__a, _VSTD::addressof(*__i));
    size() = 0;
    while (__map_.size() > 2)
    {
        __alloc_traits::deallocate(__a, __map_.front(), __block_size);
        __map_.pop_front();
    }
    switch (__map_.size())
    {
    case 1:
        __start_ = __block_size / 2;
        break;
    case 2:
        __start_ = __block_size;
        break;
    }
}

template <class _Tp, class _Allocator /*= allocator<_Tp>*/>
class _LIBCPP_TEMPLATE_VIS deque
    : private __deque_base<_Tp, _Allocator>
{
public:
    // types:

    typedef _Tp value_type;
    typedef _Allocator allocator_type;

    static_assert((is_same<typename allocator_type::value_type, value_type>::value),
                  "Allocator::value_type must be same type as value_type");

    typedef __deque_base<value_type, allocator_type> __base;

    typedef typename __base::__alloc_traits        __alloc_traits;
    typedef typename __base::reference             reference;
    typedef typename __base::const_reference       const_reference;
    typedef typename __base::iterator              iterator;
    typedef typename __base::const_iterator        const_iterator;
    typedef typename __base::size_type             size_type;
    typedef typename __base::difference_type       difference_type;

    typedef typename __base::pointer               pointer;
    typedef typename __base::const_pointer         const_pointer;
    typedef _VSTD::reverse_iterator<iterator>       reverse_iterator;
    typedef _VSTD::reverse_iterator<const_iterator> const_reverse_iterator;

    // construct/copy/destroy:
    _LIBCPP_INLINE_VISIBILITY
    deque()
        _NOEXCEPT_(is_nothrow_default_constructible<allocator_type>::value)
        {}
    _LIBCPP_INLINE_VISIBILITY explicit deque(const allocator_type& __a) : __base(__a) {}
    explicit deque(size_type __n);
#if _LIBCPP_STD_VER > 11
    explicit deque(size_type __n, const _Allocator& __a);
#endif
    deque(size_type __n, const value_type& __v);
    deque(size_type __n, const value_type& __v, const allocator_type& __a);
    template <class _InputIter>
        deque(_InputIter __f, _InputIter __l,
              typename enable_if<__is_input_iterator<_InputIter>::value>::type* = 0);
    template <class _InputIter>
        deque(_InputIter __f, _InputIter __l, const allocator_type& __a,
              typename enable_if<__is_input_iterator<_InputIter>::value>::type* = 0);
    deque(const deque& __c);
    deque(const deque& __c, const allocator_type& __a);

    deque& operator=(const deque& __c);

#ifndef _LIBCPP_CXX03_LANG
    deque(initializer_list<value_type> __il);
    deque(initializer_list<value_type> __il, const allocator_type& __a);

    _LIBCPP_INLINE_VISIBILITY
    deque& operator=(initializer_list<value_type> __il) {assign(__il); return *this;}

    _LIBCPP_INLINE_VISIBILITY
    deque(deque&& __c) _NOEXCEPT_(is_nothrow_move_constructible<__base>::value);
    _LIBCPP_INLINE_VISIBILITY
    deque(deque&& __c, const allocator_type& __a);
    _LIBCPP_INLINE_VISIBILITY
    deque& operator=(deque&& __c)
        _NOEXCEPT_(__alloc_traits::propagate_on_container_move_assignment::value &&
                   is_nothrow_move_assignable<allocator_type>::value);

    _LIBCPP_INLINE_VISIBILITY
    void assign(initializer_list<value_type> __il) {assign(__il.begin(), __il.end());}
#endif  // _LIBCPP_CXX03_LANG

    template <class _InputIter>
        void assign(_InputIter __f, _InputIter __l,
                    typename enable_if<__is_input_iterator<_InputIter>::value &&
                                      !__is_random_access_iterator<_InputIter>::value>::type* = 0);
    template <class _RAIter>
        void assign(_RAIter __f, _RAIter __l,
                    typename enable_if<__is_random_access_iterator<_RAIter>::value>::type* = 0);
    void assign(size_type __n, const value_type& __v);

    _LIBCPP_INLINE_VISIBILITY
    allocator_type get_allocator() const _NOEXCEPT;

    // iterators:

    _LIBCPP_INLINE_VISIBILITY
    iterator       begin() _NOEXCEPT       {return __base::begin();}
    _LIBCPP_INLINE_VISIBILITY
    const_iterator begin() const _NOEXCEPT {return __base::begin();}
    _LIBCPP_INLINE_VISIBILITY
    iterator       end() _NOEXCEPT         {return __base::end();}
    _LIBCPP_INLINE_VISIBILITY
    const_iterator end()   const _NOEXCEPT {return __base::end();}

    _LIBCPP_INLINE_VISIBILITY
    reverse_iterator       rbegin() _NOEXCEPT
        {return       reverse_iterator(__base::end());}
    _LIBCPP_INLINE_VISIBILITY
    const_reverse_iterator rbegin() const _NOEXCEPT
        {return const_reverse_iterator(__base::end());}
    _LIBCPP_INLINE_VISIBILITY
    reverse_iterator       rend() _NOEXCEPT
        {return       reverse_iterator(__base::begin());}
    _LIBCPP_INLINE_VISIBILITY
    const_reverse_iterator rend()   const _NOEXCEPT
        {return const_reverse_iterator(__base::begin());}

    _LIBCPP_INLINE_VISIBILITY
    const_iterator         cbegin()  const _NOEXCEPT
        {return __base::begin();}
    _LIBCPP_INLINE_VISIBILITY
    const_iterator         cend()    const _NOEXCEPT
        {return __base::end();}
    _LIBCPP_INLINE_VISIBILITY
    const_reverse_iterator crbegin() const _NOEXCEPT
        {return const_reverse_iterator(__base::end());}
    _LIBCPP_INLINE_VISIBILITY
    const_reverse_iterator crend()   const _NOEXCEPT
        {return const_reverse_iterator(__base::begin());}

    // capacity:
    _LIBCPP_INLINE_VISIBILITY
    size_type size() const _NOEXCEPT {return __base::size();}
    _LIBCPP_INLINE_VISIBILITY
    size_type max_size() const _NOEXCEPT
        {return std::min<size_type>(
            __alloc_traits::max_size(__base::__alloc()),
            numeric_limits<difference_type>::max());}
    void resize(size_type __n);
    void resize(size_type __n, const value_type& __v);
    void shrink_to_fit() _NOEXCEPT;
    _LIBCPP_INLINE_VISIBILITY
    bool empty() const _NOEXCEPT {return __base::size() == 0;}

    // element access:
    _LIBCPP_INLINE_VISIBILITY
    reference operator[](size_type __i);
    _LIBCPP_INLINE_VISIBILITY
    const_reference operator[](size_type __i) const;
    _LIBCPP_INLINE_VISIBILITY
    reference at(size_type __i);
    _LIBCPP_INLINE_VISIBILITY
    const_reference at(size_type __i) const;
    _LIBCPP_INLINE_VISIBILITY
    reference front();
    _LIBCPP_INLINE_VISIBILITY
    const_reference front() const;
    _LIBCPP_INLINE_VISIBILITY
    reference back();
    _LIBCPP_INLINE_VISIBILITY
    const_reference back() const;

    // 23.2.2.3 modifiers:
    void push_front(const value_type& __v);
    void push_back(const value_type& __v);
#ifndef _LIBCPP_CXX03_LANG
#if _LIBCPP_STD_VER > 14
    template <class... _Args> reference emplace_front(_Args&&... __args);
    template <class... _Args> reference emplace_back (_Args&&... __args);
#else
    template <class... _Args> void      emplace_front(_Args&&... __args);
    template <class... _Args> void      emplace_back (_Args&&... __args);
#endif
    template <class... _Args> iterator emplace(const_iterator __p, _Args&&... __args);

    void push_front(value_type&& __v);
    void push_back(value_type&& __v);
    iterator insert(const_iterator __p, value_type&& __v);

    _LIBCPP_INLINE_VISIBILITY
    iterator insert(const_iterator __p, initializer_list<value_type> __il)
        {return insert(__p, __il.begin(), __il.end());}
#endif  // _LIBCPP_CXX03_LANG

    iterator insert(const_iterator __p, const value_type& __v);
    iterator insert(const_iterator __p, size_type __n, const value_type& __v);
    template <class _InputIter>
        iterator insert(const_iterator __p, _InputIter __f, _InputIter __l,
                         typename enable_if<__is_input_iterator<_InputIter>::value
                                         &&!__is_forward_iterator<_InputIter>::value>::type* = 0);
    template <class _ForwardIterator>
        iterator insert(const_iterator __p, _ForwardIterator __f, _ForwardIterator __l,
                               typename enable_if<__is_forward_iterator<_ForwardIterator>::value
                                         &&!__is_bidirectional_iterator<_ForwardIterator>::value>::type* = 0);
    template <class _BiIter>
        iterator insert(const_iterator __p, _BiIter __f, _BiIter __l,
                         typename enable_if<__is_bidirectional_iterator<_BiIter>::value>::type* = 0);

    void pop_front();
    void pop_back();
    iterator erase(const_iterator __p);
    iterator erase(const_iterator __f, const_iterator __l);

    _LIBCPP_INLINE_VISIBILITY
    void swap(deque& __c)
#if _LIBCPP_STD_VER >= 14
        _NOEXCEPT;
#else
        _NOEXCEPT_(!__alloc_traits::propagate_on_container_swap::value ||
                   __is_nothrow_swappable<allocator_type>::value);
#endif
    _LIBCPP_INLINE_VISIBILITY
    void clear() _NOEXCEPT;

    _LIBCPP_INLINE_VISIBILITY
    bool __invariants() const {return __base::__invariants();}
private:
    typedef typename __base::__map_const_pointer __map_const_pointer;

    _LIBCPP_INLINE_VISIBILITY
    static size_type __recommend_blocks(size_type __n)
    {
        return __n / __base::__block_size + (__n % __base::__block_size != 0);
    }
    _LIBCPP_INLINE_VISIBILITY
    size_type __capacity() const
    {
        return __base::__map_.size() == 0 ? 0 : __base::__map_.size() * __base::__block_size - 1;
    }
    _LIBCPP_INLINE_VISIBILITY
    size_type __front_spare() const
    {
        return __base::__start_;
    }
    _LIBCPP_INLINE_VISIBILITY
    size_type __back_spare() const
    {
        return __capacity() - (__base::__start_ + __base::size());
    }

    template <class _InpIter>
        void __append(_InpIter __f, _InpIter __l,
                 typename enable_if<__is_input_iterator<_InpIter>::value &&
                                   !__is_forward_iterator<_InpIter>::value>::type* = 0);
    template <class _ForIter>
        void __append(_ForIter __f, _ForIter __l,
                      typename enable_if<__is_forward_iterator<_ForIter>::value>::type* = 0);
    void __append(size_type __n);
    void __append(size_type __n, const value_type& __v);
    void __erase_to_end(const_iterator __f);
    void __add_front_capacity();
    void __add_front_capacity(size_type __n);
    void __add_back_capacity();
    void __add_back_capacity(size_type __n);
    iterator __move_and_check(iterator __f, iterator __l, iterator __r,
                              const_pointer& __vt);
    iterator __move_backward_and_check(iterator __f, iterator __l, iterator __r,
                                       const_pointer& __vt);
    void __move_construct_and_check(iterator __f, iterator __l,
                                    iterator __r, const_pointer& __vt);
    void __move_construct_backward_and_check(iterator __f, iterator __l,
                                             iterator __r, const_pointer& __vt);

    _LIBCPP_INLINE_VISIBILITY
    void __copy_assign_alloc(const deque& __c)
        {__copy_assign_alloc(__c, integral_constant<bool,
                      __alloc_traits::propagate_on_container_copy_assignment::value>());}

    _LIBCPP_INLINE_VISIBILITY
    void __copy_assign_alloc(const deque& __c, true_type)
        {
            if (__base::__alloc() != __c.__alloc())
            {
                clear();
                shrink_to_fit();
            }
            __base::__alloc() = __c.__alloc();
            __base::__map_.__alloc() = __c.__map_.__alloc();
        }

    _LIBCPP_INLINE_VISIBILITY
    void __copy_assign_alloc(const deque&, false_type)
        {}

    void __move_assign(deque& __c, true_type)
        _NOEXCEPT_(is_nothrow_move_assignable<allocator_type>::value);
    void __move_assign(deque& __c, false_type);
};

template <class _Tp, class _Allocator>
deque<_Tp, _Allocator>::deque(size_type __n)
{
    if (__n > 0)
        __append(__n);
}

#if _LIBCPP_STD_VER > 11
template <class _Tp, class _Allocator>
deque<_Tp, _Allocator>::deque(size_type __n, const _Allocator& __a)
    : __base(__a)
{
    if (__n > 0)
        __append(__n);
}
#endif

template <class _Tp, class _Allocator>
deque<_Tp, _Allocator>::deque(size_type __n, const value_type& __v)
{
    if (__n > 0)
        __append(__n, __v);
}

template <class _Tp, class _Allocator>
deque<_Tp, _Allocator>::deque(size_type __n, const value_type& __v, const allocator_type& __a)
    : __base(__a)
{
    if (__n > 0)
        __append(__n, __v);
}

template <class _Tp, class _Allocator>
template <class _InputIter>
deque<_Tp, _Allocator>::deque(_InputIter __f, _InputIter __l,
              typename enable_if<__is_input_iterator<_InputIter>::value>::type*)
{
    __append(__f, __l);
}

template <class _Tp, class _Allocator>
template <class _InputIter>
deque<_Tp, _Allocator>::deque(_InputIter __f, _InputIter __l, const allocator_type& __a,
              typename enable_if<__is_input_iterator<_InputIter>::value>::type*)
    : __base(__a)
{
    __append(__f, __l);
}

template <class _Tp, class _Allocator>
deque<_Tp, _Allocator>::deque(const deque& __c)
    : __base(__alloc_traits::select_on_container_copy_construction(__c.__alloc()))
{
    __append(__c.begin(), __c.end());
}

template <class _Tp, class _Allocator>
deque<_Tp, _Allocator>::deque(const deque& __c, const allocator_type& __a)
    : __base(__a)
{
    __append(__c.begin(), __c.end());
}

template <class _Tp, class _Allocator>
deque<_Tp, _Allocator>&
deque<_Tp, _Allocator>::operator=(const deque& __c)
{
    if (this != &__c)
    {
        __copy_assign_alloc(__c);
        assign(__c.begin(), __c.end());
    }
    return *this;
}

#ifndef _LIBCPP_CXX03_LANG

template <class _Tp, class _Allocator>
deque<_Tp, _Allocator>::deque(initializer_list<value_type> __il)
{
    __append(__il.begin(), __il.end());
}

template <class _Tp, class _Allocator>
deque<_Tp, _Allocator>::deque(initializer_list<value_type> __il, const allocator_type& __a)
    : __base(__a)
{
    __append(__il.begin(), __il.end());
}

template <class _Tp, class _Allocator>
inline
deque<_Tp, _Allocator>::deque(deque&& __c)
    _NOEXCEPT_(is_nothrow_move_constructible<__base>::value)
    : __base(_VSTD::move(__c))
{
}

template <class _Tp, class _Allocator>
inline
deque<_Tp, _Allocator>::deque(deque&& __c, const allocator_type& __a)
    : __base(_VSTD::move(__c), __a)
{
    if (__a != __c.__alloc())
    {
        typedef move_iterator<iterator> _Ip;
        assign(_Ip(__c.begin()), _Ip(__c.end()));
    }
}

template <class _Tp, class _Allocator>
inline
deque<_Tp, _Allocator>&
deque<_Tp, _Allocator>::operator=(deque&& __c)
        _NOEXCEPT_(__alloc_traits::propagate_on_container_move_assignment::value &&
                   is_nothrow_move_assignable<allocator_type>::value)
{
    __move_assign(__c, integral_constant<bool,
          __alloc_traits::propagate_on_container_move_assignment::value>());
    return *this;
}

template <class _Tp, class _Allocator>
void
deque<_Tp, _Allocator>::__move_assign(deque& __c, false_type)
{
    if (__base::__alloc() != __c.__alloc())
    {
        typedef move_iterator<iterator> _Ip;
        assign(_Ip(__c.begin()), _Ip(__c.end()));
    }
    else
        __move_assign(__c, true_type());
}

template <class _Tp, class _Allocator>
void
deque<_Tp, _Allocator>::__move_assign(deque& __c, true_type)
    _NOEXCEPT_(is_nothrow_move_assignable<allocator_type>::value)
{
    clear();
    shrink_to_fit();
    __base::__move_assign(__c);
}

#endif  // _LIBCPP_CXX03_LANG

template <class _Tp, class _Allocator>
template <class _InputIter>
void
deque<_Tp, _Allocator>::assign(_InputIter __f, _InputIter __l,
                               typename enable_if<__is_input_iterator<_InputIter>::value &&
                                                 !__is_random_access_iterator<_InputIter>::value>::type*)
{
    iterator __i = __base::begin();
    iterator __e = __base::end();
    for (; __f != __l && __i != __e; ++__f, (void) ++__i)
        *__i = *__f;
    if (__f != __l)
        __append(__f, __l);
    else
        __erase_to_end(__i);
}

template <class _Tp, class _Allocator>
template <class _RAIter>
void
deque<_Tp, _Allocator>::assign(_RAIter __f, _RAIter __l,
                               typename enable_if<__is_random_access_iterator<_RAIter>::value>::type*)
{
    if (static_cast<size_type>(__l - __f) > __base::size())
    {
        _RAIter __m = __f + __base::size();
        _VSTD::copy(__f, __m, __base::begin());
        __append(__m, __l);
    }
    else
        __erase_to_end(_VSTD::copy(__f, __l, __base::begin()));
}

template <class _Tp, class _Allocator>
void
deque<_Tp, _Allocator>::assign(size_type __n, const value_type& __v)
{
    if (__n > __base::size())
    {
        _VSTD::fill_n(__base::begin(), __base::size(), __v);
        __n -= __base::size();
        __append(__n, __v);
    }
    else
        __erase_to_end(_VSTD::fill_n(__base::begin(), __n, __v));
}

template <class _Tp, class _Allocator>
inline
_Allocator
deque<_Tp, _Allocator>::get_allocator() const _NOEXCEPT
{
    return __base::__alloc();
}

template <class _Tp, class _Allocator>
void
deque<_Tp, _Allocator>::resize(size_type __n)
{
    if (__n > __base::size())
        __append(__n - __base::size());
    else if (__n < __base::size())
        __erase_to_end(__base::begin() + __n);
}

template <class _Tp, class _Allocator>
void
deque<_Tp, _Allocator>::resize(size_type __n, const value_type& __v)
{
    if (__n > __base::size())
        __append(__n - __base::size(), __v);
    else if (__n < __base::size())
        __erase_to_end(__base::begin() + __n);
}

template <class _Tp, class _Allocator>
void
deque<_Tp, _Allocator>::shrink_to_fit() _NOEXCEPT
{
    allocator_type& __a = __base::__alloc();
    if (empty())
    {
        while (__base::__map_.size() > 0)
        {
            __alloc_traits::deallocate(__a, __base::__map_.back(), __base::__block_size);
            __base::__map_.pop_back();
        }
        __base::__start_ = 0;
    }
    else
    {
        if (__front_spare() >= __base::__block_size)
        {
            __alloc_traits::deallocate(__a, __base::__map_.front(), __base::__block_size);
            __base::__map_.pop_front();
            __base::__start_ -= __base::__block_size;
        }
        if (__back_spare() >= __base::__block_size)
        {
            __alloc_traits::deallocate(__a, __base::__map_.back(), __base::__block_size);
            __base::__map_.pop_back();
        }
    }
    __base::__map_.shrink_to_fit();
}

template <class _Tp, class _Allocator>
inline
typename deque<_Tp, _Allocator>::reference
deque<_Tp, _Allocator>::operator[](size_type __i)
{
    size_type __p = __base::__start_ + __i;
    return *(*(__base::__map_.begin() + __p / __base::__block_size) + __p % __base::__block_size);
}

template <class _Tp, class _Allocator>
inline
typename deque<_Tp, _Allocator>::const_reference
deque<_Tp, _Allocator>::operator[](size_type __i) const
{
    size_type __p = __base::__start_ + __i;
    return *(*(__base::__map_.begin() + __p / __base::__block_size) + __p % __base::__block_size);
}

template <class _Tp, class _Allocator>
inline
typename deque<_Tp, _Allocator>::reference
deque<_Tp, _Allocator>::at(size_type __i)
{
    if (__i >= __base::size())
        __base::__throw_out_of_range();
    size_type __p = __base::__start_ + __i;
    return *(*(__base::__map_.begin() + __p / __base::__block_size) + __p % __base::__block_size);
}

template <class _Tp, class _Allocator>
inline
typename deque<_Tp, _Allocator>::const_reference
deque<_Tp, _Allocator>::at(size_type __i) const
{
    if (__i >= __base::size())
        __base::__throw_out_of_range();
    size_type __p = __base::__start_ + __i;
    return *(*(__base::__map_.begin() + __p / __base::__block_size) + __p % __base::__block_size);
}

template <class _Tp, class _Allocator>
inline
typename deque<_Tp, _Allocator>::reference
deque<_Tp, _Allocator>::front()
{
    return *(*(__base::__map_.begin() + __base::__start_ / __base::__block_size)
                                      + __base::__start_ % __base::__block_size);
}

template <class _Tp, class _Allocator>
inline
typename deque<_Tp, _Allocator>::const_reference
deque<_Tp, _Allocator>::front() const
{
    return *(*(__base::__map_.begin() + __base::__start_ / __base::__block_size)
                                      + __base::__start_ % __base::__block_size);
}

template <class _Tp, class _Allocator>
inline
typename deque<_Tp, _Allocator>::reference
deque<_Tp, _Allocator>::back()
{
    size_type __p = __base::size() + __base::__start_ - 1;
    return *(*(__base::__map_.begin() + __p / __base::__block_size) + __p % __base::__block_size);
}

template <class _Tp, class _Allocator>
inline
typename deque<_Tp, _Allocator>::const_reference
deque<_Tp, _Allocator>::back() const
{
    size_type __p = __base::size() + __base::__start_ - 1;
    return *(*(__base::__map_.begin() + __p / __base::__block_size) + __p % __base::__block_size);
}

template <class _Tp, class _Allocator>
void
deque<_Tp, _Allocator>::push_back(const value_type& __v)
{
    allocator_type& __a = __base::__alloc();
    if (__back_spare() == 0)
        __add_back_capacity();
    // __back_spare() >= 1
    __alloc_traits::construct(__a, _VSTD::addressof(*__base::end()), __v);
    ++__base::size();
}

template <class _Tp, class _Allocator>
void
deque<_Tp, _Allocator>::push_front(const value_type& __v)
{
    allocator_type& __a = __base::__alloc();
    if (__front_spare() == 0)
        __add_front_capacity();
    // __front_spare() >= 1
    __alloc_traits::construct(__a, _VSTD::addressof(*--__base::begin()), __v);
    --__base::__start_;
    ++__base::size();
}

#ifndef _LIBCPP_CXX03_LANG
template <class _Tp, class _Allocator>
void
deque<_Tp, _Allocator>::push_back(value_type&& __v)
{
    allocator_type& __a = __base::__alloc();
    if (__back_spare() == 0)
        __add_back_capacity();
    // __back_spare() >= 1
    __alloc_traits::construct(__a, _VSTD::addressof(*__base::end()), _VSTD::move(__v));
    ++__base::size();
}

template <class _Tp, class _Allocator>
template <class... _Args>
#if _LIBCPP_STD_VER > 14
typename deque<_Tp, _Allocator>::reference
#else
void
#endif
deque<_Tp, _Allocator>::emplace_back(_Args&&... __args)
{
    allocator_type& __a = __base::__alloc();
    if (__back_spare() == 0)
        __add_back_capacity();
    // __back_spare() >= 1
    __alloc_traits::construct(__a, _VSTD::addressof(*__base::end()),
                              _VSTD::forward<_Args>(__args)...);
    ++__base::size();
#if _LIBCPP_STD_VER > 14
    return *--__base::end();
#endif
}

template <class _Tp, class _Allocator>
void
deque<_Tp, _Allocator>::push_front(value_type&& __v)
{
    allocator_type& __a = __base::__alloc();
    if (__front_spare() == 0)
        __add_front_capacity();
    // __front_spare() >= 1
    __alloc_traits::construct(__a, _VSTD::addressof(*--__base::begin()), _VSTD::move(__v));
    --__base::__start_;
    ++__base::size();
}


template <class _Tp, class _Allocator>
template <class... _Args>
#if _LIBCPP_STD_VER > 14
typename deque<_Tp, _Allocator>::reference
#else
void
#endif
deque<_Tp, _Allocator>::emplace_front(_Args&&... __args)
{
    allocator_type& __a = __base::__alloc();
    if (__front_spare() == 0)
        __add_front_capacity();
    // __front_spare() >= 1
    __alloc_traits::construct(__a, _VSTD::addressof(*--__base::begin()), _VSTD::forward<_Args>(__args)...);
    --__base::__start_;
    ++__base::size();
#if _LIBCPP_STD_VER > 14
    return *__base::begin();
#endif
}

template <class _Tp, class _Allocator>
typename deque<_Tp, _Allocator>::iterator
deque<_Tp, _Allocator>::insert(const_iterator __p, value_type&& __v)
{
    size_type __pos = __p - __base::begin();
    size_type __to_end = __base::size() - __pos;
    allocator_type& __a = __base::__alloc();
    if (__pos < __to_end)
    {   // insert by shifting things backward
        if (__front_spare() == 0)
            __add_front_capacity();
        // __front_spare() >= 1
        if (__pos == 0)
        {
            __alloc_traits::construct(__a, _VSTD::addressof(*--__base::begin()), _VSTD::move(__v));
            --__base::__start_;
            ++__base::size();
        }
        else
        {
            iterator __b = __base::begin();
            iterator __bm1 = _VSTD::prev(__b);
            __alloc_traits::construct(__a, _VSTD::addressof(*__bm1), _VSTD::move(*__b));
            --__base::__start_;
            ++__base::size();
            if (__pos > 1)
                __b = _VSTD::move(_VSTD::next(__b), __b + __pos, __b);
            *__b = _VSTD::move(__v);
        }
    }
    else
    {   // insert by shifting things forward
        if (__back_spare() == 0)
            __add_back_capacity();
        // __back_capacity >= 1
        size_type __de = __base::size() - __pos;
        if (__de == 0)
        {
            __alloc_traits::construct(__a, _VSTD::addressof(*__base::end()), _VSTD::move(__v));
            ++__base::size();
        }
        else
        {
            iterator __e = __base::end();
            iterator __em1 = _VSTD::prev(__e);
            __alloc_traits::construct(__a, _VSTD::addressof(*__e), _VSTD::move(*__em1));
            ++__base::size();
            if (__de > 1)
                __e = _VSTD::move_backward(__e - __de, __em1, __e);
            *--__e = _VSTD::move(__v);
        }
    }
    return __base::begin() + __pos;
}

template <class _Tp, class _Allocator>
template <class... _Args>
typename deque<_Tp, _Allocator>::iterator
deque<_Tp, _Allocator>::emplace(const_iterator __p, _Args&&... __args)
{
    size_type __pos = __p - __base::begin();
    size_type __to_end = __base::size() - __pos;
    allocator_type& __a = __base::__alloc();
    if (__pos < __to_end)
    {   // insert by shifting things backward
        if (__front_spare() == 0)
            __add_front_capacity();
        // __front_spare() >= 1
        if (__pos == 0)
        {
            __alloc_traits::construct(__a, _VSTD::addressof(*--__base::begin()), _VSTD::forward<_Args>(__args)...);
            --__base::__start_;
            ++__base::size();
        }
        else
        {
            __temp_value<value_type, _Allocator> __tmp(this->__alloc(), _VSTD::forward<_Args>(__args)...);
            iterator __b = __base::begin();
            iterator __bm1 = _VSTD::prev(__b);
            __alloc_traits::construct(__a, _VSTD::addressof(*__bm1), _VSTD::move(*__b));
            --__base::__start_;
            ++__base::size();
            if (__pos > 1)
                __b = _VSTD::move(_VSTD::next(__b), __b + __pos, __b);
            *__b = _VSTD::move(__tmp.get());
        }
    }
    else
    {   // insert by shifting things forward
        if (__back_spare() == 0)
            __add_back_capacity();
        // __back_capacity >= 1
        size_type __de = __base::size() - __pos;
        if (__de == 0)
        {
            __alloc_traits::construct(__a, _VSTD::addressof(*__base::end()), _VSTD::forward<_Args>(__args)...);
            ++__base::size();
        }
        else
        {
            __temp_value<value_type, _Allocator> __tmp(this->__alloc(), _VSTD::forward<_Args>(__args)...);
            iterator __e = __base::end();
            iterator __em1 = _VSTD::prev(__e);
            __alloc_traits::construct(__a, _VSTD::addressof(*__e), _VSTD::move(*__em1));
            ++__base::size();
            if (__de > 1)
                __e = _VSTD::move_backward(__e - __de, __em1, __e);
            *--__e = _VSTD::move(__tmp.get());
        }
    }
    return __base::begin() + __pos;
}

#endif  // _LIBCPP_CXX03_LANG


template <class _Tp, class _Allocator>
typename deque<_Tp, _Allocator>::iterator
deque<_Tp, _Allocator>::insert(const_iterator __p, const value_type& __v)
{
    size_type __pos = __p - __base::begin();
    size_type __to_end = __base::size() - __pos;
    allocator_type& __a = __base::__alloc();
    if (__pos < __to_end)
    {   // insert by shifting things backward
        if (__front_spare() == 0)
            __add_front_capacity();
        // __front_spare() >= 1
        if (__pos == 0)
        {
            __alloc_traits::construct(__a, _VSTD::addressof(*--__base::begin()), __v);
            --__base::__start_;
            ++__base::size();
        }
        else
        {
            const_pointer __vt = pointer_traits<const_pointer>::pointer_to(__v);
            iterator __b = __base::begin();
            iterator __bm1 = _VSTD::prev(__b);
            if (__vt == pointer_traits<const_pointer>::pointer_to(*__b))
                __vt = pointer_traits<const_pointer>::pointer_to(*__bm1);
            __alloc_traits::construct(__a, _VSTD::addressof(*__bm1), _VSTD::move(*__b));
            --__base::__start_;
            ++__base::size();
            if (__pos > 1)
                __b = __move_and_check(_VSTD::next(__b), __b + __pos, __b, __vt);
            *__b = *__vt;
        }
    }
    else
    {   // insert by shifting things forward
        if (__back_spare() == 0)
            __add_back_capacity();
        // __back_capacity >= 1
        size_type __de = __base::size() - __pos;
        if (__de == 0)
        {
            __alloc_traits::construct(__a, _VSTD::addressof(*__base::end()), __v);
            ++__base::size();
        }
        else
        {
            const_pointer __vt = pointer_traits<const_pointer>::pointer_to(__v);
            iterator __e = __base::end();
            iterator __em1 = _VSTD::prev(__e);
            if (__vt == pointer_traits<const_pointer>::pointer_to(*__em1))
                __vt = pointer_traits<const_pointer>::pointer_to(*__e);
            __alloc_traits::construct(__a, _VSTD::addressof(*__e), _VSTD::move(*__em1));
            ++__base::size();
            if (__de > 1)
                __e = __move_backward_and_check(__e - __de, __em1, __e, __vt);
            *--__e = *__vt;
        }
    }
    return __base::begin() + __pos;
}

template <class _Tp, class _Allocator>
typename deque<_Tp, _Allocator>::iterator
deque<_Tp, _Allocator>::insert(const_iterator __p, size_type __n, const value_type& __v)
{
    size_type __pos = __p - __base::begin();
    size_type __to_end = __base::size() - __pos;
    allocator_type& __a = __base::__alloc();
    if (__pos < __to_end)
    {   // insert by shifting things backward
        if (__n > __front_spare())
            __add_front_capacity(__n - __front_spare());
        // __n <= __front_spare()
        iterator __old_begin = __base::begin();
        iterator __i = __old_begin;
        if (__n > __pos)
        {
            for (size_type __m = __n - __pos; __m; --__m, --__base::__start_, ++__base::size())
                __alloc_traits::construct(__a, _VSTD::addressof(*--__i), __v);
            __n = __pos;
        }
        if (__n > 0)
        {
            const_pointer __vt = pointer_traits<const_pointer>::pointer_to(__v);
            iterator __obn = __old_begin + __n;
            __move_construct_backward_and_check(__old_begin, __obn, __i, __vt);
            if (__n < __pos)
                __old_begin = __move_and_check(__obn, __old_begin + __pos, __old_begin, __vt);
            _VSTD::fill_n(__old_begin, __n, *__vt);
        }
    }
    else
    {   // insert by shifting things forward
        size_type __back_capacity = __back_spare();
        if (__n > __back_capacity)
            __add_back_capacity(__n - __back_capacity);
        // __n <= __back_capacity
        iterator __old_end = __base::end();
        iterator __i = __old_end;
        size_type __de = __base::size() - __pos;
        if (__n > __de)
        {
            for (size_type __m = __n - __de; __m; --__m, ++__i, ++__base::size())
                __alloc_traits::construct(__a, _VSTD::addressof(*__i), __v);
            __n = __de;
        }
        if (__n > 0)
        {
            const_pointer __vt = pointer_traits<const_pointer>::pointer_to(__v);
            iterator __oen = __old_end - __n;
            __move_construct_and_check(__oen, __old_end, __i, __vt);
            if (__n < __de)
                __old_end = __move_backward_and_check(__old_end - __de, __oen, __old_end, __vt);
            _VSTD::fill_n(__old_end - __n, __n, *__vt);
        }
    }
    return __base::begin() + __pos;
}

template <class _Tp, class _Allocator>
template <class _InputIter>
typename deque<_Tp, _Allocator>::iterator
deque<_Tp, _Allocator>::insert(const_iterator __p, _InputIter __f, _InputIter __l,
                               typename enable_if<__is_input_iterator<_InputIter>::value
                                               &&!__is_forward_iterator<_InputIter>::value>::type*)
{
    __split_buffer<value_type, allocator_type&> __buf(__base::__alloc());
    __buf.__construct_at_end(__f, __l);
    typedef typename __split_buffer<value_type, allocator_type&>::iterator __bi;
    return insert(__p, move_iterator<__bi>(__buf.begin()), move_iterator<__bi>(__buf.end()));
}

template <class _Tp, class _Allocator>
template <class _ForwardIterator>
typename deque<_Tp, _Allocator>::iterator
deque<_Tp, _Allocator>::insert(const_iterator __p, _ForwardIterator __f, _ForwardIterator __l,
                               typename enable_if<__is_forward_iterator<_ForwardIterator>::value
                                               &&!__is_bidirectional_iterator<_ForwardIterator>::value>::type*)
{
    size_type __n = _VSTD::distance(__f, __l);
    __split_buffer<value_type, allocator_type&> __buf(__n, 0, __base::__alloc());
    __buf.__construct_at_end(__f, __l);
    typedef typename __split_buffer<value_type, allocator_type&>::iterator __fwd;
    return insert(__p, move_iterator<__fwd>(__buf.begin()), move_iterator<__fwd>(__buf.end()));
}

template <class _Tp, class _Allocator>
template <class _BiIter>
typename deque<_Tp, _Allocator>::iterator
deque<_Tp, _Allocator>::insert(const_iterator __p, _BiIter __f, _BiIter __l,
                               typename enable_if<__is_bidirectional_iterator<_BiIter>::value>::type*)
{
    size_type __n = _VSTD::distance(__f, __l);
    size_type __pos = __p - __base::begin();
    size_type __to_end = __base::size() - __pos;
    allocator_type& __a = __base::__alloc();
    if (__pos < __to_end)
    {   // insert by shifting things backward
        if (__n > __front_spare())
            __add_front_capacity(__n - __front_spare());
        // __n <= __front_spare()
        iterator __old_begin = __base::begin();
        iterator __i = __old_begin;
        _BiIter __m = __f;
        if (__n > __pos)
        {
            __m = __pos < __n / 2 ? _VSTD::prev(__l, __pos) : _VSTD::next(__f, __n - __pos);
            for (_BiIter __j = __m; __j != __f; --__base::__start_, ++__base::size())
                __alloc_traits::construct(__a, _VSTD::addressof(*--__i), *--__j);
            __n = __pos;
        }
        if (__n > 0)
        {
            iterator __obn = __old_begin + __n;
            for (iterator __j = __obn; __j != __old_begin;)
            {
                __alloc_traits::construct(__a, _VSTD::addressof(*--__i), _VSTD::move(*--__j));
                --__base::__start_;
                ++__base::size();
            }
            if (__n < __pos)
                __old_begin = _VSTD::move(__obn, __old_begin + __pos, __old_begin);
            _VSTD::copy(__m, __l, __old_begin);
        }
    }
    else
    {   // insert by shifting things forward
        size_type __back_capacity = __back_spare();
        if (__n > __back_capacity)
            __add_back_capacity(__n - __back_capacity);
        // __n <= __back_capacity
        iterator __old_end = __base::end();
        iterator __i = __old_end;
        _BiIter __m = __l;
        size_type __de = __base::size() - __pos;
        if (__n > __de)
        {
            __m = __de < __n / 2 ? _VSTD::next(__f, __de) : _VSTD::prev(__l, __n - __de);
            for (_BiIter __j = __m; __j != __l; ++__i, (void) ++__j, ++__base::size())
                __alloc_traits::construct(__a, _VSTD::addressof(*__i), *__j);
            __n = __de;
        }
        if (__n > 0)
        {
            iterator __oen = __old_end - __n;
            for (iterator __j = __oen; __j != __old_end; ++__i, ++__j, ++__base::size())
                __alloc_traits::construct(__a, _VSTD::addressof(*__i), _VSTD::move(*__j));
            if (__n < __de)
                __old_end = _VSTD::move_backward(__old_end - __de, __oen, __old_end);
            _VSTD::copy_backward(__f, __m, __old_end);
        }
    }
    return __base::begin() + __pos;
}

template <class _Tp, class _Allocator>
template <class _InpIter>
void
deque<_Tp, _Allocator>::__append(_InpIter __f, _InpIter __l,
                                 typename enable_if<__is_input_iterator<_InpIter>::value &&
                                                   !__is_forward_iterator<_InpIter>::value>::type*)
{
    for (; __f != __l; ++__f)
        push_back(*__f);
}

template <class _Tp, class _Allocator>
template <class _ForIter>
void
deque<_Tp, _Allocator>::__append(_ForIter __f, _ForIter __l,
                                 typename enable_if<__is_forward_iterator<_ForIter>::value>::type*)
{
    size_type __n = _VSTD::distance(__f, __l);
    allocator_type& __a = __base::__alloc();
    size_type __back_capacity = __back_spare();
    if (__n > __back_capacity)
        __add_back_capacity(__n - __back_capacity);
    // __n <= __back_capacity
    for (iterator __i = __base::end(); __f != __l; ++__i, (void) ++__f, ++__base::size())
        __alloc_traits::construct(__a, _VSTD::addressof(*__i), *__f);
}

template <class _Tp, class _Allocator>
void
deque<_Tp, _Allocator>::__append(size_type __n)
{
    allocator_type& __a = __base::__alloc();
    size_type __back_capacity = __back_spare();
    if (__n > __back_capacity)
        __add_back_capacity(__n - __back_capacity);
    // __n <= __back_capacity
    for (iterator __i = __base::end(); __n; --__n, ++__i, ++__base::size())
        __alloc_traits::construct(__a, _VSTD::addressof(*__i));
}

template <class _Tp, class _Allocator>
void
deque<_Tp, _Allocator>::__append(size_type __n, const value_type& __v)
{
    allocator_type& __a = __base::__alloc();
    size_type __back_capacity = __back_spare();
    if (__n > __back_capacity)
        __add_back_capacity(__n - __back_capacity);
    // __n <= __back_capacity
    for (iterator __i = __base::end(); __n; --__n, ++__i, ++__base::size())
        __alloc_traits::construct(__a, _VSTD::addressof(*__i), __v);
}

// Create front capacity for one block of elements.
// Strong guarantee.  Either do it or don't touch anything.
template <class _Tp, class _Allocator>
void
deque<_Tp, _Allocator>::__add_front_capacity()
{
    allocator_type& __a = __base::__alloc();
    if (__back_spare() >= __base::__block_size)
    {
        __base::__start_ += __base::__block_size;
        pointer __pt = __base::__map_.back();
        __base::__map_.pop_back();
        __base::__map_.push_front(__pt);
    }
    // Else if __base::__map_.size() < __base::__map_.capacity() then we need to allocate 1 buffer
    else if (__base::__map_.size() < __base::__map_.capacity())
    {   // we can put the new buffer into the map, but don't shift things around
        // until all buffers are allocated.  If we throw, we don't need to fix
        // anything up (any added buffers are undetectible)
        if (__base::__map_.__front_spare() > 0)
            __base::__map_.push_front(__alloc_traits::allocate(__a, __base::__block_size));
        else
        {
            __base::__map_.push_back(__alloc_traits::allocate(__a, __base::__block_size));
            // Done allocating, reorder capacity
            pointer __pt = __base::__map_.back();
            __base::__map_.pop_back();
            __base::__map_.push_front(__pt);
        }
        __base::__start_ = __base::__map_.size() == 1 ?
                               __base::__block_size / 2 :
                               __base::__start_ + __base::__block_size;
    }
    // Else need to allocate 1 buffer, *and* we need to reallocate __map_.
    else
    {
        __split_buffer<pointer, typename __base::__pointer_allocator&>
            __buf(max<size_type>(2 * __base::__map_.capacity(), 1),
                  0, __base::__map_.__alloc());

        typedef __allocator_destructor<_Allocator> _Dp;
        unique_ptr<pointer, _Dp> __hold(
            __alloc_traits::allocate(__a, __base::__block_size),
                _Dp(__a, __base::__block_size));
        __buf.push_back(__hold.get());
        __hold.release();

        for (typename __base::__map_pointer __i = __base::__map_.begin();
                __i != __base::__map_.end(); ++__i)
            __buf.push_back(*__i);
        _VSTD::swap(__base::__map_.__first_, __buf.__first_);
        _VSTD::swap(__base::__map_.__begin_, __buf.__begin_);
        _VSTD::swap(__base::__map_.__end_, __buf.__end_);
        _VSTD::swap(__base::__map_.__end_cap(), __buf.__end_cap());
        __base::__start_ = __base::__map_.size() == 1 ?
                               __base::__block_size / 2 :
                               __base::__start_ + __base::__block_size;
    }
}

// Create front capacity for __n elements.
// Strong guarantee.  Either do it or don't touch anything.
template <class _Tp, class _Allocator>
void
deque<_Tp, _Allocator>::__add_front_capacity(size_type __n)
{
    allocator_type& __a = __base::__alloc();
    size_type __nb = __recommend_blocks(__n + __base::__map_.empty());
    // Number of unused blocks at back:
    size_type __back_capacity = __back_spare() / __base::__block_size;
    __back_capacity = _VSTD::min(__back_capacity, __nb);  // don't take more than you need
    __nb -= __back_capacity;  // number of blocks need to allocate
    // If __nb == 0, then we have sufficient capacity.
    if (__nb == 0)
    {
        __base::__start_ += __base::__block_size * __back_capacity;
        for (; __back_capacity > 0; --__back_capacity)
        {
            pointer __pt = __base::__map_.back();
            __base::__map_.pop_back();
            __base::__map_.push_front(__pt);
        }
    }
    // Else if __nb <= __map_.capacity() - __map_.size() then we need to allocate __nb buffers
    else if (__nb <= __base::__map_.capacity() - __base::__map_.size())
    {   // we can put the new buffers into the map, but don't shift things around
        // until all buffers are allocated.  If we throw, we don't need to fix
        // anything up (any added buffers are undetectible)
        for (; __nb > 0; --__nb, __base::__start_ += __base::__block_size - (__base::__map_.size() == 1))
        {
            if (__base::__map_.__front_spare() == 0)
                break;
            __base::__map_.push_front(__alloc_traits::allocate(__a, __base::__block_size));
        }
        for (; __nb > 0; --__nb, ++__back_capacity)
            __base::__map_.push_back(__alloc_traits::allocate(__a, __base::__block_size));
        // Done allocating, reorder capacity
        __base::__start_ += __back_capacity * __base::__block_size;
        for (; __back_capacity > 0; --__back_capacity)
        {
            pointer __pt = __base::__map_.back();
            __base::__map_.pop_back();
            __base::__map_.push_front(__pt);
        }
    }
    // Else need to allocate __nb buffers, *and* we need to reallocate __map_.
    else
    {
        size_type __ds = (__nb + __back_capacity) * __base::__block_size - __base::__map_.empty();
        __split_buffer<pointer, typename __base::__pointer_allocator&>
            __buf(max<size_type>(2* __base::__map_.capacity(),
                                 __nb + __base::__map_.size()),
                  0, __base::__map_.__alloc());
#ifndef _LIBCPP_NO_EXCEPTIONS
        try
        {
#endif  // _LIBCPP_NO_EXCEPTIONS
            for (; __nb > 0; --__nb)
                __buf.push_back(__alloc_traits::allocate(__a, __base::__block_size));
#ifndef _LIBCPP_NO_EXCEPTIONS
        }
        catch (...)
        {
            for (typename __base::__map_pointer __i = __buf.begin();
                    __i != __buf.end(); ++__i)
                __alloc_traits::deallocate(__a, *__i, __base::__block_size);
            throw;
        }
#endif  // _LIBCPP_NO_EXCEPTIONS
        for (; __back_capacity > 0; --__back_capacity)
        {
            __buf.push_back(__base::__map_.back());
            __base::__map_.pop_back();
        }
        for (typename __base::__map_pointer __i = __base::__map_.begin();
                __i != __base::__map_.end(); ++__i)
            __buf.push_back(*__i);
        _VSTD::swap(__base::__map_.__first_, __buf.__first_);
        _VSTD::swap(__base::__map_.__begin_, __buf.__begin_);
        _VSTD::swap(__base::__map_.__end_, __buf.__end_);
        _VSTD::swap(__base::__map_.__end_cap(), __buf.__end_cap());
        __base::__start_ += __ds;
    }
}

// Create back capacity for one block of elements.
// Strong guarantee.  Either do it or don't touch anything.
template <class _Tp, class _Allocator>
void
deque<_Tp, _Allocator>::__add_back_capacity()
{
    allocator_type& __a = __base::__alloc();
    if (__front_spare() >= __base::__block_size)
    {
        __base::__start_ -= __base::__block_size;
        pointer __pt = __base::__map_.front();
        __base::__map_.pop_front();
        __base::__map_.push_back(__pt);
    }
    // Else if __nb <= __map_.capacity() - __map_.size() then we need to allocate __nb buffers
    else if (__base::__map_.size() < __base::__map_.capacity())
    {   // we can put the new buffer into the map, but don't shift things around
        // until it is allocated.  If we throw, we don't need to fix
        // anything up (any added buffers are undetectible)
        if (__base::__map_.__back_spare() != 0)
            __base::__map_.push_back(__alloc_traits::allocate(__a, __base::__block_size));
        else
        {
            __base::__map_.push_front(__alloc_traits::allocate(__a, __base::__block_size));
            // Done allocating, reorder capacity
            pointer __pt = __base::__map_.front();
            __base::__map_.pop_front();
            __base::__map_.push_back(__pt);
        }
    }
    // Else need to allocate 1 buffer, *and* we need to reallocate __map_.
    else
    {
        __split_buffer<pointer, typename __base::__pointer_allocator&>
            __buf(max<size_type>(2* __base::__map_.capacity(), 1),
                  __base::__map_.size(),
                  __base::__map_.__alloc());

        typedef __allocator_destructor<_Allocator> _Dp;
        unique_ptr<pointer, _Dp> __hold(
            __alloc_traits::allocate(__a, __base::__block_size),
                _Dp(__a, __base::__block_size));
        __buf.push_back(__hold.get());
        __hold.release();

        for (typename __base::__map_pointer __i = __base::__map_.end();
                __i != __base::__map_.begin();)
            __buf.push_front(*--__i);
        _VSTD::swap(__base::__map_.__first_, __buf.__first_);
        _VSTD::swap(__base::__map_.__begin_, __buf.__begin_);
        _VSTD::swap(__base::__map_.__end_, __buf.__end_);
        _VSTD::swap(__base::__map_.__end_cap(), __buf.__end_cap());
    }
}

// Create back capacity for __n elements.
// Strong guarantee.  Either do it or don't touch anything.
template <class _Tp, class _Allocator>
void
deque<_Tp, _Allocator>::__add_back_capacity(size_type __n)
{
    allocator_type& __a = __base::__alloc();
    size_type __nb = __recommend_blocks(__n + __base::__map_.empty());
    // Number of unused blocks at front:
    size_type __front_capacity = __front_spare() / __base::__block_size;
    __front_capacity = _VSTD::min(__front_capacity, __nb);  // don't take more than you need
    __nb -= __front_capacity;  // number of blocks need to allocate
    // If __nb == 0, then we have sufficient capacity.
    if (__nb == 0)
    {
        __base::__start_ -= __base::__block_size * __front_capacity;
        for (; __front_capacity > 0; --__front_capacity)
        {
            pointer __pt = __base::__map_.front();
            __base::__map_.pop_front();
            __base::__map_.push_back(__pt);
        }
    }
    // Else if __nb <= __map_.capacity() - __map_.size() then we need to allocate __nb buffers
    else if (__nb <= __base::__map_.capacity() - __base::__map_.size())
    {   // we can put the new buffers into the map, but don't shift things around
        // until all buffers are allocated.  If we throw, we don't need to fix
        // anything up (any added buffers are undetectible)
        for (; __nb > 0; --__nb)
        {
            if (__base::__map_.__back_spare() == 0)
                break;
            __base::__map_.push_back(__alloc_traits::allocate(__a, __base::__block_size));
        }
        for (; __nb > 0; --__nb, ++__front_capacity, __base::__start_ +=
                                 __base::__block_size - (__base::__map_.size() == 1))
            __base::__map_.push_front(__alloc_traits::allocate(__a, __base::__block_size));
        // Done allocating, reorder capacity
        __base::__start_ -= __base::__block_size * __front_capacity;
        for (; __front_capacity > 0; --__front_capacity)
        {
            pointer __pt = __base::__map_.front();
            __base::__map_.pop_front();
            __base::__map_.push_back(__pt);
        }
    }
    // Else need to allocate __nb buffers, *and* we need to reallocate __map_.
    else
    {
        size_type __ds = __front_capacity * __base::__block_size;
        __split_buffer<pointer, typename __base::__pointer_allocator&>
            __buf(max<size_type>(2* __base::__map_.capacity(),
                                 __nb + __base::__map_.size()),
                  __base::__map_.size() - __front_capacity,
                  __base::__map_.__alloc());
#ifndef _LIBCPP_NO_EXCEPTIONS
        try
        {
#endif  // _LIBCPP_NO_EXCEPTIONS
            for (; __nb > 0; --__nb)
                __buf.push_back(__alloc_traits::allocate(__a, __base::__block_size));
#ifndef _LIBCPP_NO_EXCEPTIONS
        }
        catch (...)
        {
            for (typename __base::__map_pointer __i = __buf.begin();
                    __i != __buf.end(); ++__i)
                __alloc_traits::deallocate(__a, *__i, __base::__block_size);
            throw;
        }
#endif  // _LIBCPP_NO_EXCEPTIONS
        for (; __front_capacity > 0; --__front_capacity)
        {
            __buf.push_back(__base::__map_.front());
            __base::__map_.pop_front();
        }
        for (typename __base::__map_pointer __i = __base::__map_.end();
                __i != __base::__map_.begin();)
            __buf.push_front(*--__i);
        _VSTD::swap(__base::__map_.__first_, __buf.__first_);
        _VSTD::swap(__base::__map_.__begin_, __buf.__begin_);
        _VSTD::swap(__base::__map_.__end_, __buf.__end_);
        _VSTD::swap(__base::__map_.__end_cap(), __buf.__end_cap());
        __base::__start_ -= __ds;
    }
}

template <class _Tp, class _Allocator>
void
deque<_Tp, _Allocator>::pop_front()
{
    allocator_type& __a = __base::__alloc();
    __alloc_traits::destroy(__a, __to_raw_pointer(*(__base::__map_.begin() +
                                                    __base::__start_ / __base::__block_size) +
                                                    __base::__start_ % __base::__block_size));
    --__base::size();
    if (++__base::__start_ >= 2 * __base::__block_size)
    {
        __alloc_traits::deallocate(__a, __base::__map_.front(), __base::__block_size);
        __base::__map_.pop_front();
        __base::__start_ -= __base::__block_size;
    }
}

template <class _Tp, class _Allocator>
void
deque<_Tp, _Allocator>::pop_back()
{
    allocator_type& __a = __base::__alloc();
    size_type __p = __base::size() + __base::__start_ - 1;
    __alloc_traits::destroy(__a, __to_raw_pointer(*(__base::__map_.begin() +
                                                    __p / __base::__block_size) +
                                                    __p % __base::__block_size));
    --__base::size();
    if (__back_spare() >= 2 * __base::__block_size)
    {
        __alloc_traits::deallocate(__a, __base::__map_.back(), __base::__block_size);
        __base::__map_.pop_back();
    }
}

// move assign [__f, __l) to [__r, __r + (__l-__f)).
// If __vt points into [__f, __l), then subtract (__f - __r) from __vt.
template <class _Tp, class _Allocator>
typename deque<_Tp, _Allocator>::iterator
deque<_Tp, _Allocator>::__move_and_check(iterator __f, iterator __l, iterator __r,
                                         const_pointer& __vt)
{
    // as if
    //   for (; __f != __l; ++__f, ++__r)
    //       *__r = _VSTD::move(*__f);
    difference_type __n = __l - __f;
    while (__n > 0)
    {
        pointer __fb = __f.__ptr_;
        pointer __fe = *__f.__m_iter_ + __base::__block_size;
        difference_type __bs = __fe - __fb;
        if (__bs > __n)
        {
            __bs = __n;
            __fe = __fb + __bs;
        }
        if (__fb <= __vt && __vt < __fe)
            __vt = (const_iterator(static_cast<__map_const_pointer>(__f.__m_iter_), __vt) -= __f - __r).__ptr_;
        __r = _VSTD::move(__fb, __fe, __r);
        __n -= __bs;
        __f += __bs;
    }
    return __r;
}

// move assign [__f, __l) to [__r - (__l-__f), __r) backwards.
// If __vt points into [__f, __l), then add (__r - __l) to __vt.
template <class _Tp, class _Allocator>
typename deque<_Tp, _Allocator>::iterator
deque<_Tp, _Allocator>::__move_backward_and_check(iterator __f, iterator __l, iterator __r,
                                                  const_pointer& __vt)
{
    // as if
    //   while (__f != __l)
    //       *--__r = _VSTD::move(*--__l);
    difference_type __n = __l - __f;
    while (__n > 0)
    {
        --__l;
        pointer __lb = *__l.__m_iter_;
        pointer __le = __l.__ptr_ + 1;
        difference_type __bs = __le - __lb;
        if (__bs > __n)
        {
            __bs = __n;
            __lb = __le - __bs;
        }
        if (__lb <= __vt && __vt < __le)
            __vt = (const_iterator(static_cast<__map_const_pointer>(__l.__m_iter_), __vt) += __r - __l - 1).__ptr_;
        __r = _VSTD::move_backward(__lb, __le, __r);
        __n -= __bs;
        __l -= __bs - 1;
    }
    return __r;
}

// move construct [__f, __l) to [__r, __r + (__l-__f)).
// If __vt points into [__f, __l), then add (__r - __f) to __vt.
template <class _Tp, class _Allocator>
void
deque<_Tp, _Allocator>::__move_construct_and_check(iterator __f, iterator __l,
                                                   iterator __r, const_pointer& __vt)
{
    allocator_type& __a = __base::__alloc();
    // as if
    //   for (; __f != __l; ++__r, ++__f, ++__base::size())
    //       __alloc_traits::construct(__a, _VSTD::addressof(*__r), _VSTD::move(*__f));
    difference_type __n = __l - __f;
    while (__n > 0)
    {
        pointer __fb = __f.__ptr_;
        pointer __fe = *__f.__m_iter_ + __base::__block_size;
        difference_type __bs = __fe - __fb;
        if (__bs > __n)
        {
            __bs = __n;
            __fe = __fb + __bs;
        }
        if (__fb <= __vt && __vt < __fe)
            __vt = (const_iterator(static_cast<__map_const_pointer>(__f.__m_iter_), __vt) += __r - __f).__ptr_;
        for (; __fb != __fe; ++__fb, ++__r, ++__base::size())
            __alloc_traits::construct(__a, _VSTD::addressof(*__r), _VSTD::move(*__fb));
        __n -= __bs;
        __f += __bs;
    }
}

// move construct [__f, __l) to [__r - (__l-__f), __r) backwards.
// If __vt points into [__f, __l), then subtract (__l - __r) from __vt.
template <class _Tp, class _Allocator>
void
deque<_Tp, _Allocator>::__move_construct_backward_and_check(iterator __f, iterator __l,
                                                            iterator __r, const_pointer& __vt)
{
    allocator_type& __a = __base::__alloc();
    // as if
    //   for (iterator __j = __l; __j != __f;)
    //   {
    //       __alloc_traitsconstruct(__a, _VSTD::addressof(*--__r), _VSTD::move(*--__j));
    //       --__base::__start_;
    //       ++__base::size();
    //   }
    difference_type __n = __l - __f;
    while (__n > 0)
    {
        --__l;
        pointer __lb = *__l.__m_iter_;
        pointer __le = __l.__ptr_ + 1;
        difference_type __bs = __le - __lb;
        if (__bs > __n)
        {
            __bs = __n;
            __lb = __le - __bs;
        }
        if (__lb <= __vt && __vt < __le)
            __vt = (const_iterator(static_cast<__map_const_pointer>(__l.__m_iter_), __vt) -= __l - __r + 1).__ptr_;
        while (__le != __lb)
        {
            __alloc_traits::construct(__a, _VSTD::addressof(*--__r), _VSTD::move(*--__le));
            --__base::__start_;
            ++__base::size();
        }
        __n -= __bs;
        __l -= __bs - 1;
    }
}

template <class _Tp, class _Allocator>
typename deque<_Tp, _Allocator>::iterator
deque<_Tp, _Allocator>::erase(const_iterator __f)
{
    iterator __b = __base::begin();
    difference_type __pos = __f - __b;
    iterator __p = __b + __pos;
    allocator_type& __a = __base::__alloc();
    if (static_cast<size_t>(__pos) <= (__base::size() - 1) / 2)
    {   // erase from front
        _VSTD::move_backward(__b, __p, _VSTD::next(__p));
        __alloc_traits::destroy(__a, _VSTD::addressof(*__b));
        --__base::size();
        ++__base::__start_;
        if (__front_spare() >= 2 * __base::__block_size)
        {
            __alloc_traits::deallocate(__a, __base::__map_.front(), __base::__block_size);
            __base::__map_.pop_front();
            __base::__start_ -= __base::__block_size;
        }
    }
    else
    {   // erase from back
        iterator __i = _VSTD::move(_VSTD::next(__p), __base::end(), __p);
        __alloc_traits::destroy(__a, _VSTD::addressof(*__i));
        --__base::size();
        if (__back_spare() >= 2 * __base::__block_size)
        {
            __alloc_traits::deallocate(__a, __base::__map_.back(), __base::__block_size);
            __base::__map_.pop_back();
        }
    }
    return __base::begin() + __pos;
}

template <class _Tp, class _Allocator>
typename deque<_Tp, _Allocator>::iterator
deque<_Tp, _Allocator>::erase(const_iterator __f, const_iterator __l)
{
    difference_type __n = __l - __f;
    iterator __b = __base::begin();
    difference_type __pos = __f - __b;
    iterator __p = __b + __pos;
    if (__n > 0)
    {
        allocator_type& __a = __base::__alloc();
        if (static_cast<size_t>(__pos) <= (__base::size() - __n) / 2)
        {   // erase from front
            iterator __i = _VSTD::move_backward(__b, __p, __p + __n);
            for (; __b != __i; ++__b)
                __alloc_traits::destroy(__a, _VSTD::addressof(*__b));
            __base::size() -= __n;
            __base::__start_ += __n;
            while (__front_spare() >= 2 * __base::__block_size)
            {
                __alloc_traits::deallocate(__a, __base::__map_.front(), __base::__block_size);
                __base::__map_.pop_front();
                __base::__start_ -= __base::__block_size;
            }
        }
        else
        {   // erase from back
            iterator __i = _VSTD::move(__p + __n, __base::end(), __p);
            for (iterator __e = __base::end(); __i != __e; ++__i)
                __alloc_traits::destroy(__a, _VSTD::addressof(*__i));
            __base::size() -= __n;
            while (__back_spare() >= 2 * __base::__block_size)
            {
                __alloc_traits::deallocate(__a, __base::__map_.back(), __base::__block_size);
                __base::__map_.pop_back();
            }
        }
    }
    return __base::begin() + __pos;
}

template <class _Tp, class _Allocator>
void
deque<_Tp, _Allocator>::__erase_to_end(const_iterator __f)
{
    iterator __e = __base::end();
    difference_type __n = __e - __f;
    if (__n > 0)
    {
        allocator_type& __a = __base::__alloc();
        iterator __b = __base::begin();
        difference_type __pos = __f - __b;
        for (iterator __p = __b + __pos; __p != __e; ++__p)
            __alloc_traits::destroy(__a, _VSTD::addressof(*__p));
        __base::size() -= __n;
        while (__back_spare() >= 2 * __base::__block_size)
        {
            __alloc_traits::deallocate(__a, __base::__map_.back(), __base::__block_size);
            __base::__map_.pop_back();
        }
    }
}

template <class _Tp, class _Allocator>
inline
void
deque<_Tp, _Allocator>::swap(deque& __c)
#if _LIBCPP_STD_VER >= 14
        _NOEXCEPT
#else
        _NOEXCEPT_(!__alloc_traits::propagate_on_container_swap::value ||
                    __is_nothrow_swappable<allocator_type>::value)
#endif
{
    __base::swap(__c);
}

template <class _Tp, class _Allocator>
inline
void
deque<_Tp, _Allocator>::clear() _NOEXCEPT
{
    __base::clear();
}

template <class _Tp, class _Allocator>
inline _LIBCPP_INLINE_VISIBILITY
bool
operator==(const deque<_Tp, _Allocator>& __x, const deque<_Tp, _Allocator>& __y)
{
    const typename deque<_Tp, _Allocator>::size_type __sz = __x.size();
    return __sz == __y.size() && _VSTD::equal(__x.begin(), __x.end(), __y.begin());
}

template <class _Tp, class _Allocator>
inline _LIBCPP_INLINE_VISIBILITY
bool
operator!=(const deque<_Tp, _Allocator>& __x, const deque<_Tp, _Allocator>& __y)
{
    return !(__x == __y);
}

template <class _Tp, class _Allocator>
inline _LIBCPP_INLINE_VISIBILITY
bool
operator< (const deque<_Tp, _Allocator>& __x, const deque<_Tp, _Allocator>& __y)
{
    return _VSTD::lexicographical_compare(__x.begin(), __x.end(), __y.begin(), __y.end());
}

template <class _Tp, class _Allocator>
inline _LIBCPP_INLINE_VISIBILITY
bool
operator> (const deque<_Tp, _Allocator>& __x, const deque<_Tp, _Allocator>& __y)
{
    return __y < __x;
}

template <class _Tp, class _Allocator>
inline _LIBCPP_INLINE_VISIBILITY
bool
operator>=(const deque<_Tp, _Allocator>& __x, const deque<_Tp, _Allocator>& __y)
{
    return !(__x < __y);
}

template <class _Tp, class _Allocator>
inline _LIBCPP_INLINE_VISIBILITY
bool
operator<=(const deque<_Tp, _Allocator>& __x, const deque<_Tp, _Allocator>& __y)
{
    return !(__y < __x);
}

template <class _Tp, class _Allocator>
inline _LIBCPP_INLINE_VISIBILITY
void
swap(deque<_Tp, _Allocator>& __x, deque<_Tp, _Allocator>& __y)
    _NOEXCEPT_(_NOEXCEPT_(__x.swap(__y)))
{
    __x.swap(__y);
}

_LIBCPP_END_NAMESPACE_STD

_LIBCPP_POP_MACROS

#endif  // _LIBCPP_DEQUE
