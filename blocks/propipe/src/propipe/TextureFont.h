#pragma once

#include "Draw.h"

#include "cinder/gl/TextureFont.h"
#include "cinder/gl/GlslProg.h"

namespace cinder { namespace pp {

typedef std::shared_ptr<class DrawShader> DrawShaderRef;
typedef std::shared_ptr<class TextureFont> TextureFontRef;
typedef std::shared_ptr<class TextureFontShader> TextureFontShaderRef;

class DrawShader;

class TextureFont : public gl::TextureFont
{
  protected:
	TextureFont( const FontRef font, const std::string &supportedChars, gl::TextureFontAtlasRef atlas );

  public:
	//! Draws the glyphs in \a glyphMeasures at baseline \a baseline with DrawOptions \a options. \a glyphMeasures is a vector of pairs of glyph indices and offsets for the glyph baselines
	void drawGlyphs( DrawShader& shader, const std::vector<std::pair<uint16_t,Vec2f> > &glyphMeasures, const Vec2f &baseline, const DrawOptions &options = DrawOptions(), const std::vector<ColorA8u> &colors = std::vector<ColorA8u>() );
	//! Draws the glyphs in \a glyphMeasures clipped by \a clip, with \a offset added to each of the glyph offsets with DrawOptions \a options. \a glyphMeasures is a vector of pairs of glyph indices and offsets for the glyph baselines.
	void drawGlyphs( DrawShader& shader, const std::vector<std::pair<uint16_t,Vec2f> > &glyphMeasures, const Rectf &clip, Vec2f offset, const DrawOptions &options = DrawOptions(), const std::vector<ColorA8u> &colors = std::vector<ColorA8u>() );

#if defined( CINDER_ANDROID )
	static TextureFontRef create( const FontRef font, gl::TextureFontAtlasRef atlas, const std::string &supportedChars = TextureFont::defaultChars() );
#endif
	//! Creates a new TextureFontRef with font \a font, ensuring that glyphs necessary to render \a supportedChars are renderable, and format \a format
	static TextureFontRef create( const FontRef font, const Format &format = Format(), const std::string &supportedChars = TextureFont::defaultChars() );
};

typedef std::shared_ptr<class TextureFontDraw> TextureFontDrawRef;
class TextureFontDraw 
{
  public:
	static TextureFontDrawRef create(DrawShaderRef shader);

	//! Draws string \a str at baseline \a baseline with DrawOptions \a options
	void drawString( TextureFont& texFont, const std::string &str, const Vec2f &baseline, const TextureFont::DrawOptions &options = TextureFont::DrawOptions() );
	//! Draws string \a str fit inside \a fitRect, with internal offset \a offset and DrawOptions \a options
	void drawString( TextureFont& texFont, const std::string &str, const Rectf &fitRect, const Vec2f &offset = Vec2f::zero(), const TextureFont::DrawOptions &options = TextureFont::DrawOptions() );

#if defined( CINDER_COCOA ) || defined ( CINDER_ANDROID )
	//! Draws word-wrapped string \a str fit inside \a fitRect, with internal offset \a offset and DrawOptions \a options. Mac & iOS only.
	void drawStringWrapped( TextureFont& texFont, const std::string &str, const Rectf &fitRect, const Vec2f &offset = Vec2f::zero(), const TextureFont::DrawOptions &options = TextureFont::DrawOptions() );
#endif

	virtual ~TextureFontDraw();

  protected:
	TextureFontDraw(DrawShaderRef shader);
   DrawShaderRef mShader;
};

} } // namespace cinder::pp

