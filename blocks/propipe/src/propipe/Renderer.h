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

	enum ClientState {
		ENABLE_ATTRIBS  = 1 << 0,
		UPDATE_UNIFORMS = 1 << 1,
		UPLOAD_ATTRIBS  = 1 << 2,
		STATE_ALL     = 0xffff,
	};

	virtual void setModelView(const Matrix44f& mvp) = 0;
	virtual void setProjection(const Matrix44f& proj) = 0;

	//! Set vertex color if a color array is not supplied
	virtual void setColor(const ColorA& color) = 0;

	// Set vertex attribute data arrays
	virtual void setPositionArray(GLfloat* pos, int dim) = 0;
	virtual void setTexCoordArray(GLfloat* texCoord) = 0;
	virtual void setColorArray(const GLvoid* colors, int colorType = GL_UNSIGNED_BYTE, int dim = 4) = 0;
	virtual void setNormalArray(GLfloat* normals) = 0;

	virtual void resetArrays() = 0;

	//! Enables client state, updates uniforms and uploads vertex attributes before a glDraw call
	virtual void enableClientState( uint32_t clientState = STATE_ALL ) = 0;
	//! Disables client state, called after drawing
	virtual void disableClientState() = 0;

	virtual void bindProg()   = 0;
	virtual void unbindProg() = 0;

	virtual void bindTexture(const gl::Texture& tex, GLuint textureUnit = 0) = 0;
	virtual void unbindTexture(GLuint textureUnit = 0) = 0;

	static RendererRef create(RendererType rendererType = DEFAULT);
};

class RendererException : public Exception {
};


} }
