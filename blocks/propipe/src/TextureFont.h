#pragma once

#include "cinder/gl/TextureFont.h"
#include "cinder/gl/GlslProg.h"

namespace cinder { namespace pp {

typedef std::shared_ptr<class TextureFont>	TextureFontRef;

class TextureFont : public gl::TextureFont
{
  public:
	class Renderer;
	typedef std::shared_ptr<Renderer> RendererRef;

  protected:
	TextureFont( const Font &font, const std::string &supportedChars, const Format &format );
	TextureFont( const Font &font, const std::string &supportedChars, Atlas &atlas );

	//! Draws the glyphs in \a glyphMeasures at baseline \a baseline with DrawOptions \a options. \a glyphMeasures is a vector of pairs of glyph indices and offsets for the glyph baselines
	virtual void drawGlyphs( const std::vector<std::pair<uint16_t,Vec2f> > &glyphMeasures, const Vec2f &baseline, const DrawOptions &options = DrawOptions(), const std::vector<ColorA8u> &colors = std::vector<ColorA8u>() );
	//! Draws the glyphs in \a glyphMeasures clipped by \a clip, with \a offset added to each of the glyph offsets with DrawOptions \a options. \a glyphMeasures is a vector of pairs of glyph indices and offsets for the glyph baselines.
	virtual void drawGlyphs( const std::vector<std::pair<uint16_t,Vec2f> > &glyphMeasures, const Rectf &clip, Vec2f offset, const DrawOptions &options = DrawOptions(), const std::vector<ColorA8u> &colors = std::vector<ColorA8u>() );

	static RendererRef sRenderer;

  public:
	static TextureFontRef create( const Font &font, Atlas &atlas, const std::string &supportedChars = TextureFont::defaultChars() );
	//! Creates a new TextureFontRef with font \a font, ensuring that glyphs necessary to render \a supportedChars are renderable, and format \a format
	static TextureFontRef create( const Font &font, const Format &format = Format(), const std::string &supportedChars = TextureFont::defaultChars() );

	//!  Initialize the internal TextureFont renderer/shader. Must be called to enable font drawing.
	static void rendererInit(const ci::Matrix44f& mvp);
	//!  Release the internal TextureFont renderer/shader
	static void rendererRelease();
};

} } // namespace cinder::pp

