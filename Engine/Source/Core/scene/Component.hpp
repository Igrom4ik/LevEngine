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

        GameObject* GetOwner();

    protected:
        GameObject* m_owner = nullptr;

        friend class GameObject;
    };

}

