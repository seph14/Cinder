#pragma once

/*  GLES2 equivalents to some of Cinder's GL helpers */

#include "cinder/Cinder.h"
#include "cinder/Vector.h"
#include "cinder/Color.h"
#include "cinder/Rect.h"
#include "cinder/PolyLine.h"
#include "cinder/AxisAlignedBox.h"
#include "cinder/Color.h"

#if defined( CINDER_COCOA_TOUCH )
	#include <OpenGLES/ES2/gl.h>
	#include <OpenGLES/ES2/glext.h>
#elif defined( CINDER_ANDROID )
	#define GL_GLEXT_PROTOTYPES
	#include <GLES2/gl2.h>
	#include <GLES2/gl2ext.h>
#endif

// forward declarations
namespace cinder {
	class Camera; class TriMesh; class Sphere; 
    class Path2d; class Shape2d;
	namespace gl {
		 class VboMesh; class Texture; class GlslProg;
	}
} // namespace cinder

namespace cinder { namespace gl {

struct GlesAttr 
{
    GLuint vertex;
    GLuint texCoord;
    GLuint color;
    GLuint normal;

    GlesAttr();
};

struct GlesState
{
    GlesState();
    GlesState(GlslProg& shader);

    inline GlesAttr& attr() const { return mObj->attr; }

protected:
    struct Obj {
        GlesAttr attr;

        ColorA   color;

