#pragma once
#include "../Math/Transform.h"
#include "../Framework/Scene.h"
#include "../Renderer/Renderer.h"
#include "../Renderer/Model.h"
#include "../Resources/Resource.h"
#include "../Framework/Object.h"
#include "../Core/Factory.h"
#include <string>
#include <memory>
#include "Components/ColliderComponent.h"
#include "Components/Component.h"

namespace nu {
    class Texture;

    class Scene;

    struct ActorDesc {
        std::string name;
        std::string tag;
        Transform transform = Transform{ Vector2 {0.0f, 0.0f}, 0.0f, 0.0f };
        Vector2 velocity{ 0.0f,0.0f };
        float damping{ 0.0f };
        float lifespan{ 0 };
        res_t<Model> model;
        res_t<Texture> texture;

    };

    class Actor : public Object {
    public:
        Actor() = default;
        Actor(const ActorDesc& actorDesc) {
            m_tag = actorDesc.tag;
            m_transform = actorDesc.transform;
            m_velocity = actorDesc.velocity;
            m_damping = actorDesc.damping;
            m_lifespan = actorDesc.lifespan;
        }
        Actor(const Actor& other);

        CLASS_PROTOTYPE(Actor)


        virtual void Update(float dt);
        virtual void Draw(const Renderer& renderer) const;

        virtual void Start();
        virtual void Destroyed() const;

        virtual void OnCollision(Actor* other) {}

        const Transform& GetTransform() const { return m_transform; }
        void SetTransform(const Transform transform) {
            m_transform = transform;
        }
        void SetTransform(const Vector2& position, float rotation, float scale) {
            SetPosition(position);
            SetRotation(rotation);
            SetScale(scale);
        }
        void SetPosition(const Vector2& position) { m_transform.position = position; }
        void SetRotation(float rotation) { m_transform.rotation = rotation; }
        void SetScale(float scale) { m_transform.scale = scale; }

        const Vector2 GetVelocity() const { return m_velocity; }
        void SetVelocity(const Vector2& velocity) { m_velocity = velocity; }
        void AddVelocity(const Vector2& velocity) { m_velocity += velocity; }

        const std::string& GetName() const { return m_name; }
        const std::string& GetTag() const { return m_tag; };
        void SetTag(const std::string& tag) { m_tag = tag; }

        Scene* GetScene() const { return m_scene; }

        float GetRadius() const;


        void SetDestroyed(bool destroy = true) {
            m_destroyed = destroy;
        }

        bool GetDestroyed() const {
            return m_destroyed;
        }
       
        virtual void Read(const json::value_t& value) override;

        void AddComponent(std::unique_ptr<Component> component);

        bool GetPersistent() const { return m_persistent; }

        template<std::derived_from<Component> T>  
        T* GetComponent();


        friend Scene;

    protected:

        std::string m_tag;

        Transform m_transform = { {0.0f,0.0f}, 0.0f, 0.0f };
        Vector2 m_velocity{ 0.0f, 0.0f };
        float m_damping{ 0.0f };
        float m_lifespan{ 0 };
        bool m_destroyed{ false };
        bool m_persistent{ false };

        std::vector<std::unique_ptr<Component>> m_components;

        Scene* m_scene = nullptr;
    };

    template<std::derived_from<Component> T>
    T* Actor::GetComponent()
    {
        for (auto& component : m_components) {
            if (auto result = dynamic_cast<T*>(component.get())) {
                return result;
            }
        }
        return nullptr;
    }

   
};

