#pragma once

#include "defGeometry2D.hpp"

#include <list>

float GetRandomFloat(float min, float max);

class Snow
{
public:
    struct Flake
    {
        def::vf2d pos;

        float time = 0.0f;
        float speed = 1.0f;
    };

    virtual ~Snow();

    Snow(Snow const&) = delete;
    void operator=(Snow const&) = delete;

    static Snow& Get();

    void Update(float deltaTime);
    void Draw();

public:
    static int s_FlakeRadius;
    static float s_Speed;

private:
    Snow();

private:
    std::list<Flake> m_Flakes;
    
};
