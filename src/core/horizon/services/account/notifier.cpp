#include "core/horizon/services/account/notifier.hpp"

#include "core/horizon/kernel/event.hpp"
#include "core/horizon/kernel/process.hpp"

namespace hydra::horizon::services::account {

DEFINE_SERVICE_COMMAND_TABLE(INotifier, 0, GetSystemEvent)

INotifier::INotifier()
    : event{new kernel::Event(false, "account::INotifier event")} {}

result_t INotifier::GetSystemEvent(kernel::Process* process,
                                   OutHandle<HandleAttr::Copy> out_handle) {
    out_handle = process->AddHandle(event);
    return RESULT_SUCCESS;
}

} // namespace hydra::horizon::services::account
