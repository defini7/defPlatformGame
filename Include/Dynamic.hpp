#pragma once

#include "../Include/Level.hpp"
#include "../Include/Game.hpp"

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
        Jump = 1 << 3
    };

public:
    Dynamic() = default;
    Dynamic(const def::vf2d& pos);

    void Update();

public:
    virtual void UpdateControls();
    virtual void UpdateCollision();
    virtual void ApplyGravity();

    virtual void SwitchFrame(float fPeriod = 0.2f);

public:
    inline static float s_fVelocityEpsilon = 0.0f;
    inline static float s_fFriction = 0.0f;
    inline static float s_fFallSpeed = 0.0f;

    inline static def::vf2d s_vMinVelocity;
    inline static def::vf2d s_vMaxVelocity;
    
public:
    def::rect<float> rModel;
    def::vf2d vVel;

    def::vi2d vGraphicsID;

    float fFrameTimer = 0.0f;
    int nFrameCounter = 0;

    int nState = State::Left;
    bool bRedundant = false;

    static Game* s_pEngine;
};

class Dynamic_Creature : public Dynamic
{
public:
    Dynamic_Creature() = default;
    Dynamic_Creature(const def::vf2d& pos);

public:
    void UpdateControls() override;
    void UpdateCollision() override;
    void ApplyGravity() override;

    virtual void Die() = 0;

};

class Dynamic_Enemy;

class Dynamic_Player : public Dynamic_Creature
{
public:
    Dynamic_Player() = default;
    Dynamic_Player(const def::vf2d& pos);

public:
    void UpdateControls() override;
    void UpdateCollision() override;
    void SwitchFrame(float fPeriod = 0.2f) override;

    void OnEnemyTouch(Dynamic_Enemy* pEnemy, def::side side);

    void Die() override;

public:
    static float s_fGroundSpeed;
    static float s_fAirSpeed;
    static float s_fJumpSpeed;

};

class Dynamic_Enemy : public Dynamic_Creature
{
public:
    Dynamic_Enemy() = default;
    Dynamic_Enemy(const def::vf2d& pos);

public:
    void UpdateControls() override;
    void UpdateCollision() override;
    void SwitchFrame(float fPeriod = 0.2f) override;

    void Die() override;

public:
    static float s_fGroundSpeed;
    static float s_fAirSpeed;
    static float s_fJumpSpeed;

};
