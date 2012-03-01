#include "TextureFont.h"

using namespace std;

namespace cinder { namespace pp {

class TextureFont::Renderer
{
public:
	static RendererRef create()
	{
		return RendererRef(new Renderer());
	}

	Renderer()
	{
		mShader  = gl::GlslProg(vert, frag);

		position = mShader.getAttribLocation("aPosition");
		color    = mShader.getAttribLocation("aColor");
		texCoord = mShader.getAttribLocation("aTexCoord");

		useColor(false);
		mShader.uniform("sTexture", 0);
	}

	void init(const Matrix44f& mvp)
	{
		mShader.uniform("uMVP", mvp);
	}

	void useColor(bool active)
	{
		mUseColor = active;
		mShader.uniform("uUseColor", active);
	}

	bool useColor() const
	{
		return mUseColor;
	}

	void bind()
	{
		glEnableVertexAttribArray(position);
		if ( mUseColor )
			glEnableVertexAttribArray(color);
		glEnableVertexAttribArray(texCoord);
		mShader.bind();
	}

	void unbind()
	{
		mShader.unbind();
		glDisableVertexAttribArray(position);
		glDisableVertexAttribArray(color);
		glDisableVertexAttribArray(texCoord);
	}

	GLuint position;
	GLuint color;
	GLuint texCoord;

	static const char* vert;
	static const char* frag;

protected:
	gl::GlslProg mShader;
	Matrix44f mMVP;
	bool mUseColor;
};

const char* TextureFont::Renderer::vert =
        "attribute vec2 aPosition;\n"
        "attribute vec4 aColor;\n"
        "attribute vec2 aTexCoord;\n"

        "uniform mat4 uMVP;\n"
        "uniform vec4 uVertexColor;\n"

        "uniform bool uUseColor;\n"

        "varying vec4 vColor;\n"
        "varying vec2 vTexCoord;\n"

        "void main() {\n"
        "  if (uUseColor) {\n"
        "    vColor = aColor;\n"
        "  }\n"
		"  else {\n"
		"    vColor = vec4(1.0, 1.0, 1.0, 1.0);\n"
		"  }\n"
        "  vTexCoord = aTexCoord;\n"
        "  gl_Position = uMVP * vec4(aPosition, 0, 1.0);\n"
        "}\n";

const char* TextureFont::Renderer::frag =
        "precision mediump float;\n"

        "uniform sampler2D sTexture;\n"

        "varying vec4 vColor;\n"
        "varying vec2 vTexCoord;\n"

