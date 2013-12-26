#ifndef _H_VECTOR3
#define _H_VECTOR3

// this is a Class (and a data structure) that represents a point or a vector in the 3d space
class Vector3 {
   public:
	Vector3(): x(0.0f), y(0.0f), z(0.0f) {}
	Vector3(float X, float Y, float Z): x(X), y(Y), z(Z) {}
	
	Vector3& operator +=(const Vector3& a) {
		x += a.x;
		y += a.y;
		z += a.z;

		return *this;
	}

	float x;
	float y;
	float z;
};

#endif