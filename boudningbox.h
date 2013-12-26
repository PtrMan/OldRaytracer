#ifndef _H_BOUNDINGBOX
#define _H_BOUNDINGBOX

// this is a abstraction for a aabb boudning box
class BoundingBox {
   public:
	BoundingBox() : minx(9e6f), miny(9e6f), minz(9e6f), maxx(-9e6f), maxy(-9e6f), maxz(-9e6f) {}
	
	// this expands the boudning box with the min and max bounds
	inline void expand(float Minx, float Miny, float Minz, float Maxx, float Maxy, float Maxz) {
		if( Maxx > maxx ) {
			maxx = Maxx;
		}
		if( Maxy > maxy ) {
			maxy = Maxy;
		}
		if( Maxz > maxz ) {
			maxz = Maxz;
		}

		if( Minx < minx ) {
			minx = Minx;
		}
		if( Miny < miny ) {
			miny = Miny;
		}
		if( Minz < minz ) {
			minz = Minz;
		}
	}

	float minx, miny, minz, maxx, maxy, maxz;
}

#endif
