#pragma once

#include "horizon/services/service.hpp"

namespace Hydra::Horizon::Services::HosBinder {

struct Binder {
    u32 weak_ref_count = 0;
    u32 strong_ref_count = 0;
};

class IHOSBinderDriver : public ServiceBase {
  public:
    void Request(Readers& readers, Writers& writers,
                 std::function<void(ServiceBase*)> add_service) override;

    u32 AddBinder() {
        binders[binder_count] = Binder{};
        return binder_count++;
    }

  private:
    Binder binders[0x100]; // TODO: what should be the max number of binders?
    u32 binder_count = 0;
};

} // namespace Hydra::Horizon::Services::HosBinder
