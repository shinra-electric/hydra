#pragma once

#include "horizon/services/service.hpp"

namespace Hydra::Horizon::Services::Hid {

class IHidServer : public ServiceBase {
  public:
    void Request(Readers& readers, Writers& writers,
                 std::function<void(ServiceBase*)> add_service) override;
};

} // namespace Hydra::Horizon::Services::Hid
