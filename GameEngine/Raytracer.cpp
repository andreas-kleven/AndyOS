#include "Raytracer.h"
#include "Vector3.h"
#include "limits.h"
#include "debug.h"

namespace gl
{
	GC gc;

	Raytracer::Raytracer(GC _gc)
	{
		gc = _gc;
	}

	bool rayTriangleIntersect(
		Vector3 &orig, Vector3 &dir,
		Vector3 &v0, Vector3 &v1, Vector3 &v2,
		float &t)
	{
		// compute plane's normal
		Vector3 v0v1 = v1 - v0;
		Vector3 v0v2 = v2 - v0;
		// no need to normalize
		Vector3 N = v0v1.Cross(v0v2); // N 
		float area2 = N.Magnitude();

		// Step 1: finding P

		// check if ray and plane are parallel ?
		float NdotRayDirection = N.Dot(dir);
		if (abs(NdotRayDirection) < FLT_EPSILON) // almost 0 
			return false; // they are parallel so they don't intersect ! 

						  // compute d parameter using equation 2
		float d = N.Dot(v0);

		// compute t (equation 3)
		t = -(N.Dot(orig) + d) / NdotRayDirection;
		//Debug::Print("%f\n", t);
		// check if the triangle is in behind the ray
		if (t < 0) return false; // the triangle is behind 

								 // compute the intersection point using equation 1
		Vector3 P = orig + dir * t;

		// Step 2: inside-outside test
		Vector3 C; // vector perpendicular to triangle's plane 

				 // edge 0
		Vector3 edge0 = v1 - v0;
		Vector3 vp0 = P - v0;
		C = edge0.Cross(vp0);
		if (N.Dot(C) < 0) return false; // P is on the right side 

											   // edge 1
		Vector3 edge1 = v2 - v1;
		Vector3 vp1 = P - v1;
		C = edge1.Cross(vp1);
		if (N.Dot(C) < 0)  return false; // P is on the right side 

												// edge 2
		Vector3 edge2 = v0 - v2;
		Vector3 vp2 = P - v2;
		C = edge2.Cross(vp2);
		if (N.Dot(C) < 0) return false; // P is on the right side; 

		return true; // this ray hits the triangle 
	}

	Vector3 GetPos(Vertex& vert)
	{
		return Vector3(vert.tmpPos.x, vert.tmpPos.y, vert.tmpPos.z);
	}

	bool Intersect(MeshComponent* mesh, Vector3& orig, Vector3& dir, float& distance, Vector3& hit)
	{
		float minDist = FLT_MAX;

		for (int k = 0; k < mesh->vertex_count; k += 3)
		{
			Vertex& v0 = mesh->vertices[k + 0];
			Vertex& v1 = mesh->vertices[k + 1];
			Vertex& v2 = mesh->vertices[k + 2];

			Vector3 p0 = GetPos(v0);
			Vector3 p1 = GetPos(v1);
			Vector3 p2 = GetPos(v2);

			//p0 = Vector3(-1000, -1000, 0);
			//p1 = Vector3(1000, -1000, 0);
			//p2 = Vector3(0, 1000, 0);

			//Debug::Print("%f\t%f\t%f\n", p0.x + 0.01, p0.y + 0.01, p0.z + 0.01);

			if (rayTriangleIntersect(orig, dir, p0, p1, p2, distance))
			{
				if (distance < minDist) {
					minDist = distance;
				}
			}
		}

		if (minDist != FLT_MAX)
		{
			hit = orig + dir * distance;
			return true;
		}

		return false;
	}

	void Raytracer::Render(Game* game)
	{
		Camera* cam = game->GetActiveCamera();
		LightSource* lightSource = game->lights[0];

		const float fov = 60;
		const float imageAspectRatio = gc.width / (float)gc.height;
		const float dy = tan(fov / 2 * M_PI / 180);
		const float dx = dy * imageAspectRatio;

		//Transform
		for (int i = 0; i < game->objects.Count(); i++)
		{
			GameObject* obj = game->objects[i];
			Transform* trans = &obj->GetWorldTransform();

			//Vector3 v = cam->GetWorldPosition();
			//Debug::Print("%f\t%f\t%f\n", v.x + 0.001, v.y + 0.001, v.z + 0.001);

			Matrix4 T = Matrix4::CreateTranslation(trans->position.ToVector4());
			Matrix4 R = trans->rotation.ToMatrix();
			Matrix4 S = Matrix4::CreateScale(trans->scale.ToVector4());
			Matrix4 M = T * R * S;

			for (int j = 0; j < obj->meshComponents.Count(); j++)
			{
				MeshComponent* mesh = obj->meshComponents[j];

				for (int v = 0; v < mesh->vertex_count; v++)
					mesh->vertices[v].worldNormal = R * mesh->vertices[v].normal;

				for (int k = 0; k < mesh->vertex_count; k += 3)
				{
					Vertex& v0 = mesh->vertices[k + 0];
					Vertex& v1 = mesh->vertices[k + 1];
					Vertex& v2 = mesh->vertices[k + 2];

					v0.MulMatrix(M);
					v1.MulMatrix(M);
					v2.MulMatrix(M);
				}
			}
		}

		for (int y = 0; y < gc.height; y++)
		{
			for (int x = 0; x < gc.width; x++)
			{
				float Px = (2 * ((x + 0.5) / gc.width) - 1) * dx;
				float Py = (1 - 2 * ((y + 0.5) / gc.height)) * dy;

				Vector3 rayOrigin = cam->GetWorldPosition();
				//Vector3 rayOrigin(0, 0, 0);
				Vector3 ray = cam->GetWorldRotation() * Vector3(Px, Py, 1).Normalized();

				float minDist = FLT_MAX;
				Vector3 hit;

				for (int i = 0; i < game->objects.Count(); i++)
				{
					GameObject* obj = game->objects[i];

					for (int j = 0; j < obj->meshComponents.Count(); j++)
					{
						MeshComponent* mesh = obj->meshComponents[j];

						float distance;
						Vector3 pHit;

						if (Intersect(mesh, rayOrigin, ray, distance, pHit))
						{
							if (distance < minDist)
							{
								minDist = distance;
								hit = pHit;
							}
						}
					}
				}

				bool isShadow = false;

				/*if (minDist < FLT_MAX)
				{
					Vector3 shadowRay = lightSource->GetWorldPosition() - hit;
					//Vector3 shadowRay = lightSource->GetDirectionVector(hit);

					for (int i = 0; i < game->objects.Count(); i++)
					{
						GameObject* obj = game->objects[i];

						for (int j = 0; j < obj->meshComponents.Count(); j++)
						{
							MeshComponent* mesh = obj->meshComponents[j];

							float distance;
							Vector3 pHit;

							if (Intersect(mesh, hit, shadowRay, distance, pHit))
							{
								isShadow = true;
								break;
							}
						}
					}
				}*/

				if (minDist < FLT_MAX && !isShadow)
					Drawing::SetPixel(x, y, COLOR_WHITE, gc);
				else
					Drawing::SetPixel(x, y, 0xFF7F7F7F, gc);
			}
		}
	}
}
