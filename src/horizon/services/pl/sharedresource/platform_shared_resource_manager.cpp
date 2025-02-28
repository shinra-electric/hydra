#include "horizon/services/pl/sharedresource/platform_shared_resource_manager.hpp"
#include "horizon/kernel.hpp"

namespace Hydra::Horizon::Services::Pl::SharedResource {

DEFINE_SERVICE_COMMAND_TABLE(IPlatformSharedResourceManager, 4,
                             GetSharedMemoryNativeHandle)

IPlatformSharedResourceManager::IPlatformSharedResourceManager() {
    shared_memory_handle = Kernel::GetInstance().CreateSharedMemory();
}

void IPlatformSharedResourceManager::GetSharedMemoryNativeHandle(
    REQUEST_COMMAND_PARAMS) {
    writers.copy_handles_writer.Write(shared_memory_handle);
}

} // namespace Hydra::Horizon::Services::Pl::SharedResource
