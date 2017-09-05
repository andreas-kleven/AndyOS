#pragma once
#include "Component.h"
#include "Matrix3.h"

class PhysicsComponent : public Component
{
public:
	Vector3 velocity;
	Vector3 angVelocity;

	Matrix3 rotInertia;

	float mass;
	float drag;

	bool bEnabled;
	bool bEnabledGravity;
	bool bEnabledDrag;

	PhysicsComponent();

	virtual void Start();
	virtual void Update(float delta);

	void AddImpulse(Vector3 imp);
	void AddImpulseAt(Vector3 imp, Vector3 pos);

	float Speed();
	float SpeedSquared();

private:
	Vector3 impulse;
};