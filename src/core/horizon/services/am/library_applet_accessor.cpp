#include "core/horizon/services/am/library_applet_accessor.hpp"

#include "core/horizon/services/am/storage.hpp"

namespace hydra::horizon::services::am {

namespace {

struct CommonArguments {
    u32 version; // Version of the struct
    u32 size;    // Size of the struct
    u32 library_applet_api_version;
    u32 theme_color;
    bool play_startup_sound;
    u64 system_tick;
};

} // namespace

DEFINE_SERVICE_COMMAND_TABLE(ILibraryAppletAccessor, 0,
                             GetAppletStateChangedEvent, 10, Start, 100,
                             PushInData)

// TODO: autoclear event?
ILibraryAppletAccessor::ILibraryAppletAccessor(AppletId id_,
                                               LibraryAppletMode mode_)
    : id{id_}, mode{mode_}, state_changed_event(new kernel::Event()) {}

result_t ILibraryAppletAccessor::GetAppletStateChangedEvent(
    OutHandle<HandleAttr::Copy> out_handle) {
    out_handle = state_changed_event.id;
    return RESULT_SUCCESS;
}

result_t ILibraryAppletAccessor::Start() {
    const auto common_args = PopInData<CommonArguments>();
    ASSERT(common_args.version == 1, Services, "Unsupported version {}",
           common_args.version); // TODO: support version 0
    ASSERT(common_args.size == sizeof(CommonArguments), Services,
           "Invalid struct size 0x{:x}", common_args.size);

    switch (id) {
    // case AppletId::TODO:
    //     TODO;
    //     break;
    default:
        LOG_NOT_IMPLEMENTED(Services, "Applet ID {}", id);
        break;
    }

    return RESULT_SUCCESS;
}

result_t ILibraryAppletAccessor::PushInData(ServiceBase* storage_) {
    auto storage = dynamic_cast<IStorage*>(storage_);
    ASSERT_DEBUG(storage, Services, "Storage is not of type IStorage");

    in_data.push(storage->GetData());
    return RESULT_SUCCESS;
}

} // namespace hydra::horizon::services::am
