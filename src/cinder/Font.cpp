/*
 Copyright (c) 2010, The Barbarian Group
 All rights reserved.

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

#include "cinder/Font.h"

#if defined( CINDER_COCOA )
	#include "cinder/cocoa/CinderCocoa.h"
	#if defined( CINDER_COCOA_TOUCH )
		#import <UIKit/UIKit.h>
		#import <CoreText/CoreText.h>
	#else
		#import <Cocoa/Cocoa.h>
	#endif
#elif defined( CINDER_MSW )
	#define max(a, b) (((a) > (b)) ? (a) : (b))
	#define min(a, b) (((a) < (b)) ? (a) : (b))
	#include <gdiplus.h>
	#include <windows.h>
	#undef min
	#undef max
	#include "cinder/msw/CinderMsw.h"
	#include "cinder/msw/CinderMswGdiPlus.h"
	#pragma comment(lib, "gdiplus")
#elif defined( CINDER_ANDROID )
	#include <utf8.h>
	#include <iterator>
	#include <ft2build.h>
	#include FT_FREETYPE_H
    #include FT_GLYPH_H

    struct FTData {
        ci::IStreamRef	streamRef;
        FT_StreamRec	streamRec;
        FT_Face			face;
    };
#endif
#include "cinder/Utilities.h"

using std::vector;
using std::string;
using std::wstring;
using std::pair;

namespace cinder {

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// FontManager
class FontManager
{
 public:
	static FontManager*		instance();

	const vector<string>&	getNames( bool forceRefresh );
	Font					getDefault() const
	{
		if( ! mDefault )
#if defined (CINDER_ANDROID)
			mDefault = Font(loadFile("/system/fonts/DroidSans.ttf"), 12);
#else
			mDefault = Font( "Arial", 12 );
#endif
		
		return mDefault;
	}
 private:
	FontManager();
	~FontManager();

	static FontManager	*sInstance;

	bool				mFontsEnumerated;
	vector<string>		mFontNames;
	mutable Font		mDefault;
#if defined( CINDER_MSW )
	HDC					getFontDc() const { return mFontDc; }
	Gdiplus::Graphics*	getGraphics() const { return mGraphics; }
	LONG				convertSizeToLogfontHeight( float size ) { return ::MulDiv( (long)size, -::GetDeviceCaps( mFontDc, LOGPIXELSY ), 96 ); }
#elif defined( CINDER_ANDROID )
	FT_Library			getLibrary() const { return mLibrary; }
#endif
	
#if defined( CINDER_MAC )
	NSFontManager		*nsFontManager;
#elif defined( CINDER_MSW )
	HDC					mFontDc;
	Gdiplus::Graphics	*mGraphics;
#elif defined( CINDER_ANDROID )
	FT_Library          mLibrary;
#endif

	friend class Font;
};

FontManager *FontManager::sInstance = 0;

FontManager::FontManager()
{
	mFontsEnumerated = false;
#if defined( CINDER_MAC )
	nsFontManager = [NSFontManager sharedFontManager];
	[nsFontManager retain];
#elif defined( CINDER_MSW )
	mFontDc = ::CreateCompatibleDC( NULL );
	mGraphics = new Gdiplus::Graphics( mFontDc );
#elif defined( CINDER_ANDROID )
	int error = FT_Init_FreeType( &mLibrary );
	if ( error ) {
		CI_LOGI("Error initializing FreeType");
	}
#endif
}

FontManager::~FontManager()
{
#if defined( CINDER_MAC )
	[nsFontManager release];
#elif defined( CINDER_ANDROID )
	FT_Done_FreeType( mLibrary );
#endif
}

FontManager* FontManager::instance()
{
	if( ! FontManager::sInstance )
		FontManager::sInstance = new FontManager();
	
	return sInstance;
}

#if defined( CINDER_MSW )
int CALLBACK EnumFontFamiliesExProc( ENUMLOGFONTEX *lpelfe, NEWTEXTMETRICEX *lpntme, int FontType, LPARAM lParam )
{
	reinterpret_cast<vector<string>*>( lParam )->push_back( toUtf8( lpelfe->elfFullName ) );
	return 1;
}
#endif

const vector<string>& FontManager::getNames( bool forceRefresh )
{
	if( ( ! mFontsEnumerated ) || forceRefresh ) {
		mFontNames.clear();
#if defined( CINDER_MAC )
		NSArray *fontArray = [nsFontManager availableFonts];
		NSUInteger totalFonts = [fontArray count];
		for( unsigned int i = 0; i < totalFonts; ++i ) {
			NSString *str = [fontArray objectAtIndex:i];
			mFontNames.push_back( string( [str UTF8String] ) );
		}
#elif defined( CINDER_COCOA_TOUCH )
		NSArray *familyNames = [UIFont familyNames];
		NSUInteger totalFamilies = [familyNames count];
		for( unsigned int i = 0; i < totalFamilies; ++i ) {
			NSString *familyName = [familyNames objectAtIndex:i];
			NSArray *fontNames = [UIFont fontNamesForFamilyName:familyName];
			NSUInteger totalFonts = [fontNames count];
			for( unsigned int f = 0; f < totalFonts; ++f ) {
				NSString *fontName = [fontNames objectAtIndex:f];
				mFontNames.push_back( string( [fontName UTF8String] ) );
			}
		}
#elif defined( CINDER_MSW )
		// consider enumerating character sets? DEFAULT_CHARSET potentially here
		::LOGFONT lf = { 0, 0, 0, 0, 0, 0, 0, 0, ANSI_CHARSET, 0, 0, 0, 0, '\0' };
		::EnumFontFamiliesEx( getFontDc(), &lf, (FONTENUMPROC)EnumFontFamiliesExProc, reinterpret_cast<LPARAM>( &mFontNames ), 0 );	
#endif
		mFontsEnumerated = true;
	}
	
	return mFontNames;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Font
Font::Font( const string &name, float size )
	: mObj( new Font::Obj( name, size ) )
{
}

Font::Font( DataSourceRef dataSource, float size )
	: mObj( new Font::Obj( dataSource, size ) )
{
}

const vector<string>& Font::getNames( bool forceRefresh )
{
	return FontManager::instance()->getNames( forceRefresh );
}

Font Font::getDefault()
{
	return FontManager::instance()->getDefault();
}

const std::string& Font::getName() const
{ 
	return mObj->mName;
}

float Font::getSize() const
{
	return mObj->mSize;
}

#if defined( CINDER_COCOA )
std::string Font::getFullName() const
{
	CFStringRef fullName = ::CGFontCopyFullName( mObj->mCGFont );
	string result = cocoa::convertCfString( fullName );
	CFRelease( fullName );
	return result;
}

float Font::getLeading() const
{
	return ::CGFontGetLeading( mObj->mCGFont ) / (float)::CGFontGetUnitsPerEm( mObj->mCGFont ) * mObj->mSize;
}

float Font::getAscent() const
{
	return ::CGFontGetAscent( mObj->mCGFont ) / (float)::CGFontGetUnitsPerEm( mObj->mCGFont ) * mObj->mSize;
}

float Font::getDescent() const
{
	return - ::CGFontGetDescent( mObj->mCGFont ) / (float)::CGFontGetUnitsPerEm( mObj->mCGFont ) * mObj->mSize;
}

size_t Font::getNumGlyphs() const
{
	return ::CGFontGetNumberOfGlyphs( mObj->mCGFont );
}

Font::Glyph Font::getGlyphIndex( size_t index ) const
{
	return (Glyph)index;
}

Font::Glyph Font::getGlyphChar( char c ) const
{
	UniChar uc = c;
	CGGlyph result;
	::CTFontGetGlyphsForCharacters( mObj->mCTFont, &uc, &result, 1 );
	return result;
}

vector<Font::Glyph> Font::getGlyphs( const string &s ) const
{
	vector<Font::Glyph> result;

	CFRange range = CFRangeMake( 0, 0 );	
	CFAttributedStringRef attrStr = cocoa::createCfAttributedString( s, *this, ColorA( 1, 1, 1, 1 ) );
	CTLineRef line = ::CTLineCreateWithAttributedString( attrStr );
	CFArrayRef runsArray = ::CTLineGetGlyphRuns( line );
	CFIndex numRuns = ::CFArrayGetCount( runsArray );
	for( CFIndex run = 0; run < numRuns; ++run ) {
		CTRunRef runRef = (CTRunRef)::CFArrayGetValueAtIndex( runsArray, run );
		CFIndex glyphCount = ::CTRunGetGlyphCount( runRef );
		CGGlyph glyphBuffer[glyphCount];
		::CTRunGetGlyphs( runRef, range, glyphBuffer );
		for( size_t t = 0; t < glyphCount; ++t )			
			result.push_back( glyphBuffer[t] );
	}
	
	::CFRelease( attrStr );
	::CFRelease( line );
	
	return result;
}

Shape2d Font::getGlyphShape( Glyph glyphIndex ) const
{
	CGPathRef path = CTFontCreatePathForGlyph( mObj->mCTFont, static_cast<CGGlyph>( glyphIndex ), NULL );
	Shape2d resultShape;
	cocoa::convertCgPath( path, &resultShape, true );
	CGPathRelease( path );
	return resultShape;
}

Rectf Font::getGlyphBoundingBox( Glyph glyph ) const
{
	CGGlyph glyphs[1] = { glyph };
	CGRect bounds = ::CTFontGetBoundingRectsForGlyphs( mObj->mCTFont, kCTFontDefaultOrientation, glyphs, NULL, 1 );
	return Rectf( bounds.origin.x, bounds.origin.y, bounds.origin.x + bounds.size.width, bounds.origin.y + bounds.size.height );
}

CGFontRef Font::getCgFontRef() const
{
	return mObj->mCGFont;
}

CTFontRef Font::getCtFontRef() const
{
	return mObj->mCTFont;
}

#elif defined( CINDER_MSW )

std::string Font::getFullName() const
{
	return mObj->mName;
}

float Font::getLeading() const
{
	return static_cast<float>( mObj->mTextMetric.tmInternalLeading + mObj->mTextMetric.tmExternalLeading );
}

float Font::getAscent() const
{
	return static_cast<float>( mObj->mTextMetric.tmAscent );
}

float Font::getDescent() const
{
	return static_cast<float>( mObj->mTextMetric.tmDescent );
}

size_t Font::getNumGlyphs() const
{
	return mObj->mNumGlyphs;
}

Font::Glyph Font::getGlyphChar( char c ) const
{
	WORD buffer[1];
	WCHAR theChar[1] = { (WCHAR)c };
	::SelectObject( FontManager::instance()->getFontDc(), mObj->mHfont );
	if( ::GetGlyphIndices( FontManager::instance()->getFontDc(), theChar, 1, buffer, GGI_MARK_NONEXISTING_GLYPHS ) == GDI_ERROR )
		return 0;
	
	return (Glyph)buffer[0];
}

Font::Glyph Font::getGlyphIndex( size_t idx ) const
{
	size_t ct = 0;
	bool found = false;
	for( vector<pair<uint16_t,uint16_t> >::const_iterator rangeIt = mObj->mUnicodeRanges.begin(); rangeIt != mObj->mUnicodeRanges.end(); ++rangeIt ) {
		if( ct + rangeIt->second - rangeIt->first >= idx ) {
			ct = rangeIt->first + ( idx - ct );
			found = true;
			break;
		}
		else
			ct += rangeIt->second - rangeIt->first;
	}
	
	// this idx is invalid
	if( ! found )
		ct = 0;
	
	return (Glyph)ct;
}

vector<Font::Glyph> Font::getGlyphs( const string &utf8String ) const
{
	wstring wideString = toUtf16( utf8String );
	std::shared_ptr<WORD> buffer( new WORD[wideString.length()], checked_array_deleter<WORD>() );
	::SelectObject( FontManager::instance()->getFontDc(), mObj->mHfont );
	DWORD numGlyphs = ::GetGlyphIndices( FontManager::instance()->getFontDc(), &wideString[0], (int)wideString.length(), buffer.get(), GGI_MARK_NONEXISTING_GLYPHS );
	if( numGlyphs == GDI_ERROR )
		return vector<Glyph>();
	
	vector<Glyph> result;
	for( DWORD glyph = 0; glyph < numGlyphs; ++glyph )
		result.push_back( (Glyph)buffer.get()[glyph] );
	
	return result;
}

Shape2d Font::getGlyphShape( Glyph glyphIndex ) const
{
	Shape2d resultShape;
	static const MAT2 matrix = { { 0, 1 }, { 0, 0 }, { 0, 0 }, { 0, -1 } };
	GLYPHMETRICS metrics;
	DWORD bytesGlyph = ::GetGlyphOutlineW( FontManager::instance()->getFontDc(), glyphIndex,
							GGO_NATIVE | GGO_GLYPH_INDEX, &metrics, 0, NULL, &matrix);

    if( bytesGlyph == GDI_ERROR )
		throw FontGlyphFailureExc();

	std::shared_ptr<uint8_t> buffer( new uint8_t[bytesGlyph], checked_array_deleter<uint8_t>() );
	uint8_t *ptr = buffer.get();
    if( ! buffer ) {
		throw FontGlyphFailureExc();
    }

    if( ::GetGlyphOutlineW( FontManager::instance()->getFontDc(), glyphIndex,
			  GGO_NATIVE | GGO_GLYPH_INDEX, &metrics, bytesGlyph, buffer.get(), &matrix) == GDI_ERROR ) {
		throw FontGlyphFailureExc();
    }

	// This whole block is modified from code in Cairo's cairo-win32-font.c
	resultShape.clear();
	while( ptr < buffer.get() + bytesGlyph ) {
		TTPOLYGONHEADER *header = (TTPOLYGONHEADER *)ptr;
		unsigned char *endPoly = ptr + header->cb;

		ptr += sizeof( TTPOLYGONHEADER );

		resultShape.moveTo( msw::toVec2f( header->pfxStart ) ); 
		while( ptr < endPoly ) {
			TTPOLYCURVE *curve = reinterpret_cast<TTPOLYCURVE*>( ptr );
			POINTFX *points = curve->apfx;
			switch( curve->wType ) {
				case TT_PRIM_LINE:
					for( int i = 0; i < curve->cpfx; i++ ) {
						resultShape.lineTo( msw::toVec2f( points[i] ) );
					}
				break;
				case TT_PRIM_QSPLINE:
					for( int i = 0; i < curve->cpfx - 1; i++ ) {
						Vec2f p1 = resultShape.getCurrentPoint(), p2;
						Vec2f c = msw::toVec2f( points[i] ), c1, c2;
						if( i + 1 == curve->cpfx - 1 ) {
							p2 = msw::toVec2f( points[i + 1] );
						}
						else {
							// records with more than one curve use interpolation for control points, per http://support.microsoft.com/kb/q87115/
							p2 = ( c + msw::toVec2f( points[i + 1] ) ) / 2.0f;
						}
	
						c1 = 2.0f * c / 3.0f + p1 / 3.0f;
						c2 = 2.0f * c / 3.0f + p2 / 3.0f;
						resultShape.curveTo( c1, c2, p2 );
					}
				break;
				case TT_PRIM_CSPLINE:
					for( int i = 0; i < curve->cpfx - 2; i += 2 ) {
						resultShape.curveTo( msw::toVec2f( points[i] ), msw::toVec2f( points[i + 1] ),
								msw::toVec2f( points[i + 2] ) );
					}
				break;
			}
			ptr += sizeof( TTPOLYCURVE ) + sizeof( POINTFX ) * (curve->cpfx - 1);
		}
		resultShape.close();
	}
	
	return resultShape;
}

Rectf Font::getGlyphBoundingBox( Glyph glyphIndex ) const
{
	static const MAT2 matrix = { { 0, 1 }, { 0, 0 }, { 0, 0 }, { 0, -1 } };
	GLYPHMETRICS metrics;
	::SelectObject( FontManager::instance()->getFontDc(), mObj->mHfont );
	DWORD bytesGlyph = ::GetGlyphOutlineW( FontManager::instance()->getFontDc(), glyphIndex,
							GGO_METRICS | GGO_GLYPH_INDEX, &metrics, 0, NULL, &matrix);

    if( bytesGlyph == GDI_ERROR )
		throw FontGlyphFailureExc();

	return Rectf( metrics.gmptGlyphOrigin.x, metrics.gmptGlyphOrigin.y,
			metrics.gmptGlyphOrigin.x + metrics.gmBlackBoxX, metrics.gmptGlyphOrigin.y + (int)metrics.gmBlackBoxY );
}

#elif defined( CINDER_ANDROID )

FT_Face& Font::getFTFace() const
{
	return mObj->mFTData->face;
}

Font::GlyphMetrics& Font::getGlyphMetrics(Glyph glyph) const
{
	boost::unordered_map<Font::Glyph, GlyphMetrics>::iterator it = mObj->mGlyphMetrics.find(glyph);
	if (it != mObj->mGlyphMetrics.end()) {
		return it->second;
	}

	FT_Face face = getFTFace();
	int error = FT_Load_Glyph(face, glyph, FT_LOAD_DEFAULT | FT_LOAD_FORCE_AUTOHINT);
	FT_GlyphSlot slot = face->glyph;

	GlyphMetrics gm;
	gm.mAdvance = Vec2f(slot->advance.x / 64.0f, slot->advance.y / 64.0f);
	mObj->mGlyphMetrics[glyph] = gm;
	return mObj->mGlyphMetrics[glyph];
}

Vec2f& Font::getAdvance(Glyph glyph) const
{
	GlyphMetrics& gm = getGlyphMetrics(glyph);
	return gm.mAdvance;
}

float Font::getKerning(Glyph glyph, Glyph prev) const
{
	//  Cache kerning info?
	if (mObj->mHasKerning) {
		FT_Vector kerning;
		FT_Get_Kerning(getFTFace(), prev, glyph, FT_KERNING_UNSCALED, &kerning);
		return kerning.x;
	}

	return 0;
}

// XXX implement these!
std::string Font::getFullName() const
{
	return mObj->mName;
}

float Font::getLeading() const
{
	FT_Face face = getFTFace();
	// return float(face->ascender - face->descender - face->units_per_EM) / face->units_per_EM * mObj->mSize;
	return face->size->metrics.height / 64.0f;
}

float Font::getAscent() const
{
	FT_Face face = getFTFace();
	// return float(face->ascender) / face->units_per_EM * mObj->mSize;
	return face->size->metrics.ascender / 64.0f;
}

float Font::getDescent() const
{
	FT_Face face = getFTFace();
	return  (-face->size->metrics.descender) / 64.0f;
	// return float(face->descender) / face->units_per_EM * mObj->mSize;
}

size_t Font::getNumGlyphs() const
{
	return mObj->mNumGlyphs;
}

Font::Glyph Font::getGlyphChar( char c ) const
{
	return (Glyph) c;
}

Font::Glyph Font::getGlyphIndex( size_t idx ) const
{
    //  inefficient utf32 -> utf8, utf8 -> utf16
    unsigned int utf32[] = { idx, 0 };
    vector<unsigned char> utf8result;
    utf8::utf32to8(utf32, utf32 + 1, std::back_inserter(utf8result));

    vector<Glyph> utf16result;
    utf8::utf8to16(utf8result.begin(), utf8result.end(), std::back_inserter(utf16result));
	return utf16result.front();
}

vector<Font::Glyph> Font::getGlyphs( const string &utf8String ) const
{
	vector<int> utf32String;
	vector<Glyph> result;
	utf8::utf8to32(utf8String.begin(), utf8String.end(), std::back_inserter(utf32String));
	for (vector<int>::iterator it = utf32String.begin(); it != utf32String.end(); ++it) {
		result.push_back(FT_Get_Char_Index(getFTFace(), *it));
	}
    // CI_LOGI("XXX getGlyphs utf8 size %d utf32 size %d utf16 size %d", utf8String.size(), utf32String.size(), result.size());
	return result;
}

Shape2d Font::getGlyphShape( Glyph glyphIndex ) const
{
	Shape2d resultShape;
	return resultShape;
}

Rectf Font::getGlyphBoundingBox( Glyph glyphIndex ) const
{
    FT_Glyph glyph;
    FT_BBox bbox;

    FT_Face face = getFTFace();
    int error = FT_Load_Glyph(face, glyphIndex, FT_LOAD_DEFAULT);
    error = FT_Get_Glyph(face->glyph, &glyph);
    //  get bounding box size in grid-fitted pixels
    FT_Glyph_Get_CBox(glyph, FT_GLYPH_BBOX_PIXELS, &bbox);
	return Rectf(bbox.xMin, bbox.yMin, bbox.xMax, bbox.yMax);
}

#endif

Font::Obj::Obj( const string &aName, float aSize )
	: mName( aName ), mSize( aSize )
#if defined( CINDER_MSW )
	, mHfont( 0 )
#elif defined( CINDER_ANDROID )
	, mFTData(new FTData)
#endif
{
#if defined( CINDER_COCOA )
	CFStringRef cfName = cocoa::createCfString( mName.c_str() );
	mCGFont = ::CGFontCreateWithFontName( cfName );
	CFRelease( cfName );
	if( mCGFont == 0 )
		throw FontInvalidNameExc( aName );
	mCTFont = ::CTFontCreateWithGraphicsFont( mCGFont, (CGFloat)mSize, 0, 0 );
	
	::CFStringRef fullName = ::CGFontCopyFullName( mCGFont );
	string result = cocoa::convertCfString( fullName );
	::CFRelease( fullName );
#elif defined( CINDER_MSW )
	FontManager::instance(); // force GDI+ init
	assert( sizeof(wchar_t) == 2 );
    wstring faceName = toUtf16( mName );
    
	mHfont = ::CreateFont( -mSize * 72 / 96, 0, 0, 0, FW_DONTCARE, false, false, false,
						DEFAULT_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS,
						ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE,
						faceName.c_str() );
	::SelectObject( FontManager::instance()->getFontDc(), mHfont );
//    mGdiplusFont = std::shared_ptr<Gdiplus::Font>( new Gdiplus::Font( faceName.c_str(), mSize * 72 / 96 /* Mac<->PC size conversion factor */ ) );
	mGdiplusFont = std::shared_ptr<Gdiplus::Font>( new Gdiplus::Font( FontManager::instance()->getFontDc(), mHfont ) );
	
	finishSetup();
