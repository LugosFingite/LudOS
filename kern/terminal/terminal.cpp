/*
terminal.cpp

Copyright (c) 16 Yann BOUCHER (yann)

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

#include "terminal.hpp"

#include <ctype.h>
#include <memory.hpp>

#include "utils/kmsgbus.hpp"

#include "nullterm.hpp"

#include "utils/logging.hpp"
#include "utils/stlutils.hpp"
#include "unicode/utf32decoder.hpp"

#include "drivers/sound/beep.hpp"

std::unique_ptr<Terminal> current_term;
std::unique_ptr<TerminalData> current_termdata;

// TODO : editing line wrapping

Terminal::Terminal(size_t iwidth, size_t iheight, TerminalData &data)
    : m_width(iwidth), m_height(iheight), m_data(data)
{
    m_dirty_width_per_line.resize(iheight, 0);
}

void Terminal::put_char(char32_t c)
{
    if (m_escape_code)
    {
        if (m_expecting_bracket)
        {
            if (c == '[')
            {
                m_expecting_bracket = false;
                return;
            }
            else
            {
                m_escape_code = false;
                m_escape_sequence.clear();
                m_expecting_bracket = false;
                return;
            }
        }

        if (c >= 0x40 && c <= 0x7E)
        {
            m_escape_code = false;
            process_escape_code();
            m_escape_sequence.clear();
        }
        else
        {
            m_escape_sequence.push_back(c);
        }

        return;
    }

    else if (c == '\e')
    {
        m_escape_code = true;
        m_expecting_bracket = true;
    }
    else if (c == '\n')
    {
        new_line();
    }
    else if (c == '\r')
    {
        m_cursor_x = 0;
    }
    else if (c == '\b')
    {
        if (m_cursor_x > m_input_off)
        {
            --m_cursor_x;

            check_pos();
            set_entry_at({' ', m_data.color()}, m_cursor_x, m_cursor_y);
            //m_cur_line.pop_back();
            m_cur_line.erase(m_cur_line.begin() + m_cursor_x);
        }
    }
    else if (c == '\t')
    {
        for (size_t i { 0 }; i < tab_size; ++i)
        {
            put_char(' ');
        }
    }
    else if (c == '\a')
    {
        beep(200);
    }
    else
    {
        set_entry_at({c, m_data.color()}, m_cursor_x, m_cursor_y);
        if (m_cursor_x < width()-1) m_cur_line.insert(m_cur_line.begin() + m_cursor_x, {c, m_data.color()});
        else    m_cur_line.push_back({c, m_data.color()});
        ++m_cursor_x;
    }

    check_pos();
}

void Terminal::add_input(char32_t c)
{
    if (m_accept_input)
    {
        switch_to_input();
        put_char(c);
    }
}

void Terminal::clear_input()
{
    while (m_cur_line.size() > m_input_off)
    {
        m_cur_line.pop_back();
        if (m_cursor_x > 0) --m_cursor_x;
    }

    force_redraw_input();
}

void Terminal::set_input(const kpp::string &str)
{
    while (m_cur_line.size() > m_input_off)
    {
        m_cur_line.pop_back();
        if (m_cursor_x > 0) --m_cursor_x;
    }

    switch_to_input();

    write_string(str.c_str());

    force_redraw_input();
}

void Terminal::switch_to_input()
{
    if (!m_line_is_input)
    {
        m_line_is_input = true;
        m_input_off = m_cursor_x;
    }
}

void Terminal::write(const char *data, size_t size)
{
    for (size_t i = 0; i < size; i++)
    {
        m_decoder.feed(data[i]);
        if (m_decoder.ready())
        {
            char32_t c = m_decoder.spit();
            put_char(c);
        }
    }
}


void Terminal::write_string(const char *data)
{
    write(data, strlen(data));
}

void Terminal::forward_delete()
{
    if (m_cursor_x < m_cur_line.size())
    {
        m_cur_line.erase(m_cur_line.begin() + m_cursor_x);
    }
}

void Terminal::clear()
{
    clear(m_data.color());
}

void Terminal::clear(ColorPair color)
{
    m_cursor_x = m_cursor_y = 0;
    m_current_history_page = 0;
    m_cur_line.clear();
    m_decoder.reset();

    for (size_t i { 0 }; i < true_height(); ++i)
    {
        clear_line(i, color.bg, width());
    }

    for (auto& dirty_width : m_dirty_width_per_line)
        dirty_width = width();

    m_dirty = true;
    draw();
}

void Terminal::move_cursor(int offset)
{
    m_cursor_x += offset;
    if (m_cursor_x < m_input_off) m_cursor_x = m_input_off;
    if (m_cursor_x > m_cur_line.size()) m_cursor_x = m_cur_line.size();
}

void Terminal::scroll_up()
{
    scroll_history(+1);
}

void Terminal::scroll_bottom()
{
    show_history(m_data.lines() - height()+1);
    m_cursor_y = (height()-1);
    check_pos();
}


void Terminal::show_history(int page)
{
    if (m_scrolling)
    {
        if (page < 0)
        {
            page = 0;
        }

        if (static_cast<size_t>(page) > m_data.lines() - height()+1)
        {
            page = m_data.lines() - height()+1; // avoir un plafond, une limite
        }

        m_current_history_page = page;

        auto screen = m_data.get_screen(width(), height(), page);

        const auto color = m_data.color().bg;

        for (size_t i { 0 }; (i + m_data.title_height) < true_height() && i < screen.size(); ++i)
        {
            clear_line_before_write(i + m_data.title_height, color, screen[i].size());
            for (size_t j { 0 }; j < screen[i].size(); ++j)
            {
                set_entry_at(screen[i][j], j, i);
            }
        }

        force_redraw_input();
    }
}

size_t Terminal::current_history() const
{
    return m_current_history_page;
}

void Terminal::scroll_history(int scroll)
{
    show_history(current_history()+scroll);
    draw();
}

void Terminal::set_title(kpp::u32string str, ColorPair color)
{
    m_data.title_str = str;

    if (str.size() >= width())
    {
        str.resize(width());
    }

    m_data.title_color = color;

    size_t offset = width()/2 - str.size()/2;

    for (size_t j { 0 }; j < m_data.title_height; ++j)
    {
        clear_line(j, color.bg, width());
    }

    for (size_t i { 0 }; i < str.size(); ++i)
    {
        set_entry_at({str[i], color}, offset+i, 0, true);
    }

    draw();
}

void Terminal::set_title(kpp::u32string str)
{
    set_title(std::move(str), m_data.color());
}

void Terminal::set_accept_input(bool val)
{
    m_accept_input = val;
}

kpp::string Terminal::input() const
{
    kpp::string str;
    for (size_t i { m_input_off }; i < m_cur_line.size(); ++i)
    {
        str += decode_utf32(m_cur_line[i].c);
    }
    return str;
}

void Terminal::set_input_color(size_t pos, size_t sz, ColorPair color)
{
    if (m_accept_input)
    {
        for (size_t i { m_input_off + pos }; i < std::min(m_input_off + pos + sz, m_cur_line.size()); ++i)
        {
            m_cur_line[i].pair = color;
        }

        force_redraw_input();
    }
}

void Terminal::set_entry_at(TermEntry entry, size_t x, size_t y, bool absolute)
{
    if (m_enabled && y < height())
    {
        putchar(x, y + (absolute ? 0 : m_data.title_height), entry);
        m_dirty = true;
    }
}

void Terminal::clear_line_before_write(size_t y, graphics::Color color, size_t size)
{
    const size_t dirty_width = m_dirty_width_per_line[y];
    const size_t width_to_clear = std::max(size, dirty_width);

    clear_line(y, color, width_to_clear);

    m_dirty_width_per_line[y] = size;
}

void Terminal::new_line()
{
    if (!m_line_is_input || true)
    {
        add_line_to_history();
    }

    m_cursor_x = 0;
    ++m_cursor_y;

    check_pos();

    scroll_up();

    if (m_line_is_input)
    {
        m_line_is_input = false;
        TermInputEvent ev;
        ev.line = input();

        kmsgbus.send(ev);

        m_dirty_width_per_line[m_cursor_y] = 0;
    }

    clear_line(m_cursor_y, m_data.color().bg, width());
    m_cur_line.clear();
}

void Terminal::add_line_to_history()
{
    m_data.add_line(m_cur_line);
}

void Terminal::check_pos()
{
    if (m_cursor_y >= height())
    {
        m_cursor_y = height()-1;
    }
    if (m_cursor_x >= width())
    {
        m_cursor_x = width()-1;
    }

    update_cursor();
}

void Terminal::update_cursor()
{
    move_cursor(m_cursor_x, m_cursor_y + m_data.title_height);
}

void Terminal::reset()
{
    clear({0xaaaaaa, 0xaaaaaa});

    m_cur_line.clear();
}

void Terminal::process_escape_code()
{
    auto param_list = tokenize<std::vector<kpp::string>>(m_escape_sequence, ";");

    if (param_list.empty()) return;

    if (param_list.size() == 5 && param_list[0] == "38" && param_list[1] == "2")
    {
        ColorPair pair = m_data.color();
        pair.fg = graphics::Color(kpp::stoul(param_list[2]),kpp::stoul(param_list[3]),kpp::stoul(param_list[4]));

        m_data.push_color(pair);
    }
    if (param_list.size() == 5 && param_list[0] == "48" && param_list[1] == "2")
    {
        ColorPair pair = m_data.color();
        pair.bg = graphics::Color(kpp::stoul(param_list[2]),kpp::stoul(param_list[3]),kpp::stoul(param_list[4]));

        m_data.push_color(pair);
    }
    if (param_list.size() == 1 && (param_list[0] == "39" || param_list[0] == "49"))
    {
        if (m_data.color_stack.size() > 1) m_data.pop_color();
    }
}

void Terminal::clear_screen(graphics::Color color)
{
    for (size_t i { 0 }; i < height(); ++i)
    {
        clear_line(i, color, width());
    }
}

void Terminal::resize(size_t iwidth, size_t iheight)
{
    assert(iwidth && iheight);

    reset();

    m_width = iwidth;
    m_height = iheight;

    m_dirty_width_per_line.resize(iheight, width()); // clear again

    set_title(m_data.title_str, m_data.title_color);

    check_pos();

    force_redraw();
}

size_t Terminal::width() const
{
    return m_width;
}

size_t Terminal::height() const
{
    return m_height - m_data.title_height;
}

size_t Terminal::true_height() const
{
    return m_height;
}

void Terminal::force_redraw()
{
    update_cursor();

    show_history(m_current_history_page);

    force_redraw_input();

    set_title(m_data.title_str, m_data.title_color);
}

void Terminal::force_redraw_input()
{
    check_pos();

    clear_line(std::min(m_cursor_y + m_data.title_height, true_height()-1), m_data.color().bg,
               width());

    for (size_t i { 0 }; i < std::min(m_cur_line.size(), width()); ++i)
    {
        set_entry_at(m_cur_line[i], i, m_cursor_y);
    }

    draw();
}

void Terminal::draw()
{
    if (m_enabled && m_dirty)
    {
        m_dirty = false;
        draw_impl();
    }
}

Terminal& term()
{
    if (current_term == nullptr)
    {
        static TerminalData dummy(1);
        create_term<NullTerminal>(dummy);
    }

    assert(current_term);
    return *current_term;
}

TerminalData &term_data()
{
    if (!current_termdata)
    {
        current_termdata = std::make_unique<TerminalData>(1000);
    }

    return *current_termdata;
}

void reset_term()
{
    create_term<NullTerminal>(term_data());
}
