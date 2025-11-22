//
// Created by igrom on 22.11.2025.
//

#pragma once


namespace LEN {
    class GameObject;

    class Component {
    public:
        virtual void Update(float deltaTime) = 0;

        virtual ~Component() = default;

        virtual size_t GetTypeId() const = 0;

        GameObject *GetOwner();

        template<typename T>
        static size_t StaticTypeId() {
            static size_t typeId = nextId++;
            return typeId;
        }

    protected:
        GameObject *m_owner = nullptr;

        friend class GameObject;

    private:
        static size_t nextId;
    };

#define COMPONENT(ComponentClass) \
    public: \
        static  size_t TypeId() {return Component::StaticTypeId<ComponentClass>();} \
        size_t GetTypeId() const override {return TypeId();}
} // namespace LEN
