#include "GL.h"
#include "Rasterizer.h"
#include <AndyOS.h>
#include <andyos/math.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

namespace GL {
GC gc_buf;
GC gc_out;

uint32_t m_width;
uint32_t m_height;
uint32_t m_stride;

Rasterizer rasterizer;

IMAGE **m_textures;
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

int AddTexture(IMAGE *img)
{
    if (tex_index == GL_MAX_TEXTURES)
        return 0;

    m_textures[tex_index] = img;
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

int VertexShift(float da, float db, float dc)
{
    // A -> 0
    // B -> 1
    // C -> 2
    // A, B -> 0
    // B, C -> 1
    // C, A -> 2

    if (da < 0) {
        if (dc < 0)
            return 2;
        else
            return 0;
    }

    if (db < 0) {
        if (da < 0)
            return 0;
        else
            return 1;
    }

    if (dc < 0) {
        if (db < 0)
            return 1;
        else
            return 2;
    }

    return 0;
}

void LerpVertex(const Vertex *a, Vertex *b, float clip_dist)
{
    // https://cubic.org/docs/3dclip.htm
    float s = (b->tmp_pos.z + clip_dist) / (b->tmp_pos.z - a->tmp_pos.z);
    b->tmp_pos = b->tmp_pos + (a->tmp_pos - b->tmp_pos) * s;
    b->tmp_u = b->tmp_u + s * (a->tmp_u - b->tmp_u);
    b->tmp_v = b->tmp_v + s * (a->tmp_v - b->tmp_v);
}

void DrawTriangle(const Vertex &a, const Vertex &b, const Vertex &c, IMAGE *texture)
{
    // Backface culling
    Vector4 pa(a.tmp_pos.x, a.tmp_pos.y, a.tmp_pos.z, 0);
    Vector4 pb(b.tmp_pos.x, b.tmp_pos.y, b.tmp_pos.z, 0);
    Vector4 pc(c.tmp_pos.x, c.tmp_pos.y, c.tmp_pos.z, 0);
    Vector4 normal = Vector4::Cross(pb - pa, pc - pa);

    if (normal.z < 0)
        return;

    if (a.tmp_pos.w > 0 && b.tmp_pos.w > 0 && c.tmp_pos.w > 0) {
        rasterizer.DrawTriangle(a, b, c, texture);
    } else {
        kprintf("Draw warning %.2f %.2f %.2f\n", a.tmp_pos.w, b.tmp_pos.w, c.tmp_pos.w);
    }
}

void Draw(Vertex *verts, int count)
{
    Vector4 light = Vector4(0.3, -1, 0.5, 0).Normalized();
    Vector4 light2 = Vector4(-1, 0.8, -1, 0).Normalized();

    float clip_dist = 1;
    float diffuse = 1;
    float specular = 1;

    Matrix4 view_model = mat_view * mat_model;
    IMAGE *texture = m_textures[bound_tex];

    for (int i = 0; i < count; i += 3) {
        Vertex &a = verts[i];
        Vertex &b = verts[i + 1];
        Vertex &c = verts[i + 2];
        Vertex d;

        a.MulMatrix(view_model, true);
        b.MulMatrix(view_model, true);
        c.MulMatrix(view_model, true);

        Vertex *sorted[4];
        float distA = -a.tmp_pos.z - clip_dist;
        float distB = -b.tmp_pos.z - clip_dist;
        float distC = -c.tmp_pos.z - clip_dist;

        int shift = VertexShift(distA, distB, distC);
        sorted[(3 - shift) % 3] = &a;
        sorted[(4 - shift) % 3] = &b;
        sorted[(5 - shift) % 3] = &c;

        int behind = 0;

        for (int i = 0; i < 3; i++) {
            if (-sorted[i]->tmp_pos.z - clip_dist < 0)
                behind += 1;
            else
                break;
        }

        if (behind == 3)
            continue;

        // Specular
        float specA = clamp(cam_dir.Dot(-Reflect(light, a.world_normal)), 0.f, 1.f);
        float specB = clamp(cam_dir.Dot(-Reflect(light, b.world_normal)), 0.f, 1.f);
        float specC = clamp(cam_dir.Dot(-Reflect(light, c.world_normal)), 0.f, 1.f);

        // Diffuse
        float diffA = clamp(-a.world_normal.Dot(light), 0.f, 1.f);
        float diffB = clamp(-b.world_normal.Dot(light), 0.f, 1.f);
        float diffC = clamp(-c.world_normal.Dot(light), 0.f, 1.f);

        a.built_color = a.color * (0.3 + diffuse * diffA + specular * pow(specA, 5));
        b.built_color = b.color * (0.3 + diffuse * diffB + specular * pow(specB, 5));
        c.built_color = c.color * (0.3 + diffuse * diffC + specular * pow(specC, 5));

        // UVs
        a.tmp_u = a.u;
        a.tmp_v = a.v;
        b.tmp_u = b.u;
        b.tmp_v = b.v;
        c.tmp_u = c.u;
        c.tmp_v = c.v;

        if (behind == 2) {
            LerpVertex(sorted[2], sorted[0], clip_dist);
            LerpVertex(sorted[2], sorted[1], clip_dist);
        } else if (behind == 1) {
            d = *sorted[0];
            sorted[3] = &d;
            LerpVertex(sorted[1], sorted[3], clip_dist);
            LerpVertex(sorted[2], sorted[0], clip_dist);
        }

        int vertex_count = (behind == 1) ? 4 : 3;

        for (int i = 0; i < vertex_count; i++) {
            Vertex *v = sorted[i];
            v->MulMatrix(mat_projection, false);
            v->tmp_pos.w = 1 / v->tmp_pos.w;
            v->tmp_pos.x = v->tmp_pos.x * gc_buf.width * v->tmp_pos.w + gc_buf.width * 0.5;
            v->tmp_pos.y = v->tmp_pos.y * gc_buf.height * v->tmp_pos.w + gc_buf.height * 0.5;
        }

        DrawTriangle(*sorted[0], *sorted[1], *sorted[2], texture);

        if (behind == 1)
            DrawTriangle(*sorted[3], *sorted[1], *sorted[0], texture);
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

GC &GetGC()
{
    return gc_buf;
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

    m_textures = new IMAGE *[GL_MAX_TEXTURES];
    memset(m_textures, 0, sizeof(IMAGE *) * (GL_MAX_TEXTURES));

    tex_index = 1;
    bound_tex = 0;
    light_index = 0;
    mat_stack_index = 0;
}
} // namespace GL
