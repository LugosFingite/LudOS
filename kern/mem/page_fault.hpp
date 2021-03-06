/*
page_fault.hpp

Copyright (c) 03 Yann BOUCHER (yann)

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
#ifndef PAGE_FAULT_HPP
#define PAGE_FAULT_HPP

#include <stdint.h>

#include <functional.hpp>

struct PageFault
{
    void*                           mcontext;
    uintptr_t                       address;
    enum { Kernel, User           } level;
    enum { Protection, NonPresent } error;
    enum { Read, Write, Execute   } type ;
};

void page_fault_entry(const PageFault& fault);

/*
 * Handles page fault, returns true if handled by the callback or false if not
 */
using fault_handle = int;
using fault_callback = std::function<bool(const PageFault&)>;

fault_handle attach_fault_handler(void* v_addr, const fault_callback& handler);
void detach_fault_handler(fault_handle hdl);

#endif // PAGE_FAULT_HPP
