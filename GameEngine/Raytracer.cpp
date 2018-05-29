#include "Raytracer.h"
#include "Engine.h"
#include "System.h"
#include "Vector3.h"
#include "limits.h"
#include "debug.h"
#include "../GL/GL.h"

Game* game;
GC gc;

const int numPhotons = 10000;
Photon photonMap[numPhotons];
Photon causticsMap[numPhotons];
int currentNumPhotons;
int currentNumCausticsPhotons;

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

Vector3 GetPos(Vertex* vert)
{
	return vert->tmpPos.ToVector3();
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

bool TraceNode(
	KDNode* node,
	MeshComponent* mesh,
	Vector3& rayOrigin, Vector3& rayDir,
	float& distance,
	Vector3& hit,
	Triangle*& triangle,
	float& u, float& v,
	bool backfaces)
{
	if (!node)
		return false;

	Transform& trans = mesh->parent->GetWorldTransform();

	Matrix4 invT = Matrix4::CreateTranslation(-trans.position.ToVector4());
	Matrix4 invR = (-trans.rotation).ToMatrix();
	Matrix4 invS = Matrix4::CreateScale(Vector4(1 / trans.scale.x, 1 / trans.scale.y, 1 / trans.scale.z, 0));

	//Matrix4 inverse = (T * R * S).Inverse();
	Matrix4 invM = invS * invR * invT;
	Vector4 _rayOrigin = invM * rayOrigin.ToVector4();
	Vector4 _rayDir = (invM * rayDir.ToVector4(0)).Normalized();

	float t;
	if (!node->bounds.RayIntersection(_rayOrigin.ToVector3(), _rayDir.ToVector3(), t))
		return false;

	if (node->left || node->right)
	{
		return TraceNode(node->left, mesh, rayOrigin, rayDir, distance, hit, triangle, u, v, backfaces)
			|| TraceNode(node->right, mesh, rayOrigin, rayDir, distance, hit, triangle, u, v, backfaces);
	}
	else
	{
		bool hasHit = false;

		for (int i = 0; i < node->triangle_count; i++)
		{
			Triangle* tri = node->triangles[i];
			Vector3 p0 = GetPos(tri->v0);
			Vector3 p1 = GetPos(tri->v1);
			Vector3 p2 = GetPos(tri->v2);

			float dist, _u, _v;

			if (RayTriangleIntersection(rayOrigin, rayDir, p0, p1, p2, dist, _u, _v, backfaces))
			{
				if (dist < distance)
				{
					distance = dist;
					hit = rayOrigin + rayDir * dist;
					triangle = tri;
					u = _u;
					v = _v;

					hasHit = true;
				}
			}
		}

		return hasHit;
	}
}

bool Trace(
	Vector3& rayOrigin, Vector3& rayDir,
	Vector3& hit,
	MeshComponent*& hitMesh,
	Triangle*& triangle,
	float& u, float& v,
	bool backfaces,
	int maxRays)
{
	hitMesh = 0;

	float distance = FLT_MAX;

	for (int i = 0; i < game->objects.Count(); i++)
	{
		GameObject* obj = game->objects[i];
		Transform& transform = obj->GetWorldTransform();

		for (int j = 0; j < obj->meshComponents.Count(); j++)
		{
			MeshComponent* mesh = obj->meshComponents[j];
			Model3D* model = mesh->model;

			if (!model)
				continue;

			if (TraceNode(mesh->bvh->root, mesh, rayOrigin, rayDir, distance, hit, triangle, u, v, backfaces))
			{
				hitMesh = mesh;
			}
		}
	}

	return distance < FLT_MAX;
}

bool TracePhoton(
	Vector3& rayOrigin, Vector3& rayDir,
	Photon& photon,
	bool& caustics,
	bool backfaces = false,
	int maxRays = 5)
{
	Vector3 hit;
	MeshComponent* mesh;
	Triangle* triangle;
	float u, v;

	if (maxRays < 1)
		return false;

	if (!Trace(rayOrigin, rayDir, hit, mesh, triangle, u, v, backfaces, maxRays))
		return false;

	Shader& shader = mesh->shader;
	Vector3 N = triangle->WorldNormal(u, v);

	float rnd = frand();

	if (shader.ior > 0)
	{
		caustics = true;

		if (rnd > 0.5 && Fresnel(rayDir, N, shader.ior) < 1 && shader.ior < FLT_MAX)
		{
			Vector3 R = Refract(rayDir, N, shader.ior);
			return TracePhoton(hit, R, photon, caustics, true, maxRays - 1);
		}
		else
		{
			Vector3 R = Reflect(rayDir, N);
			return TracePhoton(hit, R, photon, caustics, backfaces, maxRays - 1);
		}
	}

	photon.surfaceNormal = N;
	photon.color = triangle->Color(u, v);

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
		float ru = frand();
		float rv = frand();
		float ru2 = ru * ru;

		float theta = M_PI * ru;
		float phi = acos(2 * rv - 1);

		Vector3 R;
		R.x = sqrt(1 - ru2) * cos(theta);
		R.y = sqrt(1 - ru2) * sin(theta);
		R.z = ru;

		return TracePhoton(hit, R, photon, caustics, backfaces, maxRays - 1);
	}
}

