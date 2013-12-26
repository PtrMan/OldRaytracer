#ifndef _H_INLINE
#define _H_INLINE

// this provides various inlined (with intrinsic) functions for vectors and so on

#include "intrinsic.h"

class Inline {
   public:
	// this does a crossproduct
	static void crossproduct(const Intrinsic::f4& ax, const Intrinsic::f4& ay, const Intrinsic::f4& az, const Intrinsic::f4& bx, const Intrinsic::f4& by, const Intrinsic::f4& bz, Intrinsic::f4& Ox, Intrinsic::f4& Oy, Intrinsic::f4& Oz) {
		Ox = Intrinsic::sub( Intrinsic::mul(ay, bz), Intrinsic::mul(az, by) ); // Ox = ay*bz - az*by
		Oy = Intrinsic::sub( Intrinsic::mul(az, bx), Intrinsic::mul(ax, bz) ); // Oy = az*bx - ax*bz
		Oz = Intrinsic::sub( Intrinsic::mul(ax, by), Intrinsic::mul(ay, bx) ); // Oz = ax*by - ay*bx
	}

	// this do normalisize it
	static void normalisize(Intrinsic::f4& X, Intrinsic::f4& Y, Intrinsic::f4& Z) {
		Intrinsic::f4 rlength = Intrinsic::Rsqrt( Intrinsic::add( Intrinsic::add( Intrinsic::mul(X, X), Intrinsic::mul(Y, Y) ), Intrinsic::mul(Z, Z) ) ); // rlength = 1.0f / sqrt( X*X + Y*Y + Z*Z )
		X = Intrinsic::mul(X, rlength); // x *= rlength
		Y = Intrinsic::mul(Y, rlength); // y *= rlength
		Z = Intrinsic::mul(Z, rlength); // z *= rlength
	}

	// this do calculate the dot-product
	static Intrinsic::f4 dotproduct(const Intrinsic::f4& Ax, const Intrinsic::f4& Ay, const Intrinsic::f4& Az, const Intrinsic::f4& Bx, const Intrinsic::f4& By, const Intrinsic::f4& Bz) {
		return Intrinsic::add( Intrinsic::add( Intrinsic::mul(Ax, Bx), Intrinsic::mul(Ay, By) ), Intrinsic::mul(Az, Bz) );
	}
	
	static Intrinsic::f4 dotproduct(const Intrinsic::f4& Ax, const Intrinsic::f4& Ay, const Intrinsic::f4& Az, const float* Bx, const float* By, const float* Bz) {
		Intrinsic::f4 bx = Intrinsic::shuffle_spread(Intrinsic::loadS(Bx));
		Intrinsic::f4 by = Intrinsic::shuffle_spread(Intrinsic::loadS(By));
		Intrinsic::f4 bz = Intrinsic::shuffle_spread(Intrinsic::loadS(Bz));
		return Intrinsic::add( Intrinsic::add( Intrinsic::mul(Ax, bx), Intrinsic::mul(Ay, by) ), Intrinsic::mul(Az, bz) );
	}
};

#endif