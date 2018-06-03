// -*- C++ -*-
//===------------------------------ any -----------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef _LIBCPP_ANY
#define _LIBCPP_ANY

#include <experimental/__config.hpp>
#include <memory.hpp>
#include <new.hpp>
#include <typeinfo.hpp>
#include <type_traits.hpp>
#include <stdlib.h>

#if !defined(_LIBCPP_HAS_NO_PRAGMA_SYSTEM_HEADER)
#pragma GCC system_header
#endif

_LIBCPP_BEGIN_NAMESPACE_STD

#if _LIBCPP_STD_VER > 14

_LIBCPP_NORETURN inline _LIBCPP_ALWAYS_INLINE
void __throw_bad_any_cast()
{
#ifndef _LIBCPP_NO_EXCEPTIONS
    throw bad_any_cast();
#else
        abort();
#endif
}

// Forward declarations
class _LIBCPP_TEMPLATE_VIS any;

template <class _ValueType>
_LIBCPP_INLINE_VISIBILITY
add_pointer_t<add_const_t<_ValueType>>
any_cast(any const *) _NOEXCEPT;

template <class _ValueType>
_LIBCPP_INLINE_VISIBILITY
add_pointer_t<_ValueType> any_cast(any *) _NOEXCEPT;

namespace __any_imp
{
  using _Buffer = aligned_storage_t<3*sizeof(void*), alignment_of<void*>::value>;

  template <class _Tp>
  using _IsSmallObject = integral_constant<bool
        , sizeof(_Tp) <= sizeof(_Buffer)
          && alignment_of<_Buffer>::value
             % alignment_of<_Tp>::value == 0
          && is_nothrow_move_constructible<_Tp>::value
        >;

  enum class _Action {
    _Destroy,
    _Copy,
    _Move,
    _Get,
    _TypeInfo
  };

  template <class _Tp> struct _SmallHandler;
  template <class _Tp> struct _LargeHandler;

  template <class _Tp>
  struct  _LIBCPP_TEMPLATE_VIS __unique_typeinfo { static constexpr int __id = 0; };
  template <class _Tp> constexpr int __unique_typeinfo<_Tp>::__id;

  template <class _Tp>
  inline _LIBCPP_INLINE_VISIBILITY
  constexpr const void* __get_fallback_typeid() {
      return &__unique_typeinfo<decay_t<_Tp>>::__id;
  }

  template <class _Tp>
  inline _LIBCPP_INLINE_VISIBILITY
  bool __compare_typeid(type_info const* __id, const void* __fallback_id)
  {
#if !defined(_LIBCPP_NO_RTTI)
      if (__id && *__id == typeid(_Tp))
          return true;
#endif
      if (!__id && __fallback_id == __any_imp::__get_fallback_typeid<_Tp>())
          return true;
      return false;
  }

  template <class _Tp>
  using _Handler = conditional_t<
    _IsSmallObject<_Tp>::value, _SmallHandler<_Tp>, _LargeHandler<_Tp>>;

} // namespace __any_imp

class _LIBCPP_TEMPLATE_VIS any
{
public:
  // construct/destruct
  _LIBCPP_INLINE_VISIBILITY
  constexpr any() _NOEXCEPT : __h(nullptr) {}

  _LIBCPP_INLINE_VISIBILITY
  any(any const & __other) : __h(nullptr)
  {
    if (__other.__h) __other.__call(_Action::_Copy, this);
  }

  _LIBCPP_INLINE_VISIBILITY
  any(any && __other) _NOEXCEPT : __h(nullptr)
  {
    if (__other.__h) __other.__call(_Action::_Move, this);
  }

  template <
      class _ValueType
    , class _Tp = decay_t<_ValueType>
    , class = enable_if_t<
        !is_same<_Tp, any>::value &&
        !__is_inplace_type<_ValueType>::value &&
        is_copy_constructible<_Tp>::value>
    >
  _LIBCPP_INLINE_VISIBILITY
  any(_ValueType && __value);

