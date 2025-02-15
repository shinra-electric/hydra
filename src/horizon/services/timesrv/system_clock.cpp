#include "horizon/services/timesrv/system_clock.hpp"

namespace Hydra::Horizon::Services::TimeSrv {

void ISystemClock::RequestImpl(Readers& readers, Writers& writers,
                               std::function<void(ServiceBase*)> add_service,
                               Result& result, u32 id) {
    LOG_WARNING(HorizonServices, "Unknown request {}", id);
}

} // namespace Hydra::Horizon::Services::TimeSrv
