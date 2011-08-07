#include "cinder/gl/gles2.h"
#include "cinder/gl/GlslProg.h"
#include "cinder/gl/Vbo.h"
#include "cinder/CinderMath.h"
#include "cinder/Vector.h"
#include "cinder/Camera.h"
#include "cinder/TriMesh.h"
#include "cinder/Sphere.h"
#include "cinder/gl/Texture.h"
#include "cinder/Text.h"
#include "cinder/PolyLine.h"
#include "cinder/Path2d.h"
#include "cinder/Shape2d.h"

using std::shared_ptr;

namespace cinder { namespace gl {

GlesAttr::GlesAttr()
    : vertex(0), texCoord(0), color(0), normal(0)
{
}

GlesState::Obj::Obj()
{
}

GlesState::Obj::~Obj()
{
}

GlesState::GlesState()
    : mObj( shared_ptr<Obj>( new Obj() ) )
{
}

GlesState::GlesState(GlslProg& shader)
    : mObj( shared_ptr<Obj>( new Obj() ) )
{
}

void drawLine( GlesState& state, const Vec2f &start, const Vec2f &end )
{
    GlesAttr& attr = state.attr();

	float lineVerts[2*2];
    glEnableVertexAttribArray(attr.vertex);
    glVertexAttribPointer(attr.vertex, 2, GL_FLOAT, GL_FALSE, 0, lineVerts);
	lineVerts[0] = start.x; lineVerts[1] = start.y;
	lineVerts[2] = end.x; lineVerts[3] = end.y;
    glDrawArrays(GL_LINES, 0, 2);
    glDisableVertexAttribArray(attr.vertex);
}


void drawLine( GlesState& state, const Vec3f &start, const Vec3f &end )
{
    GlesAttr& attr = state.attr();

	float lineVerts[3*2];
	glEnableVertexAttribArray(attr.vertex);
	glVertexAttribPointer( attr.vertex, 3, GL_FLOAT, GL_FALSE, 0, lineVerts );
	lineVerts[0] = start.x; lineVerts[1] = start.y; lineVerts[2] = start.z;
	lineVerts[3] = end.x; lineVerts[4] = end.y; lineVerts[5] = end.z; 
	glDrawArrays( GL_LINES, 0, 2 );
    glDisableVertexAttribArray(attr.vertex);
}

namespace {
void drawCubeImpl( GlesState& state, const Vec3f &c, const Vec3f &size, bool drawColors )
{
    GlesAttr& attr = state.attr();

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

    glEnableVertexAttribArray(attr.normal);
    glVertexAttribPointer( attr.normal, 3, GL_FLOAT, GL_FALSE, 0, normals );

    glEnableVertexAttribArray(attr.texCoord);
    glVertexAttribPointer( attr.texCoord, 2, GL_FLOAT, GL_FALSE, 0, texs );

	if( drawColors ) {
        glEnableVertexAttribArray(attr.color);
        glVertexAttribPointer( attr.color, 4, GL_UNSIGNED_BYTE, GL_FALSE, 0, colors );
	}

    glEnableVertexAttribArray(attr.vertex);
    glVertexAttribPointer( attr.vertex, 3, GL_FLOAT, GL_FALSE, 0, vertices );

	glDrawElements( GL_TRIANGLES, 36, GL_UNSIGNED_BYTE, elements );

	glDisableVertexAttribArray( attr.vertex );
	glDisableVertexAttribArray( attr.texCoord );	 
	glDisableVertexAttribArray( attr.normal );
	if( drawColors )
		glDisableVertexAttribArray( attr.color );
	 
}
} // anonymous namespace

void drawCube( GlesState& state, const Vec3f &center, const Vec3f &size )
{
	drawCubeImpl( state, center, size, false );
}


void drawColorCube( GlesState& state, const Vec3f &center, const Vec3f &size )
{
	drawCubeImpl( state, center, size, true );
}


void drawStrokedCube( GlesState& state, const Vec3f &center, const Vec3f &size )
{
    GlesAttr& attr = state.attr();

	Vec3f min = center - size * 0.5f;
	Vec3f max = center + size * 0.5f;

	gl::drawLine( state, Vec3f(min.x, min.y, min.z), Vec3f(max.x, min.y, min.z) );
	gl::drawLine( state, Vec3f(max.x, min.y, min.z), Vec3f(max.x, max.y, min.z) );
	gl::drawLine( state, Vec3f(max.x, max.y, min.z), Vec3f(min.x, max.y, min.z) );
	gl::drawLine( state, Vec3f(min.x, max.y, min.z), Vec3f(min.x, min.y, min.z) );
	
	gl::drawLine( state, Vec3f(min.x, min.y, max.z), Vec3f(max.x, min.y, max.z) );
	gl::drawLine( state, Vec3f(max.x, min.y, max.z), Vec3f(max.x, max.y, max.z) );
	gl::drawLine( state, Vec3f(max.x, max.y, max.z), Vec3f(min.x, max.y, max.z) );
	gl::drawLine( state, Vec3f(min.x, max.y, max.z), Vec3f(min.x, min.y, max.z) );
	
	gl::drawLine( state, Vec3f(min.x, min.y, min.z), Vec3f(min.x, min.y, max.z) );
	gl::drawLine( state, Vec3f(min.x, max.y, min.z), Vec3f(min.x, max.y, max.z) );
	gl::drawLine( state, Vec3f(max.x, max.y, min.z), Vec3f(max.x, max.y, max.z) );
	gl::drawLine( state, Vec3f(max.x, min.y, min.z), Vec3f(max.x, min.y, max.z) );
}


void drawSphere( GlesState& state, const Vec3f &center, float radius, int segments )
{
    GlesAttr& attr = state.attr();

	if( segments < 0 )
		return;

	float *verts = new float[(segments+1)*2*3];
	float *normals = new float[(segments+1)*2*3];
	float *texCoords = new float[(segments+1)*2*2];

    glEnableVertexAttribArray(attr.vertex);
    glVertexAttribPointer( attr.vertex, 3, GL_FLOAT, GL_FALSE, 0, verts );
    glEnableVertexAttribArray(attr.texCoord);
    glVertexAttribPointer( attr.texCoord, 2, GL_FLOAT, GL_FALSE, 0, texCoords );
    glEnableVertexAttribArray(attr.normal);
    glVertexAttribPointer( attr.normal, 3, GL_FLOAT, GL_FALSE, 0, normals );

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

    glDisableVertexAttribArray( attr.vertex );
    glDisableVertexAttribArray( attr.texCoord );
    glDisableVertexAttribArray( attr.normal );
	
	delete [] verts;
	delete [] normals;
	delete [] texCoords;
}


void draw( GlesState& state, const class Sphere &sphere, int segments )
{
    GlesAttr& attr = state.attr();

	drawSphere( state, sphere.getCenter(), sphere.getRadius(), segments );
}


void drawSolidCircle( GlesState& state, const Vec2f &center, float radius, int numSegments )
{
    GlesAttr& attr = state.attr();

	// automatically determine the number of segments from the circumference
	if( numSegments <= 0 ) {
		numSegments = (int)math<double>::floor( radius * M_PI * 2 );
	}
	if( numSegments < 2 ) numSegments = 2;
	
	GLfloat *verts = new float[(numSegments+2)*2];
	verts[0] = center.x;
	verts[1] = center.y;
	for( int s = 0; s <= numSegments; s++ ) {
		float t = s / (float)numSegments * 2.0f * 3.14159f;
		verts[(s+1)*2+0] = center.x + math<float>::cos( t ) * radius;
		verts[(s+1)*2+1] = center.y + math<float>::sin( t ) * radius;
	}
    glEnableVertexAttribArray(attr.vertex);
    glVertexAttribPointer( attr.vertex, 2, GL_FLOAT, GL_FALSE, 0, verts );
	glDrawArrays( GL_TRIANGLE_FAN, 0, numSegments + 2 );
    glDisableVertexAttribArray(attr.vertex);
	delete [] verts;
}


void drawStrokedCircle( GlesState& state, const Vec2f &center, float radius, int numSegments )
{
    GlesAttr& attr = state.attr();

	// automatically determine the number of segments from the circumference
	if( numSegments <= 0 ) {
		numSegments = (int)math<double>::floor( radius * M_PI * 2 );
	}
	if( numSegments < 2 ) numSegments = 2;
	
	GLfloat *verts = new float[numSegments*2];
	for( int s = 0; s < numSegments; s++ ) {
		float t = s / (float)numSegments * 2.0f * 3.14159f;
		verts[s*2+0] = center.x + math<float>::cos( t ) * radius;
		verts[s*2+1] = center.y + math<float>::sin( t ) * radius;
	}
    glEnableVertexAttribArray(attr.vertex);
    glVertexAttribPointer( attr.vertex, 2, GL_FLOAT, GL_FALSE, 0, verts );
	glDrawArrays( GL_LINE_LOOP, 0, numSegments );
    glDisableVertexAttribArray(attr.vertex);
	delete [] verts;
}


void drawSolidRect( GlesState& state, const Rectf &rect, bool textureRectangle )
{
    GlesAttr& attr = state.attr();

    glEnableVertexAttribArray(attr.vertex);
	GLfloat verts[8];
    glVertexAttribPointer( attr.vertex, 2, GL_FLOAT, GL_FALSE, 0, verts );
    glEnableVertexAttribArray(attr.texCoord);
	GLfloat texCoords[8];
    glVertexAttribPointer( attr.texCoord, 2, GL_FLOAT, GL_FALSE, 0, verts );
	verts[0*2+0] = rect.getX2(); texCoords[0*2+0] = ( textureRectangle ) ? rect.getX2() : 1;
	verts[0*2+1] = rect.getY1(); texCoords[0*2+1] = ( textureRectangle ) ? rect.getY1() : 0;
	verts[1*2+0] = rect.getX1(); texCoords[1*2+0] = ( textureRectangle ) ? rect.getX1() : 0;
	verts[1*2+1] = rect.getY1(); texCoords[1*2+1] = ( textureRectangle ) ? rect.getY1() : 0;
	verts[2*2+0] = rect.getX2(); texCoords[2*2+0] = ( textureRectangle ) ? rect.getX2() : 1;
	verts[2*2+1] = rect.getY2(); texCoords[2*2+1] = ( textureRectangle ) ? rect.getY2() : 1;
	verts[3*2+0] = rect.getX1(); texCoords[3*2+0] = ( textureRectangle ) ? rect.getX1() : 0;
	verts[3*2+1] = rect.getY2(); texCoords[3*2+1] = ( textureRectangle ) ? rect.getY2() : 1;

	glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );

