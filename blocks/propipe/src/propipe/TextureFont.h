#pragma once

#include "cinder/gl/TextureFont.h"
#include "cinder/gl/GlslProg.h"
#include "cinder/Exception.h"

namespace cinder { namespace pp {

typedef std::shared_ptr<class Renderer> RendererRef;
typedef std::shared_ptr<class TextureFont> TextureFontRef;
typedef std::shared_ptr<class TextureFontRenderer> TextureFontRendererRef;

class TextureFont : public gl::TextureFont
{
  protected:
	TextureFont( const Font &font, const std::string &supportedChars, const Format &format );
	TextureFont( const Font &font, const std::string &supportedChars, Atlas &atlas );

  public:
	//! Draws the glyphs in \a glyphMeasures at baseline \a baseline with DrawOptions \a options. \a glyphMeasures is a vector of pairs of glyph indices and offsets for the glyph baselines
	void drawGlyphs( TextureFontRenderer& renderer, const std::vector<std::pair<uint16_t,Vec2f> > &glyphMeasures, const Vec2f &baseline, const DrawOptions &options = DrawOptions(), const std::vector<ColorA8u> &colors = std::vector<ColorA8u>() );
	//! Draws the glyphs in \a glyphMeasures clipped by \a clip, with \a offset added to each of the glyph offsets with DrawOptions \a options. \a glyphMeasures is a vector of pairs of glyph indices and offsets for the glyph baselines.
	void drawGlyphs( TextureFontRenderer& renderer, const std::vector<std::pair<uint16_t,Vec2f> > &glyphMeasures, const Rectf &clip, Vec2f offset, const DrawOptions &options = DrawOptions(), const std::vector<ColorA8u> &colors = std::vector<ColorA8u>() );

	static TextureFontRef create( const Font &font, Atlas &atlas, const std::string &supportedChars = TextureFont::defaultChars() );
	//! Creates a new TextureFontRef with font \a font, ensuring that glyphs necessary to render \a supportedChars are renderable, and format \a format
	static TextureFontRef create( const Font &font, const Format &format = Format(), const std::string &supportedChars = TextureFont::defaultChars() );

	static TextureFontRendererRef createRenderer();
};

typedef std::shared_ptr<class TextureFontDraw> TextureFontDrawRef;
class TextureFontDraw
{
public:
    static TextureFontDrawRef create(RendererRef renderer);

    void bind();
    void unbind();

	//! Draws string \a str at baseline \a baseline with DrawOptions \a options
	void drawString( TextureFont& texFont, const std::string &str, const Vec2f &baseline, const TextureFont::DrawOptions &options = TextureFont::DrawOptions() );
	//! Draws string \a str fit inside \a fitRect, with internal offset \a offset and DrawOptions \a options
	void drawString( TextureFont& texFont, const std::string &str, const Rectf &fitRect, const Vec2f &offset = Vec2f::zero(), const TextureFont::DrawOptions &options = TextureFont::DrawOptions() );

#if defined( CINDER_COCOA ) || defined ( CINDER_ANDROID )
	//! Draws word-wrapped string \a str fit inside \a fitRect, with internal offset \a offset and DrawOptions \a options. Mac & iOS only.
	virtual void	drawStringWrapped( TextureFont& texFont, const std::string &str, const Rectf &fitRect, const Vec2f &offset = Vec2f::zero(), const TextureFont::DrawOptions &options = TextureFont::DrawOptions() );
#endif

protected:
    RendererRef mRenderer;
};

/** Base class used by the internal TextureFont renderer returned by 
 *  TextureFont::createRenderer.  This interface can be implemented by
 *  an alternative renderer.
 */
class TextureFontRenderer
{
public:
	virtual ~TextureFontRenderer() { }

	//! Set transform matrix
	virtual void setMVP(const Matrix44f& mvp)  = 0;

	//! Set color used to draw glyphs if a per-glyph color array is not supplied
	virtual void setColor(const ColorA& color) = 0;

	// Set vertex attribute data arrays
	virtual void setPositionArray(float* pos) = 0;
	virtual void setTexCoordArray(float* texCoord) = 0;
	virtual void setColorArray(ColorA8u* colors) = 0;

	//! Bind the shader
	virtual void bind()   = 0;
	//! Disables client state and unbinds the shader
	virtual void unbind() = 0;

	//! Enable client state, update uniforms and sets vertex data before a draw call
	virtual void enableClientState()  = 0;
	//! Disables client state, called after drawing
	virtual void disableClientState() = 0;

	//! Draws string \a str at baseline \a baseline with DrawOptions \a options
	virtual void drawString( TextureFont& texFont, const std::string &str, const Vec2f &baseline, const TextureFont::DrawOptions &options = TextureFont::DrawOptions() );
	//! Draws string \a str fit inside \a fitRect, with internal offset \a offset and DrawOptions \a options
	virtual void drawString( TextureFont& texFont, const std::string &str, const Rectf &fitRect, const Vec2f &offset = Vec2f::zero(), const TextureFont::DrawOptions &options = TextureFont::DrawOptions() );

#if defined( CINDER_COCOA ) || defined ( CINDER_ANDROID )
	//! Draws word-wrapped string \a str fit inside \a fitRect, with internal offset \a offset and DrawOptions \a options. Mac & iOS only.
	virtual void	drawStringWrapped( TextureFont& texFont, const std::string &str, const Rectf &fitRect, const Vec2f &offset = Vec2f::zero(), const TextureFont::DrawOptions &options = TextureFont::DrawOptions() );
#endif
};

typedef std::shared_ptr<TextureFontRenderer> TextureFontRendererRef;

class RendererException : public Exception {
};

} } // namespace cinder::pp

