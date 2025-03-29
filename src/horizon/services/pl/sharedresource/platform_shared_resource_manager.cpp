#include "horizon/services/pl/sharedresource/platform_shared_resource_manager.hpp"

namespace Hydra::Horizon::Services::Pl::SharedResource {

DEFINE_SERVICE_COMMAND_TABLE(IPlatformSharedResourceManager, 4,
                             GetSharedMemoryNativeHandle)

IPlatformSharedResourceManager::IPlatformSharedResourceManager() {
    shared_memory_handle_id =
        Kernel::GetInstance().CreateSharedMemory(0x01100000);
}

void IPlatformSharedResourceManager::GetSharedMemoryNativeHandle(
    REQUEST_COMMAND_PARAMS) {
    writers.copy_handles_writer.Write(shared_memory_handle_id);
}

} // namespace Hydra::Horizon::Services::Pl::SharedResource
