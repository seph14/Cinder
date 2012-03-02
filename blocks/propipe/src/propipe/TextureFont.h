#pragma once

#include "cinder/gl/TextureFont.h"
#include "cinder/gl/GlslProg.h"
#include "cinder/Exception.h"

namespace cinder { namespace pp {

typedef std::shared_ptr<class TextureFont> TextureFontRef;

class TextureFont : public gl::TextureFont
{
  public:
	/** Interface used by the internal TextureFont renderer.  You can supply an 
	 *  alternative renderer that implements this interface to rendererInit()
	 */
	class IRenderer
	{
	  public:
		virtual ~IRenderer() { }

        //! Set transform matrix
		virtual void setMVP(const Matrix44f& mvp)  = 0;

        //! Set color used to draw glyphs if a per-glyph color array is not supplied
		virtual void setColor(const ColorA& color) = 0;

		//! Set vertex attribute data arrays
		virtual void setPositionArray(float* pos) = 0;
		virtual void setTexCoordArray(float* texCoord) = 0;
		virtual void setColorArray(ColorA8u* colors) = 0;

        //! Enable client state, binds the shader and sets vertex data
		virtual void bind()   = 0;
        //! Disables client state and unbinds the shader
		virtual void unbind() = 0;
	};

	class Renderer;

	typedef std::shared_ptr<IRenderer> IRendererRef;

  protected:
	TextureFont( const Font &font, const std::string &supportedChars, const Format &format );
	TextureFont( const Font &font, const std::string &supportedChars, Atlas &atlas );

	static IRendererRef sRenderer;

  public:
	//! Draws the glyphs in \a glyphMeasures at baseline \a baseline with DrawOptions \a options. \a glyphMeasures is a vector of pairs of glyph indices and offsets for the glyph baselines
	virtual void drawGlyphs( const std::vector<std::pair<uint16_t,Vec2f> > &glyphMeasures, const Vec2f &baseline, const DrawOptions &options = DrawOptions(), const std::vector<ColorA8u> &colors = std::vector<ColorA8u>() );
	//! Draws the glyphs in \a glyphMeasures clipped by \a clip, with \a offset added to each of the glyph offsets with DrawOptions \a options. \a glyphMeasures is a vector of pairs of glyph indices and offsets for the glyph baselines.
	virtual void drawGlyphs( const std::vector<std::pair<uint16_t,Vec2f> > &glyphMeasures, const Rectf &clip, Vec2f offset, const DrawOptions &options = DrawOptions(), const std::vector<ColorA8u> &colors = std::vector<ColorA8u>() );

	static TextureFontRef create( const Font &font, Atlas &atlas, const std::string &supportedChars = TextureFont::defaultChars() );
	//! Creates a new TextureFontRef with font \a font, ensuring that glyphs necessary to render \a supportedChars are renderable, and format \a format
	static TextureFontRef create( const Font &font, const Format &format = Format(), const std::string &supportedChars = TextureFont::defaultChars() );

	//!  Initialize the internal TextureFont renderer/shader. Must be called to enable font drawing.
	static void rendererInit(const IRendererRef& renderer = IRendererRef());
	//!  Release the internal TextureFont renderer/shader
	static void rendererRelease();

    //!  Return a reference to the internal renderer, throws a \a RendererException if the renderer is not initialized or invalid
    static IRenderer& renderer();
};

class RendererException : public Exception {
};

} } // namespace cinder::pp

