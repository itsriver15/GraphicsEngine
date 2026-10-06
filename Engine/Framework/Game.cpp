#include "pch.h"
#include "Framework/Game.h"
#include "Framework/Scene.h"
namespace nu {
	Game::Game() = default;
	Game::Game(unique_ptr<Scene> scene) {
		m_scene = std::move(scene);
	}
	Game::~Game() = default;


	void Game::Update(float dt) {
		m_scene->Update(dt);
	}

	void Game::Draw(Renderer& renderer) {
		m_scene->Draw(renderer);
	}

	void Game::SetScene(unique_ptr<Scene> scene)
	{
		m_scene = std::move(scene);
	}

}