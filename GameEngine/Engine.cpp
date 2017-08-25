#include "Engine.h"
#include "math.h"
#include "../GL/GL.h"
#include "System.h"
#include "MeshComponent.h"
#include "ColliderComponent.h"
#include "SphereCollider.h"
#include "DirectionalLight.h"
#include "stdio.h"

using namespace gl;

int p1 = 0;
float p2 = 0;
float p3 = 0;
float p4 = 1;
float p5 = 60;

Vector last_mouse_pos;
int ticks;

void GEngine::StartGame(Game* game)
{
	GL::Init();
	active_game = game;

	Camera* cam = active_game->GetActiveCamera();
	LightSource* light = (LightSource*)active_game->GetLightSource("Light");

	GameObject* thing = active_game->GetObject("Thing");
	GameObject* thing2 = active_game->GetObject("Thing2");
	GameObject* floor = active_game->GetObject("Floor");

	MeshComponent* mesh = (MeshComponent*)thing->GetComponent("Mesh");
	MeshComponent* mesh2 = (MeshComponent*)thing2->GetComponent("Mesh");
	MeshComponent* mesh3 = (MeshComponent*)floor->GetComponent("Mesh");

	char buf[256];

	last_mouse_pos = Vector(Mouse::x, Mouse::y, 0);
	ticks = PIT::ticks;

	Matrix P = Matrix::CreatePerspectiveProjection(1024, 768, 90, 1, 10);

	GL::MatrixMode(GL_PROJECTION);
	GL::LoadMatrix(P);
	GL::MatrixMode(GL_MODELVIEW);

	while (1)
	{
		Debug::x = 0;
		Debug::y = 0;

		if (PIT::ticks != ticks)
		{
			Debug::Print("FPS: %i\n", 1000 / (PIT::ticks - ticks));
			//Debug::Print("AVG: %i\n", frames * 1000 / PIT::ticks);
		}

		Debug::Print("Ticks: %i\n", ticks);
		Debug::Print("Free %i\n", Memory::num_free);

		Debug::Print("%s\n", cam->GetTransform().ToString(buf));
		Debug::Print("%s\n", thing->transform.ToString(buf));

		Update();
		Collision();
		Render();
	}
}

void GEngine::Update()
{
	delta_time = (PIT::ticks - ticks) / 1000.f;
	ticks = PIT::ticks;

	if (Mouse::x == GL::m_width)
	{
		last_mouse_pos.x = 1;
		Mouse::x = 1;
	}
	else if (Mouse::x == 0)
	{
		last_mouse_pos.x = GL::m_width - 1;
		Mouse::x = GL::m_width - 1;
	}

	if (Mouse::y == GL::m_height)
	{
		last_mouse_pos.y = 1;
		Mouse::y = 1;
	}
	else if (Mouse::y == 0)
	{
		last_mouse_pos.y = GL::m_height - 1;
		Mouse::y = GL::m_height - 1;
	}

	Vector mouse_pos(Mouse::x, Mouse::y, 0);
	Vector mouse_axis = (mouse_pos - last_mouse_pos) * delta_time;
	last_mouse_pos = mouse_pos;


	int sign = Keyboard::shift ? -1 : 1;

	if (Keyboard::GetKeyDown(KEY_D1))
	{
		p1 += sign * delta_time * 2000;
		active_game->objects[0]->transform.rotation.x += 2 * M_PI * delta_time * sign;
	}

	if (Keyboard::GetKeyDown(KEY_D2))
	{
		p2 += sign * delta_time * 400;
		active_game->objects[0]->transform.rotation.y += 2 * M_PI * delta_time * sign;
	}

	if (Keyboard::GetKeyDown(KEY_D3))
	{
		p3 += sign * delta_time / 10;
		active_game->objects[0]->transform.rotation.z += 2 * M_PI * delta_time * sign;
	}

	if (Keyboard::GetKeyDown(KEY_D4))
		p4 += sign * delta_time * 0.1;

	if (Keyboard::GetKeyDown(KEY_D5))
		p5 += sign * delta_time * 10;

	Camera* cam = active_game->GetActiveCamera();

	if (Mouse::mouse_R)
	{
		cam->Rotate(-Vector(mouse_axis.y, mouse_axis.x, 0) * 0.2);
	}
	else if (Mouse::mouse_L)
	{
		//light->Rotate(Vector(mouse_axis.y, mouse_axis.x, 0) * 0.2);
	}

	float speed = 10;

	if (Keyboard::GetKeyDown(KEY_D))
		cam->Translate(cam->GetRightVector() * speed * delta_time);
	if (Keyboard::GetKeyDown(KEY_A))
		cam->Translate(-cam->GetRightVector() * speed * delta_time);
	if (Keyboard::GetKeyDown(KEY_W))
		cam->Translate(cam->GetForwardVector() * speed * delta_time);
	if (Keyboard::GetKeyDown(KEY_S))
		cam->Translate(-cam->GetForwardVector() * speed * delta_time);
	if (Keyboard::GetKeyDown(KEY_E))
		cam->Translate(cam->GetUpVector() * speed * delta_time);
	if (Keyboard::GetKeyDown(KEY_Q))
		cam->Translate(-cam->GetUpVector() * speed * delta_time);

	for (int i = 0; i < active_game->objects.Count(); i++)
	{
		GameObject* obj = active_game->objects[i];
		obj->Update(delta_time);
	}

	for (int i = 0; i < active_game->objects.Count(); i++)
	{
		GameObject* obj = active_game->objects[i];

		for (int j = 0; j < obj->components.Count(); j++)
		{
			Component* comp = obj->components[j];
			comp->Update(delta_time);
		}
	}
}

