/*
mtrr.hpp

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
#ifndef MTRR_HPP
#define MTRR_HPP

#include <stdint.h>

namespace mtrr
{
enum Type : uint8_t
{
    UC = 0x00,
    WC = 0x01,
    WT = 0x04,
    WP = 0x05,
    WB = 0x06
};

struct VariableMTRR
{
    uint64_t base;
    uint64_t mask;
};

bool available();
bool enabled();
bool supports_write_combining();
uint8_t available_variable_ranges();
bool range_enabled(size_t id);
VariableMTRR get_range(size_t id);

void set_mtrrs_enabled(bool val);
void set_fixed_mtrrs_enabled(bool val);

int set_variable_mtrr(uint64_t base_addr, uint64_t size, Type type);

void prepare_set();
void post_set();
}

#endif // MTRR_HPP
