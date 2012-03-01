#include "propipe.h"

using std::vector;
using namespace cinder;

Geo2D::Geo2D(GeoBuf2D& buf, GeoFlags flags) : mBuffer(buf), mFlags(flags)
{
}

Geo2D::~Geo2D()
{
}

void Geo2D::line( const Vec2f& start, const Vec2f& end )
{
   buf.positions.push_back(start);
   buf.positions.push_back(end);
}

void Geo2D::solidRect( const Rectf& rect, bool isQuad )
{
   mFlags &= (GEOM_POSITIONS | GEOM_TEXCOORDS);

   vector<Vec2f>& pos = buf.positions;
   pos.push_back(rect.getUpperRight());
   pos.push_back(rect.getUpperLeft());
   pos.push_back(rect.getLowerRight());
   pos.push_back(rect.getLowerLeft());

   if ( isQuad && (mFlags & GEOM_TEXCOORDS) ) {
      vector<Vec2f>& uv = buf.texCoords;
      uv.push_back( Vec2f(1, 0) );
      uv.push_back( Vec2f(0, 0) );
      uv.push_back( Vec2f(1, 1) );
      uv.push_back( Vec2f(0, 1) );
   }
}

void Geo2D::solidRect( const Rectf& rect, const Rectf& texCoords )
{
   solidRect(rect, false);

   if ( mFlags & GEOM_TEXCOORDS ) {
      vector<Vec2f>& uv = buf.texCoords;
      uv.push_back(texCoords.getUpperRight());
      uv.push_back(texCoords.getUpperLeft());
      uv.push_back(texCoords.getLowerRight());
      uv.push_back(texCoords.getLowerLeft());
   }
}

void Geo2D::solidCircle( const Vec2f &center, float radius, int numSegments )
{
}

void Geo2D::ellipse( const Vec2f &center, float radiusX, float radiusY, int numSegments )
{
}


GeoBuf2D& Geo2D::buffer()
{
   return mBuffer;
}

GeoFlags Geo2D::flags()
{
   return mFlags;
}


