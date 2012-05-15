#pragma once

#include "cinder/Cinder.h"

namespace cinder { namespace pp {

class Context;
class DrawShader;
typedef std::shared_ptr<DrawShader> DrawShaderRef;

typedef std::shared_ptr<Context> ContextRef;
typedef std::weak_ptr<Context>   ContextRefW;

class Context
{
  protected:
	static ContextRefW sInstance;
	Context();

	DrawShader* mBoundShader;

	void unbindProg();

  public:
	static ContextRef create(bool useGlobal = true);
	static void reset();

  public:
	//  Tracks currently bound DrawShader
	void bindProg(DrawShader& shader);
};

} }
