#include "TextureFont.h"

void TextureFont::drawGlyphs( const vector<pair<uint16_t,Vec2f> > &glyphMeasures, const Vec2f &baselineIn, const DrawOptions &options, const std::vector<ColorA8u> &colors )
{
	if( mTextures.empty() )
		return;

	if( ! colors.empty() )
		assert( glyphMeasures.size() == colors.size() );

	SaveTextureBindState saveBindState( mTextures[0].getTarget() );
	BoolState saveEnabledState( mTextures[0].getTarget() );
	ClientBoolState vertexArrayState( GL_VERTEX_ARRAY );
	ClientBoolState colorArrayState( GL_COLOR_ARRAY );
	ClientBoolState texCoordArrayState( GL_TEXTURE_COORD_ARRAY );	
	gl::enable( mTextures[0].getTarget() );

	Vec2f baseline = baselineIn;

    GlesContextRef esContext = gl::getGlesContext();
    if (!esContext)  {
        CI_LOGW("TextureFont draw error: no global ES context set");
        return;
    }

    GlesAttr& attr = esContext->attr();
    glEnableVertexAttribArray(attr.mVertex);
	if( ! colors.empty() )
        glEnableVertexAttribArray(attr.mColor);
    glEnableVertexAttribArray(attr.mTexCoord);

    esContext->selectAttrs( ES2_ATTR_VERTEX | ES2_ATTR_TEXCOORD | (colors.empty() ? 0 : ES2_ATTR_COLOR) );

	const float scale = options.getScale();
	for( size_t texIdx = 0; texIdx < mTextures.size(); ++texIdx ) {
		vector<float> verts, texCoords;
		vector<ColorA8u> vertColors;
		const gl::Texture &curTex = mTextures[texIdx];
#if defined( CINDER_GLES )
		vector<uint16_t> indices;
		uint16_t curIdx = 0;
		GLenum indexType = GL_UNSIGNED_SHORT;
#else
		vector<uint32_t> indices;
		uint32_t curIdx = 0;
		GLenum indexType = GL_UNSIGNED_INT;
#endif
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
#if defined( CINDER_GLES2 )
            verts.push_back(0);
#endif
			verts.push_back( destRect.getX1() ); verts.push_back( destRect.getY1() );
#if defined( CINDER_GLES2 )
            verts.push_back(0);
#endif
			verts.push_back( destRect.getX2() ); verts.push_back( destRect.getY2() );
#if defined( CINDER_GLES2 )
            verts.push_back(0);
#endif
			verts.push_back( destRect.getX1() ); verts.push_back( destRect.getY2() );
#if defined( CINDER_GLES2 )
            verts.push_back(0);
#endif

			texCoords.push_back( srcCoords.getX2() ); texCoords.push_back( srcCoords.getY1() );
			texCoords.push_back( srcCoords.getX1() ); texCoords.push_back( srcCoords.getY1() );
			texCoords.push_back( srcCoords.getX2() ); texCoords.push_back( srcCoords.getY2() );
			texCoords.push_back( srcCoords.getX1() ); texCoords.push_back( srcCoords.getY2() );
			
			if( ! colors.empty() ) {
				for( int i = 0; i < 4; ++i )
					vertColors.push_back( colors[glyphIt-glyphMeasures.begin()] );
			}

			indices.push_back( curIdx + 0 ); indices.push_back( curIdx + 1 ); indices.push_back( curIdx + 2 );
			indices.push_back( curIdx + 2 ); indices.push_back( curIdx + 1 ); indices.push_back( curIdx + 3 );
			curIdx += 4;
		}
		
		if( curIdx == 0 )
			continue;
		
		curTex.bind();
		glVertexAttribPointer( attr.mVertex, 3, GL_FLOAT, GL_FALSE, 0, &verts[0] );
		glVertexAttribPointer( attr.mTexCoord, 2, GL_FLOAT, GL_FALSE, 0, &texCoords[0] );
		if( ! colors.empty() )
			glVertexAttribPointer( attr.mColor, 4, GL_UNSIGNED_BYTE, GL_FALSE, 0, &vertColors[0] );
		glDrawElements( GL_TRIANGLES, indices.size(), indexType, &indices[0] );
	}
}

