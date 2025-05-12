#include "core/horizon/services/am/library_applet_accessor.hpp"

#include "core/horizon/services/am/storage.hpp"

namespace hydra::horizon::services::am {

DEFINE_SERVICE_COMMAND_TABLE(ILibraryAppletAccessor, 0,
                             GetAppletStateChangedEvent, 10, Start, 100,
                             PushInData)

ILibraryAppletAccessor::ILibraryAppletAccessor(AppletId id_,
                                               LibraryAppletMode mode_)
    : id{id_}, mode{mode_}, state_changed_event(new kernel::Event()) {}

result_t ILibraryAppletAccessor::GetAppletStateChangedEvent(
    OutHandle<HandleAttr::Copy> out_handle) {
    out_handle = state_changed_event.id;
    return RESULT_SUCCESS;
}

result_t ILibraryAppletAccessor::Start() {
    // TODO: implement
    LOG_FUNC_NOT_IMPLEMENTED(Services);
    return RESULT_SUCCESS;
}

result_t ILibraryAppletAccessor::PushInData(ServiceBase* storage_) {
    auto storage = dynamic_cast<IStorage*>(storage_);
    ASSERT_DEBUG(storage, Services, "Storage is not of type IStorage");

    in_data.push(storage);
    return RESULT_SUCCESS;
}

} // namespace hydra::horizon::services::am
