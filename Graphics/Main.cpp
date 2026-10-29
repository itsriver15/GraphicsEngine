#include "../Engine/Engine.h"
#include "../Engine/Renderer/IndexBuffer.h"

#include <iostream>
#include <random>
#include <vector>


#define CHECK_UNIFORM_BOUNDARY(T) static_assert((sizeof(T) % 16) == 0);
#define MAX_LIGHTS 8

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/transform.hpp>



using namespace nu;
using namespace std;

enum class LightType : uint32_t {
	Directional,
	Point,
	Spot
};

struct Color4
{
	float r, g, b, a;
};

struct Vertex
{
	Vector3 position;
	Color4 color;
};

struct Light {
	glm::vec3 position;
	uint32_t type;
	glm::vec3 direction;
	float range;
	glm::vec3 color;
	float intensity;
	glm::vec3 ambient;
	float innerCutoff;
	float outerCutoff;
	glm::vec3 padding;
};

struct LightUniform {
	Light lights[MAX_LIGHTS];
	uint32_t numLights;
	glm::vec3 ambient;
};

Color4 color{ 1,1,1,1 };

struct VertexUniform
{
	glm::mat4 mvp;
	glm::mat4 model;
	glm::vec3 cameraPosition;
	float padding1;
};

VertexUniform vertexUniform;

struct MaterialUniform
{
	glm::vec4 color;
	glm::vec2 tiling;
	glm::vec2 offset;
	float shininess;
	glm::vec3 padding;
};


MaterialUniform materialUniform{
	.color{1,1,1,1},
	.tiling {1.0f, 1.0f},
	.offset {1.0f},
	.shininess{32.0f}
};

Light light1{
		.position = {2.0f, 2.0f, 3.0f},
		.type = uint32_t(LightType::Point),
		.direction = glm::vec3(0.0f, -1.0f, 0.0f),
		.range = {10.0f},
		.color = {1.0f, 0.0f, 1.0f},
		.intensity = 0.5f,
		.ambient = {0.3f, 0.3f, 0.3f},
		.innerCutoff {0.2f},
		.outerCutoff{2.0f}
};
Light light2{
		.position = {2.0f, 2.0f, 3.0f},
		.type = uint32_t(LightType::Point),
		.direction = glm::vec3(0.0f, -1.0f, 0.0f),
		.range = {10.0f},
		.color = {1.0f, 0.0f, 1.0f},
		.intensity = 0.5f,
		.ambient = {0.3f, 0.3f, 0.3f},
		.innerCutoff {0.2f},
		.outerCutoff{2.0f}
};


CHECK_UNIFORM_BOUNDARY(LightUniform)
CHECK_UNIFORM_BOUNDARY(VertexUniform)
CHECK_UNIFORM_BOUNDARY(MaterialUniform)


