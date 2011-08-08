#include "cinder/app/AppNative.h"
#include "cinder/gl/GlslProg.h"
#include "cinder/Camera.h"
#include "cinder/Matrix.h"

#include <map>
#include <vector>

using namespace ci;
using namespace ci::app;
using std::map;
using std::vector;
using std::make_pair;

class ShaderTestES2 : public AppNative {
public:
    void setup();

    void update();
    void draw();

    CameraPersp     mCamera;

    Area            mViewport;
    Area            mViewportL;
    Area            mViewportR;

    Matrix44f       mModelView;
    Matrix44f       mProjection;

    gl::GlslProg    mShader;
    float           mAngle;
    GLuint          mPositionHandle;

    bool            mStereo;

    void touchesBegan( TouchEvent event );
    void touchesMoved( TouchEvent event );
    void touchesEnded( TouchEvent event );

    map<uint32_t,Vec2f>    mActivePoints;

protected:
    void drawView(Area viewport, float eyeRightOffset);
};

const GLfloat gRectVertices[] = { -0.5f, -0.5f,
                                  -0.5f,  0.5f,
                                   0.5f, -0.5f,
                                   0.5f,  0.5f };

void ShaderTestES2::setup()
{
    mStereo = false;

    try {
        mShader = gl::GlslProg(loadResource("vert.glsl"), loadResource("frag.glsl"));
        mPositionHandle = mShader.getAttribLocation("aPosition");
    }
    catch( gl::GlslProgCompileExc &exc ) {
        console() << "Shader compile error: " << std::endl;
        console() << exc.what() << std::endl;
    }
    catch( ... ) {
        console() << "Unable to load shader" << std::endl;
    }

    //  Left and right viewports for side-by-side rendering
    mViewport = getWindowBounds();

    int halfWidth = mViewport.getX2() / 2;
    mViewportL = Area(mViewport.getX1(), mViewport.getY1(), halfWidth, mViewport.getY2());
    mViewportR = Area(halfWidth, mViewport.getY1(), mViewport.getX2(), mViewport.getY2());

    float aspectRatio = float(mViewport.getWidth()) / float(mViewport.getHeight());

    mCamera.setPerspective(35.0f, aspectRatio, 0.1f, 100.0f);
    mCamera.lookAt(Vec3f(0, 0, 5.0f), Vec3f(0, 0, 0));

    glFrontFace(GL_CW);
}

void ShaderTestES2::update()
{
}

void ShaderTestES2::touchesBegan( TouchEvent event )
{
    console() << "Began: " << event << std::endl;

    int touches = mActivePoints.size();
    for( vector<TouchEvent::Touch>::const_iterator touchIt = event.getTouches().begin(); touchIt != event.getTouches().end(); ++touchIt ) {
        mActivePoints.insert( make_pair( touchIt->getId(), touchIt->getPos() ) );
    }

    //  Second finger down, switch to stereo rendering
    if (touches < 2 && mActivePoints.size() == 2) {
        mStereo = !mStereo;
    }
}

void ShaderTestES2::touchesMoved( TouchEvent event )
{
    for( vector<TouchEvent::Touch>::const_iterator touchIt = event.getTouches().begin(); touchIt != event.getTouches().end(); ++touchIt )
        mActivePoints[touchIt->getId()] = touchIt->getPos();
}

void ShaderTestES2::touchesEnded( TouchEvent event )
{
    console() << "Ended: " << event << std::endl;
    for( vector<TouchEvent::Touch>::const_iterator touchIt = event.getTouches().begin(); touchIt != event.getTouches().end(); ++touchIt ) {
        mActivePoints.erase( touchIt->getId() );
    }
}

void ShaderTestES2::draw()
{
    gl::clear();

    if (!mStereo) {
        drawView(mViewport, 0);
    }
    else {
        // Stereo render
        drawView(mViewportL, -0.1f);
        drawView(mViewportR,  0.1f);
    }
}

void ShaderTestES2::drawView(Area viewport, float eyeRightOffset)
{
    gl::setViewport(viewport);

    Vec3f right, up;
    const Vec3f eyePos = mCamera.getEyePoint();

    //  Shift camera by supplied offset
    if (eyeRightOffset != 0) {
        mCamera.getBillboardVectors(&right, &up);
        mCamera.setEyePoint(eyePos + eyeRightOffset * right);
    }
    
    mShader.bind();
    mShader.uniform("uProjection", mCamera.getProjectionMatrix());
    mShader.uniform("uModelView",  mCamera.getModelViewMatrix());

    glVertexAttribPointer(mPositionHandle, 2, GL_FLOAT, GL_FALSE, 0, gRectVertices);
    glEnableVertexAttribArray(mPositionHandle);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    //  restore camera position
    if (eyeRightOffset != 0) {
        mCamera.setEyePoint(eyePos);
    }
}

CINDER_APP_NATIVE( ShaderTestES2, RendererGl )

