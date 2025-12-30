#include "core/horizon/services/psc/pm_module.hpp"

#include "core/horizon/kernel/event.hpp"
#include "core/horizon/kernel/process.hpp"

namespace hydra::horizon::services::psc {

DEFINE_SERVICE_COMMAND_TABLE(IPmModule, 0, Initialize)

IPmModule::IPmModule() : event{new kernel::Event(false, "IPmModule event")} {}

result_t IPmModule::Initialize(kernel::Process* process, PmModuleId module_id,
                               InBuffer<BufferAttr::MapAlias> in_dep_buffer,
                               OutHandle<HandleAttr::Copy> out_event_handle) {
    (void)in_dep_buffer;
    LOG_FUNC_STUBBED(Services);

    LOG_DEBUG(Services, "Module ID: {}", module_id);

    // TODO: what are the dependencies?

    out_event_handle = process->AddHandle(event);
    return RESULT_SUCCESS;
}

} // namespace hydra::horizon::services::psc