int main() {

	SetWorkingDirectory("Assets");

	//INITALIZE
	Engine::Get().Initialize();

	auto model = Resources().Get<Model>("models/spot.obj", Engine::Get().GetRenderer());
	auto texture = Resources().Get<Texture>("textures/spot_diffuse.png", Engine::Get().GetRenderer());

	auto vshader = Resources().Get<nu::Shader>("shaders/phong.vert", Engine::Get().GetRenderer(), 3);
	auto fshader = Resources().Get<nu::Shader>("shaders/phong.frag", Engine::Get().GetRenderer(), 3, 1);

	auto pipeline = std::make_shared<Pipeline>();

	pipeline->AddVertexBuffer(sizeof(Model::Vertex));
	pipeline->AddVertexAttribute(0, SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3, offsetof(Model::Vertex, position));
	pipeline->AddVertexAttribute(1, SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2, offsetof(Model::Vertex, texcoord));
	pipeline->AddVertexAttribute(2, SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3, offsetof(Model::Vertex, normal));
	pipeline->AddVertexAttribute(3, SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3, offsetof(Model::Vertex, tangent));

	pipeline->Create(*vshader.get(), *fshader.get(),
		Engine::Get().GetRenderer().GetGPUDevice(),
		Engine::Get().GetRenderer().GetWindow());

	VertexUniform vertexUniform;
	vertexUniform.cameraPosition = { 0.0f, 2.0f, 8.0f };
	
	Transform transform;
	transform.scale = glm::vec3{ 4.0f };

	LightUniform lightUniform;

	lightUniform.lights[0] = light1;
	//lightUniform.lights[1] = light2;
	lightUniform.numLights = 1;
	lightUniform.ambient = glm::vec3{ 0.2f };

	glm::vec3 direction{ 0.0f };

	float cameraSpeed = 5.0f;

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

		//MOVEMENT
		direction = glm::vec3(0.0f);

		if (Engine::Get().GetInput().GetKeyDown(SDL_SCANCODE_A)) direction.x = -1;
		if (Engine::Get().GetInput().GetKeyDown(SDL_SCANCODE_D)) direction.x = +1;
		if (Engine::Get().GetInput().GetKeyDown(SDL_SCANCODE_Q)) direction.y = -1;
		if (Engine::Get().GetInput().GetKeyDown(SDL_SCANCODE_E)) direction.y = +1;
		if (Engine::Get().GetInput().GetKeyDown(SDL_SCANCODE_W)) direction.z = -1;
		if (Engine::Get().GetInput().GetKeyDown(SDL_SCANCODE_S)) direction.z = +1;

		if (Engine::Get().GetInput().GetKeyDown(SDL_SCANCODE_1)) lightUniform.lights[0].type = 0;
		if (Engine::Get().GetInput().GetKeyDown(SDL_SCANCODE_2)) lightUniform.lights[0].type = 1;
		if (Engine::Get().GetInput().GetKeyDown(SDL_SCANCODE_3)) {
			lightUniform.lights[0].type = 2;
			lightUniform.lights[0].innerCutoff = glm::cos(glm::radians(10.5f)),
			lightUniform.lights[0].outerCutoff = glm::cos(glm::radians(100.5f));
			lightUniform.lights[0].direction = glm::normalize(glm::vec3(0.0f, -1.0f, -1.0f));

		}
		transform.rotation.y = 180.0f;
		vertexUniform.cameraPosition += direction * cameraSpeed * dt;

		lightUniform.lights[1].position = glm::eulerAngleY(time * 4) * glm::vec4{ 0, 0, 10, 1 };
		//lightUniform.lights[2].position = glm::eulerAngleX(time * 4) * glm::vec4{ 0, 0, 10, 1 };


		/*materialUniform.tiling = glm::vec2{ 2.0f };*/

		//lightUniform.lightIntensity = std::sin(time) * 0.5f * 0.5f;

		//TRANSFORM

		/*transform.rotation.y = time * 45.0f;*/
		//VIEW MATRIX
		auto view = glm::lookAtRH(vertexUniform.cameraPosition, vertexUniform.cameraPosition + glm::vec3{ 0.0f, 0.0f, -1.0f }, glm::vec3{ 0.0f, 1.0f, 0.0f });

		int width = 1;
		int height = 1;

		SDL_GetWindowSize(Engine::Get().GetRenderer().GetWindow(), &width, &height);

		if (height < 1) height = 1;

		//PROJECTION MATRIX
		float aspectRatio = static_cast<float>(width) / static_cast<float>(height);

		//FOV

		auto projection = glm::perspectiveRH(glm::radians(70.0f), aspectRatio, 0.1f, 100.0f);

		vertexUniform.model = transform.GetMatrix();
		vertexUniform.mvp = projection * view * vertexUniform.model;



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

		Engine::Get().GetRenderer().SetVertexUniform(0, &vertexUniform, sizeof(vertexUniform));
		Engine::Get().GetRenderer().SetVertexUniform(1, &lightUniform, sizeof(lightUniform));
		Engine::Get().GetRenderer().SetVertexUniform(2, &materialUniform, sizeof(materialUniform));

		Engine::Get().GetRenderer().SetFragmentUniform(0, &vertexUniform, sizeof(vertexUniform));
		Engine::Get().GetRenderer().SetFragmentUniform(1, &lightUniform, sizeof(lightUniform));
		Engine::Get().GetRenderer().SetFragmentUniform(2, &materialUniform, sizeof(materialUniform));

		Engine::Get().GetRenderer().SetFragmentTexture(0, *texture);

		model->Draw(Engine::Get().GetRenderer());

		Engine::Get().GetRenderer().EndFrame();

	}
	//SHUTDOWN

	Engine::Get().Shutdown();

	return 0;
}