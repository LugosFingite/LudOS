///////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2015 Microsoft Corporation. All rights reserved.
//
// This code is licensed under the MIT License (MIT).
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef GSL_CONTRACTS_H
#define GSL_CONTRACTS_H

//
// There are three configuration options for this GSL implementation's behavior
// when pre/post conditions on the GSL types are violated:
//
// 1. GSL_TERMINATE_ON_CONTRACT_VIOLATION: std::terminate will be called (default)
// 2. GSL_THROW_ON_CONTRACT_VIOLATION: a gsl::fail_fast exception will be thrown
// 3. GSL_UNENFORCED_ON_CONTRACT_VIOLATION: nothing happens
//
#if !(defined(GSL_THROW_ON_CONTRACT_VIOLATION) || defined(GSL_TERMINATE_ON_CONTRACT_VIOLATION) ||  \
      defined(GSL_UNENFORCED_ON_CONTRACT_VIOLATION))
#define GSL_TERMINATE_ON_CONTRACT_VIOLATION
#endif

#define GSL_STRINGIFY_DETAIL(x) #x
#define GSL_STRINGIFY(x) GSL_STRINGIFY_DETAIL(x)

#if defined(__clang__) || defined(__GNUC__)
#define GSL_LIKELY(x) __builtin_expect(!!(x), 1)
#define GSL_UNLIKELY(x) __builtin_expect(!!(x), 0)
#else
#define GSL_LIKELY(x) (!!(x))
#define GSL_UNLIKELY(x) (!!(x))
#endif

//
// GSL_ASSUME(cond)
//
// Tell the optimizer that the predicate cond must hold. It is unspecified
// whether or not cond is actually evaluated.
//
#ifdef _MSC_VER
#define GSL_ASSUME(cond) __assume(cond)
#elif defined(__GNUC__)
#define GSL_ASSUME(cond) ((cond) ? static_cast<void>(0) : __builtin_unreachable())
#else
#define GSL_ASSUME(cond) static_cast<void>((cond) ? 0 : 0)
#endif

//
// GSL.assert: assertions
//

#if defined(GSL_THROW_ON_CONTRACT_VIOLATION)

#define GSL_CONTRACT_CHECK(type, cond)                                                             \
    (GSL_LIKELY(cond) ? static_cast<void>(0)                                                       \
                      : throw gsl::fail_fast("GSL: " type " failure at " __FILE__                  \
                                             ": " GSL_STRINGIFY(__LINE__)))

#elif defined(GSL_TERMINATE_ON_CONTRACT_VIOLATION)

#define GSL_CONTRACT_CHECK(type, cond) (GSL_LIKELY(cond) ? static_cast<void>(0) : abort())

#elif defined(GSL_UNENFORCED_ON_CONTRACT_VIOLATION)

#define GSL_CONTRACT_CHECK(type, cond) GSL_ASSUME(cond)

#endif

#define Expects(cond) GSL_CONTRACT_CHECK("Precondition", cond)
#define Ensures(cond) GSL_CONTRACT_CHECK("Postcondition", cond)

#endif // GSL_CONTRACTS_H
