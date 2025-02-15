#include "horizon/services/timesrv/steady_clock.hpp"

namespace Hydra::Horizon::Services::TimeSrv {

void ISteadyClock::RequestImpl(Readers& readers, Writers& writers,
                               std::function<void(ServiceBase*)> add_service,
                               Result& result, u32 id) {
    LOG_WARNING(HorizonServices, "Unknown request {}", id);
}

} // namespace Hydra::Horizon::Services::TimeSrv
