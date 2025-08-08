#include "core/horizon/services/nfp/nfp.hpp"

namespace hydra::horizon::services::nfp {

result_t INfp::Initialize(u64 aruid, u64 zero,
                          InBuffer<BufferAttr::MapAlias> in_version_buffer) {
    LOG_FUNC_STUBBED(Services);
    return RESULT_SUCCESS;
}

result_t INfp::GetState(u32* out_state) {
    LOG_FUNC_STUBBED(Services);

    // HACK
    *out_state = 0;
    return RESULT_SUCCESS;
};

} // namespace hydra::horizon::services::nfp
