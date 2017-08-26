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

GEngine::GEngine()
{
	active_game = 0;
	deltaTime = 1 / 100.0f;
}

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
			Debug::Print("FPS: %i\tDT:%i\n", 1000 / (PIT::ticks - ticks), PIT::ticks - ticks);
			//Debug::Print("AVG: %i\n", frames * 1000 / PIT::ticks);
		}

		Debug::Print("Ticks: %i\n", ticks);
		Debug::Print("Free %i\n", Memory::num_free);

		Debug::Print("Cam: %s\n", cam->transform.ToString(buf));
		Debug::Print("P1: %s\n", thing->transform.ToString(buf));

		if (thing->physicsComponent)
		{
			Vector vel = thing->physicsComponent->velocity;
			Debug::Print("V1: [%f, %f, %f]\n", vel.x, vel.y, vel.z);
		}

		Update();
		Collision();
		Render();
	}
}

void GEngine::Update()
{
	deltaTime = (PIT::ticks - ticks) / 1000.f;
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
	Vector mouse_axis = (mouse_pos - last_mouse_pos) * deltaTime;
	last_mouse_pos = mouse_pos;


	int sign = Keyboard::shift ? -1 : 1;

	if (Keyboard::GetKeyDown(KEY_D1))
	{
		p1 += sign * deltaTime * 2000;
		//active_game->objects[0]->transform.rotation.x += 2 * M_PI * deltaTime * sign;
		active_game->objects[0]->transform.rotation.Rotate(Vector(1, 0, 0), 2 * M_PI * deltaTime * sign);
	}

	if (Keyboard::GetKeyDown(KEY_D2))
	{
		p2 += sign * deltaTime * 400;
		active_game->objects[0]->transform.rotation.Rotate(Vector(0, 1, 0), 2 * M_PI * deltaTime * sign);
	}

	if (Keyboard::GetKeyDown(KEY_D3))
	{
		p3 += sign * deltaTime / 10;
		active_game->objects[0]->transform.rotation.Rotate(Vector(0, 0, 1), 2 * M_PI * deltaTime * sign);
	}

	if (Keyboard::GetKeyDown(KEY_D4))
		p4 += sign * deltaTime * 0.1;

	if (Keyboard::GetKeyDown(KEY_D5))
		p5 += sign * deltaTime * 10;

	Camera* cam = active_game->GetActiveCamera();

	if (Keyboard::GetKeyDown(KEY_F))
	{
		active_game->objects[0]->transform.Rotate(Vector(4 * deltaTime, 0, 0) * deltaTime, mouse_axis.y);
	}
	if (Keyboard::GetKeyDown(KEY_G))
	{
		active_game->objects[0]->transform.Rotate(Vector(0, 4 * deltaTime, 0), -mouse_axis.x);
	}

	if (Mouse::mouse_R)
	{
		//cam->transform.rotation.Rotate(Vector(1, 0, 0), -mouse_axis.y);
		//cam->transform.rotation *= Quaternion(1, 0, 0, mouse_axis.x * deltaTime);
		
		//cam->transform.Rotate(Vector(0, 1, 0), mouse_axis.x);
		//cam->Rotate(Vector(0, 1, 0) * -mouse_axis.x);
		//cam->Rotate(-Vector(mouse_axis.y, mouse_axis.x, 0) * 0.2);
	}
	else if (Mouse::mouse_L)
	{
		//light->Rotate(Vector(mouse_axis.y, mouse_axis.x, 0) * 0.2);
	}

	float speed = 10;

	if (Keyboard::GetKeyDown(KEY_D))
		cam->transform.Translate(cam->transform.GetRightVector() * speed * deltaTime);
	if (Keyboard::GetKeyDown(KEY_A))
		cam->transform.Translate(-cam->transform.GetRightVector() * speed * deltaTime);
	if (Keyboard::GetKeyDown(KEY_W))
		cam->transform.Translate(cam->transform.GetForwardVector() * speed * deltaTime);
	if (Keyboard::GetKeyDown(KEY_S))
		cam->transform.Translate(-cam->transform.GetForwardVector() * speed * deltaTime);
	if (Keyboard::GetKeyDown(KEY_E))
		cam->transform.Translate(cam->transform.GetUpVector() * speed * deltaTime);
	if (Keyboard::GetKeyDown(KEY_Q))
		cam->transform.Translate(-cam->transform.GetUpVector() * speed * deltaTime);

	for (int i = 0; i < active_game->objects.Count(); i++)
	{
		GameObject* obj = active_game->objects[i];
		obj->Update(deltaTime);
	}

	for (int i = 0; i < active_game->objects.Count(); i++)
	{
		GameObject* obj = active_game->objects[i];

		for (int j = 0; j < obj->components.Count(); j++)
		{
			Component* comp = obj->components[j];
			comp->Update(deltaTime);
		}
	}
}


