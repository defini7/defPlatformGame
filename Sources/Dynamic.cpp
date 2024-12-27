#define DEF_GEOMETRY2D_IMPL
#include "../Include/Dynamic.hpp"

Dynamic::Dynamic(const def::vf2d& pos) : rModel(pos, { 1, 1 })
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

    fFrameTimer += s_pEngine->GetDeltaTime();
}

Game* Dynamic::s_pEngine = nullptr;




Dynamic_Creature::Dynamic_Creature(const def::vf2d& pos)
    : Dynamic(pos)
{
}

void Dynamic_Creature::UpdateControls()
{
}

void Dynamic_Creature::UpdateCollision()
{
    def::vf2d vNewPos = rModel.pos + vVel * s_pEngine->GetDeltaTime();

    Level* pLevel = *s_pEngine->itCurrentLevel;

    if (-s_fVelocityEpsilon < vVel.x && vVel.x < s_fVelocityEpsilon && !IS_STATE_SET(nState, State::Jump))
        SET_STATE(nState, State::Idle);
    else
        UNSET_STATE(nState, State::Idle);

    auto go = [](TileType tile)
        {
            return tile != TileType::Grass && tile != TileType::Dirt;
        };

    if (vVel.x < 0.0f)
    {
        SET_STATE(nState, State::Left);
        UNSET_STATE(nState, State::Right);

        if (!go(pLevel->GetTile(def::vi2d(vNewPos.x, rModel.pos.y))) || !go(pLevel->GetTile(def::vi2d(vNewPos.x, rModel.pos.y + 0.9f))))
        {
            vNewPos.x = floor(vNewPos.x) + 1.0f;
            vVel.x = 0.0f;
        }
    }
    else
    {
        SET_STATE(nState, State::Right);
        UNSET_STATE(nState, State::Left);

        if (!go(pLevel->GetTile(def::vi2d(vNewPos.x + 1.0f, rModel.pos.y))) || !go(pLevel->GetTile(def::vi2d(vNewPos.x + 1.0f, rModel.pos.y + 0.9f))))
        {
            vNewPos.x = floor(vNewPos.x);
            vVel.x = 0.0f;
        }
    }

    if (vVel.y <= 0.0f)
    {
        if (!go(pLevel->GetTile(vNewPos)) || !go(pLevel->GetTile(vNewPos + def::vf2d(0.9f, 0.0f))))
        {
            vNewPos.y = floor(vNewPos.y) + 1.0f;
            vVel.y = 0.0f;
        }
    }
    else
    {
        if (!go(pLevel->GetTile(vNewPos + def::vf2d(0.0f, 1.0f))) || !go(pLevel->GetTile(vNewPos + def::vf2d(0.9f, 1.0f))))
        {
            vNewPos.y = floor(vNewPos.y);
            vVel.y = 0.0f;
            
            UNSET_STATE(nState, State::Jump);
        }
    }

    rModel.pos = vNewPos;
}

void Dynamic_Creature::ApplyGravity()
{
    float fDeltaTime = s_pEngine->GetDeltaTime();

    vVel.y += s_fFallSpeed * fDeltaTime;
    vVel.x += -s_fFriction * vVel.x * fDeltaTime;

    vVel = vVel.max(s_vMinVelocity).min(s_vMaxVelocity);

    Dynamic::ApplyGravity();
}




Dynamic_Player::Dynamic_Player(const def::vf2d& pos)
    : Dynamic_Creature(pos)
{
}

float Dynamic_Player::s_fGroundSpeed = 0.0f;
float Dynamic_Player::s_fAirSpeed = 0.0f;
float Dynamic_Player::s_fJumpSpeed = 0.0f;

void Dynamic_Player::UpdateControls()
{
    float fDeltaTime = s_pEngine->GetDeltaTime();

    if (s_pEngine->IsFocused())
    {
        if (s_pEngine->GetKey(def::Key::LEFT).held || s_pEngine->GetKey(def::Key::A).held)
            vVel.x += (IS_STATE_SET(nState, State::Jump) ? -s_fAirSpeed : -s_fGroundSpeed) * fDeltaTime;

        if (s_pEngine->GetKey(def::Key::RIGHT).held || s_pEngine->GetKey(def::Key::D).held)
            vVel.x += (IS_STATE_SET(nState, State::Jump) ? s_fAirSpeed : s_fGroundSpeed) * fDeltaTime;

        if (s_pEngine->GetKey(def::Key::SPACE).pressed ||
            s_pEngine->GetKey(def::Key::UP).pressed ||
            s_pEngine->GetKey(def::Key::W).pressed)
        {
            if (vVel.y == 0.0f) vVel.y = -s_fJumpSpeed;
            SET_STATE(nState, State::Jump);
            UNSET_STATE(nState, State::Idle);
        }
    }
}

