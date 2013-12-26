#ifndef _H_V4PTR
#define _H_V4PTR

/* this defines a structure that represents a vector of pointers to a special Type
 */

template <typename Type> struct v4ptr {
	Type _[4];
};

#endif
