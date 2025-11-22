//
// Created by igrom on 22.11.2025.
//

#include "Component.hpp"

namespace LEN {

    size_t Component::nextId = 1;

    GameObject * Component::GetOwner() {
        return m_owner;
    }
}
