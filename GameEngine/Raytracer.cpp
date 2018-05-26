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

	void swap(float& a, float& b)
	{
		float tmp = a;
		a = b;
		b = tmp;
	}

	Vector3 GetPos(Vertex& vert)
	{
		return Vector3(vert.tmpPos.x, vert.tmpPos.y, vert.tmpPos.z);
	}

	Vector3 Reflect(Vector3 I, Vector3 N)
	{
		return I - N * I.Dot(N) * 2;
	}

	Vector3 Refract(Vector3& I, Vector3& N, float &ior)
	{
		float cosi = clamp(-1.0f, 1.0f, I.Dot(N));
		float etai = 1, etat = ior;
		Vector3 n = N;

		if (cosi < 0)
		{
			cosi = -cosi;
		}
		else
		{
			swap(etai, etat);
			n = -N;
		}

		float eta = etai / etat;
		float k = 1 - eta * eta * (1 - cosi * cosi);

		if (k < 0)
			return Vector3();
		else
			return I * eta + n * (eta * cosi - sqrt(k));
	}

	float Fresnel(Vector3 &I, Vector3 &N, const float& ior)
	{
		float etai = 1;
		float etat = ior;

		float cosi = clamp(-1.0f, 1.0f, I.Dot(N));

		if (cosi > 0)
			swap(etai, etat);

		//Compute sini using Snell's law
		float sint = etai / etat * sqrt(max(0.0f, 1 - cosi * cosi));

		if (sint >= 1)
		{
			//Total internal reflection
			return 1;
		}
		else
		{
			float cost = sqrt(max(0.f, 1 - sint * sint));
			cosi = abs(cosi);
			float Rs = ((etat * cosi) - (etai * cost)) / ((etat * cosi) + (etai * cost));
			float Rp = ((etai * cosi) - (etat * cost)) / ((etai * cosi) + (etat * cost));
			return (Rs * Rs + Rp * Rp) / 2;
		}
	}

	//https://en.wikipedia.org/wiki/M%C3%B6ller%E2%80%93Trumbore_intersection_algorithm
	bool RayTriangleIntersection(
		Vector3 &rayOrigin, Vector3 &rayDir,
		Vector3 &v0, Vector3 &v1, Vector3 &v2,
		float &t,
		float &u, float &v,
		bool backfaces)
	{
		const float EPSILON = 0.0001;

		Vector3 edge1, edge2, h, s, q;
		float a, f;

		edge1 = v1 - v0;
		edge2 = v2 - v0;

		//Backface culling
		Vector3 N = edge1.Cross(edge2);
		if (!backfaces && rayDir.Dot(N.Normalized()) > 0)
			return false;

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
			return true;
		}
		else // This means that there is a line intersection but not a ray intersection.
			return false;
	}

	ColRGB GetColor(
		Vector3& rayDir,
		MeshComponent* mesh,
		Vertex& v0, Vertex& v1, Vertex& v2,
		Vector3& hit,
		float u, float v)
	{
		float w = 1 - u - v;

		LightSource* lightSource = game->lights[0];
		Vector3 lightDir = lightSource->GetDirectionVector(hit);

		Shader& shader = mesh->shader;

		Vector3 N = ((v0.worldNormal * w) + (v1.worldNormal * u) + (v2.worldNormal * v)).ToVector3();
		Vector3 R = Reflect(lightDir, N);

		float diffuse = shader.diffuse * clamp(-N.Dot(lightDir), 0.0f, 1.0f);
		float specular = shader.specular * pow(max(-R.Dot(rayDir), 0.0f), 8) * 0.08;
		float totalLight = (clamp(diffuse + specular, 0.0f, 1.0f));

		ColRGB color;
		color.r = ((v0.color.r * w) + (v1.color.r * u) + (v2.color.r * v)) * totalLight;
		color.g = ((v0.color.g * w) + (v1.color.g * u) + (v2.color.g * v)) * totalLight;
		color.b = ((v0.color.b * w) + (v1.color.b * u) + (v2.color.b * v)) * totalLight;

		BMP* texture = GL::m_textures[mesh->texId];
		if (texture)
		{
			float tex_u = (v0.tex_u * w) + (v1.tex_u * u) + (v2.tex_u * v);
			float tex_v = (v0.tex_v * w) + (v1.tex_v * u) + (v2.tex_v * v);

			int X = texture->width * tex_u;
			int Y = texture->height * tex_v;

			float lum = color.Luminosity();

			uint32 pixel = texture->pixels[(int)(Y * texture->width + X)];
			color = ColRGB(pixel) * lum;
		}

		return color * lightSource->GetIntensityAtPoint(hit);
	}

	bool Intersect(
		Vector3& rayOrigin, Vector3& rayDir,
		ColRGB& color,
		bool backfaces = false,
		int maxRays = 5)
	{
		float minDist = FLT_MAX;

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

					Vector3 p0 = GetPos(v0);
					Vector3 p1 = GetPos(v1);
					Vector3 p2 = GetPos(v2);

					float dist, u, v;

					if (RayTriangleIntersection(rayOrigin, rayDir, p0, p1, p2, dist, u, v, backfaces))
					{
						Vector3 hit = rayOrigin + rayDir * dist;
						Shader& shader = mesh->shader;

						if (dist < minDist)
						{
							minDist = dist;

							//Shadow
							LightSource* lightSource = game->lights[0];
							Vector3 lightDir = lightSource->GetDirectionVector(hit);

							ColRGB _color;
							//if (Intersect(hit, -lightDir, _color, true, maxRays - 1))
							//{
							//	color = ColRGB(0, 0, 0);
							//}
							//else
							{
								color = GetColor(rayDir, mesh, v0, v1, v2, hit, u, v);
							}

							if (maxRays > 1 && shader.ior != 0)
							{
								Vector3 N = ((v0.worldNormal * (1 - u - v)) + (v1.worldNormal * u) + (v2.worldNormal * v)).ToVector3();

								bool outside = rayDir.Dot(N) < 0;
								Vector3 bias = N * 0.01f;

								if (outside)
									bias = -bias;

								ColRGB refraction;
								ColRGB reflection;

								float kr = Fresnel(rayDir, N, shader.ior);

								if (kr < 1)
								{
									Vector3 refractionOrigin = hit + bias;
									Vector3 refractionRay = Refract(rayDir, N, shader.ior);

									if (!Intersect(refractionOrigin, refractionRay, refraction, true, maxRays - 1))
										refraction = ColRGB(0.5f, 0.5f, 0.5f);
								}

								Vector3 reflectionOrigin = hit - bias;
								Vector3 reflectionRay = Reflect(rayDir, N);

								if (!Intersect(reflectionOrigin, reflectionRay, reflection, true, maxRays - 1))
									reflection = ColRGB(0.5f, 0.5f, 0.5f);

								color = color + reflection * kr + refraction * (1 - kr);
							}
						}
					}
				}
			}
		}

		return minDist < FLT_MAX;
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

				for (int k = 0; k < mesh->vertex_count; k++)
				{
					Vertex& vert = mesh->vertices[k];

					vert.worldNormal = R * vert.normal;
					vert.MulMatrix(M);
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

				Vector3 hit;
				MeshComponent* hitMesh;
				Vertex* triangle;

				ColRGB color;

				if (Intersect(rayOrigin, rayDir, color))
				{
					Drawing::SetPixel(x, y, color.ToInt(), gc);
				}
				else
				{
					Drawing::SetPixel(x, y, 0xFF7F7F7F, gc);
				}
			}
		}
	}
}
