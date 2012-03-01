#pragma once

#include <deque>
#include <cinder/Matrix.h>

/** Helper classes for working with the programmable pipeline (OpenGL 2.x, 3.x, 4.x, ES2) */

namespace pp {

class GLDraw
{
   virtual void drawBuffer(GeoBuf& buffer);
};

/*  Base drawing class */
class Draw2D
{
public:
    virtual void drawLine(const Vec2f& start, const Vec2f& end);
    virtual void drawSolidRect();
    virtual void drawTriMesh();
}; 

class Draw3D
{
public:
    virtual void drawLine(const Vec3f& start, const Vec3f& end);
};


enum GeoFlags
{
    GEOM_INDICES   = 1 << 0,
    GEOM_POSITIONS = 1 << 1,
    GEOM_NORMALS   = 1 << 2,
    GEOM_TEXCOORDS = 1 << 3,
};

struct GeoBuf2D
{
    std::vector<ci::gl::index_t> indices;
    std::vector<ci::Vec2f> positions;
    std::vector<ci::Vec2f> texCoords;

    /*  Transform positions with a given matrix */
    void transform(const Matrix44f& xform);
    void clear();
};

struct GeoBuf3D
{
    std::vector<ci::gl::index_t> indices;
    std::vector<ci::Vec3f> positions;
    std::vector<ci::Vec3f> normals;
    std::vector<ci::Vec2f> texCoords;

    /*  Transform positions and normals with a given matrix */
    void transform(const Matrix44f& xform);
    void clear();
}

/**
GeoBuf2D buf;
Geo2D(buf, GEO_POSITIONS | GEO_TEXCOORDS).line(x, y);

GeoBuf3D buf;
Geo3D geo(buf, GEO_POSITIONS | GEO_NORMALS);
for () {
    geo.line(x, y);
}
*/

class Geo2D
{
public:
    Geo2D(GeoBuf2D& buf, GeoFlags flags);
    ~Geo2D();

    void line( const ci::Vec2f& start, const ci::Vec2f& end );
    void solidRect( const ci::Rectf& rect, bool isQuad = true );
    void solidRect( const ci::Rectf& rect, const ci::Rectf& texCoords );
    void strokedRect();
    void solidCircle( const ci::Vec2f &center, float radius, int numSegments = 0 );
    void ellipse( const ci::Vec2f &center, float radiusX, float radiusY, int numSegments = 0 );

    GeoBuf2D& buffer();
    GeoFlags  flags();

protected:
    GeoBuf2D& mBuffer;
    GeoFlags  mFlags;
};

class Geo3D
{
public:
    Geo3D(GeoFlags flags);
    ~Geo3D();

    int  line( const ci::Vec3f& start, const ci::Vec3f& end );
    int  solidRect( const ci::Rectf& rect );
    void circle( const ci::Vec3f &center, float radius, int numSegments = 0 );
    void ellipse( const ci::Vec3f &center, float radiusX, float radiusY, int numSegments = 0 );
    int  cube( const ci::Vec3f& center, const ci::Vec3f& size );
    int  sphere( const ci::Vec3f& center, float radius, int segments = 12 );
    int  torus( float outerRadius, float innerRadius, int longitudeSegments = 12, int latitudeSegments = 12 );

    GeoBuf3D& buffer();

protected:
    GeoBuf3D& mBuffer;
    GeoFlags  mFlags;
}

class MatrixStack
{
    std::deque<Matrix44f> mStack;
    Matrix44f mBack;

    void push(const Matrix44f& matrix);
    void pop();

    Matrix44f& back();
};

}
