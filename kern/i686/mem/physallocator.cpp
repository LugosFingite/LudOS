/*
physallocator.cpp

Copyright (c) 11 Yann BOUCHER (yann)

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

#include "physallocator.hpp"

#include "panic.hpp"

uintptr_t PhysPageAllocator::alloc_physical_page()
{
loop:
    for (size_t i { 0 }; i < mem_bitmap.array_size; ++i)
    {
        if (!mem_bitmap[i])
        {
            mem_bitmap[i] = true;
            last_pos = i+1;
            return i*Paging::page_size;
        }
    }

    // Reloop from the start if not found
    if (last_pos != 0)
    {
        last_pos = 0;
        goto loop;
    }

    // Free essential memory
    for (size_t i { 0 }; i < 0x1000; ++i)
    {
        mem_bitmap[i] = false;
    }

    log_serial("Out of memory\n");
    panic("Out of memory !\n");

    return 0;
}

bool PhysPageAllocator::release_physical_page(uintptr_t p_addr)
{
    const size_t base_page = p_addr >> 12;

    const bool released = mem_bitmap[base_page];
    mem_bitmap[base_page] = false;

    assert(released);

    return released;
}

void PhysPageAllocator::mark_as_used(uintptr_t addr, size_t size)
{
    for (size_t i { addr >> 12 }; i < (addr + size) >> 12; ++i)
    {
        mem_bitmap[i] = true;
    }
}

void PhysPageAllocator::mark_as_free(uintptr_t addr, size_t size)
{
    for (size_t i { addr >> 12 }; i < (addr + size) >> 12; ++i)
    {
        mem_bitmap[i] = false;
    }
}
