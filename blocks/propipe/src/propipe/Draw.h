#pragma once

#include "cinder/Cinder.h"

#include "cinder/gl/Texture.h"
#include "cinder/gl/Vbo.h"
#include "cinder/AxisAlignedBox.h"
#include "cinder/Camera.h"
#include "cinder/Color.h"
#include "cinder/Matrix.h"
#include "cinder/Rect.h"
#include "cinder/Sphere.h"
#include "cinder/TriMesh.h"
#include "cinder/Vector.h"


namespace cinder { namespace pp {

typedef std::shared_ptr<class Renderer> RendererRef;
typedef std::shared_ptr<class Draw> DrawRef;

class Draw
{
public:
    static DrawRef create(RendererRef renderer);

    void bind();
    void unbind();

    void drawLine( const Vec2f &start, const Vec2f &end );
    void drawLine( const Vec3f &start, const Vec3f &end );
    void drawCube( const Vec3f &center, const Vec3f &size, bool textured = false );
    void drawColorCube( const Vec3f &center, const Vec3f &size, bool textured = false );
    void drawStrokedCube( const Vec3f &center, const Vec3f &size );
    inline void drawStrokedCube( const AxisAlignedBox3f &aab ) { drawStrokedCube( aab.getCenter(), aab.getSize() ); }
    void drawSphere( const Vec3f &center, float radius, int segments = 12, bool textured = false );
    void draw( const class Sphere &sphere, int segments = 12, bool textured = false );
    void drawSolidCircle( const Vec2f &center, float radius, int numSegments = 0 );
    void drawStrokedCircle( const Vec2f &center, float radius, int numSegments = 0 );
    void drawSolidRect( const Rectf &rect, bool textureRectangle = false );
    void drawStrokedRect( const Rectf &rect );
    void drawCoordinateFrame( float axisLength = 1.0f, float headLength = 0.2f, float headRadius = 0.05f );
    void drawVector( const Vec3f &start, const Vec3f &end, float headLength = 0.2f, float headRadius = 0.05f );
    void drawFrustum( const Camera &cam );
    void drawTorus( float outterRadius, float innerRadius, int longitudeSegments = 12, int latitudeSegments = 12, bool textured = false );
    void drawCylinder( float baseRadius, float topRadius, float height, int slices = 12, int stacks = 1, bool textured = false );
    // void draw( const class PolyLine<Vec2f> &polyLine );
    void draw( const class PolyLine<Vec3f> &polyLine );
    void draw( const class Path2d &path2d, float approximationScale = 1.0f );
    void draw( const class Shape2d &shape2d, float approximationScale = 1.0f );

    void drawSolid( const class Path2d &path2d, float approximationScale = 1.0f );

    void draw( const TriMesh &mesh );
    void drawRange( const TriMesh &mesh, size_t startTriangle, size_t triangleCount );
    void draw( const gl::VboMesh &vbo );
    void drawRange( const gl::VboMesh &vbo, size_t startIndex, size_t indexCount, int vertexStart = -1, int vertexEnd = -1 );
    void drawArrays( const gl::VboMesh &vbo, GLint first, GLsizei count );
    void drawBillboard( const Vec3f &pos, const Vec2f &scale, float rotationDegrees, const Vec3f &bbRight, const Vec3f &bbUp );
    void draw( const gl::Texture &texture );
    void draw( const gl::Texture &texture, const Vec2f &pos );
    void draw( const gl::Texture &texture, const Rectf &rect );
    void draw( const gl::Texture &texture, const Area &srcArea, const Rectf &destRect );

    virtual ~Draw();

protected:
    Draw(RendererRef renderer);

    RendererRef mRenderer;
};

} }
