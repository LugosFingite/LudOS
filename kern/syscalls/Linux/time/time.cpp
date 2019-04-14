/*
time.cpp

Copyright (c) 14 Yann BOUCHER (yann)

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*/

#include <sys/time.h>
#include <errno.h>
#include <time.h>

#include "time/time.hpp"
#include "time/timer.hpp"

#include "utils/user_ptr.hpp"

time_t sys_time(user_ptr<time_t> t_loc)
{
    if (t_loc.as_raw() != (uintptr_t)nullptr && !t_loc.check()) return -EFAULT;

    const auto epoch = Time::epoch();
    if (t_loc.as_raw() != (uintptr_t)nullptr) *t_loc.get() = epoch;

    return epoch;
}

int sys_clock_gettime(clockid_t clock, user_ptr<timespec> tp)
{
    if (!tp.check())
        return -EFAULT;

    if (clock != CLOCK_REALTIME)
        return -EINVAL;

    const uint64_t ticks = Time::ticks_since_boot();

    const uint64_t secs = ticks / Timer::freq();
    const uint64_t nanosec_ticks = ticks % Timer::freq();

    const uint64_t nanosecs = (nanosec_ticks * 1'000'000'000) / Timer::freq();

    tp.get()->tv_sec = secs;
    tp.get()->tv_nsec = nanosecs;

    return EOK;
}
