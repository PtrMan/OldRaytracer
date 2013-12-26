#ifndef _H_MATRIX
#define _H_MATRIX

#include "vector.h"

struct matrix_44 {
	float _[4][4];
};

// this multiplicates two 4x4 matrixes
inline void matrix_mul(struct matrix_44 &A, struct matrix_44 &B, struct matrix_44 &Result) {
	Result._[0][0] = vector_dot(A._[0][0], A._[1][0], A._[2][0], A._[3][0], B._[0][0], B._[0][1], B._[0][2], B._[0][3]);
	Result._[1][0] = vector_dot(A._[0][0], A._[1][0], A._[2][0], A._[3][0], B._[1][0], B._[1][1], B._[1][2], B._[1][3]);
	Result._[2][0] = vector_dot(A._[0][0], A._[1][0], A._[2][0], A._[3][0], B._[2][0], B._[2][1], B._[2][2], B._[2][3]);
	Result._[3][0] = vector_dot(A._[0][0], A._[1][0], A._[2][0], A._[3][0], B._[3][0], B._[3][1], B._[3][2], B._[3][3]);

	Result._[0][1] = vector_dot(A._[0][1], A._[1][1], A._[2][1], A._[3][1], B._[0][0], B._[0][1], B._[0][2], B._[0][3]);
	Result._[1][1] = vector_dot(A._[0][1], A._[1][1], A._[2][1], A._[3][1], B._[1][0], B._[1][1], B._[1][2], B._[1][3]);
	Result._[2][1] = vector_dot(A._[0][1], A._[1][1], A._[2][1], A._[3][1], B._[2][0], B._[2][1], B._[2][2], B._[2][3]);
	Result._[3][1] = vector_dot(A._[0][1], A._[1][1], A._[2][1], A._[3][1], B._[3][0], B._[3][1], B._[3][2], B._[3][3]);

	Result._[0][2] = vector_dot(A._[0][2], A._[1][2], A._[2][2], A._[3][2], B._[0][0], B._[0][1], B._[0][2], B._[0][3]);
	Result._[1][2] = vector_dot(A._[0][2], A._[1][2], A._[2][2], A._[3][2], B._[1][0], B._[1][1], B._[1][2], B._[1][3]);
	Result._[2][2] = vector_dot(A._[0][2], A._[1][2], A._[2][2], A._[3][2], B._[2][0], B._[2][1], B._[2][2], B._[2][3]);
	Result._[3][2] = vector_dot(A._[0][2], A._[1][2], A._[2][2], A._[3][2], B._[3][0], B._[3][1], B._[3][2], B._[3][3]);

	Result._[0][3] = vector_dot(A._[0][3], A._[1][3], A._[2][3], A._[3][3], B._[0][0], B._[0][1], B._[0][2], B._[0][3]);
	Result._[1][3] = vector_dot(A._[0][3], A._[1][3], A._[2][3], A._[3][3], B._[1][0], B._[1][1], B._[1][2], B._[1][3]);
	Result._[2][3] = vector_dot(A._[0][3], A._[1][3], A._[2][3], A._[3][3], B._[2][0], B._[2][1], B._[2][2], B._[2][3]);
	Result._[3][3] = vector_dot(A._[0][3], A._[1][3], A._[2][3], A._[3][3], B._[3][0], B._[3][1], B._[3][2], B._[3][3]);
}

// this multiplicates a 4x4 matrix with a 3d position and return the Result
// ASK< must the 0 be 1? >
inline void matrix_mul(struct matrix_44 &A, float X, float Y, float Z, float &Resx, float &Resy, float &Resz) {
	Resx = vector_dot(X, Y, Z, 0, A._[0][0], A._[1][0], A._[2][0], A._[3][0]);
	Resy = vector_dot(X, Y, Z, 0, A._[0][1], A._[1][1], A._[2][1], A._[3][1]);
	Resz = vector_dot(X, Y, Z, 0, A._[0][2], A._[1][2], A._[2][2], A._[3][2]);
}

// this transfer the data from the matrix A to matrix B (very simple operation)
inline void matrix_move(struct matrix_44 &A, struct matrix_44 &B) {
	B._[0][0] = A._[0][0];
	B._[1][0] = A._[1][0];
	B._[2][0] = A._[2][0];
	B._[3][0] = A._[3][0];

	B._[0][1] = A._[0][1];
	B._[1][1] = A._[1][1];
	B._[2][1] = A._[2][1];
	B._[3][1] = A._[3][1];

	B._[0][2] = A._[0][2];
	B._[1][2] = A._[1][2];
	B._[2][2] = A._[2][2];
	B._[3][2] = A._[3][2];

	B._[0][3] = A._[0][3];
	B._[1][3] = A._[1][3];
	B._[2][3] = A._[2][3];
	B._[3][3] = A._[3][3];
}

// this fills a matrix with data
inline void matrix_fill(struct matrix_44 &M,
                        float _00, float _10, float _20, float _30,
                        float _01, float _11, float _21, float _31,
                        float _02, float _12, float _22, float _32,
                        float _03, float _13, float _23, float _33
) {
	M._[0][0] = _00;
	M._[1][0] = _10;
	M._[2][0] = _20;
	M._[3][0] = _30;

	M._[0][1] = _01;
	M._[1][1] = _11;
	M._[2][1] = _21;
	M._[3][1] = _31;

	M._[0][2] = _02;
	M._[1][2] = _12;
	M._[2][2] = _22;
	M._[3][2] = _32;

	M._[0][3] = _03;
	M._[1][3] = _13;
	M._[2][3] = _23;
	M._[3][3] = _33;
}

#endif