void GEngine::Collision()
{
	float energy = 0;

	std::List<ColliderComponent*> all;

	for (int i = 0; i < active_game->objects.Count(); i++)
	{
		GameObject* obj = active_game->objects[i];

		PhysicsComponent* comp = obj->physicsComponent;

		if (comp->bEnabledGravity)
			energy += 1 * 9.8 * (obj->GetWorldPosition().y + 1000);

		energy += 0.5 * comp->SpeedSquared();

		for (int j = 0; j < obj->colliderComponents.Count(); j++)
		{
			ColliderComponent* mesh = obj->colliderComponents[j];
			all.Add(mesh);
		}
	}

	Debug::Print("Energy: %f\n", energy);

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

				float d = dp.Magnitude() / (a->radius + b->radius);
				if (d < 1)
				{
					//a->parent->Translate(Vector(0, 10, 0));

					if (a->OnCollision)
						a->OnCollision();

					PhysicsComponent* pha = a->parent->physicsComponent;
					PhysicsComponent* phb = b->parent->physicsComponent;

					Vector va;
					Vector vb;

					if (pha || pha)
					{
						if (pha)
							va = pha->velocity;
						if (phb)
							vb = phb->velocity;

						Vector dv = va - vb;
						Vector dir = dp.Normalized();
						Vector dirv = dv.Normalized();
						float dot = dir.Dot(dirv);

						if (dot < 0)
						{
							Vector imp = dir * dot * dv.Magnitude() / 1.5;
							pha->AddImpulse(-imp);
							phb->AddImpulse(imp);
						}
					}
				}
			}
		}
	}
}

void GEngine::Render()
{
	Camera* cam = active_game->GetActiveCamera();
	Matrix V = Matrix::CreateView(
		-cam->transform.GetForwardVector().ToVector4(),
		cam->transform.GetUpVector().ToVector4(),
		cam->transform.GetRightVector().ToVector4(),
		cam->transform.position.ToVector4());

	//Matrix VT = Matrix::CreateTranslation(camPos);
	//Matrix VR = cam->transform.rotation.ToMatrix();
	//Matrix V = VT;

	GL::Clear(0x7F7F7F);
	GL::LoadMatrix(V);

	for (int i = 0; i < active_game->objects.Count(); i++)
	{
		GameObject* obj = active_game->objects[i];

		for (int j = 0; j < obj->meshComponents.Count(); j++)
		{
			MeshComponent* mesh = obj->meshComponents[j];

			GameObject* parent = mesh->parent;
			Transform* trans = &parent->GetWorldTransform();
			//Matrix M = Matrix::CreateTransformation(trans->position.ToVector4(), trans->rotation.ToMatrix(), trans->scale.ToVector4());
			Matrix T = Matrix::CreateTranslation(trans->position.ToVector4());
			Matrix R = trans->rotation.ToMatrix();
			Matrix S = Matrix::CreateScale(trans->scale.ToVector4());

			Matrix M = T * R * S;

			GL::PushMatrix();
			GL::MulMatrix(M);

			GL::BindTexture(mesh->texId);
			GL::VertexPointer(mesh->vertices);
			GL::Draw(0, mesh->vertex_count);

			GL::PopMatrix();
		}
	}

	GL::SwapBuffers();
}