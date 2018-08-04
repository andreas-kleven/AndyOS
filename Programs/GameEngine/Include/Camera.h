#pragma once
#include "GameObject.h"

class Camera : public GameObject
{
	Vector3 euler_angles;

public:
	float speed;

	Camera();

	void RotateEuler(const Vector3& rot);
};