  template <class _ValueType, class ..._Args,
    class _Tp = decay_t<_ValueType>,
    class = enable_if_t<
        is_constructible<_Tp, _Args...>::value &&
        is_copy_constructible<_Tp>::value
    >
  >
  _LIBCPP_INLINE_VISIBILITY
  explicit any(in_place_type_t<_ValueType>, _Args&&... __args);

  template <class _ValueType, class _Up, class ..._Args,
    class _Tp = decay_t<_ValueType>,
    class = enable_if_t<
        is_constructible<_Tp, initializer_list<_Up>&, _Args...>::value &&
        is_copy_constructible<_Tp>::value>
  >
  _LIBCPP_INLINE_VISIBILITY
  explicit any(in_place_type_t<_ValueType>, initializer_list<_Up>, _Args&&... __args);

  _LIBCPP_INLINE_VISIBILITY
  ~any() { this->reset(); }

  // assignments
  _LIBCPP_INLINE_VISIBILITY
  any & operator=(any const & __rhs) {
    any(__rhs).swap(*this);
    return *this;
  }

  _LIBCPP_INLINE_VISIBILITY
  any & operator=(any && __rhs) _NOEXCEPT {
    any(_VSTD::move(__rhs)).swap(*this);
    return *this;
  }

  template <
      class _ValueType
    , class _Tp = decay_t<_ValueType>
    , class = enable_if_t<
          !is_same<_Tp, any>::value
          && is_copy_constructible<_Tp>::value>
    >
  _LIBCPP_INLINE_VISIBILITY
  any & operator=(_ValueType && __rhs);

  template <class _ValueType, class ..._Args,
    class _Tp = decay_t<_ValueType>,
    class = enable_if_t<
        is_constructible<_Tp, _Args...>::value &&
        is_copy_constructible<_Tp>::value>
    >
  _LIBCPP_INLINE_VISIBILITY
  _Tp& emplace(_Args&&... args);

  template <class _ValueType, class _Up, class ..._Args,
    class _Tp = decay_t<_ValueType>,
    class = enable_if_t<
        is_constructible<_Tp, initializer_list<_Up>&, _Args...>::value &&
        is_copy_constructible<_Tp>::value>
  >
  _LIBCPP_INLINE_VISIBILITY
  _Tp& emplace(initializer_list<_Up>, _Args&&...);

  // 6.3.3 any modifiers
  _LIBCPP_INLINE_VISIBILITY
  void reset() _NOEXCEPT { if (__h) this->__call(_Action::_Destroy); }

  _LIBCPP_INLINE_VISIBILITY
  void swap(any & __rhs) _NOEXCEPT;

  // 6.3.4 any observers
  _LIBCPP_INLINE_VISIBILITY
  bool has_value() const _NOEXCEPT { return __h != nullptr; }

#if !defined(_LIBCPP_NO_RTTI)
  _LIBCPP_INLINE_VISIBILITY
  const type_info & type() const _NOEXCEPT {
    if (__h) {
        return *static_cast<type_info const *>(this->__call(_Action::_TypeInfo));
    } else {
        return typeid(void);
    }
  }
#endif

private:
    typedef __any_imp::_Action _Action;
    using _HandleFuncPtr =  void* (*)(_Action, any const *, any *, const type_info *,
      const void* __fallback_info);

    union _Storage {
        constexpr _Storage() : __ptr(nullptr) {}
        void *  __ptr;
        __any_imp::_Buffer __buf;
    };

    _LIBCPP_ALWAYS_INLINE
    void * __call(_Action __a, any * __other = nullptr,
                  type_info const * __info = nullptr,
                   const void* __fallback_info = nullptr) const
    {
        return __h(__a, this, __other, __info, __fallback_info);
    }

    _LIBCPP_ALWAYS_INLINE
    void * __call(_Action __a, any * __other = nullptr,
                  type_info const * __info = nullptr,
                  const void* __fallback_info = nullptr)
    {
        return __h(__a, this, __other, __info, __fallback_info);
    }

    template <class>
    friend struct __any_imp::_SmallHandler;
    template <class>
    friend struct __any_imp::_LargeHandler;

