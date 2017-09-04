#pragma once
#include "Component.h"

class PhysicsComponent : public Component
{
public:
	Vector3 velocity;
	Quaternion angVelocity;

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