ColRGB TraceColor(Vector3& rayOrigin, Vector3& rayDir, int maxRays = 5)
{
	if (maxRays < 1)
		return ColRGB(0.5, 0.5, 0.5);

	Vector3 hit;
	MeshComponent* mesh;
	Triangle* triangle;
	float u, v;

	if (!Trace(rayOrigin, rayDir, hit, mesh, triangle, u, v, false, maxRays))
		return ColRGB(0.5, 0.5, 0.5);

	Vector3 N = triangle->WorldNormal(u, v);
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
		Photon& p = photonMap[i];
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

	for (int i = 0; i < currentNumCausticsPhotons; i++)
	{
		Photon& p = causticsMap[i];
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

	return color * resolution + ColRGB(0.05, 0.05, 0.05);
}

void EmitPhotons()
{
	LightSource* lightSource = game->lights[0];
	Vector3 rayOrigin = lightSource->GetWorldPosition();

	srand(0);

	int i = 0;
	int j = 0;

	while (i < currentNumPhotons || j < currentNumCausticsPhotons)
	{
		float sin_theta = sqrt(frand());
		float cos_theta = sqrt(1 - sin_theta * sin_theta);

		float psi = frand() * 2 * M_PI;

		Vector3 rayDir;
		rayDir.x = sin_theta * cos(psi);
		rayDir.y = sin_theta * sin(psi);
		rayDir.z = cos_theta;

		Photon photon;
		photon.color = lightSource->GetColor();
		bool caustics = 0;

		if (TracePhoton(rayOrigin, rayDir, photon, caustics))
		{
			//if (caustics)
			//{
			//	if (j < currentNumPhotons)
			//	{
			//		causticsMap[j++] = photon;
			//	}
			//}
			//else
			{
				if (i < currentNumPhotons)
				{
					photonMap[i++] = photon;
				}
				else if (j == 0)
				{
					currentNumCausticsPhotons = 0;
				}
			}
		}
	}
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

			if (mesh->model)
			{
				for (int k = 0; k < mesh->model->vertices.Count(); k++)
				{
					Vertex* vert = &mesh->model->vertex_buffer[k];

					vert->worldNormal = R * vert->normal;
					vert->MulMatrix(M);
				}
			}
		}
	}
}

void Raytracer::Render()
{
	Camera* cam = game->GetActiveCamera();

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
	currentNumCausticsPhotons = 0;

	prevCamTransform = camTransform;
	mouseX = Mouse::x;
	mouseY = Mouse::y;

	//Perspective
	const float fov = 53;
	const float imageAspectRatio = gc.width / (float)gc.height;
	const float scale = tan(fov * M_PI / 180.0f / 2.0f);

	//Calculate global vertex positions
	CalculateVertices();

	int emitTime = PIT::ticks;
	EmitPhotons();

	int renderTime = PIT::ticks;

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

	int finishTime = PIT::ticks;

	Debug::y = gc.height / 16 - 4;
	Debug::Print("Total: %i\nEmit: %i\nRender: %i\nResolution: %i\n", finishTime - emitTime, renderTime - emitTime, finishTime - renderTime, resolution);
	return;

	for (int i = 0; i < currentNumPhotons; i++)
	{
		Photon& photon = photonMap[i];
		Vector3 p = GEngine::WorldToScreen(game, photon.position);
		Drawing::SetPixel(p.x, p.y, photon.color.ToInt(), gc);
	}
}
