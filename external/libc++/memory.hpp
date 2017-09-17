// -*- C++ -*-
//===-------------------------- memory ------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is dual licensed under the MIT and the University of Illinois Open
// Source Licenses. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef _LIBCPP_MEMORY
#define _LIBCPP_MEMORY

#include "__config.hpp"
#include "type_traits.hpp"
#include "typeinfo.hpp"
#include <stddef.h>
#include <stdint.h>
#include "new.hpp"
#include "utility.hpp"
#include "limits.hpp"
#include "iterator.hpp"
#include "__functional_base.hpp"
#include "iosfwd.hpp"
#include "tuple.hpp"
#include "stdexcept.hpp"
#include <string.h>
#include <assert.h>
#if !defined(_LIBCPP_HAS_NO_ATOMIC_HEADER)
#  include <atomic>
#endif

#if !defined(_LIBCPP_HAS_NO_PRAGMA_SYSTEM_HEADER)
#pragma GCC system_header
#endif

_LIBCPP_PUSH_MACROS
#include "__undef_macros.hpp"


_LIBCPP_BEGIN_NAMESPACE_STD

template <class _ValueType>
inline _LIBCPP_ALWAYS_INLINE
_ValueType __libcpp_relaxed_load(_ValueType const* __value) {
#if !defined(_LIBCPP_HAS_NO_THREADS) && \
    defined(__ATOMIC_RELAXED) &&        \
    (__has_builtin(__atomic_load_n) || _GNUC_VER >= 407)
    return __atomic_load_n(__value, __ATOMIC_RELAXED);
#else
    return *__value;
#endif
}

template <class _ValueType>
inline _LIBCPP_ALWAYS_INLINE
_ValueType __libcpp_acquire_load(_ValueType const* __value) {
#if !defined(_LIBCPP_HAS_NO_THREADS) && \
    defined(__ATOMIC_ACQUIRE) &&        \
    (__has_builtin(__atomic_load_n) || _GNUC_VER >= 407)
    return __atomic_load_n(__value, __ATOMIC_ACQUIRE);
#else
    return *__value;
#endif
}

// addressof moved to <type_traits>

template <class _Tp> class allocator;

template <>
class _LIBCPP_TEMPLATE_VIS allocator<void>
{
public:
    typedef void*             pointer;
    typedef const void*       const_pointer;
    typedef void              value_type;

    template <class _Up> struct rebind {typedef allocator<_Up> other;};
};

template <>
class _LIBCPP_TEMPLATE_VIS allocator<const void>
{
public:
    typedef const void*       pointer;
    typedef const void*       const_pointer;
    typedef const void        value_type;

    template <class _Up> struct rebind {typedef allocator<_Up> other;};
};

// pointer_traits

template <class _Tp, class = void>
struct __has_element_type : false_type {};

template <class _Tp>
struct __has_element_type<_Tp,
              typename __void_t<typename _Tp::element_type>::type> : true_type {};

template <class _Ptr, bool = __has_element_type<_Ptr>::value>
struct __pointer_traits_element_type;

template <class _Ptr>
struct __pointer_traits_element_type<_Ptr, true>
{
    typedef typename _Ptr::element_type type;
};

#ifndef _LIBCPP_HAS_NO_VARIADICS

template <template <class, class...> class _Sp, class _Tp, class ..._Args>
struct __pointer_traits_element_type<_Sp<_Tp, _Args...>, true>
{
    typedef typename _Sp<_Tp, _Args...>::element_type type;
};

template <template <class, class...> class _Sp, class _Tp, class ..._Args>
struct __pointer_traits_element_type<_Sp<_Tp, _Args...>, false>
{
    typedef _Tp type;
};

#else  // _LIBCPP_HAS_NO_VARIADICS

template <template <class> class _Sp, class _Tp>
struct __pointer_traits_element_type<_Sp<_Tp>, true>
{
    typedef typename _Sp<_Tp>::element_type type;
};

template <template <class> class _Sp, class _Tp>
struct __pointer_traits_element_type<_Sp<_Tp>, false>
{
    typedef _Tp type;
};

template <template <class, class> class _Sp, class _Tp, class _A0>
struct __pointer_traits_element_type<_Sp<_Tp, _A0>, true>
{
    typedef typename _Sp<_Tp, _A0>::element_type type;
};

template <template <class, class> class _Sp, class _Tp, class _A0>
struct __pointer_traits_element_type<_Sp<_Tp, _A0>, false>
{
    typedef _Tp type;
};

template <template <class, class, class> class _Sp, class _Tp, class _A0, class _A1>
struct __pointer_traits_element_type<_Sp<_Tp, _A0, _A1>, true>
{
    typedef typename _Sp<_Tp, _A0, _A1>::element_type type;
};

template <template <class, class, class> class _Sp, class _Tp, class _A0, class _A1>
struct __pointer_traits_element_type<_Sp<_Tp, _A0, _A1>, false>
{
    typedef _Tp type;
};

template <template <class, class, class, class> class _Sp, class _Tp, class _A0,
                                                           class _A1, class _A2>
struct __pointer_traits_element_type<_Sp<_Tp, _A0, _A1, _A2>, true>
{
    typedef typename _Sp<_Tp, _A0, _A1, _A2>::element_type type;
};

template <template <class, class, class, class> class _Sp, class _Tp, class _A0,
                                                           class _A1, class _A2>
struct __pointer_traits_element_type<_Sp<_Tp, _A0, _A1, _A2>, false>
{
    typedef _Tp type;
};

#endif  // _LIBCPP_HAS_NO_VARIADICS

template <class _Tp, class = void>
struct __has_difference_type : false_type {};

template <class _Tp>
struct __has_difference_type<_Tp,
            typename __void_t<typename _Tp::difference_type>::type> : true_type {};

template <class _Ptr, bool = __has_difference_type<_Ptr>::value>
struct __pointer_traits_difference_type
{
    typedef ptrdiff_t type;
};

template <class _Ptr>
struct __pointer_traits_difference_type<_Ptr, true>
{
    typedef typename _Ptr::difference_type type;
};

template <class _Tp, class _Up>
struct __has_rebind
{
private:
    struct __two {char __lx; char __lxx;};
    template <class _Xp> static __two __test(...);
    template <class _Xp> static char __test(typename _Xp::template rebind<_Up>* = 0);
public:
    static const bool value = sizeof(__test<_Tp>(0)) == 1;
};

template <class _Tp, class _Up, bool = __has_rebind<_Tp, _Up>::value>
struct __pointer_traits_rebind
{
#ifndef _LIBCPP_CXX03_LANG
    typedef typename _Tp::template rebind<_Up> type;
#else
    typedef typename _Tp::template rebind<_Up>::other type;
#endif
};

#ifndef _LIBCPP_HAS_NO_VARIADICS

template <template <class, class...> class _Sp, class _Tp, class ..._Args, class _Up>
struct __pointer_traits_rebind<_Sp<_Tp, _Args...>, _Up, true>
{
#ifndef _LIBCPP_CXX03_LANG
    typedef typename _Sp<_Tp, _Args...>::template rebind<_Up> type;
#else
    typedef typename _Sp<_Tp, _Args...>::template rebind<_Up>::other type;
#endif
};

template <template <class, class...> class _Sp, class _Tp, class ..._Args, class _Up>
struct __pointer_traits_rebind<_Sp<_Tp, _Args...>, _Up, false>
{
    typedef _Sp<_Up, _Args...> type;
};

#else  // _LIBCPP_HAS_NO_VARIADICS

template <template <class> class _Sp, class _Tp, class _Up>
struct __pointer_traits_rebind<_Sp<_Tp>, _Up, true>
{
#ifndef _LIBCPP_CXX03_LANG
    typedef typename _Sp<_Tp>::template rebind<_Up> type;
#else
    typedef typename _Sp<_Tp>::template rebind<_Up>::other type;
#endif
};

template <template <class> class _Sp, class _Tp, class _Up>
struct __pointer_traits_rebind<_Sp<_Tp>, _Up, false>
{
    typedef _Sp<_Up> type;
};

template <template <class, class> class _Sp, class _Tp, class _A0, class _Up>
struct __pointer_traits_rebind<_Sp<_Tp, _A0>, _Up, true>
{
#ifndef _LIBCPP_CXX03_LANG
    typedef typename _Sp<_Tp, _A0>::template rebind<_Up> type;
#else
    typedef typename _Sp<_Tp, _A0>::template rebind<_Up>::other type;
#endif
};

template <template <class, class> class _Sp, class _Tp, class _A0, class _Up>
struct __pointer_traits_rebind<_Sp<_Tp, _A0>, _Up, false>
{
    typedef _Sp<_Up, _A0> type;
};

template <template <class, class, class> class _Sp, class _Tp, class _A0,
                                         class _A1, class _Up>
struct __pointer_traits_rebind<_Sp<_Tp, _A0, _A1>, _Up, true>
{
#ifndef _LIBCPP_CXX03_LANG
    typedef typename _Sp<_Tp, _A0, _A1>::template rebind<_Up> type;
#else
    typedef typename _Sp<_Tp, _A0, _A1>::template rebind<_Up>::other type;
#endif
};

template <template <class, class, class> class _Sp, class _Tp, class _A0,
                                         class _A1, class _Up>
struct __pointer_traits_rebind<_Sp<_Tp, _A0, _A1>, _Up, false>
{
    typedef _Sp<_Up, _A0, _A1> type;
};

template <template <class, class, class, class> class _Sp, class _Tp, class _A0,
                                                class _A1, class _A2, class _Up>
struct __pointer_traits_rebind<_Sp<_Tp, _A0, _A1, _A2>, _Up, true>
{
#ifndef _LIBCPP_CXX03_LANG
    typedef typename _Sp<_Tp, _A0, _A1, _A2>::template rebind<_Up> type;
#else
    typedef typename _Sp<_Tp, _A0, _A1, _A2>::template rebind<_Up>::other type;
#endif
};

template <template <class, class, class, class> class _Sp, class _Tp, class _A0,
                                                class _A1, class _A2, class _Up>
struct __pointer_traits_rebind<_Sp<_Tp, _A0, _A1, _A2>, _Up, false>
{
    typedef _Sp<_Up, _A0, _A1, _A2> type;
};

#endif  // _LIBCPP_HAS_NO_VARIADICS

template <class _Ptr>
struct _LIBCPP_TEMPLATE_VIS pointer_traits
{
    typedef _Ptr                                                     pointer;
    typedef typename __pointer_traits_element_type<pointer>::type    element_type;
    typedef typename __pointer_traits_difference_type<pointer>::type difference_type;

#ifndef _LIBCPP_CXX03_LANG
    template <class _Up> using rebind = typename __pointer_traits_rebind<pointer, _Up>::type;
#else
    template <class _Up> struct rebind
        {typedef typename __pointer_traits_rebind<pointer, _Up>::type other;};
#endif  // _LIBCPP_CXX03_LANG

private:
    struct __nat {};
public:
    _LIBCPP_INLINE_VISIBILITY
    static pointer pointer_to(typename conditional<is_void<element_type>::value,
                                           __nat, element_type>::type& __r)
        {return pointer::pointer_to(__r);}
};

template <class _Tp>
struct _LIBCPP_TEMPLATE_VIS pointer_traits<_Tp*>
{
    typedef _Tp*      pointer;
    typedef _Tp       element_type;
    typedef ptrdiff_t difference_type;

#ifndef _LIBCPP_CXX03_LANG
    template <class _Up> using rebind = _Up*;
#else
    template <class _Up> struct rebind {typedef _Up* other;};
#endif

private:
    struct __nat {};
public:
    _LIBCPP_INLINE_VISIBILITY
    static pointer pointer_to(typename conditional<is_void<element_type>::value,
                                      __nat, element_type>::type& __r) _NOEXCEPT
        {return _VSTD::addressof(__r);}
};

template <class _From, class _To>
struct __rebind_pointer {
#ifndef _LIBCPP_CXX03_LANG
    typedef typename pointer_traits<_From>::template rebind<_To>        type;
#else
    typedef typename pointer_traits<_From>::template rebind<_To>::other type;
#endif
};

// allocator_traits

template <class _Tp, class = void>
struct __has_pointer_type : false_type {};

template <class _Tp>
struct __has_pointer_type<_Tp,
          typename __void_t<typename _Tp::pointer>::type> : true_type {};

namespace __pointer_type_imp
{

template <class _Tp, class _Dp, bool = __has_pointer_type<_Dp>::value>
struct __pointer_type
{
    typedef typename _Dp::pointer type;
};

template <class _Tp, class _Dp>
struct __pointer_type<_Tp, _Dp, false>
{
    typedef _Tp* type;
};

}  // __pointer_type_imp

template <class _Tp, class _Dp>
struct __pointer_type
{
    typedef typename __pointer_type_imp::__pointer_type<_Tp, typename remove_reference<_Dp>::type>::type type;
};

template <class _Tp, class = void>
struct __has_const_pointer : false_type {};

template <class _Tp>
struct __has_const_pointer<_Tp,
            typename __void_t<typename _Tp::const_pointer>::type> : true_type {};

template <class _Tp, class _Ptr, class _Alloc, bool = __has_const_pointer<_Alloc>::value>
struct __const_pointer
{
    typedef typename _Alloc::const_pointer type;
};

template <class _Tp, class _Ptr, class _Alloc>
struct __const_pointer<_Tp, _Ptr, _Alloc, false>
{
#ifndef _LIBCPP_CXX03_LANG
    typedef typename pointer_traits<_Ptr>::template rebind<const _Tp> type;
#else
    typedef typename pointer_traits<_Ptr>::template rebind<const _Tp>::other type;
#endif
};

template <class _Tp, class = void>
struct __has_void_pointer : false_type {};

template <class _Tp>
struct __has_void_pointer<_Tp,
               typename __void_t<typename _Tp::void_pointer>::type> : true_type {};

template <class _Ptr, class _Alloc, bool = __has_void_pointer<_Alloc>::value>
struct __void_pointer
{
    typedef typename _Alloc::void_pointer type;
};

template <class _Ptr, class _Alloc>
struct __void_pointer<_Ptr, _Alloc, false>
{
#ifndef _LIBCPP_CXX03_LANG
    typedef typename pointer_traits<_Ptr>::template rebind<void> type;
#else
    typedef typename pointer_traits<_Ptr>::template rebind<void>::other type;
#endif
};

template <class _Tp, class = void>
struct __has_const_void_pointer : false_type {};

template <class _Tp>
struct __has_const_void_pointer<_Tp,
            typename __void_t<typename _Tp::const_void_pointer>::type> : true_type {};

template <class _Ptr, class _Alloc, bool = __has_const_void_pointer<_Alloc>::value>
struct __const_void_pointer
{
    typedef typename _Alloc::const_void_pointer type;
};

template <class _Ptr, class _Alloc>
struct __const_void_pointer<_Ptr, _Alloc, false>
{
#ifndef _LIBCPP_CXX03_LANG
    typedef typename pointer_traits<_Ptr>::template rebind<const void> type;
#else
    typedef typename pointer_traits<_Ptr>::template rebind<const void>::other type;
#endif
};

template <class _Tp>
inline _LIBCPP_INLINE_VISIBILITY
_Tp*
__to_raw_pointer(_Tp* __p) _NOEXCEPT
{
    return __p;
}

template <class _Pointer>
inline _LIBCPP_INLINE_VISIBILITY
typename pointer_traits<_Pointer>::element_type*
__to_raw_pointer(_Pointer __p) _NOEXCEPT
{
    return _VSTD::__to_raw_pointer(__p.operator->());
}

template <class _Tp, class = void>
struct __has_size_type : false_type {};

template <class _Tp>
struct __has_size_type<_Tp,
               typename __void_t<typename _Tp::size_type>::type> : true_type {};

template <class _Alloc, class _DiffType, bool = __has_size_type<_Alloc>::value>
struct __size_type
{
    typedef typename make_unsigned<_DiffType>::type type;
};

template <class _Alloc, class _DiffType>
struct __size_type<_Alloc, _DiffType, true>
{
    typedef typename _Alloc::size_type type;
};

template <class _Tp, class = void>
struct __has_propagate_on_container_copy_assignment : false_type {};

template <class _Tp>
struct __has_propagate_on_container_copy_assignment<_Tp,
    typename __void_t<typename _Tp::propagate_on_container_copy_assignment>::type>
        : true_type {};

template <class _Alloc, bool = __has_propagate_on_container_copy_assignment<_Alloc>::value>
struct __propagate_on_container_copy_assignment
{
    typedef false_type type;
};

template <class _Alloc>
struct __propagate_on_container_copy_assignment<_Alloc, true>
{
    typedef typename _Alloc::propagate_on_container_copy_assignment type;
};

template <class _Tp, class = void>
struct __has_propagate_on_container_move_assignment : false_type {};

template <class _Tp>
struct __has_propagate_on_container_move_assignment<_Tp,
           typename __void_t<typename _Tp::propagate_on_container_move_assignment>::type>
               : true_type {};

template <class _Alloc, bool = __has_propagate_on_container_move_assignment<_Alloc>::value>
struct __propagate_on_container_move_assignment
{
    typedef false_type type;
};

template <class _Alloc>
struct __propagate_on_container_move_assignment<_Alloc, true>
{
    typedef typename _Alloc::propagate_on_container_move_assignment type;
};

template <class _Tp, class = void>
struct __has_propagate_on_container_swap : false_type {};

template <class _Tp>
struct __has_propagate_on_container_swap<_Tp,
           typename __void_t<typename _Tp::propagate_on_container_swap>::type>
               : true_type {};

template <class _Alloc, bool = __has_propagate_on_container_swap<_Alloc>::value>
struct __propagate_on_container_swap
{
    typedef false_type type;
};

template <class _Alloc>
struct __propagate_on_container_swap<_Alloc, true>
{
    typedef typename _Alloc::propagate_on_container_swap type;
};

template <class _Tp, class = void>
struct __has_is_always_equal : false_type {};

template <class _Tp>
struct __has_is_always_equal<_Tp,
           typename __void_t<typename _Tp::is_always_equal>::type>
               : true_type {};

template <class _Alloc, bool = __has_is_always_equal<_Alloc>::value>
struct __is_always_equal
{
    typedef typename _VSTD::is_empty<_Alloc>::type type;
};

template <class _Alloc>
struct __is_always_equal<_Alloc, true>
{
    typedef typename _Alloc::is_always_equal type;
};

template <class _Tp, class _Up, bool = __has_rebind<_Tp, _Up>::value>
struct __has_rebind_other
{
private:
    struct __two {char __lx; char __lxx;};
    template <class _Xp> static __two __test(...);
    template <class _Xp> static char __test(typename _Xp::template rebind<_Up>::other* = 0);
public:
    static const bool value = sizeof(__test<_Tp>(0)) == 1;
};

template <class _Tp, class _Up>
struct __has_rebind_other<_Tp, _Up, false>
{
    static const bool value = false;
};

template <class _Tp, class _Up, bool = __has_rebind_other<_Tp, _Up>::value>
struct __allocator_traits_rebind
{
    typedef typename _Tp::template rebind<_Up>::other type;
};

#ifndef _LIBCPP_HAS_NO_VARIADICS

template <template <class, class...> class _Alloc, class _Tp, class ..._Args, class _Up>
struct __allocator_traits_rebind<_Alloc<_Tp, _Args...>, _Up, true>
{
    typedef typename _Alloc<_Tp, _Args...>::template rebind<_Up>::other type;
};

template <template <class, class...> class _Alloc, class _Tp, class ..._Args, class _Up>
struct __allocator_traits_rebind<_Alloc<_Tp, _Args...>, _Up, false>
{
    typedef _Alloc<_Up, _Args...> type;
};

#else  // _LIBCPP_HAS_NO_VARIADICS

template <template <class> class _Alloc, class _Tp, class _Up>
struct __allocator_traits_rebind<_Alloc<_Tp>, _Up, true>
{
    typedef typename _Alloc<_Tp>::template rebind<_Up>::other type;
};

template <template <class> class _Alloc, class _Tp, class _Up>
struct __allocator_traits_rebind<_Alloc<_Tp>, _Up, false>
{
    typedef _Alloc<_Up> type;
};

template <template <class, class> class _Alloc, class _Tp, class _A0, class _Up>
struct __allocator_traits_rebind<_Alloc<_Tp, _A0>, _Up, true>
{
    typedef typename _Alloc<_Tp, _A0>::template rebind<_Up>::other type;
};

template <template <class, class> class _Alloc, class _Tp, class _A0, class _Up>
struct __allocator_traits_rebind<_Alloc<_Tp, _A0>, _Up, false>
{
    typedef _Alloc<_Up, _A0> type;
};

template <template <class, class, class> class _Alloc, class _Tp, class _A0,
                                         class _A1, class _Up>
struct __allocator_traits_rebind<_Alloc<_Tp, _A0, _A1>, _Up, true>
{
    typedef typename _Alloc<_Tp, _A0, _A1>::template rebind<_Up>::other type;
};

template <template <class, class, class> class _Alloc, class _Tp, class _A0,
                                         class _A1, class _Up>
struct __allocator_traits_rebind<_Alloc<_Tp, _A0, _A1>, _Up, false>
{
    typedef _Alloc<_Up, _A0, _A1> type;
};

template <template <class, class, class, class> class _Alloc, class _Tp, class _A0,
                                                class _A1, class _A2, class _Up>
struct __allocator_traits_rebind<_Alloc<_Tp, _A0, _A1, _A2>, _Up, true>
{
    typedef typename _Alloc<_Tp, _A0, _A1, _A2>::template rebind<_Up>::other type;
};

template <template <class, class, class, class> class _Alloc, class _Tp, class _A0,
                                                class _A1, class _A2, class _Up>
struct __allocator_traits_rebind<_Alloc<_Tp, _A0, _A1, _A2>, _Up, false>
{
    typedef _Alloc<_Up, _A0, _A1, _A2> type;
};

#endif  // _LIBCPP_HAS_NO_VARIADICS

#ifndef _LIBCPP_CXX03_LANG

template <class _Alloc, class _SizeType, class _ConstVoidPtr>
auto
__has_allocate_hint_test(_Alloc&& __a, _SizeType&& __sz, _ConstVoidPtr&& __p)
    -> decltype((void)__a.allocate(__sz, __p), true_type());

template <class _Alloc, class _SizeType, class _ConstVoidPtr>
auto
__has_allocate_hint_test(const _Alloc& __a, _SizeType&& __sz, _ConstVoidPtr&& __p)
    -> false_type;

template <class _Alloc, class _SizeType, class _ConstVoidPtr>
struct __has_allocate_hint
    : integral_constant<bool,
        is_same<
            decltype(_VSTD::__has_allocate_hint_test(declval<_Alloc>(),
                                          declval<_SizeType>(),
                                          declval<_ConstVoidPtr>())),
            true_type>::value>
{
};

#else  // _LIBCPP_CXX03_LANG

template <class _Alloc, class _SizeType, class _ConstVoidPtr>
struct __has_allocate_hint
    : true_type
{
};

#endif  // _LIBCPP_CXX03_LANG

#if !defined(_LIBCPP_CXX03_LANG)

