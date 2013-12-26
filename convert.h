#ifndef _H_CONVERT
#define _H_CONVERT

#include <string>

using namespace std;

class Convert {
   public:

	// this is a Class that the Static Methods can throw
	class Error {
	};

	// this converts a float-string to a float value
	// it throws the Class Error if something wrong happended
	static float strToFloat(string Str);
	
	// this converts a (right truncated) string into a unsigned long number
	// throws a ConvertError if it fails
	static unsigned long Convert::strToULong(string Str);
};

#endif