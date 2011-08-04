#include "cinder/gl/gles2.h"
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

namespace cinder { namespace gl {

void drawLine( GLuint attr, const Vec2f &start, const Vec2f &end )
{
}


void drawLine( GLuint attr, const Vec3f &start, const Vec3f &end )
{
}


void drawCube( GLuint attr, const Vec3f &center, const Vec3f &size )
{
}


void drawColorCube( GLuint attr, const Vec3f &center, const Vec3f &size )
{
}


void drawStrokedCube( GLuint attr, const Vec3f &center, const Vec3f &size )
{
}


void drawSphere( GLuint attr, const Vec3f &center, float radius, int segments )
{
}


void draw( GLuint attr, const class Sphere &sphere, int segments )
{
}


void drawSolidCircle( GLuint attr, const Vec2f &center, float radius, int numSegments )
{
}


void drawStrokedCircle( GLuint attr, const Vec2f &center, float radius, int numSegments )
{
}


void drawSolidRect( GLuint attr, const Rectf &rect, bool textureRectangle )
{
}


void drawStrokedRect( GLuint attr, const Rectf &rect )
{
}


void drawCoordinateFrame( GLuint attr, float axisLength, float headLength, float headRadius )
{
}


void drawVector( GLuint attr, const Vec3f &start, const Vec3f &end, float headLength, float headRadius )
{
}


void drawFrustum( GLuint attr, const Camera &cam )
{
}


void drawTorus( GLuint attr, float outterRadius, float innerRadius, int longitudeSegments, int latitudeSegments )
{
}


void drawCylinder( GLuint attr, float baseRadius, float topRadius, float height, int slices, int stacks )
{
}


void draw( GLuint attr, const class PolyLine<Vec2f> &polyLine )
{
}


void draw( GLuint attr, const class PolyLine<Vec3f> &polyLine )
{
}


void draw( GLuint attr, const class Path2d &path2d, float approximationScale )
{
}


void draw( GLuint attr, const class Shape2d &shape2d, float approximationScale )
{
}



void drawSolid( GLuint attr, const class Path2d &path2d, float approximationScale )
{
}



void draw( GLuint attr, const TriMesh &mesh )
{
}


void drawRange( GLuint attr, const TriMesh &mesh, size_t startTriangle, size_t triangleCount )
{
}


void draw( GLuint attr, const VboMesh &vbo )
{
}


void drawRange( GLuint attr, const VboMesh &vbo, size_t startIndex, size_t indexCount, int vertexStart, int vertexEnd )
{
}


void drawArrays( GLuint attr, const VboMesh &vbo, GLint first, GLsizei count )
{
}



void drawBillboard( GLuint attr, const Vec3f &pos, const Vec2f &scale, float rotationDegrees, const Vec3f &bbRight, const Vec3f &bbUp )
{
}


void draw( GLuint attr, const Texture &texture )
{
}


void draw( GLuint attr, const Texture &texture, const Vec2f &pos )
{
}


void draw( GLuint attr, const Texture &texture, const Rectf &rect )
{
}


void draw( GLuint attr, const Texture &texture, const Area &srcArea, const Rectf &destRect )
{
}

} }


