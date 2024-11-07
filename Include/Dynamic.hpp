#pragma once

#include "../Include/defGameEngine.hpp"
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
    Dynamic(const def::vf2d& pos, const def::vf2d& vel);

    virtual void UpdateControls();
    virtual void UpdateCollision();
    virtual void ApplyGravity();

    void Update();

    void SwitchFrame(float fPeriod = 0.2f);

public:
    static float s_fVelocityEpsilon;
    static float s_fFriction;
    static float s_fGroundSpeed;
    static float s_fAirSpeed;
    static float s_fJumpSpeed;
    static float s_fFallSpeed;

    static def::vf2d s_vMinVelocity;
    static def::vf2d s_vMaxVelocity;
    
public:
    def::vf2d vPos;
    def::vf2d vVel;

    def::vi2d vGraphicsID;

    float fFrameTimer = 0.0f;
    int nFrameCounter = 0;

    int nState = State::Left;

    static Game* s_pEngine;
};

class Dynamic_Creature : public Dynamic
{
public:
    Dynamic_Creature() = default;
    Dynamic_Creature(const def::vf2d& pos, const def::vf2d& vel);

public:
    void UpdateControls() override;
    void UpdateCollision() override;
    void ApplyGravity() override;

};

class Dynamic_Player : public Dynamic_Creature
{
public:
    Dynamic_Player() = default;
    Dynamic_Player(const def::vf2d& pos, const def::vf2d& vel);

public:
    void UpdateControls() override;
    void UpdateCollision() override;

};
