#ifndef _H_STACK
#define _H_STACK

template<unsigned long Size, typename Type>
class Stack {
   public:
	// this is a class that gets throwed if the Stack overflows
	class Overflow {
	};
	
	Stack() {
		count = 0;
	}
	
	// this pushs a Element
	// if the total count of the Elements exceeds the Size it throws Overflow()
	void push(Type Data) {
		if( count+1 > Size ) {
			throw class Overflow();
		}

		data[count++] = Data;
	}

	// this tries to pop a Element
	// if no element can be poped it returns NULL
	Type pop() {
		if( count == 0 ) {
			return NULL;
		}
		return data[--count];
	}

	// this method "deletes" all objects from the stack
	void flush() {
		count = 0;
	}

   protected:
	unsigned long count; // is the count of allocated elements
	Type data[Size];
};

#endif
