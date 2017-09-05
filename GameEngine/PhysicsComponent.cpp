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

void PhysicsComponent::Update(float deltaTime)
{
	if (!bEnabled)
		return;

	velocity += impulse / mass;
	impulse = Vector3();
	parent->transform.Translate(velocity * deltaTime);

	Vector3 F;

	if (bEnabledGravity)
		F.y += -9.8 * mass;

	//Vector3 rot = parent->transform.rotation.ToEuler();
	//rot += angVelocity * deltaTime;
	//parent->transform.rotation = Quaternion::FromEuler(rot);

	//Vector3 rot;
	//float ang;
	//parent->transform.rotation.ToAxisAngle(rot, ang);
	
	parent->transform.rotation.Rotate(angVelocity.Normalized(), angVelocity.Magnitude() * deltaTime);

	//parent->transform.rotation.Rotate(Vector3(1, 0, 0), 1 * delta);
	//parent->transform.Rotate(Quaternion(0, 0.5 *delta, 0, 1).Normalized());

	//if(bEnabledDrag)
	//	F += -velocity.Normalized() * SpeedSquared() * drag;

	//angVelocity.Normalize();
	//parent->transform.rotation *= angVelocity;

	velocity += F / mass * deltaTime;
}

void PhysicsComponent::AddImpulse(Vector3 imp)
{
	this->impulse += imp;
}

void PhysicsComponent::AddImpulseAt(Vector3 imp, Vector3 pos)
{
	
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