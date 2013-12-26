#ifndef _H_varray
#define _H_varray

template <typename Typ> class VArray {
	
	// this is a class that is throwed if no memory is availiable
	class NoMemory {
	};
   public:
	VArray() {
		elementsC = 0;
		allocated = 50;

		elements = reinterpret_cast<Typ*>( malloc(sizeof(Typ) * 50) );
		
		// test the memory for NULL-ptr 
		if( elements == NULL ) {
			throw class NoMemory();
		}
	}

	~VArray() {
		if( elements != NULL ) {
			free(reinterpret_cast<void*>(elements));
			elements = NULL;
		}
	}
	
	// this appends a Element at the end
	void append(Typ Element) {
		if( allocated == elementsC ) {
			// reallocate it
			elements = reinterpret_cast<Typ*>( realloc(reinterpret_cast<void*>(elements), sizeof(Typ) * (allocated + 50)) );
			
			// test the memory for NULL-ptr 
			if( elements == NULL ) {
				throw class NoMemory();
			}

			allocated += 50;
		}

		elements[elementsC++] = Element;
	}
	
	// this "flushs" the Array, set Elements-count to 0
	void flush() {
		elementsC = 0;
	}

	inline unsigned long getCount() {
		return elementsC;
	}

	Typ* elements;
  protected:
	unsigned long elementsC; // is the count of the Elements
	unsigned long allocated; // is thew count of the allocated Elements
};

#endif
