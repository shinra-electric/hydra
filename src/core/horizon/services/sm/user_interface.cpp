#include "core/horizon/services/sm/user_interface.hpp"

#include "core/horizon/kernel/process.hpp"
#include "core/horizon/os.hpp"

namespace hydra::horizon::services::sm {

DEFINE_SERVICE_COMMAND_TABLE(IUserInterface, 0, RegisterClient, 1,
                             GetServiceHandle)

result_t
IUserInterface::GetServiceHandle(kernel::Process* process, u64 name,
                                 OutHandle<HandleAttr::Move> out_handle) {
    LOG_DEBUG(Services, "Service name: \"{}\"", u64_to_str(name));

    auto client_session = OS_INSTANCE.GetServiceManager().GetPort(name);
    if (!client_session) {
        LOG_WARN(Services, "Unknown service name \"{}\"", u64_to_str(name));
        return MAKE_RESULT(Svc, kernel::Error::NotFound); // TODO: module
    }

    // TODO: can it be domain?
    ASSERT_DEBUG(!IsDomain(), Services,
                 "sm::IUserInterface cannot be a domain service");
    out_handle = process->AddHandle(client_session);

    return RESULT_SUCCESS;
}

} // namespace hydra::horizon::services::sm
