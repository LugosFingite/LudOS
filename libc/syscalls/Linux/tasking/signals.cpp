/*
signals.cpp

Copyright (c) 15 Yann BOUCHER (yann)

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
#include "syscalls/syscall_list.hpp"
#include "syscalls/defs.hpp"

#include <signal.h>
#include <errno.h>

extern "C"
{


LINUX_SYSCALL_DEFAULT_IMPL(kill,   2, int, (pid_t pid, int sig), pid, sig)
LINUX_SYSCALL_DEFAULT_IMPL(signal, 2, sighandler_t, (int num, sighandler_t handler), num, handler)
LINUX_SYSCALL_DEFAULT_IMPL(sigaction, 3, int, (int num, const struct sigaction* action, struct sigaction* old), num, action, old)

}
