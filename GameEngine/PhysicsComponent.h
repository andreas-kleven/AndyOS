#pragma once
#include "Component.h"

class PhysicsComponent : public Component
{
public:
	Vector velocity;
	Vector angVelocity;

	float mass;
	float drag;

	bool bEnabled;
	bool bEnabledGravity;
	bool bEnabledDrag;

	PhysicsComponent();

	virtual void Start();
	virtual void Update(float delta);

	void AddImpulse(Vector imp);

	float Speed();
	float SpeedSquared();

private:
	Vector impulse;
};