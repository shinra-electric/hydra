#pragma once

#include "horizon/services/service.hpp"

namespace Hydra::Horizon::Services::Am {

constexpr u32 APPLET_NO_MESSAGE = 0x680;

class ICommonStateGetter : public ServiceBase {
  public:
    void Request(Readers& readers, Writers& writers,
                 std::function<void(ServiceBase*)> add_service) override;

  private:
    // Requests
    void ReceiveMessage(Writers& writers);
    void GetCurrentFocusState(Writers& writers);
};

} // namespace Hydra::Horizon::Services::Am
