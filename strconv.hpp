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

#ifndef LAK_STRCONV_HPP
#define LAK_STRCONV_HPP

#include <string>
#include <type_traits>

namespace lak
{
    // char8_t typdef for C++ < 20
    #if __cplusplus > 201703L
    using std::u8string;
    #else
    using char8_t = uint_least8_t;
    using u8string = std::basic_string<char8_t>;
    #endif

    template<typename FROM>
    struct string_conv_iterator
    {
    private:
        using _string = std::basic_string<FROM>;
        using _iterator = typename _string::const_iterator;
        _iterator _begin;
        _iterator _end;
        char32_t _current = 0;
    public:
        string_conv_iterator(const _string& str)
        : _begin(str.begin()), _end(str.end()) { operator++(); }
        char32_t operator*() const { return _current; }
        string_conv_iterator &operator++();
    };

    using ascii_code_iterator_t = string_conv_iterator<char>;
    using wide_code_iterator_t = string_conv_iterator<wchar_t>;

    using utf8_code_iterator_t = string_conv_iterator<char8_t>;
    using utf16_code_iterator_t = string_conv_iterator<char16_t>;
    using utf32_code_iterator_t = string_conv_iterator<char32_t>;

    template<>
    ascii_code_iterator_t &ascii_code_iterator_t::operator++()
    {
        _current = 0;
        if (_begin != _end)
        {
            _current = *_begin;
            ++_begin;
        }
        return *this;
    }

    template<>
    utf8_code_iterator_t &utf8_code_iterator_t::operator++()
    {
        _current = 0;
        if (_begin != _end)
        {
            const char8_t c = *_begin;
            ++_begin;
            if (c >= 0x00 && c <= 0x7F)
            {
                _current = c;
            }
            else if (c >= 0xC0 && c <= 0xDF && (_end - _begin) > 0)
            {
                _current = ((c - 0xC0) << 6) | (*_begin - 0x80);
                ++_begin;
            }
            else if (c >= 0xE0 && c <= 0xEF && (_end - _begin) > 1)
            {
                _current = (c - 0xE0) << 12;
                _current |= (*_begin - 0x80) << 6;
                ++_begin;
                _current |= *_begin - 0x80;
                ++_begin;
            }
            else if (c >= 0xF0 && c <= 0xF7 && (_end - _begin) > 2)
            {
                _current = (c - 0xF0) << 18;
                _current |= (*_begin - 0x80) << 12;
                ++_begin;
                _current |= (*_begin - 0x80) << 6;
                ++_begin;
                _current |= *_begin - 0x80;
                ++_begin;
            }
        }
        return *this;
    }

    template<>
    utf16_code_iterator_t &utf16_code_iterator_t::operator++()
    {
        _current = 0;
        if (_begin != _end)
        {
            const char16_t c = *_begin;
            ++_begin;
            if ((c >= 0x0000 && c <= 0xD7FF) || (c >= 0xE000 && c <= 0xFFFF))
            {
                _current = c;
            }
            else if (_begin != _end)
            {
                _current = 0x010000 | (0xD800 + (c << 10));
                _current |= 0xDC00 + *_begin;
                ++_begin;
            }

        }
        return *this;
    }

    template<>
    utf32_code_iterator_t &utf32_code_iterator_t::operator++()
    {
        _current = 0;
        if (_begin != _end)
        {
            _current = *_begin;
            ++_begin;
        }
        return *this;
    }

    template<>
    wide_code_iterator_t &wide_code_iterator_t::operator++()
    {
        _current = 0;
        if (_begin != _end)
        {
            const wchar_t c = *_begin;
            ++_begin;
            if constexpr (sizeof(wchar_t) == sizeof(char8_t))
            {
                if (c >= 0x00 && c <= 0x7F)
                {
                    _current = c;
                }
                else if (c >= 0xC0 && c <= 0xDF && (_end - _begin) > 0)
                {
                    _current = ((c - 0xC0) << 6) | (*_begin - 0x80);
                    ++_begin;
                }
                else if (c >= 0xE0 && c <= 0xEF && (_end - _begin) > 1)
                {
                    _current = (c - 0xE0) << 12;
                    _current |= (*_begin - 0x80) << 6;
                    ++_begin;
                    _current |= *_begin - 0x80;
                    ++_begin;
                }
                else if (c >= 0xF0 && c <= 0xF7 && (_end - _begin) > 2)
                {
                    _current = (c - 0xF0) << 18;
                    _current |= (*_begin - 0x80) << 12;
                    ++_begin;
                    _current |= (*_begin - 0x80) << 6;
                    ++_begin;
                    _current |= *_begin - 0x80;
                    ++_begin;
                }
            }
            else if constexpr (sizeof(wchar_t) == sizeof(char16_t))
            {
                if ((c >= 0x0000 && c <= 0xD7FF) || (c >= 0xE000 && c <= 0xFFFF))
                {
                    _current = c;
                }
                else if (_begin != _end)
                {
                    _current = 0x010000 | (0xD800 + (c << 10));
                    _current |= 0xDC00 + *_begin;
                    ++_begin;
                }
            }
            else
            {
                _current = c;
            }
        }
        return *this;
    }

