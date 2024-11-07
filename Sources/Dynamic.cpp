#include "../Include/Dynamic.hpp"

Dynamic::Dynamic(const def::vf2d& pos, const def::vf2d& vel) : vPos(pos), vVel(vel)
{
    
}

void Dynamic::UpdateControls()
{
}

void Dynamic::UpdateCollision()
{
    
}

void Dynamic::ApplyGravity()
{
    if (!IS_STATE_SET(nState, State::Jump))
    {
        if (vVel.x == 0.0f && vVel.y == 0.0f)
            SET_STATE(nState, State::Idle);
    }
}

void Dynamic::Update()
{
    SwitchFrame();
    UpdateControls();
    ApplyGravity();
    UpdateCollision();
}

void Dynamic::SwitchFrame(float fPeriod)
{
    if (!IS_STATE_SET(nState, State::Jump) && !IS_STATE_SET(nState, State::Idle))
    {
        if (fFrameTimer >= fPeriod)
        {
            ++nFrameCounter %= 2;
            fFrameTimer -= fPeriod;
        }
    }

    if      (IS_STATE_SET(nState, State::Idle)) vGraphicsID.y = 0;
    else if (IS_STATE_SET(nState, State::Jump)) vGraphicsID.y = 2;
    else                                        vGraphicsID.y = nFrameCounter;

    if      (IS_STATE_SET(nState, State::Right)) vGraphicsID.x = 0;
    else if (IS_STATE_SET(nState, State::Left))  vGraphicsID.x = 1;

    fFrameTimer += s_pEngine->GetDeltaTime();
}

Game* Dynamic::s_pEngine = nullptr;

float Dynamic::s_fVelocityEpsilon = 0.0f;
float Dynamic::s_fFriction = 0.0f;
float Dynamic::s_fGroundSpeed = 0.0f;
float Dynamic::s_fAirSpeed = 0.0f;
float Dynamic::s_fJumpSpeed = 0.0f;
float Dynamic::s_fFallSpeed = 0.0f;

def::vf2d Dynamic::s_vMinVelocity = {};
def::vf2d Dynamic::s_vMaxVelocity = {};




Dynamic_Creature::Dynamic_Creature(const def::vf2d& pos, const def::vf2d& vel)
{
}

void Dynamic_Creature::UpdateControls()
{
}

void Dynamic_Creature::UpdateCollision()
{
    def::vf2d vNewPos = vPos + vVel * s_pEngine->GetDeltaTime();

    Level* pLevel = *s_pEngine->itCurrentLevel;

    if (-s_fVelocityEpsilon < vVel.x && vVel.x < s_fVelocityEpsilon)
        SET_STATE(nState, State::Idle);
    else
        UNSET_STATE(nState, State::Idle);

    if (vVel.x < 0.0f)
    {
        SET_STATE(nState, State::Left);
        UNSET_STATE(nState, State::Right);

        if (pLevel->GetTile(def::vi2d(vNewPos.x, vPos.y)) != TileType::Empty || pLevel->GetTile(def::vi2d(vNewPos.x, vPos.y + 0.9f)) != TileType::Empty)
        {
            vNewPos.x = floor(vNewPos.x) + 1.0f;
            vVel.x = 0.0f;
        }
    }
    else
    {
        SET_STATE(nState, State::Right);
        UNSET_STATE(nState, State::Left);

        if (pLevel->GetTile(def::vi2d(vNewPos.x + 1.0f, vPos.y)) != TileType::Empty || pLevel->GetTile(def::vi2d(vNewPos.x + 1.0f, vPos.y + 0.9f)) != TileType::Empty)
        {
            vNewPos.x = floor(vNewPos.x);
            vVel.x = 0.0f;
        }
    }

    SET_STATE(nState, State::Jump);

    if (vVel.y <= 0.0f)
    {
        if (pLevel->GetTile(vNewPos) != TileType::Empty || pLevel->GetTile(vNewPos + def::vf2d(0.9f, 0.0f)) != TileType::Empty)
        {
            vNewPos.y = floor(vNewPos.y) + 1.0f;
            vVel.y = 0.0f;
        }
    }
    else
    {
        if (pLevel->GetTile(vNewPos + def::vf2d(0.0f, 1.0f)) != TileType::Empty || pLevel->GetTile(vNewPos + def::vf2d(0.9f, 1.0f)) != TileType::Empty)
        {
            vNewPos.y = floor(vNewPos.y);
            vVel.y = 0.0f;
            
            UNSET_STATE(nState, State::Jump);
        }
    }

    vPos = vNewPos;
}

