#include "GL.h"
#include "Rasterizer.h"
#include <AndyOS.h>
#include <andyos/math.h>
#include <math.h>
#include <string.h>

namespace GL {
GC gc_buf;
GC gc_out;

uint32_t m_width;
uint32_t m_height;
uint32_t m_stride;

Rasterizer rasterizer;

BMP **m_textures;
int tex_index;
int bound_tex;

Vector4 lightsources[GL_MAX_LIGHTSOURCES];
int light_index;

GLMatrixMode mat_mode = GL_PROJECTION;
Matrix4 mat_model;
Matrix4 mat_view;
Matrix4 mat_projection;

Matrix4 *mat_stack;
int mat_stack_index;
Vector4 cam_dir;

Matrix4 &SelectedMatrix()
{
    switch (mat_mode) {
    case GL_MODEL:
        return mat_model;

    case GL_VIEW:
        return mat_view;

    case GL_PROJECTION:
        return mat_projection;

    default:
        // Todo: crash
        return mat_model;
    }
}

int AddTexture(BMP *bmp)
{
    if (tex_index == GL_MAX_TEXTURES)
        return 0;

    m_textures[tex_index] = bmp;
    return tex_index++;
}

void BindTexture(int id)
{
    bound_tex = id;
}

int LightSource(Vector4 light)
{
    light_index = 0;

    if (light_index == GL_MAX_LIGHTSOURCES)
        return -1;

    float strength = light.w;
    Vector4 t = SelectedMatrix() * light;
    t.w = strength;

    lightsources[light_index] = t;
    return light_index++;
}

void MatrixMode(GLMatrixMode mode)
{
    mat_mode = mode;
}

void MulMatrix(const Matrix4 &mat)
{
    Matrix4 &m = SelectedMatrix();
    m = m * mat;
}

void LoadIdentity()
{
    SelectedMatrix() = Matrix4();
}

void LoadMatrix(const Matrix4 &mat)
{
    SelectedMatrix() = mat;
}

void PushMatrix()
{
    mat_stack[mat_stack_index++] = SelectedMatrix();
}

void PopMatrix()
{
    SelectedMatrix() = mat_stack[--mat_stack_index];
}

void CameraDirection(Vector4 dir)
{
    cam_dir = dir;
}

Vector4 Reflect(Vector4 d, Vector4 n)
{
    return d - n * d.Dot(n) * 2;
}

void Draw(Vertex *verts, int count)
{
    Vector4 light = Vector4(0.3, -1, 0.5, 0).Normalized();
    Vector4 light2 = Vector4(-1, 0.8, -1, 0).Normalized();

    float diffuse = 1;
    float specular = 1;

    Matrix4 M = mat_projection * mat_view * mat_model;
    BMP *texture = m_textures[bound_tex];

    for (int i = 0; i < count; i += 3) {
        Vertex &a = verts[i];
        Vertex &b = verts[i + 1];
        Vertex &c = verts[i + 2];

        a.MulMatrix(mat_model);
        b.MulMatrix(mat_model);
        c.MulMatrix(mat_model);

        // Specular
        float specA = clamp(cam_dir.Dot(-Reflect(light, a.worldNormal)), 0.f, 1.f);
        float specB = clamp(cam_dir.Dot(-Reflect(light, b.worldNormal)), 0.f, 1.f);
        float specC = clamp(cam_dir.Dot(-Reflect(light, c.worldNormal)), 0.f, 1.f);

        // Diffuse
        float diffA = clamp(-a.worldNormal.Dot(light), 0.f, 1.f);
        float diffB = clamp(-b.worldNormal.Dot(light), 0.f, 1.f);
        float diffC = clamp(-c.worldNormal.Dot(light), 0.f, 1.f);

        a.builtColor = a.color * (0.1 + diffuse * diffA + specular * pow(specA, 5));
        b.builtColor = b.color * (0.1 + diffuse * diffB + specular * pow(specB, 5));
        c.builtColor = c.color * (0.1 + diffuse * diffC + specular * pow(specC, 5));

        a.MulMatrix(M);
        b.MulMatrix(M);
        c.MulMatrix(M);

        if (a.tmpPos.w > 0 && b.tmpPos.w > 0 && c.tmpPos.w > 0) {
            rasterizer.DrawTriangle(a, b, c, texture);
        }
    }
}

void Clear(Color color)
{
    gc_buf.Clear(color);
    rasterizer.Clear();
}

void SwapBuffers()
{
    gc_buf.CopyTo(0, 0, gc_buf.width, gc_buf.height, gc_out, 0, 0);
}

void InitGraphics(const GC &gc)
{
    gc_buf = GC(gc.width, gc.height);
    gc_out = gc;

    m_width = gc_buf.width;
    m_height = gc_buf.height;
    m_stride = gc_buf.stride;
    rasterizer = Rasterizer(gc_buf);
}

void Init()
{
    mat_stack = new Matrix4[GL_MATRIX_STACK_LENGTH];

    m_textures = new BMP *[GL_MAX_TEXTURES];
    memset(m_textures, 0, sizeof(BMP *) * (GL_MAX_TEXTURES));

    tex_index = 1;
    bound_tex = 0;
    light_index = 0;
    mat_stack_index = 0;
}
} // namespace GL