void GEngine::Collision()
{
	std::List<ColliderComponent*> all;

	for (int i = 0; i < active_game->objects.Count(); i++)
	{
		GameObject* obj = active_game->objects[i];

		for (int j = 0; j < obj->collider_components.Count(); j++)
		{
			ColliderComponent* mesh = obj->collider_components[j];
			all.Add(mesh);
		}
	}

	for (int i = 0; i < all.Count(); i++)
	{
		int start = i + 1;
		for (int j = start; j < all.Count(); j++)
		{
			if (all[i]->IsSphere() && all[j]->IsSphere())
			{
				SphereCollider* a = (SphereCollider*)all[i];
				SphereCollider* b = (SphereCollider*)all[j];

				Vector posa = a->parent->GetWorldPosition() + a->transform.position;
				Vector posb = b->parent->GetWorldPosition() + b->transform.position;

				Vector dp = posa - posb;

				if (dp.Magnitude() < a->radius + b->radius)
				{
					a->parent->Translate(Vector(0, 10, 0));
				}
			}
		}
	}
}

void GEngine::Render()
{
	Camera* cam = active_game->GetActiveCamera();
	Vector4 camPos = cam->GetTransform().position.ToVector4();
	Matrix V = Matrix::CreateView(
		-cam->GetForwardVector().ToVector4(),
		cam->GetUpVector().ToVector4(),
		cam->GetRightVector().ToVector4(),
		camPos);

	GL::Clear(0x7F7F7F);
	GL::LoadMatrix(V);

	for (int i = 0; i < active_game->objects.Count(); i++)
	{
		GameObject* obj = active_game->objects[i];

		for (int j = 0; j < obj->mesh_components.Count(); j++)
		{
			MeshComponent* mesh = obj->mesh_components[j];

			GameObject* parent = mesh->parent;
			Transform* trans = &parent->GetWorldTransform();
			Matrix M = Matrix::CreateTransformation(trans->position.ToVector4(), trans->rotation.ToVector4(), trans->scale.ToVector4());

			GL::PushMatrix();
			GL::MulMatrix(M);

			GL::BindTexture(mesh->tex_id);
			GL::VertexPointer(mesh->vertices);
			GL::Draw(0, mesh->vertex_count);

			GL::PopMatrix();
		}
	}

	GL::SwapBuffers();
}