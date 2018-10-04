#include "Components/Rigidbody.h"
#include "GameObject.h"

Rigidbody::Rigidbody()
{
	mass = 1;
	drag = 0.01;

	bEnabled = 1;
	bEnabledGravity = 1;
	bEnabledDrag = 1;
}

void Rigidbody::Start()
{
}

void Rigidbody::Update(float deltaTime)
{
	if (!bEnabled)
		return;

	velocity += impulse / mass;
	impulse = Vector3();
	parent->transform.Translate(velocity * deltaTime);

	Vector3 F;

	if (bEnabledGravity)
		F.y += -9.8 * mass;

	Quaternion eulerRot = Quaternion::FromEuler(angularVelocity * deltaTime);
	Quaternion& rot = parent->transform.rotation;
	rot *= (rot.Inverse() * eulerRot * rot);
	rot.Normalize();

	//if(bEnabledDrag)
	//	F += -velocity.Normalized() * SpeedSquared() * drag;

	velocity += F / mass * deltaTime;
}

void Rigidbody::AddImpulse(Vector3 imp)
{
	this->impulse += imp;
}

void Rigidbody::AddImpulseAt(Vector3 imp, Vector3 pos)
{

}

float Rigidbody::Speed()
{
	return velocity.Magnitude();
}

float Rigidbody::SpeedSquared()
{
	float s = velocity.Magnitude();
	return s * s;
}