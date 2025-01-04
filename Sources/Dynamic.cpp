#define DEF_GEOMETRY2D_IMPL
#include "../Include/Dynamic.hpp"

Dynamic::Dynamic(const def::vf2d& pos, const def::vf2d& size)
{
    model.size = size;
    SetPosition(pos);
}

void Dynamic::UpdateControls()
{
}

void Dynamic::UpdateCollision(std::list<def::side>& sides)
{
    sides.push_back(def::SIDE_NONE);
}

void Dynamic::ApplyGravity()
{
    if (!IS_STATE_SET(state, State::Jump))
    {
        if (velocity.x == 0.0f && velocity.y == 0.0f)
            SET_STATE(state, State::Idle);
    }
}

void Dynamic::Update()
{
    SwitchFrame(0.2f);
    UpdateControls();
    ApplyGravity();

    std::list<def::side> sides;
    UpdateCollision(sides);
}

void Dynamic::SetPosition(const def::vf2d& pos)
{
    model.pos = pos;

    float ix = 1.0f / Assets::Get().tileSize.x;
    float iy = 1.0f / Assets::Get().tileSize.y;

    // Left side
    edgelessModel[0].start = { pos.x, pos.y + iy };
    edgelessModel[0].end = { pos.x, pos.y + model.size.y - iy };

    // Top side
    edgelessModel[1].start = { pos.x + ix, pos.y };
    edgelessModel[1].end = { pos.x + model.size.x - ix, pos.y };

    // Right side
    edgelessModel[2].start = { pos.x + model.size.x, pos.y + iy };
    edgelessModel[2].end = { pos.x + model.size.x, pos.y + model.size.y - iy };

    // Bottom side
    edgelessModel[3].start = { pos.x + ix, pos.y + model.size.y };
    edgelessModel[3].end = { pos.x + model.size.x - ix, pos.y + model.size.y };
}

void Dynamic::OffsetPosition(const def::vf2d& offset)
{
    model.pos += offset;

    for (uint8_t i = 0; i < 4; i++)
    {
        edgelessModel[i].start += offset;
        edgelessModel[i].end += offset;
    }
}

void Dynamic::SwitchFrame(const float period)
{
    if (!IS_STATE_SET(state, State::Jump) && !IS_STATE_SET(state, State::Idle))
    {
        if (m_FrameTimer >= period)
        {
            ++frameCounter %= 2;
            m_FrameTimer -= period;
        }
    }

    m_FrameTimer += Game::Get().GetDeltaTime();
}




Dynamic_Creature::Dynamic_Creature(const def::vf2d& pos, const def::vf2d& size)
    : Dynamic(pos, size)
{
}

void Dynamic_Creature::UpdateControls()
{
}

void Dynamic_Creature::UpdateCollision(std::list<def::side>& sides)
{
    def::vf2d newPos = model.pos + velocity * Game::Get().GetDeltaTime();

    Level* level = *Game::Get().GetCurrentLevel();

    if (-s_VelocityEpsilon < velocity.x && velocity.x < s_VelocityEpsilon && !IS_STATE_SET(state, State::Jump))
        SET_STATE(state, State::Idle);
    else
        UNSET_STATE(state, State::Idle);

    auto Go = [](TileType tile)
        {
            return tile != TileType::Grass && tile != TileType::Dirt;
        };

    if (velocity.x < 0.0f)
    {
        SET_STATE(state, State::Left);
        UNSET_STATE(state, State::Right);

        if (!Go(level->GetTile(def::vi2d(newPos.x, model.pos.y))) || !Go(level->GetTile(def::vi2d(newPos.x, model.pos.y + model.size.y - 0.1f))))
        {
            newPos.x = floor(newPos.x) + model.size.x;
            sides.push_back(def::SIDE_LEFT);
            velocity.x = 0.0f;
        }
    }
    else if (velocity.x > 0.0f)
    {
        SET_STATE(state, State::Right);
        UNSET_STATE(state, State::Left);

        if (!Go(level->GetTile(def::vi2d(newPos.x + model.size.x, model.pos.y))) || !Go(level->GetTile(def::vi2d(newPos.x + model.size.x, model.pos.y + model.size.y - 0.1f))))
        {
            newPos.x = floor(newPos.x);
            sides.push_back(def::SIDE_RIGHT);
            velocity.x = 0.0f;
        }
    }

    if (velocity.y < 0.0f)
    {
        if (!Go(level->GetTile(newPos)) || !Go(level->GetTile(newPos + def::vf2d(model.size.x - 0.1f, 0.0f))))
        {
            newPos.y = floor(newPos.y) + model.size.y;
            sides.push_back(def::SIDE_BOTTOM);
            velocity.y = 0.0f;
        }
    }
    else if (velocity.y > 0.0f)
    {
        if (!Go(level->GetTile(newPos + def::vf2d(0.0f, 1.0f))) || !Go(level->GetTile(newPos + def::vf2d(model.size.x - 0.1f, model.size.y))))
        {
            newPos.y = floor(newPos.y);
            sides.push_back(def::SIDE_TOP);
            velocity.y = 0.0f;
            
            UNSET_STATE(state, State::Jump);
        }
    }

    SetPosition(newPos);
}

