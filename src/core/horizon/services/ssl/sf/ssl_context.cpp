#include "core/horizon/services/ssl/sf/ssl_context.hpp"

namespace hydra::horizon::services::ssl::sf {

DEFINE_SERVICE_COMMAND_TABLE(ISslContext, 4, ImportServerPki)

result_t ISslContext::ImportServerPki(CertificateFormat cert_format,
                                      InBuffer<BufferAttr::MapAlias> in_buffer,
                                      u64* out_id) {
    LOG_FUNC_STUBBED(Services);

    // HACK
    *out_id = 0;
    return RESULT_SUCCESS;
}

} // namespace hydra::horizon::services::ssl::sf