    template <class _ValueType>
    friend add_pointer_t<add_const_t<_ValueType>>
    any_cast(any const *) _NOEXCEPT;

    template <class _ValueType>
    friend add_pointer_t<_ValueType>
    any_cast(any *) _NOEXCEPT;

    _HandleFuncPtr __h = nullptr;
    _Storage __s;
};

namespace __any_imp
{
  template <class _Tp>
  struct _LIBCPP_TEMPLATE_VIS _SmallHandler
  {
     _LIBCPP_INLINE_VISIBILITY
     static void* __handle(_Action __act, any const * __this, any * __other,
                           type_info const * __info, const void* __fallback_info)
     {
        switch (__act)
        {
        case _Action::_Destroy:
          __destroy(const_cast<any &>(*__this));
          return nullptr;
        case _Action::_Copy:
            __copy(*__this, *__other);
            return nullptr;
        case _Action::_Move:
          __move(const_cast<any &>(*__this), *__other);
          return nullptr;
        case _Action::_Get:
            return __get(const_cast<any &>(*__this), __info, __fallback_info);
        case _Action::_TypeInfo:
          return __type_info();
        }
    }

    template <class ..._Args>
    _LIBCPP_INLINE_VISIBILITY
    static _Tp& __create(any & __dest, _Args&&... __args) {
        _Tp* __ret = ::new (static_cast<void*>(&__dest.__s.__buf)) _Tp(_VSTD::forward<_Args>(__args)...);
        __dest.__h = &_SmallHandler::__handle;
        return *__ret;
    }

  private:
    _LIBCPP_INLINE_VISIBILITY
    static void __destroy(any & __this) {
        _Tp & __value = *static_cast<_Tp *>(static_cast<void*>(&__this.__s.__buf));
        __value.~_Tp();
        __this.__h = nullptr;
    }

    _LIBCPP_INLINE_VISIBILITY
    static void __copy(any const & __this, any & __dest) {
        _SmallHandler::__create(__dest, *static_cast<_Tp const *>(
            static_cast<void const *>(&__this.__s.__buf)));
    }

    _LIBCPP_INLINE_VISIBILITY
    static void __move(any & __this, any & __dest) {
        _SmallHandler::__create(__dest, _VSTD::move(
            *static_cast<_Tp*>(static_cast<void*>(&__this.__s.__buf))));
        __destroy(__this);
    }

    _LIBCPP_INLINE_VISIBILITY
    static void* __get(any & __this,
                       type_info const * __info,
                       const void* __fallback_id)
    {
        if (__any_imp::__compare_typeid<_Tp>(__info, __fallback_id))
            return static_cast<void*>(&__this.__s.__buf);
        return nullptr;
    }

    _LIBCPP_INLINE_VISIBILITY
    static void* __type_info()
    {
#if !defined(_LIBCPP_NO_RTTI)
        return const_cast<void*>(static_cast<void const *>(&typeid(_Tp)));
#else
        return nullptr;
#endif
    }
  };

  template <class _Tp>
  struct _LIBCPP_TEMPLATE_VIS _LargeHandler
  {
    _LIBCPP_INLINE_VISIBILITY
    static void* __handle(_Action __act, any const * __this,
                          any * __other, type_info const * __info,
                          void const* __fallback_info)
    {
        switch (__act)
        {
        case _Action::_Destroy:
          __destroy(const_cast<any &>(*__this));
          return nullptr;
        case _Action::_Copy:
          __copy(*__this, *__other);
          return nullptr;
        case _Action::_Move:
          __move(const_cast<any &>(*__this), *__other);
          return nullptr;
        case _Action::_Get:
            return __get(const_cast<any &>(*__this), __info, __fallback_info);
        case _Action::_TypeInfo:
          return __type_info();
        }
    }