template <class _Alloc, class _Tp, class ..._Args>
decltype(_VSTD::declval<_Alloc>().construct(_VSTD::declval<_Tp*>(),
                                           _VSTD::declval<_Args>()...),
                                           true_type())
__has_construct_test(_Alloc&& __a, _Tp* __p, _Args&& ...__args);

template <class _Alloc, class _Pointer, class ..._Args>
false_type
__has_construct_test(const _Alloc& __a, _Pointer&& __p, _Args&& ...__args);

template <class _Alloc, class _Pointer, class ..._Args>
struct __has_construct
    : integral_constant<bool,
        is_same<
            decltype(_VSTD::__has_construct_test(declval<_Alloc>(),
                                          declval<_Pointer>(),
                                          declval<_Args>()...)),
            true_type>::value>
{
};

template <class _Alloc, class _Pointer>
auto
__has_destroy_test(_Alloc&& __a, _Pointer&& __p)
    -> decltype(__a.destroy(__p), true_type());

template <class _Alloc, class _Pointer>
auto
__has_destroy_test(const _Alloc& __a, _Pointer&& __p)
    -> false_type;

template <class _Alloc, class _Pointer>
struct __has_destroy
    : integral_constant<bool,
        is_same<
            decltype(_VSTD::__has_destroy_test(declval<_Alloc>(),
                                        declval<_Pointer>())),
            true_type>::value>
{
};

template <class _Alloc>
auto
__has_max_size_test(_Alloc&& __a)
    -> decltype(__a.max_size(), true_type());

template <class _Alloc>
auto
__has_max_size_test(const volatile _Alloc& __a)
    -> false_type;

template <class _Alloc>
struct __has_max_size
    : integral_constant<bool,
        is_same<
            decltype(_VSTD::__has_max_size_test(declval<_Alloc&>())),
            true_type>::value>
{
};

template <class _Alloc>
auto
__has_select_on_container_copy_construction_test(_Alloc&& __a)
    -> decltype(__a.select_on_container_copy_construction(), true_type());

template <class _Alloc>
auto
__has_select_on_container_copy_construction_test(const volatile _Alloc& __a)
    -> false_type;

template <class _Alloc>
struct __has_select_on_container_copy_construction
    : integral_constant<bool,
        is_same<
            decltype(_VSTD::__has_select_on_container_copy_construction_test(declval<_Alloc&>())),
            true_type>::value>
{
};

#else  // _LIBCPP_CXX03_LANG

#ifndef _LIBCPP_HAS_NO_VARIADICS

template <class _Alloc, class _Pointer, class ..._Args>
struct __has_construct
    : false_type
{
};

#else  // _LIBCPP_HAS_NO_VARIADICS

template <class _Alloc, class _Pointer, class _Args>
struct __has_construct
    : false_type
{
};

#endif  // _LIBCPP_HAS_NO_VARIADICS

template <class _Alloc, class _Pointer>
struct __has_destroy
    : false_type
{
};

template <class _Alloc>
struct __has_max_size
    : true_type
{
};

template <class _Alloc>
struct __has_select_on_container_copy_construction
    : false_type
{
};

#endif  // _LIBCPP_CXX03_LANG

template <class _Alloc, class _Ptr, bool = __has_difference_type<_Alloc>::value>
struct __alloc_traits_difference_type
{
    typedef typename pointer_traits<_Ptr>::difference_type type;
};

template <class _Alloc, class _Ptr>
struct __alloc_traits_difference_type<_Alloc, _Ptr, true>
{
    typedef typename _Alloc::difference_type type;
};

template <class _Alloc>
struct _LIBCPP_TEMPLATE_VIS allocator_traits
{
    typedef _Alloc                              allocator_type;
    typedef typename allocator_type::value_type value_type;

    typedef typename __pointer_type<value_type, allocator_type>::type pointer;
    typedef typename __const_pointer<value_type, pointer, allocator_type>::type const_pointer;
    typedef typename __void_pointer<pointer, allocator_type>::type void_pointer;
    typedef typename __const_void_pointer<pointer, allocator_type>::type const_void_pointer;

    typedef typename __alloc_traits_difference_type<allocator_type, pointer>::type difference_type;
    typedef typename __size_type<allocator_type, difference_type>::type size_type;

    typedef typename __propagate_on_container_copy_assignment<allocator_type>::type
                     propagate_on_container_copy_assignment;
    typedef typename __propagate_on_container_move_assignment<allocator_type>::type
                     propagate_on_container_move_assignment;
    typedef typename __propagate_on_container_swap<allocator_type>::type
                     propagate_on_container_swap;
    typedef typename __is_always_equal<allocator_type>::type
                     is_always_equal;

#ifndef _LIBCPP_CXX03_LANG
    template <class _Tp> using rebind_alloc =
                  typename __allocator_traits_rebind<allocator_type, _Tp>::type;
    template <class _Tp> using rebind_traits = allocator_traits<rebind_alloc<_Tp>>;
#else  // _LIBCPP_CXX03_LANG
    template <class _Tp> struct rebind_alloc
        {typedef typename __allocator_traits_rebind<allocator_type, _Tp>::type other;};
    template <class _Tp> struct rebind_traits
        {typedef allocator_traits<typename rebind_alloc<_Tp>::other> other;};
#endif  // _LIBCPP_CXX03_LANG

    _LIBCPP_INLINE_VISIBILITY
    static pointer allocate(allocator_type& __a, size_type __n)
        {return __a.allocate(__n);}
    _LIBCPP_INLINE_VISIBILITY
    static pointer allocate(allocator_type& __a, size_type __n, const_void_pointer __hint)
        {return __allocate(__a, __n, __hint,
            __has_allocate_hint<allocator_type, size_type, const_void_pointer>());}

    _LIBCPP_INLINE_VISIBILITY
    static void deallocate(allocator_type& __a, pointer __p, size_type __n) _NOEXCEPT
        {__a.deallocate(__p, __n);}

#ifndef _LIBCPP_HAS_NO_VARIADICS
    template <class _Tp, class... _Args>
        _LIBCPP_INLINE_VISIBILITY
        static void construct(allocator_type& __a, _Tp* __p, _Args&&... __args)
            {__construct(__has_construct<allocator_type, _Tp*, _Args...>(),
                         __a, __p, _VSTD::forward<_Args>(__args)...);}
#else  // _LIBCPP_HAS_NO_VARIADICS
    template <class _Tp>
        _LIBCPP_INLINE_VISIBILITY
        static void construct(allocator_type&, _Tp* __p)
            {
                ::new ((void*)__p) _Tp();
            }
    template <class _Tp, class _A0>
        _LIBCPP_INLINE_VISIBILITY
        static void construct(allocator_type&, _Tp* __p, const _A0& __a0)
            {
                ::new ((void*)__p) _Tp(__a0);
            }
    template <class _Tp, class _A0, class _A1>
        _LIBCPP_INLINE_VISIBILITY
        static void construct(allocator_type&, _Tp* __p, const _A0& __a0,
                              const _A1& __a1)
            {
                ::new ((void*)__p) _Tp(__a0, __a1);
            }
    template <class _Tp, class _A0, class _A1, class _A2>
        _LIBCPP_INLINE_VISIBILITY
        static void construct(allocator_type&, _Tp* __p, const _A0& __a0,
                              const _A1& __a1, const _A2& __a2)
            {
                ::new ((void*)__p) _Tp(__a0, __a1, __a2);
            }
#endif  // _LIBCPP_HAS_NO_VARIADICS

    template <class _Tp>
        _LIBCPP_INLINE_VISIBILITY
        static void destroy(allocator_type& __a, _Tp* __p)
            {__destroy(__has_destroy<allocator_type, _Tp*>(), __a, __p);}

    _LIBCPP_INLINE_VISIBILITY
    static size_type max_size(const allocator_type& __a) _NOEXCEPT
        {return __max_size(__has_max_size<const allocator_type>(), __a);}

    _LIBCPP_INLINE_VISIBILITY
    static allocator_type
        select_on_container_copy_construction(const allocator_type& __a)
            {return __select_on_container_copy_construction(
                __has_select_on_container_copy_construction<const allocator_type>(),
                __a);}

    template <class _Ptr>
        _LIBCPP_INLINE_VISIBILITY
        static
        void
        __construct_forward(allocator_type& __a, _Ptr __begin1, _Ptr __end1, _Ptr& __begin2)
        {
            for (; __begin1 != __end1; ++__begin1, ++__begin2)
                construct(__a, _VSTD::__to_raw_pointer(__begin2), _VSTD::move_if_noexcept(*__begin1));
        }

    template <class _Tp>
        _LIBCPP_INLINE_VISIBILITY
        static
        typename enable_if
        <
            (is_same<allocator_type, allocator<_Tp> >::value
                || !__has_construct<allocator_type, _Tp*, _Tp>::value) &&
             is_trivially_move_constructible<_Tp>::value,
            void
        >::type
        __construct_forward(allocator_type&, _Tp* __begin1, _Tp* __end1, _Tp*& __begin2)
        {
            ptrdiff_t _Np = __end1 - __begin1;
            if (_Np > 0)
            {
                memcpy(__begin2, __begin1, _Np * sizeof(_Tp));
                __begin2 += _Np;
            }
        }

    template <class _Iter, class _Ptr>
        _LIBCPP_INLINE_VISIBILITY
        static
        void
        __construct_range_forward(allocator_type& __a, _Iter __begin1, _Iter __end1, _Ptr& __begin2)
        {
            for (; __begin1 != __end1; ++__begin1, (void) ++__begin2)
                construct(__a, _VSTD::__to_raw_pointer(__begin2), *__begin1);
        }

    template <class _Tp>
        _LIBCPP_INLINE_VISIBILITY
        static
        typename enable_if
        <
            (is_same<allocator_type, allocator<_Tp> >::value
                || !__has_construct<allocator_type, _Tp*, _Tp>::value) &&
             is_trivially_move_constructible<_Tp>::value,
            void
        >::type
        __construct_range_forward(allocator_type&, _Tp* __begin1, _Tp* __end1, _Tp*& __begin2)
        {
            typedef typename remove_const<_Tp>::type _Vp;
            ptrdiff_t _Np = __end1 - __begin1;
            if (_Np > 0)
            {
                memcpy(const_cast<_Vp*>(__begin2), __begin1, _Np * sizeof(_Tp));
                __begin2 += _Np;
            }
        }

    template <class _Ptr>
        _LIBCPP_INLINE_VISIBILITY
        static
        void
        __construct_backward(allocator_type& __a, _Ptr __begin1, _Ptr __end1, _Ptr& __end2)
        {
            while (__end1 != __begin1)
            {
                construct(__a, _VSTD::__to_raw_pointer(__end2-1), _VSTD::move_if_noexcept(*--__end1));
                --__end2;
            }
        }

    template <class _Tp>
        _LIBCPP_INLINE_VISIBILITY
        static
        typename enable_if
        <
            (is_same<allocator_type, allocator<_Tp> >::value
                || !__has_construct<allocator_type, _Tp*, _Tp>::value) &&
             is_trivially_move_constructible<_Tp>::value,
            void
        >::type
        __construct_backward(allocator_type&, _Tp* __begin1, _Tp* __end1, _Tp*& __end2)
        {
            ptrdiff_t _Np = __end1 - __begin1;
            __end2 -= _Np;
            if (_Np > 0)
                memcpy(__end2, __begin1, _Np * sizeof(_Tp));
        }

private:

    _LIBCPP_INLINE_VISIBILITY
    static pointer __allocate(allocator_type& __a, size_type __n,
        const_void_pointer __hint, true_type)
        {return __a.allocate(__n, __hint);}
    _LIBCPP_INLINE_VISIBILITY
    static pointer __allocate(allocator_type& __a, size_type __n,
        const_void_pointer, false_type)
        {return __a.allocate(__n);}

#ifndef _LIBCPP_HAS_NO_VARIADICS
    template <class _Tp, class... _Args>
        _LIBCPP_INLINE_VISIBILITY
        static void __construct(true_type, allocator_type& __a, _Tp* __p, _Args&&... __args)
            {__a.construct(__p, _VSTD::forward<_Args>(__args)...);}
    template <class _Tp, class... _Args>
        _LIBCPP_INLINE_VISIBILITY
        static void __construct(false_type, allocator_type&, _Tp* __p, _Args&&... __args)
            {
                ::new ((void*)__p) _Tp(_VSTD::forward<_Args>(__args)...);
            }
#endif  // _LIBCPP_HAS_NO_VARIADICS

    template <class _Tp>
        _LIBCPP_INLINE_VISIBILITY
        static void __destroy(true_type, allocator_type& __a, _Tp* __p)
            {__a.destroy(__p);}
    template <class _Tp>
        _LIBCPP_INLINE_VISIBILITY
        static void __destroy(false_type, allocator_type&, _Tp* __p)
            {
                __p->~_Tp();
            }

    _LIBCPP_INLINE_VISIBILITY
    static size_type __max_size(true_type, const allocator_type& __a)
            {return __a.max_size();}
    _LIBCPP_INLINE_VISIBILITY
    static size_type __max_size(false_type, const allocator_type&)
            {return numeric_limits<size_type>::max() / sizeof(value_type);}

    _LIBCPP_INLINE_VISIBILITY
    static allocator_type
        __select_on_container_copy_construction(true_type, const allocator_type& __a)
            {return __a.select_on_container_copy_construction();}
    _LIBCPP_INLINE_VISIBILITY
    static allocator_type
        __select_on_container_copy_construction(false_type, const allocator_type& __a)
            {return __a;}
};

template <class _Traits, class _Tp>
struct __rebind_alloc_helper
{
#ifndef _LIBCPP_CXX03_LANG
    typedef typename _Traits::template rebind_alloc<_Tp>        type;
#else
    typedef typename _Traits::template rebind_alloc<_Tp>::other type;
#endif
};

// allocator

template <class _Tp>
class _LIBCPP_TEMPLATE_VIS allocator
{
public:
    typedef size_t            size_type;
    typedef ptrdiff_t         difference_type;
    typedef _Tp*              pointer;
    typedef const _Tp*        const_pointer;
    typedef _Tp&              reference;
    typedef const _Tp&        const_reference;
    typedef _Tp               value_type;

    typedef true_type propagate_on_container_move_assignment;
    typedef true_type is_always_equal;

    template <class _Up> struct rebind {typedef allocator<_Up> other;};

    _LIBCPP_INLINE_VISIBILITY allocator() _NOEXCEPT {}
    template <class _Up> _LIBCPP_INLINE_VISIBILITY allocator(const allocator<_Up>&) _NOEXCEPT {}
    _LIBCPP_INLINE_VISIBILITY pointer address(reference __x) const _NOEXCEPT
        {return _VSTD::addressof(__x);}
    _LIBCPP_INLINE_VISIBILITY const_pointer address(const_reference __x) const _NOEXCEPT
        {return _VSTD::addressof(__x);}
    _LIBCPP_INLINE_VISIBILITY pointer allocate(size_type __n, allocator<void>::const_pointer = 0)
        {
        if (__n > max_size())
            __throw_length_error("allocator<T>::allocate(size_t n)"
                                 " 'n' exceeds maximum supported size");
        return static_cast<pointer>(_VSTD::__allocate(__n * sizeof(_Tp)));
        }
    _LIBCPP_INLINE_VISIBILITY void deallocate(pointer __p, size_type) _NOEXCEPT
        {_VSTD::__libcpp_deallocate((void*)__p);}
    _LIBCPP_INLINE_VISIBILITY size_type max_size() const _NOEXCEPT
        {return size_type(~0) / sizeof(_Tp);}
#if !defined(_LIBCPP_HAS_NO_RVALUE_REFERENCES) && !defined(_LIBCPP_HAS_NO_VARIADICS)
    template <class _Up, class... _Args>
        _LIBCPP_INLINE_VISIBILITY
        void
        construct(_Up* __p, _Args&&... __args)
        {
            ::new((void*)__p) _Up(_VSTD::forward<_Args>(__args)...);
        }
#else  // !defined(_LIBCPP_HAS_NO_RVALUE_REFERENCES) && !defined(_LIBCPP_HAS_NO_VARIADICS)
        _LIBCPP_INLINE_VISIBILITY
        void
        construct(pointer __p)
        {
            ::new((void*)__p) _Tp();
        }
# if defined(_LIBCPP_HAS_NO_RVALUE_REFERENCES)

    template <class _A0>
        _LIBCPP_INLINE_VISIBILITY
        void
        construct(pointer __p, _A0& __a0)
        {
            ::new((void*)__p) _Tp(__a0);
        }
    template <class _A0>
        _LIBCPP_INLINE_VISIBILITY
        void
        construct(pointer __p, const _A0& __a0)
        {
            ::new((void*)__p) _Tp(__a0);
        }
# endif  // defined(_LIBCPP_HAS_NO_RVALUE_REFERENCES)
    template <class _A0, class _A1>
        _LIBCPP_INLINE_VISIBILITY
        void
        construct(pointer __p, _A0& __a0, _A1& __a1)
        {
            ::new((void*)__p) _Tp(__a0, __a1);
        }
    template <class _A0, class _A1>
        _LIBCPP_INLINE_VISIBILITY
        void
        construct(pointer __p, const _A0& __a0, _A1& __a1)
        {
            ::new((void*)__p) _Tp(__a0, __a1);
        }
    template <class _A0, class _A1>
        _LIBCPP_INLINE_VISIBILITY
        void
        construct(pointer __p, _A0& __a0, const _A1& __a1)
        {
            ::new((void*)__p) _Tp(__a0, __a1);
        }
    template <class _A0, class _A1>
        _LIBCPP_INLINE_VISIBILITY
        void
        construct(pointer __p, const _A0& __a0, const _A1& __a1)
        {
            ::new((void*)__p) _Tp(__a0, __a1);
        }
#endif  // !defined(_LIBCPP_HAS_NO_RVALUE_REFERENCES) && !defined(_LIBCPP_HAS_NO_VARIADICS)
    _LIBCPP_INLINE_VISIBILITY void destroy(pointer __p) {__p->~_Tp();}
};

template <class _Tp>
class _LIBCPP_TEMPLATE_VIS allocator<const _Tp>
{
public:
    typedef size_t            size_type;
    typedef ptrdiff_t         difference_type;
    typedef const _Tp*        pointer;
    typedef const _Tp*        const_pointer;
    typedef const _Tp&        reference;
    typedef const _Tp&        const_reference;
    typedef const _Tp         value_type;

    typedef true_type propagate_on_container_move_assignment;
    typedef true_type is_always_equal;

    template <class _Up> struct rebind {typedef allocator<_Up> other;};

    _LIBCPP_INLINE_VISIBILITY allocator() _NOEXCEPT {}
    template <class _Up> _LIBCPP_INLINE_VISIBILITY allocator(const allocator<_Up>&) _NOEXCEPT {}
    _LIBCPP_INLINE_VISIBILITY const_pointer address(const_reference __x) const _NOEXCEPT
        {return _VSTD::addressof(__x);}
    _LIBCPP_INLINE_VISIBILITY pointer allocate(size_type __n, allocator<void>::const_pointer = 0)
    {
        if (__n > max_size())
            __throw_length_error("allocator<const T>::allocate(size_t n)"
                                 " 'n' exceeds maximum supported size");
        return static_cast<pointer>(_VSTD::__allocate(__n * sizeof(_Tp)));
    }
    _LIBCPP_INLINE_VISIBILITY void deallocate(pointer __p, size_type) _NOEXCEPT
        {_VSTD::__libcpp_deallocate((void*) const_cast<_Tp *>(__p));}
    _LIBCPP_INLINE_VISIBILITY size_type max_size() const _NOEXCEPT
        {return size_type(~0) / sizeof(_Tp);}
#if !defined(_LIBCPP_HAS_NO_RVALUE_REFERENCES) && !defined(_LIBCPP_HAS_NO_VARIADICS)
    template <class _Up, class... _Args>
        _LIBCPP_INLINE_VISIBILITY
        void
        construct(_Up* __p, _Args&&... __args)
        {
            ::new((void*)__p) _Up(_VSTD::forward<_Args>(__args)...);
        }
#else  // !defined(_LIBCPP_HAS_NO_RVALUE_REFERENCES) && !defined(_LIBCPP_HAS_NO_VARIADICS)
        _LIBCPP_INLINE_VISIBILITY
        void
        construct(pointer __p)
        {
            ::new((void*) const_cast<_Tp *>(__p)) _Tp();
        }
# if defined(_LIBCPP_HAS_NO_RVALUE_REFERENCES)

    template <class _A0>
        _LIBCPP_INLINE_VISIBILITY
        void
        construct(pointer __p, _A0& __a0)
        {
            ::new((void*) const_cast<_Tp *>(__p)) _Tp(__a0);
        }
    template <class _A0>
        _LIBCPP_INLINE_VISIBILITY
        void
        construct(pointer __p, const _A0& __a0)
        {
            ::new((void*) const_cast<_Tp *>(__p)) _Tp(__a0);
        }
# endif  // defined(_LIBCPP_HAS_NO_RVALUE_REFERENCES)
    template <class _A0, class _A1>
        _LIBCPP_INLINE_VISIBILITY
        void
        construct(pointer __p, _A0& __a0, _A1& __a1)
        {
            ::new((void*) const_cast<_Tp *>(__p)) _Tp(__a0, __a1);
        }
    template <class _A0, class _A1>
        _LIBCPP_INLINE_VISIBILITY
        void
        construct(pointer __p, const _A0& __a0, _A1& __a1)
        {
            ::new((void*) const_cast<_Tp *>(__p)) _Tp(__a0, __a1);
        }
    template <class _A0, class _A1>
        _LIBCPP_INLINE_VISIBILITY
        void
        construct(pointer __p, _A0& __a0, const _A1& __a1)
        {
            ::new((void*) const_cast<_Tp *>(__p)) _Tp(__a0, __a1);
        }
    template <class _A0, class _A1>
        _LIBCPP_INLINE_VISIBILITY
        void
        construct(pointer __p, const _A0& __a0, const _A1& __a1)
        {
            ::new((void*) const_cast<_Tp *>(__p)) _Tp(__a0, __a1);
        }
#endif  // !defined(_LIBCPP_HAS_NO_RVALUE_REFERENCES) && !defined(_LIBCPP_HAS_NO_VARIADICS)
    _LIBCPP_INLINE_VISIBILITY void destroy(pointer __p) {__p->~_Tp();}
};

template <class _Tp, class _Up>
inline _LIBCPP_INLINE_VISIBILITY
bool operator==(const allocator<_Tp>&, const allocator<_Up>&) _NOEXCEPT {return true;}

template <class _Tp, class _Up>
inline _LIBCPP_INLINE_VISIBILITY
bool operator!=(const allocator<_Tp>&, const allocator<_Up>&) _NOEXCEPT {return false;}

