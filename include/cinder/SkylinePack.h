#pragma once

#include "cinder/Area.h"
#include "cinder/Surface.h"
#include <vector>

namespace cinder {

class SkylinePack
{
public:
    SkylinePack(int width, int height);
    Area allocateArea(int width, int height);

private:
    struct Region
    {
        int x, y, width, height;
    };

    struct Node
    {
        int x, y, width;
    };

    int  fit(size_t index, int width, int height);
    void merge();

    std::vector<Node> mNodes;
    int mWidth;
    int mHeight;
    int mUsed;
};

}

