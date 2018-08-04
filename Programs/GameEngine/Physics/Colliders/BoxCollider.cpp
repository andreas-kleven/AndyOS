#include "Components/BoxCollider.h"
#include "GameObject.h"
#include "GL.h"

BoxCollider::BoxCollider()
{
	bIsBox = 1;
}

Vector3 SupportFunction(Vector3 axis, Transform trans)
{
	Vector3 points[] = {
		Vector3(-1, -1, -1),
		Vector3(-1, -1, 1),
		Vector3(-1, 1, -1),
		Vector3(-1, 1, 1),
		Vector3(1, -1, -1),
		Vector3(1, -1, 1),
		Vector3(1, 1, -1),
		Vector3(1, 1, 1)
	};

	Matrix3 R = Matrix3::CreateRotation(trans.rotation.ToEuler());

	for (int i = 0; i < 8; i++)
		points[i] = R * points[i] * 2 + trans.position;

	int n_points = 8;

	unsigned int best = 0;
	float best_dot = Vector3::Dot(points[0], axis);

	for (unsigned int i = 1; i < n_points; i++)
	{
		float d = Vector3::Dot(points[i], axis);
		if (d > best_dot)
		{
			best = i;
			best_dot = d;
		}
	}

	return points[best];
}

//return a vector perpendicular to a and
//parallel to (and in the direction of) b
Vector3 cross_aba(const Vector3 &a, const Vector3 &b)
{
	return Vector3::Cross(Vector3::Cross(a, b), a);
}

Vector3 v;
int n;
Vector3 c;
Vector3 b;
Vector3 d;

