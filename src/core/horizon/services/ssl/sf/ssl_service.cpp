#include "core/horizon/services/ssl/sf/ssl_service.hpp"

#include "core/horizon/services/ssl/sf/ssl_context.hpp"

namespace hydra::horizon::services::ssl::sf {

DEFINE_SERVICE_COMMAND_TABLE(ISslService, 0, CreateContext, 5,
                             SetInterfaceVersion)

result_t ISslService::CreateContext(RequestContext* ctx,
                                    aligned<SslVersion, 8> version,
                                    u64 pid_placeholder) {
    LOG_DEBUG(Services, "Version: {}", version.Get());

    AddService(*ctx, new ISslContext(version));
    return RESULT_SUCCESS;
}

result_t ISslService::SetInterfaceVersion(SystemVersion version) {
    LOG_FUNC_STUBBED(Services);

    LOG_DEBUG(Services, "Version: {}", version);

    return RESULT_SUCCESS;
}

} // namespace hydra::horizon::services::ssl::sf
