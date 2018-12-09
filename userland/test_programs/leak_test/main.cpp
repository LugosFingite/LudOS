/*
main.cpp

Copyright (c) 28 Yann BOUCHER (yann)

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

#include <syscalls/syscall_list.hpp>

#include "stdio.h"

void test()
{
    int ret = fork();
    if (ret < 0)
    {
        return;
    }
    else if (ret == 0)
    {
        exit(1);

        return;
    }
    else
    {
        int status;
        waitpid(ret, &status, 0);
    }
}

uint64_t total_ticks()
{
    uint64_t ret;
    asm volatile ( "rdtsc" : "=A"(ret) );
    return ret;
}

int main()
{
    syscall_nop();

    int pid = fork();
    if (pid == 0)
    {
        while (true)
        {
            sched_yield();
        }
    }
    else
    {
        uint64_t total_test_ticks = 0;

        for (size_t i { 0 }; i < 1000; ++i)
        {
            uint64_t begin = total_ticks();
            sched_yield();
            total_test_ticks += (total_ticks() - begin);
        }

        printf("total ticks : %lld\n", total_test_ticks/1000 / 2400 / 2);

        kill(pid, SIGKILL);
    }

    syscall_nop();
}
