#include "SpaceGame.h"
#include "Engine.h"
#include "Resources/ResourceManager.h"


#include <SDL3/SDL.h>

#include <iostream>
#include <memory>

using namespace std;
using namespace nu;

bool SpaceGame::Initialize()
{
    SetWorkingDirectory("SpaceGame");

    if (!Game::Initialize())
    {
        return false;
    }

    m_scene = make_unique<Scene>();
    m_scene->SetGame(this);
    m_scene->Load("Data/scene.json");

    m_titleText =
        new Text(
            Resources().GetWithID<Font>(
                "title_font",
                "8-bit-limit/8bitlim.ttf",
                100.0f));

    m_titleText->Create(
        Engine::Get().GetRenderer(),
        "Shoot Em Space",
        Color{ 1.0f, 1.0f, 1.0f });

    m_gameOverText =
        new Text(
            Resources().GetWithID<Font>(
                "title_font",
                "8-bit-limit/8bitlim.ttf",
                100.0f));

    m_gameOverText->Create(
        Engine::Get().GetRenderer(),
        "Game Over",
        Color{ 1.0f, 0.0f, 0.0f });

    m_scoreText =
        new Text(
            Resources().GetWithID<Font>(
                "game_font",
                "8-bit-limit/8bitlim.ttf",
                50.0f));

    m_livesText =
        new Text(
            Resources().GetWithID<Font>(
                "game_font",
                "8-bit-limit/8bitlim.ttf",
                50.0f));

    Engine::Get().GetAudio().AddSound("laser", "Audio/laser.wav");
    Engine::Get().GetAudio().AddSound("explosion", "Audio/explosion.wav");

    return true;
}

void SpaceGame::Update(float dt)
{
    switch (m_gameState)
    {
    case GameState::Title:
        if (Engine::Get().GetInput().GetKeyPressed(SDL_SCANCODE_SPACE))
        {
            m_gameState = GameState::StartGame;
        }
        break;

    case GameState::StartGame:
        m_score = 0;
        m_lives = 3;
        m_spawnTime = 3.0f;
        m_spawnTimer = 0.0f;

        m_gameState = GameState::StartLevel;
        break;

    case GameState::StartLevel:
        m_scene->RemoveAllActors();
        SpawnPlayer();

        m_gameState = GameState::Game;
        break;

    case GameState::Game:
        if (nu::Engine::Get().GetInput().GetKeyPressed(SDL_SCANCODE_SPACE)) {
            Engine::Get().GetAudio().PlaySound("laser");
        }
        m_spawnTimer -= dt;

        if (m_spawnTimer <= 0.0f)
        {
            m_spawnTimer = m_spawnTime;
            SpawnEnemy();
        }
        break;

    case GameState::GameOver:
        m_scene->RemoveAllActors();

        if (Engine::Get().GetInput().GetKeyPressed(SDL_SCANCODE_RETURN))
        {
            m_gameState = GameState::Title;
        }
        break;

    default:
        break;
    }

    Game::Update(dt);
}

void SpaceGame::Draw(Renderer& renderer)
{
    renderer.DrawTexture(
        *Resources().Get<Texture>(
            "textures/background.jpg",
            Engine::Get().GetRenderer()),
        600.0f,
        500.0f,
        0.0f,
        1.0f);

    switch (m_gameState)
    {
    case GameState::Title:
        m_titleText->Draw(renderer, 325.0f, 400.0f);
        break;

    case GameState::StartGame:
        break;

    case GameState::StartLevel:
        break;

    case GameState::Game:
        m_scoreText->Create(
            renderer,
            "Score - " + to_string(m_score),
            { 1.0f, 1.0f, 1.0f });

        m_livesText->Create(
            renderer,
            "Lives - " + to_string(m_lives),
            { 1.0f, 1.0f, 1.0f });

        m_scoreText->Draw(renderer, 30.0f, 30.0f);
        m_livesText->Draw(renderer, 30.0f, 80.0f);
        break;

    case GameState::GameOver:
        m_gameOverText->Draw(renderer, 450.0f, 400.0f);

        m_scoreText->Create(
            renderer,
            "Score - " + to_string(m_score),
            { 1.0f, 1.0f, 1.0f });

        m_scoreText->Draw(renderer, 550.0f, 500.0f);
        break;

    default:
        break;
    }

    Game::Draw(renderer);
}

void SpaceGame::OnPlayerDead()
{
    Engine::Get().GetAudio().PlaySound("explosion");

    m_lives--;

    if (m_lives <= 0)
    {
        m_gameState = GameState::GameOver;
    }
    else
    {
        m_gameState = GameState::StartLevel;
    }
}

void SpaceGame::SpawnPlayer()
{
        auto actor = Factory::Instance().Create<Actor>("PlayerPrototype");
        m_scene->AddActor(move(actor));
}

void SpaceGame::SpawnEnemy()
{
    auto actor = Factory::Instance().Create<Actor>("EnemyPrototype");
    actor->SetPosition({ nu::RandomFloat(1024.0f), nu::RandomFloat(1280.0f) });
    m_scene->AddActor(move(actor));

}