void Dynamic_Creature::ApplyGravity()
{
    float fDeltaTime = Game::Get().GetDeltaTime();

    velocity.y += s_FallSpeed * fDeltaTime;
    velocity.x += -s_Friction * velocity.x * fDeltaTime;

    Dynamic::ApplyGravity();
}




Dynamic_Player::Dynamic_Player(const def::vf2d& pos)
    : Dynamic_Creature(pos, { 1.0f, 1.0f })
{
}

float Dynamic_Player::s_GroundSpeed = 0.0f;
float Dynamic_Player::s_AirSpeed = 0.0f;
float Dynamic_Player::s_JumpSpeed = 0.0f;

void Dynamic_Player::UpdateControls()
{
    Game& engine = Game::Get();
    float fDeltaTime = engine.GetDeltaTime();

    if (engine.IsFocused())
    {
        if (engine.GetKey(def::Key::LEFT).held || engine.GetKey(def::Key::A).held)
            velocity.x += (IS_STATE_SET(state, State::Jump) ? -s_AirSpeed : -s_GroundSpeed) * fDeltaTime;

        if (engine.GetKey(def::Key::RIGHT).held || engine.GetKey(def::Key::D).held)
            velocity.x += (IS_STATE_SET(state, State::Jump) ? s_AirSpeed : s_GroundSpeed) * fDeltaTime;

        if (engine.GetKey(def::Key::SPACE).pressed ||
            engine.GetKey(def::Key::UP).pressed ||
            engine.GetKey(def::Key::W).pressed)
        {
            if (velocity.y == 0.0f) velocity.y = -s_JumpSpeed;
            SET_STATE(state, State::Jump);
            UNSET_STATE(state, State::Idle);
        }
    }
}

