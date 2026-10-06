#pragma once
#include "Framework/Game.h"
#include "Renderer/Text.h"
#include "Resources/Resource.h"


using namespace nu;


class SpriteGame : public nu::Game {
public:
	enum class GameState {
		Title,
		StartGame,
		StartLevel,
		Game,
		GameOver
	};
public:
		SpriteGame() = default;
		SpriteGame(unique_ptr<Scene> scene) : nu::Game{ std::move(scene)} {};
		bool Initialize() override;

		void Update(float dt) override;
		void Draw(Renderer& renderer) override;

		void OnPlayerDead();

		void AddPoints(int points) {
			m_score += points;
		};

		void NextLevel();

	private:
		void SpawnPlayer();
		void SpawnEnemy();

	private:
		int m_score{ 0 };
		float m_health{ 0 };

		float m_stateTimer = 0.0f;

		float m_spawnTimer = 0.0f;
		float m_spawnTime = 3.0f;

		GameState m_gameState = GameState::Title;

		nu::res_t<nu::Font> m_titleFont;
		nu::res_t<nu::Font> m_gameFont;
		nu::res_t<nu::Font> m_gameOverFont;

		//Font* m_titleFont{ nullptr };
		Text* m_titleText{ nullptr };

		//Font* m_gameOverFont{ nullptr };
		Text* m_gameOverText{ nullptr };

		//Font* m_gameFont{ nullptr };

		Text* m_scoreText{ nullptr };
};