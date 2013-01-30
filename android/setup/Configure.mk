# Cinder Android build configuration options
# 
# Comment out a line to disable it.
# Changes made here may require a rebuild to take effect

# Enable FreeImage backend for Image decoding (large)
# USE_FREEIMAGE = yes

# Build ARM v5 ABI binaries (runs on all Android ARM devices)
USE_ARMEABI_V5A = yes

# Build ARM v7 ABI binaries (required for FPU and NEON support)
USE_ARMEABI_V7A = yes

# Build x86 binaries
USE_X86 = yes

# Build with C++11 support
USE_CPP_11 = yes

# Suggest leaving these enabled by default
USE_STBIMAGE = yes
USE_FREETYPE = yes

# Build with GLES2 support (disables GLES1)
# USE_GLES2 = yes

# Enable access to Android assets from native activities only
# (requires Android SDK level 9 or over)
USE_ASSET_MANAGER = yes

# Build against gnustl_shared instead of gnustl_static
# USE_STL_SHARED = yes

# Enable OCV Android Native Camera module (experimental)
# USE_OCV_CAPTURE = yes


