#include "System.h"
#include "Raytracer.h"
#include "Vector3.h"
#include "limits.h"
#include "debug.h"
#include "../GL/GL.h"

Game* game;
GC gc;

const int numPhotons = 10000;
Photon photons[numPhotons];
int currentNumPhotons;

Transform prevCamTransform;
int mouseX;
int mouseY;
int resolution;

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

Vector3 GetNormal(Vertex& v0, Vertex v1, Vertex v2, float u, float v)
{
	return ((v0.worldNormal * (1 - u - v)) + (v1.worldNormal * u) + (v2.worldNormal * v)).ToVector3();;
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

	ColRGB color;
	color.r = ((v0.color.r * w) + (v1.color.r * u) + (v2.color.r * v));
	color.g = ((v0.color.g * w) + (v1.color.g * u) + (v2.color.g * v));
	color.b = ((v0.color.b * w) + (v1.color.b * u) + (v2.color.b * v));

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

	return color;
}

bool Trace(
	Vector3& rayOrigin, Vector3& rayDir,
	Vector3& hit,
	MeshComponent*& hitMesh,
	Vertex*& triangle,
	float& u, float& v,
	bool backfaces,
	int maxRays)
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

				float dist, _u, _v;

				if (RayTriangleIntersection(rayOrigin, rayDir, p0, p1, p2, dist, _u, _v, backfaces))
				{
					if (dist < minDist)
					{
						minDist = dist;
						hit = rayOrigin + rayDir * dist;
						hitMesh = mesh;
						triangle = mesh->vertices + k;
						u = _u;
						v = _v;
					}
				}
			}
		}
	}

	return minDist < FLT_MAX;
}

bool TracePhoton(
	Vector3& rayOrigin, Vector3& rayDir,
	Photon& photon,
	bool backfaces = false,
	int maxRays = 5)
{
	Vector3 hit;
	MeshComponent* mesh;
	Vertex* triangle;
	float u, v;

	if (maxRays < 1)
		return false;

	if (!Trace(rayOrigin, rayDir, hit, mesh, triangle, u, v, backfaces, maxRays))
		return false;

	Vertex& v0 = triangle[0];
	Vertex& v1 = triangle[1];
	Vertex& v2 = triangle[2];

	Shader& shader = mesh->shader;
	Vector3 N = GetNormal(v0, v1, v2, u, v);

	float rnd = frand();

	if (shader.ior > 0)
	{
		if (rnd > 0.5 && Fresnel(rayDir, N, shader.ior) < 1)
		{
			Vector3 R = Refract(rayDir, N, shader.ior);
			return TracePhoton(hit, R, photon, backfaces, maxRays - 1);
		}
		else
		{
			Vector3 R = Reflect(rayDir, N);
			return TracePhoton(hit, R, photon, backfaces, maxRays - 1);
		}
	}

	ColRGB color = GetColor(rayDir, mesh, v0, v1, v2, hit, u, v);

	photon.surfaceNormal = N;

	photon.color = ColRGB(
		color.r,
		color.g,
		color.b);

	float Pa = 0.8;
	float Pr = 0.2;
	float Pt = 0.3;

	if (rnd < Pa || maxRays <= 1)
	{
		//Absorb
		photon.position = hit;
		photon.direction = rayDir;
	}
	else
	{
		//Reflect
		Vector3 R;

		if (shader.ior > 0)
		{
			//Mirror
			R = Reflect(rayDir, N);
		}
		else
		{
			float ru = frand();
			float rv = frand();
			float ru2 = ru * ru;

			float theta = M_PI * ru;
			float phi = acos(2 * rv - 1);

			R.x = sqrt(1 - ru2) * cos(theta);
			R.y = sqrt(1 - ru2) * sin(theta);
			R.z = ru;
		}

		return TracePhoton(hit, R, photon, backfaces, maxRays - 1);
	}
}

