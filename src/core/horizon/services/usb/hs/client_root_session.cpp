#include "core/horizon/services/usb/hs/client_root_session.hpp"

#include "core/horizon/kernel/event.hpp"
#include "core/horizon/kernel/process.hpp"

namespace hydra::horizon::services::usb::hs {

DEFINE_SERVICE_COMMAND_TABLE(IClientRootSession, 0, BindClientProcess, 4,
                             CreateInterfaceAvailableEvent, 6,
                             GetInterfaceStateChangeEvent)

IClientRootSession::IClientRootSession()
    : interface_available_event{new kernel::Event()},
      interface_state_change_event{new kernel::Event()} {}

result_t IClientRootSession::BindClientProcess() {
    LOG_FUNC_STUBBED(Services);
    return RESULT_SUCCESS;
}

result_t IClientRootSession::CreateInterfaceAvailableEvent(
    kernel::Process* process, aligned<u8, 2> index, DeviceFilter device_filter,
    OutHandle<HandleAttr::Copy> out_handle) {
    LOG_FUNC_STUBBED(Services);

    out_handle = process->AddHandle(interface_available_event);
    return RESULT_SUCCESS;
}

result_t IClientRootSession::GetInterfaceStateChangeEvent(
    kernel::Process* process, OutHandle<HandleAttr::Copy> out_handle) {
    LOG_FUNC_STUBBED(Services);

    out_handle = process->AddHandle(interface_state_change_event);
    return RESULT_SUCCESS;
}

} // namespace hydra::horizon::services::usb::hs