template <class _OutputIterator, class _Tp>
class _LIBCPP_TEMPLATE_VIS raw_storage_iterator
    : public iterator<output_iterator_tag,
                      _Tp,                                         // purposefully not C++03
                      ptrdiff_t,                                   // purposefully not C++03
                      _Tp*,                                        // purposefully not C++03
                      raw_storage_iterator<_OutputIterator, _Tp>&> // purposefully not C++03
{
private:
    _OutputIterator __x_;
public:
    _LIBCPP_INLINE_VISIBILITY explicit raw_storage_iterator(_OutputIterator __x) : __x_(__x) {}
    _LIBCPP_INLINE_VISIBILITY raw_storage_iterator& operator*() {return *this;}
    _LIBCPP_INLINE_VISIBILITY raw_storage_iterator& operator=(const _Tp& __element)
        {::new(&*__x_) _Tp(__element); return *this;}
#if _LIBCPP_STD_VER >= 14
    _LIBCPP_INLINE_VISIBILITY raw_storage_iterator& operator=(_Tp&& __element)
        {::new(&*__x_) _Tp(_VSTD::move(__element)); return *this;}
#endif
    _LIBCPP_INLINE_VISIBILITY raw_storage_iterator& operator++() {++__x_; return *this;}
    _LIBCPP_INLINE_VISIBILITY raw_storage_iterator  operator++(int)
        {raw_storage_iterator __t(*this); ++__x_; return __t;}
#if _LIBCPP_STD_VER >= 14
    _LIBCPP_INLINE_VISIBILITY _OutputIterator base() const { return __x_; }
#endif
};

template <class _Tp>
pair<_Tp*, ptrdiff_t>
get_temporary_buffer(ptrdiff_t __n) _NOEXCEPT
{
    pair<_Tp*, ptrdiff_t> __r(0, 0);
    const ptrdiff_t __m = (~ptrdiff_t(0) ^
                           ptrdiff_t(ptrdiff_t(1) << (sizeof(ptrdiff_t) * __CHAR_BIT__ - 1)))
                           / sizeof(_Tp);
    if (__n > __m)
        __n = __m;
    while (__n > 0)
    {
        __r.first = static_cast<_Tp*>(::operator new(__n * sizeof(_Tp), nothrow));
        if (__r.first)
        {
            __r.second = __n;
            break;
        }
        __n /= 2;
    }
    return __r;
}

template <class _Tp>
inline _LIBCPP_INLINE_VISIBILITY
void return_temporary_buffer(_Tp* __p) _NOEXCEPT {::operator delete(__p);}

#if _LIBCPP_STD_VER <= 14 || defined(_LIBCPP_ENABLE_CXX17_REMOVED_AUTO_PTR)
template <class _Tp>
struct auto_ptr_ref
{
    _Tp* __ptr_;
};

template<class _Tp>
class _LIBCPP_TEMPLATE_VIS auto_ptr
{
private:
    _Tp* __ptr_;
public:
    typedef _Tp element_type;

    _LIBCPP_INLINE_VISIBILITY explicit auto_ptr(_Tp* __p = 0) throw() : __ptr_(__p) {}
    _LIBCPP_INLINE_VISIBILITY auto_ptr(auto_ptr& __p) throw() : __ptr_(__p.release()) {}
    template<class _Up> _LIBCPP_INLINE_VISIBILITY auto_ptr(auto_ptr<_Up>& __p) throw()
        : __ptr_(__p.release()) {}
    _LIBCPP_INLINE_VISIBILITY auto_ptr& operator=(auto_ptr& __p) throw()
        {reset(__p.release()); return *this;}
    template<class _Up> _LIBCPP_INLINE_VISIBILITY auto_ptr& operator=(auto_ptr<_Up>& __p) throw()
        {reset(__p.release()); return *this;}
    _LIBCPP_INLINE_VISIBILITY auto_ptr& operator=(auto_ptr_ref<_Tp> __p) throw()
        {reset(__p.__ptr_); return *this;}
    _LIBCPP_INLINE_VISIBILITY ~auto_ptr() throw() {delete __ptr_;}

    _LIBCPP_INLINE_VISIBILITY _Tp& operator*() const throw()
        {return *__ptr_;}
    _LIBCPP_INLINE_VISIBILITY _Tp* operator->() const throw() {return __ptr_;}
    _LIBCPP_INLINE_VISIBILITY _Tp* get() const throw() {return __ptr_;}
    _LIBCPP_INLINE_VISIBILITY _Tp* release() throw()
    {
        _Tp* __t = __ptr_;
        __ptr_ = 0;
        return __t;
    }
    _LIBCPP_INLINE_VISIBILITY void reset(_Tp* __p = 0) throw()
    {
        if (__ptr_ != __p)
            delete __ptr_;
        __ptr_ = __p;
    }

    _LIBCPP_INLINE_VISIBILITY auto_ptr(auto_ptr_ref<_Tp> __p) throw() : __ptr_(__p.__ptr_) {}
    template<class _Up> _LIBCPP_INLINE_VISIBILITY operator auto_ptr_ref<_Up>() throw()
        {auto_ptr_ref<_Up> __t; __t.__ptr_ = release(); return __t;}
    template<class _Up> _LIBCPP_INLINE_VISIBILITY operator auto_ptr<_Up>() throw()
        {return auto_ptr<_Up>(release());}
};

template <>
class _LIBCPP_TEMPLATE_VIS auto_ptr<void>
{
public:
    typedef void element_type;
};
#endif

template <class _Tp, int _Idx,
          bool _CanBeEmptyBase =
              is_empty<_Tp>::value && !__libcpp_is_final<_Tp>::value>
struct __compressed_pair_elem {
  typedef _Tp _ParamT;
  typedef _Tp& reference;
  typedef const _Tp& const_reference;

#ifndef _LIBCPP_CXX03_LANG
  constexpr __compressed_pair_elem() : __value_() {}

  template <class _Up, class = typename enable_if<
      !is_same<__compressed_pair_elem, typename decay<_Up>::type>::value
  >::type>
  constexpr explicit
  __compressed_pair_elem(_Up&& __u)
      : __value_(_VSTD::forward<_Up>(__u)){};

  template <class... _Args, size_t... _Indexes>
  _LIBCPP_INLINE_VISIBILITY _LIBCPP_CONSTEXPR_AFTER_CXX14
  __compressed_pair_elem(piecewise_construct_t, tuple<_Args...> __args,
                         __tuple_indices<_Indexes...>)
      : __value_(_VSTD::forward<_Args>(_VSTD::get<_Indexes>(__args))...) {}
#else
  __compressed_pair_elem() : __value_() {}
  __compressed_pair_elem(_ParamT __p) : __value_(std::forward<_ParamT>(__p)) {}
#endif

  reference __get() _NOEXCEPT { return __value_; }
  const_reference __get() const _NOEXCEPT { return __value_; }

private:
  _Tp __value_;
};

template <class _Tp, int _Idx>
struct __compressed_pair_elem<_Tp, _Idx, true> : private _Tp {
  typedef _Tp _ParamT;
  typedef _Tp& reference;
  typedef const _Tp& const_reference;
  typedef _Tp __value_type;

#ifndef _LIBCPP_CXX03_LANG
  constexpr __compressed_pair_elem() = default;

  template <class _Up, class = typename enable_if<
        !is_same<__compressed_pair_elem, typename decay<_Up>::type>::value
  >::type>
  constexpr explicit
  __compressed_pair_elem(_Up&& __u)
      : __value_type(_VSTD::forward<_Up>(__u)){};

  template <class... _Args, size_t... _Indexes>
  _LIBCPP_INLINE_VISIBILITY _LIBCPP_CONSTEXPR_AFTER_CXX14
  __compressed_pair_elem(piecewise_construct_t, tuple<_Args...> __args,
                         __tuple_indices<_Indexes...>)
      : __value_type(_VSTD::forward<_Args>(_VSTD::get<_Indexes>(__args))...) {}
#else
  __compressed_pair_elem() : __value_type() {}
  __compressed_pair_elem(_ParamT __p)
      : __value_type(std::forward<_ParamT>(__p)) {}
#endif

  reference __get() _NOEXCEPT { return *this; }
  const_reference __get() const _NOEXCEPT { return *this; }
};

// Tag used to construct the second element of the compressed pair.
struct __second_tag {};

template <class _T1, class _T2>
class __compressed_pair : private __compressed_pair_elem<_T1, 0>,
                          private __compressed_pair_elem<_T2, 1> {
  typedef __compressed_pair_elem<_T1, 0> _Base1;
  typedef __compressed_pair_elem<_T2, 1> _Base2;

  // NOTE: This static assert should never fire because __compressed_pair
  // is *almost never* used in a scenario where it's possible for T1 == T2.
  // (The exception is std::function where it is possible that the function
  //  object and the allocator have the same type).
  static_assert((!is_same<_T1, _T2>::value),
    "__compressed_pair cannot be instantated when T1 and T2 are the same type; "
    "The current implementation is NOT ABI-compatible with the previous "
    "implementation for this configuration");

public:
#ifndef _LIBCPP_CXX03_LANG
  template <bool _Dummy = true,
      class = typename enable_if<
          __dependent_type<is_default_constructible<_T1>, _Dummy>::value &&
          __dependent_type<is_default_constructible<_T2>, _Dummy>::value
      >::type
  >
  _LIBCPP_INLINE_VISIBILITY
  constexpr __compressed_pair() {}

  template <class _Tp, typename enable_if<!is_same<typename decay<_Tp>::type,
                                                   __compressed_pair>::value,
                                          bool>::type = true>
  _LIBCPP_INLINE_VISIBILITY constexpr explicit
  __compressed_pair(_Tp&& __t)
      : _Base1(std::forward<_Tp>(__t)), _Base2() {}

  template <class _Tp>
  _LIBCPP_INLINE_VISIBILITY constexpr
  __compressed_pair(__second_tag, _Tp&& __t)
      : _Base1(), _Base2(std::forward<_Tp>(__t)) {}

  template <class _U1, class _U2>
  _LIBCPP_INLINE_VISIBILITY constexpr
  __compressed_pair(_U1&& __t1, _U2&& __t2)
      : _Base1(std::forward<_U1>(__t1)), _Base2(std::forward<_U2>(__t2)) {}

  template <class... _Args1, class... _Args2>
  _LIBCPP_INLINE_VISIBILITY _LIBCPP_CONSTEXPR_AFTER_CXX14
  __compressed_pair(piecewise_construct_t __pc, tuple<_Args1...> __first_args,
                    tuple<_Args2...> __second_args)
      : _Base1(__pc, _VSTD::move(__first_args),
               typename __make_tuple_indices<sizeof...(_Args1)>::type()),
        _Base2(__pc, _VSTD::move(__second_args),
               typename __make_tuple_indices<sizeof...(_Args2)>::type()) {}

#else
  _LIBCPP_INLINE_VISIBILITY
  __compressed_pair() {}

  _LIBCPP_INLINE_VISIBILITY explicit
  __compressed_pair(_T1 __t1) : _Base1(_VSTD::forward<_T1>(__t1)) {}

  _LIBCPP_INLINE_VISIBILITY
  __compressed_pair(__second_tag, _T2 __t2)
      : _Base1(), _Base2(_VSTD::forward<_T2>(__t2)) {}

  _LIBCPP_INLINE_VISIBILITY
  __compressed_pair(_T1 __t1, _T2 __t2)
      : _Base1(_VSTD::forward<_T1>(__t1)), _Base2(_VSTD::forward<_T2>(__t2)) {}
#endif

  _LIBCPP_INLINE_VISIBILITY
  typename _Base1::reference first() _NOEXCEPT {
    return static_cast<_Base1&>(*this).__get();
  }

  _LIBCPP_INLINE_VISIBILITY
  typename _Base1::const_reference first() const _NOEXCEPT {
    return static_cast<_Base1 const&>(*this).__get();
  }

  _LIBCPP_INLINE_VISIBILITY
  typename _Base2::reference second() _NOEXCEPT {
    return static_cast<_Base2&>(*this).__get();
  }

  _LIBCPP_INLINE_VISIBILITY
  typename _Base2::const_reference second() const _NOEXCEPT {
    return static_cast<_Base2 const&>(*this).__get();
  }

  _LIBCPP_INLINE_VISIBILITY
  void swap(__compressed_pair& __x)
    _NOEXCEPT_(__is_nothrow_swappable<_T1>::value &&
               __is_nothrow_swappable<_T2>::value)
  {
    using std::swap;
    swap(first(), __x.first());
    swap(second(), __x.second());
  }
};

template <class _T1, class _T2>
inline _LIBCPP_INLINE_VISIBILITY
void swap(__compressed_pair<_T1, _T2>& __x, __compressed_pair<_T1, _T2>& __y)
    _NOEXCEPT_(__is_nothrow_swappable<_T1>::value &&
               __is_nothrow_swappable<_T2>::value) {
  __x.swap(__y);
}

// default_delete

template <class _Tp>
struct _LIBCPP_TEMPLATE_VIS default_delete {
    static_assert(!is_function<_Tp>::value,
                  "default_delete cannot be instantiated for function types");
#ifndef _LIBCPP_CXX03_LANG
  _LIBCPP_INLINE_VISIBILITY constexpr default_delete() noexcept = default;
#else
  _LIBCPP_INLINE_VISIBILITY default_delete() {}
#endif
  template <class _Up>
  _LIBCPP_INLINE_VISIBILITY
  default_delete(const default_delete<_Up>&,
                 typename enable_if<is_convertible<_Up*, _Tp*>::value>::type* =
                     0) _NOEXCEPT {}

  _LIBCPP_INLINE_VISIBILITY void operator()(_Tp* __ptr) const _NOEXCEPT {
    static_assert(sizeof(_Tp) > 0,
                  "default_delete can not delete incomplete type");
    static_assert(!is_void<_Tp>::value,
                  "default_delete can not delete incomplete type");
    delete __ptr;
  }
};

template <class _Tp>
struct _LIBCPP_TEMPLATE_VIS default_delete<_Tp[]> {
private:
  template <class _Up>
  struct _EnableIfConvertible
      : enable_if<is_convertible<_Up(*)[], _Tp(*)[]>::value> {};

public:
#ifndef _LIBCPP_CXX03_LANG
  _LIBCPP_INLINE_VISIBILITY constexpr default_delete() noexcept = default;
#else
  _LIBCPP_INLINE_VISIBILITY default_delete() {}
#endif

  template <class _Up>
  _LIBCPP_INLINE_VISIBILITY
  default_delete(const default_delete<_Up[]>&,
                 typename _EnableIfConvertible<_Up>::type* = 0) _NOEXCEPT {}

  template <class _Up>
  _LIBCPP_INLINE_VISIBILITY
  typename _EnableIfConvertible<_Up>::type
  operator()(_Up* __ptr) const _NOEXCEPT {
    static_assert(sizeof(_Tp) > 0,
                  "default_delete can not delete incomplete type");
    static_assert(!is_void<_Tp>::value,
                  "default_delete can not delete void type");
    delete[] __ptr;
  }
};



#ifndef _LIBCPP_CXX03_LANG
template <class _Deleter>
struct __unique_ptr_deleter_sfinae {
  static_assert(!is_reference<_Deleter>::value, "incorrect specialization");
  typedef const _Deleter& __lval_ref_type;
  typedef _Deleter&& __good_rval_ref_type;
  typedef true_type __enable_rval_overload;
};

template <class _Deleter>
struct __unique_ptr_deleter_sfinae<_Deleter const&> {
  typedef const _Deleter& __lval_ref_type;
  typedef const _Deleter&& __bad_rval_ref_type;
  typedef false_type __enable_rval_overload;
};

template <class _Deleter>
struct __unique_ptr_deleter_sfinae<_Deleter&> {
  typedef _Deleter& __lval_ref_type;
  typedef _Deleter&& __bad_rval_ref_type;
  typedef false_type __enable_rval_overload;
};
#endif // !defined(_LIBCPP_CXX03_LANG)

template <class _Tp, class _Dp = default_delete<_Tp> >
class _LIBCPP_TEMPLATE_VIS unique_ptr {
public:
  typedef _Tp element_type;
  typedef _Dp deleter_type;
  typedef typename __pointer_type<_Tp, deleter_type>::type pointer;

  static_assert(!is_rvalue_reference<deleter_type>::value,
                "the specified deleter type cannot be an rvalue reference");

private:
  __compressed_pair<pointer, deleter_type> __ptr_;

  struct __nat { int __for_bool_; };

#ifndef _LIBCPP_CXX03_LANG
  typedef __unique_ptr_deleter_sfinae<_Dp> _DeleterSFINAE;

  template <bool _Dummy>
  using _LValRefType =
      typename __dependent_type<_DeleterSFINAE, _Dummy>::__lval_ref_type;

  template <bool _Dummy>
  using _GoodRValRefType =
      typename __dependent_type<_DeleterSFINAE, _Dummy>::__good_rval_ref_type;

  template <bool _Dummy>
  using _BadRValRefType =
      typename __dependent_type<_DeleterSFINAE, _Dummy>::__bad_rval_ref_type;

  template <bool _Dummy, class _Deleter = typename __dependent_type<
                             __identity<deleter_type>, _Dummy>::type>
  using _EnableIfDeleterDefaultConstructible =
      typename enable_if<is_default_constructible<_Deleter>::value &&
                         !is_pointer<_Deleter>::value>::type;

  template <class _ArgType>
  using _EnableIfDeleterConstructible =
      typename enable_if<is_constructible<deleter_type, _ArgType>::value>::type;

  template <class _UPtr, class _Up>
  using _EnableIfMoveConvertible = typename enable_if<
      is_convertible<typename _UPtr::pointer, pointer>::value &&
      !is_array<_Up>::value
  >::type;

  template <class _UDel>
  using _EnableIfDeleterConvertible = typename enable_if<
      (is_reference<_Dp>::value && is_same<_Dp, _UDel>::value) ||
      (!is_reference<_Dp>::value && is_convertible<_UDel, _Dp>::value)
    >::type;

  template <class _UDel>
  using _EnableIfDeleterAssignable = typename enable_if<
      is_assignable<_Dp&, _UDel&&>::value
    >::type;

public:
  template <bool _Dummy = true,
            class = _EnableIfDeleterDefaultConstructible<_Dummy>>
  _LIBCPP_INLINE_VISIBILITY
  constexpr unique_ptr() noexcept : __ptr_(pointer()) {}

  template <bool _Dummy = true,
            class = _EnableIfDeleterDefaultConstructible<_Dummy>>
  _LIBCPP_INLINE_VISIBILITY
  constexpr unique_ptr(nullptr_t) noexcept : __ptr_(pointer()) {}

  template <bool _Dummy = true,
            class = _EnableIfDeleterDefaultConstructible<_Dummy>>
  _LIBCPP_INLINE_VISIBILITY
  explicit unique_ptr(pointer __p) noexcept : __ptr_(__p) {}

  template <bool _Dummy = true,
            class = _EnableIfDeleterConstructible<_LValRefType<_Dummy>>>
  _LIBCPP_INLINE_VISIBILITY
  unique_ptr(pointer __p, _LValRefType<_Dummy> __d) noexcept
      : __ptr_(__p, __d) {}

  template <bool _Dummy = true,
            class = _EnableIfDeleterConstructible<_GoodRValRefType<_Dummy>>>
  _LIBCPP_INLINE_VISIBILITY
  unique_ptr(pointer __p, _GoodRValRefType<_Dummy> __d) noexcept
      : __ptr_(__p, _VSTD::move(__d)) {
    static_assert(!is_reference<deleter_type>::value,
                  "rvalue deleter bound to reference");
  }

  template <bool _Dummy = true,
            class = _EnableIfDeleterConstructible<_BadRValRefType<_Dummy>>>
  _LIBCPP_INLINE_VISIBILITY
  unique_ptr(pointer __p, _BadRValRefType<_Dummy> __d) = delete;

  _LIBCPP_INLINE_VISIBILITY
  unique_ptr(unique_ptr&& __u) noexcept
      : __ptr_(__u.release(), _VSTD::forward<deleter_type>(__u.get_deleter())) {
  }

  template <class _Up, class _Ep,
      class = _EnableIfMoveConvertible<unique_ptr<_Up, _Ep>, _Up>,
      class = _EnableIfDeleterConvertible<_Ep>
  >
  _LIBCPP_INLINE_VISIBILITY
  unique_ptr(unique_ptr<_Up, _Ep>&& __u) _NOEXCEPT
      : __ptr_(__u.release(), _VSTD::forward<_Ep>(__u.get_deleter())) {}

#if _LIBCPP_STD_VER <= 14 || defined(_LIBCPP_ENABLE_CXX17_REMOVED_AUTO_PTR)
  template <class _Up>
  _LIBCPP_INLINE_VISIBILITY
  unique_ptr(auto_ptr<_Up>&& __p,
             typename enable_if<is_convertible<_Up*, _Tp*>::value &&
                                    is_same<_Dp, default_delete<_Tp>>::value,
                                __nat>::type = __nat()) _NOEXCEPT
      : __ptr_(__p.release()) {}
#endif

  _LIBCPP_INLINE_VISIBILITY
  unique_ptr& operator=(unique_ptr&& __u) _NOEXCEPT {
    reset(__u.release());
    __ptr_.second() = _VSTD::forward<deleter_type>(__u.get_deleter());
    return *this;
  }

  template <class _Up, class _Ep,
      class = _EnableIfMoveConvertible<unique_ptr<_Up, _Ep>, _Up>,
      class = _EnableIfDeleterAssignable<_Ep>
  >
  _LIBCPP_INLINE_VISIBILITY
  unique_ptr& operator=(unique_ptr<_Up, _Ep>&& __u) _NOEXCEPT {
    reset(__u.release());
    __ptr_.second() = _VSTD::forward<_Ep>(__u.get_deleter());
    return *this;
  }

#else  // _LIBCPP_CXX03_LANG
private:
  unique_ptr(unique_ptr&);
  template <class _Up, class _Ep> unique_ptr(unique_ptr<_Up, _Ep>&);

  unique_ptr& operator=(unique_ptr&);
  template <class _Up, class _Ep> unique_ptr& operator=(unique_ptr<_Up, _Ep>&);

public:
  _LIBCPP_INLINE_VISIBILITY
  unique_ptr() : __ptr_(pointer())
  {
    static_assert(!is_pointer<deleter_type>::value,
                  "unique_ptr constructed with null function pointer deleter");
    static_assert(is_default_constructible<deleter_type>::value,
                  "unique_ptr::deleter_type is not default constructible");
  }
  _LIBCPP_INLINE_VISIBILITY
  unique_ptr(nullptr_t) : __ptr_(pointer())
  {
    static_assert(!is_pointer<deleter_type>::value,
                  "unique_ptr constructed with null function pointer deleter");
  }
  _LIBCPP_INLINE_VISIBILITY
  explicit unique_ptr(pointer __p)
      : __ptr_(_VSTD::move(__p)) {
    static_assert(!is_pointer<deleter_type>::value,
                  "unique_ptr constructed with null function pointer deleter");
  }

  _LIBCPP_INLINE_VISIBILITY
  operator __rv<unique_ptr>() {
    return __rv<unique_ptr>(*this);
  }

  _LIBCPP_INLINE_VISIBILITY
  unique_ptr(__rv<unique_ptr> __u)
      : __ptr_(__u->release(),
               _VSTD::forward<deleter_type>(__u->get_deleter())) {}