ColRGB TraceColor(Vector3& rayOrigin, Vector3& rayDir, int maxRays = 5)
{
	if (maxRays < 1)
		return ColRGB(0.5, 0.5, 0.5);

	Vector3 hit;
	MeshComponent* mesh;
	Vertex* triangle;
	float u, v;

	if (!Trace(rayOrigin, rayDir, hit, mesh, triangle, u, v, false, maxRays))
		return ColRGB(0.5, 0.5, 0.5);

	Vector3 N = GetNormal(triangle[0], triangle[1], triangle[2], u, v);
	Shader& shader = mesh->shader;

	if (shader.ior > 0)
	{
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
			refraction = TraceColor(refractionOrigin, refractionRay, maxRays - 1);
		}

		Vector3 reflectionOrigin = hit - bias;
		Vector3 reflectionRay = Reflect(rayDir, N);
		reflection = TraceColor(reflectionOrigin, reflectionRay, maxRays - 1);

		return reflection * kr + refraction * (1 - kr);
	}

	ColRGB color;

	for (int i = 0; i < currentNumPhotons; i++)
	{
		Photon& p = photons[i];
		float sqDist = (p.position - hit).MagnitudeSquared();

		if (sqDist < 4)
		{
			float weight = max(0.0f, N.Dot(-p.direction));
			weight *= (2 - sqrt(sqDist)) / 400;

			color.r += p.color.r * weight;
			color.g += p.color.g * weight;
			color.b += p.color.b * weight;
		}
	}

	return color * resolution;
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

	//Resolution
	const int maxResolution = 16;
	Transform& camTransform = cam->GetWorldTransform();

	if (camTransform.position == prevCamTransform.position
		&& camTransform.rotation == prevCamTransform.rotation
		&& (int)Mouse::x == mouseX && (int)Mouse::y == mouseY)
	{
		if (resolution > 1)
			resolution /= 2;
	}
	else
	{
		resolution = maxResolution;
	}

	currentNumPhotons = numPhotons / resolution;

	prevCamTransform = camTransform;
	mouseX = Mouse::x;
	mouseY = Mouse::y;

	//Perspective
	const float fov = 53;
	const float imageAspectRatio = gc.width / (float)gc.height;
	const float scale = tan(fov * M_PI / 180.0f / 2.0f);

	//Calculate global vertex positions
	CalculateVertices();


	//Photon mapping
	srand(0);

	LightSource* light = game->lights[0];
	Vector3 rayOrigin = light->GetWorldPosition();

	Matrix4 V = Matrix4::CreateView(
		-cam->transform.GetForwardVector().ToVector4(),
		cam->transform.GetUpVector().ToVector4(),
		cam->transform.GetRightVector().ToVector4(),
		cam->transform.position.ToVector4());
	Matrix4 M = Matrix4::CreatePerspectiveProjection(gc.width, gc.height, 90, 0.1, 100) * V;

	//Drawing::Clear(0xFF000000, gc);

	int i = 0;
	while (i < currentNumPhotons)
	{
		float sin_theta = sqrt(frand());
		float cos_theta = sqrt(1 - sin_theta * sin_theta);

		float psi = frand() * 2 * M_PI;

		Vector3 rayDir;
		rayDir.x = sin_theta * cos(psi);
		rayDir.y = sin_theta * sin(psi);
		rayDir.z = cos_theta;

		Photon photon;
		MeshComponent* mesh;
		photon.color = light->GetColor();

		if (TracePhoton(rayOrigin, rayDir, photon, mesh))
		{
			photons[i++] = photon;
		}
	}

	for (int y = 0; y < gc.height; y += resolution)
	{
		for (int x = 0; x < gc.width; x += resolution)
		{
			//Stop on input
			if (resolution < maxResolution)
			{
				if (Keyboard::GetLastKey().key != KEY_INVALID || (int)Mouse::x != mouseX || (int)Mouse::y != mouseY)
				{
					Keyboard::DiscardLastKey();
					resolution = maxResolution;
					return;
				}
			}

			float Px = (2 * ((x + 0.5) / gc.width) - 1) * scale * imageAspectRatio;
			float Py = (1 - 2 * ((y + 0.5) / gc.height)) * scale;

			Vector3 rayOrigin = cam->GetWorldPosition();
			Vector3 rayDir = cam->GetWorldRotation() * Vector3(Px, Py, 1).Normalized();

			ColRGB color = TraceColor(rayOrigin, rayDir);
			Drawing::FillRect(x, y, resolution, resolution, color.ToInt(), gc);
		}
	}

	return;

	for (int i = 0; i < currentNumPhotons; i++)
	{
		Photon& photon = photons[i];

		Vector4 pos = M * photon.position.ToVector4();
		int x = pos.x * GL::m_width / pos.w + GL::m_width * 0.5;
		int y = pos.y * GL::m_height / pos.w + GL::m_height * 0.5;

		Drawing::SetPixel(x, y, photon.color.ToInt(), gc);
	}
}
