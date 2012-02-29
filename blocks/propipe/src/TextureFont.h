#pragma once

#include "cinder/gl/TextureFont.h"

namespace cinder { namespace pp {

class TextureFont : public gl::TextureFont
{
  public:
	struct Renderer
	{
		Renderer();
		void reset();
	};

  protected:
	TextureFont( const Font &font, Atlas &atlas, const std::string &supportedChars = TextureFont::defaultChars() );
	TextureFont( const Font &font, const Format &format = Format(), const std::string &supportedChars = TextureFont::defaultChars() );

	//! Draws the glyphs in \a glyphMeasures at baseline \a baseline with DrawOptions \a options. \a glyphMeasures is a vector of pairs of glyph indices and offsets for the glyph baselines
	virtual void drawGlyphs( const std::vector<std::pair<uint16_t,Vec2f> > &glyphMeasures, const Vec2f &baseline, const DrawOptions &options = DrawOptions(), const std::vector<ColorA8u> &colors = std::vector<ColorA8u>() );
	//! Draws the glyphs in \a glyphMeasures clipped by \a clip, with \a offset added to each of the glyph offsets with DrawOptions \a options. \a glyphMeasures is a vector of pairs of glyph indices and offsets for the glyph baselines.
	virtual void drawGlyphs( const std::vector<std::pair<uint16_t,Vec2f> > &glyphMeasures, const Rectf &clip, Vec2f offset, const DrawOptions &options = DrawOptions(), const std::vector<ColorA8u> &colors = std::vector<ColorA8u>() );

};

} } // namespace cinder::pp

