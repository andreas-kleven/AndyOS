#include "MyBox.h"
#include "GEngine.h"

static char *img_buf = 0;
static IMAGE *image = 0;

MyBox::MyBox()
{
    Model3D *model = ModelLoader::LoadModel("gamedata/cube.a3d", Format3D::FORMAT_A3D);

    if (!image)
        image = IMAGE::Load("/gamedata/img_hd.bmp");

    MeshComponent *mesh = CreateComponent<MeshComponent>("Mesh");
    mesh->SetModel(model);
    mesh->texId = GL::AddTexture(image);

    // SphereCollider* sphere = CreateComponent<SphereCollider>("SphereCollider");
    // sphere->radius = 1;

    Rigidbody *phys = CreateComponent<Rigidbody>("Rigidbody");
    phys->mass = 1e10;
    phys->collider = new BoxCollider();
    // phys->bEnabled = 0;
    phys->bEnabledGravity = 0;
}
