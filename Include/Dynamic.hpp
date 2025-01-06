#pragma once

#include "Game.hpp"

#define IS_STATE_SET(states, state) ((states & (int)state) == (int)state)
#define SET_STATE(states, state) (states |= (int)state)
#define UNSET_STATE(states, state) (states &= ~(int)state)

class Game;

class Dynamic
{
public:
    enum State : int
    {
        Idle = 1 << 0,
        Left = 1 << 1,
        Right = 1 << 2,
        Jump = 1 << 3,
        Faster = 1 << 4
    };

public:
    Dynamic() = default;
    Dynamic(const def::vf2d& pos, const def::vf2d& size);

    void Update();

    void SetPosition(const def::vf2d& pos);
    void OffsetPosition(const def::vf2d& offset);

public:
    virtual void UpdateControls();
    virtual void UpdateCollision(std::list<def::side>& sides);
    virtual void ApplyGravity();

    virtual void SwitchFrame(const float period);

public:
    inline static float s_VelocityEpsilon = 0.0f;
    inline static float s_Friction = 0.0f;
    inline static float s_FallSpeed = 0.0f;
    
    // There exists 2 models:
    // 1) the basic rectangle
    // 2) just 4 lines that represent rectangle without borders
    //      so now it's easier to handle collision

    def::rect<float> model;
    def::line<float> edgelessModel[4];

    def::vf2d velocity;

    def::vi2d graphicsID;

    uint64_t state = State::Left;
    int frameCounter = 0;

    bool isFriendlyFire = false;

private:
    float m_FrameTimer = 0.0f;

};

class Dynamic_Creature : public Dynamic
{
public:
    Dynamic_Creature() = default;
    Dynamic_Creature(const def::vf2d& pos, const def::vf2d& size);

public:
    void UpdateControls() override;
    void UpdateCollision(std::list<def::side>& sides) override;
    void ApplyGravity() override;

};

class Dynamic_Enemy;

class Dynamic_Player : public Dynamic_Creature
{
public:
    Dynamic_Player() = default;
    Dynamic_Player(const def::vf2d& pos);

public:
    void UpdateControls() override;
    void UpdateCollision(std::list<def::side>& sides) override;
    void SwitchFrame(const float period) override;

    bool OnEnemyTouch(Dynamic_Enemy* enemy, const def::side side);

public:
    static float s_GroundSpeed;
    static float s_AirSpeed;
    static float s_JumpSpeed;

    inline static def::vf2d s_MinVelocity;
    inline static def::vf2d s_MaxVelocity;

};

class Dynamic_Enemy : public Dynamic_Creature
{
public:
    Dynamic_Enemy() = default;
    Dynamic_Enemy(const def::vf2d& pos, const def::vf2d& size);

public:
    void UpdateControls() override;
    void UpdateCollision(std::list<def::side>& sides) override;
    void SwitchFrame(const float period) override;

    virtual bool OnHit();
    virtual bool OnSideTouch(const def::side side);

public:
    static float s_GroundSpeed;
    static float s_AirSpeed;
    static float s_JumpSpeed;

    inline static def::vf2d s_MinVelocity;
    inline static def::vf2d s_MaxVelocity;

};

class Dynamic_Enemy_Mushroom : public Dynamic_Enemy
{
public:
    Dynamic_Enemy_Mushroom(const def::vf2d& pos);

public:
    bool OnHit() override;
    bool OnSideTouch(const def::side side) override;

    void SwitchFrame(const float period) override;
};

class Dynamic_Enemy_Turtle : public Dynamic_Enemy
{
public:
    enum class TurtleState
    {
        Walk,
        Shell
    } turtleState = TurtleState::Walk;

    Dynamic_Enemy_Turtle(const def::vf2d& pos);

public:
    bool OnHit() override;
    bool OnSideTouch(def::side side) override;

    void SwitchFrame(const float period) override;
};
