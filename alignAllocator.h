#ifndef _H_ALIGNALLOCATOR
#define _H_ALIGNALLOCATOR

// this is a template for a class that Allocate Align Data blocks of one type
// Mask      : is the bit-mask for the allocation of the aligned Memory
// AlignSize : is the number in bytes of the align size
// Type      : is the to align Type
template<unsigned long Mask, unsigned long AlignSize, typename Type> class AlignAllocator {
	static const unsigned long batchSize = 50; // how much do we additionaly allocate if no more space is there?

   public:
	// this is a class that gets throwed if no memory is anymore available
	class NoMemory {
	};
	
	// this is a class that gets throwed if the Position of the Memory was changed
	class MemoryPointerLocked {
	};
	
	// this is the Constructor
	// he can throw the class NoMemory
	AlignAllocator() {
		allocSize = batchSize;
		usedCount = 0;
		mpLocked = false;

		// calculate the alignedSize
		ElementSize = ((sizeof(Type) + AlignSize) / AlignSize) * AlignSize;

		rawMemory = malloc(ElementSize * allocSize + AlignSize * 2);

		if( !rawMemory ) {
			// throw a Error cos of too less memory
			throw class NoMemory();
		}

		actualPtr = alignedMemory = reinterpret_cast<void*>((reinterpret_cast<unsigned long>(rawMemory) & Mask) + AlignSize);
	}

	~AlignAllocator() {
		free( rawMemory );
	}
	
	// this Locks the memory, so it don't allow allocations that change the pointers (is a allocation with a different memory pointer)
	void setLock(bool Locked = true) {
		mpLocked = Locked;
	}
	
	// this reallocate the memory that you can use up to "Count" Elements without a reallocation
	// sets the acutal Ptr to the begin

	// throws NoMemory if no memory is availiable
	// throws MemoryPointerLocked if the Memory position has changed
	void reallocateElements(unsigned long Count) {
		void* alignedMemory2;
		
		rawMemory = realloc(rawMemory, ElementSize * (Count) + AlignSize * 2);
		
		if( !rawMemory ) {
			throw class NoMemory();
		}

		alignedMemory2 = reinterpret_cast<void*>( (reinterpret_cast<unsigned long>(rawMemory) & Mask) + AlignSize);
		actualPtr = alignedMemory2;
		
		if( alignedMemory2 != alignedMemory ) {
			alignedMemory = alignedMemory2;

			if( mpLocked ) {
					throw class MemoryPointerLocked();
			}
		}
	}

	// this allocates a new Element and return the aligned pointer at it
	// if it throws MemoryPointerLocked you must reset the allocator and do allocate all elements againt
	
	// throws NoMemory if no more memory is avaiable
	// throws MemoryPointerLocked if the Memory position has changed

	// WARNING< if you dont Lock the allocator it drop silent a memory-position change >
	Type* allocElement() {
		void* ret; // is the value that is to be returned
		void* alignedMemory2;

		if( usedCount == allocSize ) { // NOTE< we don't need > or >= cos it's impossible >
			rawMemory = realloc(rawMemory, ElementSize * (allocSize + batchSize) + AlignSize * 2);
			
			if( !rawMemory ) {
				throw class NoMemory();
			}

			alignedMemory2 = reinterpret_cast<void*>( (reinterpret_cast<unsigned long>(rawMemory) & Mask) + AlignSize);

			allocSize += batchSize;
			
			if( alignedMemory != alignedMemory ) {
				alignedMemory = alignedMemory2;
				
				if( mpLocked ) {
					throw class MemoryPointerLocked();
				}

				actualPtr = alignedMemory;
			}
		}
		
		ret = actualPtr;

		actualPtr = reinterpret_cast<void*>(reinterpret_cast<unsigned long>(actualPtr) + ElementSize);
		usedCount++;

		return reinterpret_cast<Type*>( ret );
	}
	
	// this "resets" the allocations
	// (is like a super-cheap freeing of memory, only a "bit faster)
	// NOTE< you can't use after this _any_ memory allocated with this object (you have just to reallocate all things) >
	inline void reset() {
		actualPtr = alignedMemory;
		usedCount = 0;
	}

	// is a overloaded operator that returns the pointer to an element
	// is a "bit" slower than the iterator thing
	inline Type* operator[](unsigned long Index) {
		// TODO< check the Index with my own assert thing, maybe with a template parameter? as a "check option"? >

		return reinterpret_cast<Type*>(reinterpret_cast<unsigned long>(alignedMemory) + ElementSize * Index);
	}
	
	// this returns the count of elements
	inline unsigned long getSize() {
		return usedCount;
	}

   private:
	void* rawMemory;         // is the real memory pointer
	void* alignedMemory;
	void* actualPtr;         // is the pointer at the actual (last allocated) Element
	
	unsigned long allocSize;   // is the size that was got allocated
	unsigned long usedCount;   // is the count of the Elements that are allocated for the application
	unsigned long ElementSize; // is the size in bytes of an aligned Element

	bool mpLocked; // is the memory-pointer locked (means that a reallocation can't change the pointer, if it does the AllocateElement member throws a MemoryPointerLocked exception)
	               // Tip: just use the reallocateElements member to allocate enougth memory before you use it
};

#endif
