#include "../Engine/Engine.h"
#include "../Engine/Renderer/IndexBuffer.h"

#include <iostream>
#include <random>
#include <vector>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/transform.hpp>


using namespace nu;
using namespace std;


struct Color4
{
    float r, g, b, a;
};

struct Vertex
{
    Vector3 position;
    Color4 color;
};


std::vector<Vertex> vertices =
{
    Vertex{Vector3{ -0.5f, -0.5f, 0.0f}, Color4{1, 0, 0, 1}}, // Bottom-Left
    Vertex{Vector3{  0.5f, -0.5f, 0.0f}, Color4{0, 1, 0, 1}}, // Bottom-Right
    Vertex{Vector3{  -0.5f, 0.5f, 0.0f}, Color4{0, 0, 1, 1}}, // Top-Right
    Vertex{Vector3{  0.5f, 0.5f, 0.0f}, Color4{0, 1, 1, 1}}   // Top-Left
};

std::vector<uint32_t> indices = {
    0, 1, 3,  // First triangle
    0, 3, 2   // Second triangle
};


int main() {

    SetWorkingDirectory("Assets");



    //INITALIZE
    Engine::Get().Initialize();


    auto vb = std::make_shared<VertexBuffer>();
    vb->Create<Vertex>(vertices, Engine::Get().GetRenderer());

    auto ib = std::make_shared<IndexBuffer>();
	ib->Create(indices.size(), indices.data(), Engine::Get().GetRenderer());

    auto vshader = Resources().Get<nu::Shader>("shaders/position.vert", Engine::Get().GetRenderer());
    auto fshader = Resources().Get<nu::Shader>("shaders/color.frag", Engine::Get().GetRenderer());

    auto pipeline = std::make_shared<Pipeline>();
    pipeline->AddVertexBuffer(sizeof(Vertex));
    pipeline->AddVertexAttribute(
        0,
        SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3,
        offsetof(Vertex, position));

    pipeline->AddVertexAttribute(
        1,
        SDL_GPU_VERTEXELEMENTFORMAT_FLOAT4,
        offsetof(Vertex, color));

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
    Color4 color{ 0,0,0,1 };
    struct vertexUniform
    {
        glm::mat4 mvp;
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
		auto time = Engine::Get().GetTime().GetTime();

		color.r = sin(time * 10.0f) * 0.5f + 0.5f;

        //GRAPHICS
        rotationAngle += dt * glm::radians(90.0f);
		//MODEL MATRIX
        //1 0 0 0
		//0 1 0 0
		//0 0 1 0
		//0 0 0 1
        /*uniform.model = glm::mat4{ 1.0f };
        uniform.model = glm::translate(uniform.model, glm::vec3(0.5f, 0.0f, 0.0f));
        uniform.model = glm::rotate(uniform.model, rotationAngle, glm::vec3(0.0f, 0.0f, 1.0f));
        uniform.model = glm::scale(uniform.model, glm::vec3(0.5f, 0.5f, 0.5f));*/

		
        //1 0 0 x
        //0 1 0 y
        //0 0 1 z
        //0 0 0 1
        auto mxt = glm::translate(glm::vec3{ 0.0f, 0.0f, 0.0f });
        auto mxr = glm::eulerAngleYXZ(0.0f, 0.0f, time * 3.0f);
		
        //x 0 0 0
        //0 y 0 0
        //0 0 z 0
        //0 0 0 1
        auto mxs = glm::scale(glm::vec3(0.5f, 0.5f, 0.5f));
		auto model = mxt * mxr * mxs;

		//VIEW MATRIX
        auto view = glm::lookAtRH(glm::vec3{ 0,0,5 }, glm::vec3{ 0 }, glm::vec3{ 0,1,0 });

		//PROJECTION MATRIX
        uniform.mvp = model;

        // RENDER
        Engine::Get().GetRenderer().BeginFrame();

        Engine::Get().GetRenderer().SetPipeline(*pipeline);
        Engine::Get().GetRenderer().SetVertexBuffer(*vb);
        Engine::Get().GetRenderer().SetVertexUniform(0, &uniform, sizeof(uniform));
        Engine::Get().GetRenderer().SetIndexBuffer(*ib);
        //Engine::Get().GetRenderer().Draw(vb->GetVertexCount());
        Engine::Get().GetRenderer().DrawIndex(*ib);

        Engine::Get().GetRenderer().EndFrame();
       
    }
    //SHUTDOWN

    Engine::Get().Shutdown();

    return 0;
}