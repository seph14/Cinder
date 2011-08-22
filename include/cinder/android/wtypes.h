#pragma once

#include <string>
#include <cctype>

//  Workarounds for wstring support on Android NDK built against GNU STL
//  Requires Android 2.3+ to work, currently untested
//
//  See <NDK>/docs/STANDALONE-TOOLCHAIN.html for info on wchar_t support
//
namespace std {
    typedef unsigned int wint_t;
    typedef basic_string<wchar_t> wstring;
    inline int iswspace(wchar_t c) { return isspace(c); }
    inline int iswpunct(wchar_t c) { return ispunct(c); }
}

