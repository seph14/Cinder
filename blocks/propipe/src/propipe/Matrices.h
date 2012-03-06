#include "cinder/Camera.h"
#include "cinder/Matrix.h"
#include <deque>

namespace cinder { namespace pp {

class MatrixStack
{
public:
	MatrixStack();
	~MatrixStack();
	void setMatrix(const Matrix44f &mtx);
	void pushMatrix();
	void pushMatrix(const Matrix44f &mtx);
	void multMatrix(const Matrix44f &mtx);
	void popMatrix();

	size_t size();
	bool empty();

	Matrix44f& getMatrix();

protected:
	std::deque<Matrix44f> mStack;
	Matrix44f mMatrix;
};

class Matrices
{
public:
	void setMatrices( const Camera &cam );
	void setModelView( const Camera &cam );
	void setModelView( const Matrix44f &modelview );
	void setProjection( const Camera &cam );
	void setProjection( const Matrix44f &proj );
	void pushModelView();
	void pushModelView( const Camera &cam );
	void popModelView();
	void pushProjection();
	void pushProjection( const Camera &cam );
	void popProjection();
	void pushMatrices();
	void popMatrices();
	void multModelView( const Matrix44f &mtx );
	void multProjection( const Matrix44f &mtx );

	Matrix44f getModelView();
	Matrix44f getProjection();

	//! Produces a translation by \a pos in the modelview matrix.
	void translate( const Vec2f &pos );
	//! Produces a translation by \a x and \a y in the modelview matrix.
	inline void translate( float x, float y ) { translate( Vec2f( x, y ) ); }
	//! Produces a translation by \a pos in the modelview matrix.
	void translate( const Vec3f &pos );
	//! Produces a translation by \a x, \a y and \a z in the modelview matrix.
	inline void translate( float x, float y, float z ) { translate( Vec3f( x, y, z ) ); }

	//! Produces a scale by \a scale in the modelview matrix.
	void scale( const Vec3f &scl );
	//! Produces a scale by \a scl in the modelview matrix.
	inline void scale( const Vec2f &scl ) { scale( Vec3f( scl.x, scl.y, 0 ) ); }
	//! Produces a scale by \a x and \a y in the modelview matrix.
	inline void scale( float x, float y ) { scale( Vec3f( x, y, 0 ) ); }
	//! Produces a scale by \a x, \a y and \a z in the modelview matrix.
	inline void scale( float x, float y, float z ) { scale( Vec3f( x, y, z ) ); }

	//! Produces a rotation around the X-axis by \a xyz.x degrees, the Y-axis by \a xyz.y degrees and the Z-axis by \a xyz.z degrees in the modelview matrix. Processed in X-Y-Z order.
	void rotate( const Vec3f &xyz );
	//! Produces a rotation by the quaternion \a quat in the modelview matrix.
	void rotate( const Quatf &quat );
	//! Produces a 2D rotation, the equivalent of a rotation around the Z axis by \a degrees.
	inline void rotate( float degrees ) { rotate( Vec3f( 0, 0, degrees ) ); }

	void setMatricesWindowPersp( int screenWidth, int screenHeight, float fovDegrees = 60.0f, float nearPlane = 1.0f, float farPlane = 1000.0f, bool originUpperLeft = true );
	inline void setMatricesWindowPersp( const Vec2i &screenSize, float fovDegrees = 60.0f, float nearPlane = 1.0f, float farPlane = 1000.0f, bool originUpperLeft = true )
	{ setMatricesWindowPersp( screenSize.x, screenSize.y, fovDegrees, nearPlane, farPlane ); }
	void setMatricesWindow( int screenWidth, int screenHeight, bool originUpperLeft = true );
	inline void setMatricesWindow( const Vec2i &screenSize, bool originUpperLeft = true ) { setMatricesWindow( screenSize.x, screenSize.y, originUpperLeft ); }

protected:
	MatrixStack mModelView;
	MatrixStack mProjection;
};

} }
