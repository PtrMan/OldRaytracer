#ifndef H_NSTIMER
#define H_NSTIMER

// this is a class with that we can get the timing Nano-Secound exactly (cpu-clocks)(none-the-less there is some overhead in calling this functions, but nevermind =)
template<bool Active> class NSTimer {
   public:
	// delta time
	unsigned long long delta;
	
	// the sum
	unsigned long long sum;
	
	// this adds the delta to the sum
	inline void addSum() {
		sum += delta;
	}

	inline void startTime() {
		if( Active ) {
			unsigned long sHigh_;
			unsigned long sLow_;

			__asm {
				CPUID; // just flush the cpu pipeline
				RDTSC; // get time
				// high time is in edx, low is in eax
				
				MOV sHigh_, edx;
				MOV sLow_, eax;
			}

			sLow = sLow_;
			sHigh = sHigh_;
		}
	}
	
	inline void stopTime() {
		if( Active ) {	
			unsigned long dLow = sLow;
			unsigned long dHigh = sHigh;

			__asm {
				CPUID; // just flush the cpu pipeline
				RDTSC; // get time
				// high time is in edx, low is in eax

				// calculate difference
				//SUB dHigh, edx;
				//SBB dLow, eax;
				SUB edx, dHigh;
				SBB eax, dLow;

				MOV dLow, eax;
				MOV dHigh, edx;
			}
	
			// compress the delta times into one long long value
			delta = (static_cast<unsigned long long>(dHigh) << 32) | static_cast<unsigned long long>(dLow);
		}
	}


   protected:
	// start times
	unsigned long sLow;
	unsigned long sHigh;
};

#endif