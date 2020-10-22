#include "Thing.h"
#include "GEngine.h"
#include "MyCamera.h"

Thing::Thing()
{
    mesh = 0;
    SetMesh(0);

    char *img_buf;

    if (read_file(img_buf, "/gamedata/fox.bmp")) {
        BMP *bmp = new BMP(img_buf);
        mesh->texId = GL::AddTexture(bmp);
    } else {
        printf("image not found\n");
        exit(1);
    }

    /*for (int i = 0; i < mesh->model->vertices.size(); i++) {
        mesh->model->vertex_buffer[i].color = Color::Red;
    }*/

    SphereCollider *sphere = new SphereCollider();
    sphere->radius = 1;
    CreateComponent<Rigidbody>("Rigidbody");
    rigidbody->collider = sphere;
    // rigidbody->collider = new BoxCollider();
    // rigidbody->bEnabled = 0;
    // rigidbody->bEnabledGravity = 0;
}

void Thing::SetMesh(int index)
{
    mesh_index = index % 6;
    const char *path;

    if (mesh_index == 0)
        path = "/gamedata/monkey.a3d";
    else if (mesh_index == 1)
        path = "/gamedata/sphere.a3d";
    else if (mesh_index == 2)
        path = "/gamedata/bunny.a3d";
    if (mesh_index == 3)
        path = "/gamedata/fox.a3d";
    if (mesh_index == 4)
        path = "/gamedata/tank.a3d";
    if (mesh_index == 5)
        path = "/gamedata/aventado.a3d";

    Model3D *model = ModelLoader::LoadModel(path, Format3D::FORMAT_A3D);

    if (!mesh)
        mesh = CreateComponent<MeshComponent>("Mesh");

    mesh->SetModel(model);

    for (int i = 0; i < mesh->model->vertices.size(); i++) {
        Vertex &vert = mesh->model->vertex_buffer[i];

        if (mesh_index == 3) {
            vert.pos = vert.pos / 30.f;
        }

        if (mesh_index == 4) {
            vert.color = Color::Gray;
            mesh->texId = 0;
        }

        if (mesh_index == 5) {
            vert.color = Color(0xbf870d);
            mesh->texId = 0;
        }
    }
}

void Thing::Start()
{
    if (!PlayerManager::IsLocal())
        return;

    MyCamera *cam = (MyCamera *)GEngine::game->GetObject("Camera");

    if (cam)
        cam->target = this;
}

void Thing::Update(float deltaTime)
{
    if (Input::GetKeyDown(KEY_M))
        SetMesh(mesh_index + 1);

    float sign = Input::GetKey(KEY_LSHIFT) ? -1 : 1;
    float jumpspeed = 5;
    float movespeed = deltaTime * 10;
    float velspeed = deltaTime * 10;
    float rotpeed = deltaTime * sign * 5;
    float camspeed = deltaTime * 0.03;

    float mouse_x = Input::GetAxis(AXIS_X);

    if (fabs(mouse_x) > 0)
        transform.rotation *= Quaternion::FromAxisAngle(Vector3::up, mouse_x * camspeed);

    if (Input::GetKey(KEY_LCTRL)) {
        movespeed /= 4;
        rotpeed /= 4;
    }

    if (Input::GetKey(KEY_D))
        transform.Translate(transform.GetRightVector() * movespeed);
    if (Input::GetKey(KEY_A))
        transform.Translate(-transform.GetRightVector() * movespeed);
    if (Input::GetKey(KEY_W))
        transform.Translate(transform.GetForwardVector() * movespeed);
    if (Input::GetKey(KEY_S))
        transform.Translate(-transform.GetForwardVector() * movespeed);

    if (Input::GetKeyDown(KEY_SPACE))
        rigidbody->AddImpulse(Vector3::up * jumpspeed);

    if (Input::GetKey(KEY_L)) {
        rigidbody->AddImpulse(Vector3::right * velspeed);
    }

    if (Input::GetKey(KEY_J)) {
        rigidbody->AddImpulse(-Vector3::right * velspeed);
    }

    if (Input::GetKey(KEY_I)) {
        rigidbody->AddImpulse(Vector3::forward * velspeed);
    }

    if (Input::GetKey(KEY_K)) {
        rigidbody->AddImpulse(-Vector3::forward * velspeed);
    }

    if (Input::GetKey(KEY_O)) {
        rigidbody->AddImpulse(Vector3::up * velspeed);
    }

    if (Input::GetKey(KEY_U)) {
        rigidbody->AddImpulse(-Vector3::up * velspeed);
    }

    if (Input::GetKey(KEY_D1)) {
        transform.rotation.Rotate(Vector3::right, rotpeed);
    }

    if (Input::GetKey(KEY_D2)) {
        transform.rotation.Rotate(Vector3::up, rotpeed);
    }

    if (Input::GetKey(KEY_D3)) {
        transform.rotation.Rotate(Vector3::forward, rotpeed);
    }
}
