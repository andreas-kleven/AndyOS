#pragma once

struct Shader
{
    float diffuse;
    float specular;
    float ior;

    Shader()
    {
        this->diffuse = 1;
        this->specular = 0.08;
        this->ior = 0;
    }

    Shader(float diffuse, float specular, float ior)
    {
        this->diffuse = diffuse;
        this->specular = specular;
        this->ior = ior;
    }
};