  template <class _Up, class _Ep>
  _LIBCPP_INLINE_VISIBILITY
  typename enable_if<
      !is_array<_Up>::value &&
          is_convertible<typename unique_ptr<_Up, _Ep>::pointer,
                         pointer>::value &&
          is_assignable<deleter_type&, _Ep&>::value,
      unique_ptr&>::type
  operator=(unique_ptr<_Up, _Ep> __u) {
    reset(__u.release());
    __ptr_.second() = _VSTD::forward<_Ep>(__u.get_deleter());
    return *this;
  }

  _LIBCPP_INLINE_VISIBILITY
  unique_ptr(pointer __p, deleter_type __d)
      : __ptr_(_VSTD::move(__p), _VSTD::move(__d)) {}
#endif // _LIBCPP_CXX03_LANG

#if _LIBCPP_STD_VER <= 14 || defined(_LIBCPP_ENABLE_CXX17_REMOVED_AUTO_PTR)
  template <class _Up>
  _LIBCPP_INLINE_VISIBILITY
      typename enable_if<is_convertible<_Up*, _Tp*>::value &&
                             is_same<_Dp, default_delete<_Tp> >::value,
                         unique_ptr&>::type
      operator=(auto_ptr<_Up> __p) {
    reset(__p.release());
    return *this;
  }
#endif

  _LIBCPP_INLINE_VISIBILITY
  ~unique_ptr() { reset(); }

  _LIBCPP_INLINE_VISIBILITY
  unique_ptr& operator=(nullptr_t) _NOEXCEPT {
    reset();
    return *this;
  }

  _LIBCPP_INLINE_VISIBILITY
  typename add_lvalue_reference<_Tp>::type
  operator*() const {
    return *__ptr_.first();
  }
  _LIBCPP_INLINE_VISIBILITY
  pointer operator->() const _NOEXCEPT {
    return __ptr_.first();
  }
  _LIBCPP_INLINE_VISIBILITY
  pointer get() const _NOEXCEPT {
    return __ptr_.first();
  }
  _LIBCPP_INLINE_VISIBILITY
  deleter_type& get_deleter() _NOEXCEPT {
    return __ptr_.second();
  }
  _LIBCPP_INLINE_VISIBILITY
  const deleter_type& get_deleter() const _NOEXCEPT {
    return __ptr_.second();
  }
  _LIBCPP_INLINE_VISIBILITY
  _LIBCPP_EXPLICIT operator bool() const _NOEXCEPT {
    return __ptr_.first() != nullptr;
  }

  _LIBCPP_INLINE_VISIBILITY
  pointer release() _NOEXCEPT {
    pointer __t = __ptr_.first();
    __ptr_.first() = pointer();
    return __t;
  }

  _LIBCPP_INLINE_VISIBILITY
  void reset(pointer __p = pointer()) _NOEXCEPT {
    pointer __tmp = __ptr_.first();
    __ptr_.first() = __p;
    if (__tmp)
      __ptr_.second()(__tmp);
  }

  _LIBCPP_INLINE_VISIBILITY
  void swap(unique_ptr& __u) _NOEXCEPT {
    __ptr_.swap(__u.__ptr_);
  }
};


template <class _Tp, class _Dp>
class _LIBCPP_TEMPLATE_VIS unique_ptr<_Tp[], _Dp> {
public:
  typedef _Tp element_type;
  typedef _Dp deleter_type;
  typedef typename __pointer_type<_Tp, deleter_type>::type pointer;

private:
  __compressed_pair<pointer, deleter_type> __ptr_;

  template <class _From>
  struct _CheckArrayPointerConversion : is_same<_From, pointer> {};

  template <class _FromElem>
  struct _CheckArrayPointerConversion<_FromElem*>
      : integral_constant<bool,
          is_same<_FromElem*, pointer>::value ||
            (is_same<pointer, element_type*>::value &&
             is_convertible<_FromElem(*)[], element_type(*)[]>::value)
      >
  {};

#ifndef _LIBCPP_CXX03_LANG
  typedef __unique_ptr_deleter_sfinae<_Dp> _DeleterSFINAE;

  template <bool _Dummy>
  using _LValRefType =
      typename __dependent_type<_DeleterSFINAE, _Dummy>::__lval_ref_type;

  template <bool _Dummy>
  using _GoodRValRefType =
      typename __dependent_type<_DeleterSFINAE, _Dummy>::__good_rval_ref_type;

  template <bool _Dummy>
  using _BadRValRefType =
      typename __dependent_type<_DeleterSFINAE, _Dummy>::__bad_rval_ref_type;

  template <bool _Dummy, class _Deleter = typename __dependent_type<
                             __identity<deleter_type>, _Dummy>::type>
  using _EnableIfDeleterDefaultConstructible =
      typename enable_if<is_default_constructible<_Deleter>::value &&
                         !is_pointer<_Deleter>::value>::type;

  template <class _ArgType>
  using _EnableIfDeleterConstructible =
      typename enable_if<is_constructible<deleter_type, _ArgType>::value>::type;

  template <class _Pp>
  using _EnableIfPointerConvertible = typename enable_if<
      _CheckArrayPointerConversion<_Pp>::value
  >::type;

  template <class _UPtr, class _Up,
        class _ElemT = typename _UPtr::element_type>
  using _EnableIfMoveConvertible = typename enable_if<
      is_array<_Up>::value &&
      is_same<pointer, element_type*>::value &&
      is_same<typename _UPtr::pointer, _ElemT*>::value &&
      is_convertible<_ElemT(*)[], element_type(*)[]>::value
    >::type;

  template <class _UDel>
  using _EnableIfDeleterConvertible = typename enable_if<
      (is_reference<_Dp>::value && is_same<_Dp, _UDel>::value) ||
      (!is_reference<_Dp>::value && is_convertible<_UDel, _Dp>::value)
    >::type;

  template <class _UDel>
  using _EnableIfDeleterAssignable = typename enable_if<
      is_assignable<_Dp&, _UDel&&>::value
    >::type;

public:
  template <bool _Dummy = true,
            class = _EnableIfDeleterDefaultConstructible<_Dummy>>
  _LIBCPP_INLINE_VISIBILITY
  constexpr unique_ptr() noexcept : __ptr_(pointer()) {}

  template <bool _Dummy = true,
            class = _EnableIfDeleterDefaultConstructible<_Dummy>>
  _LIBCPP_INLINE_VISIBILITY
  constexpr unique_ptr(nullptr_t) noexcept : __ptr_(pointer()) {}

  template <class _Pp, bool _Dummy = true,
            class = _EnableIfDeleterDefaultConstructible<_Dummy>,
            class = _EnableIfPointerConvertible<_Pp>>
  _LIBCPP_INLINE_VISIBILITY
  explicit unique_ptr(_Pp __p) noexcept
      : __ptr_(__p) {}

  template <class _Pp, bool _Dummy = true,
            class = _EnableIfDeleterConstructible<_LValRefType<_Dummy>>,
            class = _EnableIfPointerConvertible<_Pp>>
  _LIBCPP_INLINE_VISIBILITY
  unique_ptr(_Pp __p, _LValRefType<_Dummy> __d) noexcept
      : __ptr_(__p, __d) {}

  template <bool _Dummy = true,
            class = _EnableIfDeleterConstructible<_LValRefType<_Dummy>>>
  _LIBCPP_INLINE_VISIBILITY
  unique_ptr(nullptr_t, _LValRefType<_Dummy> __d) noexcept
      : __ptr_(nullptr, __d) {}

  template <class _Pp, bool _Dummy = true,
            class = _EnableIfDeleterConstructible<_GoodRValRefType<_Dummy>>,
            class = _EnableIfPointerConvertible<_Pp>>
  _LIBCPP_INLINE_VISIBILITY
  unique_ptr(_Pp __p, _GoodRValRefType<_Dummy> __d) noexcept
      : __ptr_(__p, _VSTD::move(__d)) {
    static_assert(!is_reference<deleter_type>::value,
                  "rvalue deleter bound to reference");
  }

  template <bool _Dummy = true,
            class = _EnableIfDeleterConstructible<_GoodRValRefType<_Dummy>>>
  _LIBCPP_INLINE_VISIBILITY
  unique_ptr(nullptr_t, _GoodRValRefType<_Dummy> __d) noexcept
      : __ptr_(nullptr, _VSTD::move(__d)) {
    static_assert(!is_reference<deleter_type>::value,
                  "rvalue deleter bound to reference");
  }

  template <class _Pp, bool _Dummy = true,
            class = _EnableIfDeleterConstructible<_BadRValRefType<_Dummy>>,
            class = _EnableIfPointerConvertible<_Pp>>
  _LIBCPP_INLINE_VISIBILITY
  unique_ptr(_Pp __p, _BadRValRefType<_Dummy> __d) = delete;

  _LIBCPP_INLINE_VISIBILITY
  unique_ptr(unique_ptr&& __u) noexcept
      : __ptr_(__u.release(), _VSTD::forward<deleter_type>(__u.get_deleter())) {
  }

  _LIBCPP_INLINE_VISIBILITY
  unique_ptr& operator=(unique_ptr&& __u) noexcept {
    reset(__u.release());
    __ptr_.second() = _VSTD::forward<deleter_type>(__u.get_deleter());
    return *this;
  }

  template <class _Up, class _Ep,
      class = _EnableIfMoveConvertible<unique_ptr<_Up, _Ep>, _Up>,
      class = _EnableIfDeleterConvertible<_Ep>
  >
  _LIBCPP_INLINE_VISIBILITY
  unique_ptr(unique_ptr<_Up, _Ep>&& __u) noexcept
      : __ptr_(__u.release(), _VSTD::forward<_Ep>(__u.get_deleter())) {
  }

  template <class _Up, class _Ep,
      class = _EnableIfMoveConvertible<unique_ptr<_Up, _Ep>, _Up>,
      class = _EnableIfDeleterAssignable<_Ep>
  >
  _LIBCPP_INLINE_VISIBILITY
  unique_ptr&
  operator=(unique_ptr<_Up, _Ep>&& __u) noexcept {
    reset(__u.release());
    __ptr_.second() = _VSTD::forward<_Ep>(__u.get_deleter());
    return *this;
  }

#else // _LIBCPP_CXX03_LANG
private:
  template <class _Up> explicit unique_ptr(_Up);

  unique_ptr(unique_ptr&);
  template <class _Up> unique_ptr(unique_ptr<_Up>&);

  unique_ptr& operator=(unique_ptr&);
  template <class _Up> unique_ptr& operator=(unique_ptr<_Up>&);

  template <class _Up>
  unique_ptr(_Up __u,
             typename conditional<
                 is_reference<deleter_type>::value, deleter_type,
                 typename add_lvalue_reference<const deleter_type>::type>::type,
             typename enable_if<is_convertible<_Up, pointer>::value,
                                __nat>::type = __nat());
public:
  _LIBCPP_INLINE_VISIBILITY
  unique_ptr() : __ptr_(pointer()) {
    static_assert(!is_pointer<deleter_type>::value,
                  "unique_ptr constructed with null function pointer deleter");
  }
  _LIBCPP_INLINE_VISIBILITY
  unique_ptr(nullptr_t) : __ptr_(pointer()) {
    static_assert(!is_pointer<deleter_type>::value,
                  "unique_ptr constructed with null function pointer deleter");
  }

  _LIBCPP_INLINE_VISIBILITY
  explicit unique_ptr(pointer __p) : __ptr_(__p) {
    static_assert(!is_pointer<deleter_type>::value,
                  "unique_ptr constructed with null function pointer deleter");
  }

  _LIBCPP_INLINE_VISIBILITY
  unique_ptr(pointer __p, deleter_type __d)
      : __ptr_(__p, _VSTD::forward<deleter_type>(__d)) {}

  _LIBCPP_INLINE_VISIBILITY
  unique_ptr(nullptr_t, deleter_type __d)
      : __ptr_(pointer(), _VSTD::forward<deleter_type>(__d)) {}

  _LIBCPP_INLINE_VISIBILITY
  operator __rv<unique_ptr>() {
    return __rv<unique_ptr>(*this);
  }

  _LIBCPP_INLINE_VISIBILITY
  unique_ptr(__rv<unique_ptr> __u)
      : __ptr_(__u->release(),
               _VSTD::forward<deleter_type>(__u->get_deleter())) {}

  _LIBCPP_INLINE_VISIBILITY
  unique_ptr& operator=(__rv<unique_ptr> __u) {
    reset(__u->release());
    __ptr_.second() = _VSTD::forward<deleter_type>(__u->get_deleter());
    return *this;
  }

#endif // _LIBCPP_CXX03_LANG

public:
  _LIBCPP_INLINE_VISIBILITY
  ~unique_ptr() { reset(); }

  _LIBCPP_INLINE_VISIBILITY
  unique_ptr& operator=(nullptr_t) _NOEXCEPT {
    reset();
    return *this;
  }

  _LIBCPP_INLINE_VISIBILITY
  typename add_lvalue_reference<_Tp>::type
  operator[](size_t __i) const {
    return __ptr_.first()[__i];
  }
  _LIBCPP_INLINE_VISIBILITY
  pointer get() const _NOEXCEPT {
    return __ptr_.first();
  }

  _LIBCPP_INLINE_VISIBILITY
  deleter_type& get_deleter() _NOEXCEPT {
    return __ptr_.second();
  }

  _LIBCPP_INLINE_VISIBILITY
  const deleter_type& get_deleter() const _NOEXCEPT {
    return __ptr_.second();
  }
  _LIBCPP_INLINE_VISIBILITY
  _LIBCPP_EXPLICIT operator bool() const _NOEXCEPT {
    return __ptr_.first() != nullptr;
  }

  _LIBCPP_INLINE_VISIBILITY
  pointer release() _NOEXCEPT {
    pointer __t = __ptr_.first();
    __ptr_.first() = pointer();
    return __t;
  }

  template <class _Pp>
  _LIBCPP_INLINE_VISIBILITY
  typename enable_if<
      _CheckArrayPointerConversion<_Pp>::value
  >::type
  reset(_Pp __p) _NOEXCEPT {
    pointer __tmp = __ptr_.first();
    __ptr_.first() = __p;
    if (__tmp)
      __ptr_.second()(__tmp);
  }

  _LIBCPP_INLINE_VISIBILITY
  void reset(nullptr_t = nullptr) _NOEXCEPT {
    pointer __tmp = __ptr_.first();
    __ptr_.first() = nullptr;
    if (__tmp)
      __ptr_.second()(__tmp);
  }

  _LIBCPP_INLINE_VISIBILITY
  void swap(unique_ptr& __u) _NOEXCEPT {
    __ptr_.swap(__u.__ptr_);
  }

};

template <class _Tp, class _Dp>
inline _LIBCPP_INLINE_VISIBILITY
typename enable_if<
    __is_swappable<_Dp>::value,
    void
>::type
swap(unique_ptr<_Tp, _Dp>& __x, unique_ptr<_Tp, _Dp>& __y) _NOEXCEPT {__x.swap(__y);}

template <class _T1, class _D1, class _T2, class _D2>
inline _LIBCPP_INLINE_VISIBILITY
bool
operator==(const unique_ptr<_T1, _D1>& __x, const unique_ptr<_T2, _D2>& __y) {return __x.get() == __y.get();}

template <class _T1, class _D1, class _T2, class _D2>
inline _LIBCPP_INLINE_VISIBILITY
bool
operator!=(const unique_ptr<_T1, _D1>& __x, const unique_ptr<_T2, _D2>& __y) {return !(__x == __y);}

template <class _T1, class _D1, class _T2, class _D2>
inline _LIBCPP_INLINE_VISIBILITY
bool
operator< (const unique_ptr<_T1, _D1>& __x, const unique_ptr<_T2, _D2>& __y)
{
    typedef typename unique_ptr<_T1, _D1>::pointer _P1;
    typedef typename unique_ptr<_T2, _D2>::pointer _P2;
    typedef typename common_type<_P1, _P2>::type _Vp;
    return less<_Vp>()(__x.get(), __y.get());
}

template <class _T1, class _D1, class _T2, class _D2>
inline _LIBCPP_INLINE_VISIBILITY
bool
operator> (const unique_ptr<_T1, _D1>& __x, const unique_ptr<_T2, _D2>& __y) {return __y < __x;}

template <class _T1, class _D1, class _T2, class _D2>
inline _LIBCPP_INLINE_VISIBILITY
bool
operator<=(const unique_ptr<_T1, _D1>& __x, const unique_ptr<_T2, _D2>& __y) {return !(__y < __x);}

template <class _T1, class _D1, class _T2, class _D2>
inline _LIBCPP_INLINE_VISIBILITY
bool
operator>=(const unique_ptr<_T1, _D1>& __x, const unique_ptr<_T2, _D2>& __y) {return !(__x < __y);}

template <class _T1, class _D1>
inline _LIBCPP_INLINE_VISIBILITY
bool
operator==(const unique_ptr<_T1, _D1>& __x, nullptr_t) _NOEXCEPT
{
    return !__x;
}

template <class _T1, class _D1>
inline _LIBCPP_INLINE_VISIBILITY
bool
operator==(nullptr_t, const unique_ptr<_T1, _D1>& __x) _NOEXCEPT
{
    return !__x;
}

template <class _T1, class _D1>
inline _LIBCPP_INLINE_VISIBILITY
bool
operator!=(const unique_ptr<_T1, _D1>& __x, nullptr_t) _NOEXCEPT
{
    return static_cast<bool>(__x);
}

template <class _T1, class _D1>
inline _LIBCPP_INLINE_VISIBILITY
bool
operator!=(nullptr_t, const unique_ptr<_T1, _D1>& __x) _NOEXCEPT
{
    return static_cast<bool>(__x);
}

template <class _T1, class _D1>
inline _LIBCPP_INLINE_VISIBILITY
bool
operator<(const unique_ptr<_T1, _D1>& __x, nullptr_t)
{
    typedef typename unique_ptr<_T1, _D1>::pointer _P1;
    return less<_P1>()(__x.get(), nullptr);
}

template <class _T1, class _D1>
inline _LIBCPP_INLINE_VISIBILITY
bool
operator<(nullptr_t, const unique_ptr<_T1, _D1>& __x)
{
    typedef typename unique_ptr<_T1, _D1>::pointer _P1;
    return less<_P1>()(nullptr, __x.get());
}

template <class _T1, class _D1>
inline _LIBCPP_INLINE_VISIBILITY
bool
operator>(const unique_ptr<_T1, _D1>& __x, nullptr_t)
{
    return nullptr < __x;
}

template <class _T1, class _D1>
inline _LIBCPP_INLINE_VISIBILITY
bool
operator>(nullptr_t, const unique_ptr<_T1, _D1>& __x)
{
    return __x < nullptr;
}

template <class _T1, class _D1>
inline _LIBCPP_INLINE_VISIBILITY
bool
operator<=(const unique_ptr<_T1, _D1>& __x, nullptr_t)
{
    return !(nullptr < __x);
}

template <class _T1, class _D1>
inline _LIBCPP_INLINE_VISIBILITY
bool
operator<=(nullptr_t, const unique_ptr<_T1, _D1>& __x)
{
    return !(__x < nullptr);
}

template <class _T1, class _D1>
inline _LIBCPP_INLINE_VISIBILITY
bool
operator>=(const unique_ptr<_T1, _D1>& __x, nullptr_t)
{
    return !(__x < nullptr);
}

template <class _T1, class _D1>
inline _LIBCPP_INLINE_VISIBILITY
bool
operator>=(nullptr_t, const unique_ptr<_T1, _D1>& __x)
{
    return !(nullptr < __x);
}

#ifdef _LIBCPP_HAS_NO_RVALUE_REFERENCES

template <class _Tp, class _Dp>
inline _LIBCPP_INLINE_VISIBILITY
unique_ptr<_Tp, _Dp>
move(unique_ptr<_Tp, _Dp>& __t)
{
    return unique_ptr<_Tp, _Dp>(__rv<unique_ptr<_Tp, _Dp> >(__t));
}

#endif

#if _LIBCPP_STD_VER > 11

template<class _Tp>
struct __unique_if
{
    typedef unique_ptr<_Tp> __unique_single;
};

template<class _Tp>
struct __unique_if<_Tp[]>
{
    typedef unique_ptr<_Tp[]> __unique_array_unknown_bound;
};

template<class _Tp, size_t _Np>
struct __unique_if<_Tp[_Np]>
{
    typedef void __unique_array_known_bound;
};

template<class _Tp, class... _Args>
inline _LIBCPP_INLINE_VISIBILITY
typename __unique_if<_Tp>::__unique_single
make_unique(_Args&&... __args)
{
    return unique_ptr<_Tp>(new _Tp(_VSTD::forward<_Args>(__args)...));
}

template<class _Tp>
inline _LIBCPP_INLINE_VISIBILITY
typename __unique_if<_Tp>::__unique_array_unknown_bound
make_unique(size_t __n)
{
    typedef typename remove_extent<_Tp>::type _Up;
    return unique_ptr<_Tp>(new _Up[__n]());
}

template<class _Tp, class... _Args>
    typename __unique_if<_Tp>::__unique_array_known_bound
    make_unique(_Args&&...) = delete;

#endif  // _LIBCPP_STD_VER > 11

template <class _Tp, class _Dp>
#ifdef _LIBCPP_CXX03_LANG
struct _LIBCPP_TEMPLATE_VIS hash<unique_ptr<_Tp, _Dp> >
#else
struct _LIBCPP_TEMPLATE_VIS hash<__enable_hash_helper<
    unique_ptr<_Tp, _Dp>, typename unique_ptr<_Tp, _Dp>::pointer>>
#endif
{
    typedef unique_ptr<_Tp, _Dp> argument_type;
    typedef size_t               result_type;
    _LIBCPP_INLINE_VISIBILITY
    result_type operator()(const argument_type& __ptr) const
    {
        typedef typename argument_type::pointer pointer;
        return hash<pointer>()(__ptr.get());
    }
};

struct __destruct_n
{
private:
    size_t __size_;

    template <class _Tp>
    _LIBCPP_INLINE_VISIBILITY void __process(_Tp* __p, false_type) _NOEXCEPT
        {for (size_t __i = 0; __i < __size_; ++__i, ++__p) __p->~_Tp();}

    template <class _Tp>
    _LIBCPP_INLINE_VISIBILITY void __process(_Tp*, true_type) _NOEXCEPT
        {}

    _LIBCPP_INLINE_VISIBILITY void __incr(false_type) _NOEXCEPT
        {++__size_;}
    _LIBCPP_INLINE_VISIBILITY void __incr(true_type) _NOEXCEPT
        {}

    _LIBCPP_INLINE_VISIBILITY void __set(size_t __s, false_type) _NOEXCEPT
        {__size_ = __s;}
    _LIBCPP_INLINE_VISIBILITY void __set(size_t, true_type) _NOEXCEPT
        {}
public:
    _LIBCPP_INLINE_VISIBILITY explicit __destruct_n(size_t __s) _NOEXCEPT
        : __size_(__s) {}

    template <class _Tp>
    _LIBCPP_INLINE_VISIBILITY void __incr(_Tp*) _NOEXCEPT
        {__incr(integral_constant<bool, is_trivially_destructible<_Tp>::value>());}

