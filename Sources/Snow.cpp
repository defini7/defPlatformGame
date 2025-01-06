#include "../Include/Snow.hpp"
#include "../Include/Dynamic.hpp"

// Here we just emulate the snow.
// Everything has been stolen from
// https://github.com/defini7/defGameEngine/blob/master/Examples/Snow.cpp

float GetRandomFloat(float min, float max)
{
    return (float)rand() / (float)RAND_MAX * (max - min) + min;
}

int Snow::s_FlakeRadius = 0;
float Snow::s_Speed = 0.0f;

Snow::Snow()
{
    srand(time(nullptr));

    m_Flakes.resize(256);

    for (auto& flake : m_Flakes)
    {
        flake.pos = {
            GetRandomFloat(0.0f, Game::Get().ScreenWidth()), float(-s_FlakeRadius)
        };
        flake.speed = GetRandomFloat(100.0f, 200.0f);
    }
}

Snow::~Snow()
{
}

Snow& Snow::Get()
{
    static Snow snow;
    return snow;
}

void Snow::Update(float deltaTime)
{
    for (auto& flake : m_Flakes)
    {
        flake.time += deltaTime;
        flake.pos.y = flake.time * (flake.speed + s_Speed);

        if (flake.pos.y >= Game::Get().ScreenHeight() + s_FlakeRadius)
        {
            flake.pos = {
                GetRandomFloat(-s_Speed / 10.0f, Game::Get().ScreenWidth()),
                float(-s_FlakeRadius)
            };

            flake.time = 0.0f;
        }

        flake.pos.x += deltaTime * s_Speed / 10.0f;
    }
}

void Snow::Draw()
{
    for (auto& flake : m_Flakes)
    {
        float col = abs(1.0f - 1.0f / flake.speed * 200.0f);
        uint8_t pixel = uint8_t(col * 255.0f);

        int width = (float)s_FlakeRadius * col;

        Game::Get().FillTextureRectangle(
            flake.pos + def::vf2d(flake.pos.x + cos(flake.time * flake.speed * 0.2f), 0.0f),
            { width, width }, def::Pixel(pixel, pixel, pixel)
        );
    }
}
