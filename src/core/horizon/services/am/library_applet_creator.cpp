#include "core/horizon/services/am/library_applet_creator.hpp"

#include "core/horizon/services/am/library_applet_accessor.hpp"
#include "core/horizon/services/am/storage.hpp"

namespace Hydra::Horizon::Services::Am {

namespace {

struct CreateLibraryAppletIn {
    AppletId id;
    LibraryAppletMode mode;
};

} // namespace

DEFINE_SERVICE_COMMAND_TABLE(ILibraryAppletCreator, 0, CreateLibraryApplet, 10,
                             CreateStorage)

void ILibraryAppletCreator::CreateLibraryApplet(REQUEST_COMMAND_PARAMS) {
    const auto in = readers.reader.Read<CreateLibraryAppletIn>();
    LOG_DEBUG(HorizonServices, "ID: {}, mode: {}", in.id, in.mode);

    add_service(new ILibraryAppletAccessor(in.id, in.mode));
}

void ILibraryAppletCreator::CreateStorage(REQUEST_COMMAND_PARAMS) {
    const auto size = readers.reader.Read<i64>();
    // TODO: ensure that the memory gets released
    add_service(new IStorage(sized_ptr(malloc(size), size)));
}

} // namespace Hydra::Horizon::Services::Am
