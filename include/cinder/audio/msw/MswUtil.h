/*
 Copyright (c) 2014, The Cinder Project

 This code is intended to be used with the Cinder C++ library, http://libcinder.org

 Redistribution and use in source and binary forms, with or without modification, are permitted provided that
 the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and
	the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and
	the following disclaimer in the documentation and/or other materials provided with the distribution.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
 WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 POSSIBILITY OF SUCH DAMAGE.
*/

#pragma once

#include "cinder/Stream.h"

#include <memory>
#include <Objidl.h>

struct tWAVEFORMATEX;
typedef struct tWAVEFORMATEX WAVEFORMATEX;

namespace cinder { namespace audio { namespace msw {

//struct ComDeleter {
//	template <typename T>
//	void operator()( T* ptr )	{ ptr->Release(); }
//};

//! Creates a unique_ptr whose deleter will properly decrement the reference count of a COM object
//template<typename T>
//inline std::unique_ptr<T, ComDeleter> makeComUnique( T *p )	{ return std::unique_ptr<T, ComDeleter>( p ); }

//! return pointer type is actually a WAVEFORMATEXTENSIBLE, identifiable by the wFormat tag
std::shared_ptr<::WAVEFORMATEX> interleavedFloatWaveFormat( size_t sampleRate, size_t numChannels );

} } } // namespace cinder::audio::msw