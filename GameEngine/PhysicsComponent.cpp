#include "PhysicsComponent.h"
#include "GameObject.h"



class Matrix6x6
{
public:
	float elem[36];

	Matrix6x6()
	{
		elem[0] = 1;
		elem[7] = 1;
		elem[14] = 1;
		elem[21] = 1;
		elem[28] = 1;
		elem[35] = 1;
	}

	Matrix6x6 operator*(const Matrix6x6 rhs)
	{

	}
};






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

	//parent->transform.rotation.Rotate(Vector(1, 0, 0), 1 * delta);
	//parent->transform.Rotate(Quaternion(0, 0.5 *delta, 0, 1).Normalized());


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