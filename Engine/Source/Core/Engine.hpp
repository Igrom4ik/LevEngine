#pragma once
#include <string_view>
#include "EngineConfig.h"

namespace LevEngine
{
class Engine final
{
public:
    Engine();

    static constexpr std::string_view version() { return Engine_VERSION_STRING; }

    ~Engine() = default;
};
}  // namespace LevEngine
