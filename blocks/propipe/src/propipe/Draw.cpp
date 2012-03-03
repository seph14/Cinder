#include "Draw.h"

#include "cinder/gl/GlslProg.h"

using namespace cinder::gl;

namespace cinder { namespace pp {

class PPDrawRenderer : public DrawRenderer
{
public:
    PPDrawRenderer() 
        : mPositionDim(3), mPositionArray(0), mTexCoordArray(0), mColorArray(0), mNormalArray(0)
    {
        try {
            mShader  = gl::GlslProg(vert, frag);
            mPositionAttrib = mShader.getAttribLocation("aPosition");
            mTexCoordAttrib = mShader.getAttribLocation("aTexCoord");
            mColorAttrib    = mShader.getAttribLocation("aColor");
        }
        catch (GlslProgCompileExc& ex) {
            CI_LOGE("Error compiling: %s", ex.what());
        }
        // mNormalAttrib   = mShader.getAttribLocation("aNormal");
    }

	virtual void setModelView(const Matrix44f& mv)
	{
		mModelView = mv;
	}

	virtual void setProjection(const Matrix44f& proj)
	{
		mProjection = proj;
	}

	virtual void setColor(const ColorA& color)
	{
		mColor = color;
	}

	virtual void setPositionArray(float* pos, int dim)
	{
		mPositionArray = pos;
        mPositionDim = dim;
	}

	virtual void setTexCoordArray(float* texCoord)
	{
		mTexCoordArray = texCoord;
	}

	virtual void setColorArray(GLubyte* colors)
	{
		mColorArray = colors;
	}

	virtual void setNormalArray(float* norms)
    {
        mNormalArray = norms;
    }

    virtual void resetArrays()
    {
        mPositionArray = NULL;
        mTexCoordArray = NULL;
        mColorArray    = NULL;
        mNormalArray   = NULL;
    }

	virtual void bind()
	{
		mShader.bind();
	}

	virtual void unbind()
	{
		mShader.unbind();
	}

	virtual void enableClientState()
	{
		glEnableVertexAttribArray(mPositionAttrib);
		if ( mColorArray )
			glEnableVertexAttribArray(mColorAttrib);
        if ( mTexCoordArray )
            glEnableVertexAttribArray(mTexCoordAttrib);
        // if ( mNormalArray )
        //     glEnableVertexAttribArray(mNormalAttrib);

		glVertexAttribPointer( mPositionAttrib, mPositionDim, GL_FLOAT, GL_FALSE, 0, mPositionArray );
		glVertexAttribPointer( mTexCoordAttrib, 2, GL_FLOAT, GL_FALSE, 0, mTexCoordArray );

		if ( mColorArray ) {
			mShader.uniform("uEnableColorAttr", true);
			glVertexAttribPointer( mColorAttrib, 4, GL_UNSIGNED_BYTE, GL_FALSE, 0, mColorArray );
		}
		else {
			mShader.uniform("uColor", mColor);
			mShader.uniform("uEnableColorAttr", false);
		}

        if (mTexCoordArray) {
            mShader.uniform("sTexture", 0);
            mShader.uniform("uEnableTextureAttr", true);
        }
        else {
            mShader.uniform("uEnableTextureAttr", false);
        }

        // if ( mNormalArray )
        //     glVertexAttribPointer("");

		mShader.uniform("uModelView", mModelView);
		mShader.uniform("uProjection", mProjection);
	}

	virtual void disableClientState()
	{
		glDisableVertexAttribArray(mPositionAttrib);
		glDisableVertexAttribArray(mColorAttrib);
		glDisableVertexAttribArray(mTexCoordAttrib);
        // glDisableVertexAttribArray(mNormalAttrib);
	}

	static const char* vert;
	static const char* frag;

protected:
	gl::GlslProg mShader;
	Matrix44f mModelView;
	Matrix44f mProjection;

	GLuint mPositionAttrib;
	GLuint mTexCoordAttrib;
	GLuint mColorAttrib;
	GLuint mNormalAttrib;

    int       mPositionDim;
	ColorA8u  mColor;

	float*    mPositionArray;
	float*    mTexCoordArray;
	GLubyte*  mColorArray;
    float*    mNormalArray;
};

const char* PPDrawRenderer::vert =
        "attribute vec4 aPosition;\n"
        "attribute vec2 aTexCoord;\n"
        "attribute vec4 aColor;\n"

