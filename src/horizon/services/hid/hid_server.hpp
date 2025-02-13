#pragma once

#include "horizon/services/service.hpp"

namespace Hydra::Horizon::Services::Hid {

class IHidServer : public ServiceBase {
  public:
    void Request(Writers& writers, Reader& reader,
                 std::function<void(ServiceBase*)> add_service) override;
};

} // namespace Hydra::Horizon::Services::Hid
