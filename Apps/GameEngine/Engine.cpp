#include "GEngine.h"
#include "math.h"
#include "stdio.h"
#include "ctype.h"

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

	startTicks = get_ticks();

	Camera* cam = active_game->GetActiveCamera();

	GameObject* thing = active_game->GetObject("Thing");
	GameObject* floor = active_game->GetObject("Floor");

	char buf[256];

	int mouse_x, mouse_y;
	get_mouse_pos(mouse_x, mouse_y);
	last_mouse_pos = Vector3(mouse_x, mouse_y, 0);

	ticks = get_ticks();

	GL::MatrixMode(GL_PROJECTION);
	GL::LoadMatrix(Matrix4::CreatePerspectiveProjection(_gc.width, _gc.height, 90, 1, 10));

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
		debug_reset();
		
		if (get_ticks() != ticks && totalFrames > 0)
		{
			debug_print("FPS: %i\tDT:%i\tAvgDT: %i\n", 1000 / (get_ticks() - ticks), get_ticks() - ticks, (ticks - startTicks) / totalFrames);
			char buf[20];
			//debug_print("AVG: %i\n", frames * 1000 / get_ticks());
		}

		debug_print("Ticks: %i\n", ticks);

		debug_print("Cam: %s\n", cam->transform.ToString(buf));
		debug_print("P1: %s\n", thing->transform.ToString(buf));

		if (thing->rigidbody)
		{
			Vector3 vel = thing->rigidbody->velocity;
			debug_print("V1: [%f, %f, %f]\n", vel.x, vel.y, vel.z);
		}

		if (Input::GetKey(KEY_TAB))
		{
			GL::MatrixMode(GL_PROJECTION);
			GL::LoadMatrix(Matrix4::CreatePerspectiveProjection(1920 * GL::gc_out.width, 1080 * GL::gc_out.height, 90, 1, 10));
		}

		if (Input::GetKey(KEY_ESCAPE))
			break;

		Update();
		//Collision();
		Render();

		totalFrames++;
	}
}

