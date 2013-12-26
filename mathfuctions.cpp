#include "mathfunctions.h"

float math_abs(float Value) {
	if( Value < 0.0f ) {
		return -Value;
	}
	else {
		return Value;
	}
}

float math_max(float a, float b) {
	if( a > b ) {
		return a;
	}
	else {
		return b;
	}
}