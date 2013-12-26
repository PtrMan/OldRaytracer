#ifndef _H_OBJIMPORT
#define _H_OBJIMPORT

#include "convert.h"

#include <string>

using namespace std;

class ObjImport {
   public:
	
	// this is a clas that the import method throws if it can't open the File
	class CantOpenFile {
	};

	// this is a class for the throwing if a parse Error occur
	class ParseError {
	};

	// this is a error that gets only thrown if the design is terrible wrong (should not happen ;) )
	class designFlaw {
	};

	// this trys to import a .obj file
	// it throws CantOpenFile if it can't open the file and ParseError if the parsing went wrong
	void import(string Filename);
	
   protected:
	// this is called by the import() method if a new Verticle was added
	virtual void addVerticle(float PosX, float PosY, float PosZ) = 0;

	// this is called by the import() method if a new Face was added
	virtual void addFace(unsigned long Verticle1, unsigned long Verticle2, unsigned long Verticle3) = 0;
};

#endif