void Dynamic_Player::UpdateCollision(std::list<def::side>& sides)
{
    Game& engine = Game::Get();
    auto& levelIterator = engine.GetCurrentLevel();
    Level* level = *levelIterator;

    velocity = velocity.max(s_MinVelocity).min(s_MaxVelocity);

    def::vf2d newPos = model.pos + velocity * engine.GetDeltaTime();

    // Check for collision with tiles

    std::list<def::vf2d> listNewPos =
    {
        newPos,
        newPos + def::vf2d(0.0f, 1.0f),
        newPos + def::vf2d(1.0f, 0.0f),
        newPos + 1.0f
    };

    for (const auto& pos : listNewPos)
    {
        if (level->GetTile(pos) == TileType::Coin)
        {
            level->SetTile(pos, TileType::Empty);
            engine.IncreaseScore();
        }
    }

    auto& dynamics = level->dynamics;

    auto CheckCollision = [&](Dynamic_Creature* pCreature1, Dynamic_Creature* pCreature2)
        {
            for (uint8_t i = 0; i < 4; i++)
                for (uint8_t j = 0; j < 4; j++)
                {
                    std::vector<def::vf2d> points;

                    if (def::intersects(pCreature1->edgelessModel[i], pCreature2->edgelessModel[j], points))
                         return def::side(j);
                }

            return def::SIDE_NONE;
        };

    for (auto it = std::next(dynamics.begin()); it != dynamics.end(); it++)
    {
        Dynamic_Enemy* enemy = static_cast<Dynamic_Enemy*>(it->dynamic);

        if (it->isRedundant || !enemy)
            continue;

        def::side side = CheckCollision(this, enemy);

        if (side != def::SIDE_NONE)
        {
            if (OnEnemyTouch(enemy, side))
            {
                if (enemy->OnHit())
                {
                    // Here we kill an enemy
                    it->isRedundant = true;
                }
            }
            else
            {
                // and here we kill a player
                dynamics.begin()->isRedundant = true;
            }
        }
    }

    for (auto it1 = std::next(dynamics.begin()); it1 != dynamics.end(); it1++)
        for (auto it2 = std::next(dynamics.begin()); it2 != dynamics.end(); it2++)
        {
            if (it1 == it2)
                continue;

            Dynamic_Enemy* enemy1 = static_cast<Dynamic_Enemy*>(it1->dynamic);
            Dynamic_Enemy* enemy2 = static_cast<Dynamic_Enemy*>(it2->dynamic);

            if (enemy1->isFriendlyFire)
            {
                if (CheckCollision(enemy1, enemy2) != def::SIDE_NONE)
                    it2->isRedundant = true;
            }

            if (enemy2->isFriendlyFire)
            {
                if (CheckCollision(enemy1, enemy2) != def::SIDE_NONE)
                    it1->isRedundant = true;
            }
        }

    Dynamic_Creature::UpdateCollision(sides);

    // Switch level (a.k.a. map)

    auto& levels = engine.GetLevels();

    if (model.pos.x < 0.0f)
    {
        if (levelIterator == levels.begin())
        {
            SetPosition({ floor(model.pos.x) + 1.0f, model.pos.y });
            velocity.x = 0.0f;
        }
        else
        {
            OffsetPosition({ float(level->GetSize().x - 1), 0.0f });

            auto& dynamics = level->dynamics;

            auto& player = dynamics.front();
            player.isRedundant = true;
            --levelIterator;

            (*levelIterator)->dynamics.push_front({ false, player.dynamic });
            Game::Get().GetPlayer() = (*levelIterator)->dynamics.begin()->dynamic;
        }
    }
    else if (model.pos.x >= level->GetSize().x - 1.0f)
    {
        if (levelIterator == levels.end() - 1)
        {
            SetPosition({ floor(model.pos.x), model.pos.y });
            velocity.x = 0.0f;
        }
        else
        {
            OffsetPosition({ float(1 - level->GetSize().x), 0.0f });

            auto& dynamics = level->dynamics;

            auto& player = dynamics.front();
            player.isRedundant = true;
            ++levelIterator;

            (*levelIterator)->dynamics.push_front({ false, player.dynamic });
            Game::Get().GetPlayer() = (*levelIterator)->dynamics.begin()->dynamic;
        }
    }
}

void Dynamic_Player::SwitchFrame(const float period)
{
    Dynamic::SwitchFrame(period);

    if      (IS_STATE_SET(state, State::Idle)) graphicsID.y = 0;
    else if (IS_STATE_SET(state, State::Jump)) graphicsID.y = 2;
    else                                       graphicsID.y = frameCounter;

    if      (IS_STATE_SET(state, State::Right)) graphicsID.x = 0;
    else if (IS_STATE_SET(state, State::Left))  graphicsID.x = 1;
}

bool Dynamic_Player::OnEnemyTouch(Dynamic_Enemy* enemy, def::side side)
{
    if (side == def::SIDE_TOP)
    {
        velocity.y = -s_JumpSpeed * 0.5f;
        return true;
    }
 
    return enemy->OnSideTouch(side);
}



