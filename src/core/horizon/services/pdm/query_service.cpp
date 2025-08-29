#include "core/horizon/services/pdm/query_service.hpp"

namespace hydra::horizon::services::pdm {

DEFINE_SERVICE_COMMAND_TABLE(IQueryService, 8, QueryPlayEvent)

result_t
IQueryService::QueryPlayEvent(i32 entry_index, i32* out_total_entries,
                              OutBuffer<BufferAttr::MapAlias> out_buffer) {
    LOG_FUNC_STUBBED(Services);

    // HACK
    *out_total_entries = 0;
    return RESULT_SUCCESS;
}

} // namespace hydra::horizon::services::pdm
