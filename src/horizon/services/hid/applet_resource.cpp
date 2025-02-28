#include "horizon/services/hid/applet_resource.hpp"
#include "horizon/kernel.hpp"

namespace Hydra::Horizon::Services::Hid {

DEFINE_SERVICE_COMMAND_TABLE(IAppletResource, 0, GetSharedMemoryHandle)

IAppletResource::IAppletResource() {
    // TODO: create once globally instead?
    shared_memory_handle = Kernel::GetInstance().CreateSharedMemory();
}

void IAppletResource::GetSharedMemoryHandle(REQUEST_COMMAND_PARAMS) {
    writers.copy_handles_writer.Write(shared_memory_handle);
}

} // namespace Hydra::Horizon::Services::Hid
