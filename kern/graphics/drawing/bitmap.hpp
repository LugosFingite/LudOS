/*
bitmap.hpp

Copyright (c) 08 Yann BOUCHER (yann)

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
#ifndef BITMAP_HPP
#define BITMAP_HPP

#include <stdint.h>

#include <vector.hpp>

#include "graphics/color.hpp"
#include "graphics/point.hpp"

namespace graphics
{

class Bitmap
{
public:
    Bitmap() = default;

    Bitmap(size_t width, size_t height, Color color = color_black);

public:
    void resize(size_t width, size_t height, bool keep_ratio = true, Color color = color_black);

    const Color &operator[](const PointU& point) const
    {
        //assert(point.x < m_width && point.y < m_height);

        return m_data[point.y * width() + point.x];
    }

    Color& operator[](const PointU& point)
    {
        //assert(point.x < m_width && point.y < m_height);

        return m_data[point.y * width() + point.x];
    }

    Color* data() { return m_data.data(); }
    const Color* data() const { return m_data.data(); }

    Bitmap copy_rect(const PointU& pos, const PointU& size) const;

    void color_multiply(const Color& color);

    void color_blend(const Color& white, const Color& transparent);

    size_t width() const { return m_width; }
    size_t height() const { return m_height; }

private:
    size_t m_width { 0 };
    size_t m_height { 0 };
    std::vector<Color> m_data;
};

void blit(Color* buffer, size_t buf_width, size_t buf_height, const Bitmap& bmp, const PointU& pos);
void blit(Color* buffer, size_t buf_width, size_t buf_height, const Bitmap &bitmap, const PointU &pos, const Color& white);
void blit(Color* buffer, size_t buf_width, size_t buf_height, const Bitmap &bitmap, const PointU &pos, const Color& white, const Color& transparent);


}

#endif // BITMAP_HPP
