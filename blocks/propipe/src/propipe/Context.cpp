#include "propipe/Context.h"
#include "propipe/DrawShader.h"

namespace cinder { namespace pp {

ContextRefW Context::sInstance;

Context::Context() : mBoundShader(0)
{
}

ContextRef Context::create(bool useGlobal)
{
	if ( useGlobal ) {
		ContextRef context = sInstance.lock();
		if (!context) {
			sInstance = context = ContextRef(new Context());
		}
		return context;
	}

	return ContextRef(new Context());
}

void Context::bindProg(DrawShader& shader)
{
	unbindProg();
	mBoundShader = &shader;
}

void Context::unbindProg()
{
	if (mBoundShader) {
		mBoundShader->unbindProg();
	}
	mBoundShader = NULL;
}

} }