    template <class ..._Args>
    _LIBCPP_INLINE_VISIBILITY
    static _Tp& __create(any & __dest, _Args&&... __args) {
        typedef allocator<_Tp> _Alloc;
        typedef __allocator_destructor<_Alloc> _Dp;
        _Alloc __a;
        unique_ptr<_Tp, _Dp> __hold(__a.allocate(1), _Dp(__a, 1));
        _Tp* __ret = ::new ((void*)__hold.get()) _Tp(_VSTD::forward<_Args>(__args)...);
        __dest.__s.__ptr = __hold.release();
        __dest.__h = &_LargeHandler::__handle;
        return *__ret;
    }

  private:

    _LIBCPP_INLINE_VISIBILITY
    static void __destroy(any & __this){
        delete static_cast<_Tp*>(__this.__s.__ptr);
        __this.__h = nullptr;
    }

    _LIBCPP_INLINE_VISIBILITY
    static void __copy(any const & __this, any & __dest) {
        _LargeHandler::__create(__dest, *static_cast<_Tp const *>(__this.__s.__ptr));
    }

    _LIBCPP_INLINE_VISIBILITY
    static void __move(any & __this, any & __dest) {
      __dest.__s.__ptr = __this.__s.__ptr;
      __dest.__h = &_LargeHandler::__handle;
      __this.__h = nullptr;
    }

    _LIBCPP_INLINE_VISIBILITY
    static void* __get(any & __this, type_info const * __info,
                       void const* __fallback_info)
    {
        if (__any_imp::__compare_typeid<_Tp>(__info, __fallback_info))
            return static_cast<void*>(__this.__s.__ptr);
        return nullptr;

    }

    _LIBCPP_INLINE_VISIBILITY
    static void* __type_info()
    {
#if !defined(_LIBCPP_NO_RTTI)
        return const_cast<void*>(static_cast<void const *>(&typeid(_Tp)));
#else
        return nullptr;
#endif
    }
  };

} // namespace __any_imp


template <class _ValueType, class _Tp, class>
any::any(_ValueType && __v) : __h(nullptr)
{
  __any_imp::_Handler<_Tp>::__create(*this, _VSTD::forward<_ValueType>(__v));
}

template <class _ValueType, class ..._Args, class _Tp, class>
any::any(in_place_type_t<_ValueType>, _Args&&... __args) {
  __any_imp::_Handler<_Tp>::__create(*this, _VSTD::forward<_Args>(__args)...);
};

template <class _ValueType, class _Up, class ..._Args, class _Tp, class>
any::any(in_place_type_t<_ValueType>, initializer_list<_Up> __il, _Args&&... __args) {
  __any_imp::_Handler<_Tp>::__create(*this, __il, _VSTD::forward<_Args>(__args)...);
}

template <class _ValueType, class, class>
inline _LIBCPP_INLINE_VISIBILITY
any & any::operator=(_ValueType && __v)
{
  any(_VSTD::forward<_ValueType>(__v)).swap(*this);
  return *this;
}

template <class _ValueType, class ..._Args, class _Tp, class>
inline _LIBCPP_INLINE_VISIBILITY
_Tp& any::emplace(_Args&&... __args) {
  reset();
  return __any_imp::_Handler<_Tp>::__create(*this, _VSTD::forward<_Args>(__args)...);
}

template <class _ValueType, class _Up, class ..._Args, class _Tp, class>
inline _LIBCPP_INLINE_VISIBILITY
_Tp& any::emplace(initializer_list<_Up> __il, _Args&&... __args) {
  reset();
  return __any_imp::_Handler<_Tp>::__create(*this, __il, _VSTD::forward<_Args>(__args)...);
}

inline _LIBCPP_INLINE_VISIBILITY
void any::swap(any & __rhs) _NOEXCEPT
{
    if (this == &__rhs)
      return;
    if (__h && __rhs.__h) {
        any __tmp;
        __rhs.__call(_Action::_Move, &__tmp);
        this->__call(_Action::_Move, &__rhs);
        __tmp.__call(_Action::_Move, this);
    }
    else if (__h) {
        this->__call(_Action::_Move, &__rhs);
    }
    else if (__rhs.__h) {
        __rhs.__call(_Action::_Move, this);
    }
}

// 6.4 Non-member functions

inline _LIBCPP_INLINE_VISIBILITY
void swap(any & __lhs, any & __rhs) _NOEXCEPT
{
    __lhs.swap(__rhs);
}

