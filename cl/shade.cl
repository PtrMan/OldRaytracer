
struct opencl_shading_in {
	// the view vector
	// is not normalisized
	float Vx;
	float Vy;
	float Vz;

	float Nx;
	float Ny;
	float Nz;
	
	unsigned int contentPtr;

	float posX;
	float posY;
	float posZ;
};

struct opencl_shading_out {
	float r;
	float g;
	float b;
};



typedef struct {
	float x;
	float y;
	float z;
} float3;

struct opencl_struct_additional {
	float light0_posx;
	float light0_posy;
	float light0_posz;
	
	float light0_vecx;
	float light0_vecy;
	float light0_vecz;

	float light0_spot;
};

// Perlin Noise
// {

inline float fade(float t) { 
	return t * t * t * (t * (t * 6.0f - 15.0f) + 10.0f);
}

inline float lerp(float t, float a, float b) { 
	return a + t * (b - a);
}

float grad(unsigned int hash, float x, float y, float z) {
	unsigned int h = hash & 15;

	float u = (((h<8)||(h==12)||(h==13)) ? x : y), v = (((h < 4)||(h == 12)||(h == 13)) ? y : z);
	return (((h & 1) == 0) ? u : -u) + (((h&2) == 0) ? v : -v);
}

float perlinNoise(float x, float y, float z, __constant unsigned int* p) {
	unsigned int X = ((int)floor(x)) & 255,
	             Y = ((int)floor(y)) & 255,
	             Z = ((int)floor(z)) & 255;
	
	x -= floor(x);
	y -= floor(y);
	z -= floor(z);
	
	float u = fade(x),
	      v = fade(y),
	      w = fade(z);
	
	unsigned int A  = p[X]+Y,
	             AA = p[A]+Z,
	             AB = p[A+1]+Z,
	             B  = p[X+1]+Y,
	             BA = p[B]+Z,
	             BB = p[B+1]+Z;
	
	return lerp(w, lerp(v, lerp(u, grad(p[AA], x, y, z),  
	                    grad(p[BA], x-1, y, z)),
	               lerp(u, grad(p[AB], x, y-1, z),
	                    grad(p[BB], x-1, y-1, z))),
	       lerp(v, lerp(u, grad(p[AA+1], x, y, z-1),
	                    grad(p[BA+1], x-1, y, z-1)),
	               lerp(u, grad(p[AB+1], x, y-1, z-1 ),
	                    grad(p[BB+1], x-1, y-1, z-1 ))));
}

// }

float dot3(float3 A, float3 B) {
	return A.x*B.x + A.y*B.y + A.z*B.z;
}

// this calculates the dot product of an float3 value and 3 components
#define dot3_v3(a, bx, by, bz) ((a.x)*(bx) + (a.y)*(by) + (a.z)*(bz))

// shading
//  N: normal
//  L: normalisized light vector (vector to the light)
float lambertian (float3 N, float3 L) {
	return dot3(N, L);
}

#define len3(A) sqrt((A.x)*(A.x) + (A.y)*(A.y) + (A.z)*(A.z))
/*float len3(float3 A) {
	return sqrt(A.x*A.x + A.y*A.y + A.z*A.z);
}*/

float3 norm3(float3 A) {
	float l = 1.0f/len3(A); // TODO< faster math functions
	float3 ret;
	ret.x = A.x*l;
	ret.y = A.y*l;
	ret.z = A.z*l;
	
	return ret;
}

float3 sub3(float3 A, float3 B) {
	float3 ret;
	ret.x = A.x - B.x;
	ret.y = A.y - B.y;
	ret.z = A.z - B.z;
	
	return ret;
}

