#include "core/horizon/services/hid/internal/resource_manager.hpp"

#include "core/input/device_manager.hpp"

namespace hydra::horizon::services::hid::internal {

void ResourceManager::SetupNpads() {
    for (auto& resource : resources) {
        if (!resource.has_value())
            continue;

        resource->SetupNpads();
    }
}

void ResourceManager::Update() {
    // Npads
    for (u32 i = 0; i < NPAD_COUNT; i++) {
        const auto index = static_cast<NpadIndex>(i);

        // Poll
        const auto state = INPUT_DEVICE_MANAGER_INSTANCE.PollNpad(index);

        // Update
        for (auto& resource : resources) {
            if (!resource.has_value())
                continue;

            resource->UpdateNpad(index, state);
        }
    }

    // Touch
    // TODO
}

} // namespace hydra::horizon::services::hid::internal
