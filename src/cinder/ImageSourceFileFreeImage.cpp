#include "cinder/ImageSourceFileFreeImage.h"
#include "cinder/Utilities.h"

namespace cinder {

ImageSourceFileFreeImageRef ImageSourceFileFreeImage::createFileFreeImageRef( DataSourceRef dataSourceRef, ImageSource::Options options )
{
	return ImageSourceFileFreeImageRef( new ImageSourceFileFreeImage( dataSourceRef, options ) );
}

ImageSourceFileFreeImage::ImageSourceFileFreeImage( DataSourceRef dataSourceRef, ImageSource::Options options )
	: ImageSource()
{
}

void ImageSourceFileFreeImage::load( ImageTargetRef target )
{
	// // get a pointer to the ImageSource function appropriate for handling our data configuration
	// ImageSource::RowFunc func = setupRowFunc( target );
	// 
	// const uint8_t *data = mData.get();
	// for( int32_t row = 0; row < mHeight; ++row ) {
	// 	((*this).*func)( target, row, data );
	// 	data += mRowBytes;
	// }
}

void ImageSourceFileFreeImage::registerSelf()
{
	const int32_t SOURCE_PRIORITY = 2;
	
	ImageIoRegistrar::registerSourceGeneric( ImageSourceFileFreeImage::createRef, SOURCE_PRIORITY );
}

} // namespace ci
