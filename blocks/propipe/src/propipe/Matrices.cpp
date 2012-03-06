#include "Matrices.h"

#include "cinder/CinderMath.h"
#include "cinder/gl/gl.h"

namespace cinder { namespace pp {

MatrixStack::MatrixStack() : mMatrix(Matrix44f::identity())
{
    mStack.push_back(Matrix44f::identity());
}

MatrixStack::~MatrixStack()
{
}

void MatrixStack::setMatrix(const Matrix44f &mtx)
{
    mMatrix = mtx;
}

void MatrixStack::pushMatrix()
{
    mStack.push_back(mMatrix);
}

void MatrixStack::pushMatrix(const Matrix44f &mtx)
{
    pushMatrix();
    mMatrix = mtx;
}

void MatrixStack::multMatrix(const Matrix44f &mtx)
{
    mMatrix *= mtx;
}

void MatrixStack::popMatrix()
{
    if (!mStack.empty()) {
        mMatrix = mStack.back();
    }
    mStack.pop_back();
}

Matrix44f& MatrixStack::getMatrix()
{
    return mMatrix;
}

size_t MatrixStack::size()
{
    return mStack.size();
}

bool MatrixStack::empty()
{
    return mStack.empty();
}

void Matrices::setMatrices( const Camera &cam )
{
    setModelView(cam);
    setProjection(cam);
}

void Matrices::setModelView( const Camera &cam )
{
    mModelView.setMatrix(cam.getModelViewMatrix());
}

void Matrices::setModelView( const Matrix44f &modelview )
{
    mModelView.setMatrix(modelview);
}

void Matrices::setProjection( const Camera &cam )
{
    mProjection.setMatrix(cam.getProjectionMatrix());
}

void Matrices::setProjection( const Matrix44f &proj )
{
    mProjection.setMatrix(proj);
}

void Matrices::pushModelView()
{
    mModelView.pushMatrix();
}

void Matrices::popModelView()
{
    mModelView.popMatrix();
}

void Matrices::pushModelView( const Camera &cam )
{
    mModelView.pushMatrix(cam.getModelViewMatrix());
}

void Matrices::pushProjection()
{
    mProjection.pushMatrix();
}

void Matrices::popProjection()
{
    mProjection.popMatrix();
}

void Matrices::pushProjection( const Camera &cam )
{
    mProjection.pushMatrix(cam.getProjectionMatrix());
}

void Matrices::pushMatrices()
{
    pushModelView();
    pushProjection();
}

void Matrices::popMatrices()
{
    popModelView();
    popProjection();
}

void Matrices::multModelView( const Matrix44f &mtx )
{
    mModelView.multMatrix(mtx);
}

void Matrices::multProjection( const Matrix44f &mtx )
{
    mProjection.multMatrix(mtx);
}

Matrix44f Matrices::getModelView()
{
    return mModelView.getMatrix();
}

Matrix44f Matrices::getProjection()
{
    return mProjection.getMatrix();
}


void Matrices::setMatricesWindowPersp( int screenWidth, int screenHeight, float fovDegrees, float nearPlane, float farPlane, bool originUpperLeft )
{
	CameraPersp cam( screenWidth, screenHeight, fovDegrees, nearPlane, farPlane );

    mProjection.setMatrix(cam.getProjectionMatrix());
    mModelView.setMatrix(cam.getModelViewMatrix());

	if( originUpperLeft ) {
        scale(Vec3f(1.0f, -1.0f, 1.0f));  // invert Y axis so increasing Y goes down.
        translate(1.0f, -1.0f, 1.0f);     // shift origin up to upper-left corner.
		glViewport( 0, 0, screenWidth, screenHeight );
	}
}

void Matrices::setMatricesWindow( int screenWidth, int screenHeight, bool originUpperLeft )
{
    CameraOrtho cam;

    if (originUpperLeft) {
        cam.setOrtho(0, screenWidth, screenHeight, 0, 1.0f, -1.0f);
    }
    else {
        cam.setOrtho(0, screenWidth, 0, screenHeight, 1.0f, -1.0f);
    }

    mProjection.setMatrix(cam.getProjectionMatrix());
    mModelView.setMatrix(cam.getModelViewMatrix());
    glViewport( 0, 0, screenWidth, screenHeight );
}

void Matrices::translate( const Vec2f &pos )
{
    mModelView.getMatrix().translate(Vec3f(pos.x, pos.y, 0));
}

void Matrices::translate( const Vec3f &pos )
{
    mModelView.getMatrix().translate(pos);
}

void Matrices::scale( const Vec3f &scl )
{
    mModelView.getMatrix().scale(scl);
}

void Matrices::rotate( const Vec3f &xyz )
{
    Vec3f xyzrad(toRadians(xyz.x), toRadians(xyz.y), toRadians(xyz.z));
    mModelView.getMatrix().rotate(xyzrad);
}

void Matrices::rotate( const Quatf &quat )
{
	Vec3f axis;
	float angle;
	quat.getAxisAngle( &axis, &angle );
    if( math<float>::abs( angle ) > EPSILON_VALUE ) {
		mModelView.getMatrix().rotate( Vec3f(axis.x, axis.y, axis.z), angle );
    }
}

} }
