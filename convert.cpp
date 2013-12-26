
#include <string>

#include "convert.h"

using namespace std;

float Convert::strToFloat(string Str) {
	float div      = 1.0f;  // is the dision number
	float number   = 0.0f;  // is the actual number
	bool  dotDone  = false; // was the dot parsed?
	bool  negative = false; // is it a negative number?

	for( size_t i = 0; i < Str.length(); i++ ) {
		char str = Str[i];

		if( str == '0' ) {
			// nothing
		}
		else if( str == '1' ) {
			number += 1.0f;
		}
		else if( str == '2' ) {
			number += 2.0f;
		}
		else if( str == '3' ) {
			number += 3.0f;
		}
		else if( str == '4' ) {
			number += 4.0f;
		}
		else if( str == '5' ) {
			number += 5.0f;
		}
		else if( str == '6' ) {
			number += 6.0f;
		}
		else if( str == '7' ) {
			number += 7.0f;
		}
		else if( str == '8' ) {
			number += 8.0f;
		}
		else if( str == '9' ) {
			number += 9.0f;
		}
		else if( str == '.' ) {
			if( !dotDone ) {
				dotDone = true;
				number /= 10.0;
			}
			else {
				// throw the Error
				throw class Error();
			}
		}
		else if( str == '-' ) {
			if( !negative ) {
				negative = true;
			}
			else {
				// throw the Error
				throw class Error();
			}
		}
		else {
			// throw the error
			throw class Error();
		}

		number *= 10.0f;
		if( dotDone ) {
			div *= 10.0f;
		}
	}

	if ( !negative ) {
		return number / div;
	}
	else {
		return - number / div;
	}
}

unsigned long Convert::strToULong(string Str) {
	unsigned long number = 0;  // is the actual number
	
	for( size_t i = 0; i < Str.length(); i++ ) {
		char str = Str[i];
		
		number *= 10;

		if( str == '0' ) {
			// nothing
		}
		else if( str == '1' ) {
			number += 1;
		}
		else if( str == '2' ) {
			number += 2;
		}
		else if( str == '3' ) {
			number += 3;
		}
		else if( str == '4' ) {
			number += 4;
		}
		else if( str == '5' ) {
			number += 5;
		}
		else if( str == '6' ) {
			number += 6;
		}
		else if( str == '7' ) {
			number += 7;
		}
		else if( str == '8' ) {
			number += 8;
		}
		else if( str == '9' ) {
			number += 9;
		}
		else {
			// throw the error
			throw class Error();
		}
	}

	return number;
}