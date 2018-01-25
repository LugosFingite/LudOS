
/*
messagebus.tpp

Copyright (c) 23 Yann BOUCHER (yann)

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

template <typename T>
MessageBus::Handle MessageBus::register_handler(std::function<void(const T&)> handler)
{
    handlers[std::type_index(typeid(T))].push_back(handler);
    return {std::type_index(typeid(T)), --handlers[std::type_index(typeid(T))].end()};
}

template <typename T>
size_t MessageBus::send(const T& event)
{
    size_t counter { 0 };
    for (const auto& callback : handlers[std::type_index(typeid(T))])
    {
        ++counter;
        std::any_cast<std::function<void(const T&)>>(callback)(event);
    }

    return counter;
}
