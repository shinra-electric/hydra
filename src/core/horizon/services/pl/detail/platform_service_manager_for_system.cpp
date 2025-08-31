#include "core/horizon/services/pl/detail/platform_service_manager_for_system.hpp"

namespace hydra::horizon::services::pl::detail {

DEFINE_SERVICE_COMMAND_TABLE(IPlatformServiceManagerForSystem, 0, RequestLoad,
                             1, GetLoadState, 2, GetSize, 3,
                             GetSharedMemoryAddressOffset, 4,
                             GetSharedMemoryNativeHandle, 5,
                             GetSharedFontInOrderOfPriority)

} // namespace hydra::horizon::services::pl::detail