    template <class _Tp>
    _LIBCPP_INLINE_VISIBILITY void __set(size_t __s, _Tp*) _NOEXCEPT
        {__set(__s, integral_constant<bool, is_trivially_destructible<_Tp>::value>());}

    template <class _Tp>
    _LIBCPP_INLINE_VISIBILITY void operator()(_Tp* __p) _NOEXCEPT
        {__process(__p, integral_constant<bool, is_trivially_destructible<_Tp>::value>());}
};

template <class _Alloc>
class __allocator_destructor
{
    typedef allocator_traits<_Alloc> __alloc_traits;
public:
    typedef typename __alloc_traits::pointer pointer;
    typedef typename __alloc_traits::size_type size_type;
private:
    _Alloc& __alloc_;
    size_type __s_;
public:
    _LIBCPP_INLINE_VISIBILITY __allocator_destructor(_Alloc& __a, size_type __s)
             _NOEXCEPT
        : __alloc_(__a), __s_(__s) {}
    _LIBCPP_INLINE_VISIBILITY
    void operator()(pointer __p) _NOEXCEPT
        {__alloc_traits::deallocate(__alloc_, __p, __s_);}
};

template <class _InputIterator, class _ForwardIterator>
_ForwardIterator
uninitialized_copy(_InputIterator __f, _InputIterator __l, _ForwardIterator __r)
{
    typedef typename iterator_traits<_ForwardIterator>::value_type value_type;
#ifndef _LIBCPP_NO_EXCEPTIONS
    _ForwardIterator __s = __r;
    try
    {
#endif
        for (; __f != __l; ++__f, (void) ++__r)
            ::new (static_cast<void*>(_VSTD::addressof(*__r))) value_type(*__f);
#ifndef _LIBCPP_NO_EXCEPTIONS
    }
    catch (...)
    {
        for (; __s != __r; ++__s)
            __s->~value_type();
        throw;
    }
#endif
    return __r;
}

template <class _InputIterator, class _Size, class _ForwardIterator>
_ForwardIterator
uninitialized_copy_n(_InputIterator __f, _Size __n, _ForwardIterator __r)
{
    typedef typename iterator_traits<_ForwardIterator>::value_type value_type;
#ifndef _LIBCPP_NO_EXCEPTIONS
    _ForwardIterator __s = __r;
    try
    {
#endif
        for (; __n > 0; ++__f, (void) ++__r, (void) --__n)
            ::new (static_cast<void*>(_VSTD::addressof(*__r))) value_type(*__f);
#ifndef _LIBCPP_NO_EXCEPTIONS
    }
    catch (...)
    {
        for (; __s != __r; ++__s)
            __s->~value_type();
        throw;
    }
#endif
    return __r;
}

template <class _ForwardIterator, class _Tp>
void
uninitialized_fill(_ForwardIterator __f, _ForwardIterator __l, const _Tp& __x)
{
    typedef typename iterator_traits<_ForwardIterator>::value_type value_type;
#ifndef _LIBCPP_NO_EXCEPTIONS
    _ForwardIterator __s = __f;
    try
    {
#endif
        for (; __f != __l; ++__f)
            ::new (static_cast<void*>(_VSTD::addressof(*__f))) value_type(__x);
#ifndef _LIBCPP_NO_EXCEPTIONS
    }
    catch (...)
    {
        for (; __s != __f; ++__s)
            __s->~value_type();
        throw;
    }
#endif
}

template <class _ForwardIterator, class _Size, class _Tp>
_ForwardIterator
uninitialized_fill_n(_ForwardIterator __f, _Size __n, const _Tp& __x)
{
    typedef typename iterator_traits<_ForwardIterator>::value_type value_type;
#ifndef _LIBCPP_NO_EXCEPTIONS
    _ForwardIterator __s = __f;
    try
    {
#endif
        for (; __n > 0; ++__f, (void) --__n)
            ::new (static_cast<void*>(_VSTD::addressof(*__f))) value_type(__x);
#ifndef _LIBCPP_NO_EXCEPTIONS
    }
    catch (...)
    {
        for (; __s != __f; ++__s)
            __s->~value_type();
        throw;
    }
#endif
    return __f;
}

#if _LIBCPP_STD_VER > 14

template <class _Tp>
inline _LIBCPP_INLINE_VISIBILITY
void destroy_at(_Tp* __loc) {
    _LIBCPP_ASSERT(__loc, "null pointer given to destroy_at");
    __loc->~_Tp();
}

template <class _ForwardIterator>
inline _LIBCPP_INLINE_VISIBILITY
void destroy(_ForwardIterator __first, _ForwardIterator __last) {
    for (; __first != __last; ++__first)
        _VSTD::destroy_at(_VSTD::addressof(*__first));
}

template <class _ForwardIterator, class _Size>
inline _LIBCPP_INLINE_VISIBILITY
_ForwardIterator destroy_n(_ForwardIterator __first, _Size __n) {
    for (; __n > 0; (void)++__first, --__n)
        _VSTD::destroy_at(_VSTD::addressof(*__first));
    return __first;
}

template <class _ForwardIterator>
inline _LIBCPP_INLINE_VISIBILITY
void uninitialized_default_construct(_ForwardIterator __first, _ForwardIterator __last) {
    using _Vt = typename iterator_traits<_ForwardIterator>::value_type;
    auto __idx = __first;
#ifndef _LIBCPP_NO_EXCEPTIONS
    try {
#endif
    for (; __idx != __last; ++__idx)
        ::new((void*)_VSTD::addressof(*__idx)) _Vt;
#ifndef _LIBCPP_NO_EXCEPTIONS
    } catch (...) {
        _VSTD::destroy(__first, __idx);
        throw;
    }
#endif
}

template <class _ForwardIterator, class _Size>
inline _LIBCPP_INLINE_VISIBILITY
_ForwardIterator uninitialized_default_construct_n(_ForwardIterator __first, _Size __n) {
    using _Vt = typename iterator_traits<_ForwardIterator>::value_type;
    auto __idx = __first;
#ifndef _LIBCPP_NO_EXCEPTIONS
    try {
#endif
    for (; __n > 0; (void)++__idx, --__n)
        ::new((void*)_VSTD::addressof(*__idx)) _Vt;
    return __idx;
#ifndef _LIBCPP_NO_EXCEPTIONS
    } catch (...) {
        _VSTD::destroy(__first, __idx);
        throw;
    }
#endif
}


template <class _ForwardIterator>
inline _LIBCPP_INLINE_VISIBILITY
void uninitialized_value_construct(_ForwardIterator __first, _ForwardIterator __last) {
    using _Vt = typename iterator_traits<_ForwardIterator>::value_type;
    auto __idx = __first;
#ifndef _LIBCPP_NO_EXCEPTIONS
    try {
#endif
    for (; __idx != __last; ++__idx)
        ::new((void*)_VSTD::addressof(*__idx)) _Vt();
#ifndef _LIBCPP_NO_EXCEPTIONS
    } catch (...) {
        _VSTD::destroy(__first, __idx);
        throw;
    }
#endif
}

template <class _ForwardIterator, class _Size>
inline _LIBCPP_INLINE_VISIBILITY
_ForwardIterator uninitialized_value_construct_n(_ForwardIterator __first, _Size __n) {
    using _Vt = typename iterator_traits<_ForwardIterator>::value_type;
    auto __idx = __first;
#ifndef _LIBCPP_NO_EXCEPTIONS
    try {
#endif
    for (; __n > 0; (void)++__idx, --__n)
        ::new((void*)_VSTD::addressof(*__idx)) _Vt();
    return __idx;
#ifndef _LIBCPP_NO_EXCEPTIONS
    } catch (...) {
        _VSTD::destroy(__first, __idx);
        throw;
    }
#endif
}


template <class _InputIt, class _ForwardIt>
inline _LIBCPP_INLINE_VISIBILITY
_ForwardIt uninitialized_move(_InputIt __first, _InputIt __last, _ForwardIt __first_res) {
    using _Vt = typename iterator_traits<_ForwardIt>::value_type;
    auto __idx = __first_res;
#ifndef _LIBCPP_NO_EXCEPTIONS
    try {
#endif
    for (; __first != __last; (void)++__idx, ++__first)
        ::new((void*)_VSTD::addressof(*__idx)) _Vt(std::move(*__first));
    return __idx;
#ifndef _LIBCPP_NO_EXCEPTIONS
    } catch (...) {
        _VSTD::destroy(__first_res, __idx);
        throw;
    }
#endif
}

template <class _InputIt, class _Size, class _ForwardIt>
inline _LIBCPP_INLINE_VISIBILITY
pair<_InputIt, _ForwardIt>
uninitialized_move_n(_InputIt __first, _Size __n, _ForwardIt __first_res) {
    using _Vt = typename iterator_traits<_ForwardIt>::value_type;
    auto __idx = __first_res;
#ifndef _LIBCPP_NO_EXCEPTIONS
    try {
#endif
    for (; __n > 0; ++__idx, (void)++__first, --__n)
        ::new((void*)_VSTD::addressof(*__idx)) _Vt(std::move(*__first));
    return {__first, __idx};
#ifndef _LIBCPP_NO_EXCEPTIONS
    } catch (...) {
        _VSTD::destroy(__first_res, __idx);
        throw;
    }
#endif
}


#endif // _LIBCPP_STD_VER > 14

// NOTE: Relaxed and acq/rel atomics (for increment and decrement respectively)
// should be sufficient for thread safety.
// See https://bugs.llvm.org/show_bug.cgi?id=22803
#if defined(__clang__) && __has_builtin(__atomic_add_fetch)          \
                       && defined(__ATOMIC_RELAXED)                  \
                       && defined(__ATOMIC_ACQ_REL)
#   define _LIBCPP_HAS_BUILTIN_ATOMIC_SUPPORT
#elif !defined(__clang__) && defined(_GNUC_VER) && _GNUC_VER >= 407
#   define _LIBCPP_HAS_BUILTIN_ATOMIC_SUPPORT
#endif

template <class _Tp>
inline _LIBCPP_INLINE_VISIBILITY _Tp
__libcpp_atomic_refcount_increment(_Tp& __t) _NOEXCEPT
{
#if defined(_LIBCPP_HAS_BUILTIN_ATOMIC_SUPPORT) && !defined(_LIBCPP_HAS_NO_THREADS)
    return __atomic_add_fetch(&__t, 1, __ATOMIC_RELAXED);
#else
    return __t += 1;
#endif
}

template <class _Tp>
inline _LIBCPP_INLINE_VISIBILITY _Tp
__libcpp_atomic_refcount_decrement(_Tp& __t) _NOEXCEPT
{
#if defined(_LIBCPP_HAS_BUILTIN_ATOMIC_SUPPORT) && !defined(_LIBCPP_HAS_NO_THREADS)
    return __atomic_add_fetch(&__t, -1, __ATOMIC_ACQ_REL);
#else
    return __t -= 1;
#endif
}

class _LIBCPP_EXCEPTION_ABI bad_weak_ptr
    : public std::exception
{
public:
    virtual ~bad_weak_ptr() _NOEXCEPT;
    virtual const char* what() const  _NOEXCEPT;
};

_LIBCPP_NORETURN inline _LIBCPP_ALWAYS_INLINE
void __throw_bad_weak_ptr()
{
#ifndef _LIBCPP_NO_EXCEPTIONS
    throw bad_weak_ptr();
#else
    abort();
#endif
}

template<class _Tp> class _LIBCPP_TEMPLATE_VIS weak_ptr;

class _LIBCPP_TYPE_VIS __shared_count
{
    __shared_count(const __shared_count&);
    __shared_count& operator=(const __shared_count&);

protected:
    long __shared_owners_;
    virtual ~__shared_count();
private:
    virtual void __on_zero_shared() _NOEXCEPT = 0;

public:
    _LIBCPP_INLINE_VISIBILITY
    explicit __shared_count(long __refs = 0) _NOEXCEPT
        : __shared_owners_(__refs) {}

#if defined(_LIBCPP_BUILDING_MEMORY) && \
    defined(_LIBCPP_DEPRECATED_ABI_LEGACY_LIBRARY_DEFINITIONS_FOR_INLINE_FUNCTIONS)
    void __add_shared() _NOEXCEPT;
    bool __release_shared() _NOEXCEPT;
#else
    _LIBCPP_INLINE_VISIBILITY
    void __add_shared() _NOEXCEPT {
      __libcpp_atomic_refcount_increment(__shared_owners_);
    }
    _LIBCPP_INLINE_VISIBILITY
    bool __release_shared() _NOEXCEPT {
      if (__libcpp_atomic_refcount_decrement(__shared_owners_) == -1) {
        __on_zero_shared();
        return true;
      }
      return false;
    }
#endif
    _LIBCPP_INLINE_VISIBILITY
    long use_count() const _NOEXCEPT {
        return __libcpp_relaxed_load(&__shared_owners_) + 1;
    }
};

class _LIBCPP_TYPE_VIS __shared_weak_count
    : private __shared_count
{
    long __shared_weak_owners_;

public:
    _LIBCPP_INLINE_VISIBILITY
    explicit __shared_weak_count(long __refs = 0) _NOEXCEPT
        : __shared_count(__refs),
          __shared_weak_owners_(__refs) {}
protected:
    virtual ~__shared_weak_count();

public:
#if defined(_LIBCPP_BUILDING_MEMORY) && \
    defined(_LIBCPP_DEPRECATED_ABI_LEGACY_LIBRARY_DEFINITIONS_FOR_INLINE_FUNCTIONS)
    void __add_shared() _NOEXCEPT;
    void __add_weak() _NOEXCEPT;
    void __release_shared() _NOEXCEPT;
#else
    _LIBCPP_INLINE_VISIBILITY
    void __add_shared() _NOEXCEPT {
      __shared_count::__add_shared();
    }
    _LIBCPP_INLINE_VISIBILITY
    void __add_weak() _NOEXCEPT {
      __libcpp_atomic_refcount_increment(__shared_weak_owners_);
    }
    _LIBCPP_INLINE_VISIBILITY
    void __release_shared() _NOEXCEPT {
      if (__shared_count::__release_shared())
        __release_weak();
    }
#endif
    void __release_weak() _NOEXCEPT;
    _LIBCPP_INLINE_VISIBILITY
    long use_count() const _NOEXCEPT {return __shared_count::use_count();}
    __shared_weak_count* lock() _NOEXCEPT;

    // Define the function out only if we build static libc++ without RTTI.
    // Otherwise we may break clients who need to compile their projects with
    // -fno-rtti and yet link against a libc++.dylib compiled
    // without -fno-rtti.
#if !defined(_LIBCPP_NO_RTTI) || !defined(_LIBCPP_BUILD_STATIC)
    virtual const void* __get_deleter(const type_info&) const _NOEXCEPT;
#endif
private:
    virtual void __on_zero_shared_weak() _NOEXCEPT = 0;
};

template <class _Tp, class _Dp, class _Alloc>
class __shared_ptr_pointer
    : public __shared_weak_count
{
    __compressed_pair<__compressed_pair<_Tp, _Dp>, _Alloc> __data_;
public:
    _LIBCPP_INLINE_VISIBILITY
    __shared_ptr_pointer(_Tp __p, _Dp __d, _Alloc __a)
        :  __data_(__compressed_pair<_Tp, _Dp>(__p, _VSTD::move(__d)), _VSTD::move(__a)) {}

#ifndef _LIBCPP_NO_RTTI
    virtual const void* __get_deleter(const type_info&) const _NOEXCEPT;
#endif

private:
    virtual void __on_zero_shared() _NOEXCEPT;
    virtual void __on_zero_shared_weak() _NOEXCEPT;
};

#ifndef _LIBCPP_NO_RTTI

template <class _Tp, class _Dp, class _Alloc>
const void*
__shared_ptr_pointer<_Tp, _Dp, _Alloc>::__get_deleter(const type_info& __t) const _NOEXCEPT
{
    return __t == typeid(_Dp) ? _VSTD::addressof(__data_.first().second()) : nullptr;
}

#endif  // _LIBCPP_NO_RTTI

template <class _Tp, class _Dp, class _Alloc>
void
__shared_ptr_pointer<_Tp, _Dp, _Alloc>::__on_zero_shared() _NOEXCEPT
{
    __data_.first().second()(__data_.first().first());
    __data_.first().second().~_Dp();
}

template <class _Tp, class _Dp, class _Alloc>
void
__shared_ptr_pointer<_Tp, _Dp, _Alloc>::__on_zero_shared_weak() _NOEXCEPT
{
    typedef typename __allocator_traits_rebind<_Alloc, __shared_ptr_pointer>::type _Al;
    typedef allocator_traits<_Al> _ATraits;
    typedef pointer_traits<typename _ATraits::pointer> _PTraits;

    _Al __a(__data_.second());
    __data_.second().~_Alloc();
    __a.deallocate(_PTraits::pointer_to(*this), 1);
}

template <class _Tp, class _Alloc>
class __shared_ptr_emplace
    : public __shared_weak_count
{
    __compressed_pair<_Alloc, _Tp> __data_;
public:
#ifndef _LIBCPP_HAS_NO_VARIADICS

    _LIBCPP_INLINE_VISIBILITY
    __shared_ptr_emplace(_Alloc __a)
        :  __data_(_VSTD::move(__a)) {}

    template <class ..._Args>
        _LIBCPP_INLINE_VISIBILITY
        __shared_ptr_emplace(_Alloc __a, _Args&& ...__args)
            :  __data_(piecewise_construct, _VSTD::forward_as_tuple(__a),
                   _VSTD::forward_as_tuple(_VSTD::forward<_Args>(__args)...)) {}

#else  // _LIBCPP_HAS_NO_VARIADICS

    _LIBCPP_INLINE_VISIBILITY
    __shared_ptr_emplace(_Alloc __a)
        :  __data_(__a) {}

    template <class _A0>
        _LIBCPP_INLINE_VISIBILITY
        __shared_ptr_emplace(_Alloc __a, _A0& __a0)
            :  __data_(__a, _Tp(__a0)) {}

    template <class _A0, class _A1>
        _LIBCPP_INLINE_VISIBILITY
        __shared_ptr_emplace(_Alloc __a, _A0& __a0, _A1& __a1)
            :  __data_(__a, _Tp(__a0, __a1)) {}

    template <class _A0, class _A1, class _A2>
        _LIBCPP_INLINE_VISIBILITY
        __shared_ptr_emplace(_Alloc __a, _A0& __a0, _A1& __a1, _A2& __a2)
            :  __data_(__a, _Tp(__a0, __a1, __a2)) {}

#endif  // _LIBCPP_HAS_NO_VARIADICS

private:
    virtual void __on_zero_shared() _NOEXCEPT;
    virtual void __on_zero_shared_weak() _NOEXCEPT;
public:
    _LIBCPP_INLINE_VISIBILITY
    _Tp* get() _NOEXCEPT {return &__data_.second();}
};

template <class _Tp, class _Alloc>
void
__shared_ptr_emplace<_Tp, _Alloc>::__on_zero_shared() _NOEXCEPT
{
    __data_.second().~_Tp();
}

template <class _Tp, class _Alloc>
void
__shared_ptr_emplace<_Tp, _Alloc>::__on_zero_shared_weak() _NOEXCEPT
{
    typedef typename __allocator_traits_rebind<_Alloc, __shared_ptr_emplace>::type _Al;
    typedef allocator_traits<_Al> _ATraits;
    typedef pointer_traits<typename _ATraits::pointer> _PTraits;
    _Al __a(__data_.first());
    __data_.first().~_Alloc();
    __a.deallocate(_PTraits::pointer_to(*this), 1);
}

struct __shared_ptr_dummy_rebind_allocator_type;
template <>
class _LIBCPP_TEMPLATE_VIS allocator<__shared_ptr_dummy_rebind_allocator_type>
{
public:
    template <class _Other>
    struct rebind
    {
        typedef allocator<_Other> other;
    };
};

template<class _Tp> class _LIBCPP_TEMPLATE_VIS enable_shared_from_this;

template<class _Tp>
class _LIBCPP_TEMPLATE_VIS shared_ptr
{
public:
    typedef _Tp element_type;

#if _LIBCPP_STD_VER > 14
    typedef weak_ptr<_Tp> weak_type;
#endif
private:
    element_type*      __ptr_;
    __shared_weak_count* __cntrl_;

    struct __nat {int __for_bool_;};
public:
    _LIBCPP_INLINE_VISIBILITY
    _LIBCPP_CONSTEXPR shared_ptr() _NOEXCEPT;
    _LIBCPP_INLINE_VISIBILITY
    _LIBCPP_CONSTEXPR shared_ptr(nullptr_t) _NOEXCEPT;
    template<class _Yp>
        explicit shared_ptr(_Yp* __p,
                            typename enable_if<is_convertible<_Yp*, element_type*>::value, __nat>::type = __nat());
    template<class _Yp, class _Dp>
        shared_ptr(_Yp* __p, _Dp __d,
                   typename enable_if<is_convertible<_Yp*, element_type*>::value, __nat>::type = __nat());
    template<class _Yp, class _Dp, class _Alloc>
        shared_ptr(_Yp* __p, _Dp __d, _Alloc __a,
                   typename enable_if<is_convertible<_Yp*, element_type*>::value, __nat>::type = __nat());
    template <class _Dp> shared_ptr(nullptr_t __p, _Dp __d);
    template <class _Dp, class _Alloc> shared_ptr(nullptr_t __p, _Dp __d, _Alloc __a);
    template<class _Yp> _LIBCPP_INLINE_VISIBILITY shared_ptr(const shared_ptr<_Yp>& __r, element_type* __p) _NOEXCEPT;
    _LIBCPP_INLINE_VISIBILITY
    shared_ptr(const shared_ptr& __r) _NOEXCEPT;
    template<class _Yp>
        _LIBCPP_INLINE_VISIBILITY
        shared_ptr(const shared_ptr<_Yp>& __r,
                   typename enable_if<is_convertible<_Yp*, element_type*>::value, __nat>::type = __nat())
                       _NOEXCEPT;
#ifndef _LIBCPP_HAS_NO_RVALUE_REFERENCES
    _LIBCPP_INLINE_VISIBILITY
    shared_ptr(shared_ptr&& __r) _NOEXCEPT;
    template<class _Yp> _LIBCPP_INLINE_VISIBILITY  shared_ptr(shared_ptr<_Yp>&& __r,
                   typename enable_if<is_convertible<_Yp*, element_type*>::value, __nat>::type = __nat())
                       _NOEXCEPT;
#endif  // _LIBCPP_HAS_NO_RVALUE_REFERENCES
    template<class _Yp> explicit shared_ptr(const weak_ptr<_Yp>& __r,
                   typename enable_if<is_convertible<_Yp*, element_type*>::value, __nat>::type= __nat());
#if _LIBCPP_STD_VER <= 14 || defined(_LIBCPP_ENABLE_CXX17_REMOVED_AUTO_PTR)
#ifndef _LIBCPP_HAS_NO_RVALUE_REFERENCES
    template<class _Yp>
        shared_ptr(auto_ptr<_Yp>&& __r,
                   typename enable_if<is_convertible<_Yp*, element_type*>::value, __nat>::type = __nat());
#else
    template<class _Yp>
        shared_ptr(auto_ptr<_Yp> __r,
                   typename enable_if<is_convertible<_Yp*, element_type*>::value, __nat>::type = __nat());
#endif
#endif
#ifndef _LIBCPP_HAS_NO_RVALUE_REFERENCES
    template <class _Yp, class _Dp>
        shared_ptr(unique_ptr<_Yp, _Dp>&&,
                   typename enable_if
                   <
                       !is_lvalue_reference<_Dp>::value &&
                       !is_array<_Yp>::value &&
                       is_convertible<typename unique_ptr<_Yp, _Dp>::pointer, element_type*>::value,
                       __nat
                   >::type = __nat());
    template <class _Yp, class _Dp>
        shared_ptr(unique_ptr<_Yp, _Dp>&&,
                   typename enable_if
                   <
                       is_lvalue_reference<_Dp>::value &&
                       !is_array<_Yp>::value &&
                       is_convertible<typename unique_ptr<_Yp, _Dp>::pointer, element_type*>::value,
                       __nat
                   >::type = __nat());
#else  // _LIBCPP_HAS_NO_RVALUE_REFERENCES
    template <class _Yp, class _Dp>
        shared_ptr(unique_ptr<_Yp, _Dp>,
                   typename enable_if
                   <
                       !is_lvalue_reference<_Dp>::value &&
                       !is_array<_Yp>::value &&
                       is_convertible<typename unique_ptr<_Yp, _Dp>::pointer, element_type*>::value,
                       __nat
                   >::type = __nat());
    template <class _Yp, class _Dp>
        shared_ptr(unique_ptr<_Yp, _Dp>,
                   typename enable_if
                   <
                       is_lvalue_reference<_Dp>::value &&
                       !is_array<_Yp>::value &&
                       is_convertible<typename unique_ptr<_Yp, _Dp>::pointer, element_type*>::value,
                       __nat
                   >::type = __nat());
#endif  // _LIBCPP_HAS_NO_RVALUE_REFERENCES

