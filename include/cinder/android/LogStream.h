#pragma once

#include <ostream>
#include <sstream>
#include <string>

#include "cinder/Cinder.h"

namespace cinder { namespace android {

template <class CharT, class TraitsT = std::char_traits<CharT> >
class basic_debugbuf : 
    public std::basic_stringbuf<CharT, TraitsT>
{
public:

    virtual ~basic_debugbuf() {
        sync();
    }

protected:
    int sync() {
        output_debug_string(this->str().c_str());
        this->str(std::basic_string<CharT>());    // Clear the string buffer

        return 0;
    }

    void output_debug_string(const CharT *text) {}
};

template<>
inline void basic_debugbuf<char>::output_debug_string(const char *text)
{
    __android_log_write(ANDROID_LOG_DEBUG, "cinder", text);
}

template<class CharT, class TraitsT = std::char_traits<CharT> >
class basic_dostream : 
    public std::basic_ostream<CharT, TraitsT>
{
public:

    basic_dostream() : std::basic_ostream<CharT, TraitsT>
                (new basic_debugbuf<CharT, TraitsT>()) {}
    ~basic_dostream() 
    {
        delete this->rdbuf(); 
    }
};

typedef basic_dostream<char>    dostream;

} } // namespace cinder::msw
