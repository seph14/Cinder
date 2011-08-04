#include "cinder/app/AppNative.h"
#include "cinder/gl/GlslProg.h"

using namespace ci;
using namespace ci::app;

class ShaderTestES2 : public AppNative {
 public: 	
	void setup();
	
	void update();
	void draw();
	
	gl::GlslProg	mShader;
	float			mAngle;
    GLuint          mPositionHandle;
};

const GLfloat gTriangleVertices[] = { 0.0f, 0.5f, -0.5f, -0.5f, 0.5f, -0.5f };

void ShaderTestES2::setup()
{
    try {
        mShader = gl::GlslProg(loadResource("vert.glsl"), loadResource("frag.glsl"));
        mPositionHandle = mShader.getAttribLocation("vPosition");
    }
    catch( gl::GlslProgCompileExc &exc ) {
        console() << "Shader compile error: " << std::endl;
        console() << exc.what() << std::endl;
    }
    catch( ... ) {
        console() << "Unable to load shader" << std::endl;
    }

    Area windowBounds = getWindowBounds();
    gl::setViewport(windowBounds);
}

void ShaderTestES2::update()
{
	mAngle += 0.05f;
}

void ShaderTestES2::draw()
{
	gl::clear();

	mShader.bind();
    glVertexAttribPointer(mPositionHandle, 2, GL_FLOAT, GL_FALSE, 0, gTriangleVertices);
    glEnableVertexAttribArray(mPositionHandle);
    glDrawArrays(GL_TRIANGLES, 0, 3);
}

CINDER_APP_NATIVE( ShaderTestES2, RendererGl )