void Dynamic_Creature::ApplyGravity()
{
    float fDeltaTime = s_pEngine->GetDeltaTime();

    vVel.y += s_fFallSpeed * fDeltaTime;

    if (!IS_STATE_SET(nState, State::Jump))
        vVel.x += -s_fFriction * vVel.x * fDeltaTime;

    vVel = vVel.max(s_vMinVelocity).min(s_vMaxVelocity);

    Dynamic::ApplyGravity();
}





Dynamic_Player::Dynamic_Player(const def::vf2d& pos, const def::vf2d& vel)
    : Dynamic_Creature(pos, vel)
{
}

void Dynamic_Player::UpdateControls()
{
    float fDeltaTime = s_pEngine->GetDeltaTime();

    if (s_pEngine->IsFocused())
    {
        if (s_pEngine->GetKey(def::Key::LEFT).held)
            vVel.x += (IS_STATE_SET(nState, State::Jump) ? -17.5f : -15.0f) * fDeltaTime;

        if (s_pEngine->GetKey(def::Key::RIGHT).held)
            vVel.x += (IS_STATE_SET(nState, State::Jump) ? 17.5f : 15.0f) * fDeltaTime;

        if (s_pEngine->GetKey(def::Key::SPACE).pressed)
        {
            if (vVel.y == 0.0f) vVel.y = -12.0f;
            SET_STATE(nState, State::Jump);
            UNSET_STATE(nState, State::Idle);
        }
    }
}

void Dynamic_Player::UpdateCollision()
{
    auto& itLevel = s_pEngine->itCurrentLevel;
    Level* pLevel = *itLevel;

    def::vf2d vNewPos = vPos + vVel * s_pEngine->GetDeltaTime();

    if (pLevel->GetTile(vNewPos) == TileType::Coin)
    {
        pLevel->SetTile(vNewPos, TileType::Empty);
        s_pEngine->nScore++;
    }

    if (pLevel->GetTile(vNewPos + def::vf2d(0.0f, 1.0f)) == TileType::Coin)
    {
        pLevel->SetTile(vNewPos + def::vf2d(0.0f, 1.0f), TileType::Empty);
        s_pEngine->nScore++;
    }

    if (pLevel->GetTile(vNewPos + def::vf2d(1.0f, 0.0f)) == TileType::Coin)
    {
        pLevel->SetTile(vNewPos + def::vf2d(1.0f, 0.0f), TileType::Empty);
        s_pEngine->nScore++;
    }

    if (pLevel->GetTile(vNewPos + 1.0f) == TileType::Coin)
    {
        pLevel->SetTile(vNewPos + 1.0f, TileType::Empty);
        s_pEngine->nScore++;
    }

    Dynamic_Creature::UpdateCollision();

    if (vPos.x < 0.0f)
    {
        if (itLevel == s_pEngine->vecLevels.begin())
        {
            vPos.x = floor(vPos.x) + 1.0f;
            vVel.x = 0.0f;
        }
        else
        {
            vPos.x += float(pLevel->GetSize().x - 1);
            --itLevel;
        }
    }
    else if (vPos.x >= pLevel->GetSize().x - 1.0f)
    {
        if (itLevel == s_pEngine->vecLevels.end() - 1)
        {
            vPos.x = floor(vPos.x);
            vVel.x = 0.0f;
        }
        else
        {
            vPos.x -= float(pLevel->GetSize().x - 1);
            ++itLevel;
        }
    }
}
