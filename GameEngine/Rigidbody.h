#pragma once
#include "../GL/Matrix3.h"
#include "Component.h"
#include "Collider.h"

class Rigidbody : public Component
{
public:
	Collider* collider;

	Vector3 velocity;
	Vector3 angularVelocity;

	Matrix3 rotInertia;

	float mass;
	float drag;

	bool bEnabled;
	bool bEnabledGravity;
	bool bEnabledDrag;

	Rigidbody();

	virtual void Start();
	virtual void Update(float delta);

	void AddImpulse(Vector3 imp);
	void AddImpulseAt(Vector3 imp, Vector3 pos);

	float Speed();
	float SpeedSquared();

private:
	Vector3 impulse;
};