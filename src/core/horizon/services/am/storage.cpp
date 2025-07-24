#include "core/horizon/services/am/storage.hpp"

#include "core/horizon/services/am/storage_accessor.hpp"

namespace hydra::horizon::services::am {

DEFINE_SERVICE_COMMAND_TABLE(IStorage, 0, Open)

result_t IStorage::Open(RequestContext* ctx) {
    AddService(*ctx, new IStorageAccessor(data));
    return RESULT_SUCCESS;
}

} // namespace hydra::horizon::services::am
