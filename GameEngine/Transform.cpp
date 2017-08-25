#include "Transform.h"
#include "string.h"
#include "stdio.h"

Transform::Transform()
{
	this->position = Vector();
	this->rotation = Vector();
	this->scale = Vector(1, 1, 1);
}

Transform::Transform(Vector pos, Vector rot, Vector scale)
{
	this->position = pos;
	this->rotation = rot;
	this->scale = scale;
}

Vector Transform::GetRightVector()
{
	return Vector(cos(rotation.y), 0, sin(rotation.y));
}

Vector Transform::GetUpVector()
{
	float sinPitch = sin(rotation.x);
	float cosPitch = cos(rotation.x);
	float sinYaw = sin(rotation.y);
	float cosYaw = cos(rotation.y);

	return Vector(sinPitch * sinYaw, cosPitch, -sinPitch * cosYaw);;
}

Vector Transform::GetForwardVector()
{
	float sinPitch = sin(rotation.x);
	float cosPitch = cos(rotation.x);
	float sinYaw = sin(rotation.y);
	float cosYaw = cos(rotation.y);

	return Vector(-cosPitch * sinYaw, sinPitch, cosPitch * cosYaw);
}


char* Transform::ToString(char* buf)
{
	vprintf(buf, "[%f, %f, %f] [%f, %f, %f] [%f, %f, %f]",
		position.x, position.y, position.z,
		rotation.x, rotation.y, rotation.z,
		scale.x, scale.y, scale.z);

	return buf;
}