    ~shared_ptr();

    _LIBCPP_INLINE_VISIBILITY
    shared_ptr& operator=(const shared_ptr& __r) _NOEXCEPT;
    template<class _Yp>
        typename enable_if
        <
            is_convertible<_Yp*, element_type*>::value,
            shared_ptr&
        >::type
        _LIBCPP_INLINE_VISIBILITY
        operator=(const shared_ptr<_Yp>& __r) _NOEXCEPT;
#ifndef _LIBCPP_HAS_NO_RVALUE_REFERENCES
    _LIBCPP_INLINE_VISIBILITY
    shared_ptr& operator=(shared_ptr&& __r) _NOEXCEPT;
    template<class _Yp>
        typename enable_if
        <
            is_convertible<_Yp*, element_type*>::value,
            shared_ptr<_Tp>&
        >::type
        _LIBCPP_INLINE_VISIBILITY
        operator=(shared_ptr<_Yp>&& __r);
#if _LIBCPP_STD_VER <= 14 || defined(_LIBCPP_ENABLE_CXX17_REMOVED_AUTO_PTR)
    template<class _Yp>
        _LIBCPP_INLINE_VISIBILITY
        typename enable_if
        <
            !is_array<_Yp>::value &&
            is_convertible<_Yp*, element_type*>::value,
            shared_ptr
        >::type&
        operator=(auto_ptr<_Yp>&& __r);
#endif
#else  // _LIBCPP_HAS_NO_RVALUE_REFERENCES
#if _LIBCPP_STD_VER <= 14 || defined(_LIBCPP_ENABLE_CXX17_REMOVED_AUTO_PTR)
    template<class _Yp>
        _LIBCPP_INLINE_VISIBILITY
        typename enable_if
        <
            !is_array<_Yp>::value &&
            is_convertible<_Yp*, element_type*>::value,
            shared_ptr&
        >::type
        operator=(auto_ptr<_Yp> __r);
#endif
#endif
    template <class _Yp, class _Dp>
        typename enable_if
        <
            !is_array<_Yp>::value &&
            is_convertible<typename unique_ptr<_Yp, _Dp>::pointer, element_type*>::value,
            shared_ptr&
        >::type
#ifndef _LIBCPP_HAS_NO_RVALUE_REFERENCES
        _LIBCPP_INLINE_VISIBILITY
        operator=(unique_ptr<_Yp, _Dp>&& __r);
#else  // _LIBCPP_HAS_NO_RVALUE_REFERENCES
        _LIBCPP_INLINE_VISIBILITY
        operator=(unique_ptr<_Yp, _Dp> __r);
#endif

    _LIBCPP_INLINE_VISIBILITY
    void swap(shared_ptr& __r) _NOEXCEPT;
    _LIBCPP_INLINE_VISIBILITY
    void reset() _NOEXCEPT;
    template<class _Yp>
        typename enable_if
        <
            is_convertible<_Yp*, element_type*>::value,
            void
        >::type
        _LIBCPP_INLINE_VISIBILITY
        reset(_Yp* __p);
    template<class _Yp, class _Dp>
        typename enable_if
        <
            is_convertible<_Yp*, element_type*>::value,
            void
        >::type
        _LIBCPP_INLINE_VISIBILITY
        reset(_Yp* __p, _Dp __d);
    template<class _Yp, class _Dp, class _Alloc>
        typename enable_if
        <
            is_convertible<_Yp*, element_type*>::value,
            void
        >::type
        _LIBCPP_INLINE_VISIBILITY
        reset(_Yp* __p, _Dp __d, _Alloc __a);

    _LIBCPP_INLINE_VISIBILITY
    element_type* get() const _NOEXCEPT {return __ptr_;}
    _LIBCPP_INLINE_VISIBILITY
    typename add_lvalue_reference<element_type>::type operator*() const _NOEXCEPT
        {return *__ptr_;}
    _LIBCPP_INLINE_VISIBILITY
    element_type* operator->() const _NOEXCEPT {return __ptr_;}
    _LIBCPP_INLINE_VISIBILITY
    long use_count() const _NOEXCEPT {return __cntrl_ ? __cntrl_->use_count() : 0;}
    _LIBCPP_INLINE_VISIBILITY
    bool unique() const _NOEXCEPT {return use_count() == 1;}
    _LIBCPP_INLINE_VISIBILITY
    _LIBCPP_EXPLICIT operator bool() const _NOEXCEPT {return get() != 0;}
    template <class _Up>
        _LIBCPP_INLINE_VISIBILITY
        bool owner_before(shared_ptr<_Up> const& __p) const _NOEXCEPT
        {return __cntrl_ < __p.__cntrl_;}
    template <class _Up>
        _LIBCPP_INLINE_VISIBILITY
        bool owner_before(weak_ptr<_Up> const& __p) const _NOEXCEPT
        {return __cntrl_ < __p.__cntrl_;}
    _LIBCPP_INLINE_VISIBILITY
    bool
    __owner_equivalent(const shared_ptr& __p) const
        {return __cntrl_ == __p.__cntrl_;}

#ifndef _LIBCPP_NO_RTTI
    template <class _Dp>
        _LIBCPP_INLINE_VISIBILITY
        _Dp* __get_deleter() const _NOEXCEPT
            {return static_cast<_Dp*>(__cntrl_
                    ? const_cast<void *>(__cntrl_->__get_deleter(typeid(_Dp)))
                      : nullptr);}
#endif  // _LIBCPP_NO_RTTI

#ifndef _LIBCPP_HAS_NO_VARIADICS

    template<class ..._Args>
        static
        shared_ptr<_Tp>
        make_shared(_Args&& ...__args);

    template<class _Alloc, class ..._Args>
        static
        shared_ptr<_Tp>
        allocate_shared(const _Alloc& __a, _Args&& ...__args);

#else  // _LIBCPP_HAS_NO_VARIADICS

    static shared_ptr<_Tp> make_shared();

    template<class _A0>
        static shared_ptr<_Tp> make_shared(_A0&);

    template<class _A0, class _A1>
        static shared_ptr<_Tp> make_shared(_A0&, _A1&);

    template<class _A0, class _A1, class _A2>
        static shared_ptr<_Tp> make_shared(_A0&, _A1&, _A2&);

    template<class _Alloc>
        static shared_ptr<_Tp>
        allocate_shared(const _Alloc& __a);

    template<class _Alloc, class _A0>
        static shared_ptr<_Tp>
        allocate_shared(const _Alloc& __a, _A0& __a0);

    template<class _Alloc, class _A0, class _A1>
        static shared_ptr<_Tp>
        allocate_shared(const _Alloc& __a, _A0& __a0, _A1& __a1);

    template<class _Alloc, class _A0, class _A1, class _A2>
        static shared_ptr<_Tp>
        allocate_shared(const _Alloc& __a, _A0& __a0, _A1& __a1, _A2& __a2);

#endif  // _LIBCPP_HAS_NO_VARIADICS

private:
    template <class _Yp, bool = is_function<_Yp>::value>
        struct __shared_ptr_default_allocator
        {
            typedef allocator<_Yp> type;
        };

    template <class _Yp>
        struct __shared_ptr_default_allocator<_Yp, true>
        {
            typedef allocator<__shared_ptr_dummy_rebind_allocator_type> type;
        };

    template <class _Yp, class _OrigPtr>
        _LIBCPP_INLINE_VISIBILITY
        typename enable_if<is_convertible<_OrigPtr*,
                                          const enable_shared_from_this<_Yp>*
        >::value,
            void>::type
        __enable_weak_this(const enable_shared_from_this<_Yp>* __e,
                           _OrigPtr* __ptr) _NOEXCEPT
        {
            typedef typename remove_cv<_Yp>::type _RawYp;
            if (__e && __e->__weak_this_.expired())
            {
                __e->__weak_this_ = shared_ptr<_RawYp>(*this,
                    const_cast<_RawYp*>(static_cast<const _Yp*>(__ptr)));
            }
        }

    _LIBCPP_INLINE_VISIBILITY void __enable_weak_this(...) _NOEXCEPT {}

    template <class _Up> friend class _LIBCPP_TEMPLATE_VIS shared_ptr;
    template <class _Up> friend class _LIBCPP_TEMPLATE_VIS weak_ptr;
};


template<class _Tp>
inline
_LIBCPP_CONSTEXPR
shared_ptr<_Tp>::shared_ptr() _NOEXCEPT
    : __ptr_(0),
      __cntrl_(0)
{
}

template<class _Tp>
inline
_LIBCPP_CONSTEXPR
shared_ptr<_Tp>::shared_ptr(nullptr_t) _NOEXCEPT
    : __ptr_(0),
      __cntrl_(0)
{
}

template<class _Tp>
template<class _Yp>
shared_ptr<_Tp>::shared_ptr(_Yp* __p,
                            typename enable_if<is_convertible<_Yp*, element_type*>::value, __nat>::type)
    : __ptr_(__p)
{
    unique_ptr<_Yp> __hold(__p);
    typedef typename __shared_ptr_default_allocator<_Yp>::type _AllocT;
    typedef __shared_ptr_pointer<_Yp*, default_delete<_Yp>, _AllocT > _CntrlBlk;
    __cntrl_ = new _CntrlBlk(__p, default_delete<_Yp>(), _AllocT());
    __hold.release();
    __enable_weak_this(__p, __p);
}

template<class _Tp>
template<class _Yp, class _Dp>
shared_ptr<_Tp>::shared_ptr(_Yp* __p, _Dp __d,
                            typename enable_if<is_convertible<_Yp*, element_type*>::value, __nat>::type)
    : __ptr_(__p)
{
#ifndef _LIBCPP_NO_EXCEPTIONS
    try
    {
#endif  // _LIBCPP_NO_EXCEPTIONS
        typedef typename __shared_ptr_default_allocator<_Yp>::type _AllocT;
        typedef __shared_ptr_pointer<_Yp*, _Dp, _AllocT > _CntrlBlk;
        __cntrl_ = new _CntrlBlk(__p, __d, _AllocT());
        __enable_weak_this(__p, __p);
#ifndef _LIBCPP_NO_EXCEPTIONS
    }
    catch (...)
    {
        __d(__p);
        throw;
    }
#endif  // _LIBCPP_NO_EXCEPTIONS
}

template<class _Tp>
template<class _Dp>
shared_ptr<_Tp>::shared_ptr(nullptr_t __p, _Dp __d)
    : __ptr_(0)
{
#ifndef _LIBCPP_NO_EXCEPTIONS
    try
    {
#endif  // _LIBCPP_NO_EXCEPTIONS
        typedef typename __shared_ptr_default_allocator<_Tp>::type _AllocT;
        typedef __shared_ptr_pointer<nullptr_t, _Dp, _AllocT > _CntrlBlk;
        __cntrl_ = new _CntrlBlk(__p, __d, _AllocT());
#ifndef _LIBCPP_NO_EXCEPTIONS
    }
    catch (...)
    {
        __d(__p);
        throw;
    }
#endif  // _LIBCPP_NO_EXCEPTIONS
}

template<class _Tp>
template<class _Yp, class _Dp, class _Alloc>
shared_ptr<_Tp>::shared_ptr(_Yp* __p, _Dp __d, _Alloc __a,
                            typename enable_if<is_convertible<_Yp*, element_type*>::value, __nat>::type)
    : __ptr_(__p)
{
#ifndef _LIBCPP_NO_EXCEPTIONS
    try
    {
#endif  // _LIBCPP_NO_EXCEPTIONS
        typedef __shared_ptr_pointer<_Yp*, _Dp, _Alloc> _CntrlBlk;
        typedef typename __allocator_traits_rebind<_Alloc, _CntrlBlk>::type _A2;
        typedef __allocator_destructor<_A2> _D2;
        _A2 __a2(__a);
        unique_ptr<_CntrlBlk, _D2> __hold2(__a2.allocate(1), _D2(__a2, 1));
        ::new(static_cast<void*>(_VSTD::addressof(*__hold2.get())))
            _CntrlBlk(__p, __d, __a);
        __cntrl_ = _VSTD::addressof(*__hold2.release());
        __enable_weak_this(__p, __p);
#ifndef _LIBCPP_NO_EXCEPTIONS
    }
    catch (...)
    {
        __d(__p);
        throw;
    }
#endif  // _LIBCPP_NO_EXCEPTIONS
}

template<class _Tp>
template<class _Dp, class _Alloc>
shared_ptr<_Tp>::shared_ptr(nullptr_t __p, _Dp __d, _Alloc __a)
    : __ptr_(0)
{
#ifndef _LIBCPP_NO_EXCEPTIONS
    try
    {
#endif  // _LIBCPP_NO_EXCEPTIONS
        typedef __shared_ptr_pointer<nullptr_t, _Dp, _Alloc> _CntrlBlk;
        typedef typename __allocator_traits_rebind<_Alloc, _CntrlBlk>::type _A2;
        typedef __allocator_destructor<_A2> _D2;
        _A2 __a2(__a);
        unique_ptr<_CntrlBlk, _D2> __hold2(__a2.allocate(1), _D2(__a2, 1));
        ::new(static_cast<void*>(_VSTD::addressof(*__hold2.get())))
            _CntrlBlk(__p, __d, __a);
        __cntrl_ = _VSTD::addressof(*__hold2.release());
#ifndef _LIBCPP_NO_EXCEPTIONS
    }
    catch (...)
    {
        __d(__p);
        throw;
    }
#endif  // _LIBCPP_NO_EXCEPTIONS
}

template<class _Tp>
template<class _Yp>
inline
shared_ptr<_Tp>::shared_ptr(const shared_ptr<_Yp>& __r, element_type *__p) _NOEXCEPT
    : __ptr_(__p),
      __cntrl_(__r.__cntrl_)
{
    if (__cntrl_)
        __cntrl_->__add_shared();
}

template<class _Tp>
inline
shared_ptr<_Tp>::shared_ptr(const shared_ptr& __r) _NOEXCEPT
    : __ptr_(__r.__ptr_),
      __cntrl_(__r.__cntrl_)
{
    if (__cntrl_)
        __cntrl_->__add_shared();
}

template<class _Tp>
template<class _Yp>
inline
shared_ptr<_Tp>::shared_ptr(const shared_ptr<_Yp>& __r,
                            typename enable_if<is_convertible<_Yp*, element_type*>::value, __nat>::type)
         _NOEXCEPT
    : __ptr_(__r.__ptr_),
      __cntrl_(__r.__cntrl_)
{
    if (__cntrl_)
        __cntrl_->__add_shared();
}

#ifndef _LIBCPP_HAS_NO_RVALUE_REFERENCES

template<class _Tp>
inline
shared_ptr<_Tp>::shared_ptr(shared_ptr&& __r) _NOEXCEPT
    : __ptr_(__r.__ptr_),
      __cntrl_(__r.__cntrl_)
{
    __r.__ptr_ = 0;
    __r.__cntrl_ = 0;
}

template<class _Tp>
template<class _Yp>
inline
shared_ptr<_Tp>::shared_ptr(shared_ptr<_Yp>&& __r,
                            typename enable_if<is_convertible<_Yp*, element_type*>::value, __nat>::type)
         _NOEXCEPT
    : __ptr_(__r.__ptr_),
      __cntrl_(__r.__cntrl_)
{
    __r.__ptr_ = 0;
    __r.__cntrl_ = 0;
}

#endif  // _LIBCPP_HAS_NO_RVALUE_REFERENCES