void Dynamic_Player::UpdateCollision()
{
    auto& itLevel = s_pEngine->itCurrentLevel;
    Level* pLevel = *itLevel;

    def::vf2d vNewPos = rModel.pos + vVel * s_pEngine->GetDeltaTime();

    // Check for collision with tiles

    std::list<def::vf2d> listNewPos =
    {
        vNewPos,
        vNewPos + def::vf2d(0.0f, 1.0f),
        vNewPos + def::vf2d(1.0f, 0.0f),
        vNewPos + 1.0f
    };

    for (const auto& pos : listNewPos)
    {
        if (pLevel->GetTile(pos) == TileType::Coin)
        {
            pLevel->SetTile(pos, TileType::Empty);
            s_pEngine->nScore++;
        }
    }

    for (auto it = std::next(s_pEngine->vecDynamics.begin()); it != s_pEngine->vecDynamics.end(); it++)
    {
        Dynamic_Enemy* pEnemy = static_cast<Dynamic_Enemy*>(*it);
        if (!pEnemy) continue;

        std::vector<def::side> vecContactSides;
        std::vector<def::vf2d> vecContactPoints =
            def::intersects(rModel, pEnemy->rModel, &vecContactSides);

        if (!vecContactPoints.empty())
        {
            for (auto side : vecContactSides)
                OnEnemyTouch(pEnemy, side);
        }
    }

    Dynamic_Creature::UpdateCollision();

    // Switch level (a.k.a. map)

    if (rModel.pos.x < 0.0f)
    {
        if (itLevel == s_pEngine->vecLevels.begin())
        {
            rModel.pos.x = floor(rModel.pos.x) + 1.0f;
            vVel.x = 0.0f;
        }
        else
        {
            rModel.pos.x += float(pLevel->GetSize().x - 1);
            --itLevel;
        }
    }
    else if (rModel.pos.x >= pLevel->GetSize().x - 1.0f)
    {
        if (itLevel == s_pEngine->vecLevels.end() - 1)
        {
            rModel.pos.x = floor(rModel.pos.x);
            vVel.x = 0.0f;
        }
        else
        {
            rModel.pos.x -= float(pLevel->GetSize().x - 1);
            ++itLevel;
        }
    }
}

void Dynamic_Player::SwitchFrame(float fPeriod)
{
    Dynamic::SwitchFrame(fPeriod);

    if      (IS_STATE_SET(nState, State::Idle)) vGraphicsID.y = 0;
    else if (IS_STATE_SET(nState, State::Jump)) vGraphicsID.y = 2;
    else                                        vGraphicsID.y = nFrameCounter;

    if      (IS_STATE_SET(nState, State::Right)) vGraphicsID.x = 0;
    else if (IS_STATE_SET(nState, State::Left))  vGraphicsID.x = 1;
}

void Dynamic_Player::OnEnemyTouch(Dynamic_Enemy* pEnemy, def::side side)
{
    if (side == def::side::BOTTOM)
    {
        vVel.y = -s_fJumpSpeed * 0.5f;
        pEnemy->Die();
    }
    else
        Die();
}

void Dynamic_Player::Die()
{
    
}




Dynamic_Enemy::Dynamic_Enemy(const def::vf2d& pos)
    : Dynamic_Creature(pos)
{
}

float Dynamic_Enemy::s_fGroundSpeed = 0.0f;
float Dynamic_Enemy::s_fAirSpeed = 0.0f;
float Dynamic_Enemy::s_fJumpSpeed = 0.0f;

void Dynamic_Enemy::UpdateControls()
{
    float fDeltaTime = s_pEngine->GetDeltaTime();

    if (IS_STATE_SET(nState, State::Left))
        vVel.x += (IS_STATE_SET(nState, State::Jump) ? -s_fAirSpeed : -s_fGroundSpeed) * fDeltaTime;

    if (IS_STATE_SET(nState, State::Right))
        vVel.x += (IS_STATE_SET(nState, State::Jump) ? s_fAirSpeed : s_fGroundSpeed) * fDeltaTime;
}

void Dynamic_Enemy::UpdateCollision()
{
    Dynamic_Creature::UpdateCollision();
}

void Dynamic_Enemy::SwitchFrame(float fPeriod)
{
    Dynamic::SwitchFrame(fPeriod);

    vGraphicsID.x = nFrameCounter;
    vGraphicsID.y = 3;
}

void Dynamic_Enemy::Die()
{
    bRedundant = true;
}
