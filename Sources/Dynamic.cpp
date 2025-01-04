#define DEF_GEOMETRY2D_IMPL
#include "../Include/Dynamic.hpp"

Dynamic::Dynamic(const def::vf2d& pos, const def::vf2d& size)
{
    rModel.size = size;
    SetPosition(pos);
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

void Dynamic::SetPosition(const def::vf2d& pos)
{
    rModel.pos = pos;

    float ix = 1.0f / Assets::Get().vTileSize.x;
    float iy = 1.0f / Assets::Get().vTileSize.y;

    // Left side
    rEdgelessModel[0].start = { pos.x, pos.y + iy };
    rEdgelessModel[0].end = { pos.x, pos.y + rModel.size.y - iy };

    // Top side
    rEdgelessModel[1].start = { pos.x + ix, pos.y };
    rEdgelessModel[1].end = { pos.x + rModel.size.x - ix, pos.y };

    // Right side
    rEdgelessModel[2].start = { pos.x + rModel.size.x, pos.y + iy };
    rEdgelessModel[2].end = { pos.x + rModel.size.x, pos.y + rModel.size.y - iy };

    // Bottom side
    rEdgelessModel[3].start = { pos.x + ix, pos.y + rModel.size.y };
    rEdgelessModel[3].end = { pos.x + rModel.size.x - ix, pos.y + rModel.size.y };
}

void Dynamic::OffsetPosition(const def::vf2d& offset)
{
    rModel.pos += offset;

    for (uint8_t i = 0; i < 4; i++)
    {
        rEdgelessModel[i].start += offset;
        rEdgelessModel[i].end += offset;
    }
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




Dynamic_Creature::Dynamic_Creature(const def::vf2d& pos, const def::vf2d& size)
    : Dynamic(pos, size)
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

        if (!go(pLevel->GetTile(def::vi2d(vNewPos.x, rModel.pos.y))) || !go(pLevel->GetTile(def::vi2d(vNewPos.x, rModel.pos.y + rModel.size.y - 0.1f))))
        {
            vNewPos.x = floor(vNewPos.x) + rModel.size.x;
            vecSides.push_back(def::SIDE_LEFT);
            vVel.x = 0.0f;
        }
    }
    else if (vVel.x > 0.0f)
    {
        SET_STATE(nState, State::Right);
        UNSET_STATE(nState, State::Left);

        if (!go(pLevel->GetTile(def::vi2d(vNewPos.x + rModel.size.x, rModel.pos.y))) || !go(pLevel->GetTile(def::vi2d(vNewPos.x + rModel.size.x, rModel.pos.y + rModel.size.y - 0.1f))))
        {
            vNewPos.x = floor(vNewPos.x);
            vecSides.push_back(def::SIDE_RIGHT);
            vVel.x = 0.0f;
        }
    }

    if (vVel.y < 0.0f)
    {
        if (!go(pLevel->GetTile(vNewPos)) || !go(pLevel->GetTile(vNewPos + def::vf2d(rModel.size.x - 0.1f, 0.0f))))
        {
            vNewPos.y = floor(vNewPos.y) + rModel.size.y;
            vecSides.push_back(def::SIDE_BOTTOM);
            vVel.y = 0.0f;
        }
    }
    else if (vVel.y > 0.0f)
    {
        if (!go(pLevel->GetTile(vNewPos + def::vf2d(0.0f, 1.0f))) || !go(pLevel->GetTile(vNewPos + def::vf2d(rModel.size.x - 0.1f, rModel.size.y))))
        {
            vNewPos.y = floor(vNewPos.y);
            vecSides.push_back(def::SIDE_TOP);
            vVel.y = 0.0f;
            
            UNSET_STATE(nState, State::Jump);
        }
    }

    SetPosition(vNewPos);
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
    : Dynamic_Creature(pos, { 1.0f, 1.0f })
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

    auto& listDynamics = pLevel->listDynamics;

    auto CheckCollision = [&](Dynamic_Creature* pCreature1, Dynamic_Creature* pCreature2)
        {
            for (uint8_t i = 0; i < 4; i++)
                for (uint8_t j = 0; j < 4; j++)
                {
                    std::vector<def::vf2d> points;

                    if (def::intersects(pCreature1->rEdgelessModel[i], pCreature2->rEdgelessModel[j], points))
                         return def::side(j);
                }

            return def::SIDE_NONE;
        };

    for (auto it = std::next(listDynamics.begin()); it != listDynamics.end(); it++)
    {
        Dynamic_Enemy* pEnemy = static_cast<Dynamic_Enemy*>(it->pDynamic);
        if (it->bRedundant || !pEnemy) continue;

        def::side side = CheckCollision(this, pEnemy);

        if (side != def::SIDE_NONE)
        {
            if (OnEnemyTouch(pEnemy, side))
            {
                if (pEnemy->OnHit())
                {
                    // Here we kill an enemy
                    it->bRedundant = true;
                }
            }
            else
            {
                // and here we kill a player
                listDynamics.begin()->bRedundant = true;
            }
        }
    }

    for (auto it1 = std::next(listDynamics.begin()); it1 != listDynamics.end(); it1++)
        for (auto it2 = std::next(listDynamics.begin()); it2 != listDynamics.end(); it2++)
        {
            if (it1 == it2) continue;

            Dynamic_Enemy* pEnemy1 = static_cast<Dynamic_Enemy*>(it1->pDynamic);
            Dynamic_Enemy* pEnemy2 = static_cast<Dynamic_Enemy*>(it2->pDynamic);

            if (pEnemy1->bFriendlyFire)
            {
                if (CheckCollision(pEnemy1, pEnemy2) != def::SIDE_NONE)
                    it2->bRedundant = true;
            }

            if (pEnemy2->bFriendlyFire)
            {
                if (CheckCollision(pEnemy1, pEnemy2) != def::SIDE_NONE)
                    it1->bRedundant = true;
            }
        }

    Dynamic_Creature::UpdateCollision(vecSides);

    // Switch level (a.k.a. map)

    if (rModel.pos.x < 0.0f)
    {
        if (itLevel == engine.vecLevels.begin())
        {
            SetPosition({ floor(rModel.pos.x) + 1.0f, rModel.pos.y });
            vVel.x = 0.0f;
        }
        else
        {
            OffsetPosition({ float(pLevel->GetSize().x - 1), 0.0f });

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
            SetPosition({ floor(rModel.pos.x), rModel.pos.y });
            vVel.x = 0.0f;
        }
        else
        {
            OffsetPosition({ float(1 - pLevel->GetSize().x), 0.0f });

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
    if (side == def::SIDE_TOP)
    {
        vVel.y = -s_fJumpSpeed * 0.5f;
        return true;
    }
 
    return pEnemy->OnSideTouch(side);
}



Dynamic_Enemy::Dynamic_Enemy(const def::vf2d& pos, const def::vf2d& size)
    : Dynamic_Creature(pos, size)
{
}

float Dynamic_Enemy::s_fGroundSpeed = 0.0f;
float Dynamic_Enemy::s_fAirSpeed = 0.0f;
float Dynamic_Enemy::s_fJumpSpeed = 0.0f;

void Dynamic_Enemy::UpdateControls()
{
    float fDeltaTime = Game::Get().GetDeltaTime();

    float fSpeed = (IS_STATE_SET(nState, State::Jump) ? s_fAirSpeed : s_fGroundSpeed);
    if (IS_STATE_SET(nState, State::Faster)) fSpeed *= 50.0f;

    if (IS_STATE_SET(nState, State::Left))
        vVel.x += -fSpeed * fDeltaTime;

    if (IS_STATE_SET(nState, State::Right))
        vVel.x += fSpeed * fDeltaTime;
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
}

bool Dynamic_Enemy::OnHit()
{
    return true;
}

bool Dynamic_Enemy::OnSideTouch(def::side nSide)
{
    return false;
}

Dynamic_Enemy_Mushroom::Dynamic_Enemy_Mushroom(const def::vf2d& pos)
    : Dynamic_Enemy(pos, { 1.0f, 1.0f })
{
    vGraphicsID.y = 3;
}

bool Dynamic_Enemy_Mushroom::OnHit()
{
    return true;
}

bool Dynamic_Enemy_Mushroom::OnSideTouch(def::side nSide)
{
    return false;
}

void Dynamic_Enemy_Mushroom::SwitchFrame(float fPeriod)
{
    Dynamic_Enemy::SwitchFrame(fPeriod);
}

Dynamic_Enemy_Turtle::Dynamic_Enemy_Turtle(const def::vf2d& pos)
    : Dynamic_Enemy(pos, { 1.0f, 2.0f })
{
    rModel.size = { 1.0f, 2.0f };
    SetPosition(pos);

    vGraphicsID.y = 4;
}

bool Dynamic_Enemy_Turtle::OnHit()
{
    switch (nTurtleState)
    {
    case TurtleState::Walk:
    {
        vGraphicsID.x = 2;
        vGraphicsID.y = 5;
        nTurtleState = TurtleState::Shell;
        vVel.x = 0.0f;
        UNSET_STATE(nState, State::Left);
        SET_STATE(nState, State::Idle);
        rModel.size = { 1.0f, 1.0f };
        SetPosition({ rModel.pos.x, rModel.pos.y + 1.0f });
    }
    break;

    }
    
    return false;
}

bool Dynamic_Enemy_Turtle::OnSideTouch(def::side nSide)
{
    if (nTurtleState == TurtleState::Walk)
        return false;

    bFriendlyFire = true;

    UNSET_STATE(nState, State::Idle);
    SET_STATE(nState, State::Faster);

    switch (nSide)
    {
    case def::SIDE_LEFT:
        SET_STATE(nState, State::Right);
    break;

    case def::SIDE_RIGHT:
        SET_STATE(nState, State::Left);
    break;

    }

    return true;
}

void Dynamic_Enemy_Turtle::SwitchFrame(float fPeriod)
{
    if (nTurtleState == TurtleState::Walk)
        Dynamic_Enemy::SwitchFrame(fPeriod);
}