void GEngine::Update()
{
	deltaTime = (get_ticks() - ticks) / 1000.f;
	ticks = get_ticks();

	/*if (Mouse::x == GL::m_width)
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
	}*/

	int mouse_x, mouse_y;
	get_mouse_pos(mouse_x, mouse_y);
	Vector3 mouse_pos = Vector3(mouse_x, mouse_y, 0);

	bool mouse_r, mouse_l, mouse_m;
	get_mouse_buttons(mouse_l, mouse_r, mouse_m);

	Vector3 mouse_delta = mouse_pos - last_mouse_pos;
	Vector3 mouse_axis = mouse_delta * deltaTime;
	last_mouse_pos = mouse_pos;

	//float sign = Keyboard::shift ? -1 : 1;.
	float sign = 1;

	if (Input::GetKey(KEY_LCTRL))
	{
		sign *= 0.2;
	}

	if (Input::GetKey(KEY_D1))
	{
		p1 += sign * deltaTime * 2000;
		//active_game->objects[0]->transform.rotation.x += 2 * M_PI * deltaTime * sign;
		active_game->objects[0]->transform.rotation.Rotate(Vector3(1, 0, 0), M_PI * deltaTime * sign);
	}

	if (Input::GetKey(KEY_D2))
	{
		p2 += sign * deltaTime * 400;
		active_game->objects[0]->transform.rotation.Rotate(Vector3(0, 1, 0), M_PI * deltaTime * sign);
	}

	if (Input::GetKey(KEY_D3))
	{
		active_game->objects[0]->transform.rotation.Rotate(Vector3(0, 0, 1), M_PI * deltaTime * sign);
	}

	if (Input::GetKey(KEY_D4))
	{
		//active_game->objects[0]->transform.rotation.x += 2 * M_PI * deltaTime * sign;
		active_game->objects[0]->transform.Translate(Vector3(1, 0, 0) * deltaTime * sign);
		//active_game->objects[1]->transform.rotation.Rotate(Vector3(1, 0, 0), M_PI * deltaTime * sign);
	}

	if (Input::GetKey(KEY_D5))
	{
		active_game->objects[0]->transform.Translate(Vector3(0, 1, 0) * deltaTime * sign);
		//active_game->objects[1]->transform.rotation.Rotate(Vector3(0, 1, 0), M_PI * deltaTime * sign);
	}

	if (Input::GetKey(KEY_D6))
	{
		active_game->objects[0]->transform.Translate(Vector3(0, 0, 1) * deltaTime * sign);
		//active_game->objects[1]->transform.rotation.Rotate(Vector3(0, 0, 1), M_PI * deltaTime * sign);
	}


	if (Input::GetKey(KEY_D0))
	{
		active_game->objects[1]->rigidbody->angularVelocity += Vector3(0, 0, 1) * M_PI * deltaTime * sign;
	}

	Camera* cam = active_game->GetActiveCamera();

	if (Input::GetKey(KEY_F))
	{
		active_game->objects[0]->transform.Rotate(Vector3(4 * deltaTime, 0, 0) * deltaTime, mouse_axis.y);
	}
	if (Input::GetKey(KEY_G))
	{
		active_game->objects[0]->transform.Rotate(Vector3(0, 4 * deltaTime, 0), -mouse_axis.x);
	}

	if (mouse_r)
	{
		LightSource* light = active_game->lights[0];
		light->transform.Translate(Vector3(mouse_delta.x, -mouse_delta.y, 0) * 0.1f);
	}
	else
	{
		cam->RotateEuler(Vector3(mouse_delta.y, mouse_delta.x, 0) * 0.01f);
	}

	float speed = 10;

	if (Input::GetKey(KEY_LSHIFT))
		speed /= 4;

	if (Input::GetKey(KEY_D))
		cam->transform.Translate(cam->transform.GetRightVector() * speed * deltaTime);
	if (Input::GetKey(KEY_A))
		cam->transform.Translate(-cam->transform.GetRightVector() * speed * deltaTime);
	if (Input::GetKey(KEY_W))
		cam->transform.Translate(cam->transform.GetForwardVector() * speed * deltaTime);
	if (Input::GetKey(KEY_S))
		cam->transform.Translate(-cam->transform.GetForwardVector() * speed * deltaTime);
	if (Input::GetKey(KEY_E))
		cam->transform.Translate(cam->transform.GetUpVector() * speed * deltaTime);
	if (Input::GetKey(KEY_Q))
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

Vector3 GEngine::WorldToScreen(Game* game, Vector3& point)
{
	Matrix4 P = Matrix4::CreatePerspectiveProjection(_gc.width, _gc.height, 90, 1, 10);

	Camera* cam = game->GetActiveCamera();
	Matrix4 V = Matrix4::CreateView(
		-cam->transform.GetForwardVector().ToVector4(),
		cam->transform.GetUpVector().ToVector4(),
		cam->transform.GetRightVector().ToVector4(),
		cam->transform.position.ToVector4());

	Vector4 p = P * V * point.ToVector4();


	if (p.w <= 0)
		return Vector3(0, 0, 0);

	Vector3 screen;
	screen.x = p.x * _gc.width / p.w + _gc.width * 0.5;
	screen.y = p.y * _gc.height / p.w + _gc.height * 0.5;

	//if (isnan(screen.x) || isnan(screen.y))
	//{
	//	debug_print("%f\t%f\t%f\n", p.x, p.y, p.w);
	//}

	return screen;
}


void GEngine::DebugLine(Game* game, Vector3 start, Vector3 end, Color& color)
{
	Vector3 lpstart = WorldToScreen(game, start);
	Vector3 lpend = WorldToScreen(game, end);

	float total = lpstart.x + lpend.x + lpstart.y + lpend.y;
	if (isnan(total))
	{
		return;
		debug_print("%f\t%f\t%i\t%i\t%i\n", lpstart.x, lpstart.y, (int)start.x, (int)start.y, (int)start.z);
		debug_print("%f\t%f\t%i\t%i\t%i\n", lpstart.y, lpstart.y, (int)end.x, (int)end.y, (int)end.z);
		while (1);
	}

	Drawing::DrawLine(lpstart.x, lpstart.y, lpend.x, lpend.y, color, _gc);
}

void GEngine::DebugBox(Game* game, Box& box, Color& color)
{
	Vector3 maxx = Vector3(box.max.x, box.min.y, box.min.z);
	Vector3 maxy = Vector3(box.min.x, box.max.y, box.min.z);
	Vector3 maxz = Vector3(box.min.x, box.min.y, box.max.z);

	Vector3 minx = Vector3(box.min.x, box.max.y, box.max.z);
	Vector3 miny = Vector3(box.max.x, box.min.y, box.max.z);
	Vector3 minz = Vector3(box.max.x, box.max.y, box.min.z);

	DebugLine(game, box.min, maxx, color);
	DebugLine(game, box.min, maxy, color);
	DebugLine(game, box.min, maxz, color);

	DebugLine(game, box.max, minx, color);
	DebugLine(game, box.max, miny, color);
	DebugLine(game, box.max, minz, color);

	DebugLine(game, maxx, miny, color);
	DebugLine(game, maxy, minz, color);
	DebugLine(game, maxz, minx, color);

	DebugLine(game, minx, maxy, color);
	DebugLine(game, miny, maxz, color);
	DebugLine(game, minz, maxx, color);
}

void PrintMatrix(Matrix3 M)
{
	for (int i = 0; i < 3; i++)
	{
		debug_print("[");
		for (int j = 0; j < 3; j++)
		{
			debug_print("%f", M[i * 3 + j]);

			if (j < 2)
				debug_print("\t");
		}
		debug_print("]\n");
	}
}

void PrintMatrix4(Matrix4 M)
{
	for (int i = 0; i < 4; i++)
	{
		debug_print("[");
		for (int j = 0; j < 3; j++)
		{
			debug_print("%f", M[i * 4 + j]);

			if (j < 3)
				debug_print("\t");
		}
		debug_print("]\n");
	}
}

Vector3 FrictionForce(Rigidbody* body, Vector3 normal, Vector3 vrel, Vector3 netForce)
{
	float fcs = 0.3;
	float fcd = 0.3;

	Vector3 normalForce = normal * netForce.Dot(normal);

	Vector3 Fs = normalForce * fcs;
	Vector3 Fk = normalForce * fcd;

	Vector3 t;

	if (abs(vrel.Dot(normal)) < 0.1)
	{
		if (abs(netForce.Dot(normal)) > 0.1)
		{
			//t = (netForce - (normal * netForce.Dot(normal))).Normalized();
		}
	}
	else {
		//t = (vrel - (vrel * max(0.f, netForce.Normalized().Dot(normal)))).Normalized();
		Vector3 nv = vrel.Normalized();
		t = (nv - (normal * nv.Dot(normal))).Normalized();
	}

	return -netForce / 4;

	Vector3 Ff = t * netForce.Dot(t);

	//if (Ff.Magnitude() > Fs.Magnitude())
	//	return -Ff;

	return normalForce - netForce;
	//return t * Fk.Magnitude();
	return Vector3();
	//if (fd < 10)
	//	return -(netForce - normal * netForce.Dot(normal));
	//else

	Vector3 fo = (netForce - (netForce * netForce.Dot(normal))).Normalized();
	return fo;

	return t * netForce.Dot(vrel.Normalized());
}

void GEngine::Collision()
{
	float energy = 0;

	List<Rigidbody*> all;

	for (int i = 0; i < active_game->objects.Count(); i++)
	{
		GameObject* obj = active_game->objects[i];

		Rigidbody* comp = obj->rigidbody;

		if (comp->bEnabled)
		{
			all.Add(comp);

			if (comp->bEnabledGravity)
				energy += 1 * 9.8 * (obj->GetWorldPosition().y + 1000);

			energy += 0.5 * comp->SpeedSquared();
			energy += 0.5 * comp->angularVelocity.MagnitudeSquared();
		}
	}

	debug_print("Energy: %f\n", energy);

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
				//a->collider->size = a->parent->GetWorldScale();
				//b->collider->size = b->parent->GetWorldScale();

				Vector3 mtv(0, 0, 0);
				int count;
				Manifold* man;

				::Collision test;

				if (test.TestIntersection(*a, *b, &mtv, man, count))
				{
					//a->velocity = Vector3(0, 0, 0);

					a->parent->transform.position += mtv * b->mass / (a->mass + b->mass);
					b->parent->transform.position -= mtv * a->mass / (a->mass + b->mass);

					//DrawLine(active_game, a->parent->GetWorldPosition(), a->parent->GetWorldPosition() - mtv, 0xFF00FF00);

					if (count == 0)
						continue;

					Vector3 colPoint;

					for (int p = 0; p < count; p++)
					{
						Manifold& m = man[p];
						colPoint += m.Point / count;

						Vector3 sc = WorldToScreen(active_game, m.Point);
						Drawing::FillRect(sc.x - 5, sc.y - 5, 10, 10, Color::Red, _gc);
					}

					Vector3 sc = WorldToScreen(active_game, colPoint);
					Drawing::FillRect(sc.x - 5, sc.y - 5, 10, 10, Color::Blue, _gc);

					Vector3 va;
					Vector3 vb;

					Quaternion rota = a->parent->GetWorldRotation();
					Quaternion rotb = b->parent->GetWorldRotation();

					Vector3 posa = a->parent->GetWorldPosition();
					Vector3 posb = b->parent->GetWorldPosition();

					Vector3 scalea = a->parent->GetWorldScale();
					Vector3 scaleb = b->parent->GetWorldScale();

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

						//a->mass = 1;
						//b->mass = 1;

						float e = 0.2;
						float ma = a->mass;
						float mb = b->mass;

						Matrix3 IbodyA = Matrix3();
						Matrix3 IbodyB = Matrix3();

						Vector3 sizea = Vector3(a->collider->size.x * scalea.x, a->collider->size.y * scalea.y, a->collider->size.z * scalea.z);
						IbodyA[0] = ma * (sizea.y * sizea.y + sizea.z * sizea.z) / 12;
						IbodyA[4] = ma * (sizea.x * sizea.x + sizea.z * sizea.z) / 12;
						IbodyA[8] = ma * (sizea.x * sizea.x + sizea.y * sizea.y) / 12;

						Vector3 sizeb = Vector3(b->collider->size.x * scaleb.x, b->collider->size.y * scaleb.y, b->collider->size.z * scaleb.z);
						IbodyB[0] = mb * (sizeb.y * sizeb.y + sizeb.z * sizeb.z) / 12;
						IbodyB[4] = mb * (sizeb.x * sizeb.x + sizeb.z * sizeb.z) / 12;
						IbodyB[8] = mb * (sizeb.x * sizeb.x + sizeb.y * sizeb.y) / 12;

						Matrix3 MRA = rota.ToMatrix3();
						Matrix3 MRB = rotb.ToMatrix3();
						//Matrix3 MatR = Matrix3::CreateRotation(a->parent->transform.rotation.ToEuler());
						//debug_print("%f %f %f %f %f %f", Ia[0], Ia[1], Ia[2], Ia[3], Ia[4], Ia[5]);
						//Matrix3 MatR2 = Matrix3::CreateRotation(b->parent->transform.rotation.ToEuler());

						Matrix3 invbA = IbodyA.Inverse();
						Matrix3 invbB = IbodyB.Inverse();
						Matrix3 invA = (MRA * invbA * MRA.Transpose());
						Matrix3 invB = (MRB * invbB * MRB.Transpose());

						PrintMatrix(invA);
						//PIT::Sleep(200000);

						Vector3 Xa = posa;
						Vector3 Xb = posb;

						Vector3 n = mtv.Normalized();

						//colPoint = Vector3(-1, 0.5, -1);

						//DrawLine(active_game, colPoint, colPoint + n, 0xFF0000FF);
						Vector3 ra = /*-rota * */(colPoint - Xa);
						Vector3 rb = /*-rotb * */(colPoint - Xb);

						Vector3 pa = (a->velocity + a->angularVelocity.Cross(ra));
						Vector3 pb = (b->velocity + b->angularVelocity.Cross(rb));
						Vector3 vr = pb - pa;
						float Vrel = vr.Dot(n);

						float N = Vrel * -(1 + e);
						float t1 = 1 / ma;
						float t2 = 1 / mb;
						float t3 = ra.Cross(n).Dot(invA * ra.Cross(n));
						float t4 = rb.Cross(n).Dot(invB * rb.Cross(n));

						//t3 = n.Dot(inva * ra.Cross(n).Cross(ra));
						//t4 = n.Dot(invb * rb.Cross(n).Cross(rb));

						//t3 = n.Dot((inva * ra.Cross(n)).Cross(ra));
						//t4 = n.Dot((invb * rb.Cross(n)).Cross(rb));

						//float t3 = 0;
						//float t4 = n.Dot((inva * ra.Cross(n)).Cross(ra) + (invb * rb.Cross(n)).Cross(rb));

						float J = N / (t1 + t2 + t3 + t4);
						Vector3 force = n * J;

						debug_print("N %f\n", N);
						debug_print("%f\t%f\t%f\t%f\n", t1, t2, t3, t4);
						debug_print("Acc %f\n", force.y / a->mass);
						//PIT::Sleep(100000);
						//PIT::Sleep(100);

						if (force.Dot(n) < 0)
						{
							a->AddImpulse(-force);
							b->AddImpulse(force);

							ra = -rota * (colPoint - Xa);
							rb = -rotb * (colPoint - Xb);

							pa = (a->velocity + a->angularVelocity.Cross(ra));
							pb = (b->velocity + b->angularVelocity.Cross(rb));
							vr = pb - pa;
							Vrel = vr.Dot(n);

							N = Vrel * -(1 + e);
							t1 = 1 / ma;
							t2 = 1 / mb;
							t3 = ra.Cross(n).Dot(invA * ra.Cross(n));
							t4 = rb.Cross(n).Dot(invB * rb.Cross(n));

							J = N / (t1 + t2 + t3 + t4);
							force = n * J;

							Vector3 waf = invA * Vector3::Cross(ra, force);
							Vector3 wbf = invB * Vector3::Cross(rb, force);

							a->angularVelocity -= waf;
							b->angularVelocity += wbf;

							DebugLine(active_game, colPoint, colPoint + waf * sqrt(waf.Magnitude()) * 10, Color::Green);

							//a->angularVelocity.x = 0;
							//a->angularVelocity.y = 0;
							//
							//b->angularVelocity.x = 0;
							//b->angularVelocity.y = 0;

							DebugLine(active_game, colPoint, colPoint + force * sqrt(force.Magnitude()) * 10, Color::Red);
							//PIT::Sleep(1000);


							//DrawLine(active_game, colPoint, colPoint + t * 10, 0xFFFF00FF);

							continue;
							Vector3 ffa = FrictionForce(a, -n, vr, (a->bEnabledGravity ? Vector3(0, -9.8, 0) * a->mass : Vector3()));
							Vector3 ffb = FrictionForce(b, n, vr, (b->bEnabledGravity ? Vector3(0, -9.8, 0) * b->mass : Vector3()));

							DebugLine(active_game, colPoint, colPoint + ffa * 10, Color::Yellow);

							a->AddImpulse(ffa * deltaTime);
							b->AddImpulse(ffb * deltaTime);
							sleep(10);
						}
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
	//Raytracer tracer(active_game, _gc);
	//tracer.Render();
	//return;

	Camera* cam = active_game->GetActiveCamera();
	Matrix4 V = Matrix4::CreateView(
		-cam->transform.GetForwardVector().ToVector4(),
		cam->transform.GetUpVector().ToVector4(),
		cam->transform.GetRightVector().ToVector4(),
		cam->transform.position.ToVector4());

	Vector3 ang = cam->GetWorldRotation() * Vector3(0, 0, 1);
	GL::CameraDirection(ang.ToVector4());

	GL::Clear(0x7F7F7F);
	GL::MatrixMode(GL_VIEW);
	GL::LoadMatrix(V);
	GL::MatrixMode(GL_MODEL);

	for (int i = 0; i < active_game->objects.Count(); i++)
	{
		GameObject* obj = active_game->objects[i];

		for (int j = 0; j < obj->meshComponents.Count(); j++)
		{
			MeshComponent* mesh = obj->meshComponents[j];
			Model3D* model = mesh->model;

			if (!model)
				return;

			GameObject* parent = mesh->parent;
			Transform trans = parent->GetWorldTransform();
			Matrix4 T = Matrix4::CreateTranslation(trans.position.ToVector4());
			Matrix4 R = trans.rotation.ToMatrix();
			Matrix4 S = Matrix4::CreateScale(trans.scale.ToVector4());

			for (int v = 0; v < mesh->model->vertices.Count(); v++)
				model->vertex_buffer[v].worldNormal = R * model->vertex_buffer[v].normal;

			Matrix4 M = T * R * S;

			GL::LoadMatrix(M);

			GL::BindTexture(mesh->texId);
			GL::Draw(model->vertex_buffer, model->vertices.Count());
		}
	}

	GL::SwapBuffers();
}