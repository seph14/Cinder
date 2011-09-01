#include "cinder/CinderMath.h"
#include "cinder/SurfacePack.h"

using namespace cinder;

SurfacePack::SurfacePack(int width, int height) 
: mWidth(width), mHeight(height), mUsed(0), mSurface(width, height, true)
{
    Node node = { 0, 0, width };
    mNodes.push_back(node);
}

void SurfacePack::setAreaData(Area area, uint8_t* data, size_t stride)
{
    Surface::Iter iter( mSurface, area );
    uint8_t* rowPtr = data;
    while (iter.line()) {
        uint8_t* pixelPtr = rowPtr;
        while (iter.pixel()) {
            iter.r() = *pixelPtr;
            iter.a() = *pixelPtr;
            ++pixelPtr;
        }
        rowPtr += stride;
    }
}

int SurfacePack::fit(size_t index, int width, int height)
{
    Node& node = mNodes[index];
    int x = node.x, y, width_left = width;
    size_t i = index;

    if ( (x + width) > mWidth ) {
        return -1;
    }

    y = node.y;

    while( width_left > 0 ) {
        Node& next = mNodes[i];
        y = math<int>::max( y, next.y );
        if( (y + height) > mHeight ) {
            return -1;
        }
        width_left -= next.width;
        ++i;
    }

    return y;
}

void SurfacePack::merge()
{
    size_t i;

    for( i=0; i< mNodes.size()-1; ++i ) {
        Node& node = mNodes[i];
        Node& next = mNodes[i+1];

        if( node.y == next.y ) {
            node.width += next.width;
            mNodes.erase(mNodes.begin()+i+1);
            --i;
        }
    }
}

Area SurfacePack::allocateArea(int width, int height)
{
    int y, best_height, best_width, best_index;
    SurfacePack::Region region = { 0, 0, width, height };
    size_t i;

    best_height = INT_MAX;
    best_index  = -1;
    best_width = INT_MAX;
    for( i=0; i < mNodes.size(); ++i ) {
        y = fit( i, width, height );
        if( y >= 0 ) {
            Node& node = mNodes[i];
            if( ( y + height < best_height ) ||
                    ( y + height == best_height && node.width < best_width) ) {
                best_height = y + height;
                best_index = i;
                best_width = node.width;
                region.x = node.x;
                region.y = y;
            }
        }
    }

    if( best_index == -1 ) {
        return Area(-1, -1, -1, -1);
    }

    Node newNode;
    newNode.x     = region.x;
    newNode.y     = region.y + height;
    newNode.width = width;
    mNodes.insert(mNodes.begin() + best_index, newNode);

    for(i = best_index+1; i < mNodes.size(); ++i) {
        Node& node = mNodes[i];
        Node& prev = mNodes[i-1];

        if (node.x < (prev.x + prev.width) ) {
            int shrink = prev.x + prev.width - node.x;
            node.x     += shrink;
            node.width -= shrink;
            if (node.width <= 0) {
                mNodes.erase(mNodes.begin() + i);
                --i;
            }
            else {
                break;
            }
        }
        else {
            break;
        }
    }
    merge();
    mUsed += width * height;
    return Area(region.x, region.y, region.x + region.width, region.y + region.height);
}

Surface& SurfacePack::getSurface() 
{ 
    return mSurface; 
}

