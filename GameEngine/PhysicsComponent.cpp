#include "PhysicsComponent.h"
#include "GameObject.h"

PhysicsComponent::PhysicsComponent()
{
	mass = 1;
	drag = 0.01;

	bEnabled = 1;
	bEnabledGravity = 1;
	bEnabledDrag = 1;
}

void PhysicsComponent::Start()
{
}

void PhysicsComponent::Update(float delta)
{
	if (!bEnabled)
		return;

	velocity += impulse;
	impulse = Vector();
	parent->transform.Translate(velocity * delta);

	Vector F;

	if (bEnabledGravity)
		F.y += -9.8 * mass;

	parent->transform.Rotate(angVelocity);

	//if(bEnabledDrag)
	//	F += -velocity.Normalized() * SpeedSquared() * drag;

	velocity += F / mass * delta;
}

void PhysicsComponent::AddImpulse(Vector imp)
{
	this->impulse += imp;
}

float PhysicsComponent::Speed()
{
	return velocity.Magnitude();
}

float PhysicsComponent::SpeedSquared()
{
	float s = velocity.Magnitude();
	return s * s;
}