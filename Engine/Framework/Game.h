#pragma once
#include "../Framework/Scene.h"
#include <memory>

namespace nu {

    class Scene;

    class Game {
    public:
        Game();
        Game(unique_ptr<Scene> scene);
        virtual ~Game();

        virtual bool Initialize() { return true; }
        virtual void Shutdown() {}

        virtual void Update(float dt);
        virtual void Draw(Renderer& renderer);

        void SetScene(unique_ptr<Scene> scene);

    protected:
        unique_ptr<Scene> m_scene;
    };

} 