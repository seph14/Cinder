#pragma once

#include <string>
#include <cctype>
#include <wchar.h>

//  Workarounds for wstring support on Android NDK built against GNU STL
//  Requires Android 2.3+ to work, currently untested
//
//  See <NDK>/docs/STANDALONE-TOOLCHAIN.html for info on wchar_t support
//
namespace std {
    typedef int wint_t;
    typedef basic_string<wchar_t> wstring;
    inline int iswspace(wint_t c) { return ::iswspace(c); }
    inline int iswpunct(wint_t c) { return ::iswpunct(c); }
    inline size_t wcslen(const wchar_t *c) { return ::wcslen(c); }
}

