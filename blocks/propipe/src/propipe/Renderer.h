#pragma once

#include "cinder/gl/gl.h"
#include "cinder/Color.h"
#include "cinder/Exception.h"
#include "cinder/Matrix.h"

namespace cinder { namespace pp {

typedef std::shared_ptr<class Renderer> RendererRef;

class Renderer 
{
public:
	enum RendererType {
		DEFAULT = 0,
	};

    virtual void setModelView(const Matrix44f& mvp) = 0;
    virtual void setProjection(const Matrix44f& proj) = 0;

	//! Set vertex color if a color array is not supplied
	virtual void setColor(const ColorA& color) = 0;

	// Set vertex attribute data arrays
	virtual void setPositionArray(float* pos, int dim) = 0;
	virtual void setTexCoordArray(float* texCoord) = 0;
	virtual void setColorArray(GLubyte* colors) = 0;
	virtual void setNormalArray(float* normals) = 0;

    virtual void resetArrays() = 0;

	//! Enables client state, updates uniforms and sets vertex data before a glDraw call
	virtual void enableClientState()  = 0;
	//! Disables client state, called after drawing
	virtual void disableClientState() = 0;

    virtual void bind()   = 0;
    virtual void unbind() = 0;

	static RendererRef create(RendererType rendererType = DEFAULT);
};

class RendererException : public Exception {
};


} }
