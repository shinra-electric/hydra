#include "core/horizon/services/am/library_applet_creator.hpp"

#include "core/horizon/services/am/storage.hpp"

namespace Hydra::Horizon::Services::Am {

DEFINE_SERVICE_COMMAND_TABLE(ILibraryAppletCreator, 0, CreateLibraryApplet, 10,
                             CreateStorage)

result_t ILibraryAppletCreator::CreateLibraryApplet(
    add_service_fn_t add_service, AppletId id, LibraryAppletMode mode) {
    LOG_DEBUG(HorizonServices, "ID: {}, mode: {}", id, mode);

    add_service(new ILibraryAppletAccessor(id, mode));
    return RESULT_SUCCESS;
}

result_t ILibraryAppletCreator::CreateStorage(add_service_fn_t add_service,
                                              i64 size) {
    // TODO: ensure that the memory gets released
    add_service(new IStorage(sized_ptr(malloc(size), size)));
    return RESULT_SUCCESS;
}

} // namespace Hydra::Horizon::Services::Am
