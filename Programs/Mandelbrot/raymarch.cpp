#include <math.h>
#include <GL/Vector2.h>
#include <GL/Vector3.h>
#include <GL/Quaternion.h>
#include <andyos/drawing.h>
#include <andyos/math.h>
#include <AndyOS.h>
#include "GUI.h"

using namespace gui;

const double min_distance = 0.001;
const double max_distance = 200;
const int max_steps = 250;
const double epsilon = 0.001f;

double power = 7;
int iterations = 16;

Vector3 cam_pos = Vector3(0, 0, -3);
Quaternion cam_rot;
Vector3 cam_euler;
Vector3 light_dir;

Vector2 distance_estimator(const Vector3 &pos)
{
    //return Vector2(1, pos.Magnitude() - 1);

    Vector3 z = pos;
    double dr = 1.0;
    double r = 0.0;

    int i;
    for (i = 0; i < iterations; i++)
    {
        r = z.Magnitude();
        if (r > 2)
            break;

        // convert to polar coordinates
        double theta = acos(z.z / r);
        double phi = atan2(z.y, z.x);
        dr = pow(r, power - 1.0) * power * dr + 1.0;

        // scale and rotate the point
        double zr = pow(r, power);
        theta *= power;
        phi *= power;

        // convert back to cartesian coordinates
        z = Vector3(sin(theta) * cos(phi), sin(phi) * sin(theta), cos(theta)) * zr;
        z += pos;
    }

    double x = double(i) / double(iterations);
    double y = 0.5 * log(r) * r / dr;
    return Vector2(x, y);
}

Vector2 trace(const Vector3 &start, const Vector3 &dir, Vector3 &end)
{
    double total = 0.0;
    int steps;

    for (steps = 0; steps < max_steps; steps++)
    {
        end = start + dir * total;
        Vector2 result = distance_estimator(end);
        double distance = result.y;
        total += distance;

        if (distance >= max_distance)
            return Vector2(0, double(steps) / double(max_steps));
        if (distance < min_distance)
            return Vector2(result.x, double(steps) / double(max_steps));
    }

    return Vector2(0, 1);
}

Vector3 estimate_normal(const Vector3 &p)
{
    double x = distance_estimator(Vector3(p.x + epsilon, p.y, p.z)).y - distance_estimator(Vector3(p.x - epsilon, p.y, p.z)).y;
    double y = distance_estimator(Vector3(p.x, p.y + epsilon, p.z)).y - distance_estimator(Vector3(p.x, p.y - epsilon, p.z)).y;
    double z = distance_estimator(Vector3(p.x, p.y, p.z + epsilon)).y - distance_estimator(Vector3(p.x, p.y, p.z - epsilon)).y;
    return Vector3(x, y, z).Normalized();
}

Color trace_color(Vector3 start, Vector3 dir)
{
    Vector3 end;
    Vector2 result = trace(start, dir, end);
    double iter = result.x;
    double steps = result.y;

    Vector3 normal = estimate_normal(end);
    double intensity = clamp(normal.Dot(light_dir), 0.0f, 1.0f);

    if (iter == 1)
        iter = 0;

    Color diffuse_color = Color(30.f / 256, 220.f / 255, 40.f / 256);
    Color iter_color = Color(60.f / 256, 80.f / 256, 220.f / 255);
    Color steps_color = iter_color;

    double ad = 1;
    double ai = 0.25;
    double as = 3;

    return diffuse_color * intensity * ad + iter_color * iter * ai + steps_color * steps * as;
}

Color raymarch_render(const GC &gc, int sx, int sy)
{
    const double fov = 53;
    const double aspect_ratio = gc.width / (double)gc.height;
    const double scale = tan(fov * M_PI / 180.0 / 2.0);

    double cx = (2 * (sx + 0.5) / (double)gc.width - 1) * aspect_ratio * scale;
    double cy = (1 - 2 * (sy + 0.5) / (double)gc.height) * scale;

    Vector3 pos = cam_pos;
    Vector3 dir = cam_rot * Vector3(cx, cy, 1).Normalized();

    return trace_color(pos, dir);
}

void raymarch_update(double delta)
{
    double distance = cam_pos.Magnitude();
    double rot_multiplier = delta * 0.5;
    double mov_multiplier = delta * pow(distance, 2) * 0.1;

    if (InputManager::GetKeyDown(KEY_LSHIFT))
    {
        rot_multiplier *= 4;
        mov_multiplier *= 4;
    }

    if (InputManager::GetKeyDown(KEY_LEFT))
        cam_euler.y -= rot_multiplier;
    if (InputManager::GetKeyDown(KEY_RIGHT))
        cam_euler.y += rot_multiplier;
    if (InputManager::GetKeyDown(KEY_UP))
        cam_euler.x -= rot_multiplier;
    if (InputManager::GetKeyDown(KEY_DOWN))
        cam_euler.x += rot_multiplier;

    cam_rot = Quaternion::FromEuler(cam_euler);

    if (InputManager::GetKeyDown(KEY_W))
        cam_pos += cam_rot * Vector3::forward * mov_multiplier;
    if (InputManager::GetKeyDown(KEY_S))
        cam_pos -= cam_rot * Vector3::forward * mov_multiplier;
    if (InputManager::GetKeyDown(KEY_Q))
        cam_pos -= cam_rot * Vector3::up* mov_multiplier;
    if (InputManager::GetKeyDown(KEY_E))
        cam_pos += cam_rot * Vector3::up * mov_multiplier;
    if (InputManager::GetKeyDown(KEY_A))
        cam_pos -= cam_rot * Vector3::right * mov_multiplier;
    if (InputManager::GetKeyDown(KEY_D))
        cam_pos += cam_rot * Vector3::right * mov_multiplier;
}

void raymarch_init()
{
    //cam_pos = Vector3(0, 0, -3);
    light_dir = -Vector3(0, 0, 1).Normalized();
}
