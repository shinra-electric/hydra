#pragma once

#include "horizon/services/service.hpp"

namespace Hydra::Horizon::Services::Am {

constexpr u32 APPLET_NO_MESSAGE = 0x680;

class ICommonStateGetter : public ServiceBase {
  public:
  protected:
    void RequestImpl(Readers& readers, Writers& writers,
                     std::function<void(ServiceBase*)> add_service,
                     Result& result, u32 id) override;

  private:
    // Requests
    void ReceiveMessage(Writers& writers);
    void GetCurrentFocusState(Writers& writers);
};

} // namespace Hydra::Horizon::Services::Am
