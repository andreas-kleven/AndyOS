#include "BoxCollider.h"
#include "GameObject.h"

BoxCollider::BoxCollider()
{
	bIsBox = 1;
}

bool BoxCollider::IsColliding(BoxCollider* other)
{
	Vector3 a = parent->GetWorldPosition() + transform.position;
	Vector3 b = other->parent->GetWorldPosition() + other->transform.position;

	Vector3 T = b - a;

	Quaternion rotA = parent->GetWorldRotation();
	Quaternion rotB = other->parent->GetWorldRotation();

	Vector3 axes[15];
	axes[0] = rotA * Vector3(1, 0, 0);
	axes[1] = rotA * Vector3(0, 1, 0);
	axes[2] = rotA * Vector3(0, 0, 1);
	axes[3] = rotB * Vector3(1, 0, 0);
	axes[4] = rotB * Vector3(0, 1, 0);
	axes[5] = rotB * Vector3(0, 0, 1);

	axes[6] = Vector3::Cross(axes[0], axes[3]);
	axes[7] = Vector3::Cross(axes[0], axes[4]);
	axes[8] = Vector3::Cross(axes[0], axes[5]);

	axes[9] = Vector3::Cross(axes[1], axes[3]);
	axes[10] = Vector3::Cross(axes[1], axes[4]);
	axes[11] = Vector3::Cross(axes[1], axes[5]);

	axes[12] = Vector3::Cross(axes[2], axes[3]);
	axes[13] = Vector3::Cross(axes[2], axes[4]);
	axes[14] = Vector3::Cross(axes[2], axes[5]);

	for (int i = 0; i < 15; i++)
	{
		Vector3 L = axes[i];

		float R = abs(Vector3::Dot(T, L));

		float P1a = abs(Vector3::Dot(axes[0] * parent->transform.scale.x, L));
		float P1b = abs(Vector3::Dot(axes[1] * parent->transform.scale.y, L));
		float P1c = abs(Vector3::Dot(axes[2] * parent->transform.scale.z, L));

		float P2a = abs(Vector3::Dot(axes[3] * other->parent->transform.scale.x, L));
		float P2b = abs(Vector3::Dot(axes[4] * other->parent->transform.scale.y, L));
		float P2c = abs(Vector3::Dot(axes[5] * other->parent->transform.scale.z, L));

		//Debug::Print("%f\t%f\n", R, P1a + P1b + P1c + P2a + P2b + P2c);

		if (R > P1a + P1b + P1c + P2a + P2b + P2c)
			return 0;
	}

	return 1;
}