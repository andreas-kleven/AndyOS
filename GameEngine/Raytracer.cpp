#include "Raytracer.h"
#include "Vector3.h"
#include "limits.h"
#include "debug.h"

namespace gl
{
	Game* game;
	GC gc;

	Raytracer::Raytracer(Game* _game, GC _gc)
	{
		game = _game;
		gc = _gc;
	}

	Vector3 GetPos(Vertex& vert)
	{
		return Vector3(vert.tmpPos.x, vert.tmpPos.y, vert.tmpPos.z);
	}

	//https://en.wikipedia.org/wiki/M%C3%B6ller%E2%80%93Trumbore_intersection_algorithm
	bool RayTriangleIntersection(
		Vector3 &rayOrigin, Vector3 &rayDir,
		Vertex &vert0, Vertex &vert1, Vertex &vert2,
		float &t,
		float &tex_u, float &tex_v)
	{
		const float EPSILON = 0.0001;
		Vector3 v0 = GetPos(vert0);
		Vector3 v1 = GetPos(vert1);
		Vector3 v2 = GetPos(vert2);

		Vector3 edge1, edge2, h, s, q;
		float a, f, u, v;

		edge1 = v1 - v0;
		edge2 = v2 - v0;

		//Backface culling
		//Vector3 N = edge1.Cross(edge2);
		//if (rayDir.Dot(N.Normalized()) > 0)
		//	return false;

		h = rayDir.Cross(edge2);
		a = edge1.Dot(h);

		if (a > -EPSILON && a < EPSILON)
			return false;

		f = 1 / a;
		s = rayOrigin - v0;
		u = f * (s.Dot(h));
		if (u < 0.0 || u > 1.0)
			return false;

		q = s.Cross(edge1);
		v = f * rayDir.Dot(q);
		if (v < 0.0 || u + v > 1.0)
			return false;
		// At this stage we can compute t to find out where the intersection point is on the line.
		t = f * edge2.Dot(q);

		if (t > EPSILON) // ray intersection
		{
			float w = 1 - u - v;

			tex_u = (vert0.tex_u * w) + (vert1.tex_u * u) + (vert2.tex_u * v);
			tex_v = (vert0.tex_v * w) + (vert1.tex_v * u) + (vert2.tex_v * v);

			return true;
		}
		else // This means that there is a line intersection but not a ray intersection.
			return false;
	}

	bool Intersect(Vector3& rayOrigin, Vector3& rayDir, float& minDist, float& tex_u, float& tex_v, Vector3& hit, MeshComponent*& hitMesh)
	{
		minDist = FLT_MAX;

		for (int i = 0; i < game->objects.Count(); i++)
		{
			GameObject* obj = game->objects[i];

			for (int j = 0; j < obj->meshComponents.Count(); j++)
			{
				MeshComponent* mesh = obj->meshComponents[j];

				for (int k = 0; k < mesh->vertex_count; k += 3)
				{
					Vertex& v0 = mesh->vertices[k + 0];
					Vertex& v1 = mesh->vertices[k + 1];
					Vertex& v2 = mesh->vertices[k + 2];

					float dist, u, v;

					if (RayTriangleIntersection(rayOrigin, rayDir, v0, v1, v2, dist, u, v))
					{
						if (dist < minDist) {
							minDist = dist;
							tex_u = u;
							tex_v = v;
							hitMesh = mesh;
						}
					}
				}
			}
		}

		if (minDist < FLT_MAX)
		{
			hit = rayOrigin + rayDir * minDist;
			return true;		}

		return false;
	}

	bool Intersect(Vector3& rayOrigin, Vector3& rayDir)
	{
		float dist, u, v;
		Vector3 hit;
		MeshComponent* mesh;

		return Intersect(rayOrigin, rayDir, dist, u, v, hit, mesh);
	}

	void CalculateVertices()
	{
		for (int i = 0; i < game->objects.Count(); i++)
		{
			GameObject* obj = game->objects[i];
			Transform* trans = &obj->GetWorldTransform();

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
	}

	void Raytracer::Render()
	{
		Camera* cam = game->GetActiveCamera();
		LightSource* lightSource = game->lights[0];

		const float fov = 53;
		const float imageAspectRatio = gc.width / (float)gc.height;
		const float scale = tan(fov * M_PI / 180.0f / 2.0f);

		CalculateVertices();

		for (int y = 0; y < gc.height; y++)
		{
			for (int x = 0; x < gc.width; x++)
			{
				float Px = (2 * ((x + 0.5) / gc.width) - 1) * scale * imageAspectRatio;
				float Py = (1 - 2 * ((y + 0.5) / gc.height)) * scale;

				Vector3 rayOrigin = cam->GetWorldPosition();
				Vector3 rayDir = cam->GetWorldRotation() * Vector3(Px, Py, 1).Normalized();

				float dist, u, v;
				Vector3 hit;
				MeshComponent* hitMesh;

				if (Intersect(rayOrigin, rayDir, dist, u, v, hit, hitMesh))
				{
					Vector3 lightVector = Vector3(-0.4, -1, 0.1);
					Vector3 shadowRay = -lightVector.Normalized();
					bool isShadow = Intersect(hit, shadowRay);

					BMP* texture = GL::m_textures[hitMesh->texId];

					int X = texture->width * u;
					int Y = texture->height * v;

					float lum = 1;

					uint32 color = texture->pixels[(int)(Y * texture->width + X)];
					float r = lum * (uint8)(color >> 16);
					float g = lum * (uint8)(color >> 8);
					float b = lum * (uint8)(color >> 0);

					if (isShadow)
					{
						r *= 0;
						g *= 0;
						b *= 0;
					}

					int col = (0xFF << 24) | ((int)(r) << 16) | ((int)(g) << 8) | (int)(b);
					Drawing::SetPixel(x, y, col, gc);
				}
				else
				{
					Drawing::SetPixel(x, y, 0xFF7F7F7F, gc);
				}
			}
		}
	}
}
