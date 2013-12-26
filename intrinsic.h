#ifndef _H_INTRINSIC
#define _H_INTRINSIC

#ifdef MSVC
#   include <emmintrin.h>
#endif
#ifdef GCC
    TODO
#endif

class Intrinsic {
   public:
	// define the generic datatype
#ifdef MSVC
	typedef __m128 f4; // four float values
	
	// one float value
	class f1 {
	   public:
		f1(const __m128 Value):value(Value){}
		f1(){}
		f1(const float Value) {
			value = _mm_load_ss(&Value);
		}
		
		__m128 value;

		// this are overloaded operators for (readable) calculations tricks
		f1 operator||(const f1& A) {
			return Intrinsic::or_f1(*this, A);
		}

		f1 operator>(const f1& A) {
			return Intrinsic::sub_f1(A, *this);
		}

		f1 operator<(const f1& A) {
			return Intrinsic::sub_f1(*this, A);
		}

		f1 operator+(const f1& A) {
			return Intrinsic::add_f1(*this, A);
		}

		f1 operator-(const f1& A) {
			return Intrinsic::sub_f1(*this, A);
		}

		f1 operator*(const f1& A) {
			return Intrinsic::mul_f1(*this, A);
		}
		
		f1 operator=(const float Op) {
			value = _mm_load_ss(&Op);
			return *this;
		}


		// helper members
		// tests for greater than zero
		inline f1 greater0() {
			// intoduce a value to calculate 0.0f
			Intrinsic::f1 a;
			Intrinsic::f1 null = Intrinsic::xor_f1(a,a);

			return f1(Intrinsic::sub_f1(null, value));
		}
		
		// tests for less than zero
		inline f1 less0() {
			return f1(value);
		}

		// gets sign
		inline bool getSign() {
			unsigned long v;
			_mm_store_ss(reinterpret_cast<float*>(&v), value);
			return v >> 31;
		}



	};
#endif
#ifdef GCC
	TODO
#endif

	inline static f4 xor(f4 A, f4 B) {
#ifdef MSVC
		return _mm_xor_ps(A, B);
#endif
	}

	inline static f1 xor_f1(const f1& A, const f1& B) {
		return f1(_mm_xor_ps(A.value, B.value));
	}

	inline static f4 mul(f4 A, f4 B) {
		return _mm_mul_ps(A, B);
	}

	inline static f1 mul_f1(const f1& A, const f1& B) {
		return f1(_mm_mul_ss(A.value, B.value));
	}
	
	inline static f4 add(f4 A, f4 B) {
		return _mm_add_ps(A, B);
	}

	inline static f1 add_f1(const f1& A, const f1& B) {
		return f1(_mm_add_ss(A.value, B.value));
	}

	inline static f4 sub(f4 A, f4 B) {
		return _mm_sub_ps(A, B);
	}

	inline static f1 sub_f1(const f1& A, const f1& B) {
		return f1(_mm_sub_ss(A.value, B.value));
	}
	
	inline static f1 or_f1(const f1& A, const f1& B) {
		// no ss version available, try later maybe inline? 
		return f1(_mm_or_ps(A.value, B.value));
	}

	inline static f4 Rsqrt(f4 A) {
		return _mm_rsqrt_ps(A);
	}

	inline static void setZero(f4 &A) {
		A = _mm_xor_ps(A, A);
	}

		/*
	inline static unsigned long get(f4 A, unsigned long Index) {
		return A.m128_i32[Index];
	}*/

	inline static float get(f4 A, unsigned long Index) {
		return A.m128_f32[Index];
	}
	
	// TODO< optimize this? >
	inline static void set(f4 &A, unsigned long _0, unsigned long _1, unsigned long _2, unsigned long _3) {
		A.m128_i32[0] = _0;
		A.m128_i32[1] = _1;
		A.m128_i32[2] = _2;
		A.m128_i32[3] = _3;
	}

	inline static void set(f1 &A, float _0) {
		A.value.m128_f32[0] = _0;
	}

	inline static f4 set(float A, float B, float C, float D) {
		return _mm_set_ps(A, B, C, D);
	}
	
	// this loads one scalar float value and cleans all other places
	inline static f1 loadS(const float* Load) {
		return f1(_mm_load_ss(Load));
	}

	inline static f4 load(const f4 Load) {
		return _mm_load_ps(&(Load.m128_f32[0]));
	}

	// this spreads a scalar value to all other values and return it
	inline static f4 shuffle_spread(f1& A) {
		return _mm_shuffle_ps(A.value, A.value, _MM_SHUFFLE(0, 0, 0, 0));
	}

};

#endif
