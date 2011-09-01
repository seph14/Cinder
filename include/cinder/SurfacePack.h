#pragma once

#include "cinder/Surface.h"
#include <vector>

namespace cinder {

class SurfacePack
{
public:
    struct Region
    {
        int x, y, width, height;
    };

    SurfacePack(int width, int height);

    void setArea(size_t x, size_t y, size_t width, size_t height, uint8_t* data, size_t stride);
    int  fit(size_t index, int width, int height);
    void merge();

    Region getRegion(int width, int height);
    Surface& getSurface();

private:
    struct Node
    {
        int x, y, width;
    };

    std::vector<Node> mNodes;
    int mWidth;
    int mHeight;
    int mUsed;
    Surface mSurface;
};

}

