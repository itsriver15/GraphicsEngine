#include "../Engine/Engine.h"
#include "SpaceGame/Player.h"
#include "SpaceGame/Enemy.h"
#include "Assets.h"

#include "SpaceGame/SpaceGame.h"
#include "SpriteGame/SpriteGame.h"

#include <iostream>
#include <random>
#include <vector>


using namespace nu;
using namespace std;



int main() {

    SetWorkingDirectory("Assets");



    //INITALIZE
    Engine::Get().Initialize();

    

    unique_ptr<Game> game = make_unique<SpriteGame>();
    game->Initialize();

 
    //MAIN LOOP
    bool quit = false;
    while (!quit)
    {
        SDL_Event event;

        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_EVENT_QUIT)
                quit = true;

            if (event.type == SDL_EVENT_KEY_DOWN &&
                event.key.scancode == SDL_SCANCODE_ESCAPE)
                quit = true;
        }
        //UPDATE
        //ENGINE
        Engine::Get().Update();

        float dt = Engine::Get().GetTime().GetDeltaTime();

        //GAME
        game->Update(dt);
        Engine::Get().GetPS().Update(Engine::Get().GetTime().GetDeltaTime());


        //RENDER
        Engine::Get().GetRenderer().SetColor(0.0f, 0.0f, 0.0f);
        Engine::Get().GetRenderer().Clear();



      
        game->Draw(Engine::Get().GetRenderer());
        

        Engine::Get().GetPS().Draw(Engine::Get().GetRenderer());
		

        Engine::Get().GetRenderer().Present();
       
    }
    //SHUTDOWN

    game.reset();

    Engine::Get().Shutdown();

    return 0;
}