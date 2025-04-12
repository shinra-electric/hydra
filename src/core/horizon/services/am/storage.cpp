#include "core/horizon/services/am/storage.hpp"

#include "core/horizon/services/am/storage_accessor.hpp"

namespace Hydra::Horizon::Services::Am {

DEFINE_SERVICE_COMMAND_TABLE(IStorage, 0, Open)

void IStorage::Open(REQUEST_COMMAND_PARAMS) {
    add_service(new IStorageAccessor(data));
}

} // namespace Hydra::Horizon::Services::Am
