#include "Raymarcher.h"
#include "GEngine.h"
#include "GL.h"

float DistanceEstimator(Vector3 pos)
{
    const float Bailout = 2;
    const float Power = 8;
    const int Iterations = 1024;

    Vector3 z = pos;
	float dr = 1.0;
	float r = 0.0;

    int i;
	for (i = 0; i < Iterations ; i++)
    {
		r = z.Magnitude();
		if (r > Bailout) break;
		
		// convert to polar coordinates
		float theta = acos(z.z / r);
		float phi = atan2(z.y, z.x);
		dr =  pow(r, Power-1.0)*Power*dr + 1.0;
		
		// scale and rotate the point
		float zr = pow(r ,Power);
		theta *= Power;
		phi *= Power;
		
		// convert back to cartesian coordinates
		z = Vector3(sin(theta)*cos(phi), sin(phi)*sin(theta), cos(theta)) * zr;
		z += pos;
	}

	return 0.5 * log(r) * r / dr;
}

float Trace(Vector3 start, Vector3 dir, Vector3& end)
{
    const float MinimumDistance = 0.001;
    const float MaximumDistance = 100;
    const int MaximumRaySteps = 128;

    float totalDistance = 0.0;
	int steps;

	for (steps = 0; steps < MaximumRaySteps; steps++)
    {
		end = start + dir * totalDistance;
		float distance = DistanceEstimator(end);
		totalDistance += distance;

        if (distance >= MaximumDistance) return 0;
		if (distance < MinimumDistance) break;
	}

	return 1.0 - float(steps) / float(MaximumRaySteps);
}

float TraceColor(Vector3 start, Vector3 dir)
{
    Vector3 p0;
    Vector3 p1;
    Vector3 p2;

    float offset = 0.001;

    if (Trace(start, dir, p0) == 0) return 0;
    if (Trace(start, Vector3(dir.x, dir.y + offset, dir.z).Normalized(), p1) == 0) return 0;
    if (Trace(start, Vector3(dir.x + offset, dir.y, dir.z).Normalized(), p2) == 0) return 0;

    Vector3 n = (p1 - p2).Cross(p2 - p0).Normalized();
    //printf("[%f, %f, %f]\t[%f, %f, %f]\n", p0.x, p0.y, p0.z, p1.x, p1.y, p1.z);

    float diff = n.Dot(-Vector3(0, 0, 1).Normalized());
    return clamp(diff + 0.2f, 0.0f, 1.0f);
}

Raymarcher::Raymarcher()
{
    max_res = 32;
    res = 32;
}

void Raymarcher::Render(GC& gc)
{
    if (Input::GetKey(KEY_UP)) max_res = min(128, max_res * 2);
    if (Input::GetKey(KEY_DOWN)) max_res = max(2, max_res / 2);

    float mx = Input::GetAxis(AXIS_X);
    float my = Input::GetAxis(AXIS_Y);

    if (res > 1)
        res /= 2;

    Camera* cam = GEngine::game->GetActiveCamera();
    Vector3 _pos = cam->GetWorldPosition() + Vector3(0, 0, 5);
    cam->speed = _pos.Magnitude() / 10;

    float scale = tan(45 * M_PI / 180); 
    float imageAspectRatio = gc.width / (float)gc.height; 

    for (int y = 0; y < gc.height; y += res)
    {
        if (res < max_res / 2 &&
            (Input::GetKey(KEY_SPACE)
            || Input::GetKey(KEY_W)
            || Input::GetKey(KEY_A)
            || Input::GetKey(KEY_S)
            || Input::GetKey(KEY_D)
            || Input::GetAxis(AXIS_X) != mx
            || Input::GetAxis(AXIS_Y) != my
        ))
        {
            res = max_res;
            return;
        }

        for (int x = 0; x < gc.width; x += res)
        {
            float cx = (2 * (x + 0.5) / (float)gc.width - 1) * imageAspectRatio * scale; 
            float cy = (1 - 2 * (y + 0.5) / (float)gc.height) * scale; 

            //_pos = Vector3(0, 0, -3);
            Vector3 dir = Vector3(cx, cy, 1).Normalized();
            dir = cam->GetWorldRotation() * dir;

            float dist = TraceColor(_pos, dir);
            Color col = Color(1, 1, 1) * dist;
            
            //gc.SetPixel(x, y, col);
            gc.FillRect(x, y, res, res, col);
        }
    }
}