Dynamic_Enemy::Dynamic_Enemy(const def::vf2d& pos, const def::vf2d& size)
    : Dynamic_Creature(pos, size)
{
}

float Dynamic_Enemy::s_GroundSpeed = 0.0f;
float Dynamic_Enemy::s_AirSpeed = 0.0f;
float Dynamic_Enemy::s_JumpSpeed = 0.0f;

void Dynamic_Enemy::UpdateControls()
{
    float deltaTime = Game::Get().GetDeltaTime();

    float speed = (IS_STATE_SET(state, State::Jump) ? s_AirSpeed : s_GroundSpeed);
    if (IS_STATE_SET(state, State::Faster)) speed *= 100.0f;

    if (IS_STATE_SET(state, State::Left))
        velocity.x += -speed * deltaTime;

    if (IS_STATE_SET(state, State::Right))
        velocity.x += speed * deltaTime;
}

void Dynamic_Enemy::UpdateCollision(std::list<def::side>& sides)
{
    velocity = velocity.max(s_MinVelocity).min(s_MaxVelocity);

    Dynamic_Creature::UpdateCollision(sides);

    for (auto side : sides)
    {
        if (side == def::SIDE_LEFT)
        {
            SET_STATE(state, State::Right);
            UNSET_STATE(state, State::Left);
        }
        else if (side == def::SIDE_RIGHT)
        {
            SET_STATE(state, State::Left);
            UNSET_STATE(state, State::Right);
        }
    }
}

void Dynamic_Enemy::SwitchFrame(const float period)
{
    Dynamic::SwitchFrame(period);

    graphicsID.x = frameCounter;
}

bool Dynamic_Enemy::OnHit()
{
    return true;
}

bool Dynamic_Enemy::OnSideTouch(const def::side nSide)
{
    return false;
}

Dynamic_Enemy_Mushroom::Dynamic_Enemy_Mushroom(const def::vf2d& pos)
    : Dynamic_Enemy(pos, { 1.0f, 1.0f })
{
    graphicsID.y = 3;
}

bool Dynamic_Enemy_Mushroom::OnHit()
{
    return true;
}

bool Dynamic_Enemy_Mushroom::OnSideTouch(const def::side side)
{
    return false;
}

void Dynamic_Enemy_Mushroom::SwitchFrame(const float period)
{
    Dynamic_Enemy::SwitchFrame(period);
}

Dynamic_Enemy_Turtle::Dynamic_Enemy_Turtle(const def::vf2d& pos)
    : Dynamic_Enemy(pos, { 1.0f, 2.0f })
{
    model.size = { 1.0f, 2.0f };
    SetPosition(pos);

    graphicsID.y = 4;
}

bool Dynamic_Enemy_Turtle::OnHit()
{
    switch (turtleState)
    {
    case TurtleState::Walk:
    {
        graphicsID.x = 2;
        graphicsID.y = 5;

        turtleState = TurtleState::Shell;

        velocity.x = 0.0f;

        UNSET_STATE(state, State::Left);
        SET_STATE(state, State::Idle);

        model.size = { 1.0f, 1.0f };
        SetPosition({ model.pos.x, model.pos.y + 1.0f });
    }
    break;

    }
    
    return false;
}

bool Dynamic_Enemy_Turtle::OnSideTouch(const def::side side)
{
    if (turtleState == TurtleState::Walk)
        return false;

    isFriendlyFire = true;

    UNSET_STATE(state, State::Idle);
    SET_STATE(state, State::Faster);

    switch (side)
    {
    case def::SIDE_LEFT:
        SET_STATE(state, State::Right);
    break;

    case def::SIDE_RIGHT:
        SET_STATE(state, State::Left);
    break;

    }

    return true;
}

void Dynamic_Enemy_Turtle::SwitchFrame(const float period)
{
    if (turtleState == TurtleState::Walk)
        Dynamic_Enemy::SwitchFrame(period);
}