#if _LIBCPP_STD_VER <= 14 || defined(_LIBCPP_ENABLE_CXX17_REMOVED_AUTO_PTR)
template<class _Tp>
template<class _Yp>
#ifndef _LIBCPP_HAS_NO_RVALUE_REFERENCES
shared_ptr<_Tp>::shared_ptr(auto_ptr<_Yp>&& __r,
#else
shared_ptr<_Tp>::shared_ptr(auto_ptr<_Yp> __r,
#endif
                            typename enable_if<is_convertible<_Yp*, element_type*>::value, __nat>::type)
    : __ptr_(__r.get())
{
    typedef __shared_ptr_pointer<_Yp*, default_delete<_Yp>, allocator<_Yp> > _CntrlBlk;
    __cntrl_ = new _CntrlBlk(__r.get(), default_delete<_Yp>(), allocator<_Yp>());
    __enable_weak_this(__r.get(), __r.get());
    __r.release();
}
#endif

template<class _Tp>
template <class _Yp, class _Dp>
#ifndef _LIBCPP_HAS_NO_RVALUE_REFERENCES
shared_ptr<_Tp>::shared_ptr(unique_ptr<_Yp, _Dp>&& __r,
#else
shared_ptr<_Tp>::shared_ptr(unique_ptr<_Yp, _Dp> __r,
#endif
                            typename enable_if
                            <
                                !is_lvalue_reference<_Dp>::value &&
                                !is_array<_Yp>::value &&
                                is_convertible<typename unique_ptr<_Yp, _Dp>::pointer, element_type*>::value,
                                __nat
                            >::type)
    : __ptr_(__r.get())
{
#if _LIBCPP_STD_VER > 11
    if (__ptr_ == nullptr)
        __cntrl_ = nullptr;
    else
#endif
    {
        typedef typename __shared_ptr_default_allocator<_Yp>::type _AllocT;
        typedef __shared_ptr_pointer<_Yp*, _Dp, _AllocT > _CntrlBlk;
        __cntrl_ = new _CntrlBlk(__r.get(), __r.get_deleter(), _AllocT());
        __enable_weak_this(__r.get(), __r.get());
    }
    __r.release();
}

template<class _Tp>
template <class _Yp, class _Dp>
#ifndef _LIBCPP_HAS_NO_RVALUE_REFERENCES
shared_ptr<_Tp>::shared_ptr(unique_ptr<_Yp, _Dp>&& __r,
#else
shared_ptr<_Tp>::shared_ptr(unique_ptr<_Yp, _Dp> __r,
#endif
                            typename enable_if
                            <
                                is_lvalue_reference<_Dp>::value &&
                                !is_array<_Yp>::value &&
                                is_convertible<typename unique_ptr<_Yp, _Dp>::pointer, element_type*>::value,
                                __nat
                            >::type)
    : __ptr_(__r.get())
{
#if _LIBCPP_STD_VER > 11
    if (__ptr_ == nullptr)
        __cntrl_ = nullptr;
    else
#endif
    {
        typedef typename __shared_ptr_default_allocator<_Yp>::type _AllocT;
        typedef __shared_ptr_pointer<_Yp*,
                                     reference_wrapper<typename remove_reference<_Dp>::type>,
                                     _AllocT > _CntrlBlk;
        __cntrl_ = new _CntrlBlk(__r.get(), ref(__r.get_deleter()), _AllocT());
        __enable_weak_this(__r.get(), __r.get());
    }
    __r.release();
}

#ifndef _LIBCPP_HAS_NO_VARIADICS

template<class _Tp>
template<class ..._Args>
shared_ptr<_Tp>
shared_ptr<_Tp>::make_shared(_Args&& ...__args)
{
    typedef __shared_ptr_emplace<_Tp, allocator<_Tp> > _CntrlBlk;
    typedef allocator<_CntrlBlk> _A2;
    typedef __allocator_destructor<_A2> _D2;
    _A2 __a2;
    unique_ptr<_CntrlBlk, _D2> __hold2(__a2.allocate(1), _D2(__a2, 1));
    ::new(__hold2.get()) _CntrlBlk(__a2, _VSTD::forward<_Args>(__args)...);
    shared_ptr<_Tp> __r;
    __r.__ptr_ = __hold2.get()->get();
    __r.__cntrl_ = __hold2.release();
    __r.__enable_weak_this(__r.__ptr_, __r.__ptr_);
    return __r;
}

template<class _Tp>
template<class _Alloc, class ..._Args>
shared_ptr<_Tp>
shared_ptr<_Tp>::allocate_shared(const _Alloc& __a, _Args&& ...__args)
{
    typedef __shared_ptr_emplace<_Tp, _Alloc> _CntrlBlk;
    typedef typename __allocator_traits_rebind<_Alloc, _CntrlBlk>::type _A2;
    typedef __allocator_destructor<_A2> _D2;
    _A2 __a2(__a);
    unique_ptr<_CntrlBlk, _D2> __hold2(__a2.allocate(1), _D2(__a2, 1));
    ::new(static_cast<void*>(_VSTD::addressof(*__hold2.get())))
        _CntrlBlk(__a, _VSTD::forward<_Args>(__args)...);
    shared_ptr<_Tp> __r;
    __r.__ptr_ = __hold2.get()->get();
    __r.__cntrl_ = _VSTD::addressof(*__hold2.release());
    __r.__enable_weak_this(__r.__ptr_, __r.__ptr_);
    return __r;
}

#else  // _LIBCPP_HAS_NO_VARIADICS

template<class _Tp>
shared_ptr<_Tp>
shared_ptr<_Tp>::make_shared()
{
    typedef __shared_ptr_emplace<_Tp, allocator<_Tp> > _CntrlBlk;
    typedef allocator<_CntrlBlk> _Alloc2;
    typedef __allocator_destructor<_Alloc2> _D2;
    _Alloc2 __alloc2;
    unique_ptr<_CntrlBlk, _D2> __hold2(__alloc2.allocate(1), _D2(__alloc2, 1));
    ::new(__hold2.get()) _CntrlBlk(__alloc2);
    shared_ptr<_Tp> __r;
    __r.__ptr_ = __hold2.get()->get();
    __r.__cntrl_ = __hold2.release();
    __r.__enable_weak_this(__r.__ptr_, __r.__ptr_);
    return __r;
}

template<class _Tp>
template<class _A0>
shared_ptr<_Tp>
shared_ptr<_Tp>::make_shared(_A0& __a0)
{
    typedef __shared_ptr_emplace<_Tp, allocator<_Tp> > _CntrlBlk;
    typedef allocator<_CntrlBlk> _Alloc2;
    typedef __allocator_destructor<_Alloc2> _D2;
    _Alloc2 __alloc2;
    unique_ptr<_CntrlBlk, _D2> __hold2(__alloc2.allocate(1), _D2(__alloc2, 1));
    ::new(__hold2.get()) _CntrlBlk(__alloc2, __a0);
    shared_ptr<_Tp> __r;
    __r.__ptr_ = __hold2.get()->get();
    __r.__cntrl_ = __hold2.release();
    __r.__enable_weak_this(__r.__ptr_, __r.__ptr_);
    return __r;
}

template<class _Tp>
template<class _A0, class _A1>
shared_ptr<_Tp>
shared_ptr<_Tp>::make_shared(_A0& __a0, _A1& __a1)
{
    typedef __shared_ptr_emplace<_Tp, allocator<_Tp> > _CntrlBlk;
    typedef allocator<_CntrlBlk> _Alloc2;
    typedef __allocator_destructor<_Alloc2> _D2;
    _Alloc2 __alloc2;
    unique_ptr<_CntrlBlk, _D2> __hold2(__alloc2.allocate(1), _D2(__alloc2, 1));
    ::new(__hold2.get()) _CntrlBlk(__alloc2, __a0, __a1);
    shared_ptr<_Tp> __r;
    __r.__ptr_ = __hold2.get()->get();
    __r.__cntrl_ = __hold2.release();
    __r.__enable_weak_this(__r.__ptr_, __r.__ptr_);
    return __r;
}

template<class _Tp>
template<class _A0, class _A1, class _A2>
shared_ptr<_Tp>
shared_ptr<_Tp>::make_shared(_A0& __a0, _A1& __a1, _A2& __a2)
{
    typedef __shared_ptr_emplace<_Tp, allocator<_Tp> > _CntrlBlk;
    typedef allocator<_CntrlBlk> _Alloc2;
    typedef __allocator_destructor<_Alloc2> _D2;
    _Alloc2 __alloc2;
    unique_ptr<_CntrlBlk, _D2> __hold2(__alloc2.allocate(1), _D2(__alloc2, 1));
    ::new(__hold2.get()) _CntrlBlk(__alloc2, __a0, __a1, __a2);
    shared_ptr<_Tp> __r;
    __r.__ptr_ = __hold2.get()->get();
    __r.__cntrl_ = __hold2.release();
    __r.__enable_weak_this(__r.__ptr_, __r.__ptr_);
    return __r;
}

template<class _Tp>
template<class _Alloc>
shared_ptr<_Tp>
shared_ptr<_Tp>::allocate_shared(const _Alloc& __a)
{
    typedef __shared_ptr_emplace<_Tp, _Alloc> _CntrlBlk;
    typedef typename __allocator_traits_rebind<_Alloc, _CntrlBlk>::type _Alloc2;
    typedef __allocator_destructor<_Alloc2> _D2;
    _Alloc2 __alloc2(__a);
    unique_ptr<_CntrlBlk, _D2> __hold2(__alloc2.allocate(1), _D2(__alloc2, 1));
    ::new(static_cast<void*>(_VSTD::addressof(*__hold2.get())))
        _CntrlBlk(__a);
    shared_ptr<_Tp> __r;
    __r.__ptr_ = __hold2.get()->get();
    __r.__cntrl_ = _VSTD::addressof(*__hold2.release());
    __r.__enable_weak_this(__r.__ptr_, __r.__ptr_);
    return __r;
}

template<class _Tp>
template<class _Alloc, class _A0>
shared_ptr<_Tp>
shared_ptr<_Tp>::allocate_shared(const _Alloc& __a, _A0& __a0)
{
    typedef __shared_ptr_emplace<_Tp, _Alloc> _CntrlBlk;
    typedef typename __allocator_traits_rebind<_Alloc, _CntrlBlk>::type _Alloc2;
    typedef __allocator_destructor<_Alloc2> _D2;
    _Alloc2 __alloc2(__a);
    unique_ptr<_CntrlBlk, _D2> __hold2(__alloc2.allocate(1), _D2(__alloc2, 1));
    ::new(static_cast<void*>(_VSTD::addressof(*__hold2.get())))
        _CntrlBlk(__a, __a0);
    shared_ptr<_Tp> __r;
    __r.__ptr_ = __hold2.get()->get();
    __r.__cntrl_ = _VSTD::addressof(*__hold2.release());
    __r.__enable_weak_this(__r.__ptr_, __r.__ptr_);
    return __r;
}

template<class _Tp>
template<class _Alloc, class _A0, class _A1>
shared_ptr<_Tp>
shared_ptr<_Tp>::allocate_shared(const _Alloc& __a, _A0& __a0, _A1& __a1)
{
    typedef __shared_ptr_emplace<_Tp, _Alloc> _CntrlBlk;
    typedef typename __allocator_traits_rebind<_Alloc, _CntrlBlk>::type _Alloc2;
    typedef __allocator_destructor<_Alloc2> _D2;
    _Alloc2 __alloc2(__a);
    unique_ptr<_CntrlBlk, _D2> __hold2(__alloc2.allocate(1), _D2(__alloc2, 1));
    ::new(static_cast<void*>(_VSTD::addressof(*__hold2.get())))
        _CntrlBlk(__a, __a0, __a1);
    shared_ptr<_Tp> __r;
    __r.__ptr_ = __hold2.get()->get();
    __r.__cntrl_ = _VSTD::addressof(*__hold2.release());
    __r.__enable_weak_this(__r.__ptr_, __r.__ptr_);
    return __r;
}

template<class _Tp>
template<class _Alloc, class _A0, class _A1, class _A2>
shared_ptr<_Tp>
shared_ptr<_Tp>::allocate_shared(const _Alloc& __a, _A0& __a0, _A1& __a1, _A2& __a2)
{
    typedef __shared_ptr_emplace<_Tp, _Alloc> _CntrlBlk;
    typedef typename __allocator_traits_rebind<_Alloc, _CntrlBlk>::type _Alloc2;
    typedef __allocator_destructor<_Alloc2> _D2;
    _Alloc2 __alloc2(__a);
    unique_ptr<_CntrlBlk, _D2> __hold2(__alloc2.allocate(1), _D2(__alloc2, 1));
    ::new(static_cast<void*>(_VSTD::addressof(*__hold2.get())))
        _CntrlBlk(__a, __a0, __a1, __a2);
    shared_ptr<_Tp> __r;
    __r.__ptr_ = __hold2.get()->get();
    __r.__cntrl_ = _VSTD::addressof(*__hold2.release());
    __r.__enable_weak_this(__r.__ptr_, __r.__ptr_);
    return __r;
}

#endif  // _LIBCPP_HAS_NO_VARIADICS

template<class _Tp>
shared_ptr<_Tp>::~shared_ptr()
{
    if (__cntrl_)
        __cntrl_->__release_shared();
}

template<class _Tp>
inline
shared_ptr<_Tp>&
shared_ptr<_Tp>::operator=(const shared_ptr& __r) _NOEXCEPT
{
    shared_ptr(__r).swap(*this);
    return *this;
}

template<class _Tp>
template<class _Yp>
inline
typename enable_if
<
    is_convertible<_Yp*, typename shared_ptr<_Tp>::element_type*>::value,
    shared_ptr<_Tp>&
>::type
shared_ptr<_Tp>::operator=(const shared_ptr<_Yp>& __r) _NOEXCEPT
{
    shared_ptr(__r).swap(*this);
    return *this;
}

#ifndef _LIBCPP_HAS_NO_RVALUE_REFERENCES

template<class _Tp>
inline
shared_ptr<_Tp>&
shared_ptr<_Tp>::operator=(shared_ptr&& __r) _NOEXCEPT
{
    shared_ptr(_VSTD::move(__r)).swap(*this);
    return *this;
}

template<class _Tp>
template<class _Yp>
inline
typename enable_if
<
    is_convertible<_Yp*, typename shared_ptr<_Tp>::element_type*>::value,
    shared_ptr<_Tp>&
>::type
shared_ptr<_Tp>::operator=(shared_ptr<_Yp>&& __r)
{
    shared_ptr(_VSTD::move(__r)).swap(*this);
    return *this;
}

#if _LIBCPP_STD_VER <= 14 || defined(_LIBCPP_ENABLE_CXX17_REMOVED_AUTO_PTR)
template<class _Tp>
template<class _Yp>
inline
typename enable_if
<
    !is_array<_Yp>::value &&
    is_convertible<_Yp*, typename shared_ptr<_Tp>::element_type*>::value,
    shared_ptr<_Tp>
>::type&
shared_ptr<_Tp>::operator=(auto_ptr<_Yp>&& __r)
{
    shared_ptr(_VSTD::move(__r)).swap(*this);
    return *this;
}
#endif

template<class _Tp>
template <class _Yp, class _Dp>
inline
typename enable_if
<
    !is_array<_Yp>::value &&
    is_convertible<typename unique_ptr<_Yp, _Dp>::pointer,
                   typename shared_ptr<_Tp>::element_type*>::value,
    shared_ptr<_Tp>&
>::type
shared_ptr<_Tp>::operator=(unique_ptr<_Yp, _Dp>&& __r)
{
    shared_ptr(_VSTD::move(__r)).swap(*this);
    return *this;
}

#else  // _LIBCPP_HAS_NO_RVALUE_REFERENCES

#if _LIBCPP_STD_VER <= 14 || defined(_LIBCPP_ENABLE_CXX17_REMOVED_AUTO_PTR)
template<class _Tp>
template<class _Yp>
inline _LIBCPP_INLINE_VISIBILITY
typename enable_if
<
    !is_array<_Yp>::value &&
    is_convertible<_Yp*, typename shared_ptr<_Tp>::element_type*>::value,
    shared_ptr<_Tp>&
>::type
shared_ptr<_Tp>::operator=(auto_ptr<_Yp> __r)
{
    shared_ptr(__r).swap(*this);
    return *this;
}
#endif

template<class _Tp>
template <class _Yp, class _Dp>
inline _LIBCPP_INLINE_VISIBILITY
typename enable_if
<
    !is_array<_Yp>::value &&
    is_convertible<typename unique_ptr<_Yp, _Dp>::pointer,
                   typename shared_ptr<_Tp>::element_type*>::value,
    shared_ptr<_Tp>&
>::type
shared_ptr<_Tp>::operator=(unique_ptr<_Yp, _Dp> __r)
{
    shared_ptr(_VSTD::move(__r)).swap(*this);
    return *this;
}

#endif  // _LIBCPP_HAS_NO_RVALUE_REFERENCES

template<class _Tp>
inline
void
shared_ptr<_Tp>::swap(shared_ptr& __r) _NOEXCEPT
{
    _VSTD::swap(__ptr_, __r.__ptr_);
    _VSTD::swap(__cntrl_, __r.__cntrl_);
}

template<class _Tp>
inline
void
shared_ptr<_Tp>::reset() _NOEXCEPT
{
    shared_ptr().swap(*this);
}

template<class _Tp>
template<class _Yp>
inline
typename enable_if
<
    is_convertible<_Yp*, typename shared_ptr<_Tp>::element_type*>::value,
    void
>::type
shared_ptr<_Tp>::reset(_Yp* __p)
{
    shared_ptr(__p).swap(*this);
}

template<class _Tp>
template<class _Yp, class _Dp>
inline
typename enable_if
<
    is_convertible<_Yp*, typename shared_ptr<_Tp>::element_type*>::value,
    void
>::type
shared_ptr<_Tp>::reset(_Yp* __p, _Dp __d)
{
    shared_ptr(__p, __d).swap(*this);
}

template<class _Tp>
template<class _Yp, class _Dp, class _Alloc>
inline
typename enable_if
<
    is_convertible<_Yp*, typename shared_ptr<_Tp>::element_type*>::value,
    void
>::type
shared_ptr<_Tp>::reset(_Yp* __p, _Dp __d, _Alloc __a)
{
    shared_ptr(__p, __d, __a).swap(*this);
}

#ifndef _LIBCPP_HAS_NO_VARIADICS

template<class _Tp, class ..._Args>
inline _LIBCPP_INLINE_VISIBILITY
typename enable_if
<
    !is_array<_Tp>::value,
    shared_ptr<_Tp>
>::type
make_shared(_Args&& ...__args)
{
    return shared_ptr<_Tp>::make_shared(_VSTD::forward<_Args>(__args)...);
}

template<class _Tp, class _Alloc, class ..._Args>
inline _LIBCPP_INLINE_VISIBILITY
typename enable_if
<
    !is_array<_Tp>::value,
    shared_ptr<_Tp>
>::type
allocate_shared(const _Alloc& __a, _Args&& ...__args)
{
    return shared_ptr<_Tp>::allocate_shared(__a, _VSTD::forward<_Args>(__args)...);
}

#else  // _LIBCPP_HAS_NO_VARIADICS

template<class _Tp>
inline _LIBCPP_INLINE_VISIBILITY
shared_ptr<_Tp>
make_shared()
{
    return shared_ptr<_Tp>::make_shared();
}

template<class _Tp, class _A0>
inline _LIBCPP_INLINE_VISIBILITY
shared_ptr<_Tp>
make_shared(_A0& __a0)
{
    return shared_ptr<_Tp>::make_shared(__a0);
}

template<class _Tp, class _A0, class _A1>
inline _LIBCPP_INLINE_VISIBILITY
shared_ptr<_Tp>
make_shared(_A0& __a0, _A1& __a1)
{
    return shared_ptr<_Tp>::make_shared(__a0, __a1);
}

template<class _Tp, class _A0, class _A1, class _A2>
inline _LIBCPP_INLINE_VISIBILITY
shared_ptr<_Tp>
make_shared(_A0& __a0, _A1& __a1, _A2& __a2)
{
    return shared_ptr<_Tp>::make_shared(__a0, __a1, __a2);
}

template<class _Tp, class _Alloc>
inline _LIBCPP_INLINE_VISIBILITY
shared_ptr<_Tp>
allocate_shared(const _Alloc& __a)
{
    return shared_ptr<_Tp>::allocate_shared(__a);
}

template<class _Tp, class _Alloc, class _A0>
inline _LIBCPP_INLINE_VISIBILITY
shared_ptr<_Tp>
allocate_shared(const _Alloc& __a, _A0& __a0)
{
    return shared_ptr<_Tp>::allocate_shared(__a, __a0);
}

template<class _Tp, class _Alloc, class _A0, class _A1>
inline _LIBCPP_INLINE_VISIBILITY
shared_ptr<_Tp>
allocate_shared(const _Alloc& __a, _A0& __a0, _A1& __a1)
{
    return shared_ptr<_Tp>::allocate_shared(__a, __a0, __a1);
}

template<class _Tp, class _Alloc, class _A0, class _A1, class _A2>
inline _LIBCPP_INLINE_VISIBILITY
shared_ptr<_Tp>
allocate_shared(const _Alloc& __a, _A0& __a0, _A1& __a1, _A2& __a2)
{
    return shared_ptr<_Tp>::allocate_shared(__a, __a0, __a1, __a2);
}

#endif  // _LIBCPP_HAS_NO_VARIADICS

template<class _Tp, class _Up>
inline _LIBCPP_INLINE_VISIBILITY
bool
operator==(const shared_ptr<_Tp>& __x, const shared_ptr<_Up>& __y) _NOEXCEPT
{
    return __x.get() == __y.get();
}

template<class _Tp, class _Up>
inline _LIBCPP_INLINE_VISIBILITY
bool
operator!=(const shared_ptr<_Tp>& __x, const shared_ptr<_Up>& __y) _NOEXCEPT
{
    return !(__x == __y);
}

template<class _Tp, class _Up>
inline _LIBCPP_INLINE_VISIBILITY
bool
operator<(const shared_ptr<_Tp>& __x, const shared_ptr<_Up>& __y) _NOEXCEPT
{
    typedef typename common_type<_Tp*, _Up*>::type _Vp;
    return less<_Vp>()(__x.get(), __y.get());
}

template<class _Tp, class _Up>
inline _LIBCPP_INLINE_VISIBILITY
bool
operator>(const shared_ptr<_Tp>& __x, const shared_ptr<_Up>& __y) _NOEXCEPT
{
    return __y < __x;
}

template<class _Tp, class _Up>
inline _LIBCPP_INLINE_VISIBILITY
bool
operator<=(const shared_ptr<_Tp>& __x, const shared_ptr<_Up>& __y) _NOEXCEPT
{
    return !(__y < __x);
}

template<class _Tp, class _Up>
inline _LIBCPP_INLINE_VISIBILITY
bool
operator>=(const shared_ptr<_Tp>& __x, const shared_ptr<_Up>& __y) _NOEXCEPT
{
    return !(__x < __y);
}

template<class _Tp>
inline _LIBCPP_INLINE_VISIBILITY
bool
operator==(const shared_ptr<_Tp>& __x, nullptr_t) _NOEXCEPT
{
    return !__x;
}

template<class _Tp>
inline _LIBCPP_INLINE_VISIBILITY
bool
operator==(nullptr_t, const shared_ptr<_Tp>& __x) _NOEXCEPT
{
    return !__x;
}

template<class _Tp>
inline _LIBCPP_INLINE_VISIBILITY
bool
operator!=(const shared_ptr<_Tp>& __x, nullptr_t) _NOEXCEPT
{
    return static_cast<bool>(__x);
}

template<class _Tp>
inline _LIBCPP_INLINE_VISIBILITY
bool
operator!=(nullptr_t, const shared_ptr<_Tp>& __x) _NOEXCEPT
{
    return static_cast<bool>(__x);
}

template<class _Tp>
inline _LIBCPP_INLINE_VISIBILITY
bool
operator<(const shared_ptr<_Tp>& __x, nullptr_t) _NOEXCEPT
{
    return less<_Tp*>()(__x.get(), nullptr);
}

template<class _Tp>
inline _LIBCPP_INLINE_VISIBILITY
bool
operator<(nullptr_t, const shared_ptr<_Tp>& __x) _NOEXCEPT
{
    return less<_Tp*>()(nullptr, __x.get());
}

template<class _Tp>
inline _LIBCPP_INLINE_VISIBILITY
bool
operator>(const shared_ptr<_Tp>& __x, nullptr_t) _NOEXCEPT
{
    return nullptr < __x;
}

template<class _Tp>
inline _LIBCPP_INLINE_VISIBILITY
bool
operator>(nullptr_t, const shared_ptr<_Tp>& __x) _NOEXCEPT
{
    return __x < nullptr;
}

template<class _Tp>
inline _LIBCPP_INLINE_VISIBILITY
bool
operator<=(const shared_ptr<_Tp>& __x, nullptr_t) _NOEXCEPT
{
    return !(nullptr < __x);
}

template<class _Tp>
inline _LIBCPP_INLINE_VISIBILITY
bool
operator<=(nullptr_t, const shared_ptr<_Tp>& __x) _NOEXCEPT
{
    return !(__x < nullptr);
}

template<class _Tp>
inline _LIBCPP_INLINE_VISIBILITY
bool
operator>=(const shared_ptr<_Tp>& __x, nullptr_t) _NOEXCEPT
{
    return !(__x < nullptr);
}

template<class _Tp>
inline _LIBCPP_INLINE_VISIBILITY
bool
operator>=(nullptr_t, const shared_ptr<_Tp>& __x) _NOEXCEPT
{
    return !(nullptr < __x);
}

template<class _Tp>
inline _LIBCPP_INLINE_VISIBILITY
void
swap(shared_ptr<_Tp>& __x, shared_ptr<_Tp>& __y) _NOEXCEPT
{
    __x.swap(__y);
}

template<class _Tp, class _Up>
inline _LIBCPP_INLINE_VISIBILITY
typename enable_if
<
    !is_array<_Tp>::value && !is_array<_Up>::value,
    shared_ptr<_Tp>
>::type
static_pointer_cast(const shared_ptr<_Up>& __r) _NOEXCEPT
{
    return shared_ptr<_Tp>(__r, static_cast<_Tp*>(__r.get()));
}

template<class _Tp, class _Up>
inline _LIBCPP_INLINE_VISIBILITY
typename enable_if
<
    !is_array<_Tp>::value && !is_array<_Up>::value,
    shared_ptr<_Tp>
>::type
dynamic_pointer_cast(const shared_ptr<_Up>& __r) _NOEXCEPT
{
    _Tp* __p = dynamic_cast<_Tp*>(__r.get());
    return __p ? shared_ptr<_Tp>(__r, __p) : shared_ptr<_Tp>();
}

template<class _Tp, class _Up>
typename enable_if
<
    is_array<_Tp>::value == is_array<_Up>::value,
    shared_ptr<_Tp>
>::type
const_pointer_cast(const shared_ptr<_Up>& __r) _NOEXCEPT
{
    typedef typename remove_extent<_Tp>::type _RTp;
    return shared_ptr<_Tp>(__r, const_cast<_RTp*>(__r.get()));
}

#ifndef _LIBCPP_NO_RTTI

template<class _Dp, class _Tp>
inline _LIBCPP_INLINE_VISIBILITY
_Dp*
get_deleter(const shared_ptr<_Tp>& __p) _NOEXCEPT
{
    return __p.template __get_deleter<_Dp>();
}

#endif  // _LIBCPP_NO_RTTI

template<class _Tp>
class _LIBCPP_TEMPLATE_VIS weak_ptr
{
public:
    typedef _Tp element_type;
private:
    element_type*        __ptr_;
    __shared_weak_count* __cntrl_;

public:
    _LIBCPP_INLINE_VISIBILITY
    _LIBCPP_CONSTEXPR weak_ptr() _NOEXCEPT;
    template<class _Yp> _LIBCPP_INLINE_VISIBILITY weak_ptr(shared_ptr<_Yp> const& __r,
                   typename enable_if<is_convertible<_Yp*, _Tp*>::value, __nat*>::type = 0)
                        _NOEXCEPT;
    _LIBCPP_INLINE_VISIBILITY
    weak_ptr(weak_ptr const& __r) _NOEXCEPT;
    template<class _Yp> _LIBCPP_INLINE_VISIBILITY weak_ptr(weak_ptr<_Yp> const& __r,
                   typename enable_if<is_convertible<_Yp*, _Tp*>::value, __nat*>::type = 0)
                         _NOEXCEPT;

#ifndef _LIBCPP_HAS_NO_RVALUE_REFERENCES
    _LIBCPP_INLINE_VISIBILITY
    weak_ptr(weak_ptr&& __r) _NOEXCEPT;
    template<class _Yp> _LIBCPP_INLINE_VISIBILITY weak_ptr(weak_ptr<_Yp>&& __r,
                   typename enable_if<is_convertible<_Yp*, _Tp*>::value, __nat*>::type = 0)
                         _NOEXCEPT;
#endif  // _LIBCPP_HAS_NO_RVALUE_REFERENCES
    ~weak_ptr();

    _LIBCPP_INLINE_VISIBILITY
    weak_ptr& operator=(weak_ptr const& __r) _NOEXCEPT;
    template<class _Yp>
        typename enable_if
        <
            is_convertible<_Yp*, element_type*>::value,
            weak_ptr&
        >::type
        _LIBCPP_INLINE_VISIBILITY
        operator=(weak_ptr<_Yp> const& __r) _NOEXCEPT;

#ifndef _LIBCPP_HAS_NO_RVALUE_REFERENCES

    _LIBCPP_INLINE_VISIBILITY
    weak_ptr& operator=(weak_ptr&& __r) _NOEXCEPT;
    template<class _Yp>
        typename enable_if
        <
            is_convertible<_Yp*, element_type*>::value,
            weak_ptr&
        >::type
        _LIBCPP_INLINE_VISIBILITY
        operator=(weak_ptr<_Yp>&& __r) _NOEXCEPT;

#endif  // _LIBCPP_HAS_NO_RVALUE_REFERENCES

    template<class _Yp>
        typename enable_if
        <
            is_convertible<_Yp*, element_type*>::value,
            weak_ptr&
        >::type
        _LIBCPP_INLINE_VISIBILITY
        operator=(shared_ptr<_Yp> const& __r) _NOEXCEPT;

    _LIBCPP_INLINE_VISIBILITY
    void swap(weak_ptr& __r) _NOEXCEPT;
    _LIBCPP_INLINE_VISIBILITY
    void reset() _NOEXCEPT;

    _LIBCPP_INLINE_VISIBILITY
    long use_count() const _NOEXCEPT
        {return __cntrl_ ? __cntrl_->use_count() : 0;}
    _LIBCPP_INLINE_VISIBILITY
    bool expired() const _NOEXCEPT
        {return __cntrl_ == 0 || __cntrl_->use_count() == 0;}
    shared_ptr<_Tp> lock() const _NOEXCEPT;
    template<class _Up>
        _LIBCPP_INLINE_VISIBILITY
        bool owner_before(const shared_ptr<_Up>& __r) const _NOEXCEPT
        {return __cntrl_ < __r.__cntrl_;}
    template<class _Up>
        _LIBCPP_INLINE_VISIBILITY
        bool owner_before(const weak_ptr<_Up>& __r) const _NOEXCEPT
        {return __cntrl_ < __r.__cntrl_;}

    template <class _Up> friend class _LIBCPP_TEMPLATE_VIS weak_ptr;
    template <class _Up> friend class _LIBCPP_TEMPLATE_VIS shared_ptr;
};

template<class _Tp>
inline
_LIBCPP_CONSTEXPR
weak_ptr<_Tp>::weak_ptr() _NOEXCEPT
    : __ptr_(0),
      __cntrl_(0)
{
}

template<class _Tp>
inline
weak_ptr<_Tp>::weak_ptr(weak_ptr const& __r) _NOEXCEPT
    : __ptr_(__r.__ptr_),
      __cntrl_(__r.__cntrl_)
{
    if (__cntrl_)
        __cntrl_->__add_weak();
}

template<class _Tp>
template<class _Yp>
inline
weak_ptr<_Tp>::weak_ptr(shared_ptr<_Yp> const& __r,
                        typename enable_if<is_convertible<_Yp*, _Tp*>::value, __nat*>::type)
                         _NOEXCEPT
    : __ptr_(__r.__ptr_),
      __cntrl_(__r.__cntrl_)
{
    if (__cntrl_)
        __cntrl_->__add_weak();
}

template<class _Tp>
template<class _Yp>
inline
weak_ptr<_Tp>::weak_ptr(weak_ptr<_Yp> const& __r,
                        typename enable_if<is_convertible<_Yp*, _Tp*>::value, __nat*>::type)
         _NOEXCEPT
    : __ptr_(__r.__ptr_),
      __cntrl_(__r.__cntrl_)
{
    if (__cntrl_)
        __cntrl_->__add_weak();
}

#ifndef _LIBCPP_HAS_NO_RVALUE_REFERENCES

template<class _Tp>
inline
weak_ptr<_Tp>::weak_ptr(weak_ptr&& __r) _NOEXCEPT
    : __ptr_(__r.__ptr_),
      __cntrl_(__r.__cntrl_)
{
    __r.__ptr_ = 0;
    __r.__cntrl_ = 0;
}

template<class _Tp>
template<class _Yp>
inline
weak_ptr<_Tp>::weak_ptr(weak_ptr<_Yp>&& __r,
                        typename enable_if<is_convertible<_Yp*, _Tp*>::value, __nat*>::type)
         _NOEXCEPT
    : __ptr_(__r.__ptr_),
      __cntrl_(__r.__cntrl_)
{
    __r.__ptr_ = 0;
    __r.__cntrl_ = 0;
}

#endif  // _LIBCPP_HAS_NO_RVALUE_REFERENCES

template<class _Tp>
weak_ptr<_Tp>::~weak_ptr()
{
    if (__cntrl_)
        __cntrl_->__release_weak();
}

template<class _Tp>
inline
weak_ptr<_Tp>&
weak_ptr<_Tp>::operator=(weak_ptr const& __r) _NOEXCEPT
{
    weak_ptr(__r).swap(*this);
    return *this;
}

template<class _Tp>
template<class _Yp>
inline
typename enable_if
<
    is_convertible<_Yp*, _Tp*>::value,
    weak_ptr<_Tp>&
>::type
weak_ptr<_Tp>::operator=(weak_ptr<_Yp> const& __r) _NOEXCEPT
{
    weak_ptr(__r).swap(*this);
    return *this;
}

#ifndef _LIBCPP_HAS_NO_RVALUE_REFERENCES

template<class _Tp>
inline
weak_ptr<_Tp>&
weak_ptr<_Tp>::operator=(weak_ptr&& __r) _NOEXCEPT
{
    weak_ptr(_VSTD::move(__r)).swap(*this);
    return *this;
}

template<class _Tp>
template<class _Yp>
inline
typename enable_if
<
    is_convertible<_Yp*, _Tp*>::value,
    weak_ptr<_Tp>&
>::type
weak_ptr<_Tp>::operator=(weak_ptr<_Yp>&& __r) _NOEXCEPT
{
    weak_ptr(_VSTD::move(__r)).swap(*this);
    return *this;
}

#endif  // _LIBCPP_HAS_NO_RVALUE_REFERENCES

template<class _Tp>
template<class _Yp>
inline
typename enable_if
<
    is_convertible<_Yp*, _Tp*>::value,
    weak_ptr<_Tp>&
>::type
weak_ptr<_Tp>::operator=(shared_ptr<_Yp> const& __r) _NOEXCEPT
{
    weak_ptr(__r).swap(*this);
    return *this;
}

template<class _Tp>
inline
void
weak_ptr<_Tp>::swap(weak_ptr& __r) _NOEXCEPT
{
    _VSTD::swap(__ptr_, __r.__ptr_);
    _VSTD::swap(__cntrl_, __r.__cntrl_);
}

template<class _Tp>
inline _LIBCPP_INLINE_VISIBILITY
void
swap(weak_ptr<_Tp>& __x, weak_ptr<_Tp>& __y) _NOEXCEPT
{
    __x.swap(__y);
}

template<class _Tp>
inline
void
weak_ptr<_Tp>::reset() _NOEXCEPT
{
    weak_ptr().swap(*this);
}

template<class _Tp>
template<class _Yp>
shared_ptr<_Tp>::shared_ptr(const weak_ptr<_Yp>& __r,
                            typename enable_if<is_convertible<_Yp*, element_type*>::value, __nat>::type)
    : __ptr_(__r.__ptr_),
      __cntrl_(__r.__cntrl_ ? __r.__cntrl_->lock() : __r.__cntrl_)
{
    if (__cntrl_ == 0)
        __throw_bad_weak_ptr();
}

template<class _Tp>
shared_ptr<_Tp>
weak_ptr<_Tp>::lock() const _NOEXCEPT
{
    shared_ptr<_Tp> __r;
    __r.__cntrl_ = __cntrl_ ? __cntrl_->lock() : __cntrl_;
    if (__r.__cntrl_)
        __r.__ptr_ = __ptr_;
    return __r;
}

#if _LIBCPP_STD_VER > 14
template <class _Tp = void> struct owner_less;
#else
template <class _Tp> struct owner_less;
#endif

template <class _Tp>
struct _LIBCPP_TEMPLATE_VIS owner_less<shared_ptr<_Tp> >
    : binary_function<shared_ptr<_Tp>, shared_ptr<_Tp>, bool>
{
    typedef bool result_type;
    _LIBCPP_INLINE_VISIBILITY
    bool operator()(shared_ptr<_Tp> const& __x, shared_ptr<_Tp> const& __y) const _NOEXCEPT
        {return __x.owner_before(__y);}
    _LIBCPP_INLINE_VISIBILITY
    bool operator()(shared_ptr<_Tp> const& __x,   weak_ptr<_Tp> const& __y) const _NOEXCEPT
        {return __x.owner_before(__y);}
    _LIBCPP_INLINE_VISIBILITY
    bool operator()(  weak_ptr<_Tp> const& __x, shared_ptr<_Tp> const& __y) const _NOEXCEPT
        {return __x.owner_before(__y);}
};

template <class _Tp>
struct _LIBCPP_TEMPLATE_VIS owner_less<weak_ptr<_Tp> >
    : binary_function<weak_ptr<_Tp>, weak_ptr<_Tp>, bool>
{
    typedef bool result_type;
    _LIBCPP_INLINE_VISIBILITY
    bool operator()(  weak_ptr<_Tp> const& __x,   weak_ptr<_Tp> const& __y) const _NOEXCEPT
        {return __x.owner_before(__y);}
    _LIBCPP_INLINE_VISIBILITY
    bool operator()(shared_ptr<_Tp> const& __x,   weak_ptr<_Tp> const& __y) const _NOEXCEPT
        {return __x.owner_before(__y);}
    _LIBCPP_INLINE_VISIBILITY
    bool operator()(  weak_ptr<_Tp> const& __x, shared_ptr<_Tp> const& __y) const _NOEXCEPT
        {return __x.owner_before(__y);}
};

#if _LIBCPP_STD_VER > 14
template <>
struct _LIBCPP_TEMPLATE_VIS owner_less<void>
{
    template <class _Tp, class _Up>
    _LIBCPP_INLINE_VISIBILITY
    bool operator()( shared_ptr<_Tp> const& __x, shared_ptr<_Up> const& __y) const _NOEXCEPT
        {return __x.owner_before(__y);}
    template <class _Tp, class _Up>
    _LIBCPP_INLINE_VISIBILITY
    bool operator()( shared_ptr<_Tp> const& __x,   weak_ptr<_Up> const& __y) const _NOEXCEPT
        {return __x.owner_before(__y);}
    template <class _Tp, class _Up>
    _LIBCPP_INLINE_VISIBILITY
    bool operator()(   weak_ptr<_Tp> const& __x, shared_ptr<_Up> const& __y) const _NOEXCEPT
        {return __x.owner_before(__y);}
    template <class _Tp, class _Up>
    _LIBCPP_INLINE_VISIBILITY
    bool operator()(   weak_ptr<_Tp> const& __x,   weak_ptr<_Up> const& __y) const _NOEXCEPT
        {return __x.owner_before(__y);}
    typedef void is_transparent;
};
#endif

template<class _Tp>
class _LIBCPP_TEMPLATE_VIS enable_shared_from_this
{
    mutable weak_ptr<_Tp> __weak_this_;
protected:
    _LIBCPP_INLINE_VISIBILITY _LIBCPP_CONSTEXPR
    enable_shared_from_this() _NOEXCEPT {}
    _LIBCPP_INLINE_VISIBILITY
    enable_shared_from_this(enable_shared_from_this const&) _NOEXCEPT {}
    _LIBCPP_INLINE_VISIBILITY
    enable_shared_from_this& operator=(enable_shared_from_this const&) _NOEXCEPT
        {return *this;}
    _LIBCPP_INLINE_VISIBILITY
    ~enable_shared_from_this() {}
public:
    _LIBCPP_INLINE_VISIBILITY
    shared_ptr<_Tp> shared_from_this()
        {return shared_ptr<_Tp>(__weak_this_);}
    _LIBCPP_INLINE_VISIBILITY
    shared_ptr<_Tp const> shared_from_this() const
        {return shared_ptr<const _Tp>(__weak_this_);}

#if _LIBCPP_STD_VER > 14
    _LIBCPP_INLINE_VISIBILITY
    weak_ptr<_Tp> weak_from_this() _NOEXCEPT
       { return __weak_this_; }

    _LIBCPP_INLINE_VISIBILITY
    weak_ptr<const _Tp> weak_from_this() const _NOEXCEPT
        { return __weak_this_; }
#endif // _LIBCPP_STD_VER > 14

    template <class _Up> friend class shared_ptr;
};

template <class _Tp>
struct _LIBCPP_TEMPLATE_VIS hash<shared_ptr<_Tp> >
{
    typedef shared_ptr<_Tp>      argument_type;
    typedef size_t               result_type;

    _LIBCPP_INLINE_VISIBILITY
    result_type operator()(const argument_type& __ptr) const _NOEXCEPT
    {
        return hash<_Tp*>()(__ptr.get());
    }
};

template<class _CharT, class _Traits, class _Yp>
inline _LIBCPP_INLINE_VISIBILITY
basic_ostream<_CharT, _Traits>&
operator<<(basic_ostream<_CharT, _Traits>& __os, shared_ptr<_Yp> const& __p);


#if !defined(_LIBCPP_HAS_NO_ATOMIC_HEADER)

class _LIBCPP_TYPE_VIS __sp_mut
{
    void* __lx;
public:
    void lock() _NOEXCEPT;
    void unlock() _NOEXCEPT;

private:
    _LIBCPP_CONSTEXPR __sp_mut(void*) _NOEXCEPT;
    __sp_mut(const __sp_mut&);
    __sp_mut& operator=(const __sp_mut&);

    friend _LIBCPP_FUNC_VIS __sp_mut& __get_sp_mut(const void*);
};

_LIBCPP_FUNC_VIS _LIBCPP_AVAILABILITY_ATOMIC_SHARED_PTR
__sp_mut& __get_sp_mut(const void*);

template <class _Tp>
inline _LIBCPP_INLINE_VISIBILITY
bool
atomic_is_lock_free(const shared_ptr<_Tp>*)
{
    return false;
}

template <class _Tp>
_LIBCPP_AVAILABILITY_ATOMIC_SHARED_PTR
shared_ptr<_Tp>
atomic_load(const shared_ptr<_Tp>* __p)
{
    __sp_mut& __m = __get_sp_mut(__p);
    __m.lock();
    shared_ptr<_Tp> __q = *__p;
    __m.unlock();
    return __q;
}

template <class _Tp>
inline _LIBCPP_INLINE_VISIBILITY
_LIBCPP_AVAILABILITY_ATOMIC_SHARED_PTR
shared_ptr<_Tp>
atomic_load_explicit(const shared_ptr<_Tp>* __p, memory_order)
{
    return atomic_load(__p);
}

template <class _Tp>
_LIBCPP_AVAILABILITY_ATOMIC_SHARED_PTR
void
atomic_store(shared_ptr<_Tp>* __p, shared_ptr<_Tp> __r)
{
    __sp_mut& __m = __get_sp_mut(__p);
    __m.lock();
    __p->swap(__r);
    __m.unlock();
}

template <class _Tp>
inline _LIBCPP_INLINE_VISIBILITY
_LIBCPP_AVAILABILITY_ATOMIC_SHARED_PTR
void
atomic_store_explicit(shared_ptr<_Tp>* __p, shared_ptr<_Tp> __r, memory_order)
{
    atomic_store(__p, __r);
}

template <class _Tp>
_LIBCPP_AVAILABILITY_ATOMIC_SHARED_PTR
shared_ptr<_Tp>
atomic_exchange(shared_ptr<_Tp>* __p, shared_ptr<_Tp> __r)
{
    __sp_mut& __m = __get_sp_mut(__p);
    __m.lock();
    __p->swap(__r);
    __m.unlock();
    return __r;
}

template <class _Tp>
inline _LIBCPP_INLINE_VISIBILITY
_LIBCPP_AVAILABILITY_ATOMIC_SHARED_PTR
shared_ptr<_Tp>
atomic_exchange_explicit(shared_ptr<_Tp>* __p, shared_ptr<_Tp> __r, memory_order)
{
    return atomic_exchange(__p, __r);
}

template <class _Tp>
_LIBCPP_AVAILABILITY_ATOMIC_SHARED_PTR
bool
atomic_compare_exchange_strong(shared_ptr<_Tp>* __p, shared_ptr<_Tp>* __v, shared_ptr<_Tp> __w)
{
    shared_ptr<_Tp> __temp;
    __sp_mut& __m = __get_sp_mut(__p);
    __m.lock();
    if (__p->__owner_equivalent(*__v))
    {
        _VSTD::swap(__temp, *__p);
        *__p = __w;
        __m.unlock();
        return true;
    }
    _VSTD::swap(__temp, *__v);
    *__v = *__p;
    __m.unlock();
    return false;
}

template <class _Tp>
inline _LIBCPP_INLINE_VISIBILITY
_LIBCPP_AVAILABILITY_ATOMIC_SHARED_PTR
bool
atomic_compare_exchange_weak(shared_ptr<_Tp>* __p, shared_ptr<_Tp>* __v, shared_ptr<_Tp> __w)
{
    return atomic_compare_exchange_strong(__p, __v, __w);
}

template <class _Tp>
inline _LIBCPP_INLINE_VISIBILITY
_LIBCPP_AVAILABILITY_ATOMIC_SHARED_PTR
bool
atomic_compare_exchange_strong_explicit(shared_ptr<_Tp>* __p, shared_ptr<_Tp>* __v,
                                        shared_ptr<_Tp> __w, memory_order, memory_order)
{
    return atomic_compare_exchange_strong(__p, __v, __w);
}

template <class _Tp>
inline _LIBCPP_INLINE_VISIBILITY
_LIBCPP_AVAILABILITY_ATOMIC_SHARED_PTR
bool
atomic_compare_exchange_weak_explicit(shared_ptr<_Tp>* __p, shared_ptr<_Tp>* __v,
                                      shared_ptr<_Tp> __w, memory_order, memory_order)
{
    return atomic_compare_exchange_weak(__p, __v, __w);
}

#endif  // !defined(_LIBCPP_HAS_NO_ATOMIC_HEADER)

//enum class
#if defined(_LIBCPP_ABI_POINTER_SAFETY_ENUM_TYPE)
# ifndef _LIBCPP_CXX03_LANG
enum class pointer_safety : unsigned char {
  relaxed,
  preferred,
  strict
};
# endif
#else
struct _LIBCPP_TYPE_VIS pointer_safety
{
    enum __lx
    {
        relaxed,
        preferred,
        strict
    };

    __lx __v_;

    _LIBCPP_INLINE_VISIBILITY
    pointer_safety() : __v_() {}

    _LIBCPP_INLINE_VISIBILITY
    pointer_safety(__lx __v) : __v_(__v) {}
    _LIBCPP_INLINE_VISIBILITY
    operator int() const {return __v_;}
};
#endif

#if !defined(_LIBCPP_ABI_POINTER_SAFETY_ENUM_TYPE) && \
    defined(_LIBCPP_BUILDING_MEMORY)
_LIBCPP_FUNC_VIS pointer_safety get_pointer_safety() _NOEXCEPT;
#else
// This function is only offered in C++03 under ABI v1.
# if !defined(_LIBCPP_ABI_POINTER_SAFETY_ENUM_TYPE) || !defined(_LIBCPP_CXX03_LANG)
inline _LIBCPP_INLINE_VISIBILITY
pointer_safety get_pointer_safety() _NOEXCEPT {
  return pointer_safety::relaxed;
}
# endif
#endif


_LIBCPP_FUNC_VIS void declare_reachable(void* __p);
_LIBCPP_FUNC_VIS void declare_no_pointers(char* __p, size_t __n);
_LIBCPP_FUNC_VIS void undeclare_no_pointers(char* __p, size_t __n);
_LIBCPP_FUNC_VIS void* __undeclare_reachable(void* __p);

template <class _Tp>
inline _LIBCPP_INLINE_VISIBILITY
_Tp*
undeclare_reachable(_Tp* __p)
{
    return static_cast<_Tp*>(__undeclare_reachable(__p));
}

_LIBCPP_FUNC_VIS void* align(size_t __align, size_t __sz, void*& __ptr, size_t& __space);

// --- Helper for container swap --
template <typename _Alloc>
inline _LIBCPP_INLINE_VISIBILITY
void __swap_allocator(_Alloc & __a1, _Alloc & __a2)
#if _LIBCPP_STD_VER >= 14
    _NOEXCEPT
#else
    _NOEXCEPT_(__is_nothrow_swappable<_Alloc>::value)
#endif
{
    __swap_allocator(__a1, __a2,
      integral_constant<bool, _VSTD::allocator_traits<_Alloc>::propagate_on_container_swap::value>());
}

template <typename _Alloc>
_LIBCPP_INLINE_VISIBILITY
void __swap_allocator(_Alloc & __a1, _Alloc & __a2, true_type)
#if _LIBCPP_STD_VER >= 14
    _NOEXCEPT
#else
    _NOEXCEPT_(__is_nothrow_swappable<_Alloc>::value)
#endif
{
    using _VSTD::swap;
    swap(__a1, __a2);
}

template <typename _Alloc>
inline _LIBCPP_INLINE_VISIBILITY
void __swap_allocator(_Alloc &, _Alloc &, false_type) _NOEXCEPT {}

template <typename _Alloc, typename _Traits=allocator_traits<_Alloc> >
struct __noexcept_move_assign_container : public integral_constant<bool,
    _Traits::propagate_on_container_move_assignment::value
#if _LIBCPP_STD_VER > 14
        || _Traits::is_always_equal::value
#else
        && is_nothrow_move_assignable<_Alloc>::value
#endif
    > {};


#ifndef _LIBCPP_HAS_NO_VARIADICS
template <class _Tp, class _Alloc>
struct __temp_value {
    typedef allocator_traits<_Alloc> _Traits;

    typename aligned_storage<sizeof(_Tp), alignof(_Tp)>::type __v;
    _Alloc &__a;

    _Tp *__addr() { return reinterpret_cast<_Tp *>(addressof(__v)); }
    _Tp &   get() { return *__addr(); }

    template<class... _Args>
    __temp_value(_Alloc &__alloc, _Args&& ... __args) : __a(__alloc)
    { _Traits::construct(__a, __addr(), _VSTD::forward<_Args>(__args)...); }

    ~__temp_value() { _Traits::destroy(__a, __addr()); }
    };
#endif

_LIBCPP_END_NAMESPACE_STD

_LIBCPP_POP_MACROS

#endif  // _LIBCPP_MEMORY