#elif defined( CINDER_ANDROID )
	// XXX creating font from name not implemented
	FontManager::instance();  // init FreeType
#endif
}

#if defined( CINDER_ANDROID )
//  FreeType stream reading callbacks
unsigned long _ReadStream(FT_Stream ftstream, 
						  unsigned long offset, 
						  unsigned char* buffer, 
						  unsigned long count)
{
	// CI_LOGI("Read stream called on %p", ftstream);
	IStream* stream = static_cast<IStream*>(ftstream->descriptor.pointer);
	stream->seekAbsolute(offset);
	if (count) {
		return static_cast<unsigned long>(stream->readDataAvailable(buffer, count));
	}

	return 0;
}

void _CloseStream(FT_Stream ftstream)
{
	//  closed automatically? check for debug
	CI_LOGI("XXX close stream called on %p", ftstream);
}
#endif

Font::Obj::Obj( DataSourceRef dataSource, float size )
	: mSize( size )
#if defined( CINDER_MSW )
	, mHfont( 0 )
#elif defined( CINDER_ANDROID )
	, mFTData(new FTData)
#endif
{
#if defined( CINDER_COCOA )
	Buffer buffer( dataSource->getBuffer() );
	std::shared_ptr<CGDataProvider> dataProvider( ::CGDataProviderCreateWithData( NULL, buffer.getData(), buffer.getDataSize(), NULL ), ::CGDataProviderRelease );
	if( ! dataProvider )
		throw FontInvalidNameExc();
	mCGFont = ::CGFontCreateWithDataProvider( dataProvider.get() );
	if( ! mCGFont )
		throw FontInvalidNameExc();
	mCTFont = ::CTFontCreateWithGraphicsFont( mCGFont, (CGFloat)mSize, 0, 0 );

#elif defined( CINDER_MSW )
	FontManager::instance(); // force GDI+ init
	INT found = 0, count = 0;
	WCHAR familyName[1024];
	Gdiplus::PrivateFontCollection privateFontCollection;

	ci::Buffer buffer = dataSource->getBuffer();
	privateFontCollection.AddMemoryFont( buffer.getData(), buffer.getDataSize() );

	// How many font families are in the private collection?
	count = privateFontCollection.GetFamilyCount();
	if( count <= 0 )
		throw FontInvalidNameExc();

	// this is admittedly troublesome, but a new/delete combo blows up. This cannot be good.
	// And the sample code implies I should even be able to allocate FontFamily's on the stack, but that is not the case it seems
	std::shared_ptr<void> fontFamily( malloc(sizeof(Gdiplus::FontFamily)), free );
	// we only know how to use the first font family here
	privateFontCollection.GetFamilies( 1, (Gdiplus::FontFamily*)fontFamily.get(), &found );

	if( found != 0 ) {
		((Gdiplus::FontFamily*)fontFamily.get())->GetFamilyName( familyName );

		mName = toUtf8( familyName );
		mGdiplusFont = std::shared_ptr<Gdiplus::Font>( new Gdiplus::Font( ((Gdiplus::FontFamily*)fontFamily.get()), size * 72 / 96 /* Mac<->PC size conversion factor */ ) );
	}
	else
		throw FontInvalidNameExc();
	
	// now that we know the name thanks to GDI+, let's load the HFONT
	// this is only because we can't seem to get the LOGFONT -> HFONT to work down in finishSetup
	DWORD numFonts = 0;
	::AddFontMemResourceEx( buffer.getData(), buffer.getDataSize(), 0, &numFonts );
	if( numFonts < 1 )
		throw FontInvalidNameExc();

	finishSetup();
#elif defined( CINDER_ANDROID )
	FontManager* mgr = FontManager::instance();  // init FreeType

	mFTData->streamRef = dataSource->createStream();
	IStreamRef& streamRef = mFTData->streamRef;
	if (!streamRef) {
		CI_LOGI("Error: invalid stream passed to font");
		return;
	}

    FT_Matrix matrix = { (int)((1.0 /*/hres*/) * 0x10000L),
                         (int)((0.0)      * 0x10000L),
                         (int)((0.0)      * 0x10000L),
                         (int)((1.0)      * 0x10000L) };

	FT_StreamRec& streamRec = mFTData->streamRec;
	streamRec.base               = NULL;
	streamRec.size               = streamRef->size();
	streamRec.pos                = 0;
	streamRec.descriptor.pointer = streamRef.get();
	streamRec.read				  = _ReadStream;
	streamRec.close        	  = _CloseStream;

	FT_Open_Args args;
	memset(&args, 0, sizeof(FT_Open_Args));
	args.flags  |= FT_OPEN_STREAM;
	args.stream  = &(mFTData->streamRec);

	int error = FT_Open_Face( mgr->getLibrary(),
							  &args,
							  0,
							  &(mFTData->face) );

	if (error == FT_Err_Unknown_File_Format) {
		CI_LOGI("Error opening font: unknown format");
	}
	else if (error) {
		CI_LOGI("Error opening font: unhandled");
	}
	else {
		FT_Face& face = mFTData->face;
		mName = string(face->family_name);
		mNumGlyphs = face->num_glyphs;
		CI_LOGI("Opened font: family name %s", mName.c_str());
		const int dpi = 72;  //  XXX query device capabilities
        error = FT_Select_Charmap( face, FT_ENCODING_UNICODE );
        //  XXX error handling
		error = FT_Set_Char_Size( face, size * 64, 0, dpi, dpi );
        FT_Set_Transform( face, &matrix, NULL );
		mHasKerning = bool( face->face_flags & FT_FACE_FLAG_KERNING );
	}
#endif
}

