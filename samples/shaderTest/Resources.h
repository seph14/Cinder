#pragma once

#if defined( CINDER_MSW )
#include "cinder/CinderResources.h"

#define RES_PASSTHRU_VERT	CINDER_RESOURCE( ../data/, passThru_vert.glsl, 128, GLSL )
#define RES_BLUR_FRAG		CINDER_RESOURCE( ../data/, gaussianBlur_frag.glsl, 129, GLSL )
#define RES_IMAGE_JPG		CINDER_RESOURCE( ../../data/, cinder_logo.png, 130, IMAGE )

#elif defined( CINDER_ANDROID )

#define RES_PASSTHRU_VERT	"passThru_vert.glsl"
#define RES_BLUR_FRAG		"gaussianBlur_frag.glsl"
#define RES_IMAGE_JPG		"cinder_logo.png"

#endif
