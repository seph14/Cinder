#include "DrawShader.h"
#include "cinder/gl/GlslProg.h"
#include "cinder/gl/Texture.h"

#include <map>

using std::map;

using namespace cinder::gl;

namespace cinder { namespace pp {

/** Default shader implementation */
class DefaultDrawShader : public DrawShader
{
public:
	DefaultDrawShader(ContextRef context) : mContext(context), mBound(false), mBoundTextures(0),
		mPositionDim(3), mPositionArray(0), mTexCoordArray(0), mColorArray(0), mNormalArray(0)
	{
		try {
			mShader  = gl::GlslProg(vert, frag, 0, 0, 0, 0, false);
			mShader.bindAttribLocation(0, "aPosition");
			mShader.link();
			mPositionAttrib = mShader.getAttribLocation("aPosition");
			mTexCoordAttrib = mShader.getAttribLocation("aTexCoord");
			mColorAttrib    = mShader.getAttribLocation("aColor");
		}
		catch (GlslProgCompileExc& ex) {
			throw DrawShaderException();
			// CI_LOGE("Error compiling: %s", ex.what());
		}
	}

	virtual void setModelView(const Matrix44f& mv)
	{
		mModelView = mv;
	}

	virtual void setProjection(const Matrix44f& proj)
	{
		mProjection = proj;
	}

	virtual void setColor(const ColorA& color)
	{
		mColor = color;
	}

	virtual void setPositionArray(float* pos, int dim)
	{
		mPositionArray = pos;
		mPositionDim = dim;
	}

	virtual void setTexCoordArray(float* texCoord)
	{
		mTexCoordArray = texCoord;
	}

	virtual void setColorArray(const GLvoid* colors, int colorType, int dim)
	{
		mColorArray = const_cast<GLvoid*>(colors);
		mColorDim = dim;
		mColorType = colorType;
	}

	virtual void setNormalArray(float* norms)
	{
		mNormalArray = norms;
	}

	virtual void resetArrays()
	{
		mPositionArray = NULL;
		mTexCoordArray = NULL;
		mColorArray	= NULL;
		mNormalArray   = NULL;
	}

	virtual void bindProg()
	{
		if (!mBound) {
			mContext->bindProg(*this);
			mShader.bind();
			mBound = true;
		}
	}

	virtual void unbindProg()
	{
		if (mBound) {
			mShader.unbind();
			mBound = false;
		}
	}

	virtual void bindTexture(const gl::Texture& tex, GLuint textureUnit)
	{
		tex.bind(textureUnit);
		mBoundTextures |= (1 << textureUnit);
		mTextureTargets[textureUnit] = tex.getTarget();
	}

	virtual void unbindTexture(GLuint textureUnit)
	{
		map<int, GLenum>::iterator it = mTextureTargets.find(textureUnit);
		if (it != mTextureTargets.end()) {
			glActiveTexture( GL_TEXTURE0 + textureUnit );
			glBindTexture( it->second, 0 );
			glActiveTexture( GL_TEXTURE0 );
			mBoundTextures &= ~(1 << textureUnit);
		}
	}

	virtual void enableClientState( uint32_t clientState )
	{
		if (clientState & ENABLE_ATTRIBS) {
			glEnableVertexAttribArray(mPositionAttrib);
			if ( mColorArray )
				glEnableVertexAttribArray(mColorAttrib);
			if ( mTexCoordArray )
				glEnableVertexAttribArray(mTexCoordAttrib);
		}

		if ( clientState & UPDATE_UNIFORMS ) {
			if ( mColorArray ) {
				mShader.uniform("uEnableColorAttr", true);
			}
			else {
				mShader.uniform("uColor", mColor);
				mShader.uniform("uEnableColorAttr", false);
			}

			if (mTexCoordArray && (mBoundTextures & 1)) {
				mShader.uniform("sTexture", 0);
				mShader.uniform("uEnableTextureAttr", true);
			}
			else {
				mShader.uniform("uEnableTextureAttr", false);
			}

			mShader.uniform("uMVP", mProjection * mModelView);
		}

		if ( clientState & UPLOAD_ATTRIBS ) {
			glVertexAttribPointer( mPositionAttrib, mPositionDim, GL_FLOAT, GL_FALSE, 0, mPositionArray );
			glVertexAttribPointer( mTexCoordAttrib, 2, GL_FLOAT, GL_FALSE, 0, mTexCoordArray );
			if ( mColorArray )
				glVertexAttribPointer( mColorAttrib, mColorDim, mColorType, GL_FALSE, 0, mColorArray );
		}
	}

	virtual void disableClientState()
	{
		glDisableVertexAttribArray(mPositionAttrib);
		glDisableVertexAttribArray(mColorAttrib);
		glDisableVertexAttribArray(mTexCoordAttrib);
	}

	static const char* vert;
	static const char* frag;

protected:
	ContextRef mContext;
	bool     mBound;
	uint32_t mBoundTextures;
	map<int, GLenum> mTextureTargets;

	gl::GlslProg mShader;
	Matrix44f mModelView;
	Matrix44f mProjection;

	GLuint mPositionAttrib;
	GLuint mTexCoordAttrib;
	GLuint mColorAttrib;
	GLuint mNormalAttrib;

	int    mPositionDim;
	int    mColorDim;
	int    mColorType;

	ColorA8u mColor;

	GLfloat* mPositionArray;
	GLfloat* mTexCoordArray;
	GLvoid*  mColorArray;
	GLfloat* mNormalArray;
};

const char* DefaultDrawShader::vert =
		"attribute vec4 aPosition;\n"
		"attribute vec2 aTexCoord;\n"
		"attribute vec4 aColor;\n"

		"uniform mat4 uMVP;\n"
		"uniform vec4 uColor;\n"

		"uniform bool uEnableColorAttr;\n"
		"uniform bool uEnableTextureAttr;\n"

		"varying vec4 vColor;\n"
		"varying vec2 vTexCoord;\n"

		"void main() {\n"
		"  if (uEnableColorAttr) {\n"
		"	vColor = aColor;\n"
		"  }\n"
		"  else {\n"
		"	vColor = uColor;\n"
		"  }\n"
		"  if (uEnableTextureAttr) {\n"
		"	vTexCoord = aTexCoord;\n"
		"  }\n"
		"  gl_Position = uMVP * aPosition;\n"
		"}\n";

const char* DefaultDrawShader::frag =
		"precision mediump float;\n"

		"uniform sampler2D sTexture;\n"

		"uniform bool uEnableTextureAttr;\n"

		"varying vec4 vColor;\n"
		"varying vec2 vTexCoord;\n"

		"void main() {\n"
		"  if (uEnableTextureAttr) {\n"
		"    gl_FragColor = vColor * texture2D(sTexture, vTexCoord);\n"
		"  }\n"
		"  else {\n"
		"    gl_FragColor = vColor;\n"
		"  }\n"
		"}\n";

DrawShaderRef DrawShader::create(DrawShaderType rendererType, ContextRef context)
{
	return DrawShaderRef(new DefaultDrawShader(context));
}

} }

