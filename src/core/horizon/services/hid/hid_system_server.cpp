#include "core/horizon/services/hid/hid_system_server.hpp"

namespace hydra::horizon::services::hid {

DEFINE_SERVICE_COMMAND_TABLE(IHidSystemServer, 303, ApplyNpadSystemCommonPolicy,
                             321, GetUniquePadsFromNpad)

result_t IHidSystemServer::GetUniquePadsFromNpad(
    ::hydra::horizon::hid::NpadIdType npad_id, i64* out_count,
    OutBuffer<BufferAttr::HipcPointer> out_buffer) {
    LOG_FUNC_STUBBED(Services);

    // HACK
    *out_count = 0;
    return RESULT_SUCCESS;
}

} // namespace hydra::horizon::services::hid
