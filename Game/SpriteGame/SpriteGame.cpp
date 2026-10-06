#include "SpriteGame.h"
#include "Engine.h"
#include "Resources/ResourceManager.h"
#include "PlayerController.h"

#include <SDL3/SDL.h>

#include <iostream>
#include <memory>

using namespace std;
using namespace nu;

bool SpriteGame::Initialize()
{
    SetWorkingDirectory("SpriteGame");

    if (!Game::Initialize())
    {
        return false;
    }

    m_scene = make_unique<Scene>();
    m_scene->SetGame(this);
    m_scene->Load("Scenes/scene.json");
    m_scene->SetName("level1");

    m_titleText =
        new Text(
            Resources().GetWithID<Font>(
                "title_font",
                "8-bit-limit/8bitlim.ttf",
                100.0f));

    m_titleText->Create(
        Engine::Get().GetRenderer(),
        "Wiz v. Pyro",
        Color{ 1.0f, 1.0f, 1.0f });

    m_gameOverText =
        new Text(
            Resources().GetWithID<Font>(
                "title_font",
                "8-bit-limit/8bitlim.ttf",
                120.0f));

    m_scoreText =
        new Text(
            Resources().GetWithID<Font>(
                "game_font",
                "8-bit-limit/8bitlim.ttf",
                50.0f));

    Engine::Get().GetAudio().AddSound("jump", "Audio/jump.wav");
    Engine::Get().GetAudio().AddSound("hit", "Audio/hit.wav");
    Engine::Get().GetAudio().AddSound("startgame", "Audio/startgame.wav");

    return true;
}

void SpriteGame::Update(float dt)
{
    switch (m_gameState)
    {
    case GameState::Title:
        if (Engine::Get().GetInput().GetKeyPressed(SDL_SCANCODE_SPACE))
        {
            Engine::Get().GetAudio().PlaySound("startgame");
            m_gameState = GameState::StartGame;
        }
        break;

    case GameState::StartGame:
        m_score = 0;

        m_gameState = GameState::StartLevel;
        break;

    case GameState::StartLevel:

        m_scene->RemoveAllActors();
        if (m_scene->GetName() == "level1") {
            m_scene->Load("scenes/level.json");
        }
        else if (m_scene->GetName() == "level2"){
            m_scene->Load("scenes/level2.json");
        }

        SpawnEnemy();
        SpawnPlayer();

        m_gameState = GameState::Game;
        break;

    case GameState::Game:
        if (Engine::Get().GetInput().GetKeyPressed(SDL_SCANCODE_R)) {
            m_gameState = GameState::StartLevel;
        }
        break;

    case GameState::GameOver:
        m_scene->RemoveAllActors();

        if (Engine::Get().GetInput().GetKeyPressed(SDL_SCANCODE_RETURN))
        {
            Engine::Get().GetAudio().PlaySound("startgame");
            m_scene->SetName("level1");
            m_gameState = GameState::Title;
        }
        break;

    default:
        break;
    }

    Game::Update(dt);
}

void SpriteGame::Draw(Renderer& renderer)
{
    renderer.EnableCamera(false);

    renderer.DrawTexture(
        *Resources().Get<Texture>(
            "textures/background.png",
            Engine::Get().GetRenderer()),
        600.0f,
        500.0f,
        0.0f,
        1.0f);

    Game::Draw(renderer);

    switch (m_gameState)
    {
    case GameState::Title:
        m_titleText->Draw(renderer, 480.0f, 400.0f);
        break;


    case GameState::StartGame:

        break;

    case GameState::StartLevel:
        break;

    case GameState::Game:
    {
        m_scoreText->Create(
            renderer,
            "Score - " + to_string(m_score),
            { 1.0f, 1.0f, 1.0f });

        m_scoreText->Draw(renderer, 30.0f, 30.0f);
    }
        break;

    case GameState::GameOver:
        if (m_scene->GetName() == "win") {
            m_gameOverText->Create(
                Engine::Get().GetRenderer(),
                "You Win!",
                Color{ 1.0f, 1.0f, 1.0f });
        } else {
            m_gameOverText->Create(
                Engine::Get().GetRenderer(),
                "Game Over",
                Color{ 1.0f, 0.0f, 0.0f });
        }

        m_gameOverText->Draw(renderer, 550.0f, 400.0f);

        m_scoreText->Create(
            renderer,
            "Score - " + to_string(m_score),
            { 1.0f, 1.0f, 1.0f });

        m_scoreText->Draw(renderer, 600.0f, 500.0f);
        break;

    default:
        break;
    }

    //renderer.EnableCamera();

    
}

void SpriteGame::OnPlayerDead()
{
    if (m_health <= 0)
    {
        m_gameState = GameState::GameOver;
    }
}

void SpriteGame::NextLevel()
{
    if (m_scene->GetName() == "level1") {
        m_scene->SetName("level2");
        m_gameState = GameState::StartLevel;
    }
    else if (m_scene->GetName() == "level2") {
        m_scene->SetName("win");
        m_gameState = GameState::GameOver;
    }
}


void SpriteGame::SpawnPlayer()
{
        if (m_scene->GetName() == "level1") {
            auto actor = Factory::Instance().Create<Actor>("PlayerPrototype");
            actor->SetPosition(Vector2{ 100.0f, 600.0f });
            m_scene->AddActor(move(actor));
        }
        else if (m_scene->GetName() == "level2") {
            auto actor = Factory::Instance().Create<Actor>("PlayerPrototype");
            actor->SetPosition(Vector2{ 100.0f, 600.0f });
            m_scene->AddActor(move(actor));
        }
}

void SpriteGame::SpawnEnemy()
{
    if (m_scene->GetName() == "level1") {
        auto actor = Factory::Instance().Create<Actor>("EnemyPrototype");
        actor->SetPosition(Vector2{ 750.0f, 500.0f });
        m_scene->AddActor(move(actor));

        auto actor2 = Factory::Instance().Create<Actor>("EnemyPrototype");
        actor2->SetPosition(Vector2{ 500.0f, 200.0f });
        m_scene->AddActor(move(actor2));

        auto actor3 = Factory::Instance().Create<Actor>("EnemyPrototype");
        actor3->SetPosition(Vector2{ 500.0f, 500.0f });
        m_scene->AddActor(move(actor3));

    }
    else if (m_scene->GetName() == "level2") {
        auto actor = Factory::Instance().Create<Actor>("EnemyPrototype");
        actor->SetPosition(Vector2{ 500.0f, 500.0f });
        m_scene->AddActor(move(actor));

        auto actor2 = Factory::Instance().Create<Actor>("EnemyPrototype");
        actor2->SetPosition(Vector2{ 1200.0f, 812.0f });
        m_scene->AddActor(move(actor2));

        auto actor3 = Factory::Instance().Create<Actor>("EnemyPrototype");
        actor3->SetPosition(Vector2{ 1250.0f, 100.0f });
        m_scene->AddActor(move(actor3));
    }
}