Font::Obj::~Obj()
{
#if defined( CINDER_COCOA )
	::CGFontRelease( mCGFont );
	::CFRelease( mCTFont );
#elif defined( CINDER_MSW )
	if( mHfont ) // this should be replaced with something exception-safe
		::DeleteObject( mHfont ); 
#elif defined( CINDER_ANDROID )
	FT_Done_Face( mFTData->face );
#endif
}

void Font::Obj::finishSetup()
{
#if defined( CINDER_MSW )
	mGdiplusFont->GetLogFontW( FontManager::instance()->getGraphics(), &mLogFont );

	if( ! mHfont )
		mHfont = ::CreateFontIndirectW( &mLogFont );
	if( ! mHfont )
		throw FontInvalidNameExc();
	
	::SelectObject( FontManager::instance()->getFontDc(), mHfont );

	if( ! ::GetTextMetrics( FontManager::instance()->getFontDc(), &mTextMetric ) )
		throw FontInvalidNameExc();

	// build the unicode ranges vector
	DWORD glyphSize = ::GetFontUnicodeRanges( FontManager::instance()->getFontDc(), NULL );
	uint8_t *buffer = new uint8_t[glyphSize];
	GLYPHSET *gs = (GLYPHSET*)buffer;
	::GetFontUnicodeRanges( FontManager::instance()->getFontDc(), gs );
	mNumGlyphs = gs->cGlyphsSupported;
	for( DWORD r = 0; r < gs->cRanges; ++r )
		mUnicodeRanges.push_back( std::make_pair( gs->ranges[r].wcLow, gs->ranges[r].wcLow + gs->ranges[r].cGlyphs ) );
	delete [] buffer;
#endif
}

#if defined( CINDER_MSW )
HDC Font::getGlobalDc()
{
	return FontManager::instance()->getFontDc();
}
#endif

FontInvalidNameExc::FontInvalidNameExc( const std::string &fontName ) throw()
{
	sprintf( mMessage, "%s", fontName.c_str() );
}

} // namespace cinder
