#define DEF_GEOMETRY2D_IMPL
#include "../Include/Dynamic.hpp"

Dynamic::Dynamic(const def::vf2d& pos) : rModel(pos, { 1, 1 })
{
    
}

void Dynamic::UpdateControls()
{
}

void Dynamic::UpdateCollision(std::list<def::side>& vecSides)
{
    vecSides.push_back(def::SIDE_NONE);
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

    std::list<def::side> vecSides;
    UpdateCollision(vecSides);
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

    fFrameTimer += Game::Get().GetDeltaTime();
}




Dynamic_Creature::Dynamic_Creature(const def::vf2d& pos)
    : Dynamic(pos)
{
}

void Dynamic_Creature::UpdateControls()
{
}

void Dynamic_Creature::UpdateCollision(std::list<def::side>& vecSides)
{
    def::vf2d vNewPos = rModel.pos + vVel * Game::Get().GetDeltaTime();

    Level* pLevel = *Game::Get().itCurrentLevel;

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
            vecSides.push_back(def::SIDE_LEFT);
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
            vecSides.push_back(def::SIDE_RIGHT);
            vVel.x = 0.0f;
        }
    }

    if (vVel.y <= 0.0f)
    {
        if (!go(pLevel->GetTile(vNewPos)) || !go(pLevel->GetTile(vNewPos + def::vf2d(0.9f, 0.0f))))
        {
            vNewPos.y = floor(vNewPos.y) + 1.0f;
            vecSides.push_back(def::SIDE_BOTTOM);
            vVel.y = 0.0f;
        }
    }
    else
    {
        if (!go(pLevel->GetTile(vNewPos + def::vf2d(0.0f, 1.0f))) || !go(pLevel->GetTile(vNewPos + def::vf2d(0.9f, 1.0f))))
        {
            vNewPos.y = floor(vNewPos.y);
            vecSides.push_back(def::SIDE_TOP);
            vVel.y = 0.0f;
            
            UNSET_STATE(nState, State::Jump);
        }
    }

    rModel.pos = vNewPos;
}

void Dynamic_Creature::ApplyGravity()
{
    float fDeltaTime = Game::Get().GetDeltaTime();

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
    Game& engine = Game::Get();
    float fDeltaTime = engine.GetDeltaTime();

    if (engine.IsFocused())
    {
        if (engine.GetKey(def::Key::LEFT).held || engine.GetKey(def::Key::A).held)
            vVel.x += (IS_STATE_SET(nState, State::Jump) ? -s_fAirSpeed : -s_fGroundSpeed) * fDeltaTime;

        if (engine.GetKey(def::Key::RIGHT).held || engine.GetKey(def::Key::D).held)
            vVel.x += (IS_STATE_SET(nState, State::Jump) ? s_fAirSpeed : s_fGroundSpeed) * fDeltaTime;

        if (engine.GetKey(def::Key::SPACE).pressed ||
            engine.GetKey(def::Key::UP).pressed ||
            engine.GetKey(def::Key::W).pressed)
        {
            if (vVel.y == 0.0f) vVel.y = -s_fJumpSpeed;
            SET_STATE(nState, State::Jump);
            UNSET_STATE(nState, State::Idle);
        }
    }
}

void Dynamic_Player::UpdateCollision(std::list<def::side>& vecSides)
{
    Game& engine = Game::Get();
    auto& itLevel = engine.itCurrentLevel;
    Level* pLevel = *itLevel;

    def::vf2d vNewPos = rModel.pos + vVel * engine.GetDeltaTime();

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
            engine.nScore++;
        }
    }

    auto& vecDynamics = pLevel->listDynamics;

    for (auto it = std::next(vecDynamics.begin()); it != vecDynamics.end(); it++)
    {
        Dynamic_Enemy* pEnemy = static_cast<Dynamic_Enemy*>(it->pDynamic);
        if (!pEnemy) continue;

        std::vector<def::side> vecContactSides;
        std::vector<def::vf2d> vecContactPoints =
            def::intersects(rModel, pEnemy->rModel, &vecContactSides);

        if (!vecContactPoints.empty())
        {
            for (auto s : vecContactSides)
            {
                if (OnEnemyTouch(pEnemy, s))
                    it->bRedundant = true;
            }
        }
    }

    Dynamic_Creature::UpdateCollision(vecSides);

    // Switch level (a.k.a. map)

    if (rModel.pos.x < 0.0f)
    {
        if (itLevel == engine.vecLevels.begin())
        {
            rModel.pos.x = floor(rModel.pos.x) + 1.0f;
            vVel.x = 0.0f;
        }
        else
        {
            rModel.pos.x += float(pLevel->GetSize().x - 1);

            auto& listDynamics = pLevel->listDynamics;

            auto& player = listDynamics.front();
            player.bRedundant = true;
            --itLevel;

            (*itLevel)->listDynamics.push_front({ false, player.pDynamic });
            Game::Get().pPlayer = (*itLevel)->listDynamics.begin()->pDynamic;
        }
    }
    else if (rModel.pos.x >= pLevel->GetSize().x - 1.0f)
    {
        if (itLevel == engine.vecLevels.end() - 1)
        {
            rModel.pos.x = floor(rModel.pos.x);
            vVel.x = 0.0f;
        }
        else
        {
            rModel.pos.x -= float(pLevel->GetSize().x - 1);

            auto& listDynamics = pLevel->listDynamics;

            auto& player = listDynamics.front();
            player.bRedundant = true;
            ++itLevel;

            (*itLevel)->listDynamics.push_front({ false, player.pDynamic });
            Game::Get().pPlayer = (*itLevel)->listDynamics.begin()->pDynamic;
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

bool Dynamic_Player::OnEnemyTouch(Dynamic_Enemy* pEnemy, def::side side)
{
    if (side == def::SIDE_BOTTOM)
    {
        vVel.y = -s_fJumpSpeed * 0.5f;
        return pEnemy->Die();
    }

    return Die();
}

bool Dynamic_Player::Die()
{
    return false;
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
    float fDeltaTime = Game::Get().GetDeltaTime();

    if (IS_STATE_SET(nState, State::Left))
        vVel.x += (IS_STATE_SET(nState, State::Jump) ? -s_fAirSpeed : -s_fGroundSpeed) * fDeltaTime;

    if (IS_STATE_SET(nState, State::Right))
        vVel.x += (IS_STATE_SET(nState, State::Jump) ? s_fAirSpeed : s_fGroundSpeed) * fDeltaTime;
}

void Dynamic_Enemy::UpdateCollision(std::list<def::side>& vecSides)
{
    Dynamic_Creature::UpdateCollision(vecSides);

    for (auto side : vecSides)
    {
        if (side == def::SIDE_LEFT)
        {
            SET_STATE(nState, State::Right);
            UNSET_STATE(nState, State::Left);
        }
        else if (side == def::SIDE_RIGHT)
        {
            SET_STATE(nState, State::Left);
            UNSET_STATE(nState, State::Right);
        }
    }
}

void Dynamic_Enemy::SwitchFrame(float fPeriod)
{
    Dynamic::SwitchFrame(fPeriod);

    vGraphicsID.x = nFrameCounter;
    vGraphicsID.y = 3;
}

bool Dynamic_Enemy::Die()
{
    return true;
}