        Obj();
        ~Obj();
    };
    std::shared_ptr<Obj>    mObj;
};

//!  Returns a built-in shader that emulates a subset of the fixed function pipeline
GlslProg fixedFunctionShader();

//!  Return a GlesState struct from one of the built-in shaders
GlesState shaderAttrs(GlslProg& shader);

//! Draws a line from \a start to \a end
void drawLine( GlesState& state, const Vec2f &start, const Vec2f &end );
//! Draws a line from \a start to \a end
void drawLine( GlesState& state, const Vec3f &start, const Vec3f &end );
//! Renders a solid cube centered at \a center of size \a size. Normals and created texture coordinates are generated for \c GL_TEXTURE_2D, with each face in the range [0,0] - [1.0,1.0]
void drawCube( GlesState& state, const Vec3f &center, const Vec3f &size );
//! Renders a solid cube centered at \a center of size \a size. Each face is assigned a unique color, and no normals or texture coordinates are generated.
void drawColorCube( GlesState& state, const Vec3f &center, const Vec3f &size );
//! Renders a stroked cube centered at \a center of size \a size.
void drawStrokedCube( GlesState& state, const Vec3f &center, const Vec3f &size );
//! Renders a stroked cube \a aab
inline void drawStrokedCube( GlesState& state, const AxisAlignedBox3f &aab ) { drawStrokedCube( state, aab.getCenter(), aab.getSize() ); }
//! Renders a solid sphere centered at \a center of radius \a radius. \a segments defines how many segments the sphere is subdivided into. Normals and texture coordinates in the range [0,1] are generated.
void drawSphere( GlesState& state, const Vec3f &center, float radius, int segments = 12 );
//! Renders a solid sphere. \a segments defines how many segments the sphere is subdivided into. Normals and texture coordinates in the range [0,1] are generated.
void draw( GlesState& state, const class Sphere &sphere, int segments = 12 );
//! Renders a solid circle using triangle fans. The default value of zero for \a numSegments automatically determines a number of segments based on the circle's circumference.
void drawSolidCircle( GlesState& state, const Vec2f &center, float radius, int numSegments = 0 );
//! Renders a stroked circle using a line loop. The default value of zero for \a numSegments automatically determines a number of segments based on the circle's circumference.
void drawStrokedCircle( GlesState& state, const Vec2f &center, float radius, int numSegments = 0 );
//! Renders a solid rectangle. Texture coordinates in the range [0,1] are generated unless \a textureRectangle.
void drawSolidRect( GlesState& state, const Rectf &rect, bool textureRectangle = false );
//! Renders a stroked rectangle.
void drawStrokedRect( GlesState& state, const Rectf &rect );
//! Renders a coordinate frame representation centered at the origin. Arrowheads are drawn at the end of each axis with radius \a headRadius and length \a headLength.
void drawCoordinateFrame( GlesState& state, float axisLength = 1.0f, float headLength = 0.2f, float headRadius = 0.05f );
//! Draws a vector starting at \a start and ending at \a end. An arrowhead is drawn at the end of radius \a headRadius and length \a headLength.
void drawVector( GlesState& state, const Vec3f &start, const Vec3f &end, float headLength = 0.2f, float headRadius = 0.05f );
//! Draws a wireframe representation of the frustum defined by \a cam.
void drawFrustum( GlesState& state, const Camera &cam );
//! Draws a torus at the origin, with an outter radius \a outterRadius and an inner radius \a innerRadius, subdivided into \a longitudeSegments and \a latitudeSegments. Normals and texture coordinates in the range [0,1] are generated.
void drawTorus( GlesState& state, float outterRadius, float innerRadius, int longitudeSegments = 12, int latitudeSegments = 12 );
//! Draws a open-ended cylinder, with base radius \a baseRadius and top radius \a topRadius, with height \a height, subdivided into \a slices and \a stacks. Normals and texture coordinates in the range [0,1] are generated.
void drawCylinder( GlesState& state, float baseRadius, float topRadius, float height, int slices = 12, int stacks = 1 );
//! Draws a 2d PolyLine \a polyLine
void draw( GlesState& state, const class PolyLine<Vec2f> &polyLine );
//! Draws a 3d PolyLine \a polyLine
void draw( GlesState& state, const class PolyLine<Vec3f> &polyLine );
//! Draws a Path2d \a path2d using approximation scale \a approximationScale. 1.0 corresponds to screenspace, 2.0 is double screen resolution, etc
void draw( GlesState& state, const class Path2d &path2d, float approximationScale = 1.0f );
//! Draws a Shape2d \a shape2d using approximation scale \a approximationScale. 1.0 corresponds to screenspace, 2.0 is double screen resolution, etc
void draw( GlesState& state, const class Shape2d &shape2d, float approximationScale = 1.0f );

//! Draws a solid (filled) Path2d \a path2d using approximation scale \a approximationScale. 1.0 corresponds to screenspace, 2.0 is double screen resolution, etc
void drawSolid( GlesState& state, const class Path2d &path2d, float approximationScale = 1.0f );

//! Draws a cinder::TriMesh \a mesh at the origin.
void draw( GlesState& state, const TriMesh &mesh );
//! Draws a range of triangles starting with triangle # \a startTriangle and a count of \a triangleCount from cinder::TriMesh \a mesh at the origin.
void drawRange( GlesState& state, const TriMesh &mesh, size_t startTriangle, size_t triangleCount );
//! Draws a cinder::gl::VboMesh \a mesh at the origin.
void draw( GlesState& state, const VboMesh &vbo );
//! Draws a range of vertices and elements of cinder::gl::VboMesh \a mesh at the origin. Default parameters for \a vertexStart and \a vertexEnd imply the VboMesh's full range of vertices.
void drawRange( GlesState& state, const VboMesh &vbo, size_t startIndex, size_t indexCount, int vertexStart = -1, int vertexEnd = -1 );
//! Draws a range of elements from a cinder::gl::VboMesh \a vbo.
void drawArrays( GlesState& state, const VboMesh &vbo, GLint first, GLsizei count );
//!	Draws a textured quad of size \a scale that is aligned with the vectors \a bbRight and \a bbUp at \a pos, rotated by \a rotationDegrees around the vector orthogonal to \a bbRight and \a bbUp.
void drawBillboard( GlesState& state, const Vec3f &pos, const Vec2f &scale, float rotationDegrees, const Vec3f &bbRight, const Vec3f &bbUp );
//! Draws \a texture on the XY-plane
void draw( GlesState& state, const Texture &texture );
//! Draws \a texture on the XY-plane at \a pos
void draw( GlesState& state, const Texture &texture, const Vec2f &pos );
//! Draws \a texture on the XY-plane in the rectangle defined by \a rect
void draw( GlesState& state, const Texture &texture, const Rectf &rect );
//! Draws the pixels inside \a srcArea of \a texture on the XY-plane in the rectangle defined by \a destRect
void draw( GlesState& state, const Texture &texture, const Area &srcArea, const Rectf &destRect );

} }

