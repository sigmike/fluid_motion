typedef double VFLOAT;

typedef struct {
	VFLOAT x, y, z;
} VECTOR3;

typedef struct {
	VFLOAT x, y, z, w;
} QUATERNION;

void quaternion_normalise(QUATERNION *q);
void quaternion_multiply( QUATERNION *r, QUATERNION *a, QUATERNION *b );
void quaternion_from_axisangle( QUATERNION *q, VECTOR3 *axis, VFLOAT angle);
void quaternion_from_euler( QUATERNION *q, VFLOAT rx, VFLOAT ry, VFLOAT rz );
void quaternion_to_axisangle(VECTOR3 *axis, VFLOAT *angle, QUATERNION *q);
