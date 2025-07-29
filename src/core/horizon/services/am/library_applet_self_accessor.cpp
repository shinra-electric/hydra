#include "core/horizon/services/am/library_applet_self_accessor.hpp"

#include "core/horizon/kernel/process.hpp"
#include "core/horizon/os.hpp"
#include "core/horizon/services/am/library_applet_controller.hpp"

#define CONTROLLER OS_INSTANCE.GetLibraryAppletSelfController()

namespace hydra::horizon::services::am {

DEFINE_SERVICE_COMMAND_TABLE(ILibraryAppletSelfAccessor, 0, PopInData, 1,
                             PushOutData, 2, PopInteractiveInData, 3,
                             PushInteractiveOutData, 10, ExitProcessAndReturn,
                             11, GetLibraryAppletInfo, 14,
                             GetCallerAppletIdentityInfo)

ILibraryAppletSelfAccessor::ILibraryAppletSelfAccessor() {
    // Verify that we have a library applet self controller available
    ASSERT(CONTROLLER, Services, "No library applet self controller");
}

result_t ILibraryAppletSelfAccessor::PopInData(RequestContext* ctx) {
    AddService(*ctx, CONTROLLER->PopInData());
    return RESULT_SUCCESS;
}

result_t ILibraryAppletSelfAccessor::PushOutData(IService* storage_) {
    auto storage = dynamic_cast<IStorage*>(storage_);
    ASSERT_DEBUG(storage, Services, "Storage is not of type IStorage");

    CONTROLLER->PushOutData(storage);
    return RESULT_SUCCESS;
}

result_t ILibraryAppletSelfAccessor::PopInteractiveInData(RequestContext* ctx) {
    AddService(*ctx, CONTROLLER->PopInteractiveInData());
    return RESULT_SUCCESS;
}

result_t
ILibraryAppletSelfAccessor::PushInteractiveOutData(IService* storage_) {
    auto storage = dynamic_cast<IStorage*>(storage_);
    ASSERT_DEBUG(storage, Services, "Storage is not of type IStorage");

    CONTROLLER->PushInteractiveOutData(storage);
    return RESULT_SUCCESS;
}

result_t
ILibraryAppletSelfAccessor::ExitProcessAndReturn(kernel::Process* process) {
    // TODO: correct?
    process->Stop();

    return RESULT_SUCCESS;
}

result_t
ILibraryAppletSelfAccessor::GetLibraryAppletInfo(LibraryAppletInfo* out_info) {
    LOG_FUNC_STUBBED(Services);

    // HACK: hardcoded for Mii Edit
    *out_info = {
        .id = AppletId::LibraryAppletMiiEdit,
        .mode = LibraryAppletMode::AllForeground,
    };
    return RESULT_SUCCESS;
}

result_t ILibraryAppletSelfAccessor::GetCallerAppletIdentityInfo(
    AppletIdentityInfo* out_info) {
    // TODO: don't hardcode
    *out_info = {
        .id = AppletId::None,
        .application_id = 0x1,
    };
    return RESULT_SUCCESS;
}

} // namespace hydra::horizon::services::am