    glDisableVertexAttribArray(attr.vertex);
    glDisableVertexAttribArray(attr.texCoord);	 
}


void drawStrokedRect( GlesState& state, const Rectf &rect )
{
    GlesAttr& attr = state.attr();

	GLfloat verts[8];
	verts[0] = rect.getX1();	verts[1] = rect.getY1();
	verts[2] = rect.getX2();	verts[3] = rect.getY1();
	verts[4] = rect.getX2();	verts[5] = rect.getY2();
	verts[6] = rect.getX1();	verts[7] = rect.getY2();
    glEnableVertexAttribArray(attr.vertex);
    glVertexAttribPointer( attr.vertex, 2, GL_FLOAT, GL_FALSE, 0, verts );
	glDrawArrays( GL_LINE_LOOP, 0, 4 );
    glDisableVertexAttribArray(attr.vertex);
}


void drawCoordinateFrame( GlesState& state, float axisLength, float headLength, float headRadius )
{
	// XXX glColor4ub( 255, 0, 0, 255 );
	drawVector( state, Vec3f::zero(), Vec3f::xAxis() * axisLength, headLength, headRadius );
	// XXX glColor4ub( 0, 255, 0, 255 );
	drawVector( state, Vec3f::zero(), Vec3f::yAxis() * axisLength, headLength, headRadius );
	// XXX glColor4ub( 0, 0, 255, 255 );
	drawVector( state, Vec3f::zero(), Vec3f::zAxis() * axisLength, headLength, headRadius );
}


void drawVector( GlesState& state, const Vec3f &start, const Vec3f &end, float headLength, float headRadius )
{
    GlesAttr& attr = state.attr();

	const int NUM_SEGMENTS = 32;
	float lineVerts[3*2];
	Vec3f coneVerts[NUM_SEGMENTS+2];
    glEnableVertexAttribArray(attr.vertex);
    glVertexAttribPointer( attr.vertex, 3, GL_FLOAT, GL_FALSE, 0, lineVerts );
	lineVerts[0] = start.x; lineVerts[1] = start.y; lineVerts[2] = start.z;
	lineVerts[3] = end.x; lineVerts[4] = end.y; lineVerts[5] = end.z;	
	glDrawArrays( GL_LINES, 0, 2 );
	
	// Draw the cone
	Vec3f axis = ( end - start ).normalized();
	Vec3f temp = ( axis.dot( Vec3f::yAxis() ) > 0.999f ) ? axis.cross( Vec3f::xAxis() ) : axis.cross( Vec3f::yAxis() );
	Vec3f left = axis.cross( temp ).normalized();
	Vec3f up = axis.cross( left ).normalized();

    glVertexAttribPointer( attr.vertex, 3, GL_FLOAT, GL_FALSE, 0, &coneVerts[0].x );
	coneVerts[0] = Vec3f( end + axis * headLength );
	for( int s = 0; s <= NUM_SEGMENTS; ++s ) {
		float t = s / (float)NUM_SEGMENTS;
		coneVerts[s+1] = Vec3f( end + left * headRadius * math<float>::cos( t * 2 * 3.14159f )
			+ up * headRadius * math<float>::sin( t * 2 * 3.14159f ) );
	}
	glDrawArrays( GL_TRIANGLE_FAN, 0, NUM_SEGMENTS+2 );

	// draw the cap
    glVertexAttribPointer( attr.vertex, 3, GL_FLOAT, GL_FALSE, 0, &coneVerts[0].x );
	coneVerts[0] = end;
	for( int s = 0; s <= NUM_SEGMENTS; ++s ) {
		float t = s / (float)NUM_SEGMENTS;
		coneVerts[s+1] = Vec3f( end - left * headRadius * math<float>::cos( t * 2 * 3.14159f )
			+ up * headRadius * math<float>::sin( t * 2 * 3.14159f ) );
	}
	glDrawArrays( GL_TRIANGLE_FAN, 0, NUM_SEGMENTS+2 );

    glDisableVertexAttribArray(attr.vertex);
}


void drawFrustum( GlesState& state, const Camera &cam )
{
    GlesAttr& attr = state.attr();

	Vec3f vertex[8];
	Vec3f nearTopLeft, nearTopRight, nearBottomLeft, nearBottomRight;
	cam.getNearClipCoordinates( &nearTopLeft, &nearTopRight, &nearBottomLeft, &nearBottomRight );

	Vec3f farTopLeft, farTopRight, farBottomLeft, farBottomRight;
	cam.getFarClipCoordinates( &farTopLeft, &farTopRight, &farBottomLeft, &farBottomRight );
	
    glEnableVertexAttribArray(attr.vertex);
    glVertexAttribPointer( attr.vertex, 3, GL_FLOAT, GL_FALSE, 0, &vertex[0].x );
	
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
    glDisableVertexAttribArray(attr.vertex);
}


void drawTorus( GlesState& state, float outterRadius, float innerRadius, int longitudeSegments, int latitudeSegments )
{
    GlesAttr& attr = state.attr();

	longitudeSegments = std::min( std::max( 7, longitudeSegments ) + 1, 255 );
	latitudeSegments = std::min( std::max( 7, latitudeSegments ) + 1, 255 );

	int i, j;
	float *normal = new float[longitudeSegments * latitudeSegments * 3];
	float *vertex = new float[longitudeSegments * latitudeSegments * 3];
	float *tex = new float[longitudeSegments * latitudeSegments * 2];
	GLushort *indices = new GLushort[latitudeSegments * 2];
	float ct, st, cp, sp;

    glEnableVertexAttribArray(attr.vertex);
    glVertexAttribPointer( attr.vertex, 3, GL_FLOAT, GL_FALSE, 0, vertex );
    glEnableVertexAttribArray(attr.texCoord);
    glVertexAttribPointer( attr.texCoord, 2, GL_FLOAT, GL_FALSE, 0, tex );
    glEnableVertexAttribArray(attr.normal);
    glVertexAttribPointer( attr.normal, 3, GL_FLOAT, GL_FALSE, 0, normal );

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

    glDisableVertexAttribArray( attr.vertex );
    glDisableVertexAttribArray( attr.texCoord );
    glDisableVertexAttribArray( attr.normal );
	
	
	delete [] normal;
	delete [] tex;
	delete [] vertex;
	delete [] indices;
}


void drawCylinder( GlesState& state, float baseRadius, float topRadius, float height, int slices, int stacks )
{
    GlesAttr& attr = state.attr();

	stacks = math<int>::max(2, stacks + 1);	// minimum of 1 stack
	slices = math<int>::max(4, slices + 1);	// minimum of 3 slices

	int i, j;
	float *normal = new float[stacks * slices * 3];
	float *vertex = new float[stacks * slices * 3];
	float *tex = new float[stacks * slices * 2];
	GLushort *indices = new GLushort[slices * 2];

    glEnableVertexAttribArray(attr.vertex);
    glVertexAttribPointer( attr.vertex, 3, GL_FLOAT, GL_FALSE, 0, vertex );
    glEnableVertexAttribArray(attr.texCoord);
    glVertexAttribPointer( attr.texCoord, 2, GL_FLOAT, GL_FALSE, 0, tex );
    glEnableVertexAttribArray(attr.normal);
    glVertexAttribPointer( attr.normal, 3, GL_FLOAT, GL_FALSE, 0, normal );

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

    glDisableVertexAttribArray( attr.vertex );
    glDisableVertexAttribArray( attr.texCoord );
    glDisableVertexAttribArray( attr.normal );

	delete [] normal;
	delete [] tex;
	delete [] vertex;
	delete [] indices;
}


void draw( GlesState& state, const class PolyLine<Vec2f> &polyLine )
{
    GlesAttr& attr = state.attr();

    glEnableVertexAttribArray(attr.vertex);
    glVertexAttribPointer( attr.vertex, 2, GL_FLOAT, GL_FALSE, 0, &(polyLine.getPoints()[0]) );
	glDrawArrays( ( polyLine.isClosed() ) ? GL_LINE_LOOP : GL_LINE_STRIP, 0, polyLine.size() );
    glDisableVertexAttribArray( attr.vertex );
}


void draw( GlesState& state, const class PolyLine<Vec3f> &polyLine )
{
    GlesAttr& attr = state.attr();

    glEnableVertexAttribArray(attr.vertex);
    glVertexAttribPointer( attr.vertex, 3, GL_FLOAT, GL_FALSE, 0, &(polyLine.getPoints()[0]) );
	glDrawArrays( ( polyLine.isClosed() ) ? GL_LINE_LOOP : GL_LINE_STRIP, 0, polyLine.size() );
    glDisableVertexAttribArray( attr.vertex );
}


void draw( GlesState& state, const class Path2d &path2d, float approximationScale )
{
    GlesAttr& attr = state.attr();

	if( path2d.getNumSegments() == 0 )
		return;
	std::vector<Vec2f> points = path2d.subdivide( approximationScale );
    glEnableVertexAttribArray(attr.vertex);
    glVertexAttribPointer( attr.vertex, 2, GL_FLOAT, GL_FALSE, 0, &(points[0]) );
	glDrawArrays( GL_LINE_STRIP, 0, points.size() );
    glDisableVertexAttribArray( attr.vertex );
}


void draw( GlesState& state, const class Shape2d &shape2d, float approximationScale )
{
    GlesAttr& attr = state.attr();

    glEnableVertexAttribArray(attr.vertex);
	for( std::vector<Path2d>::const_iterator contourIt = shape2d.getContours().begin(); contourIt != shape2d.getContours().end(); ++contourIt ) {
		if( contourIt->getNumSegments() == 0 )
			continue;
		std::vector<Vec2f> points = contourIt->subdivide( approximationScale );
        glVertexAttribPointer( attr.vertex, 2, GL_FLOAT, GL_FALSE, 0, &(points[0]) );
		glDrawArrays( GL_LINE_STRIP, 0, points.size() );
	}
    glDisableVertexAttribArray( attr.vertex );
}



void drawSolid( GlesState& state, const class Path2d &path2d, float approximationScale )
{
    GlesAttr& attr = state.attr();

	// if( path2d.getNumSegments() == 0 )
	// 	return;
	// std::vector<Vec2f> points = path2d.subdivide( approximationScale );
    // glEnableVertexAttribArray(attr.vertex);
    // glVertexAttribPointer( attr.vertex, 2, GL_FLOAT, GL_FALSE, 0, &(points[0]) );
	// glDrawArrays( GL_POLYGON, 0, points.size() );
    // glDisableVertexAttribArray( attr.vertex );
}



void draw( GlesState& state, const TriMesh &mesh )
{
    GlesAttr& attr = state.attr();

    glVertexAttribPointer( attr.vertex, 3, GL_FLOAT, GL_FALSE, 0, &(mesh.getVertices()[0]) );
    glEnableVertexAttribArray(attr.vertex);

	if( mesh.hasNormals() ) {
        glVertexAttribPointer( attr.normal, 3, GL_FLOAT, GL_FALSE, 0, &(mesh.getNormals()[0]) );
        glEnableVertexAttribArray(attr.normal);
	}
	else
		glDisableVertexAttribArray(attr.normal);
	
	if( mesh.hasColorsRGB() ) {
        
        glVertexAttribPointer( attr.color, 3, GL_UNSIGNED_BYTE, GL_FALSE, 0, &(mesh.getColorsRGB()[0]) );
        glEnableVertexAttribArray(attr.color);
	}
	else if( mesh.hasColorsRGBA() ) {
        glVertexAttribPointer( attr.color, 4, GL_UNSIGNED_BYTE, GL_FALSE, 0, &(mesh.getColorsRGBA()[0]) );
        glEnableVertexAttribArray(attr.color);
	}
	else 
        glDisableVertexAttribArray(attr.color);

	if( mesh.hasTexCoords() ) {
        glVertexAttribPointer( attr.texCoord, 2, GL_FLOAT, GL_FALSE, 0, &(mesh.getTexCoords()[0]) );
		glEnableVertexAttribArray( attr.texCoord );
	}
	else
		glDisableVertexAttribArray( attr.texCoord );
	glDrawElements( GL_TRIANGLES, mesh.getNumIndices(), GL_UNSIGNED_INT, &(mesh.getIndices()[0]) );

    glDisableVertexAttribArray( attr.vertex );
    glDisableVertexAttribArray( attr.normal );
    glDisableVertexAttribArray( attr.color );
    glDisableVertexAttribArray( attr.texCoord );
}


void drawRange( GlesState& state, const TriMesh &mesh, size_t startTriangle, size_t triangleCount )
{
    GlesAttr& attr = state.attr();

}


void draw( GlesState& state, const VboMesh &vbo )
{
    GlesAttr& attr = state.attr();

}


void drawRange( GlesState& state, const VboMesh &vbo, size_t startIndex, size_t indexCount, int vertexStart, int vertexEnd )
{
    GlesAttr& attr = state.attr();

}


void drawArrays( GlesState& state, const VboMesh &vbo, GLint first, GLsizei count )
{
    GlesAttr& attr = state.attr();

}



void drawBillboard( GlesState& state, const Vec3f &pos, const Vec2f &scale, float rotationDegrees, const Vec3f &bbRight, const Vec3f &bbUp )
{
    GlesAttr& attr = state.attr();

    glEnableVertexAttribArray(attr.vertex);
	Vec3f verts[4];
    glVertexAttribPointer( attr.vertex, 3, GL_FLOAT, GL_FALSE, 0, &verts[0] );
    glEnableVertexAttribArray(attr.texCoord);
	GLfloat texCoords[8] = { 0, 0, 0, 1, 1, 0, 1, 1 };
    glVertexAttribPointer( attr.texCoord, 2, GL_FLOAT, GL_FALSE, 0, texCoords );

	float sinA = math<float>::sin( toRadians( rotationDegrees ) );
	float cosA = math<float>::cos( toRadians( rotationDegrees ) );

	verts[0] = pos + bbRight * ( -0.5f * scale.x * cosA - 0.5f * sinA * scale.y ) + bbUp * ( -0.5f * scale.x * sinA + 0.5f * cosA * scale.y );
	verts[1] = pos + bbRight * ( -0.5f * scale.x * cosA - -0.5f * sinA * scale.y ) + bbUp * ( -0.5f * scale.x * sinA + -0.5f * cosA * scale.y );
	verts[2] = pos + bbRight * ( 0.5f * scale.x * cosA - 0.5f * sinA * scale.y ) + bbUp * ( 0.5f * scale.x * sinA + 0.5f * cosA * scale.y );
	verts[3] = pos + bbRight * ( 0.5f * scale.x * cosA - -0.5f * sinA * scale.y ) + bbUp * ( 0.5f * scale.x * sinA + -0.5f * cosA * scale.y );

	glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );

