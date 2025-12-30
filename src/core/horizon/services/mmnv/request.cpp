#include "core/horizon/services/mmnv/request.hpp"

namespace hydra::horizon::services::mmnv {

DEFINE_SERVICE_COMMAND_TABLE(IRequest, 0, InitializeOld, 1, FinalizeOld, 2,
                             SetAndWaitOld, 3, GetOld, 4, Initialize, 5,
                             Finalize, 6, SetAndWait, 7, Get)

result_t IRequest::InitializeOld(Module module, u32 priority,
                                 u32 event_clear_mode) {
    u32 unused;
    return Initialize(module, priority, event_clear_mode, &unused);
}

result_t IRequest::FinalizeOld(Module module) {
    LOG_FUNC_STUBBED(Services);

    LOG_DEBUG(Services, "Module: {}", module);

    return RESULT_SUCCESS;
}

result_t IRequest::SetAndWaitOld(Module module, u32 setting_min,
                                 u32 setting_max) {
    LOG_FUNC_WITH_ARGS_STUBBED(Services, "setting min: {}, setting max: {}",
                               setting_min, setting_max);

    LOG_DEBUG(Services, "Module: {}", module);

    return RESULT_SUCCESS;
}

result_t IRequest::GetOld(Module module, u32* out_setting_current) {
    LOG_FUNC_STUBBED(Services);

    LOG_DEBUG(Services, "Module: {}", module);

    // HACK
    *out_setting_current = 0;
    return RESULT_SUCCESS;
}

result_t IRequest::Initialize(Module module, u32 priority, u32 event_clear_mode,
                              u32* out_request_id) {
    LOG_FUNC_STUBBED(Services);

    LOG_DEBUG(Services, "Module: {}, prio: {}, event autoclear: {}", module,
              priority, event_clear_mode != 0);

    // HACK
    *out_request_id = 1;
    return RESULT_SUCCESS;
}

result_t IRequest::Finalize(u32 request_id) {
    LOG_FUNC_STUBBED(Services);

    LOG_DEBUG(Services, "Request ID: {}", request_id);

    return RESULT_SUCCESS;
}

result_t IRequest::SetAndWait(u32 request_id, u32 setting_min,
                              u32 setting_max) {
    LOG_FUNC_WITH_ARGS_STUBBED(Services, "setting min: {}, setting max: {}",
                               setting_min, setting_max);

    LOG_DEBUG(Services, "Request ID: {}", request_id);

    return RESULT_SUCCESS;
}

result_t IRequest::Get(u32 request_id, u32* out_setting_current) {
    LOG_FUNC_STUBBED(Services);

    LOG_DEBUG(Services, "Request ID: {}", request_id);

    // HACK
    *out_setting_current = 0;
    return RESULT_SUCCESS;
}

} // namespace hydra::horizon::services::mmnv
