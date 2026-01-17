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
        ASSERT_THROWING(!resource.has_value(), Services,
                        Error::AruidAlreadyTaken, "Aruid {:#x} already taken",
                        aruid);
        resource.emplace();
        return *resource;
    }

    void DestroyResource(kernel::AppletResourceUserId aruid) {
        auto& resource = GetResourceOpt(aruid);
        ASSERT_THROWING(resource.has_value(), Services, Error::InvalidAruid,
                        "Invalid aruid {:#x}", aruid);
        resource = std::nullopt;
    }

    AppletResource& GetResource(kernel::AppletResourceUserId aruid) {
        auto& resource = GetResourceOpt(aruid);
        ASSERT_THROWING(resource.has_value(), Services, Error::InvalidAruid,
                        "Invalid aruid {:#x}", aruid);
        return *resource;
    }

  private:
    std::array<std::optional<AppletResource>, kernel::MAX_APPLET_RESOURCES>
        resources = {std::nullopt};

    // Helpers
    std::optional<AppletResource>&
    GetResourceOpt(kernel::AppletResourceUserId aruid) {
        ASSERT_THROWING(aruid < kernel::MAX_APPLET_RESOURCES, Services,
                        Error::InvalidAruid, "Invalid aruid {:#x}", aruid);
        return resources[aruid];
    }
};

} // namespace hydra::horizon::services::hid::internal
