#pragma once

/*  GLES2 equivalents to some of Cinder's GL helpers */

#include "cinder/Cinder.h"
#include "cinder/Vector.h"
#include "cinder/Color.h"
#include "cinder/Rect.h"
#include "cinder/PolyLine.h"
#include "cinder/AxisAlignedBox.h"
#include "cinder/Color.h"
#include "cinder/gl/Texture.h"
#include "cinder/gl/GlslProg.h"
#include "cinder/Matrix44.h"

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

//! Draws a line from \a start to \a end
void drawLine( const Vec2f &start, const Vec2f &end );
//! Draws a line from \a start to \a end
void drawLine( const Vec3f &start, const Vec3f &end );
//! Renders a solid cube centered at \a center of size \a size. Normals and created texture coordinates are generated for \c GL_TEXTURE_2D, with each face in the range [0,0] - [1.0,1.0]
void drawCube( const Vec3f &center, const Vec3f &size );
//! Renders a solid cube centered at \a center of size \a size. Each face is assigned a unique color, and no normals or texture coordinates are generated.
void drawColorCube( const Vec3f &center, const Vec3f &size );
//! Renders a stroked cube centered at \a center of size \a size.
void drawStrokedCube( const Vec3f &center, const Vec3f &size );
//! Renders a stroked cube \a aab
inline void drawStrokedCube( const AxisAlignedBox3f &aab ) { drawStrokedCube( aab.getCenter(), aab.getSize() ); }
//! Renders a solid sphere centered at \a center of radius \a radius. \a segments defines how many segments the sphere is subdivided into. Normals and texture coordinates in the range [0,1] are generated.
void drawSphere( const Vec3f &center, float radius, int segments = 12 );
//! Renders a solid sphere. \a segments defines how many segments the sphere is subdivided into. Normals and texture coordinates in the range [0,1] are generated.
void draw( const class Sphere &sphere, int segments = 12 );
//! Renders a solid circle using triangle fans. The default value of zero for \a numSegments automatically determines a number of segments based on the circle's circumference.
void drawSolidCircle( const Vec2f &center, float radius, int numSegments = 0 );
//! Renders a stroked circle using a line loop. The default value of zero for \a numSegments automatically determines a number of segments based on the circle's circumference.
void drawStrokedCircle( const Vec2f &center, float radius, int numSegments = 0 );
//! Renders a solid rectangle. Texture coordinates in the range [0,1] are generated unless \a textureRectangle.
void drawSolidRect( const Rectf &rect, bool textureRectangle = false );
//! Renders a stroked rectangle.
void drawStrokedRect( const Rectf &rect );
//! Renders a coordinate frame representation centered at the origin. Arrowheads are drawn at the end of each axis with radius \a headRadius and length \a headLength.
void drawCoordinateFrame( float axisLength = 1.0f, float headLength = 0.2f, float headRadius = 0.05f );
//! Draws a vector starting at \a start and ending at \a end. An arrowhead is drawn at the end of radius \a headRadius and length \a headLength.
void drawVector( const Vec3f &start, const Vec3f &end, float headLength = 0.2f, float headRadius = 0.05f );
//! Draws a wireframe representation of the frustum defined by \a cam.
void drawFrustum( const Camera &cam );
//! Draws a torus at the origin, with an outter radius \a outterRadius and an inner radius \a innerRadius, subdivided into \a longitudeSegments and \a latitudeSegments. Normals and texture coordinates in the range [0,1] are generated.
void drawTorus( float outterRadius, float innerRadius, int longitudeSegments = 12, int latitudeSegments = 12 );
//! Draws a open-ended cylinder, with base radius \a baseRadius and top radius \a topRadius, with height \a height, subdivided into \a slices and \a stacks. Normals and texture coordinates in the range [0,1] are generated.
void drawCylinder( float baseRadius, float topRadius, float height, int slices = 12, int stacks = 1 );
//! Draws a 2d PolyLine \a polyLine
// void draw( const class PolyLine<Vec2f> &polyLine );
//! Draws a 3d PolyLine \a polyLine
void draw( const class PolyLine<Vec3f> &polyLine );
//! Draws a Path2d \a path2d using approximation scale \a approximationScale. 1.0 corresponds to screenspace, 2.0 is double screen resolution, etc
void draw( const class Path2d &path2d, float approximationScale = 1.0f );
//! Draws a Shape2d \a shape2d using approximation scale \a approximationScale. 1.0 corresponds to screenspace, 2.0 is double screen resolution, etc
void draw( const class Shape2d &shape2d, float approximationScale = 1.0f );

