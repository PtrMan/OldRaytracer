#ifndef _H_ALIGNSTACK
#define _H_ALIGNSTACK

#include "alignAllocator.h"

template<unsigned long Size, typename Type>
class AlignStack {
   public:
	// this is a class that gets throwed if the Stack overflows
	class Overflow {
	};
	
	AlignStack() {
		data.reallocateElements(Size);

		count = 0;
	}
	~AlignStack() {
	}

	// this pushs a Element
	// if the total count of the Elements exceeds the Size it throws Overflow()
	void push(Type Data) {
		if( count+1 > Size ) {
			throw class Overflow();
		}

		*(data[count++]) = Data;
	}

	// this tries to pop a Element
	// if no element can be poped it returns NULL
	Type pop() {
		if( count == 0 ) {
			return NULL;
		}
		return *(data[count--]);
	}

	// this method "deletes" all objects from the stack
	void flush() {
		count = 0;
	}
   protected:
	AlignAllocator<0xffffff00, 256, Type> data;
	unsigned long count; // is the count of allocated elements
};

#endif
