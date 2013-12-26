#ifndef _H_vector_cStyle
#define _H_vector_cStyle

// these are functions that do vector stuff in a C-Style (no class was used)
// i did it like this because i think its for some code the fastest and the best to line out future (sse/simd) optimization

float vector_dotproduct(float ax, float ay, float az, float bx, float by, float bz);

void vector_normalisize(float &x, float &y, float &z);

void vector_crossproduct(float ax, float ay, float az, float bx, float by, float bz, float &resx, float &resy, float &resz);

float vector_length(float x, float y, float z);

float vector_length(float x1, float y1, float z1, float x2, float y2, float z2);

#endif
