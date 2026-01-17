#pragma once

#include "core/horizon/kernel/const.hpp"
#include "core/horizon/services/hid/internal/applet_resource.hpp"

namespace hydra::horizon::services::hid::internal {

class ResourceManager {
  public:
    enum class Error {
        InvalidAruid,
        AruidAlreadyTaken,
    };

    void SetupNpads();
    void Update();

    AppletResource& CreateResource(kernel::AppletResourceUserId aruid) {
        auto& resource = GetResourceOpt(aruid);
        if (resource.has_value())
            throw Error::AruidAlreadyTaken;

        resource.emplace();
        return *resource;
    }

    void DestroyResource(kernel::AppletResourceUserId aruid) {
        auto& resource = GetResourceOpt(aruid);
        if (!resource.has_value())
            throw Error::InvalidAruid;

        resource = std::nullopt;
    }

    AppletResource& GetResource(kernel::AppletResourceUserId aruid) {
        auto& resource = GetResourceOpt(aruid);
        if (!resource.has_value())
            throw Error::InvalidAruid;

        return *resource;
    }

  private:
    std::array<std::optional<AppletResource>, kernel::MAX_APPLET_RESOURCES>
        resources;

    // Helpers
    std::optional<AppletResource>&
    GetResourceOpt(kernel::AppletResourceUserId aruid) {
        if (aruid >= kernel::MAX_APPLET_RESOURCES)
            throw Error::InvalidAruid;

        return resources[aruid];
    }
};

} // namespace hydra::horizon::services::hid::internal
