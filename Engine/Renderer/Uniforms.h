
#pragma once

#include <cstdint>
#include <glm/glm.hpp>

namespace nu
{
    constexpr uint32_t MAX_LIGHTS = 8;

    enum class LightType : uint32_t
    {
        Directional,
        Point,
        Spot
    };

    struct Light
    {
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

    struct LightUniform
    {
        Light lights[MAX_LIGHTS];
        uint32_t numLights;
        glm::vec3 ambient;
    };

    struct VertexUniform
    {
        glm::mat4 mvp;
        glm::mat4 model;
        glm::vec3 cameraPosition;
        float padding1;
    };

    struct MaterialUniform
    {
        glm::vec4 color;
        glm::vec2 tiling;
        glm::vec2 offset;
        float shininess;
        glm::vec3 padding;
    };

    // Make sure uniform structures meet the 16-byte size boundary.
#define CHECK_UNIFORM_BOUNDARY(T) \
        static_assert((sizeof(T) % 16) == 0)

    CHECK_UNIFORM_BOUNDARY(LightUniform);
    CHECK_UNIFORM_BOUNDARY(VertexUniform);
    CHECK_UNIFORM_BOUNDARY(MaterialUniform);
}