template <class _Tp, class ..._Args>
inline _LIBCPP_INLINE_VISIBILITY
any make_any(_Args&&... __args) {
    return any(in_place_type<_Tp>, _VSTD::forward<_Args>(__args)...);
}

template <class _Tp, class _Up, class ..._Args>
inline _LIBCPP_INLINE_VISIBILITY
any make_any(initializer_list<_Up> __il, _Args&&... __args) {
    return any(in_place_type<_Tp>, __il, _VSTD::forward<_Args>(__args)...);
}

template <class _ValueType>
inline _LIBCPP_INLINE_VISIBILITY
_ValueType any_cast(any const & __v)
{
    using _RawValueType = __uncvref_t<_ValueType>;
    static_assert(is_constructible<_ValueType, _RawValueType const &>::value,
                  "ValueType is required to be a const lvalue reference "
                  "or a CopyConstructible type");
    auto __tmp = _VSTD::any_cast<add_const_t<_RawValueType>>(&__v);
    if (__tmp == nullptr)
        __throw_bad_any_cast();
    return static_cast<_ValueType>(*__tmp);
}

template <class _ValueType>
inline _LIBCPP_INLINE_VISIBILITY
_ValueType any_cast(any & __v)
{
    using _RawValueType = __uncvref_t<_ValueType>;
    static_assert(is_constructible<_ValueType, _RawValueType &>::value,
                  "ValueType is required to be an lvalue reference "
                  "or a CopyConstructible type");
    auto __tmp = _VSTD::any_cast<_RawValueType>(&__v);
    if (__tmp == nullptr)
        __throw_bad_any_cast();
    return static_cast<_ValueType>(*__tmp);
}

template <class _ValueType>
inline _LIBCPP_INLINE_VISIBILITY
_ValueType any_cast(any && __v)
{
    using _RawValueType = __uncvref_t<_ValueType>;
    static_assert(is_constructible<_ValueType, _RawValueType>::value,
                  "ValueType is required to be an rvalue reference "
                  "or a CopyConstructible type");
    auto __tmp = _VSTD::any_cast<_RawValueType>(&__v);
    if (__tmp == nullptr)
        __throw_bad_any_cast();
    return static_cast<_ValueType>(_VSTD::move(*__tmp));
}

template <class _ValueType>
inline _LIBCPP_INLINE_VISIBILITY
add_pointer_t<add_const_t<_ValueType>>
any_cast(any const * __any) _NOEXCEPT
{
    static_assert(!is_reference<_ValueType>::value,
                  "_ValueType may not be a reference.");
    return _VSTD::any_cast<_ValueType>(const_cast<any *>(__any));
}

template <class _RetType>
inline _LIBCPP_INLINE_VISIBILITY
_RetType __pointer_or_func_cast(void* __p, /*IsFunction*/false_type) noexcept {
  return static_cast<_RetType>(__p);
}

template <class _RetType>
inline _LIBCPP_INLINE_VISIBILITY
_RetType __pointer_or_func_cast(void*, /*IsFunction*/true_type) noexcept {
  return nullptr;
}

template <class _ValueType>
add_pointer_t<_ValueType>
any_cast(any * __any) _NOEXCEPT
{
    using __any_imp::_Action;
    static_assert(!is_reference<_ValueType>::value,
                  "_ValueType may not be a reference.");
    typedef typename add_pointer<_ValueType>::type _ReturnType;
    if (__any && __any->__h) {
      void *__p = __any->__call(_Action::_Get, nullptr,
#if !defined(_LIBCPP_NO_RTTI)
                          &typeid(_ValueType),
#else
                          nullptr,
#endif
                          __any_imp::__get_fallback_typeid<_ValueType>());
        return _VSTD::__pointer_or_func_cast<_ReturnType>(
            __p, is_function<_ValueType>{});
    }
    return nullptr;
}

#endif // _LIBCPP_STD_VER > 14

_LIBCPP_END_NAMESPACE_STD

#endif // _LIBCPP_ANY
