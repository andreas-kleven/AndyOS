#include "Raymarcher.h"
#include "GEngine.h"
#include "GL.h"

float DistanceEstimator(Vector3 pos)
{
    const float Bailout = 2;
    const float Power = 8;
    const int Iterations = 256;

    Vector3 z = pos;
	float dr = 1.0;
	float r = 0.0;

    int i;
	for (i = 0; i < Iterations ; i++) {
		r = z.Magnitude();
		if (r>Bailout) break;
		
		// convert to polar coordinates
		float theta = acos(z.z / r);
		float phi = atan2(z.y, z.x);
		dr =  pow(r, Power-1.0)*Power*dr + 1.0;
		
		// scale and rotate the point
		float zr = pow( r,Power);
		theta = theta*Power;
		phi = phi*Power;
		
		// convert back to cartesian coordinates
		z = Vector3(sin(theta)*cos(phi), sin(phi)*sin(theta), cos(theta)) * zr;
		z+=pos;
	}

	return 0.5*log(r)*r/dr;
}

float Trace(Vector3 start, Vector3 dir)
{
    const float MinimumDistance = 0.0005;
    const float MaximumDistance = 10;
    const int MaximumRaySteps = 100;

    float totalDistance = 0.0;
	int steps;

	for (steps = 0; steps < MaximumRaySteps; steps++)
    {
		Vector3 pos = start + dir * totalDistance;
		float distance = DistanceEstimator(pos);
		totalDistance += distance;

        if (distance >= MaximumDistance) return 0;
		if (distance < MinimumDistance) break;
	}

	return 1.0 - float(steps) / float(MaximumRaySteps);
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
    Vector3 _pos = cam->GetWorldPosition();
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

            float dist = Trace(_pos, dir);
            Color col = Color(pow(dist, 5), pow(dist, 3), dist);
            
            //gc.SetPixel(x, y, col);
            gc.FillRect(x, y, res, res, col);
        }
    }
}