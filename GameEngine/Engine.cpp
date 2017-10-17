#include "Engine.h"
#include "math.h"
#include "../GL/GL.h"
#include "Matrix3.h"
#include "System.h"
#include "MeshComponent.h"
#include "Collision.h"
#include "Rigidbody.h"
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
int totalFrames = 0;
int startTicks = 0;

bool BOOL1 = false;

GC _gc;

GEngine::GEngine(GC gc)
{
	_gc = gc;
	GL::Init(gc);
	active_game = 0;
	deltaTime = 1 / 100.0f;
}

void GEngine::StartGame(Game* game)
{
	active_game = game;

	startTicks = PIT::ticks;

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

	GL::MatrixMode(GL_PROJECTION);
	GL::LoadMatrix(Matrix4::CreatePerspectiveProjection(_gc.width, _gc.height, 90, 1, 10));
	GL::MatrixMode(GL_MODELVIEW);

	/*while (1)
	{
		Point points[] = {
			Point(100, 100),
			Point(200, 200),

			Point(250, 300),
			Point(300, 400),

			Point(550, 300),
			Point(800, 200),

			Point(600, 500),
			Point(400, 800),

			Point(200, 500)
		};

		//Drawing::Clear(0, gc);
		Drawing::DrawBezierQuad(points, sizeof(points) / sizeof(Point));
		Drawing::Draw(gc);
	}*/

	while (1)
	{
		Debug::x = 0;
		Debug::y = 0;

		if (PIT::ticks != ticks)
		{
			Debug::Print("FPS: %i\tDT:%i\tAvgDT: %i\n", 1000 / (PIT::ticks - ticks), PIT::ticks - ticks, (ticks - startTicks) / totalFrames);
			char buf[20];
			//Debug::Print("AVG: %i\n", frames * 1000 / PIT::ticks);
		}

		Debug::Print("Ticks: %i\n", ticks);
		Debug::Print("Free %i\n", Memory::num_free);

		Debug::Print("Cam: %s\n", cam->transform.ToString(buf));
		Debug::Print("P1: %s\n", thing->transform.ToString(buf));

		if (thing->rigidbody)
		{
			Vector3 vel = thing->rigidbody->velocity;
			Debug::Print("V1: [%f, %f, %f]\n", vel.x, vel.y, vel.z);
		}

		if (Keyboard::GetKeyDown(KEY_TAB))
		{
			GL::MatrixMode(GL_PROJECTION);
			GL::LoadMatrix(Matrix4::CreatePerspectiveProjection(1920 * GL::gc_out.width, 1080 * GL::gc_out.height, 90, 1, 10));
			GL::MatrixMode(GL_MODELVIEW);
		}

		if (Keyboard::GetKeyDown(KEY_ESCAPE))
			break;

		Update();
		//Collision();
		Render();

		totalFrames++;
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
	Vector3 mouse_delta = mouse_pos - last_mouse_pos;
	Vector3 mouse_axis = mouse_delta * deltaTime;
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
		active_game->objects[0]->transform.rotation.Rotate(Vector3(0, 0, 1), 2 * M_PI * deltaTime * sign);
	}

	if (Keyboard::GetKeyDown(KEY_D4))
	{
		//active_game->objects[0]->transform.rotation.x += 2 * M_PI * deltaTime * sign;
		active_game->objects[1]->transform.rotation.Rotate(Vector3(1, 0, 0), 2 * M_PI * deltaTime * sign);
	}

	if (Keyboard::GetKeyDown(KEY_D5))
	{
		active_game->objects[1]->transform.rotation.Rotate(Vector3(0, 1, 0), 2 * M_PI * deltaTime * sign);
	}

	if (Keyboard::GetKeyDown(KEY_D6))
	{
		active_game->objects[1]->transform.rotation.Rotate(Vector3(0, 0, 1), 2 * M_PI * deltaTime * sign);
	}


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
		cam->RotateEuler(Vector3(mouse_delta.y, mouse_delta.x, 0) * 0.01);
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

float err = 0;

void GEngine::Collision()
{
	float energy = 0;

	std::List<Rigidbody*> all;

	for (int i = 0; i < active_game->objects.Count(); i++)
	{
		GameObject* obj = active_game->objects[i];

		Rigidbody* comp = obj->rigidbody;
		all.Add(comp);

		if (comp->bEnabledGravity)
			energy += 1 * 9.8 * (obj->GetWorldPosition().y + 1000);
		energy += 0.5 * comp->SpeedSquared();
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

	/*Debug::color = 0xFFFFFF00;
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

	Debug::color = 0xFF00FF00;*/

	float hjk = (PIT::ticks % 2000) / 2000.0f;
	while (hjk > M_PI / 2)
		hjk -= M_PI / 2;
	//hjk = 30 * M_PI / 180.0f;
	//hjk = 0.54630248984379051325517946578029;
	Debug::Print("%f %f %f %f\n", tan(hjk), atan(hjk), (atan(tan(hjk)) - hjk) * 1000, 0);


	float asd = (atan(tan(hjk)) - hjk) * 1000;
	if (asd > err)
		err = asd;

	Debug::Print("Err: %f\n", err);

	for (int i = 0; i < all.Count(); i++)
	{
		int start = i + 1;
		for (int j = start; j < all.Count(); j++)
		{
			//if (all[i]->IsBox() && all[j]->IsBox())
			if (1)
			{
				Rigidbody* a = (Rigidbody*)all[i];
				Rigidbody* b = (Rigidbody*)all[j];
				a->collider->size = a->parent->GetWorldScale();
				b->collider->size = b->parent->GetWorldScale();

				//if (a->parent->GetWorldPosition().y > 9)
				//	a->parent->transform.rotation = Quaternion::FromEuler(Vector3(0, 0, M_PI_4));


				Vector3 mtv(0, 0, 0);
				int count;
				Manifold* man;

				//if (a->IsColliding(b, mtv))
					//if(a->parent->transform.position.y < 0)

				::Collision test;

				if (test.TestIntersection(*a, *b, &mtv, man, count))
				{
					Debug::color = 0xFFFFFFFF;

					//a->velocity = Vector3(0, 0, 0);
					a->parent->transform.position += mtv;

					//Debug::Print("COLLISION");

					Debug::color = 0xFFFF0000;
					for (int p = 0; p < count; p++)
					{
						Manifold& m = man[p];

						int h = m.Bod1 == a;
						Debug::Print("%i %i\t%f %f %f\n", p, h, m.Point.x, m.Point.y, m.Point.z);
					}
					Debug::color = 0xFF00FF00;

					if (count == 0)
						continue;

					//PIT::Sleep(100);

					Vector3 va;
					Vector3 vb;

					Vector3 posa = a->parent->GetWorldPosition();
					Vector3 posb = b->parent->GetWorldPosition();

					Vector3 dp = posa - posb;

					va = a->velocity;
					vb = b->velocity;

					Vector3 dv = va - vb;
					Vector3 dir = dp.Normalized();
					Vector3 dirv = dv.Normalized();
					float dot = dir.Dot(dirv);

					//if (dot < 0 && !BOOL1)
					{
						//BOOL1 = 1;
						//Vector3 imp = dir * dot * dv.Magnitude() / 1.5;
						//pha->AddImpulse(-imp);
						//phb->AddImpulse(imp);

						//pha->AddImpulseAt(-imp, a->parent->GetWorldPosition());

						a->mass = 1;
						//b->mass = 1;
						b->mass = 1e10;

						float e = 1;
						float ma = a->mass;
						float mb = b->mass;

						Matrix3 Ia = Matrix3();
						Matrix3 Ib = Matrix3();

						Vector3& sizea = a->collider->size;
						Ia[0] = ma * (sizea.y * sizea.y + sizea.z * sizea.z) / 12;
						Ia[4] = ma * (sizea.x * sizea.x + sizea.z * sizea.z) / 12;
						Ia[8] = ma * (sizea.x * sizea.x + sizea.y * sizea.y) / 12;

						Vector3& sizeb = b->collider->size;
						Ib[0] = mb * (sizeb.y * sizeb.y + sizeb.z * sizeb.z) / 12;
						Ib[4] = mb * (sizeb.x * sizeb.x + sizeb.z * sizeb.z) / 12;
						Ib[8] = mb * (sizeb.x * sizeb.x + sizeb.y * sizeb.y) / 12;


						//Matrix3 MatR = a->parent->GetWorldRotation().ToMatrix3();
						//Matrix3 MatR = Matrix3::CreateRotation(a->parent->transform.rotation.ToEuler());
						//Ia = (MatR * Ia * MatR.Transpose());
						//Debug::Print("%f %f %f %f %f %f", Ia[0], Ia[1], Ia[2], Ia[3], Ia[4], Ia[5]);
						//Matrix3 MatR2 = b->parent->GetWorldRotation().ToMatrix3();
						//Matrix3 MatR2 = Matrix3::CreateRotation(b->parent->transform.rotation.ToEuler());
						//Ib = (MatR2 * Ib * MatR2.Transpose());

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

						Debug::color = 0xFF0000FF;
						Debug::Print("%f\t%f\t%f\n", vaf.x, vaf.y, vaf.z);
						Debug::Print("%f\t%f\t%f\n\n", vbf.x, vbf.y, vbf.z);
						Debug::Print("%f\t%f\t%f\n", waf.x, waf.y, waf.z);
						Debug::Print("%f\t%f\t%f\n", wbf.x, wbf.y, wbf.z);
						Debug::color = 0xFF00FF00;

						pha->angularVelocity = waf;
						phb->angularVelocity = wbf;*/

						Vector3 Xa = posa;
						Vector3 Xb = posb;

						Vector3 n = -mtv.Normalized();
						//Vector3 n = test.points[0]->Normal;

						Vector3 totalra;
						Vector3 totalrb;

						for (int p = 0; p < count; p++)
						{
							if (man[p].Bod1 == a)
							{
								man[p].Point = -man[p].Point;
								//n = -n;
							}
							//n = man[p].Normal;

							n = -Vector3(0, 1, 0);

							//ra = a->parent->GetWorldRotation() * Vector3(0, -1, 0);
							//rb = b->parent->GetWorldRotation() * Vector3(0, 1, 0);

							//ra = Vector3(-1, -1, -1);
							//rb = Vector3(1, 1, 1);

							Vector3 ra = a->parent->GetWorldRotation() * -(Xa - man[p].Point);
							Vector3 rb = b->parent->GetWorldRotation() * -(man[p].Point - Xb);
							Debug::Print("%f %f %f\n", ra.x, ra.y, ra.z);
							Debug::Print("%f %f %f\n", n.x, n.y, n.z);
							//PIT::Sleep(10000);

							//ra = Vector3(0, -1, 0);
							//rb = Vector3(0, 1, 0);

							//ra = a->parent->GetWorldRotation() * Vector3((posb.x - posa.x) / 2, -1, (posb.z - posa.z) / 2);
							//rb = b->parent->GetWorldRotation() * Vector3((posb.x - posa.x) / -2, 1, (posb.z - posa.z) / -2);
							//ra = man[0].Point - a->parent->GetWorldPosition();
							//rb = man[0].Point - a->parent->GetWorldPosition();

							Matrix3 inva = Ia.Inverse();
							Matrix3 invb = Ib.Inverse();

							float Vrel = -(b->velocity - a->velocity).Dot(n);


							float N = -(1 + e) * Vrel;
							float t1 = 1 / ma;
							float t2 = 1 / mb;
							float t3 = n.Dot(inva * ra.Cross(n).Cross(ra));
							float t4 = n.Dot(invb * rb.Cross(n).Cross(rb));

							float J = N / (t1 + t2 + t3 + t4);
							Vector3 force = n * J;

							a->AddImpulse(force);
							b->AddImpulse(-force);

							Vector3 waf = inva * Vector3::Cross(ra, force);
							Vector3 wbf = invb * Vector3::Cross(rb, force);

							//a->angularVelocity = waf;
							//b->angularVelocity = wbf;
							totalra -= waf;
							totalrb += wbf;

							//Debug::color = 0xFF0000FF;
							//Debug::Print("%f\t%f\t%f\n", waf.x, waf.y, waf.z);
							//Debug::Print("%f\t%f\t%f\n", wbf.x, wbf.y, wbf.z);
							//Debug::color = 0xFF00FF00;

							//Debug::Print("J: %f\n", j);
						}

						//a->parent->transform.rotation = Quaternion::FromAxisAngle(Vector3(0, 0, 1), 0.1);
						a->angularVelocity = totalra;
						b->angularVelocity = totalrb;

						//Debug::Print("_ %f\t%f\t%f\n", mtv.x, mtv.y, mtv.z);
						//PIT::Sleep(100);
					}
				}
			}
			//else if (all[i]->IsSphere() && all[j]->IsSphere())
			else
			{
				Rigidbody* a = all[i];
				Rigidbody* b = all[j];

				Vector3 posa = a->parent->GetWorldPosition() + a->parent->transform.position;
				Vector3 posb = b->parent->GetWorldPosition() + b->parent->transform.position;

				Vector3 dp = posa - posb;

				float d = dp.Magnitude() / (2 + 2);
				if (d < 1)
				{
					//a->parent->Translate(Vector3(0, 10, 0));

					Vector3 va;
					Vector3 vb;

					va = a->velocity;
					vb = b->velocity;

					Vector3 dv = va - vb;
					Vector3 dir = dp.Normalized();
					Vector3 dirv = dv.Normalized();
					float dot = dir.Dot(dirv);

					if (dot < 0)
					{
						Vector3 imp = dir * dot * dv.Magnitude() / 1.5;
						a->AddImpulse(-imp);
						b->AddImpulse(imp);
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

			for (int v = 0; v < mesh->vertex_count; v++)
				mesh->vertices[v].worldNormal = R * mesh->vertices[v].normal;

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