#include "Rigidbody.h"
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

	parent->transform.Rotate(Quaternion::FromEuler(angularVelocity * deltaTime));

	//Vector3 rot = parent->transform.rotation.ToEuler();
	//rot += angularVelocity * deltaTime;
	//parent->transform.rotation = Quaternion::FromEuler(rot);

	//parent->transform.rotation.Rotate(angularVelocity.Normalized(), angularVelocity.Magnitude() * deltaTime);

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