//! Draws a solid (filled) Path2d \a path2d using approximation scale \a approximationScale. 1.0 corresponds to screenspace, 2.0 is double screen resolution, etc
// void drawSolid( const class Path2d &path2d, float approximationScale = 1.0f );

//! Draws a cinder::TriMesh \a mesh at the origin.
void draw( const TriMesh &mesh );
//! Draws a range of triangles starting with triangle # \a startTriangle and a count of \a triangleCount from cinder::TriMesh \a mesh at the origin.
void drawRange( const TriMesh &mesh, size_t startTriangle, size_t triangleCount );
//! Draws a cinder::gl::VboMesh \a mesh at the origin.
void draw( const VboMesh &vbo );
//! Draws a range of vertices and elements of cinder::gl::VboMesh \a mesh at the origin. Default parameters for \a vertexStart and \a vertexEnd imply the VboMesh's full range of vertices.
void drawRange( const VboMesh &vbo, size_t startIndex, size_t indexCount, int vertexStart = -1, int vertexEnd = -1 );
//! Draws a range of elements from a cinder::gl::VboMesh \a vbo.
void drawArrays( const VboMesh &vbo, GLint first, GLsizei count );
//!	Draws a textured quad of size \a scale that is aligned with the vectors \a bbRight and \a bbUp at \a pos, rotated by \a rotationDegrees around the vector orthogonal to \a bbRight and \a bbUp.
void drawBillboard( const Vec3f &pos, const Vec2f &scale, float rotationDegrees, const Vec3f &bbRight, const Vec3f &bbUp );
//! Draws \a texture on the XY-plane
void draw( const Texture &texture );
//! Draws \a texture on the XY-plane at \a pos
void draw( const Texture &texture, const Vec2f &pos );
//! Draws \a texture on the XY-plane in the rectangle defined by \a rect
void draw( const Texture &texture, const Rectf &rect );
//! Draws the pixels inside \a srcArea of \a texture on the XY-plane in the rectangle defined by \a destRect
void draw( const Texture &texture, const Area &srcArea, const Rectf &destRect );

//  Attributes used by the draw* methods
enum ShaderAttrs
{
    ES2_ATTR_VERTEX   = 1 << 0,
    ES2_ATTR_TEXCOORD = 1 << 1,
    ES2_ATTR_COLOR    = 1 << 2,
    ES2_ATTR_NORMAL   = 1 << 3,
};

class SelectAttrCallback {
public:
    virtual void selectAttrs(uint32_t activeAttrs) = 0; 
};

//  Shader attributes and draw methods that use them
struct GlesAttr 
{
    GLuint mVertex;
    GLuint mTexCoord;
    GLuint mColor;
    GLuint mNormal;

    SelectAttrCallback* mSelectAttr;

    GlesAttr(GLuint vertex=0, GLuint texCoord=0, GLuint color=0, GLuint normal=0);
    inline void selectAttrs(uint32_t activeAttrs) { if ( mSelectAttr ) mSelectAttr->selectAttrs( activeAttrs ); }

    void drawLine( const Vec2f &start, const Vec2f &end );
    void drawLine( const Vec3f &start, const Vec3f &end );
    void drawCube( const Vec3f &center, const Vec3f &size );
    void drawColorCube( const Vec3f &center, const Vec3f &size );
    void drawStrokedCube( const Vec3f &center, const Vec3f &size );
    inline void drawStrokedCube( const AxisAlignedBox3f &aab ) { drawStrokedCube( aab.getCenter(), aab.getSize() ); }
    void drawSphere( const Vec3f &center, float radius, int segments = 12 );
    void draw( const class Sphere &sphere, int segments = 12 );
    void drawSolidCircle( const Vec2f &center, float radius, int numSegments = 0 );
    void drawStrokedCircle( const Vec2f &center, float radius, int numSegments = 0 );
    void drawSolidRect( const Rectf &rect, bool textureRectangle = false );
    void drawStrokedRect( const Rectf &rect );
    void drawCoordinateFrame( float axisLength = 1.0f, float headLength = 0.2f, float headRadius = 0.05f );
    void drawVector( const Vec3f &start, const Vec3f &end, float headLength = 0.2f, float headRadius = 0.05f );
    void drawFrustum( const Camera &cam );
    void drawTorus( float outterRadius, float innerRadius, int longitudeSegments = 12, int latitudeSegments = 12 );
    void drawCylinder( float baseRadius, float topRadius, float height, int slices = 12, int stacks = 1 );
    // void draw( const class PolyLine<Vec2f> &polyLine );
    void draw( const class PolyLine<Vec3f> &polyLine );
    void draw( const class Path2d &path2d, float approximationScale = 1.0f );
    void draw( const class Shape2d &shape2d, float approximationScale = 1.0f );

