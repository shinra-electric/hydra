#include "core/horizon/services/ovln/sender_service.hpp"

#include "core/horizon/services/ovln/sender.hpp"

namespace hydra::horizon::services::ovln {

DEFINE_SERVICE_COMMAND_TABLE(ISenderService, 0, OpenSender)

result_t ISenderService::OpenSender(RequestContext* ctx) {
    AddService(*ctx, new ISender());
    return RESULT_SUCCESS;
}

} // namespace hydra::horizon::services::ovln
