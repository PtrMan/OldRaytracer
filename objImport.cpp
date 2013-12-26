#include "objImport.h"

#include "convert.h"

#include <fstream>
#include <string>

using namespace std;

void ObjImport::import(string Filename) {
	enum state {
		TOKEN,          // we are in the token state (what is the next line about?)
		
		VERTICLE_X,     // the next is the x coordinate of the verticle
		VERTICLE_Y,     // the next is the y coordinate of the verticle
		VERTICLE_Z,     // the next is the z coordinate of the verticle
		
		NOTHING,        // the next must be nothing (else throw a Error)

		VERTICLE_1,     // the next is the 1st Verticle of a triangle
		VERTICLE_2,     // the next is the 2nd Verticle of a triangle
		VERTICLE_3      // the next is the 3rd Verticle of a triangle
	};

	state  state;
	string line;
	string actualText; // is the actual text of the actual data
	
	float verticleX; // is the X Position of the Verticle
	float verticleY; // is the Y Position of the Verticle
	float verticleZ; // is the Z position of the Verticle
	
	unsigned long verticle1; // is the 1st verticle id of the face
	unsigned long verticle2; // is the 2nd verticle id of the face
	unsigned long verticle3; // is the 3rd verticle id of the face

	ifstream f(Filename.c_str());
	
	if ( !f ) {
		throw class CantOpenFile();
	}

	while( getline(f, line) ) {
		state      = TOKEN;
		actualText = "";

		for (size_t i = 0; i < line.size(); ++i) {
			char sign = line[i];
			
			if( sign == '#' ) {
				if( state == TOKEN ) {
					// its a comment, skip the line

					state = NOTHING;

					break;
				}
				else {
					// throw an Error (cos we don't except a comment in any other way)
					throw class ParseError();
				}
			}
			else if( sign == ' ' ) {
				if( state == VERTICLE_X || state == VERTICLE_Y || state == VERTICLE_1 || state == VERTICLE_2 ) {
					if( actualText == "" ) {
						// throw an error because we have a space and no Data
						throw class ParseError();
					}
					else {
						// translate the text into a float value and save it
						if( state == VERTICLE_X ) {
							verticleX = Convert::strToFloat(actualText);
							
							state      = VERTICLE_Y;
							actualText = "";
						}
						else if( state == VERTICLE_Y ) {
							verticleY = Convert::strToFloat(actualText);
							
							state      = VERTICLE_Z;
							actualText = "";
						}

						else if( state == VERTICLE_1 ) {
							verticle1 = Convert::strToULong(actualText) - 1;

							state      = VERTICLE_2;
							actualText = "";
						}
						else if( state == VERTICLE_2 ) {
							verticle2 = Convert::strToULong(actualText) - 1;

							state      = VERTICLE_3;
							actualText = "";
						}

						else {
							// throw an Error, cos this is not possible
							throw class designFlaw();
						}
					}
				}
				else if( state == TOKEN ) {
					if( actualText == "v" ) {
						// its a verticle
						
						state      = VERTICLE_X;
						actualText = "";
					}
					else if( actualText == "f" ) {
						// it is a face

						state      = VERTICLE_1;
						actualText = "";
					}
					else if( actualText == "usemtl" ) {
						// it is the material specification, we skip it
						// TODO< maybe use this? >

						state = NOTHING;
						break;
					}
					else if( actualText == "s" ) {
						// i don't know what this means, we skip it
						// TODO< maybe use this? >

						state = NOTHING;
						break;
					}
					else {
						// throw an Error (v is only at the beginning allowed)
						throw class ParseError();
					}
				}
				else {
					// throw an Error
					throw class ParseError();
				}
			}
			else {
				actualText += sign;
			}
		}
		
		if( state == VERTICLE_Z ) {
			verticleZ = Convert::strToFloat(actualText);

			// call the callback for a new verticle
			addVerticle(verticleX, verticleY, verticleZ);
		}
		else if( state == VERTICLE_3 ) {
			verticle3 = Convert::strToULong(actualText) - 1;

			// call the callback for a new face
			addFace(verticle1, verticle2, verticle3);
		}
		else if( state != NOTHING ) {
			// we have incomplete Data
			// throw an error
			throw class ParseError();
		}
	}
}
