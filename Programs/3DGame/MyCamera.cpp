#include "MyCamera.h"
#include "Thing.h"

MyCamera::MyCamera()
{
    this->speed = 10;
    this->yaw = 0;
    this->distance = 10;
    this->target = 0;
}

void MyCamera::Start()
{
    target = GEngine::game->GetObject("Thing");
}

void MyCamera::LateUpdate(float deltaTime)
{
    float camspeed = deltaTime * 0.02;
    float movespeed = deltaTime * this->speed;
    float lerpspeed = 30;

    float mouse_x = Input::GetAxis(AXIS_X) * camspeed;
    float mouse_y = Input::GetAxis(AXIS_Y) * camspeed;

    if (target) {
        Vector3 dir = -target->transform.GetForwardVector();
        dir.y = 0;
        dir.Normalize();

        float pitch = atan2(-dir.x, dir.z);
        yaw += mouse_y;
        dir = Vector3(yaw, pitch, 0).EulerAngles();

        Vector3 tagetpos = target->transform.position + dir * distance;
        transform.position = Math::Lerp(tagetpos, transform.position, deltaTime * lerpspeed);

        transform.rotation =
            Quaternion::LookAt(transform.position, target->transform.position, Vector3::up);

    } else {
        RotateEuler(Vector3(mouse_y, mouse_x, 0));

        if (Input::GetKey(KEY_LSHIFT))
            movespeed /= 4;

        if (Input::GetKey(KEY_D))
            transform.Translate(transform.GetRightVector() * movespeed);
        if (Input::GetKey(KEY_A))
            transform.Translate(-transform.GetRightVector() * movespeed);
        if (Input::GetKey(KEY_W))
            transform.Translate(transform.GetForwardVector() * movespeed);
        if (Input::GetKey(KEY_S))
            transform.Translate(-transform.GetForwardVector() * movespeed);
        if (Input::GetKey(KEY_E))
            transform.Translate(transform.GetUpVector() * movespeed);
        if (Input::GetKey(KEY_Q))
            transform.Translate(-transform.GetUpVector() * movespeed);
    }
}
