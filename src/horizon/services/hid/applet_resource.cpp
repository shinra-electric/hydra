#include "horizon/services/hid/applet_resource.hpp"

#include "horizon/os.hpp"

namespace Hydra::Horizon::Services::Hid {

DEFINE_SERVICE_COMMAND_TABLE(IAppletResource, 0, GetSharedMemoryHandle)

void IAppletResource::GetSharedMemoryHandle(REQUEST_COMMAND_PARAMS) {
    writers.copy_handles_writer.Write(OS::GetInstance().GetHidSharedMemoryId());
}

} // namespace Hydra::Horizon::Services::Hid
