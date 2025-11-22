//
// Created by igrom on 22.11.2025.
//

#include "Component.hpp"

namespace LEN {
    GameObject * Component::GetOwner() {
        return m_owner;
    }
}
