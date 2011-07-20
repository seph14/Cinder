#include "cinder/ImageSourceFileFreeImage.h"
#include "cinder/Utilities.h"

namespace cinder {

//  FreeImage IO implemented with IStream
static unsigned _ReadProc(void *buffer, unsigned size, unsigned count, fi_handle handle) 
{
	IStream* stream = static_cast<IStream*>(handle);
	return stream->readDataAvailable(buffer, count*size);
}

static unsigned _WriteProc(void *buffer, unsigned size, unsigned count, fi_handle handle) 
{
	//  Writing not supported on input stream
	return size;
}

static int _SeekProc(fi_handle handle, long offset, int origin) 
{
	assert(origin != SEEK_END);

	IStream* stream = static_cast<IStream*>(handle);

	if (origin == SEEK_SET) {
		stream->seekAbsolute(offset);
	} else {
		stream->seekRelative(offset);
	}

	return 0;
}

static long _TellProc(fi_handle handle) {
	IStream* stream = static_cast<IStream*>(handle);
	return stream->tell();
}

// ----------------------------------------------------------


ImageSourceFileFreeImageRef ImageSourceFileFreeImage::createFileFreeImageRef( DataSourceRef dataSourceRef, ImageSource::Options options )
{
	return ImageSourceFileFreeImageRef( new ImageSourceFileFreeImage( dataSourceRef, options ) );
}

ImageSourceFileFreeImage::ImageSourceFileFreeImage( DataSourceRef dataSourceRef, ImageSource::Options options )
	: ImageSource()
{
	FreeImageIO io;

	io.read_proc  = _ReadProc;
	io.write_proc = _WriteProc;
	io.tell_proc  = _TellProc;
	io.seek_proc  = _SeekProc;

	IStreamRef stream = dataSourceRef->createStream();
	// XXX match file extension to image format
	mBitmap = FreeImage_LoadFromHandle(FIF_PNG, &io, stream.get());
	mWidth  = FreeImage_GetWidth(mBitmap);
	mHeight = FreeImage_GetHeight(mBitmap);
	CI_LOGI("XXX ImageSourceFileFreeImage loaded PNG from %s (%d x %d)", stream->getFileName().c_str(),
			mWidth, mHeight);
	// switch( FreeImage_GetColorType(mBitmap) ) {
	// 	case FIC_MINISBLACK:
	// 		mColorModel = CM_GRAY;
	// 		break;
	// 	case FIC_RGBA:
	// 		mC
	// }
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
	CI_LOGI("XXX ImageSourceFileFreeImage registered as a generic source");
}

} // namespace ci