    inline void append_code(std::string &str, const char32_t code)
    {
        if (code <= 0x7F)
            str += static_cast<char>(code);
    }

    inline void append_code(u8string &str, const char32_t code)
    {
        if (code <= 0x7F)
        {
            str += static_cast<char8_t>(code);
        }
        else if (code >= 0x80 && code <= 0x7FF)
        {
            str += 0xC0 + static_cast<char8_t>(code >> 6);
            str += 0x80 + static_cast<char8_t>(code & 0x3F);
        }
        else if (code >= 0x800 && code <= 0xFFFF)
        {
            str += 0xE0 + static_cast<char8_t>(code >> 12);
            str += 0x80 + static_cast<char8_t>((code >> 6) & 0x3F);
            str += 0x80 + static_cast<char8_t>(code & 0x3F);
        }
        else if (code >= 0x10000 && code <= 0x10FFFF)
        {
            str += 0xF0 + static_cast<char8_t>(code >> 18);
            str += 0x80 + static_cast<char8_t>((code >> 12) & 0x3F);
            str += 0x80 + static_cast<char8_t>((code >> 6) & 0x3F);
            str += 0x80 + static_cast<char8_t>(code & 0x3F);
        }
    }

    inline void append_code(std::u16string &str, const char32_t code)
    {
        if (code <= 0xFFFF)
        {
            str += static_cast<char16_t>(code);
        }
        else
        {
            const auto _code = code - 0x01000;
            str += static_cast<char16_t>(0xD800 + ((_code >> 10) & 0x3FF));
            str += static_cast<char16_t>(0xDC00 + (_code & 0x3FF));
        }
    }

    inline void append_code(std::u32string &str, const char32_t code)
    {
        str += code;
    }

    inline void append_code(std::wstring &str, const char32_t code)
    {
        if constexpr (sizeof(wchar_t) == sizeof(char8_t))
        {
            if (code <= 0x7F)
            {
                str += static_cast<wchar_t>(code);
            }
            else if (code >= 0x80 && code <= 0x7FF)
            {
                str += 0xC0 + static_cast<wchar_t>(code >> 6);
                str += 0x80 + static_cast<wchar_t>(code & 0x3F);
            }
            else if (code >= 0x800 && code <= 0xFFFF)
            {
                str += 0xE0 + static_cast<wchar_t>(code >> 12);
                str += 0x80 + static_cast<wchar_t>((code >> 6) & 0x3F);
                str += 0x80 + static_cast<wchar_t>(code & 0x3F);
            }
            else if (code >= 0x10000 && code <= 0x10FFFF)
            {
                str += 0xF0 + static_cast<wchar_t>(code >> 18);
                str += 0x80 + static_cast<wchar_t>((code >> 12) & 0x3F);
                str += 0x80 + static_cast<wchar_t>((code >> 6) & 0x3F);
                str += 0x80 + static_cast<wchar_t>(code & 0x3F);
            }
        }
        else if constexpr (sizeof(wchar_t) == sizeof(char16_t))
        {
            if (code <= 0xFFFF)
            {
                str += static_cast<wchar_t>(code);
            }
            else
            {
                const auto _code = code - 0x01000;
                str += static_cast<wchar_t>(0xD800 + ((_code >> 10) & 0x3FF));
                str += static_cast<wchar_t>(0xDC00 + (_code & 0x3FF));
            }
        }
        else
        {
            str += static_cast<wchar_t>(code);
        }
    }

    template<typename TO, typename FROM>
    std::basic_string<TO> strconv(const std::basic_string<FROM> &str)
    {
        std::basic_string<TO> result;

        for (auto it = string_conv_iterator<FROM>(str); *it != 0; ++it)
            append_code(result, *it);

        return result;
    }

    template<typename FROM>
    inline std::string strconv_ascii(const std::basic_string<FROM> &str)
    {
        return strconv<char>(str);
    }

    template<typename FROM>
    inline std::wstring strconv_wide(const std::basic_string<FROM> &str)
    {
        return strconv<wchar_t>(str);
    }

    template<typename FROM>
    inline u8string strconv_u8(const std::basic_string<FROM> &str)
    {
        return strconv<char8_t>(str);
    }

    template<typename FROM>
    inline std::u16string strconv_u16(const std::basic_string<FROM> &str)
    {
        return strconv<char16_t>(str);
    }

    template<typename FROM>
    inline std::u32string strconv_u32(const std::basic_string<FROM> &str)
    {
        return strconv<char32_t>(str);
    }
}

#endif // LAK_STRCONV_HPP
