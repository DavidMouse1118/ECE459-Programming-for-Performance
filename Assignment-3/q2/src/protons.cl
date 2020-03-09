#define coulombConstant 8.99 * pow(10.0f, 9.0f)
#define chargeProton    1.60217662 * pow(10.0f, -19.0f)
#define chargeElectron  -chargeProton
#define massProton      1.67262190 * pow(10.0f, -27.0f)
#define massElectron    9.10938356 * pow(10.0f, -31.0f)

#define PROTON 'p'
#define ELECTRON 'e'

float getCharge(char type) {
	if (type == ELECTRON) {
		return chargeElectron;
	} else {
		return chargeProton;
	}
}

float getMass(char type) {
	if (type == ELECTRON) {
		return massElectron;
	} else {
		return massProton;
	}
}

float magnitude(float3 vec) {
    return sqrt((float)(pow(vec.x, 2.0f) + pow(vec.y, 2.0f) + pow(vec.z, 2.0f)));
}

float print_float3(float3 vec) {
	printf("%.10e, %.10e, %.10e\n", vec.x, vec.y, vec.z);
}

float3 normal(float3 a) {
    float factor = 1.0f / magnitude(a);
    return a * factor;
}

__kernel void computeForces(
	__global float *h,
	__global const char *types,
	__global float3 *positions,
	__global float3 *forces
) {
	int id = get_global_id(0);
	
	if (types[id] == ELECTRON) {
		for (int i = 0; i < get_global_size(0); i++) {
			if (i == id) {
				continue;
			}

			float3 direction = positions[id] - positions[i];
			float q1 = getCharge(types[id]);
			float q2 = getCharge(types[i]);
			float r = magnitude(direction);

			forces[id] += (float3) normal(direction) * (float) (coulombConstant * q1 * q2 / pow(r, 2.0f));
		}
		// if (id == 49) {
		// 	printf("computeForces\n");
		// 	print_float3(positions[id]);
		// 	print_float3(forces[id]);
		// }
	}
}

__kernel void computePositions(
	__global float *h,
	__global const char *types,
	__global float3 *forces_0,
	__global float3 *forces_1,
	__global float3 *positions
) {
	int id = get_global_id(0);

	// printf("%d\n", id);

	float mass = getMass(types[id]);
	// printf("%.40f\n", mass);

	float3 f0 = forces_0[id];
	float3 f1 = forces_1[id];

	// print_float3(f0);
	// print_float3(f1);

	float3 avgForce = (f0 + f1) / 2.0f;
	// print_float3(avgForce);
	// printf("pow(*h, 2.0f) %f\n", pow(*h, 2.0f));
	// printf("%d\n", avgForce.x);
	float3 deltaDist = avgForce * pow(*h, 2.0f) / mass;

	// printf("%d\n", deltaDist.x);
	positions[id] += deltaDist;
	// if (id == 49) {
	// 	printf("computeApproxPositions\n");
	// 	print_float3(f0);
	// 	print_float3(f1);
	// 	print_float3(positions[id]);
	// }
}

__kernel void isErrorAcceptable(
	const float errorTolerance,
	__global float3 *positions_0,
	__global float3 *positions_1,
	__global int *success
) {
	int id = get_global_id(0);
	// printf("errorTolerance: %.10f\n", errorTolerance);
	// printf("success: %d\n", *success);
	// printf("positions_0: ");
	// print_float3(positions_0[id]);
	// printf("positions_1: ");
	// if (id == 49) {
	// 	printf("isErrorAcceptable\n");
	// 	print_float3(positions_0[id]);
	// 	print_float3(positions_1[id]);
	// 	print_float3(positions_0[id] - positions_1[id]);
	// 	printf("magnitude(positions_0[id] - positions_1[id]): %.10f\n", magnitude(positions_0[id] - positions_1[id]));
	// }


	if (magnitude(positions_1[id] - positions_0[id]) > errorTolerance) {
		*success = 0;
	}
}