        "uniform mat4 uModelView;\n"
        "uniform mat4 uProjection;\n"
        "uniform vec4 uColor;\n"

        "uniform bool uEnableColorAttr;\n"
        "uniform bool uEnableTextureAttr;\n"

        "varying vec4 vColor;\n"
        "varying vec2 vTexCoord;\n"

        "void main() {\n"
        "  if (uEnableColorAttr) {\n"
        "    vColor = aColor;\n"
        "  }\n"
		"  else {\n"
		"    vColor = uColor;\n"
		"  }\n"
        "  if (uEnableTextureAttr) {\n"
        "    vTexCoord = aTexCoord;\n"
        "  }\n"
        "  gl_Position = uProjection * uModelView * aPosition;\n"
        "}\n";

const char* PPDrawRenderer::frag =
        "precision mediump float;\n"

        "uniform sampler2D sTexture;\n"

        "uniform bool uEnableTextureAttr;\n"

        "varying vec4 vColor;\n"
        "varying vec2 vTexCoord;\n"

        "void main() {\n"
        "  if (uEnableTextureAttr) {\n"
        "    gl_FragColor = vColor * texture2D(sTexture, vTexCoord);\n"
        "  }\n"
        "  else {\n"
        "    gl_FragColor = vColor;\n"
        "  }\n"
        "}\n";


void DrawRenderer::drawLine( const Vec2f &start, const Vec2f &end )
{
    drawLine(Vec3f(start.x, start.y, 0), Vec3f(end.x, end.y, 0));
}

void DrawRenderer::drawLine( const Vec3f &start, const Vec3f &end )
{
    float lineVerts[3*2];
    resetArrays();
    setPositionArray(lineVerts, 3);
    lineVerts[0] = start.x; lineVerts[1] = start.y; lineVerts[2] = start.z;
    lineVerts[3] = end.x; lineVerts[4] = end.y; lineVerts[5] = end.z; 
    enableClientState();
    glDrawArrays( GL_LINES, 0, 2 );
    disableClientState();
}

namespace {

void drawCubeImpl( DrawRenderer& render, const Vec3f &c, const Vec3f &size, bool drawColors, bool textured )
{
	GLfloat sx = size.x * 0.5f;
	GLfloat sy = size.y * 0.5f;
	GLfloat sz = size.z * 0.5f;
	GLfloat vertices[24*3]={c.x+1.0f*sx,c.y+1.0f*sy,c.z+1.0f*sz,	c.x+1.0f*sx,c.y+-1.0f*sy,c.z+1.0f*sz,	c.x+1.0f*sx,c.y+-1.0f*sy,c.z+-1.0f*sz,	c.x+1.0f*sx,c.y+1.0f*sy,c.z+-1.0f*sz,		// +X
							c.x+1.0f*sx,c.y+1.0f*sy,c.z+1.0f*sz,	c.x+1.0f*sx,c.y+1.0f*sy,c.z+-1.0f*sz,	c.x+-1.0f*sx,c.y+1.0f*sy,c.z+-1.0f*sz,	c.x+-1.0f*sx,c.y+1.0f*sy,c.z+1.0f*sz,		// +Y
							c.x+1.0f*sx,c.y+1.0f*sy,c.z+1.0f*sz,	c.x+-1.0f*sx,c.y+1.0f*sy,c.z+1.0f*sz,	c.x+-1.0f*sx,c.y+-1.0f*sy,c.z+1.0f*sz,	c.x+1.0f*sx,c.y+-1.0f*sy,c.z+1.0f*sz,		// +Z
							c.x+-1.0f*sx,c.y+1.0f*sy,c.z+1.0f*sz,	c.x+-1.0f*sx,c.y+1.0f*sy,c.z+-1.0f*sz,	c.x+-1.0f*sx,c.y+-1.0f*sy,c.z+-1.0f*sz,	c.x+-1.0f*sx,c.y+-1.0f*sy,c.z+1.0f*sz,	// -X
							c.x+-1.0f*sx,c.y+-1.0f*sy,c.z+-1.0f*sz,	c.x+1.0f*sx,c.y+-1.0f*sy,c.z+-1.0f*sz,	c.x+1.0f*sx,c.y+-1.0f*sy,c.z+1.0f*sz,	c.x+-1.0f*sx,c.y+-1.0f*sy,c.z+1.0f*sz,	// -Y
							c.x+1.0f*sx,c.y+-1.0f*sy,c.z+-1.0f*sz,	c.x+-1.0f*sx,c.y+-1.0f*sy,c.z+-1.0f*sz,	c.x+-1.0f*sx,c.y+1.0f*sy,c.z+-1.0f*sz,	c.x+1.0f*sx,c.y+1.0f*sy,c.z+-1.0f*sz};	// -Z


	static GLfloat normals[24*3]={ 1,0,0,	1,0,0,	1,0,0,	1,0,0,
								  0,1,0,	0,1,0,	0,1,0,	0,1,0,
									0,0,1,	0,0,1,	0,0,1,	0,0,1,
								  -1,0,0,	-1,0,0,	-1,0,0,	-1,0,0,
								  0,-1,0,	0,-1,0,  0,-1,0,0,-1,0,
								  0,0,-1,	0,0,-1,	0,0,-1,	0,0,-1};
	 
	static GLubyte colors[24*4]={	255,0,0,255,	255,0,0,255,	255,0,0,255,	255,0,0,255,	// +X = red
									0,255,0,255,	0,255,0,255,	0,255,0,255,	0,255,0,255,	// +Y = green
									0,0,255,255,	0,0,255,255,	0,0,255,255,	0,0,255,255,	// +Z = blue
									0,255,255,255,	0,255,255,255,	0,255,255,255,	0,255,255,255,	// -X = cyan
									255,0,255,255,	255,0,255,255,	255,0,255,255,	255,0,255,255,	// -Y = purple
									255,255,0,255,	255,255,0,255,	255,255,0,255,	255,255,0,255 };// -Z = yellow

	static GLfloat texs[24*2]={	0,1,	1,1,	1,0,	0,0,
								1,1,	1,0,	0,0,	0,1,
								0,1,	1,1,	1,0,	0,0,							
								1,1,	1,0,	0,0,	0,1,
								1,0,	0,0,	0,1,	1,1,
								1,0,	0,0,	0,1,	1,1 };
	
	static GLubyte elements[6*6] ={	0, 1, 2, 0, 2, 3,
									4, 5, 6, 4, 6, 7,
									8, 9,10, 8, 10,11,
									12,13,14,12,14,15,
									16,17,18,16,18,19,
									20,21,22,20,22,23 };

    render.resetArrays();

    render.setNormalArray( normals );
    if ( textured ) {
        render.setTexCoordArray( texs );
    }
    if ( drawColors ) {
        render.setColorArray( colors );
    }
    render.setPositionArray( vertices, 3 );

    render.enableClientState();
	glDrawElements( GL_TRIANGLES, 36, GL_UNSIGNED_BYTE, elements );
    render.disableClientState();
}
} // anonymous namespace


void DrawRenderer::drawCube( const Vec3f &center, const Vec3f &size, bool textured )
{
	drawCubeImpl( *this, center, size, false, textured );
}

void DrawRenderer::drawColorCube( const Vec3f &center, const Vec3f &size, bool textured )
{
	drawCubeImpl( *this, center, size, true, textured );
}

void DrawRenderer::drawStrokedCube( const Vec3f &center, const Vec3f &size )
{
	Vec3f min = center - size * 0.5f;
	Vec3f max = center + size * 0.5f;

	drawLine( Vec3f(min.x, min.y, min.z), Vec3f(max.x, min.y, min.z) );
	drawLine( Vec3f(max.x, min.y, min.z), Vec3f(max.x, max.y, min.z) );
	drawLine( Vec3f(max.x, max.y, min.z), Vec3f(min.x, max.y, min.z) );
	drawLine( Vec3f(min.x, max.y, min.z), Vec3f(min.x, min.y, min.z) );
	
	drawLine( Vec3f(min.x, min.y, max.z), Vec3f(max.x, min.y, max.z) );
	drawLine( Vec3f(max.x, min.y, max.z), Vec3f(max.x, max.y, max.z) );
	drawLine( Vec3f(max.x, max.y, max.z), Vec3f(min.x, max.y, max.z) );
	drawLine( Vec3f(min.x, max.y, max.z), Vec3f(min.x, min.y, max.z) );
	
	drawLine( Vec3f(min.x, min.y, min.z), Vec3f(min.x, min.y, max.z) );
	drawLine( Vec3f(min.x, max.y, min.z), Vec3f(min.x, max.y, max.z) );
	drawLine( Vec3f(max.x, max.y, min.z), Vec3f(max.x, max.y, max.z) );
	drawLine( Vec3f(max.x, min.y, min.z), Vec3f(max.x, min.y, max.z) );
}

void DrawRenderer::drawSphere( const Vec3f &center, float radius, int segments, bool textured )
{
	if( segments < 0 )
		return;

	float *verts = new float[(segments+1)*2*3];
	float *normals = new float[(segments+1)*2*3];
	float *texCoords = new float[(segments+1)*2*2];

    resetArrays();
    setPositionArray( verts, 3 );
    if ( textured )
        setTexCoordArray( texCoords );
    setNormalArray( normals );

    enableClientState();

	for( int j = 0; j < segments / 2; j++ ) {
		float theta1 = j * 2 * 3.14159f / segments - ( 3.14159f / 2.0f );
		float theta2 = (j + 1) * 2 * 3.14159f / segments - ( 3.14159f / 2.0f );

		for( int i = 0; i <= segments; i++ ) {
			Vec3f e, p;
			float theta3 = i * 2 * 3.14159f / segments;

			e.x = math<float>::cos( theta1 ) * math<float>::cos( theta3 );
			e.y = math<float>::sin( theta1 );
			e.z = math<float>::cos( theta1 ) * math<float>::sin( theta3 );
			p = e * radius + center;
			normals[i*3*2+0] = e.x; normals[i*3*2+1] = e.y; normals[i*3*2+2] = e.z;
			texCoords[i*2*2+0] = 0.999f - i / (float)segments; texCoords[i*2*2+1] = 0.999f - 2 * j / (float)segments;
			verts[i*3*2+0] = p.x; verts[i*3*2+1] = p.y; verts[i*3*2+2] = p.z;

			e.x = math<float>::cos( theta2 ) * math<float>::cos( theta3 );
			e.y = math<float>::sin( theta2 );
			e.z = math<float>::cos( theta2 ) * math<float>::sin( theta3 );
			p = e * radius + center;
			normals[i*3*2+3] = e.x; normals[i*3*2+4] = e.y; normals[i*3*2+5] = e.z;
			texCoords[i*2*2+2] = 0.999f - i / (float)segments; texCoords[i*2*2+3] = 0.999f - 2 * ( j + 1 ) / (float)segments;
			verts[i*3*2+3] = p.x; verts[i*3*2+4] = p.y; verts[i*3*2+5] = p.z;
		}
		glDrawArrays( GL_TRIANGLE_STRIP, 0, (segments + 1)*2 );
	}

    disableClientState();
	
	delete [] verts;
	delete [] normals;
	delete [] texCoords;
}

void DrawRenderer::draw( const class Sphere &sphere, int segments, bool textured )
{
	drawSphere( sphere.getCenter(), sphere.getRadius(), segments, textured );
}

void DrawRenderer::drawSolidCircle( const Vec2f &center, float radius, int numSegments )
{
	// automatically determine the number of segments from the circumference
	if( numSegments <= 0 ) {
		numSegments = (int)math<double>::floor( radius * M_PI * 2 );
	}
	if( numSegments < 2 ) numSegments = 2;
	
	GLfloat *verts = new float[(numSegments+2)*3];
	verts[0] = center.x;
	verts[1] = center.y;
    verts[2] = 0;
	for( int s = 0; s <= numSegments; s++ ) {
		float t = s / (float)numSegments * 2.0f * 3.14159f;
		verts[(s+1)*2+0] = center.x + math<float>::cos( t ) * radius;
		verts[(s+1)*2+1] = center.y + math<float>::sin( t ) * radius;
	}

    resetArrays();
    setPositionArray( verts, 2 );
    enableClientState();
	glDrawArrays( GL_TRIANGLE_FAN, 0, numSegments + 2 );
    disableClientState();
	delete [] verts;
}

void DrawRenderer::drawStrokedCircle( const Vec2f &center, float radius, int numSegments )
{
	// automatically determine the number of segments from the circumference
	if( numSegments <= 0 ) {
		numSegments = (int)math<double>::floor( radius * M_PI * 2 );
	}
	if( numSegments < 2 ) numSegments = 2;
	
	GLfloat *verts = new float[numSegments*3];
	for( int s = 0; s < numSegments; s++ ) {
		float t = s / (float)numSegments * 2.0f * 3.14159f;
		verts[s*2+0] = center.x + math<float>::cos( t ) * radius;
		verts[s*2+1] = center.y + math<float>::sin( t ) * radius;
	}

    resetArrays();
    setPositionArray( verts, 2 );
    enableClientState();
	glDrawArrays( GL_LINE_LOOP, 0, numSegments );
    disableClientState();
	delete [] verts;
}

void DrawRenderer::drawSolidRect( const Rectf &rect, bool textureRectangle )
{
	GLfloat verts[8];
	GLfloat texCoords[8];

	verts[0*2+0] = rect.getX2(); texCoords[0*2+0] = ( textureRectangle ) ? rect.getX2() : 1;
	verts[0*2+1] = rect.getY1(); texCoords[0*2+1] = ( textureRectangle ) ? rect.getY1() : 0;
	verts[1*2+0] = rect.getX1(); texCoords[1*2+0] = ( textureRectangle ) ? rect.getX1() : 0;
	verts[1*2+1] = rect.getY1(); texCoords[1*2+1] = ( textureRectangle ) ? rect.getY1() : 0;
	verts[2*2+0] = rect.getX2(); texCoords[2*2+0] = ( textureRectangle ) ? rect.getX2() : 1;
	verts[2*2+1] = rect.getY2(); texCoords[2*2+1] = ( textureRectangle ) ? rect.getY2() : 1;
	verts[3*2+0] = rect.getX1(); texCoords[3*2+0] = ( textureRectangle ) ? rect.getX1() : 0;
	verts[3*2+1] = rect.getY2(); texCoords[3*2+1] = ( textureRectangle ) ? rect.getY2() : 1;

    resetArrays();
    setPositionArray(verts, 2);
    setTexCoordArray(texCoords);
    enableClientState();
	glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );
    disableClientState();
}

void DrawRenderer::drawStrokedRect( const Rectf &rect )
{
	GLfloat verts[8];
	verts[0] = rect.getX1();	verts[1] = rect.getY1();
	verts[2] = rect.getX2();	verts[3] = rect.getY1();
	verts[4] = rect.getX2();	verts[5] = rect.getY2();
	verts[6] = rect.getX1();	verts[7] = rect.getY2();

    resetArrays();
    setPositionArray( verts, 2 );
    enableClientState();
	glDrawArrays( GL_LINE_LOOP, 0, 4 );
    disableClientState();
}

void DrawRenderer::drawCoordinateFrame( float axisLength, float headLength, float headRadius )
{
    setColor(ColorA8u(255, 0, 0, 255));
	drawVector( Vec3f::zero(), Vec3f::xAxis() * axisLength, headLength, headRadius );
    setColor(ColorA8u(0, 255, 0, 255));
	drawVector( Vec3f::zero(), Vec3f::yAxis() * axisLength, headLength, headRadius );
    setColor(ColorA8u(0, 0, 255, 255));
	drawVector( Vec3f::zero(), Vec3f::zAxis() * axisLength, headLength, headRadius );
}

void DrawRenderer::drawVector( const Vec3f &start, const Vec3f &end, float headLength, float headRadius )
{
	const int NUM_SEGMENTS = 32;
	float lineVerts[3*2];
	Vec3f coneVerts[NUM_SEGMENTS+2];

    resetArrays();
    setPositionArray(lineVerts, 3);
    enableClientState();

	lineVerts[0] = start.x; lineVerts[1] = start.y; lineVerts[2] = start.z;
	lineVerts[3] = end.x; lineVerts[4] = end.y; lineVerts[5] = end.z;	
	glDrawArrays( GL_LINES, 0, 2 );
	
	// Draw the cone
	Vec3f axis = ( end - start ).normalized();
	Vec3f temp = ( axis.dot( Vec3f::yAxis() ) > 0.999f ) ? axis.cross( Vec3f::xAxis() ) : axis.cross( Vec3f::yAxis() );
	Vec3f left = axis.cross( temp ).normalized();
	Vec3f up = axis.cross( left ).normalized();

    setPositionArray(&coneVerts[0].x, 3);
	coneVerts[0] = Vec3f( end + axis * headLength );
	for( int s = 0; s <= NUM_SEGMENTS; ++s ) {
		float t = s / (float)NUM_SEGMENTS;
		coneVerts[s+1] = Vec3f( end + left * headRadius * math<float>::cos( t * 2 * 3.14159f )
			+ up * headRadius * math<float>::sin( t * 2 * 3.14159f ) );
	}
	glDrawArrays( GL_TRIANGLE_FAN, 0, NUM_SEGMENTS+2 );

	// draw the cap
    setPositionArray(&coneVerts[0].x, 3);
	coneVerts[0] = end;
	for( int s = 0; s <= NUM_SEGMENTS; ++s ) {
		float t = s / (float)NUM_SEGMENTS;
		coneVerts[s+1] = Vec3f( end - left * headRadius * math<float>::cos( t * 2 * 3.14159f )
			+ up * headRadius * math<float>::sin( t * 2 * 3.14159f ) );
	}
	glDrawArrays( GL_TRIANGLE_FAN, 0, NUM_SEGMENTS+2 );

    disableClientState();
}

void DrawRenderer::drawFrustum( const Camera &cam )
{
	Vec3f vertex[8];
	Vec3f nearTopLeft, nearTopRight, nearBottomLeft, nearBottomRight;
	cam.getNearClipCoordinates( &nearTopLeft, &nearTopRight, &nearBottomLeft, &nearBottomRight );

	Vec3f farTopLeft, farTopRight, farBottomLeft, farBottomRight;
	cam.getFarClipCoordinates( &farTopLeft, &farTopRight, &farBottomLeft, &farBottomRight );
	
    resetArrays();
    setPositionArray(&vertex[0].x, 3);
    enableClientState();
	
	vertex[0] = cam.getEyePoint();
	vertex[1] = nearTopLeft;
	vertex[2] = cam.getEyePoint();
	vertex[3] = nearTopRight;
	vertex[4] = cam.getEyePoint();
	vertex[5] = nearBottomRight;
	vertex[6] = cam.getEyePoint();
	vertex[7] = nearBottomLeft;
	glDrawArrays( GL_LINES, 0, 8 );

	vertex[0] = farTopLeft;
	vertex[1] = nearTopLeft;
	vertex[2] = farTopRight;
	vertex[3] = nearTopRight;
	vertex[4] = farBottomRight;
	vertex[5] = nearBottomRight;
	vertex[6] = farBottomLeft;
	vertex[7] = nearBottomLeft;
	glDrawArrays( GL_LINES, 0, 8 );

	glLineWidth( 2.0f );
	vertex[0] = nearTopLeft;
	vertex[1] = nearTopRight;
	vertex[2] = nearBottomRight;
	vertex[3] = nearBottomLeft;
	glDrawArrays( GL_LINE_LOOP, 0, 4 );

	vertex[0] = farTopLeft;
	vertex[1] = farTopRight;
	vertex[2] = farBottomRight;
	vertex[3] = farBottomLeft;
	glDrawArrays( GL_LINE_LOOP, 0, 4 );
	
	glLineWidth( 1.0f );
    
    disableClientState();
}

void DrawRenderer::drawTorus( float outterRadius, float innerRadius, int longitudeSegments, int latitudeSegments, bool textured )
{
	longitudeSegments = std::min( std::max( 7, longitudeSegments ) + 1, 255 );
	latitudeSegments = std::min( std::max( 7, latitudeSegments ) + 1, 255 );

	int i, j;
	float *normal = new float[longitudeSegments * latitudeSegments * 3];
	float *vertex = new float[longitudeSegments * latitudeSegments * 3];
	float *tex = new float[longitudeSegments * latitudeSegments * 2];
	GLushort *indices = new GLushort[latitudeSegments * 2];
	float ct, st, cp, sp;

    resetArrays();
    setPositionArray(vertex, 3);
    if (textured)
        setTexCoordArray(tex);
    setNormalArray(normal);
    enableClientState();

	for( i = 0; i < longitudeSegments; i++ ) {
		ct = cos(2.0f * (float)M_PI * (float)i / (float)(longitudeSegments - 1));
		st = sin(2.0f * (float)M_PI * (float)i / (float)(longitudeSegments - 1));

		for ( j = 0; j < latitudeSegments; j++ ) {
			cp = cos(2.0f * (float)M_PI * (float)j / (float)(latitudeSegments - 1));
			sp = sin(2.0f * (float)M_PI * (float)j / (float)(latitudeSegments - 1));

			normal[3 * (i + longitudeSegments * j)    ] = cp * ct;
			normal[3 * (i + longitudeSegments * j) + 1] = sp * ct;
			normal[3 * (i + longitudeSegments * j) + 2] = st;

			tex[2 * (i + longitudeSegments * j)    ] = 1.0f * (float)i / (float)(longitudeSegments - 1);
			tex[2 * (i + longitudeSegments * j) + 1] = 5.0f * (float)j / (float)(latitudeSegments - 1);

			vertex[3 * (i + longitudeSegments * j)    ] = cp * (outterRadius  + innerRadius * ct);
			vertex[3 * (i + longitudeSegments * j) + 1] = sp * (outterRadius  + innerRadius * ct);
			vertex[3 * (i + longitudeSegments * j) + 2] = innerRadius * st;
		}
	}

	for ( i = 0; i < longitudeSegments - 1; i++ ) {
		for ( j = 0; j < latitudeSegments; j++ ) {
			indices[j*2+0] = i + 1 + longitudeSegments * j;
			indices[j*2+1] = i + longitudeSegments * j;
		}
		glDrawElements( GL_TRIANGLE_STRIP, (latitudeSegments)*2, GL_UNSIGNED_SHORT, indices );
	}

    disableClientState();
	
	delete [] normal;
	delete [] tex;
	delete [] vertex;
	delete [] indices;
}

void DrawRenderer::drawCylinder( float baseRadius, float topRadius, float height, int slices, int stacks, bool textured )
{
	stacks = math<int>::max(2, stacks + 1);	// minimum of 1 stack
	slices = math<int>::max(4, slices + 1);	// minimum of 3 slices

	int i, j;
	float *normal = new float[stacks * slices * 3];
	float *vertex = new float[stacks * slices * 3];
	float *tex = new float[stacks * slices * 2];
	GLushort *indices = new GLushort[slices * 2];

    resetArrays();
    setPositionArray(vertex, 3);
    setTexCoordArray(tex);
    setNormalArray(normal);
    enableClientState();

	for(i=0;i<slices;i++) {
		float u = (float)i / (float)(slices - 1);
		float ct = cos(2.0f * (float)M_PI * u);
		float st = sin(2.0f * (float)M_PI * u);

		for(j=0;j<stacks;j++) {
			float v = (float)j / (float)(stacks-1);
			float radius = lerp<float>(baseRadius, topRadius, v); 

			int index = 3 * (i * stacks + j);

			normal[index    ] = ct;
			normal[index + 1] = 0;
			normal[index + 2] = st;

			tex[2 * (i * stacks + j)    ] = u;
			tex[2 * (i * stacks + j) + 1] = 1.0f - v; // top of texture is top of cylinder

			vertex[index    ] = ct * radius;
			vertex[index + 1] = v * height;
			vertex[index + 2] = st * radius;
		}
	}

	for(i=0;i<(stacks - 1);i++) {
		for(j=0;j<slices;j++) {
			indices[j*2+0] = i + 0 + j * stacks;
			indices[j*2+1] = i + 1 + j * stacks;
		}
		glDrawElements( GL_TRIANGLE_STRIP, (slices)*2, GL_UNSIGNED_SHORT, indices );
	}

    disableClientState();

	delete [] normal;
	delete [] tex;
	delete [] vertex;
	delete [] indices;
}

// void draw( const class PolyLine<Vec2f> &polyLine );
void DrawRenderer::draw( const class PolyLine<Vec3f> &polyLine )
{
    resetArrays();
    setPositionArray((float*) &(polyLine.getPoints()[0]), 3);
    enableClientState();
    glDrawArrays( ( polyLine.isClosed() ) ? GL_LINE_LOOP : GL_LINE_STRIP, 0, polyLine.size() );

    disableClientState();
}

void DrawRenderer::draw( const class Path2d &path2d, float approximationScale )
{
	if( path2d.getNumSegments() == 0 )
		return;
	std::vector<Vec2f> points = path2d.subdivide( approximationScale );

    resetArrays();
    setPositionArray((float*) &(points[0]), 2);
    enableClientState();
	glDrawArrays( GL_LINE_STRIP, 0, points.size() );
    disableClientState();
}

void DrawRenderer::draw( const class Shape2d &shape2d, float approximationScale )
{
//    glEnableVertexAttribArray(mVertex);
//	for( std::vector<Path2d>::const_iterator contourIt = shape2d.getContours().begin(); contourIt != shape2d.getContours().end(); ++contourIt ) {
//		if( contourIt->getNumSegments() == 0 )
//			continue;
//		std::vector<Vec2f> points2f = contourIt->subdivide( approximationScale );
//        std::vector<Vec3f> points;
//        v3Fromv2( points, points2f );
//        glVertexAttribPointer( mVertex, 3, GL_FLOAT, GL_FALSE, 0, &(points[0]) );
//		glDrawArrays( GL_LINE_STRIP, 0, points.size() );
//	}
//    glDisableVertexAttribArray( mVertex );
}


void DrawRenderer::drawSolid( const class Path2d &path2d, float approximationScale )
{
}


void DrawRenderer::draw( const TriMesh &mesh )
{
}

void DrawRenderer::drawRange( const TriMesh &mesh, size_t startTriangle, size_t triangleCount )
{
}

void DrawRenderer::draw( const VboMesh &vbo )
{
}

void DrawRenderer::drawRange( const VboMesh &vbo, size_t startIndex, size_t indexCount, int vertexStart, int vertexEnd )
{
}

void DrawRenderer::drawArrays( const VboMesh &vbo, GLint first, GLsizei count )
{
}

void DrawRenderer::drawBillboard( const Vec3f &pos, const Vec2f &scale, float rotationDegrees, const Vec3f &bbRight, const Vec3f &bbUp )
{
}

void DrawRenderer::draw( const Texture &texture )
{
	draw( texture, Area( texture.getCleanBounds() ), texture.getCleanBounds() );
}

void DrawRenderer::draw( const Texture &texture, const Vec2f &pos )
{
	draw( texture, texture.getCleanBounds(), Rectf( pos.x, pos.y, pos.x + texture.getCleanWidth(), pos.y + texture.getCleanHeight() ) );
}

void DrawRenderer::draw( const Texture &texture, const Rectf &rect )
{
	draw( texture, texture.getCleanBounds(), rect );
}

void DrawRenderer::draw( const Texture &texture, const Area &srcArea, const Rectf &destRect )
{
    texture.bind();

    GLfloat verts[8];
    GLfloat texCoords[8];

    resetArrays();
    setPositionArray(verts, 2);
    setTexCoordArray(texCoords);
    enableClientState();

    verts[0*2+0] = destRect.getX2(); verts[0*2+1] = destRect.getY1(); 
    verts[1*2+0] = destRect.getX1(); verts[1*2+1] = destRect.getY1(); 
    verts[2*2+0] = destRect.getX2(); verts[2*2+1] = destRect.getY2(); 
    verts[3*2+0] = destRect.getX1(); verts[3*2+1] = destRect.getY2(); 

    const Rectf srcCoords = texture.getAreaTexCoords( srcArea );
    texCoords[0*2+0] = srcCoords.getX2(); texCoords[0*2+1] = srcCoords.getY1();	
    texCoords[1*2+0] = srcCoords.getX1(); texCoords[1*2+1] = srcCoords.getY1();	
    texCoords[2*2+0] = srcCoords.getX2(); texCoords[2*2+1] = srcCoords.getY2();	
    texCoords[3*2+0] = srcCoords.getX1(); texCoords[3*2+1] = srcCoords.getY2();	

    glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );

    disableClientState();
}

DrawRendererRef DrawRenderer::create()
{
    return DrawRendererRef(new PPDrawRenderer());
}

} }
