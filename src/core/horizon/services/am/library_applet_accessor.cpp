#include "core/horizon/services/am/library_applet_accessor.hpp"

#include "core/horizon/services/am/storage.hpp"

namespace Hydra::Horizon::Services::Am {

DEFINE_SERVICE_COMMAND_TABLE(ILibraryAppletAccessor, 0,
                             GetAppletStateChangedEvent, 10, Start, 100,
                             PushInData)

ILibraryAppletAccessor::ILibraryAppletAccessor(AppletId id_,
                                               LibraryAppletMode mode_)
    : id{id_}, mode{mode_}, state_changed_event(new Kernel::Event()) {}

void ILibraryAppletAccessor::GetAppletStateChangedEvent(
    REQUEST_COMMAND_PARAMS) {
    writers.copy_handles_writer.Write(state_changed_event.id);
}

void ILibraryAppletAccessor::Start(REQUEST_COMMAND_PARAMS) {
    // TODO: implement
    LOG_FUNC_NOT_IMPLEMENTED(HorizonServices);
}

void ILibraryAppletAccessor::PushInData(REQUEST_COMMAND_PARAMS) {
    const auto storage_handle = readers.objects_reader->Read<handle_id_t>();
    auto storage = dynamic_cast<IStorage*>(get_service(storage_handle));
    ASSERT_DEBUG(storage, HorizonServices,
                 "Storage handle (0x{:x}) is invalid ", storage_handle);

    in_data.push(storage);
}

} // namespace Hydra::Horizon::Services::Am