        "void main() {\n"
        "      gl_FragColor = vColor * texture2D(sTexture, vTexCoord);\n"
        "}\n";

TextureFont::RendererRef TextureFont::sRenderer;

TextureFont::TextureFont( const Font &font, const string &supportedChars, const Format &format )
	: gl::TextureFont( font, supportedChars, format )
{
}

TextureFont::TextureFont( const Font &font, const string &supportedChars, Atlas &atlas )
	: gl::TextureFont( font, supportedChars, atlas )
{
}

void TextureFont::drawGlyphs( const vector<pair<uint16_t,Vec2f> > &glyphMeasures, const Vec2f &baselineIn, const DrawOptions &options, const vector<ColorA8u> &colors )
{
	//  Requires the internal renderer to be initialized
	if (!sRenderer)
		return;

	if( mTextures.empty() )
		return;

	if( ! colors.empty() ) {
		assert( glyphMeasures.size() == colors.size() );
		sRenderer->useColor( true );
	}
	else {
		sRenderer->useColor( false );
	}

	gl::SaveTextureBindState saveBindState( mTextures[0].getTarget() );
	gl::enable( mTextures[0].getTarget() );

	Vec2f baseline = baselineIn;

	const float scale = options.getScale();
	for( size_t texIdx = 0; texIdx < mTextures.size(); ++texIdx ) {
		vector<float> verts, texCoords;
		vector<ColorA8u> vertColors;
		const gl::Texture &curTex = mTextures[texIdx];
		vector<gl::index_t> indices;
		uint16_t curIdx = 0;
		if( options.getPixelSnap() )
			baseline = Vec2f( floor( baseline.x ), floor( baseline.y ) );
			
		for( vector<pair<uint16_t,Vec2f> >::const_iterator glyphIt = glyphMeasures.begin(); glyphIt != glyphMeasures.end(); ++glyphIt ) {
			boost::unordered_map<Font::Glyph, GlyphInfo>::const_iterator glyphInfoIt = mGlyphMap.find( glyphIt->first );
			if( (glyphInfoIt == mGlyphMap.end()) || (mGlyphMap[glyphIt->first].mTextureIndex != texIdx) )
				continue;
				
			const GlyphInfo &glyphInfo = glyphInfoIt->second;
			
			Rectf destRect( glyphInfo.mTexCoords );
			Rectf srcCoords = curTex.getAreaTexCoords( glyphInfo.mTexCoords );
			destRect -= destRect.getUpperLeft();
			destRect.scale( scale );
			destRect += glyphIt->second * scale;
			destRect += Vec2f( floor( glyphInfo.mOriginOffset.x + 0.5f ), floor( glyphInfo.mOriginOffset.y ) ) * scale;
			destRect += Vec2f( baseline.x, baseline.y - mFont.getAscent() * scale );
			if( options.getPixelSnap() )
				destRect -= Vec2f( destRect.x1 - floor( destRect.x1 ), destRect.y1 - floor( destRect.y1 ) );				
			
			verts.push_back( destRect.getX2() ); verts.push_back( destRect.getY1() );
			verts.push_back( destRect.getX1() ); verts.push_back( destRect.getY1() );
			verts.push_back( destRect.getX2() ); verts.push_back( destRect.getY2() );
			verts.push_back( destRect.getX1() ); verts.push_back( destRect.getY2() );

			texCoords.push_back( srcCoords.getX2() ); texCoords.push_back( srcCoords.getY1() );
			texCoords.push_back( srcCoords.getX1() ); texCoords.push_back( srcCoords.getY1() );
			texCoords.push_back( srcCoords.getX2() ); texCoords.push_back( srcCoords.getY2() );
			texCoords.push_back( srcCoords.getX1() ); texCoords.push_back( srcCoords.getY2() );
			
			if( sRenderer->useColor() ) {
				for( int i = 0; i < 4; ++i )
					vertColors.push_back( colors[glyphIt-glyphMeasures.begin()] );
			}

			indices.push_back( curIdx + 0 ); indices.push_back( curIdx + 1 ); indices.push_back( curIdx + 2 );
			indices.push_back( curIdx + 2 ); indices.push_back( curIdx + 1 ); indices.push_back( curIdx + 3 );
			curIdx += 4;
		}
		
		if( curIdx == 0 )
			continue;
		
		sRenderer->bind();
		curTex.bind();

		glVertexAttribPointer( sRenderer->position, 2, GL_FLOAT, GL_FALSE, 0, &verts[0] );
		glVertexAttribPointer( sRenderer->texCoord, 2, GL_FLOAT, GL_FALSE, 0, &texCoords[0] );
		if( sRenderer->useColor() ) {
			glVertexAttribPointer( sRenderer->color, 4, GL_UNSIGNED_BYTE, GL_FALSE, 0, &vertColors[0] );
		}

		glDrawElements( GL_TRIANGLES, indices.size(), CINDER_GL_INDEX_TYPE, &indices[0] );
		sRenderer->unbind();
	}
}

void TextureFont::drawGlyphs( const vector<pair<uint16_t,Vec2f> > &glyphMeasures, const Rectf &clip, Vec2f offset, const DrawOptions &options, const vector<ColorA8u> &colors )
{
	//  Requires the internal renderer to be initialized
	if (!sRenderer)
		return;

	if( mTextures.empty() )
		return;

	if( ! colors.empty() ) {
		assert( glyphMeasures.size() == colors.size() );
		sRenderer->useColor( true );
	}
	else {
		sRenderer->useColor( false );
	}

	gl::SaveTextureBindState saveBindState( mTextures[0].getTarget() );
	gl::enable( mTextures[0].getTarget() );
	const float scale = options.getScale();

	for ( size_t texIdx = 0; texIdx < mTextures.size(); ++texIdx ) {
		vector<float> verts, texCoords;
		vector<ColorA8u> vertColors;
		const gl::Texture &curTex = mTextures[texIdx];
		vector<gl::index_t> indices;
		uint16_t curIdx = 0;
		if( options.getPixelSnap() )
			offset = Vec2f( floor( offset.x ), floor( offset.y ) );

		for( vector<pair<uint16_t,Vec2f> >::const_iterator glyphIt = glyphMeasures.begin(); glyphIt != glyphMeasures.end(); ++glyphIt ) {
			boost::unordered_map<Font::Glyph, GlyphInfo>::const_iterator glyphInfoIt = mGlyphMap.find( glyphIt->first );
			if( (glyphInfoIt == mGlyphMap.end()) || (mGlyphMap[glyphIt->first].mTextureIndex != texIdx) )
				continue;
				
			const GlyphInfo &glyphInfo = glyphInfoIt->second;
			Rectf srcTexCoords = curTex.getAreaTexCoords( glyphInfo.mTexCoords );
			Rectf destRect( glyphInfo.mTexCoords );
			destRect -= destRect.getUpperLeft();
			destRect.scale( scale );
			destRect += glyphIt->second * scale;
			destRect += Vec2f( floor( glyphInfo.mOriginOffset.x + 0.5f ), floor( glyphInfo.mOriginOffset.y ) ) * scale;
			destRect += Vec2f( offset.x, offset.y );
			if( options.getPixelSnap() )
				destRect -= Vec2f( destRect.x1 - floor( destRect.x1 ), destRect.y1 - floor( destRect.y1 ) );				

			// clip
			Rectf clipped( destRect );
			if( options.getClipHorizontal() ) {
				clipped.x1 = max( destRect.x1, clip.x1 );
				clipped.x2 = min( destRect.x2, clip.x2 );
			}
			if( options.getClipVertical() ) {
				clipped.y1 = max( destRect.y1, clip.y1 );
				clipped.y2 = min( destRect.y2, clip.y2 );
			}
			
			if( clipped.x1 >= clipped.x2 || clipped.y1 >= clipped.y2 )
				continue;
			
			Vec2f coordScale( 1 / (float)destRect.getWidth() / curTex.getWidth() * glyphInfo.mTexCoords.getWidth(),
				1 / (float)destRect.getHeight() / curTex.getHeight() * glyphInfo.mTexCoords.getHeight() );
			srcTexCoords.x1 = srcTexCoords.x1 + ( clipped.x1 - destRect.x1 ) * coordScale.x;
			srcTexCoords.x2 = srcTexCoords.x1 + ( clipped.x2 - clipped.x1 ) * coordScale.x;
			srcTexCoords.y1 = srcTexCoords.y1 + ( clipped.y1 - destRect.y1 ) * coordScale.y;
			srcTexCoords.y2 = srcTexCoords.y1 + ( clipped.y2 - clipped.y1 ) * coordScale.y;

			verts.push_back( clipped.getX2() ); verts.push_back( clipped.getY1() );
			verts.push_back( clipped.getX1() ); verts.push_back( clipped.getY1() );
			verts.push_back( clipped.getX2() ); verts.push_back( clipped.getY2() );
			verts.push_back( clipped.getX1() ); verts.push_back( clipped.getY2() );

			texCoords.push_back( srcTexCoords.getX2() ); texCoords.push_back( srcTexCoords.getY1() );
			texCoords.push_back( srcTexCoords.getX1() ); texCoords.push_back( srcTexCoords.getY1() );
			texCoords.push_back( srcTexCoords.getX2() ); texCoords.push_back( srcTexCoords.getY2() );
			texCoords.push_back( srcTexCoords.getX1() ); texCoords.push_back( srcTexCoords.getY2() );

			if( sRenderer->useColor() ) {
				for( int i = 0; i < 4; ++i )
					vertColors.push_back( colors[glyphIt-glyphMeasures.begin()] );
			}
			
			indices.push_back( curIdx + 0 ); indices.push_back( curIdx + 1 ); indices.push_back( curIdx + 2 );
			indices.push_back( curIdx + 2 ); indices.push_back( curIdx + 1 ); indices.push_back( curIdx + 3 );
			curIdx += 4;
		}
		
		if( curIdx == 0 )
			continue;
		
		sRenderer->bind();
		curTex.bind();

		glVertexAttribPointer( sRenderer->position, 2, GL_FLOAT, GL_FALSE, 0, &verts[0] );
		glVertexAttribPointer( sRenderer->texCoord, 2, GL_FLOAT, GL_FALSE, 0, &texCoords[0] );
		if( sRenderer->useColor() )
			glVertexAttribPointer( sRenderer->color, 4, GL_UNSIGNED_BYTE, GL_FALSE, 0, &vertColors[0] );

		glDrawElements( GL_TRIANGLES, indices.size(), CINDER_GL_INDEX_TYPE, &indices[0] );
		sRenderer->unbind();
	}
}

void TextureFont::rendererInit(const Matrix44f& mvp)
{
	if (!sRenderer) {
		sRenderer = TextureFont::Renderer::create();
	}

	sRenderer->init(mvp);
}

void TextureFont::rendererRelease()
{
	sRenderer = RendererRef();
}

TextureFontRef TextureFont::create( const Font &font, Atlas &atlas, const std::string &supportedChars )
{
	return TextureFontRef( new TextureFont( font, supportedChars, atlas ) ); 
}

TextureFontRef TextureFont::create( const Font &font, const Format &format, const std::string &supportedChars )
{
	return TextureFontRef( new TextureFont( font, supportedChars, format ) ); 
}

} }  // namespace cinder::pp