bool update(Vector3 a)
{
	if (n == 0)
	{
		b = a;
		v = -a;
		n = 1;
	}

	if (n == 1)
	{
		v = cross_aba(b - a, -a);

		c = b;
		b = a; //silly, yes, we'll come back to this
		n = 2;

		//can't possibly contain the origin unless we've
		//built a tetrahedron, so just return false
		return 0;
	}

	if (n == 2)
	{
		Vector3 ao = -a; //silly, yes, clarity is important

					  //compute the vectors parallel to the edges we'll test
		Vector3 ab = b - a;
		Vector3 ac = c - a;

		//compute the triangle's normal
		Vector3 abc = Vector3::Cross(ab, ac);

		//compute a vector within the plane of the triangle,
		//pointing away from the edge ab
		Vector3 abp = Vector3::Cross(ab, abc);

		if (Vector3::Dot(abp, ao) > 0)
		{
			//the origin lies outside the triangle,
			//near the edge ab
			c = b;
			b = a;

			v = cross_aba(ab, ao);

			return false;
		}

		//perform a similar test for the edge ac

		Vector3 acp = Vector3::Cross(abc, ac);

		if (Vector3::Dot(acp, ao) > 0)
		{
			b = a;
			v = cross_aba(ac, ao);

			return false;
		}

		//if we get here, then the origin must be
		//within the triangle, but we care whether
		//it is above or below it, so test

		if (Vector3::Dot(abc, ao) > 0)
		{
			d = c;
			c = b;
			b = a;

			v = abc;
		}
		else
		{
			d = b;
			b = a;

			v = -abc;
		}

		n = 3;

		//again, need a tetrahedron to enclose the origin
		return false;
	}

	if (n == 3)
	{
		Vector3 ao = -a;

		Vector3 ab = b - a;
		Vector3 ac = c - a;
		Vector3 ad = d - a;

		Vector3 abc = Vector3::Cross(ab, ac);
		Vector3 acd = Vector3::Cross(ac, ad);
		Vector3 adb = Vector3::Cross(ad, ab);

		Vector3 tmp;

		const int over_abc = 0x1;
		const int over_acd = 0x2;
		const int over_adb = 0x4;

		int plane_tests =
			(Vector3::Dot(abc, ao) > 0 ? over_abc : 0) |
			(Vector3::Dot(acd, ao) > 0 ? over_acd : 0) |
			(Vector3::Dot(adb, ao) > 0 ? over_adb : 0);

		switch (plane_tests)
		{
		case 0:
			//behind all three faces, thus inside the tetrahedron - we're done
			return true;

		case over_abc:
			goto check_one_face;

		case over_acd:
			//rotate ACD into ABC

			b = c;
			c = d;

			ab = ac;
			ac = ad;

			abc = acd;

			goto check_one_face;

		case over_adb:
			//rotate ADB into ABC

			c = b;
			b = d;

			ac = ab;
			ab = ad;

			abc = adb;

			goto check_one_face;

		case over_abc | over_acd:
			goto check_two_faces;

		case over_acd | over_adb:
			//rotate ACD, ADB into ABC, ACD

			tmp = b;
			b = c;
			c = d;
			d = tmp;

			tmp = ab;
			ab = ac;
			ac = ad;
			ad = tmp;

			abc = acd;
			acd = adb;

			goto check_two_faces;

		case over_adb | over_abc:
			//rotate ADB, ABC into ABC, ACD

			tmp = c;
			c = b;
			b = d;
			d = tmp;

			tmp = ac;
			ac = ab;
			ab = ad;
			ad = tmp;

			acd = abc;
			abc = adb;

			goto check_two_faces;

		default:
			//we've managed to build a horribly degenerate simplex
			//this could just as easily be an assert, but since this
			//code was originally used to reject definite non-intersections
			//as an optimization it conservatively returns true
			return true;
		}

	check_one_face:

		if (Vector3::Dot(Vector3::Cross(abc, ac), ao) > 0)
		{
			//in the region of AC

			b = a;

			v = cross_aba(ac, ao);

			n = 2;

			return false;
		}

	check_one_face_part_2:

		if (Vector3::Dot(Vector3::Cross(ab, abc), ao) > 0)
		{
			//in the region of edge AB

			c = b;
			b = a;

			v = cross_aba(ab, ao);

			n = 2;

			return false;
		}

		//in the region of ABC

		d = c;
		c = b;
		b = a;

		v = abc;

		n = 3;

		return false;

	check_two_faces:

		if (Vector3::Dot(Vector3::Cross(abc, ac), ao) > 0)
		{
			//the origin is beyond AC from ABC's
			//perspective, effectively excluding
			//ACD from consideration

			//we thus need test only ACD

			b = c;
			c = d;

			ab = ac;
			ac = ad;

			abc = acd;

			goto check_one_face;
		}

		//at this point we know we're either over
		//ABC or over AB - all that's left is the
		//second half of the one-fase test

		goto check_one_face_part_2;
	}
}


#define B3_LINEAR_SLOP 0.005

bool BoxCollider::IsColliding(BoxCollider* other, Vector3& mtv)
{
	/*other->parent->transform.rotation = parent->transform.rotation;

	v = Vector3(1, 0, 0); //some arbitrary starting vector
	n = 0;

	c = SupportFunction(v, parent->GetWorldTransform());
	if (Vector3::Dot(c, v) < 0)
		return false;

	v = -c;
	b = SupportFunction(v, parent->GetWorldTransform());

	if (Vector3::Dot(b, v) < 0)
		return false;

	v = cross_aba(c - b, -b);
	n = 2;

	for (int iterations = 0; iterations < 32; iterations++)
	{
		Vector3 a = SupportFunction(v, parent->GetWorldTransform());

		if (Vector3::Dot(a, v) < 0)
			return false;

		if (update(a))
			return true;
	}

	//out of iterations, be conservative
	return true;*/


	/*Vector3 a = parent->GetWorldPosition() + transform.position;
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

	float smallest = 1e100;
	int smallestIndex = 0;

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

		float sum = P1a + P1b + P1c + P2a + P2b + P2c;

		if (R > sum)
		{
			return 0;
		}
		else
		{
			if (sum < smallest)
			{
				smallest = sum;
				smallestIndex = i;
			}
		}
	}

	mtv = axes[smallestIndex] * smallest;
	return 1;*/

	return 0;
}