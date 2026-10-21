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


    /*auto vb = std::make_shared<VertexBuffer>();
    vb->Create<Vertex>(vertices, Engine::Get().GetRenderer());

    auto ib = std::make_shared<IndexBuffer>();
	ib->Create((uint32_t)indices.size(), indices.data(), Engine::Get().GetRenderer());

    auto vshader = Resources().Get<nu::Shader>("shaders/unlit.vert", Engine::Get().GetRenderer(), 1);
    auto fshader = Resources().Get<nu::Shader>("shaders/unlit.frag", Engine::Get().GetRenderer(), 1);

    auto pipeline = std::make_shared<Pipeline>();
    pipeline->AddVertexBuffer(sizeof(Vertex));
    pipeline->AddVertexAttribute(
        0,
        SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3,
        offsetof(Vertex, position));

    pipeline->AddVertexAttribute(
        1,
        SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2,
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
    }*/
    auto model = Resources().Get<Model>("models/spot.obj", Engine::Get().GetRenderer());
    auto texture = Resources().Get<Texture>("textures/spot_diffuse.png", Engine::Get().GetRenderer());

    auto vshader = Resources().Get<nu::Shader>("shaders/unlit.vert", Engine::Get().GetRenderer(), 1);
    auto fshader = Resources().Get<nu::Shader>("shaders/unlit.frag", Engine::Get().GetRenderer(), 1, 1);

    auto pipeline = std::make_shared<Pipeline>();

    pipeline->AddVertexBuffer(sizeof(Model::Vertex));
    pipeline->AddVertexAttribute(0, SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3, offsetof(Model::Vertex, position));
    pipeline->AddVertexAttribute(1, SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2, offsetof(Model::Vertex, texcoord));
    pipeline->AddVertexAttribute(2, SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3, offsetof(Model::Vertex, normal));
    pipeline->AddVertexAttribute(3, SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3, offsetof(Model::Vertex, tangent));

    pipeline->Create(*vshader.get(), *fshader.get(),
        Engine::Get().GetRenderer().GetGPUDevice(),
        Engine::Get().GetRenderer().GetWindow());

    Color4 color{ 0,0,0,1 };
    struct vertexUniform
    {
        glm::mat4 mvp;
    };

    Transform transform;

    glm::vec3 cameraPosition{ 0.0f, 0.0f, 5.0f };
    glm::vec3 direction{ 0.0f };

    float cameraSpeed = 5.0f;

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
        //MOVEMENT
        direction = glm::vec3(0.0f);

        if (Engine::Get().GetInput().GetKeyDown(SDL_SCANCODE_A)) direction.x = -1;
        if (Engine::Get().GetInput().GetKeyDown(SDL_SCANCODE_D)) direction.x = +1;
        if (Engine::Get().GetInput().GetKeyDown(SDL_SCANCODE_Q)) direction.y = -1;
        if (Engine::Get().GetInput().GetKeyDown(SDL_SCANCODE_E)) direction.y = +1;
        if (Engine::Get().GetInput().GetKeyDown(SDL_SCANCODE_W)) direction.z = -1;
        if (Engine::Get().GetInput().GetKeyDown(SDL_SCANCODE_S)) direction.z = +1;

        cameraPosition += direction * cameraSpeed * dt;

        //TRANSFORM
        
        transform.rotation.y = time * 90;
		//VIEW MATRIX
        auto view = glm::lookAtRH(cameraPosition, cameraPosition + glm::vec3{ 0.0f, 0.0f, -1.0f }, glm::vec3{ 0.0f, 1.0f, 0.0f });

        int width = 1;
        int height = 1;

		SDL_GetWindowSize(Engine::Get().GetRenderer().GetWindow(), &width, &height);
        
		if (height < 1) height = 1;

		//PROJECTION MATRIX
		float aspectRatio = static_cast<float>(width) / static_cast<float>(height);

		auto projection = glm::perspectiveRH(glm::radians(45.0f), aspectRatio, 0.1f, 100.0f);

		uniform.mvp = projection * view * transform.GetMatrix();
        

        // RENDER
        //Engine::Get().GetRenderer().BeginFrame();

        //Engine::Get().GetRenderer().SetPipeline(*pipeline);
        //Engine::Get().GetRenderer().SetVertexBuffer(*vb);
        //Engine::Get().GetRenderer().SetVertexUniform(0, &uniform, sizeof(uniform));
        //Engine::Get().GetRenderer().SetIndexBuffer(*ib);
        ////Engine::Get().GetRenderer().Draw(vb->GetVertexCount());
        //Engine::Get().GetRenderer().DrawIndex(*ib);

        //Engine::Get().GetRenderer().EndFrame();

        Engine::Get().GetRenderer().BeginFrame();

        Engine::Get().GetRenderer().SetPipeline(*pipeline);
        Engine::Get().GetRenderer().SetVertexUniform(0, &uniform, sizeof(uniform));
        Engine::Get().GetRenderer().SetFragmentTexture(0, *texture);
        //Engine::Get().GetRenderer().SetVertexUniform(0, &color, sizeof(color));

        model->Draw(Engine::Get().GetRenderer());

        Engine::Get().GetRenderer().EndFrame();
       
    }
    //SHUTDOWN

    Engine::Get().Shutdown();

    return 0;
}