    glDisableVertexAttribArray( attr.vertex );
    glDisableVertexAttribArray( attr.texCoord );
}


void draw( GlesState& state, const Texture &texture )
{
	draw( state, texture, Area( texture.getCleanBounds() ), texture.getCleanBounds() );
}


void draw( GlesState& state, const Texture &texture, const Vec2f &pos )
{
	draw( state, texture, texture.getCleanBounds(), Rectf( pos.x, pos.y, pos.x + texture.getCleanWidth(), pos.y + texture.getCleanHeight() ) );
}


void draw( GlesState& state, const Texture &texture, const Rectf &rect )
{
	draw( state, texture, texture.getCleanBounds(), rect );
}


void draw( GlesState& state, const Texture &texture, const Area &srcArea, const Rectf &destRect )
{
    GlesAttr& attr = state.attr();

    // XXX save state?
	// SaveTextureBindState saveBindState( texture.getTarget() );
	// BoolState saveEnabledState( texture.getTarget() );
	// ClientBoolState vertexArrayState( GL_VERTEX_ARRAY );
	// ClientBoolState texCoordArrayState( GL_TEXTURE_COORD_ARRAY );	
	texture.enableAndBind();

    glEnableVertexAttribArray(attr.vertex);
	GLfloat verts[8];
    glVertexAttribPointer( attr.vertex, 2, GL_FLOAT, GL_FALSE, 0, verts );
    glEnableVertexAttribArray(attr.texCoord);
	GLfloat texCoords[8];
    glVertexAttribPointer( attr.texCoord, 2, GL_FLOAT, GL_FALSE, 0, texCoords );

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
    // XXX restore state?
}

} }


