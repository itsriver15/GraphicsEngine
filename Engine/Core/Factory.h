#pragma once
#include <memory>
#include <map>
#include <iostream>

#include "../Framework/Object.h"
#include "../Core/Singleton.h"
#include "../Core/StringUtils.h"

#define FACTORY_REGISTER(classname) class Register##classname { public: Register##classname() { nu::Factory::Instance().Register<classname>(#classname); } }; static Register##classname registerInstance;


namespace nu
{
    class ICreator
    {
    public:
        virtual ~ICreator() = default;
        virtual std::unique_ptr<Object> Create() = 0;
    };

    template <typename T>
        requires std::derived_from<T, Object>
    class Creator : public ICreator
    {
    public:
        std::unique_ptr<Object> Create() override { return std::make_unique<T>(); }
    };

    template <typename T>
        requires std::derived_from<T, Object>
    class PrototypeCreator : public ICreator
    {
    public:
        PrototypeCreator(std::unique_ptr<Object> prototype)
            : m_prototype{ std::move(prototype) }
        {
        }

        std::unique_ptr<Object> Create() override
        {
            return m_prototype->Clone();
        }

    private:
        std::unique_ptr<Object> m_prototype;
    };

    class Factory : public Singleton<Factory> {
    public:
        template <typename T>
            requires std::derived_from<T, Object>
        void Register(const std::string& name);

        template <typename T>
            requires std::derived_from<T, Object>
        void RegisterPrototype(const std::string& name, std::unique_ptr<Object> prototype);

        template <typename T = class Object>
            requires std::derived_from<T, Object>
        std::unique_ptr<T> Create(const std::string& name);

    private:
        std::map<std::string, std::unique_ptr<ICreator>> m_registry;
    };

    // --- DEFINITIONS ---
    template<typename T>
        requires std::derived_from<T, Object>
    inline void Factory::Register(const std::string& name)
    {
        std::string lowerName = ToLower(name);
        if (m_registry.contains(lowerName)) {
            std::cerr << "Object [" << lowerName << "] already registered" << std::endl;
            return;
        }

        std::cout << "Object registered: " << name << std::endl;

        m_registry[lowerName] = std::make_unique<Creator<T>>();
    }

    template <typename T>
        requires std::derived_from<T, Object>
    inline void Factory::RegisterPrototype(
        const std::string& name,
        std::unique_ptr<Object> prototype)
    {
        std::string lowerName = ToLower(name);

        if (m_registry.contains(lowerName))
        {
            std::cerr << "Object [" << lowerName
                << "] already registered" << std::endl;
            return;
        }

        m_registry[lowerName] =
            std::make_unique<PrototypeCreator<T>>(std::move(prototype));
    }

    template<typename T>
        requires std::derived_from<T, Object>
    inline std::unique_ptr<T> Factory::Create(const std::string& name)
    {
        std::string lowerName = ToLower(name);
        if (!m_registry.contains(lowerName)) {
            std::cerr << "Object [" << lowerName << "] not registered, cannot create" << std::endl;
            return std::unique_ptr<T>();
        }

        auto iter = m_registry.find(lowerName);
        // create unique ptr to object
        auto object = iter->second->Create();

        // check if object is derived from T
        T* derived = dynamic_cast<T*>(object.get());
        if (derived) {
            // release unique ptr ownership
            object.release();

            // create new unique ptr with derived ptr
            return std::unique_ptr<T>(derived);
        }
        else {
            std::cerr << "Object [" << lowerName << "] not derived from specified type" << std::endl;
        }

        return std::unique_ptr<T>();
    }
}



