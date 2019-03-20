/*
clone.cpp

Copyright (c) 19 Yann BOUCHER (yann)

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

#include "i686/syscalls/syscall.hpp"

#include "tasking/process.hpp"
#include "i686/tasking/process.hpp"

#include "errno.h"

static void fork_return()
{
    Process::current().jump_to_user_space();
}

int sys_clone(int flags, user_ptr<void> child_stack)
{
    auto child = Process::clone(Process::current(), flags);
    if (!child) return -ENOMEM;

    child->arch_context->init_regs->eip = (uintptr_t)fork_return;

    *child->arch_context->user_regs = *Process::current().arch_context->user_regs;
    child->arch_context->user_regs->eax = 0; // return zero in the child
    child->arch_context->user_regs->esp = child_stack.as_raw();

    return child->pid;
}
