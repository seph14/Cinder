#pragma once

#include "cinder/Area.h"
#include "cinder/Surface.h"
#include <vector>

namespace cinder {

class SurfacePack
{
public:
    SurfacePack(int width, int height);

    int  fit(size_t index, int width, int height);
    void merge();

    Area allocateArea(int width, int height);
    void setAreaData(Area area, uint8_t* data, size_t stride);

    Surface& getSurface();

private:
    struct Region
    {
        int x, y, width, height;
    };

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

