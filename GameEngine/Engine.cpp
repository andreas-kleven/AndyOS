#include "Engine.h"
#include "math.h"
#include "../GL/GL.h"
#include "Matrix3.h"
#include "System.h"
#include "MeshComponent.h"
#include "ColliderComponent.h"
#include "SphereCollider.h"
#include "BoxCollider.h"
#include "DirectionalLight.h"
#include "stdio.h"

using namespace gl;

int p1 = 0;
float p2 = 0;
float p3 = 0;
float p4 = 1;
float p5 = 60;

Vector3 last_mouse_pos;
int ticks;

bool BOOL1 = false;

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

	last_mouse_pos = Vector3(Mouse::x, Mouse::y, 0);
	ticks = PIT::ticks;

	Matrix4 P = Matrix4::CreatePerspectiveProjection(1024, 768, 90, 1, 10);

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
			char buf[20];
			//Debug::Print("AVG: %i\n", frames * 1000 / PIT::ticks);
		}

		Debug::Print("Ticks: %i\n", ticks);
		Debug::Print("Free %i\n", Memory::num_free);

		Debug::Print("Cam: %s\n", cam->transform.ToString(buf));
		Debug::Print("P1: %s\n", thing->transform.ToString(buf));

		if (thing->physicsComponent)
		{
			Vector3 vel = thing->physicsComponent->velocity;
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

	Vector3 mouse_pos(Mouse::x, Mouse::y, 0);
	Vector3 mouse_axis = (mouse_pos - last_mouse_pos) * deltaTime;
	last_mouse_pos = mouse_pos;


	int sign = Keyboard::shift ? -1 : 1;

	if (Keyboard::GetKeyDown(KEY_D1))
	{
		p1 += sign * deltaTime * 2000;
		//active_game->objects[0]->transform.rotation.x += 2 * M_PI * deltaTime * sign;
		active_game->objects[0]->transform.rotation.Rotate(Vector3(1, 0, 0), 2 * M_PI * deltaTime * sign);
	}

	if (Keyboard::GetKeyDown(KEY_D2))
	{
		p2 += sign * deltaTime * 400;
		active_game->objects[0]->transform.rotation.Rotate(Vector3(0, 1, 0), 2 * M_PI * deltaTime * sign);
	}

	if (Keyboard::GetKeyDown(KEY_D3))
	{
		p3 += sign * deltaTime / 10;
		active_game->objects[0]->transform.rotation.Rotate(Vector3(0, 0, 1), 2 * M_PI * deltaTime * sign);
	}

	if (Keyboard::GetKeyDown(KEY_D4))
		p4 += sign * deltaTime * 0.1;

	if (Keyboard::GetKeyDown(KEY_D5))
		p5 += sign * deltaTime * 10;

	Camera* cam = active_game->GetActiveCamera();

	if (Keyboard::GetKeyDown(KEY_F))
	{
		active_game->objects[0]->transform.Rotate(Vector3(4 * deltaTime, 0, 0) * deltaTime, mouse_axis.y);
	}
	if (Keyboard::GetKeyDown(KEY_G))
	{
		active_game->objects[0]->transform.Rotate(Vector3(0, 4 * deltaTime, 0), -mouse_axis.x);
	}

	if (Mouse::mouse_R)
	{
		Vector3 euler = cam->transform.rotation.ToEuler();
		euler.x += mouse_axis.y;
		euler.y += mouse_axis.x;
		cam->transform.rotation = Quaternion::FromEuler(euler);

		//cam->transform.rotation.Rotate(Vector3(1, 0, 0), -mouse_axis.y);
		//cam->transform.rotation *= Quaternion(1, 0, 0, mouse_axis.x * deltaTime);

		//cam->transform.Rotate(Vector3(0, 1, 0), mouse_axis.x);
		//cam->Rotate(Vector3(0, 1, 0) * -mouse_axis.x);
		//cam->Rotate(-Vector3(mouse_axis.y, mouse_axis.x, 0) * 0.2);
	}
	else if (Mouse::mouse_L)
	{
		//light->Rotate(Vector3(mouse_axis.y, mouse_axis.x, 0) * 0.2);
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

	/*Matrix mat;
	mat[0] = 3;
	mat[1] = 12;
	mat[2] = 4;
	mat[3] = 5;
	mat[4] = 6;
	mat[5] = 8;
	mat[6] = 1;
	mat[7] = 0;
	mat[8] = 2;

	Matrix m2 = mat.Inverse();
	Debug::Clear(0);

	for (int i = 0; i < 9; i++)
	{
		Debug::Print("%f\t", m2[i]);
	}
	while (1);*/

	/*Debug::color = 0xFFFF00;
	Debug::Print("%f\n", atan2(1, 1));
	Debug::Print("%f\n", tan(atan(4)));
	Debug::Print("%f\n", atan(PIT::ticks / 1000.0f) - PIT::ticks / 1000.0f);

	Vector3 start(M_PI, 0, 0);
	Quaternion quat = Quaternion::FromEuler(start);
	//quat.Rotate(Vector3(1, 0, 0), M_PI);
	Vector3 ve = quat.ToEuler();
	Quaternion quat2 = Quaternion::FromEuler(ve);

	Debug::Print("%f\t%f\t%f\n", ve.x, ve.y, ve.z);
	Debug::Print("%f\t%f\t%f\n", start.x, start.y, start.z);

	Debug::color = 0xFF00;*/

	for (int i = 0; i < all.Count(); i++)
	{
		int start = i + 1;
		for (int j = start; j < all.Count(); j++)
		{
			if (all[i]->IsBox() && all[j]->IsBox())
			{
				BoxCollider* a = (BoxCollider*)all[i];
				BoxCollider* b = (BoxCollider*)all[j];

				//a->parent->transform.rotation = Quaternion::FromEuler(Vector3(M_PI_2, 0, 0));

				if (a->IsColliding(b))
				//if(a->parent->transform.position.y < 0)
				{
					//Debug::color = 0xFF0000;
					//Debug::Print("COLLISION");
					//Debug::color = 0xFF00;

					PhysicsComponent* pha = a->parent->physicsComponent;
					PhysicsComponent* phb = b->parent->physicsComponent;

					Vector3 va;
					Vector3 vb;

					Vector3 posa = a->parent->GetWorldPosition() + a->transform.position;
					Vector3 posb = b->parent->GetWorldPosition() + b->transform.position;

					Vector3 dp = posa - posb;
					
					if (pha || pha)
					{
						if (pha)
							va = pha->velocity;
						if (phb)
							vb = phb->velocity;

						Vector3 dv = va - vb;
						Vector3 dir = dp.Normalized();
						Vector3 dirv = dv.Normalized();
						float dot = dir.Dot(dirv);

						if (dot < 0 && !BOOL1)
						{
							BOOL1 = 1;
							//Vector3 imp = dir * dot * dv.Magnitude() / 1.5;
							//pha->AddImpulse(-imp);
							//phb->AddImpulse(imp);

							//pha->AddImpulseAt(-imp, a->parent->GetWorldPosition());

							pha->mass = 1;
							phb->mass = 1;

							float e = 0.5;
							float ma = pha->mass;
							float mb = phb->mass;

							Matrix3 Ia = Matrix3();
							Matrix3 Ib = Matrix3();

							Ia[0] = ma / 6;
							Ia[4] = ma / 6;
							Ia[8] = ma / 6;

							Ib[0] = mb / 6;
							Ib[4] = mb / 6;
							Ib[8] = mb / 6;


							//Matrix3 MatR = Matrix3::CreateRotation(pha->parent->transform.rotation.ToEuler());
							//Ia = (MatR * Ia * MatR.Transpose());

							//for (int i = 0; i < 9; i++)
							//{
							//	Ia[i] = 0;
							//	Ib[i] = 0;
							//}

							/*Vector3 ra = Vector3((posb.x - posa.x) / 2, -1, 0) + posa;
							Vector3 rb = Vector3((posb.x - posa.x) / -2, 1, 0) + posb;

							Vector3 n = -dir;

							Vector3 vai = pha->velocity;
							Vector3 vbi = phb->velocity;

							Vector3 wai = Vector3();
							Vector3 wbi = Vector3();

							Vector3 vaf;
							Vector3 vbf;

							Vector3 waf;
							Vector3 wbf;

							Matrix3 IaInverse = Ia.Inverse();
							Matrix3 IbInverse = Ib.Inverse();

							Vector3 normal = n.Normalized();

							Vector3 angularVelChangea = normal; // start calculating the change in angular rotation of a
							angularVelChangea = angularVelChangea.Cross(ra);
							angularVelChangea = IaInverse * angularVelChangea;
							Vector3 vaLinDueToR = angularVelChangea.Cross(ra);  // calculate the linear velocity of collision point on a due to rotation of a

							double scalar = 1 / ma + vaLinDueToR.Dot(normal);

							Vector3 angularVelChangeb = normal; // start calculating the change in angular rotation of b
							angularVelChangeb = angularVelChangeb.Cross(rb);
							angularVelChangeb = IbInverse * angularVelChangeb;
							Vector3 vbLinDueToR = angularVelChangeb.Cross(rb);  // calculate the linear velocity of collision point on b due to rotation of b

							scalar += 1 / mb + vbLinDueToR.Dot(normal);

							double Jmod = (e + 1)*(vai - vbi).Magnitude() / scalar;
							Vector3 J = normal * Jmod;

							vaf = vai - J * (1 / ma);
							vbf = vbi - J * (1 / mb);
							waf = wai - angularVelChangea;
							wbf = wbi - angularVelChangeb;

							pha->velocity = vaf;
							phb->velocity = -vbf;

							Debug::color = 0xFF;
							Debug::Print("%f\t%f\t%f\n", vaf.x, vaf.y, vaf.z);
							Debug::Print("%f\t%f\t%f\n\n", vbf.x, vbf.y, vbf.z);
							Debug::Print("%f\t%f\t%f\n", waf.x, waf.y, waf.z);
							Debug::Print("%f\t%f\t%f\n", wbf.x, wbf.y, wbf.z);
							Debug::color = 0xFF00;

							pha->angVelocity = waf;
							phb->angVelocity = wbf;*/

							Vector3 Xa = posa;
							Vector3 Xb = posb;
							float Vrel = (phb->velocity - pha->velocity).Magnitude();

							Vector3 n = Vector3(0, -1, 0);
							//Vector3 Pa = Vector3(0, -1, 0);
							//Vector3 Pb = Vector3(0, 1, 0);
							//Vector3 Pa = Vector3((posb.x - posa.x) / 2, -1, 0) + posa;
							//Vector3 Pb = Vector3((posb.x - posa.x) / -2, 1, 0) + posb;
							//
							//Vector3 ra = Pa - Xa;
							//Vector3 rb = Pb - Xb;

							Vector3 ra = Vector3((posb.x - posa.x) / -2, -1, (posb.z - posa.z) / -2);
							Vector3 rb = Vector3((posb.x - posa.x) / 2, 1, (posb.z - posa.z) / 2);

							Matrix3 inva = Ia.Inverse();
							Matrix3 invb = Ib.Inverse();

							float N = -(1 + e) * Vrel;
							float t1 = 1 / ma;
							float t2 = 1 / mb;
							float t3 = n.Dot(inva * ra.Cross(n).Cross(ra));
							float t4 = n.Dot(inva * ra.Cross(n).Cross(ra));

							float J = N / (t1 + t2 + t3 + t4);
							Vector3 force = n * J;
							
							pha->AddImpulse(force);
							phb->AddImpulse(-force);

							Vector3 waf = inva * Vector3::Cross(ra, force);
							Vector3 wbf = invb * Vector3::Cross(rb, -force);

							pha->angVelocity = -waf;
							phb->angVelocity = -wbf;

							Debug::color = 0xFF;
							Debug::Print("%f\t%f\t%f\n", waf.x, waf.y, waf.z);
							Debug::Print("%f\t%f\t%f\n", wbf.x, wbf.y, wbf.z);
							Debug::color = 0xFF00;

							Debug::Print("J: %f\n", j);
						}
					}
				}
			}
			else if (all[i]->IsSphere() && all[j]->IsSphere())
			{
				SphereCollider* a = (SphereCollider*)all[i];
				SphereCollider* b = (SphereCollider*)all[j];

				Vector3 posa = a->parent->GetWorldPosition() + a->transform.position;
				Vector3 posb = b->parent->GetWorldPosition() + b->transform.position;

				Vector3 dp = posa - posb;

				float d = dp.Magnitude() / (a->radius + b->radius);
				if (d < 1)
				{
					//a->parent->Translate(Vector3(0, 10, 0));

					if (a->OnCollision)
						a->OnCollision();

					PhysicsComponent* pha = a->parent->physicsComponent;
					PhysicsComponent* phb = b->parent->physicsComponent;

					Vector3 va;
					Vector3 vb;

					if (pha || pha)
					{
						if (pha)
							va = pha->velocity;
						if (phb)
							vb = phb->velocity;

						Vector3 dv = va - vb;
						Vector3 dir = dp.Normalized();
						Vector3 dirv = dv.Normalized();
						float dot = dir.Dot(dirv);

						if (dot < 0)
						{
							Vector3 imp = dir * dot * dv.Magnitude() / 1.5;
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
	Matrix4 V = Matrix4::CreateView(
		-cam->transform.GetForwardVector().ToVector4(),
		cam->transform.GetUpVector().ToVector4(),
		cam->transform.GetRightVector().ToVector4(),
		cam->transform.position.ToVector4());

	Vector3 v = cam->transform.GetUpVector();
	Debug::Print("%f\t%f\t%f", v.x, v.y, v.z);

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
			Matrix4 T = Matrix4::CreateTranslation(trans->position.ToVector4());
			Matrix4 R = trans->rotation.ToMatrix();
			Matrix4 S = Matrix4::CreateScale(trans->scale.ToVector4());

			Matrix4 M = T * R * S;

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