void TextureFont::drawGlyphs( const std::vector<std::pair<uint16_t,Vec2f> > &glyphMeasures, const Rectf &clip, Vec2f offset, const DrawOptions &options, const std::vector<ColorA8u> &colors )
{
	if( mTextures.empty() )
		return;

	if( ! colors.empty() )
		assert( glyphMeasures.size() == colors.size() );

	SaveTextureBindState saveBindState( mTextures[0].getTarget() );
	BoolState saveEnabledState( mTextures[0].getTarget() );
	ClientBoolState vertexArrayState( GL_VERTEX_ARRAY );
	ClientBoolState colorArrayState( GL_COLOR_ARRAY );
	ClientBoolState texCoordArrayState( GL_TEXTURE_COORD_ARRAY );	
	gl::enable( mTextures[0].getTarget() );
	const float scale = options.getScale();

    GlesContextRef esContext = gl::getGlesContext();
    if ( ! esContext )  {
        CI_LOGW("TextureFont draw error: no global ES context set");
        return;
    }

    GlesAttr& attr = esContext->attr();
    glEnableVertexAttribArray(attr.mVertex);
	if( ! colors.empty() )
        glEnableVertexAttribArray(attr.mColor);
    glEnableVertexAttribArray(attr.mTexCoord);

    esContext->selectAttrs( ES2_ATTR_VERTEX | ES2_ATTR_TEXCOORD | (colors.empty() ? 0 : ES2_ATTR_COLOR) );

	for( size_t texIdx = 0; texIdx < mTextures.size(); ++texIdx ) {
		vector<float> verts, texCoords;
		vector<ColorA8u> vertColors;
		const gl::Texture &curTex = mTextures[texIdx];
#if defined( CINDER_GLES )
		vector<uint16_t> indices;
		uint16_t curIdx = 0;
		GLenum indexType = GL_UNSIGNED_SHORT;
#else
		vector<uint32_t> indices;
		uint32_t curIdx = 0;
		GLenum indexType = GL_UNSIGNED_INT;
#endif
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
				clipped.x1 = std::max( destRect.x1, clip.x1 );
				clipped.x2 = std::min( destRect.x2, clip.x2 );
			}
			if( options.getClipVertical() ) {
				clipped.y1 = std::max( destRect.y1, clip.y1 );
				clipped.y2 = std::min( destRect.y2, clip.y2 );
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
#if defined( CINDER_GLES2 )
            verts.push_back(0);
#endif
			verts.push_back( clipped.getX1() ); verts.push_back( clipped.getY1() );
#if defined( CINDER_GLES2 )
            verts.push_back(0);
#endif
			verts.push_back( clipped.getX2() ); verts.push_back( clipped.getY2() );
#if defined( CINDER_GLES2 )
            verts.push_back(0);
#endif
			verts.push_back( clipped.getX1() ); verts.push_back( clipped.getY2() );
#if defined( CINDER_GLES2 )
            verts.push_back(0);
#endif

			texCoords.push_back( srcTexCoords.getX2() ); texCoords.push_back( srcTexCoords.getY1() );
			texCoords.push_back( srcTexCoords.getX1() ); texCoords.push_back( srcTexCoords.getY1() );
			texCoords.push_back( srcTexCoords.getX2() ); texCoords.push_back( srcTexCoords.getY2() );
			texCoords.push_back( srcTexCoords.getX1() ); texCoords.push_back( srcTexCoords.getY2() );

			if( ! colors.empty() ) {
				for( int i = 0; i < 4; ++i )
					vertColors.push_back( colors[glyphIt-glyphMeasures.begin()] );
			}
			
			indices.push_back( curIdx + 0 ); indices.push_back( curIdx + 1 ); indices.push_back( curIdx + 2 );
			indices.push_back( curIdx + 2 ); indices.push_back( curIdx + 1 ); indices.push_back( curIdx + 3 );
			curIdx += 4;
		}
		
		if( curIdx == 0 )
			continue;
		
		curTex.bind();
		glVertexAttribPointer( attr.mVertex, 3, GL_FLOAT, GL_FALSE, 0, &verts[0] );
		glVertexAttribPointer( attr.mTexCoord, 2, GL_FLOAT, GL_FALSE, 0, &texCoords[0] );
		if( ! colors.empty() )
			glVertexAttribPointer( attr.mColor, 4, GL_UNSIGNED_BYTE, GL_FALSE, 0, &vertColors[0] );
		glDrawElements( GL_TRIANGLES, indices.size(), indexType, &indices[0] );
	}
}
