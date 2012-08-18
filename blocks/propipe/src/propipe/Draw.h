#pragma once

#include "propipe/DrawShader.h"

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

typedef std::shared_ptr<class Draw> DrawRef;

class Draw
{
  public:
	static DrawRef create(DrawShaderRef shader);

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
	//! Renders a solid ellipse using triangle fans. The default value of zero for \a numSegments automatically determines a number of segments based on the ellipse's circumference.
	void drawSolidEllipse( const Vec2f &center, float radiusX, float radiusY, int numSegments = 0 );
	//! Renders a stroked circle using a line loop. The default value of zero for \a numSegments automatically determines a number of segments based on the circle's circumference.
	void drawStrokedEllipse( const Vec2f &center, float radiusX, float radiusY, int numSegments = 0 );
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
	void draw( const class PolyLine<Vec2f> &polyLine );
	//! Draws a 3d PolyLine \a polyLine
	void draw( const class PolyLine<Vec3f> &polyLine );
	//! Draws a Path2d \a path2d using approximation scale \a approximationScale. 1.0 corresponds to screenspace, 2.0 is double screen resolution, etc
	void draw( const class Path2d &path2d, float approximationScale = 1.0f );
	//! Draws a Shape2d \a shape2d using approximation scale \a approximationScale. 1.0 corresponds to screenspace, 2.0 is double screen resolution, etc
	void draw( const class Shape2d &shape2d, float approximationScale = 1.0f );

	//! Draws a solid (filled) Path2d \a path2d using approximation scale \a approximationScale. 1.0 corresponds to screenspace, 2.0 is double screen resolution, etc. Performance warning: This routine tesselates the polygon into triangles. Consider using Triangulator directly.
	void drawSolid( const class Path2d &path2d, float approximationScale = 1.0f );
	//! Draws a solid (filled) Shape2d \a shape2d using approximation scale \a approximationScale. 1.0 corresponds to screenspace, 2.0 is double screen resolution, etc. Performance warning: This routine tesselates the polygon into triangles. Consider using Triangulator directly.
	void drawSolid( const class Shape2d &shape2d, float approximationScale = 1.0f );
	//! Draws a solid (filled) PolyLine2f \a polyLine. Performance warning: This routine tesselates the polygon into triangles. Consider using Triangulator directly.
	void drawSolid( const PolyLine2f &polyLine );

	//! Draws a cinder::TriMesh \a mesh at the origin.
	void draw( const TriMesh2d &mesh );
	//! Draws a range of triangles starting with triangle # \a startTriangle and a count of \a triangleCount from cinder::TriMesh \a mesh at the origin.
	// void drawRange( const TriMesh2d &mesh, size_t startTriangle, size_t triangleCount );
	//! Draws a cinder::TriMesh \a mesh at the origin.
	void draw( const TriMesh &mesh );
	//! Draws a range of triangles starting with triangle # \a startTriangle and a count of \a triangleCount from cinder::TriMesh \a mesh at the origin.
	// void drawRange( const TriMesh &mesh, size_t startTriangle, size_t triangleCount );
	//! Draws a cinder::gl::VboMesh \a mesh at the origin.
	void draw( const gl::VboMesh &vbo );
	//! Draws a range of vertices and elements of cinder::gl::VboMesh \a mesh at the origin. Default parameters for \a vertexStart and \a vertexEnd imply the VboMesh's full range of vertices.
	// void drawRange( const gl::VboMesh &vbo, size_t startIndex, size_t indexCount, int vertexStart = -1, int vertexEnd = -1 );
	//! Draws a range of elements from a cinder::gl::VboMesh \a vbo.
	// void drawArrays( const gl::VboMesh &vbo, GLint first, GLsizei count );
	//!	Draws a textured quad of size \a scale that is aligned with the vectors \a bbRight and \a bbUp at \a pos, rotated by \a rotationDegrees around the vector orthogonal to \a bbRight and \a bbUp.
	void drawBillboard( const Vec3f &pos, const Vec2f &scale, float rotationDegrees, const Vec3f &bbRight, const Vec3f &bbUp );
	//! Draws \a texture on the XY-plane
	void draw( const gl::Texture &texture );
	//! Draws \a texture on the XY-plane at \a pos
	void draw( const gl::Texture &texture, const Vec2f &pos );
	//! Draws \a texture on the XY-plane in the rectangle defined by \a rect
	void draw( const gl::Texture &texture, const Rectf &rect );
	//! Draws the pixels inside \a srcArea of \a texture on the XY-plane in the rectangle defined by \a destRect
	void draw( const gl::Texture &texture, const Area &srcArea, const Rectf &destRect );

	virtual ~Draw();

  protected:
	Draw(DrawShaderRef shader);
   DrawShaderRef mShader;
};

} }
