#include "../Engine/Engine.h"

#include <iostream>
#include <random>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


using namespace nu;
using namespace std;

struct Vertex
{
    float x, y, z;
};

std::vector<Vertex> vertices =
{
    Vertex{ -0.5f, 0.0f, 0.0f}, // Bottom-Left
    Vertex{  0.5f, -1.0f, 0.0f}, // Bottom-Right
    Vertex{  0.25f,  0.20f,0.0f}, // Top-Middle
};


int main() {

    SetWorkingDirectory("Assets");



    //INITALIZE
    Engine::Get().Initialize();


    auto vb = std::make_shared<VertexBuffer>();
    vb->Create<Vertex>(vertices, Engine::Get().GetRenderer().GetGPUDevice());

    auto vshader = Resources().Get<nu::Shader>("shaders/position.vert", Engine::Get().GetRenderer());
    auto fshader = Resources().Get<nu::Shader>("shaders/color.frag", Engine::Get().GetRenderer());

    auto pipeline = std::make_shared<Pipeline>();
    pipeline->AddVertexBuffer(sizeof(Vertex));
    pipeline->AddVertexAttribute(
        0,
        SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3,
        offsetof(Vertex, x));

    if (!pipeline->Create(
        *vshader,
        *fshader,
        Engine::Get().GetRenderer().GetGPUDevice(),
        Engine::Get().GetRenderer().GetWindow()))
    {
        std::cerr << "Pipeline creation failed; stopping.\n";
        Engine::Get().Shutdown();
        return -1;
    }

    struct vertexUniform
    {
        glm::mat4 model;
    };

    vertexUniform uniform;

    float rotationAngle = 0.0f;
 
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

        //GRAPHICS
        rotationAngle += dt * glm::radians(90.0f);

        uniform.model = glm::mat4(1.0f);
        uniform.model = glm::translate(uniform.model, glm::vec3(0.5f, 0.0f, 0.0f));
        uniform.model = glm::rotate(uniform.model, rotationAngle, glm::vec3(0.0f, 0.0f, 1.0f));
        uniform.model = glm::scale(uniform.model, glm::vec3(0.5f, 0.5f, 0.5f));

// RENDER
        Engine::Get().GetRenderer().BeginFrame();

        Engine::Get().GetRenderer().SetPipeline(*pipeline);
        Engine::Get().GetRenderer().SetVertexBuffer(*vb);
        Engine::Get().GetRenderer().SetVertexUniform(0, &uniform, sizeof(uniform));
        Engine::Get().GetRenderer().Draw(vb->GetVertexCount());

        Engine::Get().GetRenderer().EndFrame();
       
    }
    //SHUTDOWN

    Engine::Get().Shutdown();

    return 0;
}