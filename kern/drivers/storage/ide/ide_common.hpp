/*
ide_common.hpp

Copyright (c) 07 Yann BOUCHER (yann)

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
#ifndef IDE_COMMON_HPP
#define IDE_COMMON_HPP

#include <stdint.h>

#include <string.hpp>

#include "utils/stlutils.hpp"

namespace ide
{

struct [[gnu::packed]] identify_data
{
    uint16_t flags; // 0
    uint16_t unused1[9]; // 9
    char     serial[20]; // 19
    uint16_t unused2[3]; // 22
    char     firmware[8]; // 26
    char    model[40]; // 46
    uint16_t sectors_per_int; // 47
    uint16_t unused3; // 48
    uint16_t capabilities[2]; // 50
    uint16_t unused4[2]; // 52
    uint16_t valid_ext_data; // 53
    uint16_t unused5[5]; // 58
    uint16_t size_of_rw_mult; // 59
    uint32_t sectors_28; // 61
    uint16_t unused6[38]; // 99
    uint64_t sectors_48; // 103
    uint16_t unused7[2]; // 105
    uint16_t phys_log_size; // 106
    uint16_t unused8[10]; // 116
    uint32_t sector_size; // 118
    uint16_t unused9[137];
};

static_assert(sizeof(identify_data) == 512);

enum DriveType : uint8_t
{
    Master = 0xE0,
    Slave = 0xF0
};

enum BusPort : uint16_t
{
    Primary = 0x1F0,
    Secondary = 0x170,
    Third = 0x1E8,
    Fourth = 0x168
};

template <typename T>
std::string ata_string(T&& arr)
{
    using type = std::remove_reference_t<T>;
    static_assert(std::is_array_v<type>, "Must use const char[] !");

    std::string str;
    for (size_t i { 0 }; i < std::extent_v<type>; i+=2)
    {
        str += arr[i + 1];
        str += arr[i];
    }

    return trim_right(str);
}

}

#endif // IDE_COMMON_HPP