    void drawSolid( const class Path2d &path2d, float approximationScale = 1.0f );

    void draw( const TriMesh &mesh );
    void drawRange( const TriMesh &mesh, size_t startTriangle, size_t triangleCount );
    void draw( const VboMesh &vbo );
    void drawRange( const VboMesh &vbo, size_t startIndex, size_t indexCount, int vertexStart = -1, int vertexEnd = -1 );
    void drawArrays( const VboMesh &vbo, GLint first, GLsizei count );
    void drawBillboard( const Vec3f &pos, const Vec2f &scale, float rotationDegrees, const Vec3f &bbRight, const Vec3f &bbUp );
    void draw( const Texture &texture );
    void draw( const Texture &texture, const Vec2f &pos );
    void draw( const Texture &texture, const Rectf &rect );
    void draw( const Texture &texture, const Area &srcArea, const Rectf &destRect );
};

//  Emulates a GL context
class GlesContext : public SelectAttrCallback
{
public:
    GlesContext();
    GlesContext(GlslProg shader);

    void bind();
    void unbind();

    //  SelectAttrCallback implementation
    virtual void selectAttrs(uint32_t activeAttrs);

    GlesAttr& attr();

    void setMatrices( const Camera &cam );
    void setModelView( const Camera &cam );
    void setProjection( const Camera &cam );
    // void pushModelView();
    // void popModelView();
    // void pushModelView( const Camera &cam );
    // void pushProjection( const Camera &cam );
    // void pushMatrices();
    // void popMatrices();
    // void multModelView( const Matrix44f &mtx );
    // void multProjection( const Matrix44f &mtx );
    // Matrix44f getModelView();
    // Matrix44f getProjection();

    // void setMatricesWindowPersp( int screenWidth, int screenHeight, float fovDegrees = 60.0f, float nearPlane = 1.0f, float farPlane = 1000.0f, bool originUpperLeft = true );
    // inline void setMatricesWindowPersp( const Vec2i &screenSize, float fovDegrees = 60.0f, float nearPlane = 1.0f, float farPlane = 1000.0f, bool originUpperLeft = true )
    // { setMatricesWindowPersp( screenSize.x, screenSize.y, fovDegrees, nearPlane, farPlane ); }
    void setMatricesWindow( int screenWidth, int screenHeight, bool originUpperLeft = true );
    inline void setMatricesWindow( const Vec2i &screenSize, bool originUpperLeft = true ) { setMatricesWindow( screenSize.x, screenSize.y, originUpperLeft ); }

    // void translate( const Vec2f &pos );
    // inline void translate( float x, float y ) { translate( Vec2f( x, y ) ); }
    // void translate( const Vec3f &pos );
    // inline void translate( float x, float y, float z ) { translate( Vec3f( x, y, z ) ); }

    // void scale( const Vec3f &scl );
    // inline void scale( const Vec2f &scl ) { scale( Vec3f( scl.x, scl.y, 0 ) ); }
    // inline void scale( float x, float y ) { scale( Vec3f( x, y, 0 ) ); }
    // inline void scale( float x, float y, float z ) { scale( Vec3f( x, y, z ) ); }

    // void rotate( const Vec3f &xyz );
    // void rotate( const Quatf &quat );
    // inline void rotate( float degrees ) { rotate( Vec3f( 0, 0, degrees ) ); }

    void color( float r, float g, float b );
    void color( float r, float g, float b, float a );
    // void color( const Color8u &c );
    // void color( const ColorA8u &c );
    void color( const Color &c );
    void color( const ColorA &c );

protected:
    void updateUniforms();

    //  Shader program
    GlslProg mProg;

    //  Shader attributes
    GlesAttr mAttr;

    //  Uniforms
    Matrix44f mProj;
    Matrix44f mModelView;
    ColorA    mColor;
    Texture   mTexture;
    uint32_t  mActiveAttrs;

    bool mBound;

    bool mProjDirty;
    bool mModelViewDirty;
    bool mColorDirty;
    bool mTextureDirty;
    bool mActiveAttrsDirty;
};

} }

