#include "quaternion.h"
#include <math.h>

#define TOLERANCE 0.00001f

void quaternion_normalise(QUATERNION *q)
{
	VFLOAT mag;
	VFLOAT mag2 = q->w * q->w + q->x * q->x + q->y * q->y + q->z * q->z;
	// Don't normalize if we don't have to
	if (fabs(mag2 - 1.0f) > TOLERANCE) {
		mag = sqrt(mag2);
		q->w /= mag;
		q->x /= mag;
		q->y /= mag;
		q->z /= mag;
	}
}

void quaternion_multiply( QUATERNION *r, QUATERNION *a, QUATERNION *b )
{
	r->x = a->w * b->x + a->x * b->w + a->y * b->z - a->z * b->y;
	r->y = a->w * b->y + a->y * b->w + a->z * b->x - a->x * b->z;
	r->z = a->w * b->z + a->z * b->w + a->x * b->y - a->y * b->x;
	r->w = a->w * b->w - a->x * b->x - a->y * b->y - a->z * b->z;
}


void quaternion_from_axisangle( QUATERNION *q, VECTOR3 *axis, VFLOAT angle)
{
	double sin_a = sin( angle / 2 );
	double cos_a = cos( angle / 2 );

	q->x = axis->x * sin_a;
	q->y = axis->y * sin_a;
	q->z = axis->z * sin_a;
	q->w = cos_a;

	quaternion_normalise( q );
}

void quaternion_from_euler( QUATERNION *q, VFLOAT rx, VFLOAT ry, VFLOAT rz )
{
	VECTOR3 vx = { 1, 0, 0 }, vy = { 0, 1, 0 }, vz = { 0, 0, 1 };
	QUATERNION qx, qy, qz, qt;

	quaternion_from_axisangle( &qx, &vx, rx );
	quaternion_from_axisangle( &qy, &vy, ry );
	quaternion_from_axisangle( &qz, &vz, rz );

	quaternion_multiply( &qt, &qx, &qy );
	quaternion_multiply( q,  &qt, &qz );
}

void quaternion_to_axisangle(VECTOR3 *axis, VFLOAT *angle, QUATERNION *q)
{
	float scale = sqrt(q->x * q->x + q->y * q->y + q->z * q->z);
	axis->x = q->x / scale;
	axis->y = q->y / scale;
	axis->z = q->z / scale;
	*angle = acos(q->w) * 2.0f;
}