// Oren-Nayar
// source: http://en.wikipedia.org/wiki/Oren%E2%80%93Nayar_Reflectance_Model
float shading_OrenNayar(float3 Light, float3 Eye, float3 Normal) {
	// calculate A and B
	// TODO< this can be precalculated outside >
	// sigma is the roughness of the surface
	float sigma = 0.5f;
	
	float A = 1.0f - 0.5f*((sigma*sigma)/(sigma*sigma+0.33f));
	float B = 0.45f * ((sigma*sigma)/(sigma*sigma + 0.09f));
	
	// light is the normalisized direction to the light
	// eye is the normalisized direction to the eye
	
	// calculate the cosine of theta(i)
	float cos_thetaI = dot3(Normal, Light);
	float cos_thetaO = dot3(Normal, Eye);
	
	// calculate the angle of thetaI and thetaO
	float thetaI = acos(cos_thetaI);
	float thetaO = acos(cos_thetaO);
	
	// calculate alpha and beta
	float alpha = fmax(thetaI, thetaO);
	float beta  = fmax(thetaI, thetaO);
	
	// calculate the lighting
	return cos_thetaI * (A + B*fmax(0.0f, cos(thetaI-thetaO))*sin(alpha)*tan(beta));
}



__kernel void shade(
	__global struct opencl_shading_in* in,
	__global struct opencl_shading_out* out,
	__constant unsigned int* perlinNoiseArray,
	__write_only image2d_t screen,
	__constant struct opencl_struct_additional* additional
) {
	uint id = (get_global_id(0) + get_global_id(1)*1024);
	
	float4 outColor = (float4)(0.0f, 0.0f, 0.0f, 0.0f);

	// N : normal of the surface of that pixel
	// P : position of that Pixel
	// View : is the (unormalisized) view vector
	// Eye  : is the normalisized eye vector (from point to the eye)
	float3 N;
	float3 P;
	float3 View;
	float3 Eye;
	
	// unpack the Normal vector
	N.x = in[id].Nx;
	N.y = in[id].Ny;
	N.z = in[id].Nz;
	
	P.x = in[id].posX;
	P.y = in[id].posY;
	P.z = in[id].posZ;
	
	View.x = in[id].Vx;
	View.y = in[id].Vy;
	View.z = in[id].Vz;
	
	// calculate the Eye vector
	Eye.x = -View.x;
	Eye.y = -View.y;
	Eye.z = -View.z;

	Eye = norm3(Eye);

	outColor.x = 0.0f;
	outColor.y = 0.0f;
	outColor.z = 0.0f;
	
	
	if( in[id].contentPtr != 1 ) {
		// calculate the difference between the hit-point and the position of the light
		float3 diff_a = (float3){	additional[0].light0_posx - P.x, additional[0].light0_posy - P.y, additional[0].light0_posz - P.z };

		float diff_a_length = -1.0f/len3(diff_a);
		// this is the diff for the spot-light-calculations
		diff_a.x *= diff_a_length;
		diff_a.y *= diff_a_length;
		diff_a.z *= diff_a_length;
		
		// this is the difference for the light calculations
		float3 diff_b = (float3){-diff_a.x, -diff_a.y, -diff_a.z};
		
		float light;

		// TODO< take the "field of view" into account >
		// calculate the degree of the diff vector and the "field of view" vector of the spotlight
		float degree0 = dot3_v3(diff_a, additional[0].light0_vecx, additional[0].light0_vecy, additional[0].light0_vecz);
		float degree0_rad = acos(degree0);
		if( (degree0 > 0.0f) 
							 && (degree0_rad > additional[0].light0_spot) ) {
			// calculate lambertin lighting
			light = lambertian(N, diff_b);
			light = fmax(light, 0.0f);
		}
		else {
			// old code
			//light = 0.0f;
			
			// new code
				light = lambertian(N, diff_b);
			light = fmax(light, 0.0f);
		
		}

		
		outColor.x = light/* * 255.0f*/;
		outColor.y = light/* * 255.0f*/;
		outColor.z = light/* * 255.0f*/;
	}

	// write the color to the screen
	
	write_imagef(
		screen,
		(int2)(get_global_id(0), get_global_id(1)),
		outColor
	);
}
