/*
MIT License

Copyright (c) 2019 Lucas Kleiss (LAK132)

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

#ifndef LAK_TOSTRING_HPP
#define LAK_TOSTRING_HPP

#include "strconv.hpp"

namespace lak
{
    template<typename TO, typename FROM>
    std::basic_string<TO> to_string(FROM from)
    {
        return strconv<TO>(std::to_string(from));
    }

    template<typename FROM>
    std::string to_string(FROM from)
    {
        return std::to_string(from);
    }

    template<typename FROM>
    std::wstring to_wstring(FROM from)
    {
        return std::to_wstring(from);
    }

    template<typename FROM>
    u8string to_u8string(FROM from)
    {
        return strconv_u8(std::to_string(from));
    }

    template<typename FROM>
    std::u16string to_u16string(FROM from)
    {
        return strconv_u16(std::to_string(from));
    }

    template<typename FROM>
    std::u32string to_u32string(FROM from)
    {
        return strconv_u32(std::to_string(from));
    }
}

#endif // LAK_TOSTRING_HPP