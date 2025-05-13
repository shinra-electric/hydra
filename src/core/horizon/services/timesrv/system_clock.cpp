#include "core/horizon/services/timesrv/system_clock.hpp"

namespace hydra::horizon::services::timesrv {

DEFINE_SERVICE_COMMAND_TABLE(ISystemClock, 0, GetCurrentTime, 2, GetSystemClockContext)

} // namespace hydra::horizon::services::timesrv
